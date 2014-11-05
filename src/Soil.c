#include "include/Cycles.h"

void InitializeSoil (SoilClass * Soil, WeatherClass * Weather, SimControlClass * SimControl)
{
    double          WC33;       /* volumetric water content at 33 J/kg */
    double          WC1500;     /* volumetric water contetn at 1500 J/kg */
    int             i;

    /* Initialize soil vairables */


    Soil->nodeDepth = (double *)malloc ((Soil->totalLayers + 1) * sizeof (double));
    Soil->cumulativeDepth = (double *)malloc ((Soil->totalLayers) * sizeof (double));
    Soil->waterContent = (double *)malloc ((Soil->totalLayers) * sizeof (double));
    Soil->soilTemperature = (double *)malloc ((Soil->totalLayers) * sizeof (double));
    Soil->Porosity = (double *)malloc ((Soil->totalLayers) * sizeof (double));
    Soil->PAW = (double *)malloc ((Soil->totalLayers) * sizeof (double));
    Soil->FC_WaterPotential = (double *)malloc ((Soil->totalLayers) * sizeof (double));
    Soil->airEntryPotential = (double *)malloc ((Soil->totalLayers) * sizeof (double));
    Soil->B_Value = (double *)malloc ((Soil->totalLayers) * sizeof (double));
    Soil->M_Value = (double *)malloc ((Soil->totalLayers) * sizeof (double));
    Soil->SOC_Conc = (double *)malloc ((Soil->totalLayers) * sizeof (double));
    Soil->SOC_Mass = (double *)malloc ((Soil->totalLayers) * sizeof (double));
    Soil->SON_Mass = (double *)malloc ((Soil->totalLayers) * sizeof (double));
    Soil->MBC_Mass = (double *)malloc ((Soil->totalLayers) * sizeof (double));
    Soil->MBN_Mass = (double *)malloc ((Soil->totalLayers) * sizeof (double));
    Soil->waterUptake = (double *)malloc ((Soil->totalLayers) * sizeof (double));
    Soil->pH = (double *)malloc ((Soil->totalLayers) * sizeof (double));
    Soil->n2o = (double *)malloc ((Soil->totalLayers) * sizeof (double));

    Soil->cumulativeDepth[0] = 0;
    Soil->nodeDepth[0] = -0.5 * Soil->layerThickness[0];

    for (i = 0; i < Soil->totalLayers; i++)
    {
        Soil->Clay[i] = Soil->Clay[i] / 100.;
        Soil->Sand[i] = Soil->Sand[i] / 100.;
        Soil->IOM[i] = Soil->IOM[i];
        Soil->NO3[i] = Soil->NO3[i] / 1000.;
        Soil->NH4[i] = Soil->NH4[i] / 1000.;
        if (i > 0)
            Soil->cumulativeDepth[i] = Soil->cumulativeDepth[i - 1] + Soil->layerThickness[i];
        Soil->nodeDepth[i] = Soil->cumulativeDepth[i] + Soil->layerThickness[i] / 2.;
    }

    Soil->nodeDepth[Soil->totalLayers] = Soil->cumulativeDepth[Soil->totalLayers - 1] + Soil->layerThickness[Soil->totalLayers - 1] / 2.;

    for (i = 0; i < Soil->totalLayers; i++)
    {
        if (Soil->BD[i] == BADVAL)  /* Buld Density switch */
            Soil->BD[i] = BulkDensity (Soil->Clay[i], Soil->Sand[i], Soil->IOM[i]);

        Soil->Porosity[i] = 1. - Soil->BD[i] / 2.65;
        WC33 = VolumetricWCAt33Jkg (Soil->Clay[i], Soil->Sand[i], Soil->IOM[i]);
        WC1500 = VolumetricWCAt1500Jkg (Soil->Clay[i], Soil->Sand[i], Soil->IOM[i]);
        Soil->B_Value[i] = (log (1500.) / log (33.)) / (log (WC33) - log (WC1500));
        Soil->airEntryPotential[i] = -33. * pow (WC33 / Soil->Porosity[i], Soil->B_Value[i]);
        Soil->M_Value[i] = 2. * Soil->B_Value[i] + 3.;

        if (Soil->FC[i] == BADVAL)  /* Field Capacity switch */
        {
            Soil->FC_WaterPotential[i] = -0.35088 * Soil->Clay[i] * 100. - 28.947;
            Soil->FC[i] = SoilWaterContent (Soil->Porosity[i], Soil->airEntryPotential[i], Soil->B_Value[i], Soil->FC_WaterPotential[i]);
        }
        else
            Soil->FC_WaterPotential[i] = SoilWaterPotential (Soil->Porosity[i], Soil->airEntryPotential[i], Soil->B_Value[i], Soil->FC[i]);

        if (Soil->PWP[i] == BADVAL) /* Permanent Wilting Point switch */
            Soil->PWP[i] = SoilWaterContent (Soil->Porosity[i], Soil->airEntryPotential[i], Soil->B_Value[i], -1500.);

        if (Soil->PWP[i] >= Soil->FC[i])
        {
            printf ("ERROR: Permanent Wilting Point must be less than Field Capacity in Layer %d\n", i + 1);
            exit (1);
        }
    }
    /* initialize variables depending on previous loop */
    for (i = 0; i < Soil->totalLayers; i++)
    {
        Soil->SOC_Conc[i] = Soil->IOM[i] * 10. * 0.58;
        Soil->SOC_Mass[i] = Soil->IOM[i] / 100. * 0.58 * Soil->layerThickness[i] * Soil->BD[i] * 10000.;
        Soil->SON_Mass[i] = Soil->SOC_Mass[i] / 10.;    /* Initializes with CN ratio = 11 */
        Soil->MBC_Mass[i] = 0.03 * Soil->SOC_Mass[i];   /* Initializes as 3% of SOC_Mass
                                                         * but "added" C */
        Soil->MBN_Mass[i] = Soil->MBC_Mass[i] / 10.;    /* Initializes with CN ratio = 10 */
        Soil->PAW[i] = Soil->FC[i] - Soil->PWP[i];
        Soil->waterContent[i] = (Soil->FC[i] + Soil->PWP[i]) / 2.;
    }

    /* initializes soil temperature in first day of simulation */
    Soil->dampingDepth = 2.;

    printf ("Latitude = %lf\n", Weather->siteLatitude);
    if (Weather->siteLatitude >= 0)
        Soil->annualTemperaturePhase = 100;
    else
        Soil->annualTemperaturePhase = 280;

    printf ("phase = %d\n", Soil->annualTemperaturePhase);

    Soil->soilTemperature = (double *)malloc ((Soil->totalLayers + 1) * sizeof (double));
    for (i = 0; i < Soil->totalLayers + 1; i++)
    {
        Soil->soilTemperature[i] = EstimatedSoilTemperature (Soil->nodeDepth[i], 1, annualAvgTemperature (Weather, SimControl->simStartYear), annualAmplitude (Weather, SimControl->simStartYear), Soil->annualTemperaturePhase, Soil->dampingDepth);
        printf ("return value %lf\n", EstimatedSoilTemperature (Soil->nodeDepth[i], 1, annualAvgTemperature (Weather, SimControl->simStartYear), annualAmplitude (Weather, SimControl->simStartYear), Soil->annualTemperaturePhase, Soil->dampingDepth));
        printf ("STC = %lf; %lf %d %lf %d %lf \n", Soil->soilTemperature[i], Soil->nodeDepth[i], 1, annualAvgTemperature (Weather, SimControl->simStartYear), annualAmplitude (Weather, SimControl->simStartYear), Soil->annualTemperaturePhase, Soil->dampingDepth);
    }

#ifdef _DEBUG_
    printf ("Clay\t\tSand\t\tIOM\t\tNO3\t\tNH4\t\tnodeDepth\t\tcumulativeD\t\tBD\t\tPorosity\t\tb\t\tairEntryPot\t\tm\t\tFC_pot\t\tFC\t\tPWP\t\tSOC_c\t\tSOC_m\t\tSON_m\t\tMBC_m\t\tMBN_m\t\tPAW\t\twaterContent\n");
    for (i = 0; i < Soil->totalLayers; i++)
    {
        printf ("%lf\t", Soil->Clay[i]);
        printf ("%lf\t", Soil->Sand[i]);
        printf ("%lf\t", Soil->IOM[i]);
        printf ("%lf\t", Soil->NO3[i]);
        printf ("%lf\t", Soil->NH4[i]);
        printf ("%lf\t", Soil->nodeDepth[i]);
        printf ("%lf\t", Soil->cumulativeDepth[i]);
        printf ("%lf\t", Soil->BD[i]);
        printf ("%lf\t", Soil->Porosity[i]);
        printf ("%lf\t", Soil->B_Value[i]);
        printf ("%lf\t", Soil->airEntryPotential[i]);
        printf ("%lf\t", Soil->M_Value[i]);
        printf ("%lf\t", Soil->FC_WaterPotential[i]);
        printf ("%lf\t", Soil->FC[i]);
        printf ("%lf\t", Soil->PWP[i]);
        printf ("%lf\t", Soil->SOC_Conc[i]);
        printf ("%lf\t", Soil->SOC_Mass[i]);
        printf ("%lf\t", Soil->SON_Mass[i]);
        printf ("%lf\t", Soil->MBC_Mass[i]);
        printf ("%lf\t", Soil->MBN_Mass[i]);
        printf ("%lf\t", Soil->PAW[i]);
        printf ("%lf\n", Soil->waterContent[i]);
    }
#endif
}

