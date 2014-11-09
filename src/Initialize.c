#include "include/Cycles.h"

void Initialize (SimControlStruct *SimControl, WeatherStruct *Weather, SoilStruct *Soil, ResidueStruct *Residue)
{
    int max = 0;
    int i;

    /*
     * Initialize weather variables
     */
    CalculateDerivedWeather (Weather);

    /*
     * Initialize soil variables
     */
    InitializeSoil (Soil, Weather, SimControl);

    /* Initialize residue */
    InitializeResidue (Residue, SimControl->totalYears, Soil->totalLayers);
    /*
     * Initialize crops
     */

    /* Convert units of crop parameters */
//    ModifyDescriptions (Cycles->describedCrops, Cycles->NumDescribedCrop);
//    LinkRotationAndDescription (Cycles->plantingOrder, Cycles->describedCrops, Cycles->totalCropsPerRotation, Cycles->NumDescribedCrop);

//    StorePlantingOrder (Cycles->plantedCrops, Cycles->plantingOrder, &Cycles->totalCropsPerRotation, Cycles->SimControl.yearsInRotation, Cycles->NumPlantedCrop);

}
