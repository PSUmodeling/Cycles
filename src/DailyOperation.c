#ifdef _PIHM_
#include "pihm.h"
#else
#include "Cycles.h"
#endif

#ifdef _PIHM_
void DailyCycles (int t, pihm_struct pihm)
{
    int             i;
    elem_struct    *elem;
    time_t          rawtime;
    struct tm      *timestamp;
    int             start_year;
    int             y, d;
    int             last_doy;

    rawtime = (time_t)pihm->ctrl.starttime;
    timestamp = gmtime (&rawtime);
    start_year = timestamp->tm_year + 1900;

    rawtime = (time_t)t;
    timestamp = gmtime (&rawtime);
    y = timestamp->tm_year + 1900 - start_year;

    d = DOY (t);

    last_doy = LastDOY (t);

    for (i = 0; i < pihm->numele; i++)
    {
        elem = &pihm->elem[i];

        //elem->weather.wind = elem->daily.wind;
        //elem->weather.ETref 
        //elem->weather.RHmax = elem->daily.rhmax;
        elem->weather.tMin[y][d - 1] = elem->daily.tmin - TFREEZ;
        elem->weather.tMax[y][d - 1] = elem->daily.tmax - TFREEZ;
        /* Calculate vpd in kPa from q2d */
        elem->weather.vpd[y][d - 1] = elem->daily.q2d * 0.622 / elem->daily.sfcprs / 1000.0;
        /* Convert to MJ */
        elem->weather.solarRadiation[y][d - 1] = elem->daily.solar_total / 1.0E6;
        elem->weather.lastDoy[y] = last_doy;
    }

    for (i = 0; i < pihm->numele; i++)
    {
        elem = &pihm->elem[i];

        DailyOperations (y, d, &elem->cropmgmt, &elem->comm, &elem->residue,
            &pihm->ctrl, &elem->soil, &elem->soilc, &elem->weather, &elem->wf);
    }
}
#endif

#ifdef _PIHM_
void DailyOperations (int y, int doy, cropmgmt_struct *CropManagement, comm_struct *Community, residue_struct *Residue, ctrl_struct *SimControl, soil_struct *Soil, soilc_struct *SoilCarbon, weather_struct *Weather, wf_struct *wf)
#else
void DailyOperations (int y, int doy, cropmgmt_struct *CropManagement, comm_struct *Community, residue_struct *Residue, ctrl_struct *SimControl, snow_struct *Snow, soil_struct *Soil, soilc_struct *SoilCarbon, weather_struct *Weather, summary_struct *Summary)
#endif
{
    /*
     * -----------------------------------------------------------------------
     * LOCAL VARIABLES
     *
     * Variable             Type        Description
     * ==========           ==========  ====================
     * FixedFertilization   op_struct*
     * Tillage		    op_struct*
     * FixedIrrigation	    op_struct*
     */

    if (doy == 1)
    { 
#ifdef _PIHM_
        FirstDOY (&CropManagement->rotationYear, CropManagement->yearsInRotation, Soil->totalLayers, SoilCarbon, Residue, Soil);
#else
        FirstDOY (&CropManagement->rotationYear, SimControl->yearsInRotation, Soil->totalLayers, SoilCarbon, Residue, Soil);
#endif
    }

#ifdef _PIHM_
    GrowingCrop (y, doy, Community, Residue, SimControl, Soil, SoilCarbon, CropManagement, Weather);
#else
    GrowingCrop (y, doy, Community, Residue, SimControl, Soil, SoilCarbon, Weather, Snow);
#endif

    FieldOperation (CropManagement->rotationYear, y, doy, CropManagement, Community, Soil, Residue, SimControl, SoilCarbon, Weather);

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
        LastDOY (y, SimControl->simStartYear, Soil->totalLayers, Soil, SoilCarbon, Residue, Summary);
    }
}

#ifdef _PIHM_
void GrowingCrop (int y, int d, comm_struct *Community, residue_struct *Residue, const ctrl_struct *SimControl, soil_struct *Soil, soilc_struct *SoilCarbon, cropmgmt_struct *CropManagement, weather_struct *Weather)
#else
void GrowingCrop (int y, int d, comm_struct *Community, residue_struct *Residue, const ctrl_struct *SimControl, soil_struct *Soil, soilc_struct *SoilCarbon, const weather_struct *Weather, const snow_struct *Snow)
#endif
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

    /* If any crop in the community is growing, run the growing crop subroutine */
    if (Community->NumActiveCrop <= 0)
    {
        return;
    }

    CropStage (d, Community, Weather->lastDoy[y]);

    Phenology (y, d, Weather, Community);

    RadiationInterception (y, d, Community);

#ifndef _PIHM_
    WaterUptake (y, d, Community, Soil, Weather);
#endif

#ifdef _PIHM_
    Processes (y, d, CropManagement->automaticNitrogen, Community, Residue, Weather, Soil, SoilCarbon);
