#include "include/Cycles.h"

void Processes (int y, int doy, int autoNitrogen, CropStruct *Crop, ResidueStruct *Residue, const WeatherStruct *Weather, SoilStruct *Soil, SoilCarbonStruct *SoilCarbon)
{
    double          stage;          /* Fractional phenological stage        */
    double          dailyGrowth;    /* Apparent daily growth rate as it
                                     * includes rizhodeposition             */
    double          N_AbgdConcReq;  /* Maximum concentration in daily growth*/
    double          N_RootConcReq;  
    double          NxAbgd;         /* Maximum concentration of cumulative
                                     * abgd                                 */
    double          NcAbgd;         /* Critical concentration of cumulative
                                     * abgd                                 */
    double          NnAbgd;         /* Minimum concentration of cumulative
                                     * abgd                                 */
    double          NaAbgd;         /* Actual nitrogen concentration in
                                     * aboveground biomass                  */
    double          NxRoot;         /* Maximum nitrogen concentration of
                                     * cumulative root biomass              */

    if (Crop->svRadiationInterception > 0)
    {
        stage = (Crop->svTT_Cumulative - Crop->userEmergenceTT) / (Crop->calculatedMaturityTT - Crop->userEmergenceTT);

        /* compute reference N concentration of biomass and required
         * concentration of new growth (Eulerian) */
        CropNitrogenConcentration (&N_AbgdConcReq, &N_RootConcReq, &NaAbgd, &NxAbgd, &NcAbgd, &NnAbgd, &NxRoot, stage, Crop);

        /* compute nitrogen stress */
        CropNitrogenStress (NaAbgd, NcAbgd, NnAbgd, Crop);

        /* compute growth */
        CropGrowth (y, doy, &dailyGrowth, stage, Crop, Residue, Weather);

        if (dailyGrowth > 0)
        {
            /* compute N demand based on growth and N uptake */
            CropNitrogenUptake (N_AbgdConcReq, N_RootConcReq, NaAbgd, NxAbgd, NxRoot, autoNitrogen, Crop, Soil);

            /* distribute rizhodeposition in soil layers */
            DistributeRootDetritus (y, 0, Crop->svRizhoDailyDeposition, 0, Crop->svN_RizhoDailyDeposition, Soil, Crop, Residue, SoilCarbon);
        }
    }
}

void CropGrowth (int y, int doy, double *DailyGrowth, double Stage, CropStruct *Crop, ResidueStruct *Residue, const WeatherStruct * Weather)
{
    double          ShootPartitioning;      /* fraction of daily growth
                                             * allocated to aboveground
                                             * biomass                  */
    double          RadiationGrowth;
    double          TranspirationGrowth;
    double          UnstressedDailyGrowth;
    double          daytimeVPD;
    double          TUE;
    double          RUE;
    double          PRG;                    /* potential growth based on
                                             * radiation                */
    double          PTG;                    /* potential growth based on
                                             * transpiration            */
    double          RRD = 0.6;              /* root to rizhodeposition
                                             * ratio of biomass allocated
                                             * to roots                 */

    daytimeVPD = 0.66 * SatVP (Weather->tMax[y][doy - 1]) * (1. - Weather->RHmin[y][doy - 1] / 100.);

    if (daytimeVPD < 0.2)
        daytimeVPD = 0.2;

    /* g/MJ solar radiation intercepted, 0.01 converts to Mg/ha */
    RUE = 0.01 * Crop->userRadiationUseEfficiency;  
    TUE = 0.01 * Crop->userTranspirationUseEfficiency * pow (daytimeVPD, -0.59);
    ShootPartitioning = ShootBiomassPartitioning (Stage, Crop->userShootPartitionInitial, Crop->userShootPartitionFinal);

    /* Unstressed growth of aboveground biomass
     * might be used to calculate N demand */
    PRG = Crop->svRadiationInterception * Weather->solarRadiation[y][doy - 1] * RUE;
    PTG = Crop->svTranspirationPotential * TUE;
    UnstressedDailyGrowth = PRG < PTG ? PRG : PTG;
    Crop->svUnstressedShootDailyGrowth = UnstressedDailyGrowth * ShootPartitioning;
    Crop->svUnstressedRootDailyGrowth = UnstressedDailyGrowth * (1. - ShootPartitioning) * RRD;
    Crop->svShootUnstressed += Crop->svUnstressedShootDailyGrowth;

    /* Actual growth rate */
    RadiationGrowth = Crop->svRadiationInterception * Weather->solarRadiation[y][doy - 1] * RUE * (1. - pow (Crop->svN_StressFactor, 1.0));
    /* This coefficient is higher to represent drop in WUE with N stress
     * (radiation limits more) */
    TranspirationGrowth = Crop->svTranspiration * TUE * (1. - pow (Crop->svN_StressFactor, 1.25));  
    *DailyGrowth = RadiationGrowth < TranspirationGrowth ? RadiationGrowth : TranspirationGrowth;

    /* Update biomass pools */
    Crop->svShootDailyGrowth = *DailyGrowth * ShootPartitioning;
    Crop->svRootDailyGrowth = *DailyGrowth * (1. - ShootPartitioning) * RRD;
    Crop->svRizhoDailyDeposition = *DailyGrowth * (1. - ShootPartitioning) * (1. - RRD);
    Crop->svBiomass += *DailyGrowth - Crop->svRizhoDailyDeposition;
    Crop->svShoot += Crop->svShootDailyGrowth;
    Crop->svRoot += Crop->svRootDailyGrowth;
    Crop->svRizho += Crop->svRizhoDailyDeposition;

    if (Crop->svTT_Cumulative > Crop->calculatedFloweringTT)
        Crop->svPostFloweringShootBiomass += Crop->svShootDailyGrowth;
}

