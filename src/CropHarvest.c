#include "include/Cycles.h"

void GrainHarvest (int y, int doy, int startYear, CropStruct *Crop, RealizedCropStruct *RealizedCrop, ResidueStruct *Residue, SoilStruct *Soil, SoilCarbonStruct *SoilCarbon)
{
    /* update roots and residue biomass at harvest */

    double HI, NHI;
    double residueMass = 0.;
    double forageYield = 0.;
    double retainedResidue = 0.;
    double grainNitrogenYield = 0.;
    double forageNitrogenYield = 0.;

    HI = ComputeHarvestIndex (Crop->userHIx, Crop->userHIo, Crop->userHIk, Crop->svShoot, Crop->svPostFloweringShootBiomass);
    NHI = pow (HI, 1. - 2. * (1. - HI) * HI);
    residueMass = Crop->svShoot * (1. - HI);
    forageYield = residueMass * Crop->userFractionResidueRemoved;
    grainNitrogenYield = Crop->svN_Shoot * NHI;
    forageNitrogenYield = (Crop->svN_Shoot - grainNitrogenYield) * Crop->userFractionResidueRemoved;

    retainedResidue = residueMass - forageYield;
    Residue->stanResidueMass += retainedResidue * Crop->userFractionResidueStanding;
    Residue->flatResidueMass += retainedResidue * (1. - Crop->userFractionResidueStanding);
    Residue->stanResidueN += (Crop->svN_Shoot - grainNitrogenYield - forageNitrogenYield) * Crop->userFractionResidueStanding;
    Residue->flatResidueN += (Crop->svN_Shoot - grainNitrogenYield - forageNitrogenYield) * (1. - Crop->userFractionResidueStanding);
    Residue->stanResidueWater += retainedResidue * Crop->userFractionResidueStanding / 10. * 0.5;       /* assume 33% residue moisture at harvest */
    Residue->flatResidueWater += retainedResidue * (1 - Crop->userFractionResidueStanding) / 10 * 0.5;  /* assume 33% residue moisture at harvest */

    /* add roots of harvested annual crop to a root residue pool in each layer */
    DistributeRootDetritus(y, Crop->svRoot, 0, Crop->svN_Root, 0, Soil, Crop, Residue, SoilCarbon);

    /* yearly output variables */
    Residue->yearResidueBiomass += retainedResidue;
    Residue->yearRootBiomass += Crop->svRoot;
    Residue->yearRhizodepositionBiomass += Crop->svRizho;

    /* season outputs */
    RealizedCrop->rcBiomass = Crop->svBiomass;
    RealizedCrop->rcRoot = Crop->svRoot;
    RealizedCrop->rcGrainYield = Crop->svShoot * HI;
    RealizedCrop->rcResidueBiomass = retainedResidue;
    RealizedCrop->rcForageYield = forageYield;
    RealizedCrop->rcHarvestIndex = HI;
    RealizedCrop->rcTotalNitrogen = Crop->svN_Shoot + Crop->svN_Root;
    RealizedCrop->rcRootNitrogen = Crop->svN_Root;
    RealizedCrop->rcGrainNitrogenYield = grainNitrogenYield;
    RealizedCrop->rcforageNitrogenYield = forageNitrogenYield;
    RealizedCrop->rcNitrogenCumulative = Crop->svN_StressCumulative;

    RealizedCrop->rcYear = y + startYear - 1;
    RealizedCrop->rcDoy = doy;
    RealizedCrop->rcActiveStatus = 0;

    KillCrop(Crop->cropUniqueIdentifier);
}

