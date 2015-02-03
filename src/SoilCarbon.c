#include "include/Cycles.h"

void InitializeSoilCarbon (SoilCarbonStruct *SoilCarbon, int totalLayers)
{
    SoilCarbon->factorComposite = (double *)calloc (totalLayers, sizeof (double));
    SoilCarbon->carbonRespired = (double *)calloc (totalLayers, sizeof (double));
    SoilCarbon->rootBiomassInput = (double *)calloc (totalLayers, sizeof (double));
    SoilCarbon->rhizBiomassInput = (double *)calloc (totalLayers, sizeof (double));
    SoilCarbon->abgdBiomassInput = (double *)calloc (totalLayers, sizeof (double));
    SoilCarbon->rootCarbonInput = (double *)calloc (totalLayers, sizeof (double));
    SoilCarbon->rhizCarbonInput = (double *)calloc (totalLayers, sizeof (double));
    SoilCarbon->manuCarbonInput = (double *)calloc (totalLayers, sizeof (double));
    SoilCarbon->abgdCarbonInput = (double *)calloc (totalLayers, sizeof (double));
    SoilCarbon->carbonMassInitial = (double *)calloc (totalLayers, sizeof (double));
    SoilCarbon->carbonMassFinal = (double *)calloc (totalLayers, sizeof (double));
    SoilCarbon->annualDecompositionFactor = (double *)calloc (totalLayers, sizeof (double));
    SoilCarbon->annualSoilCarbonDecompositionRate = (double *)calloc (totalLayers, sizeof (double));
    SoilCarbon->annualCarbonInputByLayer = (double *)calloc (totalLayers, sizeof (double));
    SoilCarbon->annualHumifiedCarbonMass = (double *)calloc (totalLayers, sizeof (double));
    SoilCarbon->annualRespiredCarbonMass = (double *)calloc (totalLayers, sizeof (double));
    SoilCarbon->annualRespiredResidueCarbonMass = (double *)calloc (totalLayers, sizeof (double));
    SoilCarbon->annualHumificationCoefficient = (double *)calloc (totalLayers, sizeof (double));
}

void ComputeFactorComposite (SoilCarbonStruct *SoilCarbon, int doy, int y, SoilStruct *Soil)
{
    static double   avg[50];
    double          waterPotential; /* J/kg */
    double          airContent; /* m3/m3 */
    double          factorMoisture; /* unitless */
    double          factorTemperature;  /* unitless */
    double          factorAeration = 1.0;   /* cumulative aeration factor accounts
                                             * empirically for air content of layers above
                                             * that considered in the calculations
                                             */
    int             i;

    if (doy == 1)
    {
        for (i = 0; i < Soil->totalLayers; i++)
            avg[i] = 0.0;
    }

    for (i = 0; i < Soil->totalLayers; i++)
    {
        waterPotential = SoilWaterPotential (Soil->Porosity[i], Soil->airEntryPotential[i], Soil->B_Value[i], Soil->waterContent[i]);
        airContent = Soil->Porosity[i] - Soil->waterContent[i];
        factorMoisture = Moisture (waterPotential);
        factorTemperature = TemperatureFunction (Soil->soilTemperature[i]);
        factorAeration *= Aeration (airContent);
        SoilCarbon->factorComposite[i] = factorMoisture * factorAeration * factorTemperature;
        avg[i] += SoilCarbon->factorComposite[i];
        if (doy == 365)
            SoilCarbon->annualDecompositionFactor[i] = avg[i] / 365.0;
    }
}

