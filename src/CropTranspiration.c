#include "include/Cycles.h"
   
void WaterUptake (int y, int doy, CropStruct *Crop, SoilStruct *Soil, WeatherStruct *Weather)
{
    double waterExtraction[Soil->totalLayers];          /* water extraction by layer, mm/day */

    double PTx = 15.;               /* maximum full cover transpiration, mm/day */
    double PT = 0.00001;            /* potential transpiration, mm/day */
    double TE;                      /* expected transpiration, the minimum of PT and PWU, mm/day */
    double TA;                      /* attainable transpiration (or TE adjusted by LWP), mm/day */

    double transpirationRatio;
    double temperatureAvg;
    double factorTemperature;
    double factorAeration;

    double plantHC;                 /* plant hydraulic conductance, (kg/m2)/(J/kg) = kg2/(J m2) */
    double rootHC;                  /* root hydraulic conductance */
    double shootHC;                 /* shoot hydraulic conductance */
    double Root_HC_Adjustment;      /* dryness adjustment of hydraulic conductance */
    double layerPlantHC[Soil->totalLayers];
    double layerRootHC[Soil->totalLayers];
    double layerShootHC[Soil->totalLayers];
    double Layer_Root_HC_Adjustment[Soil->totalLayers];
    double rootActivity[Soil->totalLayers];      /* root activity factor to compute plant hydraulic conductance */
    double rootFraction[Soil->totalLayers];

    double LWP;                     /* leaf water potential, J/kg */
    double LWP_StressOnset = -1100.;    /* leaf water potential at the onset of stomatal closure */
    double LWP_WiltingPoint = -2000.;   /* leaf water potential at wilting point */
    double SWP_FC As Double = -33.;     /* water potential at field capacity, J/kg */
    double SWP_Average;                 /* weighted soil water potential, J/kg */
    double soilWP[Soil->totalLayers];
    double layerSalinityFactor[Soil->totalLayers];

    Crop->svTranspiration = 0.;
    Crop->svTranspirationPotential = 0.;
    Crop->svWaterStressFactor = 0.;

    if (Crop->cropGrowing && Crop->svTT_Cumulative > Crop->userEmergenceTT)
    {
        temperatureAvg = 0.5 * (Weather->tMax[y, doy - 1] + Weather->tMin[y, doy - 1]);
        factorTemperature = TemperatureLimitation (temperatureAvg, Crop->userTranspirationMinTemperature, Crop->userTranspirationThresholdTemperature);

        /* calculate potential transpiration rate (kg/m2/d = mm/d) */
        PT = (1. + (Crop->userKc - 1.) * Crop->svRadiationInterception) * factorTemperature * Crop->svRadiationInterception * Weather->ETref[y, doy - 1];
        /* calculate crop maximum water uptake rate (kg/m2/d = mm/d) */
        PTx = PTx * factorTemperature * Crop->svRadiationInterception;
        /* calculate maximum crop transpiration rate (kg/m2/d = mm/d) */
        TE = PT < PTx ? PT : PTx;

        /* calculate root fraction per soil layer */
        for (i = 0; i < Soil->totalLayers; i++)
        {
            Soil->waterUptake[i] = 0.;
            soilWP[i] = Soil->SoilWaterPotential (Soil->Porosity[i], Soil->airEntryPotential[i], Soil->B_Value[i], Soil->waterContent[i]);
        }

        CalcRootFraction (rootFraction, Soil, Crop);

        /* calculate plant hydraulic conductivity (kg^2)/(m2 J d)
         * this is the hydraulic conductivity of a canopy fully covering the ground */
        plantHC = PTx / (SWP_FC - LWP_StressOnset);
        rootHC = plantHC / 0.65;
        shootHC = plantHC / 0.35;

        /* adjust plant hydraulic conductance based on soil dryness */
        Root_HC_Adjustment = 0.;

        for (i = 0; i < Soil->totalLayers; i++)
        {
            rootActivity[i] = 1.;
            layerSalinityFactor[i] = 1. /* place holder */
            rootActivity[i] = 1. - pow((soilWP[i] - SWP_FC) / (LWP_WiltingPoint - SWP_FC), 8);
            if (rootActivity[i] > 1.)
                rootActivity[i] = 1.;
            if (rootActivity[i] < 0.)
                rootActivity[i] = 0.;

            Layer_Root_HC_Adjustment[i] = rootActivity[i] * rootFraction[i] * layerSalinityFactor[i];
            layerRootHC[i] = rootHC * Layer_Root_HC_Adjustment[i];
            Root_HC_Adjustment += Layer_Root_HC_Adjustment[i];
        }

        for (i = 0; i < Soil->totalLayers; i++)
        {
            if (Layer_Root_HC_Adjustment[i] > 0.)
            {
                layerShootHC[i] = shootHC * Layer_Root_HC_Adjustment[i] / Root_HC_Adjustment;
                layerPlantHC[i] = layerRootHC[i] * layerShootHC[i] / (layerRootHC[i] + layerShootHC[i]);
            }
            else
                layerPlantHC[i] = 0.;

        }

        rootHC *= Root_HC_Adjustment;
        plantHC = (rootHC * shootHC) / (rootHC + shootHC);

        if (plantHC > 0.)
        {
            /* calculate average soil water potential (J/kg) */
            SWP_Average = 0.;

            for (i = 0; i < Soil->totalLayers; i++)
                SWP_Average += soilWP[i] * Layer_Root_HC_Adjustment[i] / Root_HC_Adjustment;

            /* calculate leaf water potential */
            LWP = SWP_Average - TE / plantHC;

            if (LWP < LWP_StressOnset)
                LWP = (plantHC * SWP_Average * (LWP_StressOnset - LWP_WiltingPoint) + LWP_WiltingPoint * TE) / (plantHC * (LWP_StressOnset - LWP_WiltingPoint) + TE);

            if (LWP < LWP_WiltingPoint)
                LWP = LWP_WiltingPoint;

            /* reduce transpiration when LWP < LWP at the onset of stomatal
             * closure */
            if (LWP < LWP_StressOnset)
            {
                TA = TE * (LWP - LWP_WiltingPoint) / (LWP_StressOnset - LWP_WiltingPoint);
                transpirationRatio = TA / TE;
            }
            else
                transpirationRatio = 1.;

            /* calculate crop water uptake (kg/m2/d = mm/d) */
            for (i = 0; i < Soil->totalLayers; i++)
            {
                Soil->waterUptake[i] = layerPlantHC[i] * (soilWP[i] - LWP) * transpirationRatio;
                Soil->waterContent[i] -= Soil->waterUptake[i] / (Soil->layerThickness[i] * WATER_DENSITY);
            }
        }

        Crop->svTranspiration = Soil->waterUptake.Sum
        Crop->svTranspirationPotential = TE
        Crop->svWaterStressFactor = 1 - Soil->waterUptake.Sum / TE
    }   /* end plant growing */
}

