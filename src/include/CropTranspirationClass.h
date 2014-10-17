Option Explicit On
Imports System.Math

Friend Class CropTranspirationClass

    Public Sub WaterUptake(ByRef Crop As CropClass, ByRef Soil As SoilClass, ByVal Weather As WeatherClass)

        'Dim waterExtraction(Soil.totalLayers) As Double         'water extraction by layer, mm/day

        Dim PTx As Double = 15                                  'maximum full cover transpiration, mm/day
        Dim PT As Double = 0.00001                              'potential transpiration, mm/day
        Dim TE As Double                                        'expected transpiration, the minimum of PT and PWU, mm/day
        Dim TA As Double                                        'attainable transpiration (or TE adjusted by LWP), mm/day

        Dim transpirationRatio As Double
        Dim temperatureAvg As Double
        Dim factorTemperature As Double
        Dim factorAeration As Double

        Dim plantHC As Double                                   'plant hydraulic conductance, (kg/m2)/(J/kg) = kg2/(J m2)
        Dim rootHC As Double                                    'root hydraulic conductance
        Dim shootHC As Double                                   'shoot hydraulic conductance
        Dim Root_HC_Adjustment As Double                        'dryness adjustment of hydraulic conductance
        Dim layerPlantHC(Soil.totalLayers) As Double
        Dim layerRootHC(Soil.totalLayers) As Double
        Dim layerShootHC(Soil.totalLayers) As Double
        Dim Layer_Root_HC_Adjustment(Soil.totalLayers) As Double
        Dim rootActivity(Soil.totalLayers) As Double            'root activity factor to compute plant hydraulic conductance
        Dim rootFraction(Soil.totalLayers) As Double

        Dim LWP As Double                                       'leaf water potential, J/kg
        Dim LWP_StressOnset As Double = -1100                   'leaf water potential at the onset of stomatal closure
        Dim LWP_WiltingPoint As Double = -2000                  'leaf water potential at wilting point
        Dim SWP_FC As Double = -33                              'water potential at field capacity, J/kg
        Dim SWP_Average As Double                               'weighted soil water potential, J/kg
        Dim soilWP(Soil.totalLayers) As Double
        Dim layerSalinityFactor(Soil.totalLayers) As Double

        Crop.svTranspiration = 0
        Crop.svTranspirationPotential = 0
        Crop.svWaterStressFactor = 0

        If Crop.cropGrowing And Crop.svTT_Cumulative > Crop.userEmergenceTT Then

            temperatureAvg = 0.5 * (Weather.dailyTemperatureMax + Weather.dailyTemperatureMin)
            factorTemperature = TemperatureLimitation _
                    (temperatureAvg, Crop.userTranspirationMinTemperature, Crop.userTranspirationThresholdTemperature)

            'calculate potential transpiration rate (kg/m2/d = mm/d)
            'calculate crop maximum water uptake rate (kg/m2/d = mm/d)
            'calculate maximum crop transpiration rate (kg/m2/d = mm/d)
            PT = (1 + (Crop.userKc - 1) * Crop.svRadiationInterception) * factorTemperature * Crop.svRadiationInterception * Weather.dailyETref
            PTx = PTx * factorTemperature * Crop.svRadiationInterception
            TE = Min(PT, PTx)

            'calculate root fraction per soil layer
            For i As Integer = 1 To Soil.totalLayers
                Soil.waterUptake(i) = 0
                soilWP(i) = Soil.SoilWaterPotential(Soil.Porosity(i), Soil.airEntryPotential(i), Soil.B_Value(i), Soil.waterContent(i))
            Next

            'For i As Integer = 1 To Soil.totalLayers
            '    rootFraction(i) = CalculateRootFraction(Soil.cumulativeDepth(i), Soil.layerThickness(i), Crop.svRootingDepth)
            'Next

            ''adjust root fraction so that the sum of root fraction for all layers = 1
            'If (Crop.svRootingDepth >= Soil.cumulativeDepth(Soil.totalLayers)) And (rootFraction.Sum <> 1) Then
            '   Dim sumRoot as Double
            '   sumRoot = rootFraction.Sum
            '   For i As Integer = 1 To Soil.totalLayers
            '        rootFraction(i) = rootFraction(i) / sumRoot
            '   Next
            'End If

            Call CalcRootFraction(rootFraction, Soil, Crop)

            'calculate plant hydraulic conductivity (kg^2)/(m2 J d)
            'this is the hydraulic conductivity of a canopy fully covering the ground
            plantHC = PTx / (SWP_FC - LWP_StressOnset)
            rootHC = plantHC / 0.65
            shootHC = plantHC / 0.35

            'adjust plant hydraulic conductance based on soil dryness
            Root_HC_Adjustment = 0

            For i As Integer = 1 To Soil.totalLayers
                rootActivity(i) = 1
                layerSalinityFactor(i) = 1 'place holder
                '*
                rootActivity(i) = 1 - ((soilWP(i) - SWP_FC) / (LWP_WiltingPoint - SWP_FC)) ^ 8
                Select Case rootActivity(i)
                    Case Is > 1
                        rootActivity(i) = 1
                    Case Is < 0
                        rootActivity(i) = 0
                End Select
                '*
                Layer_Root_HC_Adjustment(i) = rootActivity(i) * rootFraction(i) * layerSalinityFactor(i)
                layerRootHC(i) = rootHC * Layer_Root_HC_Adjustment(i)
                Root_HC_Adjustment += Layer_Root_HC_Adjustment(i)
            Next

            For i As Integer = 1 To Soil.totalLayers
                If Layer_Root_HC_Adjustment(i) > 0 Then
                    layerShootHC(i) = shootHC * Layer_Root_HC_Adjustment(i) / Root_HC_Adjustment
                    layerPlantHC(i) = layerRootHC(i) * layerShootHC(i) / (layerRootHC(i) + layerShootHC(i))
                Else
                    layerPlantHC(i) = 0
                End If
            Next

            rootHC *= Root_HC_Adjustment
            plantHC = (rootHC * shootHC) / (rootHC + shootHC)

            If plantHC > 0 Then
                'calculate average soil water potential (J/kg)
                SWP_Average = 0

                For i As Integer = 1 To Soil.totalLayers
                    SWP_Average += soilWP(i) * Layer_Root_HC_Adjustment(i) / Root_HC_Adjustment
                Next

                'calculate leaf water potential
                'LWP = SWP_Average - PT / plantHC
                LWP = SWP_Average - TE / plantHC

                If LWP < LWP_StressOnset Then
                    'LWP = (plantHC * SWP_Average * (LWP_StressOnset - LWP_WiltingPoint) + LWP_WiltingPoint * PT) _
                    '        / (plantHC * (LWP_StressOnset - LWP_WiltingPoint) + PT)
                    LWP = (plantHC * SWP_Average * (LWP_StressOnset - LWP_WiltingPoint) + LWP_WiltingPoint * TE) _
                            / (plantHC * (LWP_StressOnset - LWP_WiltingPoint) + TE)

                End If

                If LWP < LWP_WiltingPoint Then LWP = LWP_WiltingPoint

                'Select Case LWP
                '    Case Is < LWP_WiltingPoint
                '        LWP = LWP_WiltingPoint
                '    Case Is < LWP_StressOnset
                '        'LWP = (plantHC * SWP_Average * (LWP_StressOnset - LWP_WiltingPoint) + LWP_WiltingPoint * PT) _
                '        '        / (plantHC * (LWP_StressOnset - LWP_WiltingPoint) + PT)
                '        LWP = (plantHC * SWP_Average * (LWP_StressOnset - LWP_WiltingPoint) + LWP_WiltingPoint * TE) _
                '                / (plantHC * (LWP_StressOnset - LWP_WiltingPoint) + TE)
                'End Select

                'reduce transpiration when LWP < LWP at the onset of stomatal closure
                If LWP < LWP_StressOnset Then
                    TA = TE * (LWP - LWP_WiltingPoint) / (LWP_StressOnset - LWP_WiltingPoint)
                    transpirationRatio = TA / TE
                Else
                    transpirationRatio = 1
                End If

                'calculate crop water uptake (kg/m2/d = mm/d)
                For i As Integer = 1 To Soil.totalLayers
                    Soil.waterUptake(i) = layerPlantHC(i) * (soilWP(i) - LWP) * transpirationRatio
                    Soil.waterContent(i) -= Soil.waterUptake(i) / (Soil.layerThickness(i) * DailyClass.WATER_DENSITY)
                Next

            End If

            Crop.svTranspiration = Soil.waterUptake.Sum
            Crop.svTranspirationPotential = TE
            Crop.svWaterStressFactor = 1 - Soil.waterUptake.Sum / TE
        End If
    End Sub

    'Private Function CalculateRootFraction(ByVal z As Double, ByVal dz As Double, ByVal Rd As Double) As Double
    '    'cumulative bottom depth, layer thickness, root depth
    '    Select Case Rd
    '        Case Is > z
    '            Return dz * (2 * (Rd - z) + dz) / (Rd * Rd)
    '        Case Is < (z - dz + 0.00001)
    '            Return 0
    '        Case Else
    '            Return ((Rd - z + dz) / Rd) ^ 2
    '    End Select
    'End Function

    Private Sub CalcRootFraction(ByRef fractionRootsByLayer() As Double, ByVal Soil As SoilClass, ByVal Crop As CropClass)

        'this function computes root fraction in each layer 
        'it assumes that the roots will reach a final distribution 
        'Root=a*exp[-b*z], with b~4
        'Root_layer = a / b * (Exp(-b * z1) - Exp(-b * z2)), where z1 and z2 are the top and bottom of layer
        'to compute the progression, the roots are "assumed" to have reached the maximum depth
        'then the root lenght in each layer is computed
        'only the roots within the current rooting depth are considered for the calculation
        'the root fraction is then recalculated for the layers with roots
        'the net effect is that the roots are not sharply exponential during early growth
        Const a As Double = 1
        Const b As Double = 4 '4 'units of 1/m
        Dim rootIntegral As Double = 0
        Dim rootSum As Double

        Dim rootDistribution(Soil.totalLayers) As Double
        Dim cumulativeRootingDepth As Double = 0
        Dim z1, z2 As Double
        Dim j As Integer = 0

        rootIntegral = a / b * (Exp(-b * 0) - Exp(-b * Crop.userMaximumRootingDepth))
        While cumulativeRootingDepth < Crop.svRootingDepth And j < Soil.totalLayers
            j += 1

            Select Case Soil.cumulativeDepth(j)
                Case Is < Crop.svRootingDepth
                    cumulativeRootingDepth = Soil.cumulativeDepth(j)
                Case Is >= Crop.svRootingDepth
                    cumulativeRootingDepth = Crop.svRootingDepth
            End Select

            z1 = Soil.cumulativeDepth(j - 1)
            z2 = cumulativeRootingDepth
            rootDistribution(j) = ((a / b) * (Exp(-b * z1) - Exp(-b * z2))) / rootIntegral
        End While

        rootSum = rootDistribution.Sum
        'ensures sum fraction = 1
        For i As Integer = 1 To j  'exits loop on the same layer as the previous loop
            fractionRootsByLayer(i) = rootDistribution(i) / rootSum
        Next
    End Sub

    Private Function TemperatureLimitation(ByVal T As Double, ByVal T_Min As Double, ByVal T_Threshold As Double) As Double
        If T <= T_Min Then
            Return 0.01
        ElseIf T > T_Min And T <= T_Threshold Then
            Return (T - T_Min) / (T_Threshold - T_Min)
        Else
            Return 1
        End If
    End Function

    Private Function Aeration(ByRef AC As Double) As Double
        'AC = soil air content
        Const A1 As Double = 0.05
        Const A2 As Integer = 4

        Return 1 - 0.6 / (1 + (AC / A1) ^ A2)
    End Function
End Class