void ComputeSoilCarbonBalance (SoilCarbonStruct *SoilCarbon, int y, ResidueStruct *Residue, SoilStruct *Soil, double *tillageFactor)
{
    int             i;
    double          socDecompositionRate;
    double          micrDecompositionRate;
    double          humifiedCarbon;
    double          humifiedNitrogen;
    double          abgdHumificationFactor;
    double          rootHumificationFactor;
    double          rhizHumificationFactor;
    double          manuHumificationFactor;
    double          micrHumificationFactor;
    double          soilMass;
    double          satSOCConc; /* g C kg soil */
    double          humificationAdjustmentBySOC;
    double          decompositionAdjustmentBySOC;
    double          contactFractionFlat;    /* fraction of surface residues
                                             * subject to decomposition */
    double          contactFractionStan;    /* fraction of surface residues
                                             * subject to decomposition */

    /* residue mass decomposition (xx1-5), manure carbon decomposition (xx6-7)
     * and organic matter and microbial pool decomposition (xx8 and xx9)
     * (Mg/ha/day) */
    double          xx0, xx1, xx2, xx3, xx4, xx5, xx6, xx7, xx8, xx9;

    /* residue nitrogen net mineralization */
    double          nm0, nm1, nm2, nm3, nm4, nm5, nm6, nm7, nm8, nm9;

    /* residue nitrogen removed by decomposition from each residue pool */
    double          nr0, nr1, nr2, nr3, nr4, nr5, nr6, nr7, nr8, nr9;

    /* residue nitrogen transfered to microbial biomass (nh1-8) or from
     * microbial biomass to SOM */
    double          nh0, nh1, nh2, nh3, nh4, nh5, nh6, nh7, nh8, nh9;
    double          aux1, aux2, aux3;
    double          xxPartialSum;
    double          NMineralization;
    double          NImmobilization;
    double          NNetMineralization;
    double          stanCNRatio;    /* CN ratio of standing residues */
    double          flatCNRatio;    /* CN ratio of flat residues */
    double          abgdCNRatio;    /* CN ratio of aboveground residues (in each soil
                                     * layer) */
    double          rootCNRatio;    /* CN ratio of root residues */
    double          rhizCNRatio;    /* CN ratio of rizhodeposition */
    double          smanCNRatio;    /* CN ratio of surface manure */
    double          manuCNRatio;    /* CN ratio of manure */
    double          micrCNRatio;    /* CN ratio of microbial biomass */
    double          somCNratio; /* sman = surface manure, manu = manure */
    double          CNnew;      /* CN of destiny pool (microbial biomass)
                                 * calculated before each microbial attack; for
                                 * microbial biomass it equal CN microbial biomass */
    double          NMineralConcentration;  /* g N-NO3 g soil */
    double          NMineral;   /* sum of NO3 and NH4 (be careful with units) */
    double          NH4_Fraction;   /* fraction of NH4 in the sum NO3 + NH4 */
    double          decompReductionFactor;
    double          NInitial, NFinal;

    NInitial = 0.0;
    NFinal = 0.0;
    NInitial = Residue->stanResidueN + Residue->flatResidueN + Residue->manureSurfaceN;
    contactFractionFlat = 0.0;
    contactFractionStan = 0.0;
    Soil->N_Immobilization = 0.0;
    Soil->N_Mineralization = 0.0;
    Soil->N_NetMineralization = 0.0;
    Soil->SOCProfile = 0.0;
    Soil->SONProfile = 0.0;
    Soil->C_Humified = 0.0;
    Soil->C_ResidueRespired = 0.0;
    Soil->C_SoilRespired = 0.0;

    for (i = 0; i < Soil->totalLayers; i++)
    {
        NInitial += Soil->SON_Mass[i] + Soil->MBN_Mass[i] + Soil->NO3[i] + Soil->NH4[i] + Residue->residueAbgdN[i] + Residue->residueRtN[i] + Residue->residueRzN[i] + Residue->manureN[i];

        socDecompositionRate = 0.0;
        micrDecompositionRate = 0.0;
        humifiedCarbon = 0.0;
        humifiedNitrogen = 0.0;
        NMineralization = 0.0;
        NImmobilization = 0.0;
        NNetMineralization = 0.0;
        CNnew = 0.0;
        stanCNRatio = 0.0;
        flatCNRatio = 0.0;
        abgdCNRatio = 0.0;
        rootCNRatio = 0.0;
        rhizCNRatio = 0.0;
        smanCNRatio = 0.0;
        manuCNRatio = 0.0;
        micrCNRatio = 0.0;

        xx0 = xx1 = xx2 = xx3 = xx4 = xx5 = xx6 = xx7 = xx8 = xx9 = 0.0;
        nm0 = nm1 = nm2 = nm3 = nm4 = nm5 = nm6 = nm7 = nm8 = nm9 = 0.0;
        nr0 = nr1 = nr2 = nr3 = nr4 = nr5 = nr6 = nr7 = nr8 = nr9 = 0.0;
        nh0 = nh1 = nh2 = nh3 = nh4 = nh5 = nh6 = nh7 = nh8 = nh9 = 0.0;

        /* compute auxiliar variables */
        soilMass = 10000.0 * Soil->BD[i] * Soil->layerThickness[i]; /* 10000
                                                                     * converts from Mg/m2 to Mg/ha */
        NMineral = Soil->NO3[i] + Soil->NH4[i];
        NH4_Fraction = Soil->NH4[i] / NMineral;
        NMineralConcentration = Soil->NO3[i] / soilMass;
        satSOCConc = 21.1 + 0.375 * Soil->Clay[i] * 100.0;

        /* compute C/N ratios */
        if (i == 0)
        {
            if (Residue->stanResidueMass > 0.0)
                stanCNRatio = Residue->stanResidueMass * FRACTION_CARBON_PLANT / Residue->stanResidueN;
            if (Residue->flatResidueMass > 0.0)
                flatCNRatio = Residue->flatResidueMass * FRACTION_CARBON_PLANT / Residue->flatResidueN;
            if (Residue->manureSurfaceC > 0.0)
                smanCNRatio = Residue->manureSurfaceC / Residue->manureSurfaceN;
        }

        somCNratio = Soil->SOC_Mass[i] / Soil->SON_Mass[i];
        micrCNRatio = Soil->MBC_Mass[i] / Soil->MBN_Mass[i];

        if (Residue->residueAbgd[i] > 0.0)
            abgdCNRatio = Residue->residueAbgd[i] * FRACTION_CARBON_PLANT / Residue->residueAbgdN[i];
        if (Residue->residueRt[i] > 0.0)
            rootCNRatio = Residue->residueRt[i] * FRACTION_CARBON_PLANT / Residue->residueRtN[i];
        if (Residue->residueRz[i] > 0.0)
            rhizCNRatio = Residue->residueRz[i] * FRACTION_CARBON_RIZHO / Residue->residueRzN[i];
        if (Residue->manureC[i] > 0.0)
            manuCNRatio = Residue->manureC[i] / Residue->manureN[i];

        /* humification */
        /* humification reduction when C conc approaches saturation */
        humificationAdjustmentBySOC = 1.0 - pow (Soil->SOC_Conc[i] / satSOCConc, SOC_HUMIFICATION_POWER);
        humificationAdjustmentBySOC = humificationAdjustmentBySOC > 0.0 ? humificationAdjustmentBySOC : 0.0;

        abgdHumificationFactor = sqrt (MaximumAbgdHumificationFactor (Soil->Clay[i]) * humificationAdjustmentBySOC);
        rootHumificationFactor = sqrt (MaximumRootHumificationFactor (Soil->Clay[i]) * humificationAdjustmentBySOC);
        rhizHumificationFactor = sqrt (MaximumRhizHumificationFactor (Soil->Clay[i]) * humificationAdjustmentBySOC);
        manuHumificationFactor = sqrt (MaximumManuHumificationFactor (Soil->Clay[i]) * humificationAdjustmentBySOC);

        /* temporarily assigned abgd humification, then checked if it can be
         * higher if manure is decomposing, but never lower */
        micrHumificationFactor = abgdHumificationFactor;

        /* RESIDUE AND MANURE DECOMPOSITION */
        if (i == 0)
        {
            contactFractionStan = pow (Residue->stanResidueTau, exp (-1.5 / sqrt (1.0 - Residue->stanResidueTau)));
            contactFractionFlat = pow (Residue->flatResidueTau, exp (-1.5 / sqrt (1.0 - Residue->flatResidueTau)));

            xx2 = SoilCarbon->factorComposite[i] * MAXIMUM_RESIDUE_DECOMPOSITION_RATE * contactFractionStan * Residue->stanResidueMass;
            xx3 = SoilCarbon->factorComposite[i] * MAXIMUM_RESIDUE_DECOMPOSITION_RATE * contactFractionFlat * Residue->flatResidueMass;
            xx6 = SoilCarbon->factorComposite[i] * MAXIMUM_MANURE_DECOMPOSITION_RATE * Residue->manureSurfaceC;
        }

        xx1 = SoilCarbon->factorComposite[i] * MAXIMUM_RESIDUE_DECOMPOSITION_RATE * Residue->residueAbgd[i];
        xx4 = SoilCarbon->factorComposite[i] * MAXIMUM_ROOT_DECOMPOSITION_RATE * Residue->residueRt[i];
        xx5 = SoilCarbon->factorComposite[i] * MAXIMUM_RHIZO_DECOMPOSITION_RATE * Residue->residueRz[i];
        xx7 = SoilCarbon->factorComposite[i] * MAXIMUM_MANURE_DECOMPOSITION_RATE * Residue->manureC[i];

        /* inorganic N limitation for decomposition */
        /* if decomposition > 0 then compute net N mineralization and
         * accumulate negatives */
        if (i == 0)
        {
            if (xx2 > 0.0)
            {
                CNnew = CNdestiny (NMineralConcentration, stanCNRatio);
                nm2 = NitrogenMineralization (stanCNRatio, CNnew, abgdHumificationFactor, xx2 * FRACTION_CARBON_PLANT);
            }
            if (xx3 > 0.0)
            {
                CNnew = CNdestiny (NMineralConcentration, flatCNRatio);
                nm3 = NitrogenMineralization (flatCNRatio, CNnew, abgdHumificationFactor, xx3 * FRACTION_CARBON_PLANT);
            }
            if (xx6 > 0.0)
            {
                CNnew = CNdestiny (NMineralConcentration, smanCNRatio);
                nm6 = NitrogenMineralization (smanCNRatio, CNnew, manuHumificationFactor, xx6);
            }

            if (nm2 < 0.0)
                nm0 += nm2;
            if (nm3 < 0.0)
                nm0 += nm3;
            if (nm6 < 0.0)
                nm0 += nm6;
        }

        if (xx1 > 0.0)
        {
            CNnew = CNdestiny (NMineralConcentration, abgdCNRatio);
            nm1 = NitrogenMineralization (abgdCNRatio, CNnew, abgdHumificationFactor, xx1 * FRACTION_CARBON_PLANT);
        }
        if (xx4 > 0.0)
        {
            CNnew = CNdestiny (NMineralConcentration, rootCNRatio);
            nm4 = NitrogenMineralization (rootCNRatio, CNnew, rootHumificationFactor, xx4 * FRACTION_CARBON_PLANT);
        }
        if (xx5 > 0.0)
        {
            CNnew = CNdestiny (NMineralConcentration, rhizCNRatio);
            nm5 = NitrogenMineralization (rhizCNRatio, CNnew, rhizHumificationFactor, xx5 * FRACTION_CARBON_RIZHO);
        }
        if (xx7 > 0.0)
        {
            CNnew = CNdestiny (NMineralConcentration, manuCNRatio);
            nm7 = NitrogenMineralization (manuCNRatio, CNnew, manuHumificationFactor, xx7);
        }

        if (nm1 < 0.0)
            nm0 += nm1;
        if (nm4 < 0.0)
            nm0 += nm4;
        if (nm5 < 0.0)
            nm0 += nm5;
        if (nm7 < 0.0)
            nm0 += nm7;

        if (-nm0 > NMineral)
            decompReductionFactor = NMineral / (-nm0);
        else
            decompReductionFactor = 1.0;

        if (decompReductionFactor < 1.0)
        {
            /* adjust actual decomposition as a function on mineral N
             * availability */
            if (nm1 < 0.0)
                xx1 *= decompReductionFactor;
            if (nm2 < 0.0)
                xx2 *= decompReductionFactor;
            if (nm3 < 0.0)
                xx3 *= decompReductionFactor;
            if (nm4 < 0.0)
                xx4 *= decompReductionFactor;
            if (nm5 < 0.0)
                xx5 *= decompReductionFactor;
            if (nm6 < 0.0)
                xx6 *= decompReductionFactor;
            if (nm7 < 0.0)
                xx7 *= decompReductionFactor;

            /* recalculate net mineralization only for pools with adjusted
             * decomposition */
            if (nm1 < 0.0)
            {
                CNnew = CNdestiny (NMineralConcentration, abgdCNRatio);
                nm1 = NitrogenMineralization (abgdCNRatio, CNnew, abgdHumificationFactor, xx1 * FRACTION_CARBON_PLANT);
            }
            if (nm2 < 0.0)
            {
                CNnew = CNdestiny (NMineralConcentration, stanCNRatio);
                nm2 = NitrogenMineralization (stanCNRatio, CNnew, abgdHumificationFactor, xx2 * FRACTION_CARBON_PLANT);
            }
            if (nm3 < 0.0)
            {
                CNnew = CNdestiny (NMineralConcentration, flatCNRatio);
                nm3 = NitrogenMineralization (flatCNRatio, CNnew, abgdHumificationFactor, xx3 * FRACTION_CARBON_PLANT);
            }
            if (nm4 < 0.0)
            {
                CNnew = CNdestiny (NMineralConcentration, rootCNRatio);
                nm4 = NitrogenMineralization (rootCNRatio, CNnew, rootHumificationFactor, xx4 * FRACTION_CARBON_PLANT);
            }
            if (nm5 < 0.0)
            {
                CNnew = CNdestiny (NMineralConcentration, rhizCNRatio);
                nm5 = NitrogenMineralization (rhizCNRatio, CNnew, rhizHumificationFactor, xx5 * FRACTION_CARBON_RIZHO);
            }
            if (nm6 < 0.0)
            {
                CNnew = CNdestiny (NMineralConcentration, smanCNRatio);
                nm6 = NitrogenMineralization (smanCNRatio, CNnew, manuHumificationFactor, xx6);
            }
            if (nm7 < 0.0)
            {
                CNnew = CNdestiny (NMineralConcentration, manuCNRatio);
                nm7 = NitrogenMineralization (manuCNRatio, CNnew, manuHumificationFactor, xx7);
            }
        }

        /* calculate weighted C retention efficiency from microbial biomass to
         * soil organic matter */
        xxPartialSum = xx1 + xx2 + xx3 + xx4 + xx5 + xx6 + xx7;
        if (xxPartialSum > 0.0)
            micrHumificationFactor = micrHumificationFactor > (1.0 / xxPartialSum) * (abgdHumificationFactor * (xx1 + xx2 + xx3) + rootHumificationFactor * xx4 + rhizHumificationFactor * xx5 + manuHumificationFactor * (xx6 + xx7)) ? micrHumificationFactor : (1.0 / xxPartialSum) * (abgdHumificationFactor * (xx1 + xx2 + xx3) + rootHumificationFactor * xx4 + rhizHumificationFactor * xx5 + manuHumificationFactor * (xx6 + xx7));

        /* SOC decomposition and SON mineralization */
        decompositionAdjustmentBySOC = 1.0 - 1.0 / (1.0 + pow ((Soil->SOC_Conc[i] / satSOCConc) / 0.22, 3.0));
        decompositionAdjustmentBySOC = decompositionAdjustmentBySOC < 1.0 ? decompositionAdjustmentBySOC : 1.0;
        socDecompositionRate = SoilCarbon->factorComposite[i] * (1.0 + tillageFactor[i]) * MAXIMUM_UNDISTURBED_SOC_DECOMPOSITION_RATE * decompositionAdjustmentBySOC / (1.0 - pow (micrHumificationFactor, 2.0));

        xx8 = socDecompositionRate * Soil->SOC_Mass[i];
        if (xx8 > 0.0)
        {
            CNnew = CNdestiny (NMineralConcentration, somCNratio);
            nm8 = NitrogenMineralization (somCNratio, CNnew, micrHumificationFactor, xx8);
        }

        /* microbial biomass decomposition and N mineralization */
        aux1 = (Soil->MBC_Mass[i] / Soil->SOC_Mass[i]) / 0.03;
        aux2 = exp (10.0 * (aux1 - 1.0));   /* acceleration of microbial
                                             * turnover if > 3% of SOC */

        /* Charlie's steady state km so that Cm = 0.03 of organic carbon
         * km = 0.97ks / 0.03(ex(1-Cs/Cx))1/2.
         * Also notice ks acceleration from apparent to actual turnover
         * ks/(1-e^2) */
        aux3 = aux2 * MAXIMUM_UNDISTURBED_SOC_DECOMPOSITION_RATE * decompositionAdjustmentBySOC / (1.0 - pow (micrHumificationFactor, 2.0)) * (0.97 / 0.03) * (1.0 / micrHumificationFactor);
        micrDecompositionRate = SoilCarbon->factorComposite[i] * (1.0 + tillageFactor[i]) * aux3;
        xx9 = micrDecompositionRate * Soil->MBC_Mass[i];
        if (xx9 > 0.0)
            nm9 = NitrogenMineralization (micrCNRatio, micrCNRatio, micrHumificationFactor, xx9);

        /* calculate N removal from decomposing pools */
        if (xx1 > 0.0)
            nr1 = xx1 * FRACTION_CARBON_PLANT / abgdCNRatio;
        if (xx2 > 0.0)
            nr2 = xx2 * FRACTION_CARBON_PLANT / stanCNRatio;
        if (xx3 > 0.0)
            nr3 = xx3 * FRACTION_CARBON_PLANT / flatCNRatio;
        if (xx4 > 0.0)
            nr4 = xx4 * FRACTION_CARBON_PLANT / rootCNRatio;
        if (xx5 > 0.0)
            nr5 = xx5 * FRACTION_CARBON_RIZHO / rhizCNRatio;
        if (xx6 > 0.0)
            nr6 = xx6 / smanCNRatio;
        if (xx7 > 0.0)
            nr7 = xx7 / manuCNRatio;
        if (xx8 > 0.0)
            nr8 = xx8 / somCNratio;
        if (xx9 > 0.0)
            nr9 = xx9 / micrCNRatio;

        /* calculate N contribution (N humification) to microbial pool of each decomposing pool */
        if (nm1 > 0.0)
            nh1 = nr1 - nm1;
        else
            nh1 = nr1;
        if (nm2 > 0.0)
            nh2 = nr2 - nm2;
        else
            nh2 = nr2;
        if (nm3 > 0.0)
            nh3 = nr3 - nm3;
        else
            nh3 = nr3;
        if (nm4 > 0.0)
            nh4 = nr4 - nm4;
        else
            nh4 = nr4;
        if (nm5 > 0.0)
            nh5 = nr5 - nm5;
        else
            nh5 = nr5;
        if (nm6 > 0.0)
            nh6 = nr6 - nm6;
        else
            nh6 = nr6;
        if (nm7 > 0.0)
            nh7 = nr7 - nm7;
        else
            nh7 = nr7;
        if (nm8 > 0.0)
            nh8 = nr8 - nm8;
        else
            nh8 = nr8;
        if (nm9 > 0.0)
            nh9 = nr9 - nm9;
        else
            nh9 = nr9;

        /* calculate total residue, manure, and som carbon tansfer to
         * microbial pool */
        humifiedCarbon = abgdHumificationFactor * FRACTION_CARBON_PLANT * (xx1 + xx2 + xx3) + rootHumificationFactor * FRACTION_CARBON_PLANT * xx4 + rhizHumificationFactor * FRACTION_CARBON_RIZHO * xx5 + manuHumificationFactor * (xx6 + xx7) + micrHumificationFactor * xx8;

        /* calculate total residue, manure, and som nitrogen transfer to
         * microbial pool */
        humifiedNitrogen = nh1 + nh2 + nh3 + nh4 + nh5 + nh6 + nh7 + nh8;

        /* accumulate N mineralization, immobilization, and net
         * mineralization */
        if (nm1 > 0.0)
            NMineralization += nm1;
        else
            NImmobilization += nm1;
        if (nm2 > 0.0)
            NMineralization += nm2;
        else
            NImmobilization += nm2;
        if (nm3 > 0.0)
            NMineralization += nm3;
        else
            NImmobilization += nm3;
        if (nm4 > 0.0)
            NMineralization += nm4;
        else
            NImmobilization += nm4;
        if (nm5 > 0.0)
            NMineralization += nm5;
        else
            NImmobilization += nm5;
        if (nm6 > 0.0)
            NMineralization += nm6;
        else
            NImmobilization += nm6;
        if (nm7 > 0.0)
            NMineralization += nm7;
        else
            NImmobilization += nm7;
        if (nm8 > 0.0)
            NMineralization += nm8;
        else
            NImmobilization += nm8;
        if (nm9 > 0.0)
            NMineralization += nm9;
        else
            NImmobilization += nm9;

        NNetMineralization = NMineralization + NImmobilization;

        /* UPDATE POOLS (N immbilization is negative) */
        Soil->NO3[i] += NImmobilization * (1.0 - NH4_Fraction);
        Soil->NH4[i] += NImmobilization * NH4_Fraction + NMineralization;

        if (i == 0)
        {
            Residue->stanResidueWater *= (1.0 - xx2 / (Residue->stanResidueMass + 1e-10));
            Residue->flatResidueWater *= (1.0 - xx3 / (Residue->flatResidueMass + 1e-10));
            Residue->stanResidueMass -= xx2;
            Residue->flatResidueMass -= xx3;
            Residue->manureSurfaceC -= xx6;
            Residue->stanResidueN -= nr2;
            Residue->flatResidueN -= nr3;
            Residue->manureSurfaceN -= nr6;
        }

        Residue->residueAbgd[i] -= xx1;
        Residue->residueRt[i] -= xx4;
        Residue->residueRz[i] -= xx5;
        Residue->manureC[i] -= xx7;
        Residue->residueAbgdN[i] -= nr1;
        Residue->residueRtN[i] -= nr4;
        Residue->residueRzN[i] -= nr5;
        Residue->manureN[i] -= nr7;

        Soil->SOC_Mass[i] += xx9 * micrHumificationFactor - xx8;
        Soil->SON_Mass[i] += nh9 - nr8;
        Soil->SOC_Conc[i] = Soil->SOC_Mass[i] * 1000.0 / soilMass;
        Soil->MBC_Mass[i] += humifiedCarbon - xx9;
        Soil->MBN_Mass[i] += humifiedNitrogen + (-NImmobilization) - nr9;
        SoilCarbon->carbonRespired[i] = (1.0 - abgdHumificationFactor) * FRACTION_CARBON_PLANT * (xx1 + xx2 + xx3) + (1.0 - rootHumificationFactor) * FRACTION_CARBON_PLANT * xx4 + (1.0 - rhizHumificationFactor) * FRACTION_CARBON_RIZHO * xx5 + (1.0 - manuHumificationFactor) * (xx6 + xx7) + (1.0 - micrHumificationFactor) * (xx8 + xx9);

        /* for OUTPUT */
        SoilCarbon->annualSoilCarbonDecompositionRate[i] += socDecompositionRate;

        /* excludes residue and manure */
        SoilCarbon->annualRespiredCarbonMass[i] += (1.0 - micrHumificationFactor) * (xx8 + xx9);

        /* residue, roots and manure only */
        SoilCarbon->annualRespiredResidueCarbonMass[i] += SoilCarbon->carbonRespired[i] - (1.0 - micrHumificationFactor) * (xx8 + xx9);
        SoilCarbon->abgdCarbonInput[i] += FRACTION_CARBON_PLANT * (xx1 + xx2 + xx3);
        SoilCarbon->rootCarbonInput[i] += FRACTION_CARBON_PLANT * xx4;
        SoilCarbon->rhizCarbonInput[i] += FRACTION_CARBON_RIZHO * xx5;
        SoilCarbon->manuCarbonInput[i] += (xx6 + xx7);
        SoilCarbon->annualCarbonInputByLayer[i] = SoilCarbon->abgdCarbonInput[i] + SoilCarbon->rootCarbonInput[i] + SoilCarbon->rhizCarbonInput[i] + SoilCarbon->manuCarbonInput[i];

        /* C that goes to SOC and C that goes to microbial mass */
        SoilCarbon->annualHumifiedCarbonMass[i] += xx9 * micrHumificationFactor + humifiedCarbon;
        Soil->N_Immobilization += NImmobilization;
        Soil->N_Mineralization += NMineralization;
        Soil->N_NetMineralization += NNetMineralization;
        Soil->SOCProfile += Soil->SOC_Mass[i] + Soil->MBC_Mass[i];
        Soil->SONProfile += Soil->SON_Mass[i] + Soil->MBN_Mass[i];

        /* C that goes to SOC and C that goes to microbial mass */
        Soil->C_Humified += xx9 * micrHumificationFactor + humifiedCarbon;

        /* residues, roots and manure */
        Soil->C_ResidueRespired += SoilCarbon->carbonRespired[i] - (1.0 - micrHumificationFactor) * (xx8 + xx9);
        Soil->C_SoilRespired += (1.0 - micrHumificationFactor) * (xx8 + xx9);
        NFinal += Soil->SON_Mass[i] + Soil->MBN_Mass[i] + Soil->NO3[i] + Soil->NH4[i] + Residue->residueAbgdN[i] + Residue->residueRtN[i] + Residue->residueRzN[i] + Residue->manureN[i];
    }                           /* End soil layer loop */

    NFinal += Residue->stanResidueN + Residue->flatResidueN + Residue->manureSurfaceN;

    if (fabs (NFinal - NInitial) > 0.00001)
    {
        exit (1);
    }
}

