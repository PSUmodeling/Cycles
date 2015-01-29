#include "include/Cycles.h"

void ExecuteTillage (int y, double *abgdBiomassInput, FieldOperationStruct * Tillage, double *tillageFactor, SoilStruct * Soil, ResidueStruct * Residue)
{
    /*
     * Tillage performed on the soil profile as described by the tillage
     * parameters
     */
    const int       mixVariables = 17;
    int             i, j;
    int             lastLayer = Soil->totalLayers;
    double          conc[Soil->totalLayers][mixVariables];
    double          mixed[mixVariables];
    double          toolDepth;
    double          soilMass[Soil->totalLayers];
    double          soilMassNotMixed[Soil->totalLayers];
    double          soilMassMixed[Soil->totalLayers];
    double          totalSoilMassMixed;
    double          partialSoilMassMixed;   /* Mg/m3 */
    //double          SOCConc[Soil->totalLayers];
    //double          SONConc[Soil->totalLayers];
    //double          NO3Conc[Soil->totalLayers];
    //double          NH4Conc[Soil->totalLayers];
    double          incorporatedResidueBiomass;
    double          incorporatedResidueN;
    double          incorporatedManureC;
    double          incorporatedManureN;
    double          mixedFraction;  /* fraction of soil layer mixed */
    double          flattenedFraction;  /* fraction of the standing residues that are flattened */

    /* flatten standing residues */
    flattenedFraction = pow (Tillage->opMixingEfficiency, 0.5);
    Residue->flatResidueMass += flattenedFraction * Residue->stanResidueMass;
    Residue->stanResidueMass *= (1.0 - flattenedFraction);
    Residue->flatResidueN += flattenedFraction * Residue->stanResidueN;
    Residue->stanResidueN *= (1.0 - flattenedFraction);
    Residue->flatResidueWater += flattenedFraction * Residue->stanResidueWater;
    Residue->stanResidueWater *= (1.0 - flattenedFraction);

    /* set tillage depth */
    if (Tillage->opMixingEfficiency == 0.0 || Tillage->opDepth == 0.0)
        return;
    else if (Soil->cumulativeDepth[Soil->totalLayers - 1] >= Tillage->opDepth)
        toolDepth = Tillage->opDepth;
    else
        toolDepth = Soil->cumulativeDepth[Soil->totalLayers - 1];

    /* add water mixing, microbial biomass */
    /* start soil mixing */
    for (i = 0; i < Soil->totalLayers; i++)
    {
        if (i > 0)
        {
            if (Soil->cumulativeDepth[i - 1] >= toolDepth)
                break;
        }

        soilMass[i] = Soil->BD[i] * Soil->layerThickness[i] * 10000.0;   /* mass in Mg C ha-1 */
        conc[i][0] = Soil->Clay[i];
        conc[i][1] = Soil->SOC_Conc[i];
        conc[i][2] = Soil->SON_Mass[i] / soilMass[i];
        conc[i][3] = Residue->residueAbgd[i] / soilMass[i];
        conc[i][4] = Residue->residueRt[i] / soilMass[i];
        conc[i][5] = Residue->residueRz[i] / soilMass[i];
        conc[i][6] = Residue->residueAbgdN[i] / soilMass[i];
        conc[i][7] = Residue->residueRtN[i] / soilMass[i];
        conc[i][8] = Residue->residueRzN[i] / soilMass[i];
        conc[i][9] = Residue->manureC[i] / soilMass[i];
        conc[i][10] = Residue->manureN[i] / soilMass[i];
        conc[i][11] = Soil->NO3[i] / soilMass[i];
        conc[i][12] = Soil->NH4[i] / soilMass[i];
        conc[i][13] = Soil->MBC_Mass[i] / soilMass[i];
        conc[i][14] = Soil->MBN_Mass[i] / soilMass[i];
        conc[i][15] = Soil->waterContent[i] * Soil->layerThickness[i] / soilMass[i];
        conc[i][16] = Soil->Sand[i];
        lastLayer = i + 1;
    }

    totalSoilMassMixed = 0.0;
    for (i = 0; i < lastLayer; i++)
    {
        if (toolDepth >= Soil->cumulativeDepth[i])
            soilMassMixed[i] = Tillage->opMixingEfficiency * soilMass[i];

        else
            soilMassMixed[i] = Tillage->opMixingEfficiency * soilMass[i] * (toolDepth - Soil->cumulativeDepth[i - 1]) / Soil->layerThickness[i];
        soilMassNotMixed[i] = soilMass[i] - soilMassMixed[i];

        partialSoilMassMixed = totalSoilMassMixed + soilMassMixed[i];
        for (j = 0; j < mixVariables; j++)
        {
            mixed[j] = Fraction (totalSoilMassMixed, mixed[j], soilMassMixed[i], conc[i][j], partialSoilMassMixed);
        }
        totalSoilMassMixed = partialSoilMassMixed;
    }

    /* Redistribute mixed material re-computing state variables while preserving the layer bulk density */
    for (i = 0; i < lastLayer; i++)

    {
        Soil->Clay[i] = Fraction (conc[i][0], soilMassNotMixed[i], mixed[0], soilMassMixed[i], soilMass[i]);
        Soil->SOC_Conc[i] = Fraction (conc[i][1], soilMassNotMixed[i], mixed[1], soilMassMixed[i], soilMass[i]);
        Soil->SON_Mass[i] = soilMass[i] * Fraction (conc[i][2], soilMassNotMixed[i], mixed[2], soilMassMixed[i], soilMass[i]);
        Residue->residueAbgd[i] = soilMass[i] * Fraction (conc[i][3], soilMassNotMixed[i], mixed[3], soilMassMixed[i], soilMass[i]);
        Residue->residueRt[i] = soilMass[i] * Fraction (conc[i][4], soilMassNotMixed[i], mixed[4], soilMassMixed[i], soilMass[i]);
        Residue->residueRz[i] = soilMass[i] * Fraction (conc[i][5], soilMassNotMixed[i], mixed[5], soilMassMixed[i], soilMass[i]);
        Residue->residueAbgdN[i] = soilMass[i] * Fraction (conc[i][6], soilMassNotMixed[i], mixed[6], soilMassMixed[i], soilMass[i]);
        Residue->residueRtN[i] = soilMass[i] * Fraction (conc[i][7], soilMassNotMixed[i], mixed[7], soilMassMixed[i], soilMass[i]);
        Residue->residueRzN[i] = soilMass[i] * Fraction (conc[i][8], soilMassNotMixed[i], mixed[8], soilMassMixed[i], soilMass[i]);
        Residue->manureC[i] = soilMass[i] * Fraction (conc[i][9], soilMassNotMixed[i], mixed[9], soilMassMixed[i], soilMass[i]);
        Residue->manureN[i] = soilMass[i] * Fraction (conc[i][10], soilMassNotMixed[i], mixed[10], soilMassMixed[i], soilMass[i]);
        Soil->NO3[i] = soilMass[i] * Fraction (conc[i][11], soilMassNotMixed[i], mixed[11], soilMassMixed[i], soilMass[i]);
        Soil->NH4[i] = soilMass[i] * Fraction (conc[i][12], soilMassNotMixed[i], mixed[12], soilMassMixed[i], soilMass[i]);
        Soil->MBC_Mass[i] = soilMass[i] * Fraction (conc[i][13], soilMassNotMixed[i], mixed[13], soilMassMixed[i], soilMass[i]);
        Soil->MBN_Mass[i] = soilMass[i] * Fraction (conc[i][14], soilMassNotMixed[i], mixed[14], soilMassMixed[i], soilMass[i]);
        Soil->waterContent[i] = soilMass[i] / Soil->layerThickness[i] * Fraction (conc[i][15], soilMassNotMixed[i], mixed[15], soilMassMixed[i], soilMass[i]);
        Soil->Sand[i] = Fraction (conc[i][16], soilMassNotMixed[i], mixed[16], soilMassMixed[i], soilMass[i]);
        Soil->SOC_Mass[i] = Soil->SOC_Conc[i] / 1000.0 * soilMass[i];    /* mass in Mg/ha */
    }

    /* Remove residue from the surface and incorporate within each layer */
    incorporatedResidueBiomass = (Residue->stanResidueMass + Residue->flatResidueMass) * Tillage->opMixingEfficiency;
    incorporatedResidueN = (Residue->stanResidueN + Residue->flatResidueN) * Tillage->opMixingEfficiency;
    incorporatedManureC = Residue->manureSurfaceC * Tillage->opMixingEfficiency;
    incorporatedManureN = Residue->manureSurfaceN * Tillage->opMixingEfficiency;

    /* Distribute among soil layers */
    for (i = 0; i < lastLayer; i++)

    {
        if (toolDepth >= Soil->cumulativeDepth[i])
            mixedFraction = Soil->layerThickness[i] / toolDepth;
        else
        {
            if (i > 0)
                mixedFraction = (toolDepth - Soil->cumulativeDepth[i - 1]) / toolDepth;
            else
                mixedFraction = toolDepth / toolDepth;
        }
        Residue->residueAbgd[i] += incorporatedResidueBiomass * mixedFraction;
        Residue->residueAbgdN[i] += incorporatedResidueN * mixedFraction;
        Residue->manureC[i] += incorporatedManureC * mixedFraction;
        Residue->manureN[i] += incorporatedManureN * mixedFraction;
        abgdBiomassInput[i] += incorporatedResidueBiomass * mixedFraction;
    }

    /* Update surface pools */
    Residue->flatResidueMass *= (1.0 - Tillage->opMixingEfficiency);
    Residue->stanResidueMass *= (1.0 - Tillage->opMixingEfficiency);
    Residue->flatResidueN *= (1.0 - Tillage->opMixingEfficiency);
    Residue->stanResidueN *= (1.0 - Tillage->opMixingEfficiency);
    Residue->manureSurfaceC *= (1.0 - Tillage->opMixingEfficiency);
    Residue->manureSurfaceN *= (1.0 - Tillage->opMixingEfficiency);
    Residue->flatResidueWater *= (1.0 - Tillage->opMixingEfficiency);
    Residue->stanResidueWater *= (1.0 - Tillage->opMixingEfficiency);
    ComputeTillageFactor (Tillage, tillageFactor, Soil, Soil->cumulativeDepth, toolDepth);
}

