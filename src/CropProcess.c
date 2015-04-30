#include "Cycles.h"

void Processes (int y, int doy, int autoNitrogen, CommunityStruct *Community, ResidueStruct *Residue, const WeatherStruct *Weather, SoilStruct *Soil, SoilCarbonStruct *SoilCarbon)
{
    /* 
     * -----------------------------------------------------------------------
     * LOCAL VARIABLES
     *
     * Variable             Type        Description
     * ==========           ==========  ====================
     * stage                double      Fractional phenological stage
     * dailyGrowth          double      Apparent daily growth rate as it
     *                                    includes rizhodeposition
     * N_AbgdConcReq        double      Maximum above ground concentration in
     *                                    daily growth
     * N_RootConcReq        double      Maximum root concentration in daily
     *                                    growth
     * NxAbgd               double      Maximum concentration of cumulative
     *                                    above ground
     * NnAbgd               double      Critical concentration of cumulative
     *                                    above ground
     * NaAbgd               double      Actual nitrogen concentration in above
     *                                    ground biomass
     * NxRoot               double      Maximum nitrogen concentration of
     *                                    cumulative root biomass
     */
    double          stage;
    double          dailyGrowth = 0.0;
    double          N_AbgdConcReq = 0.0;
    double          N_RootConcReq = 0.0;
    double          NxAbgd = 0.0;
    double          NcAbgd = 0.0;
    double          NnAbgd = 0.0;
    double          NaAbgd = 0.0;
    double          NxRoot = 0.0;
    int             i;
    CropStruct     *Crop;

    for (i = 0; i < Community->NumCrop; i++)
    {
        Crop = &Community->Crop[i];

        if (Crop->stageGrowth > NO_CROP)
        {
            if (Crop->svRadiationInterception > 0.0)
            {
                stage = (Crop->svTT_Cumulative - Crop->userEmergenceTT) / (Crop->calculatedMaturityTT - Crop->userEmergenceTT);

                /* Compute reference N concentration of biomass and required
                 * concentration of new growth (Eulerian) */
                CropNitrogenConcentration (&N_AbgdConcReq, &N_RootConcReq, &NaAbgd, &NxAbgd, &NcAbgd, &NnAbgd, &NxRoot, stage, Crop);

                /* Compute nitrogen stress */
                CropNitrogenStress (NaAbgd, NcAbgd, NnAbgd, Crop);

                /* Compute growth */
                CropGrowth (y, doy, &dailyGrowth, stage, Crop, Residue, Weather);

                if (dailyGrowth > 0.0)
                {
                    /* Compute N demand based on growth and N uptake */
                    CropNitrogenUptake (N_AbgdConcReq, N_RootConcReq, NaAbgd, NxAbgd, NxRoot, autoNitrogen, Crop, Soil);

                    /* Distribute rizhodeposition in soil layers */
                    DistributeRootDetritus (y, 0.0, Crop->svRizhoDailyDeposition, 0.0, Crop->svN_RizhoDailyDeposition, Soil, Crop, Residue, SoilCarbon);
                }
            }
        }
    }
}

