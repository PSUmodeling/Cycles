#include "include/Cycles.h"

void GrainHarvest (int y, int doy, int startYear, CropStruct *Crop, ResidueStruct *Residue, const SoilStruct *Soil, SoilCarbonStruct *SoilCarbon, const char *project)
{
    /*
     * Update roots and residue biomass at harvest
     */
    double          HI, NHI;
    double          residueMass;
    double          forageYield;
    double          retainedResidue;
    double          grainNitrogenYield;
    double          forageNitrogenYield;

    if (verbose_mode)
        printf ("DOY %3.3d %-20s %s\n", doy, "Grain Harvest", Crop->cropName);

    HI = ComputeHarvestIndex (Crop->userHIx, Crop->userHIo, Crop->userHIk, Crop->svShoot, Crop->svPostFloweringShootBiomass);
    NHI = pow (HI, 1.0 - 2.0 * (1.0 - HI) * HI);
    residueMass = Crop->svShoot * (1.0 - HI);
    forageYield = residueMass * Crop->userFractionResidueRemoved;
    grainNitrogenYield = Crop->svN_Shoot * NHI;
    forageNitrogenYield = (Crop->svN_Shoot - grainNitrogenYield) * Crop->userFractionResidueRemoved;

    retainedResidue = residueMass - forageYield;
    Residue->stanResidueMass += retainedResidue * Crop->userFractionResidueStanding;
    Residue->flatResidueMass += retainedResidue * (1.0 - Crop->userFractionResidueStanding);
    Residue->stanResidueN += (Crop->svN_Shoot - grainNitrogenYield - forageNitrogenYield) * Crop->userFractionResidueStanding;
    Residue->flatResidueN += (Crop->svN_Shoot - grainNitrogenYield - forageNitrogenYield) * (1.0 - Crop->userFractionResidueStanding);
    /* Assume 33% residue moisture at harvest */
    Residue->stanResidueWater += retainedResidue * Crop->userFractionResidueStanding / 10.0 * 0.5;
    Residue->flatResidueWater += retainedResidue * (1.0 - Crop->userFractionResidueStanding) / 10.0 * 0.5;

    /* add roots of harvested annual crop to a root residue pool in each
     * layer */
    DistributeRootDetritus (y, Crop->svRoot, 0.0, Crop->svN_Root, 0.0, Soil, Crop, Residue, SoilCarbon);

    /* yearly output variables */
    Residue->yearResidueBiomass += retainedResidue;
    Residue->yearRootBiomass += Crop->svRoot;
    Residue->yearRhizodepositionBiomass += Crop->svRizho;

    /* season outputs */
    Crop->rcBiomass = Crop->svBiomass;
    Crop->rcRoot = Crop->svRoot;
    Crop->rcGrainYield = Crop->svShoot * HI;
    Crop->rcResidueBiomass = retainedResidue;
    Crop->rcForageYield = forageYield;
    Crop->rcHarvestIndex = HI;
    Crop->rcTotalNitrogen = Crop->svN_Shoot + Crop->svN_Root;
    Crop->rcRootNitrogen = Crop->svN_Root;
    Crop->rcGrainNitrogenYield = grainNitrogenYield;
    Crop->rcForageNitrogenYield = forageNitrogenYield;
    Crop->rcNitrogenCumulative = Crop->svN_StressCumulative;
    //Crop->rcYear = y + startYear - 1;
    //Crop->rcDoy = doy;
    //Crop->rcActiveStatus = 0;

    PrintSeasonOutput (y, doy, startYear, Crop, project);

    KillCrop (Crop);
}

