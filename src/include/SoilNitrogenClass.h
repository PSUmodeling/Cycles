Imports System.Math

Friend Class SoilNitrogenClass
    'Nitrogen Transformation
    Private Const NITRIFICATION_CONSTANT As Double = 0.2                    '1/day
    Private Const POTENTIAL_DENITRIFICATION As Double = 0.000032            'kg N / kg soil / day
    Private Const DENITRIFICATION_HALF_RATE As Double = 0.00006             'kg N / kg Soil
    Private Const NITRIFICATION_NO3_NH4_RATIO As Integer = 8                'NO3-N / NH4-N
   
    Public Sub NitrogenTransformation(ByRef Soil As SoilClass, ByVal Crop As CropClass, ByVal Residue As ResidueClass, _
                                      ByVal Weather As WeatherClass, ByVal SoilCarbon As SoilCarbonClass)

        Dim Profile_N_Nitrified As Double           'N daily nitrification, Mg N/ha
        Dim Profile_N2O_Nitri As Double             'N daily N2O from nitrification, Mg/ha
        Dim Profile_N_Denitrified As Double         'N daily denitrification, Mg N/ha
        Dim Profile_N2O_Denit As Double             'N daily denitrification in the form of N2O, Mg N/ha
        Dim Profile_NH4_Volatilization As Double    'NH4 volatilization, Mg N/ha

        'Dim NMineralInitial, NMineralFinal As Double
        'Dim NBalance As Double

        Profile_N_Nitrified = 0
        Profile_N_Denitrified = 0
        Profile_N2O_Denit = 0
        Profile_NH4_Volatilization = 0

        Soil.NH4_Nitrification = 0
        Soil.NO3_Denitrification = 0
        Soil.N2O_Denitrification = 0
        Soil.NH4_Volatilization = 0

        Call Denitrification(Profile_N_Denitrified, Profile_N2O_Denit, Soil, SoilCarbon)
        Call Nitrification(Profile_N_Nitrified, Profile_N2O_Nitri, Soil, SoilCarbon)
        Call Volatilization(Profile_NH4_Volatilization, Soil, Crop, Residue, Weather)

        'For i As Integer = 1 To Soil.totalLayers
        '    NMineralInitial += Soil.NO3(i) + Soil.NH4(i)
        'Next

        Soil.NO3Profile = Soil.NO3.Sum
        Soil.NH4Profile = Soil.NH4.Sum

        'NMineralFinal = Soil.NO3Profile + Soil.NH4Profile + Profile_NH4_Volatilization
        'NBalance = NMineralFinal - NMineralInitial

        Soil.NH4_Nitrification = Profile_N_Nitrified
        Soil.N2O_Nitrification = Profile_N2O_Nitri
        Soil.NO3_Denitrification = Profile_N_Denitrified
        Soil.N2O_Denitrification = Profile_N2O_Denit
        Soil.NH4_Volatilization = Profile_NH4_Volatilization

    End Sub
    Private Sub Nitrification(ByRef Profile_N_Nitrified As Double, ByRef Profile_N2O_Nitrified As Double, ByRef Soil As SoilClass, ByVal SoilCarbon As SoilCarbonClass)

        Dim NH4_Nitrified As Double         'N daily nitrification, Mg/ha
        Dim N2O_Nitrified As Double         'N daily N2O from denitrification and nitrification, Mg/ha
        Dim pH_Factor As Double = 1         'pH function controlling nitrification
        Dim NO3NH4Ratio As Double           'ratio of NO3 to NH4
        Dim ratioFactor As Double           'nitrification control by NO3/NH4 ratio
        Dim AirContent As Double            'porosity - watercontent, m3/m3
        Dim AirFactor As Double             'nitrification control by air content 
        Dim N2O_Fraction As Double          'fraction of nitrification released as N2O
        Dim TempFactor As Double            'nitrification control by temperature 

        For i As Integer = 1 To Soil.totalLayers
            If Soil.NH4(i) > 0 Then
                NO3NH4Ratio = Soil.NO3(i) / Soil.NH4(i)
                ratioFactor = 1 / (1 + (NO3NH4Ratio / 8) ^ 6)
                AirContent = Soil.Porosity(i) - Soil.waterContent(i)
                AirFactor = 1 - 1 / (1 + (AirContent / 0.1) ^ 3)
                N2O_Fraction = N2OFractionNitrification(AirContent) 'added August 2013
                TempFactor = TemperatureFunction(Soil.soilTemperature(i))
                NH4_Nitrified = Soil.NH4(i) * NITRIFICATION_CONSTANT * ratioFactor * pH_Factor * AirFactor * TempFactor
                N2O_Nitrified = N2O_Fraction * NH4_Nitrified
                Soil.NH4(i) -= (NH4_Nitrified + N2O_Nitrified)
                Soil.NO3(i) += NH4_Nitrified
                Profile_N_Nitrified += NH4_Nitrified
                Profile_N2O_Nitrified += N2O_Nitrified

                Soil.n2o(i) = N2O_Nitrified
            Else
                NH4_Nitrified = 0
            End If
        Next
    End Sub
    'Private Sub Nitrification(ByRef Profile_N_Nitrified As Double, ByRef Profile_N2O_Nitrified As Double, ByRef Soil As SoilClass, ByVal SoilCarbon As SoilCarbonClass)
    '    'ADD N2O EMISSION FROM NITRIFICATION

    '    Dim NH4_Nitrified As Double         'N daily nitrification, Mg/ha
    '    Dim N2O_Nitrified As Double         'N daily N2O from denitrification and nitrification, Mg/ha
    '    Dim pH_Factor As Double = 1         'pH function controlling nitrification
    '    Dim NO3NH4Ratio As Double           'ratio of NO3 to NH4
    '    Dim NitrificationFactor As Double   'nitrification control as a function NO3/NH4 ratio

    '    For i As Integer = 1 To Soil.totalLayers
    '        If Soil.NH4(i) > 0 Then
    '            NO3NH4Ratio = Soil.NO3(i) / Soil.NH4(i)
    '            NitrificationFactor = 1 / (1 + (NO3NH4Ratio / 8) ^ 6)
    '            NH4_Nitrified = Soil.NH4(i) * NITRIFICATION_CONSTANT * NitrificationFactor * pH_Factor * SoilCarbon.factorComposite(i)
    '            N2O_Nitrified = 0.0025 * NH4_Nitrified
    '            Soil.NH4(i) -= (NH4_Nitrified + N2O_Nitrified)
    '            Soil.NO3(i) += NH4_Nitrified
    '            Profile_N_Nitrified += NH4_Nitrified
    '            Profile_N2O_Nitrified += N2O_Nitrified

    '            Soil.n2o(i) = N2O_Nitrified
    '        Else
    '            NH4_Nitrified = 0
    '        End If
    '    Next
    'End Sub

    Private Sub Denitrification(ByRef Profile_N_Denitrified As Double, ByRef Profile_N2O_Denitrified As Double, _
                                ByRef Soil As SoilClass, ByVal SoilCarbon As SoilCarbonClass)

        Dim N_Denit As Double                   'nitrogen daily denitrification (N2 + N2O), Mg N/ha
        Dim N2O_Emission As Double              'nitrogen daily denitrification as N2O, kg/m2
        Dim N2O_Fraction As Double              'ratio of N2O to total denitrification
        Dim Soil_Mass As Double                 'Mg/ha
        Dim NO3_Conc As Double                  'Mg NO3 / Mg dry soil
        Dim NO3_Factor As Double                'nitrate concentration control of denitrification
        Dim Res_Factor As Double = 1            'this respiration factor considers temperature control already
        Dim Oxy_Factor As Double                'oxygen control of denitrification, using porosity occupied by air as a surrogate
        Dim rr1 As Double                       'carbon respired Mg C / Mg dry soil
        Dim AirVol As Double                    'fractional volume of air in the soil, m3/m3
        Dim cc1 As Single                       'compute coefficient for denitrification based on clay concentration
        Const cc2 As Double = 60                'coefficient of denitrification curve response to aereation

        For i As Integer = 1 To Soil.totalLayers
            N_Denit = 0
            N2O_Emission = 0
            AirVol = Soil.Porosity(i) - Soil.waterContent(i)

            If Soil.NO3(i) > 0.000001 And AirVol < 0.25 Then

                cc1 = 0.9 + 0.1 * Soil.Clay(i)
                Oxy_Factor = 1 / (1 + ((1 - AirVol) / cc1) ^ (-cc2))

                Soil_Mass = Soil.BD(i) * Soil.layerThickness(i) * 10000 'converted to Mg soil/ha
                rr1 = SoilCarbon.carbonRespired(i) / Soil_Mass
                Res_Factor = Math.Min(1, rr1 / 0.00005)

                NO3_Conc = Soil.NO3(i) / Soil_Mass
                NO3_Factor = NO3_Conc / (NO3_Conc + DENITRIFICATION_HALF_RATE)

                'N_Denit = POTENTIAL_DENITRIFICATION * Soil_Mass * Oxy_Factor * Res_Factor * NO3_Factor
                N_Denit = POTENTIAL_DENITRIFICATION * Soil_Mass * Oxy_Factor ^ 0.5 * Res_Factor * NO3_Factor 'changed August 2013
                N_Denit = Math.Min(Soil.NO3(i), N_Denit)
                N2O_Fraction = NO3_Factor * (1 - Oxy_Factor ^ 0.5) * (1 - Res_Factor ^ 0.33)
                'N2O_Fraction = NO3_Factor * (1 - Oxy_Factor ^ 2) * (1 - Res_Factor ^ 0.33)
                N2O_Emission = N_Denit * N2O_Fraction

                Soil.NO3(i) -= N_Denit

            End If

            Profile_N_Denitrified += N_Denit
            Profile_N2O_Denitrified += N2O_Emission
            Soil.n2o(i) = N2O_Emission
        Next i

    End Sub


    Private Sub Volatilization(ByRef Profile_NH4_Volatilization As Double, ByRef Soil As SoilClass, ByVal Crop As CropClass, _
                               ByVal Residue As ResidueClass, ByVal Weather As WeatherClass)

        'this subroutine uses an empirical approach to estimate the amount of NH4 that is subject to volatilization
        'and applies Henry equilibrium to estimate NH3 volatilization using turbulent conductance of the atmosphere 
        'and a proxy for conductance from the soil surface to the canopy exchange surface 
        'an Eulerian approach with daily time step can cause excess volatilization in a day

        Dim layerTop(Soil.totalLayers) As Double    'm
        Dim layerBottom(Soil.totalLayers) As Double 'm
        Dim layerMidpoint As Double                 'm
        Dim DepthFactor As Double                   'depth factor affection fraction of ammonium volatilizable (no diffusion processes in soil simulated)
        Dim CEC As Double                           'CEC, estimated from clay and soil organic carbon
        Dim CECFactor As Double                     'CEC factor controlling fraction of ammonia that is available for volatilization
        Dim fVol As Double                          'fraction of layer ammonium that can volatilize
        Dim NH4Volatilizable As Double              'NH4 of a layer subject to volatilization
        Dim NH4Volatilized As Double                'NH4 volatilized from a given layer
        Dim Tavg As Double                          'weighted daily average temperature (favoring daytime temperature)
        Dim pAtm As Double                          'atmospheric pressure, Pa
        Dim AMD As Double                           'air molar density, mol/m3
        Dim GBL As Double                           'molar atmospheric boundary layer conductance, mol/m3/second
        Dim GG1 As Double                           'NH3 flux correction based on flat residue cover, 0 - 1 temporary
        Dim GG2 As Double                           'NH3 flux correction based on canopy cover,  0 - 1 temporary
        Dim GG3 As Double                           'product GBL*GG2*GG3
        Dim pK As Double
        Dim pH As Double
        Dim henrysCoeff As Double
        Dim henrysConst As Double                   'Henry's constant, (liter Pa) / mol
        Dim waterVolume As Double
        Dim NH4Conc As Double
        Dim NH3Conc As Double
        Dim NH3MolarFraction As Double              'molar fraction

        pH = 6.5 ' FIX THIS INPUT

        Tavg = 273.15 + 0.67 * Weather.dailyTemperatureMax + 0.33 * Weather.dailyTemperatureMin
        pAtm = Weather.AtmosphericPressure * 1000
        AMD = AirMolarDensity(Tavg, pAtm)
        GBL = BoundaryLayerConductance(Crop.svRadiationInterception, Residue.stanResidueMass, Weather.dailyWindSpeed, AMD)
        GG1 = 1 - 0.85 * Crop.svRadiationInterception ^ 3
        GG2 = 0.95 * Residue.flatResidueTau ^ 2
        GG3 = GBL * GG1 * GG2

        pK = 0.09018 + 2729.92 / Tavg
        henrysCoeff = 10 ^ (1477.7 / Tavg - 1.69)
        henrysConst = 1000 * 8.3143 * Tavg / henrysCoeff  '1000 converts from m3 to liters

        For i As Integer = 1 To Soil.totalLayers
            If i > 1 Then
                layerBottom(i) = layerBottom(i - 1) + Soil.layerThickness(i)
                layerTop(i) = layerBottom(i - 1)
            Else
                layerBottom(i) = Soil.layerThickness(i)
                layerTop(i) = 0
            End If

            CEC = Math.Min(0.7 * Soil.Clay(i) * 100, 13 * Soil.SOC_Conc(i) * 100)
            CECFactor = 0.2 + 0.8 * Exp(-0.08 * CEC)

            layerMidpoint = 0.5 * (layerTop(i) + layerBottom(i))
            DepthFactor = 1 / 3 _
                    * (VolatilizationDepthFunction(layerTop(i)) _
                    + VolatilizationDepthFunction(layerMidpoint) _
                    + VolatilizationDepthFunction(layerBottom(i)))

            fVol = CECFactor * DepthFactor
            NH4Volatilizable = fVol * Soil.NH4(i)

            waterVolume = Soil.waterContent(i) * Soil.layerThickness(i) * DailyClass.WATER_DENSITY * 10000    'm3/ha
            NH4Conc = NH4Volatilizable * (18 / 14) / waterVolume                                    'Mg/m3; 18/14 converts mass of N to mass of NH4 
            NH3Conc = NH4Conc / (1 + 10 ^ (pK - pH))                                                'Mg/m3
            NH3MolarFraction = henrysConst * (NH3Conc / 0.000017) / pAtm                            '0.000017 = Mg/mol of NH3
            NH4Volatilized = GG3 * NH3MolarFraction * 86400 * 0.000017 * 10000 * (14 / 17)          'Mg NH3 / ha / day; 14/17 converts mass of N to mass of NH4 

            Soil.NH4(i) -= NH4Volatilized
            Profile_NH4_Volatilization += NH4Volatilized
        Next
    End Sub

    Private Function TemperatureFunction(ByVal T As Double) As Double
        Dim Temperature_Function As Double
        Dim Q As Double

        Const T_Min As Double = -5.0
        Const T_Opt As Double = 35.0
        Const T_Max As Double = 50.0

        If T < 0 Or T > T_Max Then
            Temperature_Function = 0
        Else
            Q = (T_Min - T_Opt) / (T_Opt - T_Max)
            Temperature_Function = ((T - T_Min) ^ (Q) * (T_Max - T)) / ((T_Opt - T_Min) ^ (Q) * (T_Max - T_Opt))
            If Temperature_Function > 1 Then Temperature_Function = 1.0
        End If

        Return Temperature_Function
    End Function

    Private Function N2OFractionNitrification(ByVal air As Double) As Double
        Dim N2OFraction_Function As Double
        Dim Q As Double

        Const f_Base As Double = 0.0025
        Const f_Max As Double = 0.1
        Const a_Min As Double = 0.0
        Const a_Opt As Double = 0.05
        Const a_Max As Double = 0.1

        If air < 0 Or air > a_Max Then
            N2OFraction_Function = f_Base
        Else
            Q = (a_Min - a_Opt) / (a_Opt - a_Max)
            N2OFraction_Function = f_Base + f_Max * ((air - a_Min) ^ (Q) * (a_Max - air)) / ((a_Opt - a_Min) ^ (Q) * (a_Max - a_Opt))
            If N2OFraction_Function > 1 Then N2OFraction_Function = 1.0
        End If

        Return N2OFraction_Function
    End Function


    Private Function pHFunction(ByVal pH As Single) As Single
        Dim pH_Min As Single = 3.5
        Dim pH_Max As Single = 6.5

        Return (pH - pH_Min) / (pH_Max - pH_Min)

    End Function

    Private Function VolatilizationDepthFunction(ByVal depth As Double) As Double
        Return 1 / (1 + (depth / 0.03) ^ 3.5)
    End Function

    Private Function AirMolarDensity(ByVal T As Double, ByVal P As Double) As Double
        Const R As Double = 8.3143  'gas constant, J/mol/C
        Return P / (R * T)
    End Function

    Private Function BoundaryLayerConductance(ByVal RI As Double, ByVal RM As Double, ByVal WS As Double, ByVal AMD As Double) As Double

        'RI = fractional solar radiation interception, 0 - 1
        'RM = mass of standing residue, Mg/ha
        'AMD = air molar density, mol/m3
        'WS = wind speed at 2 meters, m/s

        Dim CropHeight As Double
        Dim SoilHeight As Double
        Dim ResidueHeight As Double

        Dim h As Double
        Dim d As Double     'zero plane displacement (at d' = d + Zm, wind speed profile extrapolates to zero)
        Dim Zm As Double    'momentum roughness length
        Dim Zs As Double    'scalar roughness length

        CropHeight = RI / (RI + 0.5)           'temporary, until a function for crop height is incorporated; assumed max = 1 m
        SoilHeight = 0.05                      'temporary, until something better is available
        ResidueHeight = 0.3 * RM / (RM + 2)    '2 in Mg/ha or residue, assumed max = 0.3 m

        h = Math.Max(CropHeight, SoilHeight)
        h = Math.Max(h, ResidueHeight)
        d = 0.65 * h
        Zm = 0.1 * h
        Zs = 0.2 * Zm

        Return 0.41 ^ 2 * AMD * WS / (Log((2 - d) / Zm) * Log((2 - d) / Zs))

    End Function

End Class