void ForageHarvest(int y, int doy, int startYear, CropStruct *Crop, RealizedCropStruct *RealizedCrop, ResidueStruct *Residue, SoilStruct *Soil, SoilCarbonStruct *SoilCarbon)
{

    double forageYield;
    double residueMass;
    double rootMassDead;
    double nitrogenForageYield;
    double Residue_N_Mass;
    double Root_N_Mass_Dead;
    double nitrogenStressCumulative;        /* stored for reporting before resetting after harvest */
    const double fractionalHarvestLosses = 0.05;
    double fractionalNitrogenRemoval;

    fractionalNitrogenRemoval = 1. - pow (1. - Crop->userFractionResidueRemoved, 0.7);

    /* with this method, forage yield is accumulated over the life of the crop
     * for season harvest (not per year) unless a new variable is created ...
     * Fractional_Nitrogen_Removal allows for N to remain in the crop to avoid
     * harvest-induced N stress. The negative effect is that the removed
     * biomass has a lower N concentration than the remaining biomass, which
     * is likely incorrect to fix this, N should be allocated to different
     * compartments and re-allocated after a clipping event */

    nitrogenStressCumulative = Crop->svN_StressCumulative;

    forageYield = Crop->svShoot * Crop->userFractionResidueRemoved * (1. - fractionalHarvestLosses);
    residueMass = Crop->svShoot * Crop->userFractionResidueRemoved * fractionalHarvestLosses;
    rootMassDead = Crop->svRoot * Crop->userFractionResidueRemoved;
    nitrogenForageYield = Crop->svN_Shoot * fractionalNitrogenRemoval * (1. - fractionalHarvestLosses);
    Residue_N_Mass = Crop->svN_Shoot * Crop->userFractionResidueRemoved * fractionalHarvestLosses;
    Root_N_Mass_Dead = Crop->svN_Root * Crop->userFractionResidueRemoved;

    /* add roots of clipped crop to a root residue pool in each layer */
    DistributeRootDetritus (y, rootMassDead, 0, Root_N_Mass_Dead, 0, Soil, Crop, Residue, SoilCarbon);

    Crop->svBiomass -= (forageYield + residueMass + rootMassDead);
    Crop->svShoot -= (forageYield + residueMass);
    Crop->svRoot -= rootMassDead;
    Crop->svN_Shoot -= (nitrogenForageYield + Residue_N_Mass);
    Crop->svN_Root -= Root_N_Mass_Dead;
    Crop->svN_StressCumulative *= (Crop->svTT_Cumulative - Crop->userEmergenceTT) * (1. - pow (Crop->userFractionResidueRemoved, 0.75)) / Crop->calculatedMaturityTT;   /* resetting of cumulative nitrogen stress after haverst */
    Crop->svTT_Cumulative = Crop->userEmergenceTT + (Crop->svTT_Cumulative - Crop->userEmergenceTT) * (1. - pow (Crop->userFractionResidueRemoved, 0.75));
    Crop->svRadiationInterception = Crop->svRadiationInterception * (1. - pow (Crop->userFractionResidueRemoved, 0.75));
    Crop->svShootUnstressed = Crop->svShoot;    /* try this approach first */

    Residue->stanResidueMass += residueMass * Crop->userFractionResidueStanding;
    Residue->flatResidueMass += residueMass * (1. - Crop->userFractionResidueStanding);
    Residue->stanResidueN += Residue_N_Mass * Crop->userFractionResidueStanding;
    Residue->flatResidueN += Residue_N_Mass * (1. - Crop->userFractionResidueStanding);
    Residue->stanResidueWater += residueMass * Crop->userFractionResidueStanding / 10. * 0.5;       /* assume 33% residue moisture at harvest */
    Residue->flatResidueWater += residueMass * (1. - Crop->userFractionResidueStanding) / 10. * 0.5;    /* assume 33% residue moisture at harvest */

    /* yearly output variables */
    Residue->yearResidueBiomass += residueMass;
    Residue->yearRootBiomass += rootMassDead;

    /* season outputs */
    RealizedCrop->rcBiomass += forageYield + residueMass + rootMassDead;
    RealizedCrop->rcRoot += rootMassDead;
    RealizedCrop->rcForageYield += forageYield;
    RealizedCrop->rcResidueBiomass += residueMass;
    RealizedCrop->rcTotalNitrogen = nitrogenForageYield + Residue_N_Mass + Root_N_Mass_Dead;
    RealizedCrop->rcRootNitrogen = Root_N_Mass_Dead;
    RealizedCrop->rcforageNitrogenYield = nitrogenForageYield;
    RealizedCrop->rcNitrogenCumulative = nitrogenStressCumulative / Crop->userClippingTiming;

    RealizedCrop->rcYear = y + startYear;
    RealizedCrop->rcDoy = doy;
}

