#ifdef _PIHM_
#include "pihm.h"
#else
#include "Cycles.h"
#endif

void InitCropSV (crop_struct *Crop)
{
    Crop->svTT_Daily = 0.0;
    Crop->svTT_Cumulative = 0.0;
    Crop->svRadiationInterception = 0.0;
    Crop->svRadiationInterception_nc = 0.0;
    Crop->svBiomass = 0.0;
    Crop->svShoot = 0.0;
    Crop->svRoot = 0.0;
    Crop->svRizho = 0.0;
    Crop->svShootDailyGrowth = 0.0;
    Crop->svRootDailyGrowth = 0.0;
    Crop->svRizhoDailyDeposition = 0.0;
    Crop->svUnstressedShootDailyGrowth = 0.0;
    Crop->svUnstressedRootDailyGrowth = 0.0;
    Crop->svPostFloweringShootBiomass = 0.0;
    Crop->svRootingDepth = 0.0;
    Crop->svTranspiration = 0.0;
    Crop->svTranspirationPotential = 0.0;
#ifdef _PIHM_
    Crop->dailyTranspiration = 0.0;
    Crop->dailyTranspirationPotential = 0.0;
#endif
    Crop->svN_Shoot = 0.0;
    Crop->svN_Root = 0.0;
    Crop->svN_Rhizo = 0.0;
    Crop->svN_RizhoDailyDeposition = 0.0;
    Crop->svN_AutoAdded = 0.0;
    Crop->svN_Fixation = 0.0;
    Crop->svWaterStressFactor = 0.0;
    Crop->svN_StressFactor = 0.0;

    Crop->svShootUnstressed = 0.0;
    Crop->svN_StressCumulative = 0.0;

    Crop->rcForageYield = 0.0;
    Crop->rcGrainYield = 0.0;
    Crop->rcBiomass = 0.0;
    Crop->rcRoot = 0.0;
    Crop->rcResidueBiomass = 0.0;
    Crop->rcCropTranspiration = 0.0;
    Crop->rcCropTranspirationPotential = 0.0;
    Crop->rcSoilWaterEvaporation = 0.0;
    Crop->rcHarvestIndex = 0;
    Crop->rcTotalNitrogen = 0.0;
    Crop->rcRootNitrogen = 0.0;
    Crop->rcGrainNitrogenYield = 0.0;
    Crop->rcForageNitrogenYield = 0.0;
    Crop->rcNitrogenCumulative = 0.0;
    Crop->rcNitrogenInHarvest = 0.0;
    Crop->rcNitrogenInResidue = 0.0;
    Crop->rcNitrogenForageConc = 0.0;

    Crop->harvestDateFinal = -1;
    Crop->harvestCount = 0;
}

void PlantingCrop (comm_struct *Community,
    const cropmgmt_struct *CropManagement, int plantingIndex)
{
    op_struct      *plantingOrder;
    autoirr_struct *autoIrrigation;
    crop_struct    *Crop;

    plantingOrder = &(CropManagement->plantingOrder[plantingIndex]);

    Crop = &Community->Crop[plantingOrder->plantID];

    Crop->stageGrowth = PLANTING;

    Community->NumActiveCrop++;

    if (plantingOrder->usesAutoIrrigation > 0)
    {
        autoIrrigation =
            &(CropManagement->autoIrrigation[plantingOrder->
                usesAutoIrrigation]);
        Crop->autoIrrigationUsed = 1;
        Crop->autoIrrigationStartDay = autoIrrigation->startDay;
        Crop->autoIrrigationStopDay = autoIrrigation->stopDay;
        Crop->autoIrrigationWaterDepletion = autoIrrigation->waterDepletion;
        Crop->autoIrrigationLastSoilLayer = autoIrrigation->lastSoilLayer;
    }
    else
        Crop->autoIrrigationUsed = 0;

    Crop->userPlantingDensity = plantingOrder->plantingDensity;

    Crop->userClippingStart = plantingOrder->clippingStart;
    Crop->userClippingEnd = plantingOrder->clippingEnd;

    InitCropSV (Crop);
}

