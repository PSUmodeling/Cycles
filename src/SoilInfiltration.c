#include "Cycles.h"

void Redistribution (int y, int doy, double precipitation, double snowFall, double snowMelt, int hourlyInfiltration, const CropStruct *Crop, SoilStruct *Soil, ResidueStruct *Residue)
{
    /*
     * This sub controls surface hydrology, infiltration, and redistribution
     * in the profile calculates input from irrigation estimates potential
     * input from irrigation, precipitation and snowmelt wet surface residues
     * estimates runoff infiltrates and/or redistributes water
     */

    int             i;
    double          irrigation_vol;

    Soil->infiltrationVol = 0.0;
    Soil->runoffVol = 0.0;
    Soil->drainageVol = 0.0;
    Soil->NO3Leaching = 0.0;
    Soil->NH4Leaching = 0.0;

    if (Crop->autoIrrigationUsed)
    {
        if (Crop->autoIrrigationStartDay < Crop->autoIrrigationStopDay)
        {
            if (doy >= Crop->autoIrrigationStartDay && doy <= Crop->autoIrrigationStopDay)
            {
                irrigation_vol = FindIrrigationVolume (Crop->autoIrrigationLastSoilLayer, Crop->autoIrrigationWaterDepletion, Soil);
                if (irrigation_vol > 0.0)
                {
                    if (verbose_mode)
                        printf ("DOY %3.3d %-20s %lf\n", doy, "Auto Irrigation", irrigation_vol);
                }
                Soil->irrigationVol += irrigation_vol;
            }
        }
        else
        {
            if (doy >= Crop->autoIrrigationStartDay || doy <= Crop->autoIrrigationStopDay)
            {
                irrigation_vol = FindIrrigationVolume (Crop->autoIrrigationLastSoilLayer, Crop->autoIrrigationWaterDepletion, Soil);
                if (irrigation_vol > 0.0)
                {
                    if (verbose_mode)
                        printf ("DOY %3.3d %-20s %lf\n", doy, "Auto Irrigation", irrigation_vol);
                }
                Soil->irrigationVol += irrigation_vol;
            }
        }
    }

    Soil->infiltrationVol = (precipitation - snowFall) + Soil->irrigationVol + snowMelt;

    if (Soil->infiltrationVol > 0.0)
    {
        ResidueWetting (Residue, Soil); /* wet surface residues before runoff */
        CurveNumber (Soil);     /* runoff before infiltration */
    }

    if (hourlyInfiltration)
    {
        SubDailyRedistribution (Soil);
    }
    else if (Soil->infiltrationVol > 0.0)
    {
        CascadeRedistribution (Soil);
    }
}

void CascadeRedistribution (SoilStruct *Soil)
{
    /*
     * cascade approach, excess from FC is drainage
     */
    int             i, j;
    double          Win = Soil->infiltrationVol;
    double          Wout;
    double          WFlux[Soil->totalLayers + 1];
    double          WCi[Soil->totalLayers]; /* store initial soil water content */

    for (i = 0; i < Soil->totalLayers; i++)
        WCi[i] = Soil->waterContent[i];

    WFlux[0] = Win;

    for (j = 0; j < Soil->totalLayers; j++)
    {
        if (Win > WATER_DENSITY * (Soil->FC[j] - Soil->waterContent[j]) * Soil->layerThickness[j])
        {
            Wout = Win - WATER_DENSITY * (Soil->FC[j] - Soil->waterContent[j]) * Soil->layerThickness[j];
            Soil->waterContent[j] = Soil->FC[j];
        }
        else
        {
            Soil->waterContent[j] += Win / (WATER_DENSITY * Soil->layerThickness[j]);
            Wout = 0.0;
        }

        WFlux[j + 1] = Wout;
        Win = Wout;
        if (Win <= 0.0)
            break;
    }

    if (Wout > 0.0)
        Soil->drainageVol += Wout;

    SoluteTransport (Soil->totalLayers, 0.0, 0, &(Soil->NO3Leaching), WFlux, Soil->NO3, Soil->BD, Soil->layerThickness, Soil->Porosity, WCi);
    SoluteTransport (Soil->totalLayers, 5.6, 0, &(Soil->NH4Leaching), WFlux, Soil->NH4, Soil->BD, Soil->layerThickness, Soil->Porosity, WCi);
}