void CropNitrogenConcentration (double *N_AbgdConcReq, double *N_RootConcReq, double *NaAbgd, double *NxAbgd, double *NcAbgd, double *NnAbgd, double *NxRoot, double Stage, const CropStruct *Crop)
{
    /*
     * This sub may need to be generic for N, P, S
     * Root nitrogen concentration depends only on phenology
     * In this subroutine variables with names Nxxxx stand for nitrogen
     * CONCENTRATION (Mg N / Mg Biomass)
     * N_AbgdConcReq and N_RootConcReq = nitrogen concentration requirements,
     * passed ByRef and returned to caller
     * NaAbgd = nitrogen concentration in aboveground biomass
     * NxAbgd, NcAbgd, NnAbgd = maximum, critical and minimum nitrogen
     * concentration in aboveground biomass
     * NcAbgd, NnAbgd aboveground biomass maximum, critical, and minimum N
     * concentration in biomass
     */

    double          BTNx;           /* Threshold abgd biomass level after
                                     * which N maximum / critical / minimum
                                     * Conc dilution curve starts, Mg/ha    */
    double          BTNc;
    double          BTNn;           
    double          NcEmergence;    /* Critical, minimum nitrogen
                                     * concentration at crop emergence,
                                     * maximum is passed                    */
    double          NnEmergence;    
    double          Ax, Ac, An;     /* biomass dilution curve constants */
    const double    Root_NxEmergence = 0.02;
    const double    R1 = 1.;        /* root half constant for phenology-based
                                     * dilution                             */
    const double    R2 = 0.33;      /* root power of equation for phenology-
                                     * based dilution                       */

    if (Crop->userC3orC4)
    {
        /* C3 */
        BTNx = 1.5;
        BTNc = 1.5;
        BTNn = 0.5;
    }
    else
    {
        /* C4 */
        BTNx = 1.0;
        BTNc = 1.0;
        BTNn = 0.5;
    }

    /* Compute current N concentration in aboveground biomass
     * (this is reported back) */
    if (Crop->svN_Shoot > 0.)
        *NaAbgd = Crop->svN_Shoot / Crop->svShoot;
    else
        *NaAbgd = Crop->userNMaxConcentration;

    /* if N concentration drops below critical N, assing critical N up to 120
     * Cday past emergence to account for seed N
     * this calculation done in nitrogen uptake subroutine */
    NcEmergence = Crop->userNMaxConcentration * 0.62;   /* Scaling from Nx to
                                                         * Ncritical        */
    NnEmergence = Crop->userNMaxConcentration * 0.45;   /* Scaling from Nx to
                                                         * Nminimum */

    Ax = Crop->userNMaxConcentration / (pow (BTNx, -Crop->userNDilutionSlope));
    Ac = NcEmergence / (pow (BTNc, -Crop->userNDilutionSlope));
    An = NnEmergence / (pow (BTNn, -Crop->userNDilutionSlope));

    *NxAbgd = Ax * pow (Crop->svShootUnstressed, -Crop->userNDilutionSlope);
    *NxAbgd = *NxAbgd < Crop->userNMaxConcentration ? *NxAbgd : Crop->userNMaxConcentration;
    *NcAbgd = Ac * pow (Crop->svShootUnstressed, -Crop->userNDilutionSlope);
    *NcAbgd = *NcAbgd < NcEmergence ? *NcAbgd : NcEmergence;
    *NnAbgd = An * pow (Crop->svShootUnstressed, -Crop->userNDilutionSlope);
    *NnAbgd = *NnAbgd < NnEmergence ? *NnAbgd : NnEmergence;

    /* compute abgd and root N concentration of new growth */
    /* based on biomass y = a*x^(-b); y'= -(a*b-a)/x^b
     * where x = abgd biomass */
    if (Crop->svShoot < BTNx)
        *N_AbgdConcReq = Crop->userNMaxConcentration;

    else
        *N_AbgdConcReq = (1. - Crop->userNDilutionSlope) * Ax * pow (Crop->svShootUnstressed, -Crop->userNDilutionSlope);

    *N_RootConcReq = Root_NxEmergence / (1. + pow (Stage / R1, R2));
    *NxRoot = *N_RootConcReq;   /* in this case the two have the same value */
}

