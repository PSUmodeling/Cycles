#include "include/Cycles.h"

double FindIrrigationVolume (int opLayer, double opWaterDepletion, const SoilStruct *Soil)
{
    /*
     * This sub calculates the volume of an automatic irrigation
     */
    int             i;
    double          depletionZonePAW = 0.;
    double          depletionZoneWater = 0.;
    double          tempVal;

    if (opLayer > Soil->totalLayers)
        opLayer = Soil->totalLayers;

    for (i = 0; i < opLayer; i++)
    {
        if (Soil->waterContent[i] >= Soil->PWP[i])
        {
            depletionZonePAW += Soil->PAW[i] * Soil->layerThickness[i];
            depletionZoneWater += (Soil->waterContent[i] - Soil->PWP[i]) * Soil->layerThickness[i];
        }
        else    /* this adds deficit below PWP */
            depletionZonePAW += (Soil->FC[i] - Soil->waterContent[i]) * Soil->layerThickness[i];
    }

    if (depletionZoneWater + Soil->irrigationVol < depletionZonePAW * (1. - opWaterDepletion))
        /* convert m to mm */
        tempVal = (depletionZonePAW - depletionZoneWater) * 1000. - Soil->irrigationVol;
}
