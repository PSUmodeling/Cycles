/****************************************************************************
 * Cycles
 * Latest update:   Oct 17, 2014
 * Developer:       Yuning Shi (yshi@psu.edu)
 ***************************************************************************/

#ifndef SNOW_HEADER 
    #define SNOW_HEADER

#define THRESHOLD_TEMPERATURE_SNOWFALL      1   /* degree C */
#define THRESHOLD_TEMPERATURE_SNOWMELT      -1  /* degree C */
#define SNOWMELT_RATE                       2.5 /* mm/(C day) or degree day
                                                 * melting factor */
struct SnowClass
{
    double          Snow;
    double          Snow_Fall;
    double          Snow_Melt;
    double          Snow_Cover;
    double          Snow_Evaporation;
} SnowClass;

#endif