void CropNitrogenStress (double NaAbgd, double NcAbgd, double NnAbgd, CropStruct *Crop)
{
    double          tempStress;
    double          factor = 0.67;      /* weigthing of tempStress in
                                         * determining today's plant stress */

    /* Compute stress based on abgd nitrogen concentration only */
    if (Crop->svShoot > 0.)
    {
        if (NaAbgd > NcAbgd)
            tempStress = 0.;
        else if (NaAbgd < NnAbgd)
            tempStress = 1. - 0.001;
        else
            tempStress = 1. - (NaAbgd - NnAbgd) / (NcAbgd - NnAbgd);
    }
    else
        tempStress = 0.;

    Crop->svN_StressFactor = factor * tempStress + (1. - factor) * Crop->svN_StressFactor;

    /* calculating cumulative nitrogen stress */
    if (Crop->svTT_Cumulative < Crop->calculatedMaturityTT)
        Crop->svN_StressCumulative += Crop->svN_StressFactor * Crop->svTT_Daily / (Crop->calculatedMaturityTT - Crop->userEmergenceTT);
}

void CropNitrogenUptake (double N_AbgdConcReq, double N_RootConcReq, double NaAbgd, double NxAbgd, double NxRoot, int autoNitrogen, CropStruct *Crop, SoilStruct *Soil)
{
    int             i;
    double          N_CropDemand;
    double          N_ReqAbgdGrowth;                /* N mass required to
                                                     * satisfy abgd growth
                                                     * based on luxury
                                                     * consumption          */
    double          N_ReqRootGrowth;                /* N mass required to
                                                     * satisfy root growth
                                                     * based on luxury
                                                     * consumption          */
    double          N_ReqRhizodeposition;           /* N mass required to
                                                     * satisfy
                                                     * rizhodeposition      */
    double          N_AbgdConc, N_RootConc;         /* N concentration in
                                                     * biomass              */
    double          UptakeFactor[Soil->totalLayers];/* factor used to increase
                                                     * soil supply in top
                                                     * layers and to decrease
                                                     * it when crop is
                                                     * maturing             */
    double          NO3Uptake[Soil->totalLayers];   /* array passed to Sub
                                                     * PotentialSoluteUptake*/
    double          NH4Uptake[Soil->totalLayers];   /* array passed to Sub
                                                     * PotentialSoluteUptake*/
    double          NO3supply, NH4Supply;           /* NO3 and NH4 cumulative
                                                     * supply through the
                                                     * rooting depth        */
    double          N_SoilSupply;                   /* NO3 and NH4 cumulative
                                                     * supply               */
    double          N_Uptake;                       /* NO3 and NH4 total N
                                                     * uptake               */
    double          N_FractionalSatisfiedDemand;
    double          N_FractionalSatisfiedSupply;
    double          Fraction_NO3, Fraction_NH4;     /* auxiliar variables to
                                                     * update NO3 and NH4
                                                     * pools                */
    double          N_Surplus;                      /* variable used to remove
                                                     * excess N in biomass due
                                                     * to dilution          */
    double          NInitial = 0.;
    double          NFinal = 0.;
    double          Nfixation = 0.;
    double          Nauto = 0.;
    double          Nratio;

    N_CropDemand = 0.;
    N_Surplus = 0.;

    /* calculate N demand for today's growth
     * (assume no deficit demand to be satisfied - testing) */
    N_ReqAbgdGrowth = Crop->svShootDailyGrowth * N_AbgdConcReq;
    N_ReqRootGrowth = Crop->svRootDailyGrowth * N_RootConcReq;
    N_ReqRhizodeposition = Crop->svRizhoDailyDeposition * N_RootConcReq;
    N_CropDemand = N_ReqAbgdGrowth + N_ReqRootGrowth + N_ReqRhizodeposition;

    /* PotentialSoluteUptakeOption2 */
    /* Mg solute/ha */
    PotentialSoluteUptakeOption2 (&NO3supply, NO3Uptake, 0, Soil->totalLayers, Soil->BD, Soil->layerThickness, Soil->waterUptake, Soil->NO3, Soil->waterContent);
    PotentialSoluteUptakeOption2 (&NH4Supply, NH4Uptake, 5.6, Soil->totalLayers, Soil->BD, Soil->layerThickness, Soil->waterUptake, Soil->NH4, Soil->waterContent);

    /* calculate actual N uptake and update plant N concentrations */
    N_SoilSupply = (NO3supply + NH4Supply);

    //N_Uptake = Math.Min(N_SoilSupply, N_CropDemand)
    Nratio = N_SoilSupply / N_CropDemand;

    //N_Uptake = N_CropDemand * (1. - exp(-(Nratio + 0.2 * Nratio * Nratio)));
    N_Uptake = N_CropDemand * (1. - exp (-Nratio));
    N_FractionalSatisfiedDemand = N_Uptake / N_CropDemand;
    N_FractionalSatisfiedSupply = N_Uptake / N_SoilSupply;

    /* update soil NO3 and NH4 pools */
    for (i = 0; i < Soil->totalLayers; i++)
    {
        Soil->NO3[i] -= N_FractionalSatisfiedSupply * NO3Uptake[i];
        Soil->NH4[i] -= N_FractionalSatisfiedSupply * NH4Uptake[i];
    }

    /* split the code below in update pools surrogate for removal from seed
     * nitrogen fixation autofertilization */
    Crop->svN_Shoot += N_FractionalSatisfiedDemand * N_ReqAbgdGrowth;
    Crop->svN_Root += N_FractionalSatisfiedDemand * N_ReqRootGrowth;
    Crop->svN_RizhoDailyDeposition = N_FractionalSatisfiedDemand * N_ReqRhizodeposition;
    NaAbgd = Crop->svN_Shoot / Crop->svShoot;

    /* legume condition
     * auto nitrogen for no N limitation
     * auto nitrogen to consider N seed storage (auto nitrogen if stressed and
     * 1.5 leaf stage, approx.) */
    if (Crop->userLegume)
    {
        /* N fixation */
        if (N_FractionalSatisfiedDemand < 0.9)
        {
            Crop->svN_Shoot += N_ReqAbgdGrowth * (0.9 - N_FractionalSatisfiedDemand);
            Crop->svN_Root += N_ReqRootGrowth * (0.9 - N_FractionalSatisfiedDemand);
            Crop->svN_RizhoDailyDeposition += N_ReqRhizodeposition * (0.9 - N_FractionalSatisfiedDemand);
            Crop->svN_Fixation += (0.9 - N_FractionalSatisfiedDemand) * (N_ReqAbgdGrowth + N_ReqRootGrowth + N_ReqRhizodeposition);
            Nfixation = (0.9 - N_FractionalSatisfiedDemand) * (N_ReqAbgdGrowth + N_ReqRootGrowth + N_ReqRhizodeposition);
        }
    }
    else if (autoNitrogen)
    {
        if (N_FractionalSatisfiedDemand < 0.65)
        {
            Crop->svN_Shoot += N_ReqAbgdGrowth * (0.65 - N_FractionalSatisfiedDemand);
            Crop->svN_Root += N_ReqRootGrowth * (0.65 - N_FractionalSatisfiedDemand);
            Crop->svN_RizhoDailyDeposition += N_ReqRhizodeposition * (0.65 - N_FractionalSatisfiedDemand);
            Crop->svN_AutoAdded += (0.65 - N_FractionalSatisfiedDemand) * (N_ReqAbgdGrowth + N_ReqRootGrowth + N_ReqRhizodeposition);
            //Nauto = (0.65 - N_FractionalSatisfiedDemand) * (N_ReqAbgdGrowth + N_ReqRootGrowth + N_ReqRhizodeposition)
        }
    }
    else if (Crop->svTT_Cumulative < 2.5 * Crop->userEmergenceTT && NaAbgd < 0.65 * NxAbgd)
    {
        if (N_FractionalSatisfiedDemand < 0.65)
        {
            Crop->svN_Shoot += N_ReqAbgdGrowth * (0.65 - N_FractionalSatisfiedDemand);
            Crop->svN_Root += N_ReqRootGrowth * (0.65 - N_FractionalSatisfiedDemand);
            Crop->svN_RizhoDailyDeposition += N_ReqRhizodeposition * (0.65 - N_FractionalSatisfiedDemand);
            Crop->svN_AutoAdded += (0.65 - N_FractionalSatisfiedDemand) * (N_ReqAbgdGrowth + N_ReqRootGrowth + N_ReqRhizodeposition);
        }
    }

    Crop->svN_Rhizo += Crop->svN_RizhoDailyDeposition;
    N_AbgdConc = Crop->svN_Shoot / Crop->svShoot;
    N_RootConc = Crop->svN_Root / Crop->svRoot;

    /* trim N above maximum and return to soil layer 1 as nitrate */
    if (N_AbgdConc > NxAbgd)
    {
        N_Surplus = 0.;
        N_Surplus = Crop->svShoot * (N_AbgdConc - NxAbgd);
        Soil->NO3[0] += N_Surplus;
        Crop->svN_Shoot = NxAbgd * Crop->svShoot;
    }

    if (N_RootConc > NxRoot)
    {
        N_Surplus = 0.;
        N_Surplus = Crop->svRoot * (N_RootConc - NxRoot);
        Soil->NO3[0] += N_Surplus;
        Crop->svN_Root = NxRoot * Crop->svRoot;
    }
}