void TillageFactorSettling (double *tillageFactor, int totalLayers, const double *waterContent, const double *Porosity)
{
    int             i;

    for (i = 0; i < totalLayers; i++)
    {
        tillageFactor[i] *= (1.0 - 0.02 * waterContent[i] / Porosity[i]);
    }
}

double Fraction (double a, double b, double c, double d, double f)
{
    return ((a * b + c * d) / f);
}

void ComputeTillageFactor (const FieldOperationStruct * Tillage, double *tillageFactor, SoilStruct * Soil, double *soilLayerBottom, double toolDepth)
{
    /*
     * Multiplier for soil decomposition rate based on tillage intensity and
     * soil type is returned
     * SDR is the sum for the year of NRCS' Soil Disturbance Rating
     */
    int             i;
    const double    k1 = 5.5;   /* empirical constant */
    const double    k2 = 0.05;  /* empirical constant */
    double          SDR, SDR1, SDR2;
    double          FX, DFX;
    double          textureFactor;
    double          TF1;
    double          XX;

    for (i = 0; i < Soil->totalLayers; i++)
    {
        textureFactor = ComputeTextureFactor (Soil->Clay[i]);
        TF1 = tillageFactor[i] / textureFactor;

        if (TF1 > 0.01)
        {
            SDR1 = 35.0;         /* iteration starting value */
            SDR2 = 0.0;

            while (fabs ((SDR1 - SDR2) / SDR1) > 0.05)
            {
                SDR2 = SDR1;
                XX = exp (k1 - k2 * SDR1);
                FX = SDR1 / (SDR1 + XX) - TF1;
                DFX = (1.0 + k2 * SDR1) * XX / pow (SDR1 + XX, 2.0);
                SDR1 = SDR1 - FX / DFX;
            }

            SDR = SDR1;
        }
        else
            SDR = 1.0;

        if (soilLayerBottom[i] <= toolDepth)
            SDR += Tillage->opSDR;
        else if (soilLayerBottom[i] > toolDepth && soilLayerBottom[i - 1] < toolDepth)
            SDR += Tillage->opSDR * (toolDepth - soilLayerBottom[i - 1]) / Soil->layerThickness[i];

        tillageFactor[i] = textureFactor * SDR / (SDR + exp (k1 - k2 * SDR));
    }
}

double ComputeTextureFactor (double Clay)
{

    /* clay is expressed fractionally */
    const double    aClay = 1.0;  /* the maximum (lots of tillage) multiplier in a 100% clay soil is (A_clay + 1) */
    const double    aSand = 5.0;  /* the maximum multiplier in 100% sand soil is (A_sand + 1) */
    const double    kClay = 5.5;    /* a curvature factor */
    return (aClay + (aSand - aClay) * exp (-kClay * Clay)); /* clay dependent term */
}
