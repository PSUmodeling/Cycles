#include "include/Cycles.h"

void Initialize (CyclesStruct Cycles)
{
    int max = 0;
    int i;

    /*
     * Initialize weather variables
     */
    CalculateDerivedWeather (&Cycles->Weather, &Cycles->SimControl);

    /*
     * Initialize soil variables
     */
    InitializeSoil (&Cycles->Soil, &Cycles->Weather, &Cycles->SimControl);

    /*
     * Initialize crops
     */
    /* Store planting order */
    for (i = 0; i < Cycles->NumPlantedCrop; i++)
    {
        if (Cycles->plantedCrops[i].seedingYear <= Cycles->SimControl.yearsInRotation)
            max++;
    }

    Cycles->plantingOrder = (plantingOrderStruct *) malloc (max * sizeof (plantingOrderStruct));

    Cycles->totalCropsPerRotation = max;

    max = 0;

    for (i = 0; i < Cycles->NumPlantedCrop; i++)
    {
        if (Cycles->plantedCrops[i].seedingYear <= Cycles->SimControl.yearsInRotation)
        {
            Cycles->plantingOrder[max] = Cycles->plantedCrops[i];
            max++;
        }
    }

    /* Convert units of crop parameters */
    ModifyDescriptions (Cycles->describedCrops, Cycles->NumDescribedCrop);
    LinkRotationAndDescription (Cycles->plantingOrder, Cycles->describedCrops, Cycles->totalCropsPerRotation, Cycles->NumDescribedCrop);

//    StorePlantingOrder (Cycles->plantedCrops, Cycles->plantingOrder, &Cycles->totalCropsPerRotation, Cycles->SimControl.yearsInRotation, Cycles->NumPlantedCrop);

}