double SoilWaterPotential (double SaturationWC, double AirEntryPot, double Campbell_b, double WC)
{
    return AirEntryPot * pow (WC / SaturationWC, -Campbell_b);
}

double VolumetricWCAt33Jkg (double Clay, double Sand, double OM)
{
    /* Saxton and Rawls 2006 SSSAJ 70:1569-1578 Eq 2 (r2 = 0.63) */
    /* clay and sand fractional, OM as %
     * (original paper says % for everything, results make no sense) */
    double          x1;

    x1 = 0.299 - 0.251 * Sand + 0.195 * Clay + 0.011 * OM + 0.006 * Sand * OM - 0.027 * Clay * OM + 0.452 * Sand * Clay;
    return -0.015 + 0.636 * x1 + 1.283 * pow (x1, 2);
}

double VolumetricWCAt1500Jkg (double Clay, double Sand, double OM)
{
    /* Saxton and Rawls 2006 SSSAJ 70:1569-1578 Eq 1 (r2 = 0.86) */
    /* clay and sand fractional, OM as %
     * (original paper says % for everything, results make no sense) */
    double          x1;

    x1 = 0.031 - 0.024 * Sand + 0.487 * Clay + 0.006 * OM + 0.005 * Sand * OM - 0.013 * Clay * OM + 0.068 * Sand * Clay;
    return -0.02 + 1.14 * x1;
}

double SoilWaterContent (double SaturationWC, double AirEntryPot, double Campbell_b, double Water_Potential)
{
    return SaturationWC * pow (Water_Potential / AirEntryPot, -1. / Campbell_b);
}

double BulkDensity (double Clay, double Sand, double OM)
{
    /* Saxton and Rawls 2006 SSSAJ 70:1569-1578 Eq 6,5 (r2 = 0.29)
     * really poor fit */
    /* clay and sand fractional, OM as %
     * (original paper says % for everything, results make no sense) */
    /*  Note: X2 is Eq 3, supposedly representing moisture from FC to
     *  Saturation; however, porosity is further adjusted by sand, an
     *  inconsistency */

    double          x1;
    double          x2;
    double          FC;
    double          Porosity;

    x1 = 0.078 + 0.278 * Sand + 0.034 * Clay + 0.022 * OM - 0.018 * Sand * OM - 0.027 * Clay * OM - 0.584 * Sand * Clay;
    x2 = -0.107 + 1.636 * x1;
    FC = VolumetricWCAt33Jkg (Clay, Sand, OM);
    Porosity = 0.043 + FC + x2 - 0.097 * Sand;
    return (1. - Porosity) * 2.65;
}