void CalcRootFraction (double *fractionRootsByLayer, SoilStruct *Soil As SoilClass, CropStruct Crop)
{
    /* 
     * this function computes root fraction in each layer
     * it assumes that the roots will reach a final distribution
     * Root=a*exp[-b*z], with b~4
     * Root_layer = a / b * (Exp(-b * z1) - Exp(-b * z2)), where z1 and z2 are the top and bottom of layer
     * to compute the progression, the roots are "assumed" to have reached the maximum depth
     * then the root lenght in each layer is computed
     * only the roots within the current rooting depth are considered for the calculation
     * the root fraction is then recalculated for the layers with roots
     * the net effect is that the roots are not sharply exponential during early growth
     */

    double a = 1;
    double b = 4.;      /* units of 1/m */
    double rootIntegral  = 0.;
    double rootSum;

    double rootDistribution[Soil->totalLayers];
    double cumulativeRootingDepth = 0.;
    double z1, z2;
    int i, j = 0;
       
    rootIntegral = a / b * (exp(-b * 0.) - exp(-b * Crop->userMaximumRootingDepth));
    while (cumulativeRootingDepth < Crop->svRootingDepth && j < Soil->totalLayers)
    {
        if (Soil->cumulativeDepth[j] < Crop->svRootingDepth)
            cumulativeRootingDepth = Soil->cumulativeDepth[j];
        else if (Soil->cumulativeDepth >= Crop->svRootingDepth)
            cumulativeRootingDepth = Crop->svRootingDepth;

        if (j == 0)
            z1 = 0.;
        else
            z1 = Soil->cumulativeDepth[j - 1];
        z2 = cumulativeRootingDepth;
        rootDistribution[j] = ((a / b) * (exp(-b * z1) - exp(-b * z2))) / rootIntegral;
        j++;
    }

    rootSum = 0.;
    for (i = 0; i < j; i++)
        rootSum = rootSum + rootDistribution[j];

    /* ensures sum fraction = 1 */
    for (i = 0; i < j; i++)     /* exits loop on the same layer as the previous loop */
        fractionRootsByLayer[i] = rootDistribution[i] / rootSum;
}

double TemperatureLimitation (double T, double T_Min, double T_Threshold)
{
    if (T <= T_Min)
        return (0.01);
    else if (T > T_Min && T <= T_Threshold)
        return ((T - T_Min) / (T_Threshold - T_Min));
    else
        return (1.);
}

double Aeration(double AC)
{
    /* AC = soil air content */
    double A1 = 0.05;
    double A2 = 4.;

    return (1. - 0.6 / (1. + pow(AC / A1, A2)));
}
