#include "Cycles.h"

void DailyOperations (int y, int doy, CropManagementStruct *CropManagement, CommunityStruct *Community, ResidueStruct *Residue, SimControlStruct *SimControl, SnowStruct *Snow, SoilStruct *Soil, SoilCarbonStruct *SoilCarbon, WeatherStruct *Weather, SummaryStruct *Summary, const char *project)
{
    /*
     * -----------------------------------------------------------------------
     * LOCAL VARIABLES
     *
     * Variable             Type        Description
     * ==========           ==========  ====================
     * FixedFertilization   FieldOperationStruct*
     * Tillage		    FieldOperationStruct*
     * FixedIrrigation	    FieldOperationStruct*
     */

    if (doy == 1)
    { 
        FirstDOY (&CropManagement->rotationYear, SimControl->yearsInRotation, Soil->totalLayers, SoilCarbon, Residue, Soil);
    }

    GrowingCrop (CropManagement->rotationYear, y, doy, Community, Residue, SimControl, Soil, SoilCarbon, Weather, Snow, project);

    FieldOperation (CropManagement->rotationYear, y, doy, CropManagement, Community, Soil, Residue, SimControl, SoilCarbon, Weather, project);

    ComputeResidueCover (Residue);

    TillageFactorSettling (CropManagement->tillageFactor, Soil->totalLayers, Soil->waterContent, Soil->Porosity);

    SnowProcesses (Snow, y, doy, Weather, Residue->stanResidueTau, Community->svRadiationInterception);

    Redistribution (y, doy, Weather->precipitation[y][doy - 1], Snow->snowFall, Snow->snowMelt, SimControl->hourlyInfiltration, Community, Soil, Residue);

    ResidueEvaporation (Residue, Soil, Community, Weather->ETref[y][doy - 1], Snow->snowCover);

    Evaporation (Soil, Community, Residue, Weather->ETref[y][doy - 1], Snow->snowCover);

    Temperature (y, doy, Snow->snowCover, Community->svRadiationInterception, Soil, Weather, Residue);

    ComputeFactorComposite (SoilCarbon, doy, y, Weather->lastDoy[y], Soil);

    ComputeSoilCarbonBalanceMB (SoilCarbon, y, Residue, Soil, CropManagement->tillageFactor);

    NitrogenTransformation (y, doy, Soil, Community, Residue, Weather, SoilCarbon);

    if (doy == Weather->lastDoy[y])
    {
        LastDOY (y, SimControl->simStartYear, Soil->totalLayers, Soil, SoilCarbon, Residue, Summary, project);
    }
}

void GrowingCrop (int rotationYear, int y, int d, CommunityStruct *Community, ResidueStruct *Residue, const SimControlStruct *SimControl, SoilStruct *Soil, SoilCarbonStruct *SoilCarbon, const WeatherStruct *Weather, const SnowStruct *Snow, const char *project)
{
    /*
     * Processes that only occur while a crop is growing are performed
     * Any needed harvests/forages have occured
     * Final Harvest date set once crop maturity achieved
     * -----------------------------------------------------------------------
     * LOCAL VARIABLES
     *
     * Variable             Type        Description
     * ==========           ==========  ====================
     * forcedHarvest	    int
     * i		    int
     */
    int             forcedHarvest = 0;
    int             i;
    int             clippingFlag;
    int             clippingWindow = 1;

    /* If any crop in the community is growing, run the growing crop subroutine */
    if (Community->NumActiveCrop <= 0)
    {
        return;
    }

    CropStage (y, d, Community, Weather);

    Phenology (y, d, Weather, Community);

    RadiationInterception (y, d, Community);

    WaterUptake (y, d, Community, Soil, Weather);

    Processes (y, d, SimControl->automaticNitrogen, Community, Residue, Weather, Soil, SoilCarbon);

    clippingFlag = ForcedClipping (d, Community);

    for (i = 0; i < Community->NumCrop; i++)
    {
        if (Community->Crop[i].stageGrowth > NO_CROP)
        {
            if (Weather->tMin[y][d - 1] < Community->Crop[i].userColdDamageThresholdTemperature)
            {
                if (Community->Crop[i].userAnnual && Community->Crop[i].svTT_Cumulative > Community->Crop[i].calculatedFloweringTT)
                {
                    GrainHarvest (y, d, SimControl->simStartYear, &Community->Crop[i], Residue, Soil, SoilCarbon, Weather, project);
                    Community->NumActiveCrop--;
                }
                else
                    ComputeColdDamage (y, d, &Community->Crop[i], Weather, Snow, Residue);
            }

            if (d == Community->Crop[i].harvestDateFinal || forcedHarvest)
            {
                if (Community->Crop[i].userAnnual)
                {
                    GrainHarvest (y, d, SimControl->simStartYear, &Community->Crop[i], Residue, Soil, SoilCarbon, Weather, project);
                    Community->NumActiveCrop--;
                }
                else
                {
                    if (Community->Crop[i].svShoot >= Community->Crop[i].userClippingBiomassThresholdLower * (1.0 - exp (-Community->Crop[i].userPlantingDensity)))
                    {
                        ForageHarvest (y, d, SimControl->simStartYear, &Community->Crop[i], Residue, Soil, SoilCarbon, Weather, project);
                    }
                    HarvestCrop (y, d, SimControl->simStartYear, &Community->Crop[i], Residue, Soil, SoilCarbon, Weather, project);
                    Community->NumActiveCrop--;
                }
            }
            else if (clippingFlag == 1)
            {
                if (Community->Crop[i].svShoot >= Community->Crop[i].userClippingBiomassThresholdLower * (1.0 - exp (-Community->Crop[i].userPlantingDensity)))
                {
                    ForageHarvest (y, d, SimControl->simStartYear, &Community->Crop[i], Residue, Soil, SoilCarbon, Weather, project);
                    AddCrop (&Community->Crop[i]);
                    Community->Crop[i].stageGrowth = CLIPPING;
                    Community->Crop[i].harvestCount += 1;
                }
            }

            Community->Crop[i].rcCropTranspirationPotential += Community->Crop[i].svTranspirationPotential;
            Community->Crop[i].rcCropTranspiration += Community->Crop[i].svTranspiration;
            Community->Crop[i].rcSoilWaterEvaporation += Soil->evaporationVol;
        }
    }

    UpdateCommunity (Community);
}

