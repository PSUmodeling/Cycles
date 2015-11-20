#include "Cycles.h"

#ifndef _CYCLES_
void Initialize (SimControlStruct *SimControl, WeatherStruct *Weather, SoilStruct *Soil, ResidueStruct *Residue, SoilCarbonStruct *SoilCarbon, CommunityStruct *Community, CropManagementStruct *CropManagement, SnowStruct *Snow, SummaryStruct *Summary)
{
    int             i;
    /* Initialize weather variables */
    CalculateDerivedWeather (Weather, SimControl->totalYears);

    /* Initialize soil variables */
    InitializeSoil (Soil, Weather, SimControl);

    /* Initialize residue */
    InitializeResidue (Residue, SimControl->totalYears, Soil->totalLayers);

    /* Initialize soil carbon */
    InitializeSoilCarbon (SoilCarbon, Soil->totalLayers);

    /* Initialize crops */
    Community->NumActiveCrop = 0;
    for (i = 0; i < Community->NumCrop; i++)
        Community->Crop[i].stageGrowth = NO_CROP;

    /* Initialize tillage factors */
    CropManagement->tillageFactor = (double *)malloc (Soil->totalLayers * sizeof (double));

    /* Initialize snow structure */
    Snow->Snow = 0.0;

    /* Initialize summary structure */
    *Summary = (SummaryStruct) {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
}
#endif

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

void LastDOY (int y, int simStartYear, int totalLayers, SoilStruct *Soil, SoilCarbonStruct *SoilCarbon, ResidueStruct *Residue, SummaryStruct *Summary, char *project)
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

void StoreSummary (SummaryStruct *Summary, const SoilCarbonStruct *SoilCarbon, const ResidueStruct *Residue, int totalLayers, int y)
{
    int             i;

    Summary->final_soc = 0.0;

    for (i = 0; i < totalLayers; i++)
    {
        Summary->abgd_c_input += SoilCarbon->abgdCarbonInput[i];
        Summary->root_c_input += SoilCarbon->rootCarbonInput[i] + SoilCarbon->rhizCarbonInput[i];
        Summary->residue_resp += SoilCarbon->annualRespiredResidueCarbonMass[i];
        Summary->hum += SoilCarbon->annualHumifiedCarbonMass[i];
        Summary->soil_resp += SoilCarbon->annualRespiredCarbonMass[i];
        
        Summary->n_mineralization += SoilCarbon->annualNmineralization[i];
        Summary->n_immobilization += SoilCarbon->annualNImmobilization[i];
        Summary->n_net_mineralization += SoilCarbon->annualNNetMineralization[i];
        Summary->final_soc += SoilCarbon->carbonMassFinal[i];
        if (y == 0)
            Summary->initial_soc += SoilCarbon->carbonMassInitial[i];
    }
    
    Summary->nh4_nitrification += SoilCarbon->annualAmmoniumNitrification;
    Summary->n2o_from_nitrification += SoilCarbon->annualNitrousOxidefromNitrification;
    Summary->nh3_volatilization += SoilCarbon->annualAmmoniaVolatilization;
    Summary->no3_denirification += SoilCarbon->annualNO3Denitrification;
    Summary->n2o_from_denitrification += SoilCarbon->annualNitrousOxidefromDenitrification;
    Summary->no3_leaching += SoilCarbon->annualNitrateLeaching;
    Summary->nh4_leaching += SoilCarbon->annualAmmoniumLeaching;
    
    Summary->residue_biomass += Residue->yearResidueBiomass;
    Summary->produced_root += Residue->yearRootBiomass + Residue->yearRhizodepositionBiomass;
}
//void FreeCyclesStruct (CyclesStruct Cycles, int total_years)
//{
//    /*
//     * -----------------------------------------------------------------------
//     * LOCAL VARIABLES
//     *
//     * Variable             Type        Description
//     * ==========           ==========  ====================
//     * i		    int		Loop counter
//     */
//    int             i;
//
//    free (Cycles->CropManagement.tillageFactor);
//    free (Cycles->CropManagement.plantingOrder);
//    free (Cycles->CropManagement.FixedFertilization);
//    free (Cycles->CropManagement.FixedIrrigation);
//    free (Cycles->CropManagement.Tillage);
//    free (Cycles->CropManagement.autoIrrigation);
//    free (Cycles->CropManagement.ForcedHarvest);
//
//    free (Cycles->Community.Crop);
//
//    free (Cycles->Soil.layerThickness);
//    free (Cycles->Soil.Clay);
//    free (Cycles->Soil.Sand);
//    free (Cycles->Soil.IOM);
//    free (Cycles->Soil.BD);
//    free (Cycles->Soil.FC);
//    free (Cycles->Soil.PWP);
//    free (Cycles->Soil.NO3);
//    free (Cycles->Soil.NH4);
//    free (Cycles->Soil.nodeDepth);
//    free (Cycles->Soil.cumulativeDepth);
//    free (Cycles->Soil.waterContent);
//    free (Cycles->Soil.soilTemperature);
//    free (Cycles->Soil.Porosity);
//    free (Cycles->Soil.PAW);
//    free (Cycles->Soil.FC_WaterPotential);
//    free (Cycles->Soil.airEntryPotential);
//    free (Cycles->Soil.B_Value);
//    free (Cycles->Soil.M_Value);
//    free (Cycles->Soil.SOC_Conc);
//    free (Cycles->Soil.SOC_Mass);
//    free (Cycles->Soil.SON_Mass);
//    free (Cycles->Soil.MBC_Mass);
//    free (Cycles->Soil.MBN_Mass);
//    free (Cycles->Soil.waterUptake);
//    free (Cycles->Soil.pH);
//    free (Cycles->Soil.n2o);
//
//    for (i = 0; i < total_years; i++)
//    {
//        free (Cycles->Weather.wind[i]);
//        free (Cycles->Weather.ETref[i]);
//        free (Cycles->Weather.precipitation[i]);
//        free (Cycles->Weather.RHmax[i]);
//        free (Cycles->Weather.RHmin[i]);
//        free (Cycles->Weather.solarRadiation[i]);
//        free (Cycles->Weather.tMax[i]);
//        free (Cycles->Weather.tMin[i]);
//    }
//    free (Cycles->Weather.wind);
//    free (Cycles->Weather.ETref);
//    free (Cycles->Weather.precipitation);
//    free (Cycles->Weather.RHmax);
//    free (Cycles->Weather.RHmin);
//    free (Cycles->Weather.solarRadiation);
//    free (Cycles->Weather.tMax);
//    free (Cycles->Weather.tMin);
//    free (Cycles->Weather.yearlyAmplitude);
//    free (Cycles->Weather.annualAverageTemperature);
//    free (Cycles->Weather.lastDoy);
//
//    free (Cycles->Residue.residueAbgd);
//    free (Cycles->Residue.residueRt);
//    free (Cycles->Residue.residueRz);
//    free (Cycles->Residue.residueAbgdN);
//    free (Cycles->Residue.residueRtN);
//    free (Cycles->Residue.residueRzN);
//    free (Cycles->Residue.manureC);
//    free (Cycles->Residue.manureN);
//
//    free (Cycles->SoilCarbon.factorComposite);
//    free (Cycles->SoilCarbon.carbonRespired);
//    free (Cycles->SoilCarbon.rootBiomassInput);
//    free (Cycles->SoilCarbon.rhizBiomassInput);
//    free (Cycles->SoilCarbon.abgdBiomassInput);
//    free (Cycles->SoilCarbon.rootCarbonInput);
//    free (Cycles->SoilCarbon.rhizCarbonInput);
//    free (Cycles->SoilCarbon.manuCarbonInput);
//    free (Cycles->SoilCarbon.abgdCarbonInput);
//    free (Cycles->SoilCarbon.carbonMassInitial);
//    free (Cycles->SoilCarbon.carbonMassFinal);
//    free (Cycles->SoilCarbon.annualDecompositionFactor);
//    free (Cycles->SoilCarbon.annualSoilCarbonDecompositionRate);
//    free (Cycles->SoilCarbon.annualCarbonInputByLayer);
//    free (Cycles->SoilCarbon.annualHumifiedCarbonMass);
//    free (Cycles->SoilCarbon.annualRespiredCarbonMass);
//    free (Cycles->SoilCarbon.annualRespiredResidueCarbonMass);
//    free (Cycles->SoilCarbon.annualHumificationCoefficient);
//    free (Cycles->SoilCarbon.annualNmineralization);
//    free (Cycles->SoilCarbon.annualNImmobilization);
//    free (Cycles->SoilCarbon.annualNNetMineralization);
//}