#else
    Processes (y, d, SimControl->automaticNitrogen, Community, Residue, Weather, Soil, SoilCarbon);
#endif

    clippingFlag = ForcedClipping (d, Community);

    for (i = 0; i < Community->NumCrop; i++)
    {
        if (Community->Crop[i].stageGrowth > NO_CROP)
        {
            if (Weather->tMin[y][d - 1] < Community->Crop[i].userColdDamageThresholdTemperature)
            {
                if (Community->Crop[i].userAnnual && Community->Crop[i].svTT_Cumulative > Community->Crop[i].userFloweringTT)
                {
#ifdef _PIHM_
                    GrainHarvest (y, d, &Community->Crop[i], Residue, Soil, SoilCarbon);
#else
                    GrainHarvest (y, d, SimControl->simStartYear, &Community->Crop[i], Residue, Soil, SoilCarbon, Weather);
#endif
                    Community->NumActiveCrop--;
                }
                else
                    ComputeColdDamage (y, d, &Community->Crop[i], Weather, Snow, Residue);
            }

            if (d == Community->Crop[i].harvestDateFinal || forcedHarvest)
            {
                if (Community->Crop[i].userAnnual)
                {
#ifdef _PIHM_
                    GrainHarvest (y, d, &Community->Crop[i], Residue, Soil, SoilCarbon);
#else
                    GrainHarvest (y, d, SimControl->simStartYear, &Community->Crop[i], Residue, Soil, SoilCarbon, Weather);
#endif
                    Community->NumActiveCrop--;
                }
                else
                {
                    if (Community->Crop[i].svShoot >= Community->Crop[i].userClippingBiomassThresholdLower * (1.0 - exp (-Community->Crop[i].userPlantingDensity)))
                    {
                        ForageHarvest (y, d, SimControl->simStartYear, &Community->Crop[i], Residue, Soil, SoilCarbon, Weather);
                    }
                    HarvestCrop (y, d, SimControl->simStartYear, &Community->Crop[i], Residue, Soil, SoilCarbon, Weather);
                    Community->NumActiveCrop--;
                }
            }
            else if (clippingFlag == 1)
            {
                if (Community->Crop[i].svShoot >= Community->Crop[i].userClippingBiomassThresholdLower * (1.0 - exp (-Community->Crop[i].userPlantingDensity)))
                {
                    ForageHarvest (y, d, SimControl->simStartYear, &Community->Crop[i], Residue, Soil, SoilCarbon, Weather);
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

void CropStage (int d, comm_struct *Community, int last_doy)
{
    int             i;

    for (i = 0; i < Community->NumCrop; i++)
    {
        if (Community->Crop[i].stageGrowth > NO_CROP)
        {
            if (Community->Crop[i].svTT_Cumulative < Community->Crop[i].userEmergenceTT)
                Community->Crop[i].stageGrowth = PRE_EMERGENCE;
            else if (Community->Crop[i].svTT_Cumulative < Community->Crop[i].userFloweringTT)
            {
                if (Community->Crop[i].userAnnual)
                    Community->Crop[i].stageGrowth = VEGETATIVE_GROWTH;
                else
                    Community->Crop[i].stageGrowth = PERENNIAL;
            }
            else if (Community->Crop[i].svTT_Cumulative < Community->Crop[i].userMaturityTT)
            {
                if (Community->Crop[i].userAnnual)
                    Community->Crop[i].stageGrowth = REPRODUCTIVE_GROWTH;
                else
                    Community->Crop[i].stageGrowth = PERENNIAL;
            }
            else if (Community->Crop[i].svTT_Cumulative >= Community->Crop[i].userMaturityTT)
            {
                Community->Crop[i].stageGrowth = MATURITY;

                if (Community->Crop[i].harvestDateFinal < 1)
                {
                    /* SetCropStatusToMature */
                    Community->Crop[i].cropMature = 1;
                    Community->Crop[i].harvestDateFinal = FinalHarvestDate (last_doy, d);
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

int ForcedClipping (int d, comm_struct *Community)
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
                    if (Community->Crop[i].userClippingTiming <= Community->Crop[i].svTT_Cumulative / Community->Crop[i].userMaturityTT &&
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

void FirstDOY (int *rotationYear, int yearsInRotation, int totalLayers, soilc_struct *SoilCarbon, residue_struct *Residue, const soil_struct *Soil)
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

void LastDOY (int y, int simStartYear, int totalLayers, soil_struct *Soil, soilc_struct *SoilCarbon, residue_struct *Residue, summary_struct *Summary)
{
    int             i;

    for (i = 0; i < totalLayers; i++)
    {
        SoilCarbon->carbonMassFinal[i] = Soil->SOC_Mass[i];
    }

#ifndef _PIHM_
    PrintAnnualOutput (y, simStartYear, Soil, SoilCarbon);

    PrintCarbonEvolution (y, simStartYear, totalLayers, Soil, SoilCarbon, Residue);
#endif

    StoreSummary (Summary, SoilCarbon, Residue, totalLayers, y);
}