void PotentialSoluteUptakeOption2 (double *SoluteSupply, double *SoluteUptake, double Kd, int totalLayers, const double *BD, const double *dz, const double *WaterUptake, const double *Solute, const double *WC)
{
    /* This sub considers the solute as "available" for uptake if in solution
     * and in a layer with positive water uptake */
    /* 2014 02 20 a function was added to limit uptake if concentration is too
     * low, this function only valid for NO3 and NH4 */
    int             i;
    double          soluteConc;             /* kg solute / kg H2O           */
    double          soilBufferPower;        /* dimensionless                */
    double          layerPotentialUptake;   /* kg solute/m2/day             */
    double          totalPotentialUptake;   /* kg solute/m2/day             */
    double          ppmSolute;              /* solute concentration in parts
                                             * per million                  */
    double          factorSoluteUptake;     /* factor the affects potential
                                             * uptake based on ppm, current
                                             * function valid for NO3 and
                                             * NH4                          */
    double          temp1, temp2;

    /* soil buffer power is dimensionless
     * soil bulk density must be in g/cm3 and
     * slope of the adsortion isotherm (Kd) is in cm3/g */
    totalPotentialUptake = 0.;

    for (i = 0; i < totalLayers; i++)
    {
        soluteConc = 0.;

        if (WaterUptake[i] > 0. && Solute[i] > 0.)
        {
            ppmSolute = 100. * Solute[i] / (dz[i] * BD[i]); /* calculate solute concentration in ppm */
            factorSoluteUptake = 1. - exp (-0.1 * ppmSolute);
            soilBufferPower = Kd * BD[i] + WC[i];
            soluteConc = Solute[i] / 10. / (dz[i] * soilBufferPower * WATER_DENSITY);   /* 1/10 converts from Mg/ha to kg/m2 */
            temp1 = WATER_DENSITY * WC[i] * dz[i] * soluteConc * factorSoluteUptake;
            temp2 = WaterUptake[i] * 0.0006;    /* max allowed concentration in uptake, about 0.6 g N / kg H2O */
            layerPotentialUptake = temp1 < temp2 ? temp1 : temp2;
            layerPotentialUptake *= 10.;    /* 10 converts from kg/m2 to Mg/ha */
        }
        else
            layerPotentialUptake = 0.;

        totalPotentialUptake += layerPotentialUptake;
        SoluteUptake[i] = layerPotentialUptake;
    }

    *SoluteSupply = totalPotentialUptake;

}

