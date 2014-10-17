'version dec 22 08
Option Explicit On
Imports System.Math

Friend Class ReferenceETClass

    Public Function CalculatePMET(ByVal lat As Double, ByVal pAtm As Double, ByVal screeningHeight As Double, ByVal Tmax As Double, _
            ByVal Tmin As Double, ByVal sRad As Double, ByVal rhMax As Double, ByVal rhMin As Double, ByVal wind As Double, ByVal doy As Integer) As Double

        Dim PMET As Double
        Const rc As Double = 0.00081    'surface resistance to vapor flow, day/m (0.00081 for 350-400 ppm)
        'Const rc As Double = 0.00058    'surface resistance to vapor flow, day/m (0.00058 for 280 ppm)
        'Const rc As Double = 0.00157    'surface resistance to vapor flow, day/m (0.00157 for 500 ppm)
        Const CP As Double = 0.001013   'specific heat of dry air, J/(kg C)
        Const R As Double = 0.28704     'specific gas constant for dry air, kJ/(kg °C) (recall that Patm is in kPa)

        Dim esTmax As Double    'saturation vapor pressure at Tmax, kPa
        Dim esTmin As Double    'saturation vapor pressure at Tmin,  kPa
        Dim ea As Double        'actual air vapor pressure, kPa
        Dim vpd, potRad, netRad, Tave, esTave, delta, gamma, lambda, Tkv, volCP, ra, aeroTerm, radTerm As Double

        Tave = (Tmax + Tmin) / 2
        esTave = Me.SaturatedVaporPressure(Tave)
        esTmax = Me.SaturatedVaporPressure(Tmax)
        esTmin = Me.SaturatedVaporPressure(Tmin)
        ea = 0.5 * (esTmin * rhMax + esTmax * rhMin) / 100
        vpd = (esTmax + esTmin) / 2 - ea
        potRad = Me.PotentialRadiation(lat, doy)
        netRad = Me.NetRadiation(potRad, sRad, ea, Tmax, Tmin)
        ra = Me.Aero_Res(wind, screeningHeight)                             'aerodynamic resistance to vapor flow, day/m
        delta = 4098 * esTave / ((Tave + 237.3) ^ 2)                        'slope of saturated vapor pressure vs temperature function, kPA/°C
        lambda = 2.501 - 0.002361 * Tave                                    'latent heat of vaporization, MJ/kg
        gamma = CP * pAtm / (0.622 * lambda)                                'psychrometric constant, kPa/°C
        Tkv = 1.01 * (Tave + 273)                                           'approximates virtual temperature, K
        volCP = CP * pAtm / (R * Tkv)                                       'CP * AirDensity (J/kg * kg/m3)
        aeroTerm = (volCP * vpd / ra) / (delta + gamma * (1 + rc / ra))     'MJ/m2
        radTerm = delta * netRad / (delta + gamma * (1 + rc / ra))          'MJ/m2
        PMET = (aeroTerm + radTerm) / lambda                                'kg water/m2 or mm (water density = 1 Mg/m3)
        If PMET < 0 Then Return 0.001 Else Return PMET 'preventing a negative value, usually small and indicative of condensation

    End Function

    Private Function SaturatedVaporPressure(ByVal T As Double) As Double
        Return 0.6108 * Exp(17.27 * T / (T + 237.3))
    End Function

    Private Function PotentialRadiation(ByVal Lat As Double, ByVal doy As Integer) As Double
        Const Solar_Constant As Double = 118.08
        Dim Lat_Rad, DR, SolDec, SunsetHourAngle, Term As Double

        Lat_Rad = Lat * PI / 180
        DR = 1 + 0.033 * Cos(2 * PI * doy / 365)
        SolDec = 0.409 * Sin(2 * PI * doy / 365 - 1.39)
        SunsetHourAngle = Acos(-Tan(Lat_Rad) * Tan(SolDec))
        Term = SunsetHourAngle * Sin(Lat_Rad) * Sin(SolDec) + Cos(Lat_Rad) * Cos(SolDec) * Sin(SunsetHourAngle)

        Return Solar_Constant * DR * Term / PI
    End Function

    Private Function NetRadiation(ByVal Pot_Rad As Double, ByVal Solar_Rad As Double, ByVal Actual_VP As Double, ByVal TMax As Double, ByVal TMin As Double) As Double

        Dim Rns, F_Cloud, F_Hum, LWR, Rnl As Double
        Const Albedo As Double = 0.23

        Rns = (1 - Albedo) * Solar_Rad                                                              'calculate shortwave net radiation
        F_Cloud = 1.35 * (Solar_Rad / (Pot_Rad * 0.75)) - 0.35                                      'calculate cloud factor
        F_Hum = (0.34 - 0.14 * Sqrt(Actual_VP))                                                     'calculate humidity factor
        LWR = 86400 * 0.0000000567 * 0.5 * ((TMax + 273.15) ^ 4 + (TMin + 273.15) ^ 4) / 1000000    'calculate isothermal LW net radiation
        Rnl = LWR * F_Cloud * F_Hum
        Return Rns - Rnl                                                                            'calculate Rn
    End Function

    Private Function Aero_Res(ByVal uz As Double, ByVal z As Double) As Double

        Dim u2, d, zom, zoh, zm, zh As Double
        Const VK As Double = 0.41 'von Karman's constant

        If uz = 0 Then uz = 0.00001
        If z = 2 Then u2 = uz Else u2 = uz * (4.87 / (Log(67.8 * z - 5.42)))
        u2 = u2 * 86400     'convert to m/day
        d = 0.08            'zero plane displacement height, m
        zom = 0.01476       'roughness length for momentum transfer, m
        zoh = 0.001476      'roughness length for heat and vapor transfer, m
        zm = 2              'height of wind measurement, m
        zh = 2              'height of vapor measurement, m

        Return Log((zm - d) / zom) * Log((zh - d) / zoh) / (VK * VK * u2)
    End Function

    'Private Function Delta(ByVal Tmean As Double, ByVal esTmean As Double) As Double
    '    Return 4098 * esTmean / ((Tmean + 237.3) ^ 2)
    'End Function

    'Private Function Lambda(ByVal Tmean As Double) As Double
    '    Return 2.501 - 0.002361 * Tmean
    'End Function

    'Private Function Gamma(ByVal Lam As Double, ByVal Patm As Double) As Double
    '    Return CP * Patm / (0.622 * Lam)
    'End Function
End Class
