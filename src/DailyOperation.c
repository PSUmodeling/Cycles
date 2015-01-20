﻿#include "include/Cycles.h"

void DailyOperations (int rotationYear, int y, int doy, int *nextSeedingYear, int *nextSeedingDate, CropManagementStruct *CropManagement, CropStruct *Crop, ResidueStruct *Residue, SimControlStruct *SimControl, SnowStruct *Snow, SoilStruct *Soil, SoilCarbonStruct *SoilCarbon, WeatherStruct *Weather)
{
    FieldOperationStruct *FixedFertilization;
    FieldOperationStruct *Tillage;
    FieldOperationStruct *FixedIrrigation;

    if (Crop->cropUniqueIdentifier >= 0)
    {
        GrowingCrop(rotationYear, y, doy, nextSeedingYear, nextSeedingDate, Crop, Residue, SimControl, Soil, SoilCarbon, Weather, Snow);
    }
    else if (doy == *nextSeedingDate && rotationYear == *nextSeedingYear)
    {
        PlantingCrop (doy, nextSeedingYear, nextSeedingDate, CropManagement, Crop);
#ifdef _DEBUG_
        printf ("DOY %3.3d Planting %s\n", doy, Crop->cropName);
#endif
    }

    while (IsOperationToday (rotationYear, doy, CropManagement->FixedFertilization, CropManagement->fertilizationIndex))
    {
        FixedFertilization = &CropManagement->FixedFertilization[CropManagement->fertilizationIndex];
#ifdef _DEBUG_
        printf ("DOY %3.3d Fixed Fertilization %s\n", doy, FixedFertilization->opSource);
#endif
        ApplyFertilizer(FixedFertilization, Soil, Residue);
        SelectNextOperation(CropManagement->numFertilization, &CropManagement->fertilizationIndex);
    }

    while (IsOperationToday (rotationYear, doy, CropManagement->Tillage, CropManagement->tillageIndex))
    {
        Tillage = &(CropManagement->Tillage[CropManagement->tillageIndex]);
#ifdef _DEBUG_
        printf ("DOY %3.3d Tillage %s\n", doy, Tillage->opToolName);
#endif
        if (strcasecmp(Tillage->opToolName, "Kill_Crop") != 0)
            ExecuteTillage(y, SoilCarbon->abgdBiomassInput, Tillage, CropManagement->tillageFactor, Soil, Residue);
        else if (Crop->cropGrowing)
            HarvestCrop(y, doy, SimControl->simStartYear, Crop, Residue, Soil, SoilCarbon);

        SelectNextOperation(CropManagement->numTillage, &CropManagement->tillageIndex);
    }

    Soil->irrigationVol = 0.;
    while (IsOperationToday (rotationYear, doy, CropManagement->FixedIrrigation, CropManagement->irrigationIndex))
    {
        FixedIrrigation = &(CropManagement->FixedIrrigation[CropManagement->irrigationIndex]);
        printf ("DOY %3.3d Irrigation %lf\n", doy, FixedIrrigation->opVolume);
        Soil->irrigationVol += FixedIrrigation->opVolume;
        SelectNextOperation(CropManagement->numIrrigation, &CropManagement->irrigationIndex);
    }

    ComputeResidueCover(Residue);
    TillageFactorSettling (CropManagement->tillageFactor, Soil->totalLayers, Soil->waterContent, Soil->Porosity);
    SnowProcesses (Snow, y, doy, Weather, Residue->stanResidueTau, Crop->svRadiationInterception);
    Redistribution(y, doy, Weather->precipitation[y][doy - 1], Snow->snowFall, Snow->snowMelt, SimControl->hourlyInfiltration, Crop, Soil, Residue);
    ResidueEvaporation (Residue, Soil, Crop, Weather->ETref[y][doy - 1], Snow->snowCover);
    Evaporation (Soil, Crop, Residue, Weather->ETref[y][doy - 1], Snow->snowCover);
    Temperature (y, doy, Snow->snowCover, Crop->svRadiationInterception, Soil, Weather, Residue);
    ComputeFactorComposite (SoilCarbon, doy, y, Soil);
    ComputeSoilCarbonBalance (SoilCarbon, y, Residue, Soil, CropManagement->tillageFactor);
    NitrogenTransformation(y, doy, Soil, Crop, Residue, Weather, SoilCarbon);
}

