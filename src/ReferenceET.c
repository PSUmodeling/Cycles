#include "include/Cycles.h"

double CalculatePMET (double lat, double pAtm, double screeningHeight, double Tmax, double Tmin, double sRad, double rhMax, double rhMin, double wind, double doy)
{
    double PMET;
    double rc = 0.00081;    /* surface resistance to vapor flow, day/m (0.00081 for 350-400 ppm) */
    //double rc = 0.00058;  /* surface resistance to vapor flow, day/m (0.00058 for 280 ppm) */
    //double rc = 0.00157;  /* surface resistance to vapor flow, day/m (0.00157 for 500 ppm) */
    double CP = 0.001013;   /* specific heat of dry air, J/(kg C) */
    double R = 0.28704;      /* specific gas constant for dry air, kJ/(kg degree C) (recall that Patm is in kPa) */

    double esTmax;          /* saturation vapor pressure at Tmax, kPa */
    double esTmin;          /* saturation vapor pressure at Tmin, kPa */
    double ea;              /* actual air vapor pressure, kPa */
    double vpd, potRad, netRad, Tave, esTave, delta, gamma, lambda, Tkv, volCP, ra, aeroTerm, radTerm;

    Tave = (Tmax + Tmin) / 2.;
        esTave = SaturatedVaporPressure(Tave);
        esTmax = SaturatedVaporPressure(Tmax);
        esTmin = SaturatedVaporPressure(Tmin);
        ea = 0.5 * (esTmin * rhMax + esTmax * rhMin) / 100.;
        vpd = (esTmax + esTmin) / 2. - ea;
        potRad = PotentialRadiation(lat, doy);
        netRad = NetRadiation(potRad, sRad, ea, Tmax, Tmin);
        ra = Aero_Res(wind, screeningHeight);       /* aerodynamic resistance to vapor flow, day/m */
        delta = 4098. * esTave / (pow(Tave + 237.3, 2));    /* slope of saturated vapor pressure vs temperature function, kPA/C */
        lambda = 2.501 - 0.002361 * Tave;       /* latent heat of vaporization, MJ/kg */
        gamma = CP * pAtm / (0.622 * lambda);   /* psychrometric constant, kPaC */
        Tkv = 1.01 * (Tave + 273);              /* approximates virtual temperature, K */
        volCP = CP * pAtm / (R * Tkv);          /* CP * AirDensity (J/kg * kg/m3) */
        aeroTerm = (volCP * vpd / ra) / (delta + gamma * (1. + rc / ra));    /* MJ/m2 */
        radTerm = delta * netRad / (delta + gamma * (1. + rc / ra));            /* MJ/m2 */
        PMET = (aeroTerm + radTerm) / lambda;       /* kg water/m2 or mm (water density = 1 Mg/m3) */
        if (PMET < 0)
            return 0.001;
        else
            return PMET;    /* preventing a negative value, usually small and indicative of condensation */
}

double SaturatedVaporPressure (double T)
{
    return 0.6108 * exp(17.27 * T / (T + 237.3));
}

double PotentialRadiation (double Lat, int doy)
{
    double Solar_Constant = 118.08;
    double Lat_Rad, DR, SolDec, SunsetHourAngle, Term;

    Lat_Rad = Lat * PI / 180.;
    DR = 1 + 0.033 * cos(2. * PI * doy / 365.);
    SolDec = 0.409 * sin(2. * PI * doy / 365. - 1.39);
    SunsetHourAngle = acos(-tan(Lat_Rad) * tan(SolDec));
    Term = SunsetHourAngle * sin(Lat_Rad) * sin(SolDec) + cos(Lat_Rad) * cos(SolDec) * sin(SunsetHourAngle);

    return Solar_Constant * DR * Term / PI;
}

double NetRadiation(double Pot_Rad, double Solar_Rad, double Actual_VP, double TMax, double TMin)
{
    double Rns, F_Cloud, F_Hum, LWR, Rnl;
    double Albedo = 0.23;

    Rns = (1. - Albedo) * Solar_Rad;    /* calculate shortwave net radiation */
    F_Cloud = 1.35 * (Solar_Rad / (Pot_Rad * 0.75)) - 0.35;     /* calculate cloud factor */
    F_Hum = (0.34 - 0.14 * sqrt(Actual_VP));                    /* calculate humidity factor */
    LWR = 86400. * 0.0000000567 * 0.5 * (pow(TMax + 273.15, 4) + pow(TMin + 273.15, 4)) / 1000000.; /* calculate isothermal LW net radiation */
    Rnl = LWR * F_Cloud * F_Hum;
    return Rns - Rnl;       /* calculate Rn */
}

double Aero_Res(double uz, double z)
{
    double u2, d, zom, zoh, zm, zh;
    double VK = 0.41;   /* von Karman's constant */

    if (uz == 0)
        uz = 0.00001;
    if (z == 2.)
        u2 = uz;
    else
        u2 = uz * (4.87 / (log(67.8 * z - 5.42)));
    u2 = u2 * 86400.;   /* convert to m/day */
    d = 0.08;           /* zero plane displacement height, m */
    zom = 0.01476;      /* roughness length for momentum transfer, m */
    zoh = 0.001476;     /* roughness length for heat and vapor transfer, m */
    zm = 2.;            /* height of wind measurement, m */
    zh = 2.;            /* height of vapor measurement, m */

    return log((zm - d) / zom) * log((zh - d) / zoh) / (VK * VK * u2);
}

//  double Delta (double Tmean, double esTmean)
//  {
//      return 4098. * esTmean / (pow(Tmean + 237.3, 2));
//  }

//  double Lambda (double Tmean)
//  {
//      return 2.501 - 0.002361 * Tmean;
//  }

//  double Gamma(double Lam, double Patm)
//  {
//      return CP * Patm / (0.622 * Lam);
//  }