void CropGrowth (int y, int doy, double *DailyGrowth, double Stage, CropStruct *Crop, ResidueStruct *Residue, const WeatherStruct *Weather)
{
    /* 
     * LOCAL VARIABLES
     *
     * Variable             Type        Description
     * ==========           ==========  ====================
     * ShootPartitioning    double      Fraction of daily growth allocated to
     *                                    aboveground biomass
     * RadiationGrowth      double
     * TranspirationGrowth  double
     * UnstressedGrowth double
     * daytimeVPD           double
     * TUE                  double
     * RUE                  double
     * PRG                  double      Potential growth based on radiation
     * PTG                  double      Potential growth based on
     *                                    transpiration
     * RRD                  double      Root to rizhodeposition ratio of
     *                                    biomass allocated to roots
     */
    double          ShootPartitioning;
    double          RadiationGrowth;
    double          TranspirationGrowth;
    double          UnstressedGrowth;
    double          daytimeVPD;
    double          TUE;
    double          RUE;
    double          PRG;
    double          PTG;
    const double    RRD = 0.6;

    daytimeVPD = 0.66 * SatVP (Weather->tMax[y][doy - 1]) * (1.0 - Weather->RHmin[y][doy - 1] / 100.0);

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
    UnstressedGrowth = PRG < PTG ? PRG : PTG;
    Crop->svUnstressedShootDailyGrowth = UnstressedGrowth * ShootPartitioning;
    Crop->svUnstressedRootDailyGrowth = UnstressedGrowth * (1.0 - ShootPartitioning) * RRD;
    Crop->svShootUnstressed += Crop->svUnstressedShootDailyGrowth;

    /* Actual growth rate */
    RadiationGrowth = Crop->svRadiationInterception * Weather->solarRadiation[y][doy - 1] * RUE * (1.0 - pow (Crop->svN_StressFactor, 1.0));
    /* This coefficient is higher to represent drop in WUE with N stress
     * (radiation limits more) */
    TranspirationGrowth = Crop->svTranspiration * TUE * (1.0 - pow (Crop->svN_StressFactor, 1.25));
    *DailyGrowth = RadiationGrowth < TranspirationGrowth ? RadiationGrowth : TranspirationGrowth;

    /* Update biomass pools */
    Crop->svShootDailyGrowth = *DailyGrowth * ShootPartitioning;
    Crop->svRootDailyGrowth = *DailyGrowth * (1.0 - ShootPartitioning) * RRD;
    Crop->svRizhoDailyDeposition = *DailyGrowth * (1.0 - ShootPartitioning) * (1.0 - RRD);
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
     * -----------------------------------------------------------------------
     * LOCAL VARIABLES
     *
     * Variable             Type        Description
     * ==========           ==========  ====================
     * BTNx		    double      Threshold abgd biomass level after
     *					  which N maximum Conc dilution curve
     *					  starts (Mg/ha)
     * BTNx		    double      Threshold abgd biomass level after
     *					  which N critical Conc dilution curve
     *					  starts (Mg/ha)
     * BTNx		    double      Threshold abgd biomass level after
     *					  which N minimum Conc dilution curve
     *					  starts (Mg/ha)
     * NcEmergence	    double	Critical nitrogen concentration at
     *					  crop emergence, maximum is passed
     * NnEmergence	    double	Minimum nitrogen concentration at crop
     *					  emergence, maximum is passed
     * Ax		    double	Biomass dilution curve constants
     * Ac		    double	Biomass dilution curve constants
     * An		    double	Biomass dilution curve constants
     * Root_NxEmergence	    double
     * R1		    double	Root half constant for phenology-based
     *					  dilution
     * R2		    double	Root power of equation for phenology-
     *					  based dilution
     */
    double          BTNx;
    double          BTNc;
    double          BTNn;
    double          NcEmergence;    
    double          NnEmergence;
    double          Ax, Ac, An; 
    const double    Root_NxEmergence = 0.02;
    const double    R1 = 1.0;
    const double    R2 = 0.33;

    if (Crop->userC3orC4)
    {	/* C3 */
        BTNx = 1.5;
        BTNc = 1.5;
        BTNn = 0.5;
    }
    else
    {	/* C4 */
        BTNx = 1.0;
        BTNc = 1.0;
        BTNn = 0.5;
    }

    /* Compute current N concentration in aboveground biomass
     * (this is reported back) */
    if (Crop->svN_Shoot > 0.0)
        *NaAbgd = Crop->svN_Shoot / Crop->svShoot;
    else
        *NaAbgd = Crop->userNMaxConcentration;
    /* if N concentration drops below critical N, assing critical N up to 120
     * Cday past emergence to account for seed N
     * This calculation done in nitrogen uptake subroutine */

    /* Scaling from Nx to Ncritical */
    NcEmergence = Crop->userNMaxConcentration * 0.62;
    /* Scaling from Nx to Nminimum */
    NnEmergence = Crop->userNMaxConcentration * 0.45;

    Ax = Crop->userNMaxConcentration / (pow (BTNx, -Crop->userNDilutionSlope));
    Ac = NcEmergence / (pow (BTNc, -Crop->userNDilutionSlope));
    An = NnEmergence / (pow (BTNn, -Crop->userNDilutionSlope));

    *NxAbgd = Ax * pow (Crop->svShootUnstressed, -Crop->userNDilutionSlope);
    *NxAbgd = *NxAbgd < Crop->userNMaxConcentration ? *NxAbgd : Crop->userNMaxConcentration;
    *NcAbgd = Ac * pow (Crop->svShootUnstressed, -Crop->userNDilutionSlope);
    *NcAbgd = *NcAbgd < NcEmergence ? *NcAbgd : NcEmergence;
    *NnAbgd = An * pow (Crop->svShootUnstressed, -Crop->userNDilutionSlope);
    *NnAbgd = *NnAbgd < NnEmergence ? *NnAbgd : NnEmergence;

    /* Compute abgd and root N concentration of new growth based on biomass
     * y = a * x^(-b); y' =  -(a * b - a) / x^b, where x = abgd biomass */
    if (Crop->svShoot < BTNx)
        *N_AbgdConcReq = Crop->userNMaxConcentration;
    else
        *N_AbgdConcReq = (1.0 - Crop->userNDilutionSlope) * Ax * pow (Crop->svShootUnstressed, -Crop->userNDilutionSlope);

    *N_RootConcReq = Root_NxEmergence / (1.0 + pow (Stage / R1, R2));
    *NxRoot = *N_RootConcReq;   /* in this case the two have the same value */
}