void GrowingCrop (int rotationYear, int y, int d, int *nextSeedingYear, int *nextSeedingDate, CropStruct *Crop, ResidueStruct *Residue, const SimControlStruct *SimControl, SoilStruct *Soil, SoilCarbonStruct *SoilCarbon, const WeatherStruct *Weather, const SnowStruct *Snow)
{
    /*
     * Processes that only occur while a crop is growing are performed
     * Any needed harvests/forages have occured
     * Final Harvest date set once crop maturity achieved
     */

    int forcedHarvest;

    forcedHarvest = ForcedMaturity(rotationYear, d, *nextSeedingYear, *nextSeedingDate, SimControl->yearsInRotation);

    if (Crop->svTT_Cumulative < Crop->userEmergenceTT)
        Crop->stageGrowth = PRE_EMERGENCE;
    else if (Crop->svTT_Cumulative < Crop->calculatedFloweringTT)
    {
        if (Crop->userAnnual)
            Crop->stageGrowth = VEGETATIVE_GROWTH;
        else
            Crop->stageGrowth = PERENNIAL;
    }
    else if (Crop->svTT_Cumulative < Crop->calculatedMaturityTT)
    {
        if (Crop->userAnnual)
            Crop->stageGrowth = REPRODUCTIVE_GROWTH;
        else
            Crop->stageGrowth = PERENNIAL;
    }
    else if (Crop->svTT_Cumulative >= Crop->calculatedMaturityTT)
    {
        Crop->stageGrowth = MATURITY;
        if (Crop->harvestDateFinal < 1)
        {
            /* SetCropStatusToMature */
            Crop->cropMature = 1;
            SetFinalHarvestDate (365, d, &Crop->harvestDateFinal);
        }
    }

    Phenology (y, d, Weather, Crop);
    RadiationInterception(y, d, Crop);
    WaterUptake(y, d, Crop, Soil, Weather);

    Processes(y, d, SimControl->automaticNitrogen, Crop, Residue, Weather, Soil, SoilCarbon);

    if (Weather->tMin[y][d - 1] < Crop->userColdDamageThresholdTemperature)
    {
        if (Crop->userAnnual && Crop->svTT_Cumulative > Crop->calculatedFloweringTT)
        {
            printf ("Crop->userAnnual && Crop->svTT_Cumulative > Crop->calculatedFloweringTT\n");
            GrainHarvest(y, d, SimControl->simStartYear, Crop, Residue, Soil, SoilCarbon);
        }
        else
            ComputeColdDamage(y, d, Crop, Weather, Snow, Residue);
    }

    if (Crop->cropUniqueIdentifier < 0)
    {
        printf ("Crop ID  = %d\n", Crop->cropUniqueIdentifier);
        exit(1);
    }

    if (d == Crop->harvestDateFinal || forcedHarvest)
    {
        if (Crop->userAnnual)
            GrainHarvest (y, d, SimControl->simStartYear, Crop, Residue, Soil, SoilCarbon);
        else
        {
            ForageHarvest(y, d, SimControl->simStartYear, Crop, Residue, Soil, SoilCarbon);
            HarvestCrop(y, d, SimControl->simStartYear, Crop, Residue, Soil, SoilCarbon);
        }
    }
    else if (Crop->userClippingTiming > 0)
    {
        if (Crop->userClippingTiming <= Crop->svTT_Cumulative / Crop->calculatedMaturityTT)
        {
            if ((Crop->harvestCount < 3 && Crop->userAnnual) || (!Crop->userAnnual))
            {
                ForageHarvest (y, d, SimControl->simStartYear, Crop, Residue, Soil, SoilCarbon);
                AddCrop(Crop);
                Crop->stageGrowth = CLIPPING;
                Crop->harvestCount += 1;
            }
        }
    }

    Crop->rcCropTranspirationPotential += Crop->svTranspirationPotential;
    Crop->rcCropTranspiration += Crop->svTranspiration;
    Crop->rcSoilWaterEvaporation += Soil->evaporationVol;
}

void PlantingCrop (int doy, int *nextSeedingYear, int *nextSeedingDate, CropManagementStruct *CropManagement, CropStruct *Crop)
{
    /*
     * new realized crop is created next crop in the rotation selected
     * status set to growing
     * harvestDate is reset to an unreachable date
     * dateSet is set to false
     */

    SelectNextCrop (CropManagement);

    NewCrop(Crop, CropManagement);
    AddCrop(Crop);

    *nextSeedingYear = CropManagement->nextCropSeedingYear;
    *nextSeedingDate = CropManagement->nextCropSeedingDate;

    Crop->stageGrowth = PLANTING;
}

void SetFinalHarvestDate(int lastDoy, int d, int *harvestDate)
{
    *harvestDate = d + 10;
    
    if (*harvestDate > lastDoy)
        *harvestDate -= lastDoy;
}

int ForcedMaturity (int rotationYear, int d, int nextSeedingYear, int nextSeedingDate, int rotationSize)
{
    /*
     * Returns true if planted crop is within saftey margin of days of the
     * next crop to be planted
     */

    int margin = 10;

    int nextRotationYear;

    nextRotationYear = rotationYear;

    if (nextRotationYear < rotationSize)
        nextRotationYear++;
    else
        nextRotationYear = 1;

    if (rotationYear == nextSeedingYear)
    {
        if (d < nextSeedingDate && (d + margin) >= nextSeedingDate)
            return (1);
    }
    else if (nextRotationYear == nextSeedingYear)
    {
        if (d >= (365 + nextSeedingDate - margin))
            return (1);
    }

    return (0);
}