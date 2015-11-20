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

    CropManagement->rotationYear = 0;

    /* Initialize tillage factors */
    CropManagement->tillageFactor = (double *)malloc (Soil->totalLayers * sizeof (double));

    /* Initialize snow structure */
    Snow->Snow = 0.0;

    /* Initialize summary structure */
    *Summary = (SummaryStruct) {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
}
#endif

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

void FreeCyclesStruct (CropManagementStruct *CropManagement, CommunityStruct *Community, SoilStruct *Soil, WeatherStruct *Weather, ResidueStruct *Residue, SoilCarbonStruct *SoilCarbon, int total_years)
{
    /*
     * -----------------------------------------------------------------------
     * LOCAL VARIABLES
     *
     * Variable             Type        Description
     * ==========           ==========  ====================
     * i		    int		Loop counter
     */
    int             i;

    free (CropManagement->tillageFactor);
    free (CropManagement->plantingOrder);
    free (CropManagement->FixedFertilization);
    free (CropManagement->FixedIrrigation);
    free (CropManagement->Tillage);
    free (CropManagement->autoIrrigation);
    free (CropManagement->ForcedHarvest);

    free (Community->Crop);

    free (Soil->layerThickness);
    free (Soil->Clay);
    free (Soil->Sand);
    free (Soil->IOM);
    free (Soil->BD);
    free (Soil->FC);
    free (Soil->PWP);
    free (Soil->NO3);
    free (Soil->NH4);
    free (Soil->nodeDepth);
    free (Soil->cumulativeDepth);
    free (Soil->waterContent);
    free (Soil->soilTemperature);
    free (Soil->Porosity);
    free (Soil->PAW);
    free (Soil->FC_WaterPotential);
    free (Soil->airEntryPotential);
    free (Soil->B_Value);
    free (Soil->M_Value);
    free (Soil->SOC_Conc);
    free (Soil->SOC_Mass);
    free (Soil->SON_Mass);
    free (Soil->MBC_Mass);
    free (Soil->MBN_Mass);
    free (Soil->waterUptake);
    free (Soil->pH);
    free (Soil->n2o);

    for (i = 0; i < total_years; i++)
    {
        free (Weather->wind[i]);
        free (Weather->ETref[i]);
        free (Weather->precipitation[i]);
        free (Weather->RHmax[i]);
        free (Weather->RHmin[i]);
        free (Weather->solarRadiation[i]);
        free (Weather->tMax[i]);
        free (Weather->tMin[i]);
    }
    free (Weather->wind);
    free (Weather->ETref);
    free (Weather->precipitation);
    free (Weather->RHmax);
    free (Weather->RHmin);
    free (Weather->solarRadiation);
    free (Weather->tMax);
    free (Weather->tMin);
    free (Weather->yearlyAmplitude);
    free (Weather->annualAverageTemperature);
    free (Weather->lastDoy);

    free (Residue->residueAbgd);
    free (Residue->residueRt);
    free (Residue->residueRz);
    free (Residue->residueAbgdN);
    free (Residue->residueRtN);
    free (Residue->residueRzN);
    free (Residue->manureC);
    free (Residue->manureN);

    free (SoilCarbon->factorComposite);
    free (SoilCarbon->carbonRespired);
    free (SoilCarbon->rootBiomassInput);
    free (SoilCarbon->rhizBiomassInput);
    free (SoilCarbon->abgdBiomassInput);
    free (SoilCarbon->rootCarbonInput);
    free (SoilCarbon->rhizCarbonInput);
    free (SoilCarbon->manuCarbonInput);
    free (SoilCarbon->abgdCarbonInput);
    free (SoilCarbon->carbonMassInitial);
    free (SoilCarbon->carbonMassFinal);
    free (SoilCarbon->annualDecompositionFactor);
    free (SoilCarbon->annualSoilCarbonDecompositionRate);
    free (SoilCarbon->annualCarbonInputByLayer);
    free (SoilCarbon->annualHumifiedCarbonMass);
    free (SoilCarbon->annualRespiredCarbonMass);
    free (SoilCarbon->annualRespiredResidueCarbonMass);
    free (SoilCarbon->annualHumificationCoefficient);
    free (SoilCarbon->annualNmineralization);
    free (SoilCarbon->annualNImmobilization);
    free (SoilCarbon->annualNNetMineralization);
}
