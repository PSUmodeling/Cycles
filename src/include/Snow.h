/*****************************************************************************
 * C version of Cycles
 * version dec 22 08
 ****************************************************************************/

#ifndef SNOW_HEADER 
    #define SNOW_HEADER

#include "Weather.h"

#define THRESHOLD_TEMPERATURE_SNOWFALL      1   /* degree C */
#define THRESHOLD_TEMPERATURE_SNOWMELT      -1  /* degree C */
#define SNOWMELT_RATE                       2.5 /* mm/(C day) or degree day
                                                 * melting factor */
class SnowClass
{
private:
    double  Snow;
    double  Snow_Fall;
    double  Snow_Melt;
    double  Snow_Cover;
    double  Snow_Evaporation;
public:
    double  snowFall() {return Snow_Fall;}
    double  snowMelt() {return Snow_Melt;}
    double  snowCover() {return Snow_Cover;}
    double  snowEvaporation() {return Snow_Evaporation;}
    void    SnowProcesses (int y, int doy, WeatherClass Weather, double TauStandingRes, double CropInterception);
private:
    void    CalculateSnowFall (double Tavg, double PP);
    void    CalculateSnowMelt (double Tavg, double Tx, double Tn);
    void    CalculateSnowEvaporation (double TauStandingRes, double CropInterception, double ETo);
    double  CalculateSnowCover();
};
#endif