double ShootBiomassPartitioning (double Stage, double Po, double Pf)
{
    double          P1 = 0.4;   /* stage at which partitioning is half way
                                 * between Po and Pf                        */
    double          P2 = 4.;    /* curvature factor (not recommended to be
                                 * available to user)                       */

    return (Po + (Pf - Po) / (1. + pow ((Stage + 0.0001) / P1, -P2)));
}


   //void UptakeCorrectionFactor(ByVal totalLayers As Integer, ByVal Thickness() As Double, ByVal RootDepth As Double, _
   //                                       ByRef UCF() As Double, ByVal TT As Double, ByVal TT_Flowering As Double, ByVal TT_Maturity As Double)
   //
   //        'this function enhances uptake above mass flow with a decreasing effect of depth 
   //        'and accounts for root senescence after flowering
   //
   //        'UEF = uptake correction factor by layer
   //        Dim RootSenilityFactor As Double
   //        Dim TT1 As Double
   //        Dim Depth As Double = 0
   //        Dim LayerMidpoint As Double = 0
   //        Dim C1 As Double
   //
   //        RootSenilityFactor = 1
   //
   //        If TT > TT_Flowering Then
   //            TT1 = (TT - TT_Flowering) / (TT_Maturity - TT_Flowering)
   //            RootSenilityFactor = 1 ' 1 / (1 + (TT1 / 0.45) ^ 2)
   //        End If
   //
   //        Thickness(totalLayers + 1) = 0
   //        C1 = 0.5 * (1 - Exp(-RootDepth))
   //
   //        For i As Integer = 1 To totalLayers
   //            LayerMidpoint = Depth + 0.5 * Thickness(i)
   //            Depth += Thickness(i)
   //            UCF(i) = (1 + 4 / (1 + (LayerMidpoint / C1) ^ 3)) * RootSenilityFactor
   //            If Depth > RootDepth Then Exit For
   //        Next
   //
   //    End Sub
   //