void CropNitrogenStress (double NaAbgd, double NcAbgd, double NnAbgd, CropStruct *Crop)
{
    /* 
     * -----------------------------------------------------------------------
     * LOCAL VARIABLES
     *
     * Variable             Type        Description
     * ==========           ==========  ====================
     * tempStress	    double
     * factor		    double	Weigthing of tempStress in determining
     *					  today's plant stress
     */
    double          tempStress;
    const double    factor = 0.67;

    /* Compute stress based on abgd nitrogen concentration only */
    if (Crop->svShoot > 0.0)
    {
        if (NaAbgd > NcAbgd)
            tempStress = 0.0;
        else if (NaAbgd < NnAbgd)
            tempStress = 1.0 - 0.001;
        else
            tempStress = 1.0 - (NaAbgd - NnAbgd) / (NcAbgd - NnAbgd);
    }
    else
        tempStress = 0.0;

    Crop->svN_StressFactor = factor * tempStress + (1.0 - factor) * Crop->svN_StressFactor;

    /* calculating cumulative nitrogen stress */
    if (Crop->svTT_Cumulative < Crop->calculatedMaturityTT)
        Crop->svN_StressCumulative += Crop->svN_StressFactor * Crop->svTT_Daily / (Crop->calculatedMaturityTT - Crop->userEmergenceTT);
}

