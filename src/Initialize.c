#include "include/Cycles.h"

void Initialize (SimControlStruct *SimControl, WeatherStruct *Weather, SoilStruct *Soil, ResidueStruct *Residue, SoilCarbonStruct *SoilCarbon, CropStruct *Crop, CropManagementStruct *CropManagement)
{
    int i;
    /* Initialize weather variables */
    CalculateDerivedWeather (Weather, SimControl->totalYears);

    /* Initialize soil variables */
    InitializeSoil (Soil, Weather, SimControl);

    /* Initialize residue */
    InitializeResidue (Residue, SimControl->totalYears, Soil->totalLayers);

    /* Initialize soil carbon */
    InitializeSoilCarbon (SoilCarbon, Soil->totalLayers);

    ///* Initialize crops */
    Crop->cropUniqueIdentifier = -1;

    /* Initialize tillage factors */
    for (i = 0; i < CropManagement->numTillage; i++)
        CropManagement->Tillage[i].tillageFactor = (double *) malloc (Soil->totalLayers * sizeof (double));
        
    /* Convert units of crop parameters */
//    ModifyDescriptions (Cycles->describedCrops, Cycles->NumDescribedCrop);
//    LinkRotationAndDescription (Cycles->plantingOrder, Cycles->describedCrops, Cycles->totalCropsPerRotation, Cycles->NumDescribedCrop);

//    StorePlantingOrder (Cycles->plantedCrops, Cycles->plantingOrder, &Cycles->totalCropsPerRotation, Cycles->SimControl.yearsInRotation, Cycles->NumPlantedCrop);

}
