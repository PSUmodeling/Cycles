Option Explicit On
Imports System.Math

Friend Class SoilInfiltrationClass

    Public Sub Redistribution(ByVal y As Integer, ByVal doy As Integer, ByRef precipitation As Double, ByVal snowFall As Double, _
                              ByVal snowMelt As Double, ByVal hourlyInfiltration As Boolean, ByVal Crop As CropClass, _
                              ByRef Soil As SoilClass, ByRef Residue As ResidueClass, ByRef FixedIrrigation As FixedIrrigationClass, _
                              ByRef AutoIrrigation As AutoIrrigationClass)

        'this sub controls surface hydrology, infiltration, and redistribution in the profile
        'calculates input from irrigation
        'estimates potential input from irrigation, precipitation and snowmelt
        'wet surface residues
        'estimates runoff
        'infiltrates and/or redistributes water

        Soil.irrigationVol = 0
        Soil.infiltrationVol = 0
        Soil.runoffVol = 0
        Soil.drainageVol = 0
        Soil.NO3Leaching = 0
        Soil.NH4Leaching = 0

        Do While FixedIrrigation.IsOperationToday(y, doy)
            Soil.irrigationVol += FixedIrrigation.opVolume()
            Call FixedIrrigation.SelectNextOperation()
        Loop

        'If y = 2 Then Stop

        If Crop.autoIrrigationUsed Then

            If Crop.autoIrrigationStartDay < Crop.autoIrrigationStopDay Then
                If doy >= Crop.autoIrrigationStartDay And doy <= Crop.autoIrrigationStopDay Then
                    Soil.irrigationVol += AutoIrrigation.FindIrrigationVolume(Crop.autoIrrigationLastSoilLayer, Crop.autoIrrigationWaterDepletion, Soil)
                End If
            Else
                If doy >= Crop.autoIrrigationStartDay Or doy <= Crop.autoIrrigationStopDay Then
                    Soil.irrigationVol += AutoIrrigation.FindIrrigationVolume(Crop.autoIrrigationLastSoilLayer, Crop.autoIrrigationWaterDepletion, Soil)
                End If
            End If
        End If

        Soil.infiltrationVol = (precipitation - snowFall) + Soil.irrigationVol + snowMelt

        If Soil.infiltrationVol > 0 Then
            Call Residue.ResidueWetting(Soil)   'wet surface residues before runoff
            Call CurveNumber(Soil)              'runoff before infiltration
        End If

        If hourlyInfiltration Then
            Call SubDailyRedistribution(Soil)
        ElseIf Soil.infiltrationVol > 0 Then
            Call CascadeRedistribution(Soil)
        End If

    End Sub

    Private Sub CascadeRedistribution(ByRef Soil As SoilClass)

        'cascade approach, excess from FC is drainage
        Dim SoilSolute As New SoilSoluteClass

        Dim Win As Double = Soil.infiltrationVol
        Dim Wout As Double
        Dim WFlux(Soil.totalLayers) As Double
        Dim WCi() As Double = Soil.waterContent        'store initial soil water content

        WFlux(0) = Win

        For j As Integer = 1 To Soil.totalLayers

            If Win > DailyClass.WATER_DENSITY * (Soil.FC(j) - Soil.waterContent(j)) * Soil.layerThickness(j) Then
                Wout = Win - DailyClass.WATER_DENSITY * (Soil.FC(j) - Soil.waterContent(j)) * Soil.layerThickness(j)
                Soil.waterContent(j) = Soil.FC(j)
            Else
                Soil.waterContent(j) += Win / (DailyClass.WATER_DENSITY * Soil.layerThickness(j))
                Wout = 0
            End If

            WFlux(j) = Wout
            Win = Wout
            If Win <= 0 Then Exit For
        Next

        If Wout > 0 Then
            Soil.drainageVol += Wout
        End If

        'Solute = "Nitrate",Kd = 0; "Ammonium", Kd = 5.6; "Phosphorus", Kd = 60
        Call SoilSolute.SoluteTransport(Soil.totalLayers, 0, 0, Soil.NO3Leaching, WFlux, Soil.NO3, Soil.BD, Soil.layerThickness, Soil.Porosity, WCi)
        Call SoilSolute.SoluteTransport(Soil.totalLayers, 5.6, 0, Soil.NH4Leaching, WFlux, Soil.NH4, Soil.BD, Soil.layerThickness, Soil.Porosity, WCi)

        SoilSolute = Nothing
    End Sub

    Private Sub SubDailyRedistribution(ByRef Soil As SoilClass)

        Dim SoilSolute As New SoilSoluteClass

        Dim Win As Double = Soil.infiltrationVol

        Dim dzx(Soil.totalLayers + 1) As Double   'layer thickness times water density, kg/m2 (to reduce computations)
        Dim ksat(Soil.totalLayers + 1) As Double  'saturated hydraulic conductivity kg s / m3
        Dim wp(Soil.totalLayers + 1) As Double    'water potential, J/kg
        Dim kh(Soil.totalLayers + 1) As Double    'hydraulic conductance, kg s / m3

        Dim t1, t2, dt, cum_dt, x1, x2, x3, x4 As Double
        Dim RedistributionFlag As Boolean = False
        Dim SoluteFlag As Boolean = False

        'local array dimension AND copy
        Dim WC() As Double = Soil.waterContent
        Dim FC() As Double = Soil.FC
        Dim sat() As Double = Soil.Porosity
        Dim b() As Double = Soil.B_Value
        Dim aep() As Double = Soil.airEntryPotential
        Dim wpfc() As Double = Soil.FC_WaterPotential

        Dim WCi() As Double = Soil.waterContent             'store initial soil water content
        Dim WFlux(Soil.totalLayers) As Double

        Const g As Double = 9.81
        Const s As Double = 86400 '/ 2                       'seconds per day (or fraction of the day)

        RedistributionFlag = False

        For j As Integer = 1 To Soil.totalLayers
            dzx(j) = Soil.layerThickness(j) * DailyClass.WATER_DENSITY
            ksat(j) = K_Sat(sat(j), FC(j), b(j))
        Next

        If Win > 0 Then
            RedistributionFlag = True
            SoluteFlag = True
        End If

        'check if RedistributionFlag has to be set to TRUE when Win=0
        If Not RedistributionFlag Then
            For j As Integer = 1 To Soil.totalLayers
                If WC(j) > FC(j) Then
                    RedistributionFlag = True
                    SoluteFlag = True
                    Exit For
                End If
            Next
        End If

        'redistribution
        cum_dt = 0

        Do While RedistributionFlag
            x1 = 0

            If Win > 0 Then                     'wet layer(1) - up to saturation
                x1 = (sat(1) - WC(1)) * dzx(1)
                If Win > x1 Then
                    WC(1) = sat(1)
                    Win -= x1
                Else
                    WC(1) += Win / dzx(1)
                    x1 = Win
                    Win = 0
                End If
            End If

            WFlux(0) += x1                'store flux into layer 1

            'check if redistribution still true
            If Win = 0 Then
                RedistributionFlag = False

                For j As Integer = 1 To Soil.totalLayers
                    If WC(j) > FC(j) Then
                        RedistributionFlag = True
                        Exit For
                    End If
                Next

                If Not RedistributionFlag Then Exit Do
            End If

            'compute travel time for all emitting layers, in seconds
            'select minimum travel time of emitting and receiving 
            dt = s

            For j As Integer = 1 To Soil.totalLayers
                If WC(j) > FC(j) Then
                    t1 = s
                    t2 = s

                    wp(j) = Soil.SoilWaterPotential(sat(j), aep(j), b(j), WC(j))
                    If wpfc(j) / wp(j) < 1.001 Then wp(j) = wp(j) * 0.999
                    kh(j) = AverageHydraulicConductance(sat(j), FC(j), aep(j), b(j), wp(j), wpfc(j), ksat(j))
                    t1 = (WC(j) - FC(j)) * dzx(j) / (kh(j) * g)                                         'emitting layer

                    If j < Soil.totalLayers Then
                        wp(j + 1) = Soil.SoilWaterPotential(sat(j + 1), aep(j + 1), b(j + 1), WC(j + 1))
                        If wpfc(j + 1) / wp(j + 1) < 1.001 Then wp(j + 1) = wp(j + 1) * 0.999
                        kh(j + 1) = AverageHydraulicConductance(sat(j + 1), FC(j + 1), aep(j + 1), b(j + 1), wp(j + 1), wpfc(j + 1), ksat(j + 1))
                        t2 = (sat(j + 1) - FC(j + 1)) * dzx(j + 1) / (kh(j + 1) * g) 'maximum volume of receiving layer
                    End If

                    '                    If j < Soil.totalLayers Then t2 = (sat(j + 1) - FC(j + 1)) * dzx(j + 1) / (kh(j + 1) * g) 'maximum volume of receiving layer
                    If t2 < t1 Then t1 = t2
                End If

                If t1 < dt Then dt = t1
                If dt < 3600 Then dt = 3600
            Next j

            cum_dt += dt

            If cum_dt >= s Then
                dt = dt - (cum_dt - s)
                cum_dt = s
                RedistributionFlag = False
            End If

            For j As Integer = Soil.totalLayers To 1 Step -1

                x2 = 0 : x3 = 0 : x4 = 0

                If WC(j) > FC(j) Then
                    x2 = g * kh(j) * dt
                    x3 = (WC(j) - FC(j)) * dzx(j)
                    If (j + 1) < Soil.totalLayers Then
                        x4 = (sat(j + 1) - WC(j + 1)) * dzx(j + 1)
                    Else
                        x4 = 1000 'a big number to allow gravitational drainage from last layer
                    End If

                    '                    If x2 < 0 Then Stop
                    '                    If x3 < 0 Then Stop
                    '                   If x4 < 0 Then Stop

                    x2 = Math.Min(x2, Math.Min(x3, x4))
                    WC(j) = WC(j) - x2 / dzx(j)

                    If j < Soil.totalLayers Then
                        WC(j + 1) += x2 / dzx(j + 1)
                    Else
                        Soil.drainageVol += x2    'drainage from last layer
                    End If

                End If

                WFlux(j) += x2                 'stored for solute transport
            Next

            'if remainder of infiltration > 0 when Cum_DT = 86400 then saturate layers from top to bottom
            'if there is excess water then drainage
            'note: wonder if this excess should not be considered runoff, check how often it happens and the volume
            If Not RedistributionFlag And Win > 0 Then
                x1 = 0
                For j As Integer = 1 To Soil.totalLayers
                    x1 = (sat(j) - WC(j)) * dzx(j)
                    If Win > x1 Then
                        Win = Win - x1
                        WC(j) = sat(j)
                    Else
                        WC(j) = WC(j) + Win / dzx(j)
                        WFlux(j) = WFlux(j) + Win
                        Win = 0
                        x1 = 0
                        Exit For
                    End If
                    WFlux(j) = WFlux(j) + x1
                Next j
                Soil.drainageVol += Win 'keep adding drainage
            End If

        Loop

        If SoluteFlag = True Then
            'Solute = "Nitrate",Kd = 0; "Ammonium", Kd = 5.6; "Phosphorus", Kd = 60
            Call SoilSolute.SoluteTransport(Soil.totalLayers, 0, 0, Soil.NO3Leaching, WFlux, Soil.NO3, Soil.BD, Soil.layerThickness, Soil.Porosity, WCi)
            Call SoilSolute.SoluteTransport(Soil.totalLayers, 5.6, 0, Soil.NH4Leaching, WFlux, Soil.NH4, Soil.BD, Soil.layerThickness, Soil.Porosity, WCi)
        End If

        'update water content
        For j As Integer = 1 To Soil.totalLayers
            Soil.waterContent(j) = WC(j)
        Next

        SoilSolute = Nothing
    End Sub

    Private Sub CurveNumber(ByRef Soil As SoilClass)
        Const a As Double = 0.2

        Dim airDryWC(Soil.totalLayers) As Double
        Dim WCFraction(Soil.totalLayers) As Double

        Dim s As Double                                 ' curve number retention factor
        Dim CN As Double                                ' curve number used in computation (depends on CN1, CN2, CN3, and WC)
        Dim CN1 As Double                               ' curve number dry condition
        Dim CN2 As Double = Soil.Curve_Number           ' curve number average condition (input value)
        Dim CN3 As Double                               ' curve number wet condition
        Dim slope As Double = Soil.Percent_Slope        ' slope, %

        Dim factorSlope As Double                      ' correction of 's' (equivalent to CN) by slope as given in APEX Eq. 34
        Dim cumulativeWeightedFactor As Double
        Dim weightedFactorDepth(Soil.totalLayers) As Double
        Dim weightedWCF As Double
        Dim WC_Factor As Double
        Dim Term1 As Double                             ' help computation

        For i = 1 To Soil.totalLayers
            airDryWC(i) = Soil.PWP(i) / 3 'an approximation to air dry
        Next

        cumulativeWeightedFactor = 0

        For i As Integer = 1 To Soil.totalLayers
            If Soil.cumulativeDepth(i) <= 0.6 Then
                cumulativeWeightedFactor = cumulativeWeightedFactor + 1 - (Soil.cumulativeDepth(i) - 0.5 * Soil.layerThickness(i)) / 0.6
            Else
                Exit For
            End If
        Next

        weightedWCF = 0

        For i As Integer = 1 To Soil.totalLayers
            If Soil.cumulativeDepth(i) <= 0.6 Then
                weightedFactorDepth(i) = (1 - (Soil.cumulativeDepth(i) - 0.5 * Soil.layerThickness(i)) / 0.6) / cumulativeWeightedFactor
                WCFraction(i) = (Soil.waterContent(i) - airDryWC(i)) / (Soil.FC(i) - airDryWC(i))
                weightedWCF += weightedFactorDepth(i) * WCFraction(i)
            Else
                Exit For
            End If
        Next

        WC_Factor = weightedWCF / (weightedWCF + Exp(11 - 15 * weightedWCF))

        factorSlope = 1.1 - slope / (slope + Exp(3.7 + 0.02 * slope))
        CN1 = CN2 / (2.3 - 0.013 * CN2)
        CN3 = CN2 / (0.4 + 0.006 * CN2)

        CN = CN1 + (CN3 - CN1) * WC_Factor
        s = factorSlope * 254 * (100 / CN - 1)

        If Soil.infiltrationVol > a * s Then
            Term1 = Soil.infiltrationVol - a * s
            Soil.runoffVol = Term1 ^ 2 / (s + Term1)
            Soil.infiltrationVol -= Soil.runoffVol
        End If

    End Sub

    Public Function K_Sat(ByRef WCATSAT As Double, ByRef WCAT33 As Double, ByRef b As Double) As Double
        ' kg s / m^3
        Return 1930 * (WCATSAT - WCAT33) ^ (3 - 1 / b) * (1 / 3600) / 9.81
    End Function

    Private Function AverageHydraulicConductance(ByRef WCSAT As Double, ByRef WCFC As Double, ByRef aep As Double, ByRef b As Double, ByRef SWP1 As Double, ByRef SWP2 As Double, ByRef ks As Double) As Double
        Return ks * (Numerator(aep, b, SWP1) - Numerator(aep, b, SWP2)) / (Denominator(aep, b, SWP1) - Denominator(aep, b, SWP2))
    End Function

    Private Function Numerator(ByRef aep As Double, ByRef b As Double, ByRef SWP As Double) As Double
        Return -b * ((aep / SWP) ^ (2 + 4 / b)) / (4 + 2 * b)
    End Function

    Private Function Denominator(ByRef aep As Double, ByRef b As Double, ByRef SWP As Double) As Double
        Return -b * (aep / SWP) ^ (1 / b)
    End Function

End Class
