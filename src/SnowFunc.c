#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "Cycles.h"

void SnowProcesses (SnowClass * Snow, int y, int doy, WeatherClass * Weather, double TauStandingRes, double CropInterception)
{
    double          PP, Tavg, Tx, Tn;

    Snow->Snow_Fall = 0.;
    Snow->Snow_Melt = 0.;
    Snow->Snow_Cover = 0.;
    Snow->Snow_Evaporation = 0.;

    PP = Weather->dailyPrecipitation;
    Tx = Weather->dailyTemperatureMax;
    Tn = Weather->dailyTemperatureMin;
    Tavg = 0.5 * (Tx + Tn);

    CalculateSnowFall (Snow, Tavg, PP);

    if (Snow->Snow > 0)
    {
        CalculateSnowMelt (Snow, Tavg, Tx, Tn);
        if (Snow->Snow > 0)
            CalculateSnowEvaporation (Snow, TauStandingRes, CropInterception, Weather->dailyETref);
        Snow->Snow_Cover = CalculateSnowCover (Snow);
    }
}

void CalculateSnowFall (SnowClass * Snow, double Tavg, double PP)
{
    if (PP > 0 && Tavg < THRESHOLD_TEMPERATURE_SNOWFALL)
    {
        Snow->Snow_Fall = PP;
        Snow->Snow = Snow->Snow + Snow->Snow_Fall;
    }
}

void CalculateSnowMelt (SnowClass * Snow, double Tavg, double Tx, double Tn)
{
    double          TTmelt;

    if (Tn > THRESHOLD_TEMPERATURE_SNOWMELT)
        TTmelt = Tavg - THRESHOLD_TEMPERATURE_SNOWMELT;
    else if (Tx < THRESHOLD_TEMPERATURE_SNOWMELT)
        TTmelt = 0;
    else
        TTmelt = pow (Tx - THRESHOLD_TEMPERATURE_SNOWMELT, 2.) / (Tx - Tn);

    Snow->Snow_Melt = TTmelt * SNOWMELT_RATE;
    if (Snow->Snow_Melt > Snow->Snow)
        Snow->Snow_Melt = Snow->Snow;
    Snow->Snow = Snow->Snow - Snow->Snow_Melt;
}

void CalculateSnowEvaporation (SnowClass * Snow, double TauStandingRes, double CropInterception, double ETo)
{
    double          evaporativeDemand;

    evaporativeDemand = TauStandingRes * (1. - CropInterception) * ETo;

    if (evaporativeDemand > Snow->Snow)
        Snow->Snow_Evaporation = Snow->Snow;
    else
        Snow->Snow_Evaporation = evaporativeDemand;

    Snow->Snow = Snow->Snow - Snow->Snow_Evaporation;
}

double CalculateSnowCover (SnowClass * Snow)
{
    //SnowCover = 1 - exp(C1 * pow(Snow->Snow, 1 + C1 / 3)) with C1 = 0.427
    return 1 - exp (-0.43 * pow (Snow->Snow, 1.14));
}