void RadiationInterception (int y, int doy, CropStruct *Crop)
{
    double          Fractional_TT;
    double          Delta_Fractional_TT;
    double          Rate_Crop_Interception;
    double          Delta_Crop_Interception;
    double          Reserves_Use_Allowance;         /* Allows LAI expansion
                                                     * larger than daily
                                                     * growth * SLA         */
    double          Delta_Crop_Interception_Growth; /* crop interception
                                                     * expansion based on daily growth and SLA */
    double          Compensatory_Expansion;         /* if crop expansion is limited by past stress, it is allowed to compensate, but still limited by daily growth */
    double          Rate_Root_Depth;
    double          WSF;                            /* water stress factor - copied from stored value; 1 = no stress */
    double          NSF;                            /* nitroge stress factor - copied from stored value; 1 = no stress */
    double          Sx, Sn, SF;                     /* the minimum and maximum of the two stresses, and the compounded stress factor
                                                     * =0.96/(1+EXP(4-20*AB179/$AB$249)+EXP(-15+16*AB179/$AB$249))
                                                     * note: these parameters need to be crop dependent e.g. canola radiation interception progresses faster than wheat
                                                     * current setting is good for wheat, barley, corn, sorghum */
    const double    a = 6.;                         /* 5, 7 crop radiation interception parameter modified logistic curve */
    const double    b = 20.;                        /*  crop radiation interception parameter modified logistic curve */
    const double    c = 15.;                        /*  crop radiation interception parameter modified logistic curve */
    const double    d = 16.;                        /* crop radiation interception parameter modified logistic curve [reduce the number increase senescence] */
    const double    e = 5.;                         /* root parameter logistic curve */
    const double    f = 15.;                        /* root parameter logistic curve */
    const double    rde = 0.3;                      /* m, root depth at emergence */

    Fractional_TT = (Crop->svTT_Cumulative - 1. * Crop->svTT_Daily) / (Crop->calculatedMaturityTT - Crop->userEmergenceTT);
    Delta_Fractional_TT = Crop->svTT_Daily / (Crop->calculatedMaturityTT - Crop->userEmergenceTT);

    if (Crop->svTT_Cumulative < Crop->calculatedMaturityTT)
    {
        if (Crop->svTT_Cumulative < Crop->userEmergenceTT)
        {
            Crop->svRadiationInterception = 0.;
            Crop->svRootingDepth = rde * Crop->svTT_Cumulative / Crop->userEmergenceTT;
        }
        else
        {
            Rate_Crop_Interception = (b * exp (a - b * Fractional_TT) - c * exp (c * Fractional_TT - d)) / pow (1. + exp (a - b * Fractional_TT) + exp (c * Fractional_TT - d), 2);
            Rate_Root_Depth = f * exp (e - f * Fractional_TT) / pow (1. + exp (e - f * Fractional_TT), 2);

            /* compute stress effect on canopy expansion or senescence */
            WSF = 1. - Crop->svWaterStressFactor;
            NSF = pow (1. - Crop->svN_StressFactor, 3);

            /* select max and min stress */
            if (WSF > NSF)
            {
                Sx = WSF;
                Sn = NSF;
            }
            else
            {
                Sx = NSF;
                Sn = WSF;
            }

            /* stress reduces rate of increase of canopy interception or accelerates senescence */
            if (Rate_Crop_Interception > 0)
                SF = 1. - pow (1 - Sn, Sx);
            else
                SF = 1. + pow (1. - Sn, 3 * Sx);

            /* compute compensatory expansion factor and reserves use allowance */
            if (Rate_Crop_Interception > 0)
            {
                Compensatory_Expansion = sqrt ((Crop->userMaximumSoilCoverage / (1. + exp (a - b * Fractional_TT) + exp (-c + d * Fractional_TT))) / Crop->svRadiationInterception);
                Reserves_Use_Allowance = 1. + 3. / (1. + pow (Crop->svRadiationInterception / 0.15, 4));
            }
            else
            {
                Compensatory_Expansion = 1.;
                Reserves_Use_Allowance = 1.;
            }

            Delta_Crop_Interception = Crop->userMaximumSoilCoverage * Rate_Crop_Interception * Delta_Fractional_TT * SF * Compensatory_Expansion;
            Delta_Crop_Interception_Growth = Reserves_Use_Allowance * 0.75 * (1. - Crop->svRadiationInterception) * (0.1 * Crop->svShootDailyGrowth * 25.);

            if (Delta_Crop_Interception > Delta_Crop_Interception_Growth)
                Delta_Crop_Interception = Delta_Crop_Interception_Growth;

            Crop->svRadiationInterception += Delta_Crop_Interception;

            /* just in case stress accelerates senescence too much - not sure it is needed */
            /* just in case cold damage defoliates too much - not sure it is needed */
            if (Crop->svRadiationInterception < 0.001)
                Crop->svRadiationInterception = 0.001;

            if (Crop->svRadiationInterception > 0.98)
                Crop->svRadiationInterception = 0.98;

            Crop->svRootingDepth += (Crop->userMaximumRootingDepth - rde) * Rate_Root_Depth * Delta_Fractional_TT;

            if (Crop->svRootingDepth > Crop->userMaximumRootingDepth)
                Crop->svRootingDepth = Crop->userMaximumRootingDepth;
        }   /* if Crop->svTT_Cumulative >= Crop->userEmergenceTT */
    }   /* if Crop->svTT_Cumulative < Crop->calculatedMaturityTT */
}