void ForageHarvest (int y, int doy, int startYear, CropStruct *Crop, ResidueStruct *Residue, const SoilStruct *Soil, SoilCarbonStruct *SoilCarbon, const char *project)
{
    double          forageYield;
    double          residueMass;
    double          rootMassDead;
    double          nitrogenForageYield;
    double          Residue_N_Mass;
    double          Root_N_Mass_Dead;
    double          nitrogenStressCumulative;   /* stored for reporting before
                                                 * resetting after harvest  */
    const double    fractionalHarvestLosses = 0.05;
    double          fractionalNitrogenRemoval;

    if (verbose_mode)
        printf ("DOY %3.3d %-20s %s\n", doy, "Forage Harvest", Crop->cropName);

    fractionalNitrogenRemoval = 1.0 - pow (1.0 - Crop->userFractionResidueRemoved, 0.7);

    /* With this method, forage yield is accumulated over the life of the crop
     * for season harvest (not per year) unless a new variable is created ...
     * Fractional_Nitrogen_Removal allows for N to remain in the crop to avoid
     * harvest-induced N stress. The negative effect is that the removed
     * biomass has a lower N concentration than the remaining biomass, which
     * is likely incorrect to fix this, N should be allocated to different
     * compartments and re-allocated after a clipping event */

    nitrogenStressCumulative = Crop->svN_StressCumulative;

    forageYield = Crop->svShoot * Crop->userFractionResidueRemoved * (1.0 - fractionalHarvestLosses);
    residueMass = Crop->svShoot * Crop->userFractionResidueRemoved * fractionalHarvestLosses;
    rootMassDead = Crop->svRoot * Crop->userFractionResidueRemoved;
    nitrogenForageYield = Crop->svN_Shoot * fractionalNitrogenRemoval * (1.0 - fractionalHarvestLosses);
    Residue_N_Mass = Crop->svN_Shoot * Crop->userFractionResidueRemoved * fractionalHarvestLosses;
    Root_N_Mass_Dead = Crop->svN_Root * Crop->userFractionResidueRemoved;

    /* add roots of clipped crop to a root residue pool in each layer */
    DistributeRootDetritus (y, rootMassDead, 0.0, Root_N_Mass_Dead, 0.0, Soil, Crop, Residue, SoilCarbon);

    Crop->svBiomass -= (forageYield + residueMass + rootMassDead);
    Crop->svShoot -= (forageYield + residueMass);
    Crop->svRoot -= rootMassDead;
    Crop->svN_Shoot -= (nitrogenForageYield + Residue_N_Mass);
    Crop->svN_Root -= Root_N_Mass_Dead;
    /* Resetting of cumulative nitrogen stress after haverst */
    Crop->svN_StressCumulative *= (Crop->svTT_Cumulative - Crop->userEmergenceTT) * (1.0 - pow (Crop->userFractionResidueRemoved, 0.75)) / Crop->calculatedMaturityTT;
    Crop->svTT_Cumulative = Crop->userEmergenceTT + (Crop->svTT_Cumulative - Crop->userEmergenceTT) * (1.0 - pow (Crop->userFractionResidueRemoved, 0.75));
    Crop->svRadiationInterception = Crop->svRadiationInterception * (1.0 - pow (Crop->userFractionResidueRemoved, 0.75));
    Crop->svShootUnstressed = Crop->svShoot;

    Residue->stanResidueMass += residueMass * Crop->userFractionResidueStanding;
    Residue->flatResidueMass += residueMass * (1.0 - Crop->userFractionResidueStanding);
    Residue->stanResidueN += Residue_N_Mass * Crop->userFractionResidueStanding;
    Residue->flatResidueN += Residue_N_Mass * (1.0 - Crop->userFractionResidueStanding);
    /* Assume 33% residue moisture at harvest */
    Residue->stanResidueWater += residueMass * Crop->userFractionResidueStanding / 10.0 * 0.5;
    Residue->flatResidueWater += residueMass * (1.0 - Crop->userFractionResidueStanding) / 10.0 * 0.5;

    /* yearly output variables */
    Residue->yearResidueBiomass += residueMass;
    Residue->yearRootBiomass += rootMassDead;

    /* season outputs */
    Crop->rcBiomass += forageYield + residueMass + rootMassDead;
    Crop->rcRoot += rootMassDead;
    Crop->rcForageYield += forageYield;
    Crop->rcResidueBiomass += residueMass;
    Crop->rcTotalNitrogen = nitrogenForageYield + Residue_N_Mass + Root_N_Mass_Dead;
    Crop->rcRootNitrogen = Root_N_Mass_Dead;
    Crop->rcForageNitrogenYield = nitrogenForageYield;
    Crop->rcNitrogenCumulative = nitrogenStressCumulative / Crop->userClippingTiming;
    //Crop->rcYear = y + startYear;
    //Crop->rcDoy = doy;

    PrintSeasonOutput (y, doy, startYear, Crop, project);
}

void HarvestCrop (int y, int doy, int startYear, CropStruct *Crop, ResidueStruct *Residue, const SoilStruct *Soil, SoilCarbonStruct *SoilCarbon, const char *project)
{
    /*
     * Set crop status to Killed
     * Final crop values based on a killed crop performed
     */

    double          residueMass;
    double          rootMassDead;
    double          Residue_N_Mass;
    double          Root_N_Mass_Dead;

    residueMass = Crop->svShoot;
    rootMassDead = Crop->svRoot;
    Residue_N_Mass = Crop->svN_Shoot;
    Root_N_Mass_Dead = Crop->svN_Root;

    Residue->stanResidueMass += residueMass * Crop->userFractionResidueStanding;
    Residue->flatResidueMass += residueMass * (1.0 - Crop->userFractionResidueStanding);
    Residue->stanResidueN += Residue_N_Mass * Crop->userFractionResidueStanding;
    Residue->flatResidueN += Residue_N_Mass * (1.0 - Crop->userFractionResidueStanding);
    /* Assume 33% residue moisture at harvest */
    Residue->stanResidueWater += residueMass * Crop->userFractionResidueStanding / 10.0 * 0.5;
    Residue->flatResidueWater += residueMass * (1.0 - Crop->userFractionResidueStanding) / 10.0 * 0.5;

    /* Add roots of terminated crop to a root residue pool in each layer */
    DistributeRootDetritus (y, rootMassDead, 0.0, Root_N_Mass_Dead, 0.0, Soil, Crop, Residue, SoilCarbon);

    /* Yearly output variables */
    Residue->yearResidueBiomass += residueMass;
    Residue->yearRootBiomass += rootMassDead;
    Residue->yearRhizodepositionBiomass += Crop->svRizho;

    /* Season outputs */
    Crop->rcBiomass += Crop->svBiomass;
    Crop->rcRoot += Crop->svRoot;
    Crop->rcResidueBiomass += residueMass;
    //Crop->rcYear = y + startYear;
    //Crop->rcDoy = doy;
    //Crop->rcActiveStatus = 0;

    if (verbose_mode)
        printf ("DOY %3.3d %-20s %s\n", doy, "Harvest", Crop->cropName);

    PrintSeasonOutput (y, doy, startYear, Crop, project);

    KillCrop (Crop);
}

