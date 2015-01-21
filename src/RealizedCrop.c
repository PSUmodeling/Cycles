#include "include/Cycles.h"

void AddCrop(CropStruct *Crop)
{
    Crop->rcActiveStatus = 1;
    Crop->rcYear = 0;
    Crop->rcDoy = 0;
    Crop->rcForageYield = 0;
    Crop->rcGrainYield = 0;
    Crop->rcBiomass = 0;
    Crop->rcRoot = 0;
    Crop->rcResidueBiomass = 0;
    Crop->rcCropTranspiration = 0;
    Crop->rcCropTranspirationPotential = 0;
    Crop->rcSoilWaterEvaporation = 0;
    Crop->rcHarvestIndex = 0;
    Crop->rcTotalNitrogen = 0;
    Crop->rcRootNitrogen = 0;
    Crop->rcGrainNitrogenYield = 0;
    Crop->rcForageNitrogenYield = 0;
    Crop->rcNitrogenCumulative = 0;
}