void Phenology (int y, int doy, const WeatherStruct *Weather, CropStruct *Crop)
{
    Crop->svTT_Daily = 0.5 * (ThermalTime (Crop->userTemperatureBase, Crop->userTemperatureOptimum, Crop->userTemperatureMaximum, Weather->tMin[y][doy - 1])
                        + ThermalTime (Crop->userTemperatureBase, Crop->userTemperatureOptimum, Crop->userTemperatureMaximum, Weather->tMax[y][doy - 1]));
    Crop->svTT_Cumulative += Crop->svTT_Daily;
}

void ComputeColdDamage (int y, int doy, CropStruct *Crop, const WeatherStruct *Weather, const SnowStruct *Snow, ResidueStruct *Residue)
{
    double          Min_Temperature;
    double          Cold_Damage;            /* fraction of interception
                                             * decreased by cold damage     */
    double          Crop_Tn;                /* minimum temperature for cold
                                             * damage                       */
    double          Crop_Tth;               /* threshold temperature for
                                             * cold damage                  */
    double          residueMass;
    double          Residue_N_Mass;
    double          Phenology_Delay_Factor; /* the more advanced the cycle the
                                             * lesser the effect on cold
                                             * damage on delaying phenology */

    Min_Temperature = Weather->tMin[y][doy - 1];

    Crop_Tn = Crop->userColdDamageMinTemperature;
    Crop_Tth = Crop->userColdDamageThresholdTemperature;
    Cold_Damage = ColdDamage (Min_Temperature, Crop_Tn, Crop_Tth) * (1. - Snow->snowCover);

    if (Crop->userAnnual)
        Phenology_Delay_Factor = 1. / (1. + pow ((Crop->svTT_Cumulative / Crop->calculatedFloweringTT) / 0.15, 4));
    else
        Phenology_Delay_Factor = 1.;

    residueMass = Crop->svShoot * pow (Cold_Damage, 3);
    Residue_N_Mass = Crop->svN_Shoot * pow (Cold_Damage, 3);

    Crop->svBiomass -= residueMass;
    Crop->svShoot -= residueMass;
    Crop->svN_Shoot -= Residue_N_Mass;

    if (Crop->svTT_Cumulative > Crop->userEmergenceTT)
        Crop->svTT_Cumulative = Crop->userEmergenceTT + (Crop->svTT_Cumulative - Crop->userEmergenceTT) * (1. - Phenology_Delay_Factor * pow (Cold_Damage, 2.5));

    Crop->svRadiationInterception = Crop->svRadiationInterception * (1. - pow (Cold_Damage, 3));

    Residue->stanResidueMass += residueMass * Crop->userFractionResidueStanding;
    Residue->flatResidueMass += residueMass * (1. - Crop->userFractionResidueStanding);
    Residue->stanResidueN += Residue_N_Mass * Crop->userFractionResidueStanding;
    Residue->flatResidueN += Residue_N_Mass * (1. - Crop->userFractionResidueStanding);
    /* Assume 33% residue moisture at harvest */
    Residue->stanResidueWater += residueMass * Crop->userFractionResidueStanding / 10. * 0.5;   
    Residue->flatResidueWater += residueMass * (1 - Crop->userFractionResidueStanding) / 10. * 0.5; 

    /* yearly output variables */
    Residue->yearResidueBiomass += residueMass;

    /* season outputs */
    Crop->rcResidueBiomass += residueMass;
    Crop->rcBiomass += residueMass;
}

double ColdDamage (const double T, const double Crop_Tn, const double Crop_Tth)
{
    double          damage;

    if (T <= Crop_Tn)
        damage = 0.99;
    else if (T > Crop_Tn && T <= Crop_Tth)
        damage = 1. - (T - Crop_Tn) / (Crop_Tth - Crop_Tn);
    else
        damage = 0.;

    return (damage);
}