void StoreOutput (SoilCarbonStruct *SoilCarbon, int y, int totalLayers, double *SOCMass)
{
    int             i;

    for (i = 0; i < totalLayers; i++)
        SoilCarbon->carbonMassFinal[i] = SOCMass[i];
}

double Aeration (double AC)
{
    /* AC = soil air content */
    const double    A1 = 0.05;
    const double    A2 = 4;

    return (1.0 - 0.6 / (1.0 + pow (AC / A1, A2)));
}

double Moisture (double wp)
{
    /* WP = soil water potential */
    double          M1 = -600.0;
    double          M2 = 3.0;

    return (1.0 / (1.0 + pow (wp / M1, M2)));
}

double TemperatureFunction (double T)
{
    double          temp;
    double          Q;
    double          tMin = -5.0;
    double          tOpt = 35.0;
    double          tMax = 50.0;

    if (T < 0.0 || T > tMax)
        temp = 0.0;
    else
    {
        Q = (tMin - tOpt) / (tOpt - tMax);
        temp = (pow (T - tMin, Q) * (tMax - T)) / (pow (tOpt - tMin, Q) * (tMax - tOpt));
        if (temp > 1.0)
            temp = 1.0;
    }

    return temp;
}

double MaximumAbgdHumificationFactor (double clayFraction)
{
    return (0.092 + 0.104 * (1.0 - exp (-5.5 * clayFraction)));
}