void CropStage (int y, int d, CommunityStruct *Community, const WeatherStruct *Weather)
{
    int             i;

    for (i = 0; i < Community->NumCrop; i++)
    {
        if (Community->Crop[i].stageGrowth > NO_CROP)
        {
            if (Community->Crop[i].svTT_Cumulative < Community->Crop[i].userEmergenceTT)
                Community->Crop[i].stageGrowth = PRE_EMERGENCE;
            else if (Community->Crop[i].svTT_Cumulative < Community->Crop[i].calculatedFloweringTT)
            {
                if (Community->Crop[i].userAnnual)
                    Community->Crop[i].stageGrowth = VEGETATIVE_GROWTH;
                else
                    Community->Crop[i].stageGrowth = PERENNIAL;
            }
            else if (Community->Crop[i].svTT_Cumulative < Community->Crop[i].calculatedMaturityTT)
            {
                if (Community->Crop[i].userAnnual)
                    Community->Crop[i].stageGrowth = REPRODUCTIVE_GROWTH;
                else
                    Community->Crop[i].stageGrowth = PERENNIAL;
            }
            else if (Community->Crop[i].svTT_Cumulative >= Community->Crop[i].calculatedMaturityTT)
            {
                Community->Crop[i].stageGrowth = MATURITY;

                if (Community->Crop[i].harvestDateFinal < 1)
                {
                    /* SetCropStatusToMature */
                    Community->Crop[i].cropMature = 1;
                    Community->Crop[i].harvestDateFinal = FinalHarvestDate (Weather->lastDoy[y], d);
                }
            }
        }
    }
}

double FinalHarvestDate (int lastDoy, int d)
{
    /* 
     * -----------------------------------------------------------------------
     * LOCAL VARIABLES
     *
     * Variable             Type        Description
     * ==========           ==========  ====================
     * harvestDate	    int		[return value]
     */
    int             harvestDate;

    harvestDate = d + 10;

    if (harvestDate > lastDoy)
        harvestDate -= lastDoy;

    return (harvestDate);
}

int ForcedMaturity (int rotationYear, int d, int lastDoy, int nextSeedingYear, int nextSeedingDate, int rotationSize)
{
    /*
     * Returns true if planted crop is within saftey margin of days of the
     * next crop to be planted
     * -----------------------------------------------------------------------
     * LOCAL VARIABLES
     *
     * Variable             Type        Description
     * ==========           ==========  ====================
     * forced_maturity	    int
     * margin		    int
     * nextRotationYear	    int
     */
    int             forced_maturity = 0;
    int             margin = 9;
    int             nextRotationYear;

    nextRotationYear = rotationYear;

    if (nextRotationYear < rotationSize)
        nextRotationYear++;
    else
        nextRotationYear = 1;

    if (rotationYear == nextSeedingYear)
    {
        if (d < nextSeedingDate && (d + margin) >= nextSeedingDate)
            forced_maturity = 1;
    }
    else if (nextRotationYear == nextSeedingYear)
    {
        if (d >= (lastDoy + nextSeedingDate - margin))
            forced_maturity = 1;
    }

    return (forced_maturity);
}