void SubDailyRedistribution (SoilStruct *Soil)
{
    int             i, j;
    double          Win;
    double          dzx[Soil->totalLayers + 1]; /* layer thickness times
                                                 * water density, kg/m2
                                                 * (to reduce
                                                 * computations) */
    double          ksat[Soil->totalLayers + 1];    /* saturated hydraulic
                                                     * conductivity kg s/m3 */
    double          wp[Soil->totalLayers + 1];  /* water potential J/kg */
    double          kh[Soil->totalLayers + 1];  /* hydraulic conductance
                                                 * kg s/m3 */
    double          t1, t2, dt, cum_dt, x1, x2, x3, x4;
    int             RedistributionFlag = 0;
    int             SoluteFlag = 0;

    /* local array dimension AND copy */
    double          WC[Soil->totalLayers];
    double          FC[Soil->totalLayers];
    double          sat[Soil->totalLayers];
    double          b[Soil->totalLayers];
    double          aep[Soil->totalLayers];
    double          wpfc[Soil->totalLayers];
    double          WCi[Soil->totalLayers];
    double          WFlux[Soil->totalLayers + 1];
    const double    g = 9.81;
    const double    s = 86400.0;    /* seconds per day (or fraction of the day) */

    RedistributionFlag = 0;

    Win = Soil->infiltrationVol;

    for (i = 0; i < Soil->totalLayers; i++)
    {
        WC[i] = Soil->waterContent[i];
        FC[i] = Soil->FC[i];
        sat[i] = Soil->Porosity[i];
        b[i] = Soil->B_Value[i];
        aep[i] = Soil->airEntryPotential[i];
        wpfc[i] = Soil->FC_WaterPotential[i];
        WCi[i] = Soil->waterContent[i];
    }

    for (i = 0; i < Soil->totalLayers + 1; i++)
        WFlux[i] = 0.0;

    for (j = 0; j < Soil->totalLayers; j++)
    {
        dzx[j] = Soil->layerThickness[j] * WATER_DENSITY;
        ksat[j] = K_Sat (sat[j], FC[j], b[j]);
    }

    if (Win > 0.0)
    {
        RedistributionFlag = 1;
        SoluteFlag = 1;
    }

    /* check if RedistributionFlag has to be set to TRUE when Win=0 */
    if (!RedistributionFlag)
    {
        for (j = 0; j < Soil->totalLayers; j++)
        {
            if (WC[j] > FC[j])
            {
                RedistributionFlag = 1;
                SoluteFlag = 1;
                break;
            }
        }
    }

    /* redistribution */
    cum_dt = 0.0;

    while (RedistributionFlag)
    {
        x1 = 0.0;

        if (Win > 0.0)          /* wet layer[0] - up to saturation */
        {
            x1 = (sat[0] - WC[0]) * dzx[0];
            if (Win > x1)
            {
                WC[0] = sat[0];
                Win -= x1;
            }
            else
            {
                WC[0] += Win / dzx[0];
                x1 = Win;
                Win = 0.0;
            }
        }


        WFlux[0] += x1;         /* store flux into layer 0 */

        /* check if redistribution still true */
        if (Win == 0.0)
        {
            RedistributionFlag = 0;

            for (j = 0; j < Soil->totalLayers; j++)
            {
                if (WC[j] > FC[j])
                {
                    RedistributionFlag = 1;
                    break;
                }
            }

            if (!RedistributionFlag)
                break;
        }

        /* compute travel time for all emitting layers, in seconds select
         * minimum travel time of emitting and receiving */
        dt = s;

        for (j = 0; j < Soil->totalLayers; j++)
        {
            if (WC[j] > FC[j])
            {
                t1 = s;
                t2 = s;

                wp[j] = SoilWaterPotential (sat[j], aep[j], b[j], WC[j]);
                if (wpfc[j] / wp[j] < 1.001)
                    wp[j] = wp[j] * 0.999;

                kh[j] = AverageHydraulicConductance (sat[j], FC[j], aep[j], b[j], wp[j], wpfc[j], ksat[j]);

                t1 = (WC[j] - FC[j]) * dzx[j] / (kh[j] * g);    /* emitting layer */

                if (j < Soil->totalLayers - 1)
                {
                    wp[j + 1] = SoilWaterPotential (sat[j + 1], aep[j + 1], b[j + 1], WC[j + 1]);
                    if (wpfc[j + 1] / wp[j + 1] < 1.001)
                        wp[j + 1] = wp[j + 1] * 0.999;

                    kh[j + 1] = AverageHydraulicConductance (sat[j + 1], FC[j + 1], aep[j + 1], b[j + 1], wp[j + 1], wpfc[j + 1], ksat[j + 1]);

                    t2 = (sat[j + 1] - FC[j + 1]) * dzx[j + 1] / (kh[j + 1] * g);   /* maximum volume of receiving layer */
                }
                if (t2 < t1)
                    t1 = t2;
            }

            if (t1 < dt)
                dt = t1;

            if (dt < 3600.0)
                dt = 3600.0;
        }

        cum_dt += dt;

        if (cum_dt >= s)
        {
            dt = dt - (cum_dt - s);
            cum_dt = s;
            RedistributionFlag = 0;
        }

        for (j = Soil->totalLayers - 1; j >= 0; j--)
        {
            x2 = 0.0;
            x3 = 0.0;
            x4 = 0.0;

            if (WC[j] > FC[j])
            {
                x2 = g * kh[j] * dt;
                x3 = (WC[j] - FC[j]) * dzx[j];
                if (j + 1 < Soil->totalLayers - 1)
                    x4 = (sat[j + 1] - WC[j + 1]) * dzx[j + 1];
                else
                    x4 = 1000.0;    /* a big number to allow gravitational drainage from last layer */
                x2 = x2 < x3 ? x2 : x3;
                x2 = x2 < x4 ? x2 : x4;
                WC[j] = WC[j] - x2 / dzx[j];

                if (j < Soil->totalLayers - 1)
                    WC[j + 1] += x2 / dzx[j + 1];
                else
                    Soil->drainageVol += x2;    /* drainage from last layer */
            }

            WFlux[j + 1] += x2; /* stored for solute transport */
        }

        /* if remainder of infiltration > 0 when Cum_DT = 86400 then saturate
         * layers from top to bottom
         * if there is excess water then drainage
         * note: wonder if this excess should not be considered runoff, check
         * how often it happens and the volume */
        if (!RedistributionFlag && Win > 0.0)
        {
            x1 = 0.0;
            for (j = 0; j < Soil->totalLayers; j++)
            {
                x1 = (sat[j] - WC[j]) * dzx[j];
                if (Win > x1)
                {
                    Win = Win - x1;
                    WC[j] = sat[j];
                }
                else
                {
                    WC[j] = WC[j] + Win / dzx[j];
                    WFlux[j + 1] = WFlux[j + 1] + Win;
                    Win = 0.0;
                    x1 = 0.0;
                    break;
                }
                WFlux[j + 1] = WFlux[j + 1] + x1;
            }
            Soil->drainageVol += Win;   /* keep adding drainage */
        }
    }

    if (SoluteFlag)
    {
        SoluteTransport (Soil->totalLayers, 0.0, 0, &(Soil->NO3Leaching), WFlux, Soil->NO3, Soil->BD, Soil->layerThickness, Soil->Porosity, WCi);
        SoluteTransport (Soil->totalLayers, 5.6, 0, &(Soil->NH4Leaching), WFlux, Soil->NH4, Soil->BD, Soil->layerThickness, Soil->Porosity, WCi);
    }

    /* update water content */
    for (j = 0; j < Soil->totalLayers; j++)
    {
        Soil->waterContent[j] = WC[j];
    }
}

