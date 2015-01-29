#include "include/Cycles.h"

double SatVP (double T)
{
    // Saturated vapor pressure (kPa).
    return 0.6108 * exp (17.27 * T / (T + 237.3));
}

void CalculateDerivedWeather (WeatherStruct *Weather, int total_years)
{
    int             y, doy;
    double          annualMaxTemperatureSum;
    double          annualMinTemperatureSum;

    Weather->atmosphericPressure = 101.325 * exp (-Weather->siteAltitude / 8200.0);

    for (y = 0; y < total_years; y++)
    {
        annualMaxTemperatureSum = 0.0;
        annualMinTemperatureSum = 0.0;

        for (doy = 1; doy < 366; doy++)
        {
            Weather->ETref[y][doy - 1] = CalculatePMET (Weather->siteLatitude, Weather->atmosphericPressure, Weather->screeningHeight, Weather->tMax[y][doy - 1], Weather->tMin[y][doy - 1], Weather->solarRadiation[y][doy - 1], Weather->RHmax[y][doy - 1], Weather->RHmin[y][doy - 1], Weather->wind[y][doy - 1], doy);
            annualMaxTemperatureSum = annualMaxTemperatureSum + Weather->tMax[y][doy - 1];
            annualMinTemperatureSum = annualMinTemperatureSum + Weather->tMin[y][doy - 1];
        }
        Weather->annualAverageTemperature[y] = (annualMaxTemperatureSum + annualMinTemperatureSum) / 365.0 / 2.0;
        Weather->yearlyAmplitude[y] = (annualMaxTemperatureSum - annualMinTemperatureSum) / 365.0;
    }
}