void HarvestCrop (int y, int doy, int startYear, CropStruct *Crop, RealizedCropStruct *RealizedCrop, ResidueStruct *Residue, SoilStruct *Soil, SoilCarbonStruct *SoilCarbon)
{

    /* status set to Killed
     * Final crop values based on a killed crop performed */

    double residueMass;
    double rootMassDead;
    double Residue_N_Mass;
    double Root_N_Mass_Dead;

    residueMass = Crop->svShoot;
    rootMassDead = Crop->svRoot;
    Residue_N_Mass = Crop->svN_Shoot;
    Root_N_Mass_Dead = Crop->svN_Root;

    Residue->stanResidueMass += residueMass * Crop->userFractionResidueStanding;
    Residue->flatResidueMass += residueMass * (1. - Crop->userFractionResidueStanding);
    Residue->stanResidueN += Residue_N_Mass * Crop->userFractionResidueStanding;
    Residue->flatResidueN += Residue_N_Mass * (1. - Crop->userFractionResidueStanding);
    Residue->stanResidueWater += residueMass * Crop->userFractionResidueStanding / 10. * 0.5;       /* assume 33% residue moisture at harvest */
    Residue->flatResidueWater += residueMass * (1. - Crop->userFractionResidueStanding) / 10. * 0.5;/* assume 33% residue moisture at harvest */

    /* add roots of terminated crop to a root residue pool in each layer */
    DistributeRootDetritus (y, rootMassDead, 0, Root_N_Mass_Dead, 0, Soil, Crop, Residue, SoilCarbon);

    /* yearly output variables */
    Residue->yearResidueBiomass += residueMass;
    Residue->yearRootBiomass += rootMassDead;
    Residue->yearRhizodepositionBiomass += Crop->svRizho;

    /* season outputs */
    RealizedCrop->rcBiomass += Crop->svBiomass;
    RealizedCrop->rcRoot += Crop->svRoot;
    RealizedCrop->rcResidueBiomass += residueMass;

    RealizedCrop->rcYear = y + startYear;
    RealizedCrop->rcDoy = doy;
    RealizedCrop->rcActiveStatus = 0;

    KillCrop(Crop->cropUniqueIdentifier);
}

void SimulatedAverageYields (CropStruct *Crop, RealizedCropStruct *RealizedCrop, ResidueStruct *Residue);
{
    typedef struct cropYieldInformationStruct
    {
        char Name[128];
        double SumYield;
        int TimesInRotation;
        double average;
        double max;
        double min;
    } cropYieldInformationStruct;

    cropYieldInformationStruct cropYieldInformation[CropManagement->CropsPerRotation];
    double tempYield;

    SelectCropInitialPosition;

    for (c = 0; c < CropManagement->CropsPerRotation; c++)
    {
        SelectNextCrop();
        cropYieldInformation[c].Name = Crop->cropName;
        tempYield = RealizedCrop->rcGrainYield;

        cropYieldInformation[c].SumYield += tempYield
        cropYieldInformation[c].TimesInRotation += 1;

        if (cropYieldInformation[c].TimesInRotation == 1)
        {
            cropYieldInformation[c].max = tempYield;
            cropYieldInformation[c].min = tempYield;
        }
        else
        {
            if (tempYield > cropYieldInformation[c].max)
                cropYieldInformation[c].max = tempYield;
            if (tempYield < cropYieldInformation[c])
                cropYieldInformation[c].min = tempYield;
        }
    }

    SelectCropInitialPosition;

    for (c = 0; c < CropManagement->CropsPerRotation)
    {
        SelectNextCrop;
        if (cropYieldInformation[c].TimesInRotation > 0)
            cropYieldInformation[c].average = cropYieldInformation[c].SumYield / cropYieldInformation[c].TimesInRotation;
        Crop->calculatedSimAvgYield = cropYieldInformation[c].average;
        Crop->calculatedSimMaxYield = cropYieldInformation[c].max;
        Crop->calculatedSimMinYield = cropYieldInformation[c].min;
    }
}
 