void CurveNumber (SoilStruct *Soil)
{
    int             i;
    double          a = 0.2;
    double          airDryWC[Soil->totalLayers];
    double          WCFraction[Soil->totalLayers];
    double          s;          /* curve number retention factor */
    double          CN;         /* curve number used in computation (depends on CN1, CN2, CN3, and WC) */
    double          CN1;        /* curve number dry condition */
    double          CN2;        /* curve number average condition (input value) */
    double          CN3;        /* curve number wet condition */
    double          slope;      /* slope, % */
    double          factorSlope;    /* correction of 's' (equivalent to CN) by slope as given in APEX Eq. 34 */
    double          cumulativeWeightedFactor;
    double          weightedFactorDepth[Soil->totalLayers];
    double          weightedWCF;
    double          WC_Factor;
    double          Term1;      /* help computation */

    CN2 = Soil->Curve_Number;
    slope = Soil->Percent_Slope;

    for (i = 0; i < Soil->totalLayers; i++)
    {
        airDryWC[i] = Soil->PWP[i] / 3.0;   /* an approximation to air dry */
    }

    cumulativeWeightedFactor = 0.0;

    for (i = 0; i < Soil->totalLayers; i++)
    {
        if (Soil->cumulativeDepth[i] < 0.6 + 1e-6)
        {
            cumulativeWeightedFactor = cumulativeWeightedFactor + 1.0 - (Soil->cumulativeDepth[i] - 0.5 * Soil->layerThickness[i]) / 0.6;
        }
        else
            break;
    }

    weightedWCF = 0.0;

    for (i = 0; i < Soil->totalLayers; i++)
    {
        if (Soil->cumulativeDepth[i] < 0.6 + 1e-6)
        {
            weightedFactorDepth[i] = (1.0 - (Soil->cumulativeDepth[i] - 0.5 * Soil->layerThickness[i]) / 0.6) / cumulativeWeightedFactor;
            WCFraction[i] = (Soil->waterContent[i] - airDryWC[i]) / (Soil->FC[i] - airDryWC[i]);
            weightedWCF += weightedFactorDepth[i] * WCFraction[i];
        }
        else
            break;
    }

    WC_Factor = weightedWCF / (weightedWCF + exp (11.0 - 15.0 * weightedWCF));

    factorSlope = 1.1 - slope / (slope + exp (3.7 + 0.02 * slope));
    CN1 = CN2 / (2.3 - 0.013 * CN2);
    CN3 = CN2 / (0.4 + 0.006 * CN2);

    CN = CN1 + (CN3 - CN1) * WC_Factor;
    s = factorSlope * 254.0 * (100.0 / CN - 1.0);


    if (Soil->infiltrationVol > a * s)
    {
        Term1 = Soil->infiltrationVol - a * s;
        Soil->runoffVol = Term1 * Term1 / (s + Term1);
        Soil->infiltrationVol -= Soil->runoffVol;
    }
}

double K_Sat (double WCATSAT, double WCAT33, double b)
{
    /* kg s / m^3 */
    return (1930.0 * pow (WCATSAT - WCAT33, 3.0 - 1.0 / b) * (1.0 / 3600.0) / 9.81);
}

double AverageHydraulicConductance (double WCSAT, double WCFC, double aep, double b, double SWP1, double SWP2, double ks)
{
    return (ks * (Numerator (aep, b, SWP1) - Numerator (aep, b, SWP2)) / (Denominator (aep, b, SWP1) - Denominator (aep, b, SWP2)));
}

double Numerator (double aep, double b, double SWP)
{
    return (-b * pow (aep / SWP, 2.0 + 4.0 / b) / (4.0 + 2.0 * b));
}

double Denominator (double aep, double b, double SWP)
{
    return (-b * pow (aep / SWP, 1.0 / b));
}