void DistributeRootDetritus (int y, double rootMass, double rhizoMass, double rootN, double rhizoN, const SoilStruct *Soil, const CropStruct *Crop, ResidueStruct *Residue, SoilCarbonStruct *SoilCarbon)
{
    /*
     * This subroutine distributes dead roots and rizhodeposition in soil
     * layer. The function always computes the fraction of roots based on
     * maximum root depth for the crop then, it corrects the allocation using
     * the actual rooting depth this prevents an over-representation of the
     * roots in the top layer
     * The function is the same than that for root distribution used in water
     * uptake, but is kept in a separate sub in case parameterization
     * changes
     */

    /* Function distribution Root=a*exp[-b*z], with b~4 */
    /* Integral to compute mass in each layer
     * Root_layer = a / b * (Exp(-b * z1) - Exp(-b * z2)),
     * where z1 and z2 are the top and bottom of layer */

    const double    a = 1.0;
    const double    b = 4.0;    /* units of 1/m */
    double          rootIntegral;
    double          rootSum;
    double          cumulativeRootingDepth = 0.0;
    double          z1, z2;
    int             i, j = 0;
    double          rootDistribution[Soil->totalLayers];
    double          fractionRootsByLayer[Soil->totalLayers];

    rootIntegral = a / b * (exp (-b * 0.0) - exp (-b * Crop->userMaximumRootingDepth));

    for (i = 0; i < Soil->totalLayers; i++)
    {
        rootDistribution[i] = 0.0;
        fractionRootsByLayer[i] = 0.0;
    }

    j = 0;
    while (cumulativeRootingDepth < Crop->svRootingDepth && j < Soil->totalLayers)
    {
        if (Soil->cumulativeDepth[j] < Crop->svRootingDepth)
            cumulativeRootingDepth = Soil->cumulativeDepth[j];
        else
            cumulativeRootingDepth = Crop->svRootingDepth;

        if (j == 0)
            z1 = 0.0;
        else
            z1 = Soil->cumulativeDepth[j - 1];

        z2 = cumulativeRootingDepth;
        rootDistribution[j] = ((a / b) * (exp (-b * z1) - exp (-b * z2))) / rootIntegral;
        j++;
    }

    /* Ensures that cumulative fractional root distribution = 1 */
    rootSum = 0.0;
    for (i = 0; i < j; i++)
        rootSum = rootSum + rootDistribution[i];

    /* compute input of biomass from roots to each layer */
    for (i = 0; i < j; i++)     
    {   /* exits loop on the same layer as the previous loop */
        if (rootMass > 0.0)
        {
            fractionRootsByLayer[i] = rootDistribution[i] / rootSum;
            Residue->residueRt[i] += fractionRootsByLayer[i] * rootMass;
            Residue->residueRtN[i] += fractionRootsByLayer[i] * rootN;
            SoilCarbon->rootBiomassInput[i] += fractionRootsByLayer[i] * rootMass;
        }

        if (rhizoMass > 0.0)
        {
            Residue->residueRz[i] += fractionRootsByLayer[i] * rhizoMass;
            Residue->residueRzN[i] += fractionRootsByLayer[i] * rhizoN;
            SoilCarbon->rhizBiomassInput[i] += fractionRootsByLayer[i] * rhizoMass;
        }
    }
}

double ComputeHarvestIndex (double HIx, double HIo, double HIk, double cumulativeShoot, double cumulativePostFloweringShootBiomass)
{
    /*
     * Update roots and residue biomass at harvest
     */
    double          fg;         /* fractional post-anthesis growth */
    double          harvest_index;

    if (GT (cumulativePostFloweringShootBiomass, 0.0) && GT (cumulativeShoot, 0.0))
    {
        fg = cumulativePostFloweringShootBiomass / cumulativeShoot;
        harvest_index = (HIx - (HIx - HIo) * exp (-HIk * fg));
    }
    else
        harvest_index = 0.0;

    return (harvest_index);
}