void AddCrop (crop_struct *Crop)
{
    Crop->rcForageYield = 0.0;
    Crop->rcGrainYield = 0.0;
    Crop->rcBiomass = 0.0;
    Crop->rcRoot = 0.0;
    Crop->rcResidueBiomass = 0.0;
    Crop->rcCropTranspiration = 0.0;
    Crop->rcCropTranspirationPotential = 0.0;
    Crop->rcSoilWaterEvaporation = 0.0;
    Crop->rcHarvestIndex = 0;
    Crop->rcTotalNitrogen = 0.0;
    Crop->rcRootNitrogen = 0.0;
    Crop->rcGrainNitrogenYield = 0.0;
    Crop->rcForageNitrogenYield = 0.0;
    Crop->rcNitrogenCumulative = 0.0;
    Crop->rcNitrogenInHarvest = 0.0;
    Crop->rcNitrogenInResidue = 0.0;
    Crop->rcNitrogenForageConc = 0.0;
}

void KillCrop (crop_struct *Crop)
{
    Crop->stageGrowth = NO_CROP;
    Crop->autoIrrigationUsed = -1;
    Crop->autoIrrigationStartDay = 0;
    Crop->autoIrrigationStopDay = 0;
    Crop->autoIrrigationWaterDepletion = 0.0;
    Crop->autoIrrigationLastSoilLayer = 0;

    Crop->userPlantingDensity = 0.0;

    Crop->userClippingStart = 0;
    Crop->userClippingEnd = 0;

    InitCropSV (Crop);
}

void UpdateCommunity (comm_struct *Community)
{
    int             i;

    Community->svRadiationInterception = 0.0;
    Community->svBiomass = 0.0;
    Community->svShoot = 0.0;
    Community->svRoot = 0.0;
    Community->svRizho = 0.0;
    Community->svShootDailyGrowth = 0.0;
    Community->svRootDailyGrowth = 0.0;
    Community->svRizhoDailyDeposition = 0.0;
    Community->svRootingDepth = 0.0;
    Community->svTranspiration = 0.0;
    Community->svTranspirationPotential = 0.0;
    Community->svN_Shoot = 0.0;
    Community->svN_Root = 0.0;
    Community->svN_Rhizo = 0.0;
    Community->svN_RizhoDailyDeposition = 0.0;
    Community->svN_AutoAdded = 0.0;
    Community->svN_Fixation = 0.0;
    Community->svWaterStressFactor = 0.0;
    Community->svN_StressFactor = 0.0;

    for (i = 0; i < Community->NumCrop; i++)
    {
        if (Community->Crop[i].stageGrowth > NO_CROP)
        {
            Community->svRadiationInterception +=
                Community->Crop[i].svRadiationInterception;
            Community->svBiomass += Community->Crop[i].svBiomass;
            Community->svShoot += Community->Crop[i].svShoot;
            Community->svRoot += Community->Crop[i].svRoot;
            Community->svRizho += Community->Crop[i].svRizho;
            Community->svShootDailyGrowth +=
                Community->Crop[i].svShootDailyGrowth;
            Community->svRootDailyGrowth +=
                Community->Crop[i].svRootDailyGrowth;
            Community->svRizhoDailyDeposition +=
                Community->Crop[i].svRizhoDailyDeposition;
            Community->svRootingDepth =
                Community->Crop[i].svRootingDepth >
                Community->svRootingDepth ? Community->Crop[i].
                svRootingDepth : Community->svRootingDepth;
            Community->svTranspiration += Community->Crop[i].svTranspiration;
            Community->svTranspirationPotential +=
                Community->Crop[i].svTranspirationPotential;
            Community->svN_Shoot += Community->Crop[i].svN_Shoot;
            Community->svN_Root += Community->Crop[i].svN_Root;
            Community->svN_Rhizo += Community->Crop[i].svN_Rhizo;
            Community->svN_RizhoDailyDeposition +=
                Community->Crop[i].svN_RizhoDailyDeposition;
            Community->svN_AutoAdded += Community->Crop[i].svN_AutoAdded;
            Community->svN_Fixation += Community->Crop[i].svN_Fixation;
            Community->svWaterStressFactor +=
                Community->Crop[i].svWaterStressFactor;
            Community->svN_StressFactor +=
                Community->Crop[i].svN_StressFactor;
        }
    }
}