void CropNitrogenUptake (double N_AbgdConcReq, double N_RootConcReq, double NaAbgd, double NxAbgd, double NxRoot, int autoNitrogen, CropStruct *Crop, SoilStruct *Soil)
{
    /* 
     * -----------------------------------------------------------------------
     * LOCAL VARIABLES
     *
     * Variable             Type        Description
     * ==========           ==========  ====================
     * i		    int
     * N_CropDemand	    double
     * N_ReqAbgdGrowth	    double	N mass required to satisfy abgd growth
     *					  based on luxury consumption
     * N_ReqRootGrowth	    double	N mass required to satisfy root growth
     *					  based on luxury consumption
     * N_ReqRhizodeposition double	N mass required to satisfy
     *					  rizhodeposition
     * N_AbgdConc	    double	Aboveground N concentration in biomass
     * N_RootConc	    double	Root N concentration in biomass
     * NO3Uptake	    double[]	Array passed to Subrouthine
     *					  PotentialSoluteUptake
     * NH4Uptake	    double[]	Array passed to Subrouthine
     *					  PotentialSoluteUptake
     * NO3supply	    double	NO3 cumulative supply through the
     *					  rooting depth
     * NH4supply	    double	NH4 cumulative supply through the
     *					  rooting depth
     * N_SoilSupply	    double	NO3 and NH4 cumulative supply
     * N_Uptake		    double	NO3 and NH4 total N uptake
     * N_FractionalSatisfiedDemand
     *			    double
     * N_FractionalSatisfiedSupply
     *			    double
     * N_Surplus	    double	Variable used to remove excess N in
     *					  biomass due to dilution
     * Nfixation	    double
     * Nratio		    double
     */
    int             i;
    double          N_CropDemand;
    double          N_ReqAbgdGrowth;
    double          N_ReqRootGrowth;
    double          N_ReqRhizodeposition;
    double          N_AbgdConc, N_RootConc;
    double          NO3Uptake[Soil->totalLayers];
    double          NH4Uptake[Soil->totalLayers];
    double          NO3supply, NH4Supply;
    double          N_SoilSupply;
    double          N_Uptake;
    double          N_FractionalSatisfiedDemand;
    double          N_FractionalSatisfiedSupply;
    double          N_Surplus;  
    double          Nfixation = 0.0;
    double          Nratio;

    N_CropDemand = 0.0;
    N_Surplus = 0.0;

    /* calculate N demand for today's growth
     * (assume no deficit demand to be satisfied - testing) */
    N_ReqAbgdGrowth = Crop->svShootDailyGrowth * N_AbgdConcReq;
    N_ReqRootGrowth = Crop->svRootDailyGrowth * N_RootConcReq;
    N_ReqRhizodeposition = Crop->svRizhoDailyDeposition * N_RootConcReq;
    N_CropDemand = N_ReqAbgdGrowth + N_ReqRootGrowth + N_ReqRhizodeposition;

    /* PotentialSoluteUptakeOption2 */
    /* Mg solute/ha */
    PotentialSoluteUptakeOption2 (&NO3supply, NO3Uptake, 0.0, Soil->totalLayers, Soil->BD, Soil->layerThickness, Soil->waterUptake, Soil->NO3, Soil->waterContent);
    PotentialSoluteUptakeOption2 (&NH4Supply, NH4Uptake, 5.6, Soil->totalLayers, Soil->BD, Soil->layerThickness, Soil->waterUptake, Soil->NH4, Soil->waterContent);

    /* Calculate actual N uptake and update plant N concentrations */
    N_SoilSupply = NO3supply + NH4Supply;

    Nratio = N_SoilSupply / N_CropDemand;

    N_Uptake = N_CropDemand * (1.0 - exp (-Nratio));
    N_FractionalSatisfiedDemand = N_Uptake / N_CropDemand;
    N_FractionalSatisfiedSupply = N_Uptake / N_SoilSupply;

    /* Update soil NO3 and NH4 pools */
    for (i = 0; i < Soil->totalLayers; i++)
    {
        Soil->NO3[i] -= N_FractionalSatisfiedSupply * NO3Uptake[i];
        Soil->NH4[i] -= N_FractionalSatisfiedSupply * NH4Uptake[i];
    }

    /* Split the code below in update pools surrogate for removal from seed
     * nitrogen fixation autofertilization */
    Crop->svN_Shoot += N_FractionalSatisfiedDemand * N_ReqAbgdGrowth;
    Crop->svN_Root += N_FractionalSatisfiedDemand * N_ReqRootGrowth;
    Crop->svN_RizhoDailyDeposition = N_FractionalSatisfiedDemand * N_ReqRhizodeposition;
    NaAbgd = Crop->svN_Shoot / Crop->svShoot;

    /* Legume condition
     * Auto nitrogen for no N limitation
     * Auto nitrogen to consider N seed storage (auto nitrogen if stressed and
     * 1.5 leaf stage, approx.) */
    if (Crop->userLegume)
    {	/* N fixation */
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

    /* Trim N above maximum and return to soil layer 0 as nitrate */
    if (N_AbgdConc > NxAbgd)
    {
        N_Surplus = 0.0;
        N_Surplus = Crop->svShoot * (N_AbgdConc - NxAbgd);
        Soil->NO3[0] += N_Surplus;
        Crop->svN_Shoot = NxAbgd * Crop->svShoot;
    }

    if (N_RootConc > NxRoot)
    {
        N_Surplus = 0.0;
        N_Surplus = Crop->svRoot * (N_RootConc - NxRoot);
        Soil->NO3[0] += N_Surplus;
        Crop->svN_Root = NxRoot * Crop->svRoot;
    }
}

void PotentialSoluteUptakeOption2 (double *SoluteSupply, double *SoluteUptake, double Kd, int totalLayers, const double *BD, const double *dz, const double *WaterUptake, const double *Solute, const double *WC)
{
    /*
     * This sub considers the solute as "available" for uptake if in solution
     * and in a layer with positive water uptake
     * 2014 02 20 a function was added to limit uptake if concentration is too
     * low, this function only valid for NO3 and NH4
     * -----------------------------------------------------------------------
     * LOCAL VARIABLES
     *
     * Variable             Type        Description
     * ==========           ==========  ====================
     * i		    int
     * soluteConc	    double	Solute concentration (kg/kg)
     * soilBufferPower	    double	(-)
     * layerPotentialUptake double	(kg/m2/day)
     * totalPotentialUptake double	(kg/m2/day)
     * ppmSolute	    double	Solute concentration (parts per million)
     * factorSoluteUptake   double	Factor the affects potential uptake
     *					  based on ppm, current function valid
     *					  for NO3 and NH4
     * temp1		    double
     * temp2		    double
     */
    int             i;
    double          soluteConc;
    double          soilBufferPower;
    double          layerPotentialUptake;
    double          totalPotentialUptake;
    double          ppmSolute;
    double          factorSoluteUptake;
    double          temp1, temp2;

    /* Soil buffer power is dimensionless
     * Soil bulk density must be in g/cm3 and slope of the adsortion isotherm
     * (Kd) is in cm3/g */
    totalPotentialUptake = 0.0;

    for (i = 0; i < totalLayers; i++)
    {
        soluteConc = 0.0;

        if (WaterUptake[i] > 0.0 && Solute[i] > 0.0)
        {
	    /* Calculate solute concentration in ppm */
            ppmSolute = 100.0 * Solute[i] / (dz[i] * BD[i]);
            factorSoluteUptake = 1.0 - exp (-0.1 * ppmSolute);
            soilBufferPower = Kd * BD[i] + WC[i];
	    /* 1/10 converts from Mg/ha to kg/m2 */
            soluteConc = Solute[i] / 10.0 / (dz[i] * soilBufferPower * WATER_DENSITY);
            temp1 = WATER_DENSITY * WC[i] * dz[i] * soluteConc * factorSoluteUptake;
	    /* Max allowed concentration in uptake, about 0.6 g N / kg H2O */
            temp2 = WaterUptake[i] * 0.0006;
            layerPotentialUptake = temp1 < temp2 ? temp1 : temp2;
	    /* 10 converts from kg/m2 to Mg/ha */
            layerPotentialUptake *= 10.0;
        }
        else
            layerPotentialUptake = 0.0;

        totalPotentialUptake += layerPotentialUptake;
        SoluteUptake[i] = layerPotentialUptake;
    }

    *SoluteSupply = totalPotentialUptake;
}

double ShootBiomassPartitioning (double Stage, double Po, double Pf)
{
    /* 
     * -----------------------------------------------------------------------
     * LOCAL VARIABLES
     *
     * Variable             Type        Description
     * ==========           ==========  ====================
     * P1		    double	Stage at which partitioning is half
     *					  way between Po and Pf
     * P2		    double	Curvature factor (not recommended to
     *					  be available to user)
     * partitioning	    double	[return value]
     */
    const double    P1 = 0.4;
    const double    P2 = 4.0;
    double	    partitioning;

    partitioning = Po + (Pf - Po) / (1.0 + pow ((Stage + 0.0001) / P1, -P2));

    return (partitioning);
}


void RadiationInterception (int y, int doy, CommunityStruct *Community)
{
    /* 
     * -----------------------------------------------------------------------
     * LOCAL VARIABLES
     *
     * Variable             Type        Description
     * ==========           ==========  ====================
     * Fractional_TT	    double
     * Delta_Fractional_TT  double
     * Rate_Crop_Interception
     *			    double
     * Delta_Crop_Interception
     *			    double
     * Reserves_Use_Allowance
     *			    double	Allows LAI expansion larger than daily
     *					  growth SLA
     * Delta_Crop_Interception_Growth
     *			    double	Crop interception expansion based on
     *					  daily growth and SLA
     * Compensatory_Expansion
     *			    double
     * Rate_Root_Depth	    double
     * WSF		    double	Water stress factor - copied from
     *					  stored value; 1 = no stress
     * NSF		    double	Nitroge stress factor - copied from
     *					  stored value; 1 = no stress
     * Sx		    double	The maximum of the two stresses
     * Sn		    double	The minimum of the two stresses
     * SF		    double
     * a		    double	Crop radiation interception parameter
     *					  modified logistic curve
     * b		    double	Crop radiation interception parameter
     *					  modified logistic curve
     * c		    double	Crop radiation interception parameter
     *					  modified logistic curve
     * d		    double	Crop radiation interception parameter
     *					  modified logistic curve (reduce the
     *					  number increase senescence)
     * e		    double	Root parameter logistic curve
     * f		    double	Root parameter logistic curve
     * rde		    double	Root depth at emergence (m)
     */
    double          Fractional_TT;
    double          Delta_Fractional_TT;
    double          Rate_Crop_Interception;
    double          Delta_Crop_Interception;
    double          Reserves_Use_Allowance;
    double          Delta_Crop_Interception_Growth;
    double          Compensatory_Expansion;
    double          Rate_Root_Depth;
    double          WSF;
    double          NSF;
    double          Sx, Sn, SF;
    const double    a = 6.0;
    const double    b = 20.0;
    const double    c = 15.0;
    const double    d = 16.0;
    const double    e = 5.0;
    const double    f = 15.0;
    const double    rde = 0.3;
    int             i;
    CropStruct     *Crop;

    for (i = 0; i < Community->NumCrop; i++)
    {
        Crop = &Community->Crop[i];

        if (Crop->stageGrowth > NO_CROP)
        {
            Fractional_TT = (Crop->svTT_Cumulative - 1.0 * Crop->svTT_Daily) / (Crop->calculatedMaturityTT - Crop->userEmergenceTT);
            Delta_Fractional_TT = Crop->svTT_Daily / (Crop->calculatedMaturityTT - Crop->userEmergenceTT);

            if (Crop->svTT_Cumulative < Crop->calculatedMaturityTT)
            {
                if (Crop->svTT_Cumulative < Crop->userEmergenceTT)
                {
                    Crop->svRadiationInterception = 0.0;
                    Crop->svRootingDepth = rde * Crop->svTT_Cumulative / Crop->userEmergenceTT;
                }
                else
                {
                    Rate_Crop_Interception = (b * exp (a - b * Fractional_TT) - c * exp (c * Fractional_TT - d)) / pow (1.0 + exp (a - b * Fractional_TT) + exp (c * Fractional_TT - d), 2.0);
                    Rate_Root_Depth = f * exp (e - f * Fractional_TT) / pow (1.0 + exp (e - f * Fractional_TT), 2.0);

                    /* Compute stress effect on canopy expansion or senescence */
                    WSF = 1.0 - Crop->svWaterStressFactor;
                    NSF = pow (1.0 - Crop->svN_StressFactor, 3.0);

                    /* Select max and min stress */
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

                    /* Stress reduces rate of increase of canopy interception or
                     * accelerates senescence */
                    if (Rate_Crop_Interception > 0.0)
                        SF = 1.0 - pow (1.0 - Sn, Sx);
                    else
                        SF = 1.0 + pow (1.0 - Sn, 3.0 * Sx);

                    /* Compute compensatory expansion factor and reserves use
                     * allowance */
                    if (Rate_Crop_Interception > 0.0)
                    {
                        Compensatory_Expansion = sqrt ((Crop->userMaximumSoilCoverage / (1.0 + exp (a - b * Fractional_TT) + exp (-c + d * Fractional_TT))) / Crop->svRadiationInterception);
                        Reserves_Use_Allowance = 1.0 + 3.0 / (1.0 + pow (Crop->svRadiationInterception / 0.15, 4.0));
                    }
                    else
                    {
                        Compensatory_Expansion = 1.0;
                        Reserves_Use_Allowance = 1.0;
                    }

                    Delta_Crop_Interception = Crop->userMaximumSoilCoverage * Rate_Crop_Interception * Delta_Fractional_TT * SF * Compensatory_Expansion;
                    Delta_Crop_Interception_Growth = Reserves_Use_Allowance * 0.75 * (1.0 - Crop->svRadiationInterception) * (0.1 * Crop->svShootDailyGrowth * 25.0);
                    if (Delta_Crop_Interception > Delta_Crop_Interception_Growth)
                        Delta_Crop_Interception = Delta_Crop_Interception_Growth;

                    Crop->svRadiationInterception += Delta_Crop_Interception;

                    /* Just in case stress accelerates senescence too much - not sure
                     * it is needed */
                    /* Just in case cold damage defoliates too much - not sure it is
                     * needed */
                    if (Crop->svRadiationInterception < 0.001)
                        Crop->svRadiationInterception = 0.001;
                    if (Crop->svRadiationInterception > 0.98)
                        Crop->svRadiationInterception = 0.98;

                    Crop->svRootingDepth += (Crop->userMaximumRootingDepth - rde) * Rate_Root_Depth * Delta_Fractional_TT;
                    if (Crop->svRootingDepth > Crop->userMaximumRootingDepth)
                        Crop->svRootingDepth = Crop->userMaximumRootingDepth;
                } /* end if Crop->svTT_Cumulative >= Crop->userEmergenceTT */
            }	/* end if Crop->svTT_Cumulative < Crop->calculatedMaturityTT */
        }
    }
}

void Phenology (int y, int doy, const WeatherStruct *Weather, CommunityStruct *Community)
{
    int             i;
    CropStruct     *Crop;

    for (i = 0; i < Community->NumCrop; i++)
    {
        Crop = &Community->Crop[i];
        if (Crop->stageGrowth > NO_CROP)
        {
            Crop->svTT_Daily = 0.5 * (ThermalTime (Crop->userTemperatureBase, Crop->userTemperatureOptimum, Crop->userTemperatureMaximum, Weather->tMin[y][doy - 1]) + ThermalTime (Crop->userTemperatureBase, Crop->userTemperatureOptimum, Crop->userTemperatureMaximum, Weather->tMax[y][doy - 1]));
            Crop->svTT_Cumulative += Crop->svTT_Daily;
        }
    }
}

void ComputeColdDamage (int y, int doy, CropStruct *Crop, const WeatherStruct *Weather, const SnowStruct *Snow, ResidueStruct *Residue)
{
    /* 
     * -----------------------------------------------------------------------
     * LOCAL VARIABLES
     *
     * Variable             Type        Description
     * ==========           ==========  ====================
     * Min_Temperature	    double
     * Cold_Damage	    double	Fraction of interception decreased by
     *					  cold damage
     * Crop_Tn		    double	Minimum temperature for cold damage
     * Crop_Tth		    double	Threshold temperature for cold damage
     * residueMass
     * Residue_N_Mass
     * Phenology_Delay_Factor
     *			    double	The more advanced the cycle the lesser
     *					  the effect on cold damage on
     *					  delaying phenology
     */
    double          Min_Temperature;
    double          Cold_Damage;
    double          Crop_Tn;
    double          Crop_Tth;
    double          residueMass;
    double          Residue_N_Mass;
    double          Phenology_Delay_Factor;

    Min_Temperature = Weather->tMin[y][doy - 1];

    Crop_Tn = Crop->userColdDamageMinTemperature;
    Crop_Tth = Crop->userColdDamageThresholdTemperature;
    Cold_Damage = ColdDamage (Min_Temperature, Crop_Tn, Crop_Tth) * (1.0 - Snow->snowCover);

    if (Crop->userAnnual)
        Phenology_Delay_Factor = 1.0 / (1.0 + pow ((Crop->svTT_Cumulative / Crop->calculatedFloweringTT) / 0.15, 4.0));
    else
        Phenology_Delay_Factor = 1.0;

    residueMass = Crop->svShoot * pow (Cold_Damage, 3.0);
    Residue_N_Mass = Crop->svN_Shoot * pow (Cold_Damage, 3.0);

    Crop->svBiomass -= residueMass;
    Crop->svShoot -= residueMass;
    Crop->svN_Shoot -= Residue_N_Mass;

    if (Crop->svTT_Cumulative > Crop->userEmergenceTT)
        Crop->svTT_Cumulative = Crop->userEmergenceTT + (Crop->svTT_Cumulative - Crop->userEmergenceTT) * (1.0 - Phenology_Delay_Factor * pow (Cold_Damage, 2.5));

    Crop->svRadiationInterception = Crop->svRadiationInterception * (1.0 - pow (Cold_Damage, 3.0));
    Residue->stanResidueMass += residueMass * Crop->userFractionResidueStanding;
    Residue->flatResidueMass += residueMass * (1.0 - Crop->userFractionResidueStanding);
    Residue->stanResidueN += Residue_N_Mass * Crop->userFractionResidueStanding;
    Residue->flatResidueN += Residue_N_Mass * (1.0 - Crop->userFractionResidueStanding);
    /* Assume 33% residue moisture at harvest */
    Residue->stanResidueWater += residueMass * Crop->userFractionResidueStanding / 10.0 * 0.5;
    Residue->flatResidueWater += residueMass * (1.0 - Crop->userFractionResidueStanding) / 10.0 * 0.5;

    /* Yearly output variables */
    Residue->yearResidueBiomass += residueMass;

    /* Season outputs */
    Crop->rcResidueBiomass += residueMass;
    Crop->rcBiomass += residueMass;
}

double ColdDamage (double T, double Crop_Tn, double Crop_Tth)
{
    /* 
     * -----------------------------------------------------------------------
     * LOCAL VARIABLES
     *
     * Variable             Type        Description
     * ==========           ==========  ====================
     * damage		    doulbe	[return value]
     */
    double          damage;

    if (T <= Crop_Tn)
        damage = 0.99;
    else if (T > Crop_Tn && T <= Crop_Tth)
        damage = 1.0 - (T - Crop_Tn) / (Crop_Tth - Crop_Tn);
    else
        damage = 0.0;

    return (damage);
}