double MaximumRootHumificationFactor (double clayFraction)
{
    return (0.092 + 0.104 * (1.0 - exp (-5.5 * clayFraction)));
}

double MaximumRhizHumificationFactor (double clayFraction)
{
    return (0.0 + 0.084 * (1.0 - exp (-5.5 * clayFraction)));
}

double MaximumManuHumificationFactor (double clayFraction)
{
    return (0.15 + 0.25 * (1.0 - exp (-5.5 * clayFraction)));
}

double NitrogenMineralization (double CNDecomposing, double CNnew, double humRate, double decomposedMass)
{
    return (decomposedMass * (1.0 / CNDecomposing - humRate / CNnew));
}

double CNdestiny (double NmineralConc, double CNdecomposing)
{

    /* returns CN ratio of newly formed microbial biomass based on CN or
     * decomposing residue and N mineral in soil. Same function that for one-
     * pool model, but applied to microbial biomass */
    double          YY2;

    NmineralConc = NmineralConc + 0.0000001;
    YY2 = 5.5 * (1.0 - 1.0 / (1.0 + pow (CNdecomposing / 110.0, 3.0)));

    return (8.5 + YY2 * (0.5 + 0.5 / (1.0 + pow (NmineralConc / 0.000008, 3.0))));
}

double PoolNitrogenMineralization (double NmineralConc, double CNRatioDecomposing, double humRate, double decomposedMass, double carbonConc)
{
    double          newCN;      /* CN of new organic matter (humified residue) */

    decomposedMass *= carbonConc;
    newCN = Function_CNnew (NmineralConc, CNRatioDecomposing);

    return (decomposedMass * (1.0 / CNRatioDecomposing - humRate / newCN));
}

double Function_CNnew (double NmineralConc, double CNDecomposingPool)
{

    /* returns CN ratio of newly formed organic matter based on CN or
     * decomposing residue and N mineral in soil */
    double          YY2;

    NmineralConc = NmineralConc + 0.0000001;
    YY2 = 5.5 * (1.0 - 1.0 / (1.0 + pow (CNDecomposingPool / 110.0, 3.0)));

    return (8.5 + YY2 * (0.5 + 0.5 / (1.0 + pow (NmineralConc / 0.000008, 3.0))));
}
