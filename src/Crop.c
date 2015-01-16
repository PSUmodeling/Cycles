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
        
//        Public Sub KillCrop(ByVal uniqueIdentifier As Integer)
//            'Precondition:  uniqueIdentifier is only found once in the list
//            'Postcondition: node containing the unique value is deleted
//
//            Me.opList.DeleteNode(uniqueIdentifier)
//        End Sub
//
void SetCropStatusToMature(CropStruct *Crop)
{
    /*
     * growing and mature
     */
//            Me.opList.Data(14) = True
}