void DistributeRootDetritus (int y, double rootMass, double rhizoMass, double rootN, double rhizoN, SoilStruct *Soil, CropStruct *Crop, ResidueStruct *Residue, SoilCarbonStruct *SoilCarbon)
{
    /*
     * this subroutine distributes dead roots and rizhodeposition in soil
     * layer. The function always computes the fraction of roots based on
     * maximum root depth for the crop then, it corrects the allocation using
     * the actual rooting depth this prevents an over-representation of the
     * roots in the top layer
     */

    /* the function is the same than that for root distribution used in water
     * uptake, but is kept in a separate sub in case parameterization
     * changes */

    /* Function distribution Root=a*exp[-b*z], with b~4 */
    /* Integral to compute mass in each layer
     * Root_layer = a / b * (Exp(-b * z1) - Exp(-b * z2)),
     * where z1 and z2 are the top and bottom of layer */

    const double a = 1.;
    const double b = 4.;    /* units of 1/m */
    double rootIntegral;
    double rootSum;

    double cumulativeRootingDepth = 0.;
    double z1, z2;
    int j = 0;

    double rootDistribution[Soil->totalLayers];
    double fractionRootsByLayer[Soil.totalLayers];

    rootIntegral = a / b * (exp(-b * 0) - exp(-b * Crop->userMaximumRootingDepth));
    while (cumulativeRootingDepth < Crop->svRootingDepth && j < Soil->totalLayers)
    {
        if (Soil->cumulativeDepth[j] < Crop->svRootingDepth)
            cumulativeRootingDepth = Soil->cumulativeDepth[j];
        if (Soil->cumulativeDepth[j] >= Crop->svRootingDepth)
            cumulativeRootingDepth = Crop->svRootingDepth;

        if (j == 0)
            z1 = 0.;
        else
            z1 = Soil->cumulativeDepth[j - 1];

        z2 = cumulativeRootingDepth;
        rootDistribution[j] = ((a / b) * (exp(-b * z1) - exp(-b * z2))) / rootIntegral;
        j++;
    }

    /* ensures that cumulative fractional root distribution = 1 */
    rootSum = 0.;
    for (i = 0; i < j; i++)
        rootSum = rootSum + rootDistribution[i];

    /* compute input of biomass from roots to each layer */
    for (i = 0; i < j; i++)     /* exits loop on the same layer as the previous loop */
    {
        if (rootMass > 0.)
        {
            fractionRootsByLayer[i] = rootDistribution[i] / rootSum;
            Residue->residueRt[i] += fractionRootsByLayer[i] * rootMass;
            Residue->residueRtN[i] += fractionRootsByLayer[i] * rootN;
            SoilCarbon->rootBiomassInput[i] += fractionRootsByLayer[i] * rootMass;
        }

        if (rhizoMass > 0.)
        {
            Residue->residueRz[i] += fractionRootsByLayer[i] * rhizoMass;
            Residue->residueRzN[i] += fractionRootsByLayer[i] * rhizoN;
            SoilCarbon->rhizBiomassInput[i] += fractionRootsByLayer[i] * rhizoMass;
        }
    }
}

double ComputeHarvestIndex (double HIx, double HIo, double HIk, double cumulativeShoot, double cumulativePostFloweringShootBiomass)
{
    /* update roots and residue biomass at harvest: */

    double fg;      /* fractional post-anthesis growth */

    if (cumulativePostFloweringShootBiomass > 0. && cumulativeShoot > 0.)
    {
        fg = cumulativePostFloweringShootBiomass / cumulativeShoot;
        return (HIx - (HIx - HIo) * exp(-HIk * fg));
    }
    else
        return 0.;
}