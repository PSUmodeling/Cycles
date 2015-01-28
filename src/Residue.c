#include "include/Cycles.h"

void InitializeResidue (ResidueStruct * Residue, int totalYears, int totalLayers)
{
    Residue->residueAbgd = (double *)malloc (totalLayers * sizeof (double));
    Residue->residueRt = (double *)malloc (totalLayers * sizeof (double));
    Residue->residueRz = (double *)malloc (totalLayers * sizeof (double));
    Residue->residueAbgdN = (double *)malloc (totalLayers * sizeof (double));
    Residue->residueRtN = (double *)malloc (totalLayers * sizeof (double));
    Residue->residueRzN = (double *)malloc (totalLayers * sizeof (double));
    Residue->manureC = (double *)malloc (totalLayers * sizeof (double));
    Residue->manureN = (double *)malloc (totalLayers * sizeof (double));

    Residue->residueInterception = 0.;
    Residue->stanResidueTau = 1.;
    Residue->flatResidueTau = 1.;
    Residue->stanResidueMass = 0.;
    Residue->flatResidueMass = 0.;
    Residue->stanResidueN = 0.;
    Residue->flatResidueN = 0.;
    Residue->manureSurfaceC = 0.;
    Residue->manureSurfaceN = 0.;
    Residue->stanResidueWater = 0.;
    Residue->flatResidueWater = 0.;
}

void ComputeResidueCover (ResidueStruct * Residue)
{
    double          stanResidueAI;  /* standing residue area index
                                     * (m2 residue / m2 ground) */
    double          flatResidueAI;  /* flat residue area index
                                     * (m2 residue / m2 ground) */

    /* factor 0.1 converts from Mg/ha to kg/m2 */
    stanResidueAI = STAN_RESIDUE_SA * Residue->stanResidueMass * 0.1;
    flatResidueAI = FLAT_RESIDUE_SA * Residue->flatResidueMass * 0.1;
    Residue->stanResidueTau = exp (-STAN_RESIDUE_K * stanResidueAI);
    Residue->flatResidueTau = exp (-FLAT_RESIDUE_K * flatResidueAI);
    Residue->residueInterception = (1. - Residue->stanResidueTau) + Residue->stanResidueTau * (1. - Residue->flatResidueTau);

#ifdef _DEBUG_
    printf ("ComputeResidueCover:\n");
    printf ("stanResidueTau = %lf, flatResidueTau = %lf, residueInterception = %lf\n", Residue->stanResidueTau, Residue->flatResidueTau, Residue->residueInterception);
#endif
}

void ResidueWetting (ResidueStruct * Residue, SoilStruct * Soil)
{
    const double    residueMaxWaterConcentration = 3.3; /* (kg / kg) */
    double          flatResidueWaterDeficit;    /* Water need to saturate residue
                                                 * (mm) */
    double          standingResidueWaterDeficit;    /* mm, water need to saturate residue */
    double          waterWettingResidue;    /* mm, amount of water interceptable by residue */
    double          waterRetainedResidue;   /* mm, water retained in residue and discounted for infiltration */

    flatResidueWaterDeficit = residueMaxWaterConcentration * Residue->flatResidueMass / 10. - Residue->flatResidueWater;    /* 10 converts residue from Mg/ha to kg/m2 */
    standingResidueWaterDeficit = residueMaxWaterConcentration * Residue->stanResidueMass / 10. - Residue->stanResidueWater;    /* 10 converts residue from Mg/ha to kg/m2 */
    waterWettingResidue = Soil->infiltrationVol * Residue->residueInterception;

#ifdef _DEBUG_
    printf ("ResidueWetting:\n");
    printf ("flatResidueWaterDeficit = %lf, standingResidueWaterDeficit = %lf, waterWettingResidue = %lf\n", flatResidueWaterDeficit, standingResidueWaterDeficit, waterWettingResidue);
#endif

    waterRetainedResidue = 0.;

    /* wet flat residue first */
    if (waterWettingResidue > flatResidueWaterDeficit)
    {
        Residue->flatResidueWater += flatResidueWaterDeficit;
        waterRetainedResidue += flatResidueWaterDeficit;
        waterWettingResidue -= flatResidueWaterDeficit;
    }
    else
    {
        Residue->flatResidueWater += waterWettingResidue;
        waterRetainedResidue += waterWettingResidue;
        waterWettingResidue -= waterWettingResidue;
    }

#ifdef _DEBUG_
    printf ("flatResidueWater = %lf, waterRetainedResidue = %lf, waterWettingResidue = %lf\n", Residue->flatResidueWater, waterRetainedResidue, waterWettingResidue);
#endif

    if (waterWettingResidue > standingResidueWaterDeficit)
    {
        Residue->stanResidueWater += standingResidueWaterDeficit;
        waterRetainedResidue += standingResidueWaterDeficit;
        waterWettingResidue -= standingResidueWaterDeficit;
    }
    else
    {
        Residue->stanResidueWater += waterWettingResidue;
        waterRetainedResidue += waterWettingResidue;
        waterWettingResidue -= waterWettingResidue;
    }

#ifdef _DEBUG_
    printf ("stanResidueWater = %lf, waterRetainedResidue = %lf, waterWettingResidue = %lf\n", Residue->stanResidueWater, waterRetainedResidue, waterWettingResidue);
#endif

   Soil->infiltrationVol -= waterRetainedResidue;

#ifdef _DEBUG_
    printf ("infiltrationVol = %lf\n", Soil->infiltrationVol);
#endif
}

void ResidueEvaporation (ResidueStruct * Residue, SoilStruct * Soil, CropStruct * Crop, double ETo, double snowCover)
{
    const double    residueMaxWaterConcentration = 3.3; /* kg water / kg residue */
    double          flatEvapFactor;
    double          standingEvapFactor;
    double          flatEvap;
    double          standingEvap;
    double          residueEvapDemand;
    double          xx;

    Soil->residueEvaporationVol = 0.;
    if (Residue->stanResidueWater > 0. || Residue->flatResidueWater > 0.)
    {
        Soil->residueEvaporationVol = 0.;
        residueEvapDemand = Residue->residueInterception * (1. - snowCover) * (1. - Crop->svRadiationInterception) * ETo;
        standingEvapFactor = pow (Residue->stanResidueWater / (residueMaxWaterConcentration * Residue->stanResidueMass / 10.), 2);  /* 10 converts residue from Mg/ha to kg/m2 */
        flatEvapFactor = pow (Residue->flatResidueWater / (residueMaxWaterConcentration * Residue->flatResidueMass / 10), 2);   /* 10 converts residue from Mg/ha to kg/m2 */

        /* dry standing residue first */
        xx = residueEvapDemand * standingEvapFactor;
        if (Residue->stanResidueWater >= xx)
            standingEvap = xx;
        else
            standingEvap = Residue->stanResidueWater;

        residueEvapDemand -= standingEvap;

        xx = residueEvapDemand * flatEvapFactor;
        if (Residue->flatResidueWater >= xx)
            flatEvap = xx;
        else
            flatEvap = Residue->flatResidueWater;

        Soil->residueEvaporationVol = standingEvap + flatEvap;
        Residue->stanResidueWater -= standingEvap;
        Residue->flatResidueWater -= flatEvap;
    }
}