int ForcedClipping (int d, CommunityStruct *Community)
{
    int             i;
    int             clippingWindow = 1;

    /* Check if clipping window is open */
    for (i = 0; i < Community->NumCrop; i++)
    {
        if (Community->Crop[i].stageGrowth > NO_CROP)
        {
            /* Clipping window in summer */
            if (Community->Crop[i].userClippingEnd >= Community->Crop[i].userClippingStart)
            {
                if (d < Community->Crop[i].userClippingStart ||
                    d > Community->Crop[i].userClippingEnd)
                {
                    clippingWindow = 0;
                    break;
                }
            }
            /* Clipping window in winter */
            else
            {
                if (d < Community->Crop[i].userClippingStart &&
                    d > Community->Crop[i].userClippingEnd)
                {
                    clippingWindow = 0;
                    break;
                }
            }
        }
    }

    if (!clippingWindow)
    {
        return (0);
    }
    else
    {
        for (i = 0; i < Community->NumCrop; i++)
        {
            if (Community->Crop[i].stageGrowth > NO_CROP)
            {
                if (Community->Crop[i].svShoot / Community->Crop[i].userPlantingDensity >
                    Community->Crop[i].userClippingBiomassThresholdUpper)
                {
                    return (1);
                }
                else if (Community->Crop[i].userClippingTiming > 0.0)
                {
                    if (Community->Crop[i].userClippingTiming <= Community->Crop[i].svTT_Cumulative / Community->Crop[i].calculatedMaturityTT &&
                        Community->Crop[i].svShoot >= Community->Crop[i].userClippingBiomassThresholdLower * (1.0 - exp (-Community->Crop[i].userPlantingDensity)))
                    {
                        if ((Community->Crop[i].harvestCount < 3 && Community->Crop[i].userAnnual) || (!Community->Crop[i].userAnnual))
                        {
                            return (1);
                        }
                    }
                }
            }
        }
        
        return (0);
    }
}

void FirstDOY (int *rotationYear, int yearsInRotation, int totalLayers, SoilCarbonStruct *SoilCarbon, ResidueStruct *Residue, const SoilStruct *Soil)
{
    int             i;

    if (*rotationYear < yearsInRotation)
    {
        (*rotationYear)++;
    }
    else
    {
        *rotationYear = 1;
    }

    if (debug_mode)
    {
        printf ("*%-15s = %-d\n", "Rotation year", *rotationYear);
    }

    /* Initialize annual variables */
    for (i = 0; i < totalLayers; i++)
    {
        SoilCarbon->carbonMassInitial[i] = Soil->SOC_Mass[i];
        SoilCarbon->carbonMassFinal[i] = 0.0;
        SoilCarbon->annualHumifiedCarbonMass[i] = 0.0;
        SoilCarbon->annualRespiredCarbonMass[i] = 0.0;
        SoilCarbon->annualRespiredResidueCarbonMass[i] = 0.0;
        SoilCarbon->annualSoilCarbonDecompositionRate[i] = 0.0;
        SoilCarbon->abgdBiomassInput[i] = 0.0;
        SoilCarbon->rootBiomassInput[i] = 0.0;
        SoilCarbon->rhizBiomassInput[i] = 0.0;
        SoilCarbon->abgdCarbonInput[i] = 0.0;
        SoilCarbon->rootCarbonInput[i] = 0.0;
        SoilCarbon->annualNmineralization[i] = 0.0;
        SoilCarbon->annualNImmobilization[i] = 0.0;
        SoilCarbon->annualNNetMineralization[i] = 0.0;
        SoilCarbon->annualAmmoniumNitrification = 0.0;
        SoilCarbon->annualNitrousOxidefromNitrification = 0.0;
        SoilCarbon->annualAmmoniaVolatilization = 0.0;
        SoilCarbon->annualNO3Denitrification = 0.0;
        SoilCarbon->annualNitrousOxidefromDenitrification = 0.0;
        SoilCarbon->annualNitrateLeaching = 0.0;
        SoilCarbon->annualAmmoniumLeaching = 0.0;

        Residue->yearResidueBiomass = 0.0;
        Residue->yearRootBiomass = 0.0;
        Residue->yearRhizodepositionBiomass = 0.0;
    }
}

void LastDOY (int y, int simStartYear, int totalLayers, SoilStruct *Soil, SoilCarbonStruct *SoilCarbon, ResidueStruct *Residue, SummaryStruct *Summary, const char *project)
{
    int             i;

    for (i = 0; i < totalLayers; i++)
    {
        SoilCarbon->carbonMassFinal[i] = Soil->SOC_Mass[i];
    }

#ifndef _CYCLES_
    PrintAnnualOutput (y, simStartYear, Soil, SoilCarbon, project);

    PrintCarbonEvolution (y, simStartYear, totalLayers, Soil, SoilCarbon, Residue, project);
#endif

    StoreSummary (Summary, SoilCarbon, Residue, totalLayers, y);
}
