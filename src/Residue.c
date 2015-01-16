#include "include/Cycles.h"

void InitializeResidue (ResidueStruct *Residue, int totalYears, int totalLayers)
{
    Residue->residueAbgd = (double *) malloc (totalLayers * sizeof (double));
    Residue->residueRt = (double *) malloc (totalLayers * sizeof (double));
    Residue->residueRz = (double *) malloc (totalLayers * sizeof (double));
    Residue->residueAbgdN = (double *) malloc (totalLayers * sizeof (double));
    Residue->residueRtN = (double *) malloc (totalLayers * sizeof (double));
    Residue->residueRzN = (double *) malloc (totalLayers * sizeof (double));
    Residue->manureC = (double *) malloc (totalLayers * sizeof (double));
    Residue->manureN = (double *) malloc (totalLayers * sizeof (double));

    Residue->residueInterception = 0.;
    Residue->stanResidueTau = 1.;
    Residue->flatResidueTau = 1.;
    Residue->stanResidueMass = 0.;
    Residue->flatResidueMass = 0.;
    Residue->stanResidueN = 0.;
    Residue->flatResidueN = 0.;
    Residue->stanResidueWater = 0.;
    Residue->flatResidueWater = 0.;
}

void ComputeResidueCover (ResidueStruct *Residue)
{
    double stanResidueAI;       /* standing residue area index
                                 * (m2 residue / m2 ground) */
    double flatResidueAI;       /* flat residue area index
                                 * (m2 residue / m2 ground) */

    /* factor 0.1 converts from Mg/ha to kg/m2 */
    stanResidueAI = STAN_RESIDUE_SA * Residue->stanResidueMass * 0.1;
    flatResidueAI = FLAT_RESIDUE_SA * Residue->flatResidueMass * 0.1;
    Residue->stanResidueTau = exp(-STAN_RESIDUE_K * stanResidueAI);
    Residue->flatResidueTau = exp(-FLAT_RESIDUE_K * flatResidueAI);
    Residue->residueInterception = (1. - Residue->stanResidueTau) + Residue->stanResidueTau * (1. - Residue->flatResidueTau);
}

//void ResidueWetting (ResidueStruct *Residue, SoilStruct *Soil)
//{
//    const double residueMaxWaterConcentration = 3.3;    /* (kg / kg) */
//    double flatResidueWaterDeficit;     /* Water need to saturate residue
//                                         * (mm) */
//    doublestandingResidueWaterDeficit As Double               'mm, water need to saturate residue
//        Dim waterWettingResidue As Double                       'mm, amount of water interceptable by residue
//        Dim waterRetainedResidue As Double                      'mm, water retained in residue and discounted for infiltration
//
//
