Option Explicit On
Imports System.Math

Friend Class SoilTemperatureClass

    Public Sub Temperature(ByRef y As Integer, ByRef doy As Integer, ByVal snowCover As Double, ByRef cropInterception As Double, _
                                ByRef Soil As SoilClass, ByRef Weather As WeatherClass, ByRef Residue As ResidueClass)

        'Const secondsPerHour as Integer = 3600
        'Const secondsPerDay As Integer = 86400

        Dim m As Integer = Soil.totalLayers + 1
        Dim CP(m), k(m), a(m), b(m), c(m), d(m), T(m), Tn(m) As Double
        Dim tAvg As Double              'average air temperature, C
        Dim fCover As Double            'soil cover factor accouting for flat residue and snow cover
        Dim soilCover As Double         'soil fraction covered by snow and flat residues
        Dim counter As Integer

        Dim f As Double = 0.6           ' constant for setting forward, backward or centered difference method
        Dim g As Double = 1 - f

        m -= 1
        CP(0) = 0 ' heat capacity of boundary layer  = 0
        k(0) = 20 ' boundary layer conductance (W/m2 K)

        For i As Integer = 1 To m
            ' calculates heat capacity weighted by solid and water phase only
            CP(i) = Me.HeatCapacity(Soil.BD(i), Soil.waterContent(i)) * Soil.layerThickness(i)
            ' calculates conductance per layer, equation 4.20 for thermal conductivity
            k(i) = Me.HeatConductivity(Soil.BD(i), Soil.waterContent(i), Soil.Clay(i)) / (Soil.nodeDepth(i + 1) - Soil.nodeDepth(i))
        Next i

        'recalculates bottom boundary condition
        T(m + 1) = Me.EstimatedSoilTemperature(Soil.nodeDepth(m + 1), doy, Weather.annualAvgTemperature, Weather.annualAmplitude, Soil.annualTemperaturePhase, Soil.dampingDepth)
        Tn(m + 1) = T(m + 1)

        'passes previous time step temperatures for all soil layers
        T = Soil.soilTemperature

        'calculates temperature of upper boundary condition
        'uses an empirical factor to weight the effect of air temperature, residue cover, and snow cover on the upper node temperature
        'this is an empirical approach to allow for residue or snow insulation effect
        tAvg = 0.5 * (Weather.dailyTemperatureMax + Weather.dailyTemperatureMin)
        soilCover = 1 - (1 - cropInterception) * (1 - snowCover) * Residue.flatResidueTau
        fCover = 0.4 * soilCover + 0.3 * snowCover / (soilCover + 0.001)
        Tn(0) = (1 - fCover) * tAvg + fCover * T(1)
        T(0) = Tn(0)

        counter = 0

        Do
            'this loop updates temperatures after the first Thomas loop and is needed
            'due to the inclusion of Tsurface [T(1)] to calculate net radiation at "exchange" surface
            'these two layers are used as criteria to leave the loop (seen end of loop)
            counter += 1
            If counter > 1 Then
                T(1) = Tn(1)
                T(2) = Tn(2)
            End If

            For i As Integer = 1 To m                ' calculates matrix elements
                c(i) = -k(i) * f
                a(i + 1) = c(i)
                'b(i) = f * (k(i) + k(i - 1)) + CP(i) / secondsPerHour
                b(i) = f * (k(i) + k(i - 1)) + CP(i) / 86400 'changed to seconds per day, quite long time step
                'd(i) = g * k(i - 1) * T(i - 1) + (CP(i) / secondsPerHour - g * (k(i) + k(i - 1))) * T(i) + g * k(i) * T(i + 1)
                d(i) = g * k(i - 1) * T(i - 1) + (CP(i) / 86400 - g * (k(i) + k(i - 1))) * T(i) + g * k(i) * T(i + 1) 'changed to seconds per day, quite long time step
            Next

            d(1) += k(0) * Tn(0) * f '+ Rnet - Hourly_Latent_Heat ' add net radiation and latent heat transfer here for subdaily time step
            d(m) += k(m) * f * Tn(m + 1)

            'Thomas algorithm starts
            For i As Integer = 1 To m - 1
                c(i) = c(i) / b(i)
                d(i) = d(i) / b(i)
                b(i + 1) = b(i + 1) - a(i + 1) * c(i)
                d(i + 1) = d(i + 1) - a(i + 1) * d(i)
            Next i

            Tn(m) = d(m) / b(m)

            For i As Integer = m - 1 To 1 Step -1
                Tn(i) = d(i) - c(i) * Tn(i + 1)
            Next
            'Thomas algorithm ends

        Loop While Abs(T(1) - Tn(1)) > 0.02 Or Abs(T(2) - Tn(2)) > 0.02

        Soil.soilTemperature = Tn
    End Sub

    Private Function HeatCapacity(ByVal bulkDensity As Double, ByVal volumetricWC As Double) As Double
        Return 2400000 * bulkDensity / 2.65 + 4180000 * volumetricWC
    End Function

    Private Function HeatConductivity(ByVal bulkDensity As Double, ByVal volumetricWC As Double, ByVal fractionClay As Double) As Double
        Dim C1, C2, C3, C4 As Double                                'coeff to calculate thermal conductivity (page 32+)

        C1 = 0.65 - 0.78 * bulkDensity + 0.6 * bulkDensity * bulkDensity    'equation 4.27; coeff of 4.20
        C2 = 1.06 * bulkDensity                                             'equation 4.25; coeff of 4.20
        C3 = 1 + 2.6 / Sqrt(fractionClay)                                   'equation 4.28; coeff of 4.20
        C4 = 0.03 + 0.1 * bulkDensity * bulkDensity                         'equation 4.22; coeff of 4.20

        Return C1 + C2 * volumetricWC - (C1 - C4) * Exp(-(C3 * volumetricWC) ^ 4)
    End Function

    Public Function EstimatedSoilTemperature(ByVal nodeDepth As Double, ByVal doy As Integer, ByVal annualAvgTemperature As Double, _
                                              ByVal yearlyAmplitude As Double, ByVal phase As Integer, ByVal dampingDepth As Double) As Double

        Return annualAvgTemperature + yearlyAmplitude _
               * Exp(-nodeDepth / dampingDepth) * Sin(2 * 3.1416 / 365 * (doy - phase) - nodeDepth / dampingDepth)
    End Function

End Class
