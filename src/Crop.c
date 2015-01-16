#include "include/Cycles.h"

void SelectCropInitialPosition(CropManagementStruct *CropManagement)
{
    CropManagement->plantingIndex = -1;
    CropManagement->describedIndex = -1;

    PeekNextCrop (CropManagement);
}

void SelectNextCrop (CropManagementStruct *CropManagement)
{
    /*
     * Planting index set to next crop (if any) to be planted in the rotation
     * Described index set to previously searched value inside plantingOrder
     * Next crop in the rotation (if any)has limited values stored
     */

    if (CropManagement->plantingIndex < CropManagement->totalCropsPerRotation)
        CropManagement->plantingIndex++;
    else
        CropManagement->plantingIndex = 0;

    CropManagement->describedIndex = CropManagement->plantingOrder[CropManagement->plantingIndex].plantID;

    PeekNextCrop (CropManagement);
}

void PeekNextCrop(CropManagementStruct *CropManagement)
{
    /*
     * Store the name, year, day, and type values for the next crop
     */
    int tempIndex = CropManagement->plantingIndex;
    
    if (CropManagement->totalCropsPerRotation > 0)
    {
        /* Select next crop */
        if (tempIndex < CropManagement->totalCropsPerRotation - 1)
            tempIndex++;
        else
            tempIndex = 0;

        CropManagement->nextCropSeedingYear = CropManagement->plantingOrder[tempIndex].opYear;
        CropManagement->nextCropSeedingDate = CropManagement->plantingOrder[tempIndex].opDay;
        strcpy (CropManagement->nextCropName, CropManagement->plantingOrder[tempIndex].cropName);
    }
}
void NewCrop(CropStruct *Crop, CropManagementStruct *CropManagement)
{
    FieldOperationStruct *plantingOrder;
    autoIrrigationStruct *autoIrrigation;
    describedCropStruct *describedCrop;

    plantingOrder = &CropManagement->plantingOrder[CropManagement->plantingIndex];
    autoIrrigation = &CropManagement->autoIrrigation[plantingOrder->usesAutoIrrigation];
    describedCrop = &CropManagement->describedCrop[CropManagement->describedIndex];

    Crop->cropUniqueIdentifier = plantingOrder->plantID;
    strcpy (Crop->cropName, plantingOrder->cropName);

    if (plantingOrder->usesAutoIrrigation)
    {
        Crop->autoIrrigationUsed = 1;
        Crop->autoIrrigationStartDay = autoIrrigation->startDay;
        Crop->autoIrrigationStopDay = autoIrrigation->stopDay;
        Crop->autoIrrigationWaterDepletion = autoIrrigation->waterDepletion;
        Crop->autoIrrigationLastSoilLayer = autoIrrigation->lastSoilLayer;        
    }
    else
        Crop->autoIrrigationUsed = 0;

    Crop->userSeedingDate = describedCrop->userSeedingDate;
    Crop->userFloweringDate = describedCrop->userFloweringDate;
    Crop->userMaturityDate = describedCrop->userMaturityDate;
    Crop->userMaximumSoilCoverage = describedCrop->userMaximumSoilCoverage;
    Crop->userMaximumRootingDepth = describedCrop->userMaximumRootingDepth;
    Crop->userExpectedYieldAvg = describedCrop->userExpectedYieldAvg;
    Crop->userExpectedYieldMax = describedCrop->userExpectedYieldMax;
    Crop->userExpectedYieldMin = describedCrop->userExpectedYieldMin;
    Crop->userPercentMoistureInYield = describedCrop->userPercentMoistureInYield;
    Crop->userFractionResidueStanding = describedCrop->userFractionResidueStanding;
    Crop->userFractionResidueRemoved = describedCrop->userFractionResidueRemoved;
    Crop->userClippingTiming = describedCrop->userClippingTiming;
    Crop->userTranspirationMinTemperature = describedCrop->userTranspirationMinTemperature;
    Crop->userTranspirationThresholdTemperature = describedCrop->userTranspirationThresholdTemperature;
    Crop->userColdDamageMinTemperature = describedCrop->userColdDamageMinTemperature;
    Crop->userColdDamageThresholdTemperature = describedCrop->userColdDamageThresholdTemperature;
    Crop->userTemperatureBase = describedCrop->userTemperatureBase;
    Crop->userTemperatureOptimum = describedCrop->userTemperatureOptimum;
    Crop->userTemperatureMaximum = describedCrop->userTemperatureMaximum;
    Crop->userShootPartitionInitial = describedCrop->userShootPartitionInitial;
    Crop->userShootPartitionFinal = describedCrop->userShootPartitionFinal;
    Crop->userRadiationUseEfficiency = describedCrop->userRadiationUseEfficiency;
    Crop->userTranspirationUseEfficiency = describedCrop->userTranspirationUseEfficiency;
    Crop->userHIx = describedCrop->userHIx;
    Crop->userHIo = describedCrop->userHIo;    /* intercept harvest index */
    Crop->userHIk = describedCrop->userHIk;
    Crop->userEmergenceTT = describedCrop->userEmergenceTT;
    Crop->userNMaxConcentration = describedCrop->userNMaxConcentration;
    Crop->userNDilutionSlope = describedCrop->userNDilutionSlope;
    Crop->userKc = describedCrop->userKc;
    Crop->userAnnual = describedCrop->userAnnual;
    Crop->userLegume = describedCrop->userLegume;
    Crop->userC3orC4 = describedCrop->userC3orC4;
    Crop->calculatedFloweringTT = describedCrop->calculatedFloweringTT;
    Crop->calculatedMaturityTT = describedCrop->calculatedMaturityTT;
    Crop->calculatedSimAvgYield = describedCrop->calculatedSimAvgYield;
    Crop->calculatedSimMaxYield = describedCrop->calculatedSimMaxYield;
    Crop->calculatedSimMinYield = describedCrop->calculatedSimMinYield;

    Crop->harvestDateFinal = -1;
    Crop->harvestCount = 0;
    Crop->stageGrowth = -1;

#ifdef _DEBUG_
    printf ("*Planted crop is %s (ID %d)\n", Crop->cropName, Crop->cropUniqueIdentifier);
#endif
}
        
void KillCrop (CropStruct *Crop)
{
    Crop->cropUniqueIdentifier = -1;
    strcpy (Crop->cropName, "");

    Crop->autoIrrigationUsed = -1;
    Crop->autoIrrigationStartDay = 0;
    Crop->autoIrrigationStopDay = 0;
    Crop->autoIrrigationWaterDepletion = 0;
    Crop->autoIrrigationLastSoilLayer = 0;        

    Crop->userSeedingDate = 0;
    Crop->userFloweringDate = 0;
    Crop->userMaturityDate = 0;
    Crop->userMaximumSoilCoverage = 0;
    Crop->userMaximumRootingDepth = 0;
    Crop->userExpectedYieldAvg = 0;
    Crop->userExpectedYieldMax = 0;
    Crop->userExpectedYieldMin = 0;
    Crop->userPercentMoistureInYield = 0;
    Crop->userFractionResidueStanding = 0;
    Crop->userFractionResidueRemoved = 0;
    Crop->userClippingTiming = 0;
    Crop->userTranspirationMinTemperature = 0;
    Crop->userTranspirationThresholdTemperature = 0;
    Crop->userColdDamageMinTemperature = 0;
    Crop->userColdDamageThresholdTemperature = 0;
    Crop->userTemperatureBase = 0;
    Crop->userTemperatureOptimum = 0;
    Crop->userTemperatureMaximum = 0;
    Crop->userShootPartitionInitial = 0;
    Crop->userShootPartitionFinal = 0;
    Crop->userRadiationUseEfficiency = 0;
    Crop->userTranspirationUseEfficiency = 0;
    Crop->userHIx = 0;
    Crop->userHIo = 0;    /* intercept harvest index */
    Crop->userHIk = 0;
    Crop->userEmergenceTT = 0;
    Crop->userNMaxConcentration = 0;
    Crop->userNDilutionSlope = 0;
    Crop->userKc = 0;
    Crop->userAnnual = 0;
    Crop->userLegume = 0;
    Crop->userC3orC4 = 0;
    Crop->calculatedFloweringTT = 0;
    Crop->calculatedMaturityTT = 0;
    Crop->calculatedSimAvgYield = 0;
    Crop->calculatedSimMaxYield = 0;
    Crop->calculatedSimMinYield = 0;

    Crop->harvestDateFinal = -1;
    Crop->harvestCount = -1;
    Crop->stageGrowth = -1;

}
void SetCropStatusToMature(CropStruct *Crop)
{
    /*
     * growing and mature
     */
//            Me.opList.Data(14) = True
}
