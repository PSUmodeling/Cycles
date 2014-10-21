/****************************************************************************
 * Cycles
 * Latest update:   Oct 17, 2014
 * Developer:       Yuning Shi (yshi@psu.edu)
 ***************************************************************************/
typdedef struct CropClass
{
    /*
     * Instance of a crop that has been planted
     * Instance should be deleted once the crop fallow or killed
     */
    char    cropName[128];

    /* User Defined Auto Irrigation */
    int    *autoIrrigationUsed;
    int    *autoIrrigationStartDay;
    int    *autoIrrigationStopDay;
    double    *autoIrrigationWaterDepletion;
    int    *autoIrrigationLastSoilLayer;

    /* User Defined Auto Fertilization */
    int    *autoFetilizationUsed;
    int    *autoFetilizationStartDay;
    int    *autoFetilizationStopDay;
    double    *autoFetilizationMass;
    char    autoFetilizationSource[128];
    char     *autoFetilizationForm;
    int     *autoFetilizationMethod

    /* Crop Status Flags */
    int     *cropGrowing;
    int     *cropMature;

    /* State Variables */
    double  *svTT_Daily;
    double  *svTT_Cumulative;
    double  *svRadiationInterception;
    double  *svBiomass;
    double  *svShoot;
    double  *svRoot;
    double  *svRizho;
    double  *svShootDailyGrowth;
    double  *svRootDailyGrowth;
    double  *svRizhoDailyDeposition;
    double  *svUnstressedShootDailyGrowth;
    double  *svUnstressedRootDailyGrowth;
    double  *svPostFloweringShootBiomass;
    double  *svRootingDepth;
    double  *svTranspiration;
    double  *svTranspirationPotential;
    double  *svN_Shoot;
    double  *svN_Root;
    double  *svN_Rhizo;
    double  *svN_RizhoDailyDeposition;
    double  *svN_AutoAdded;
    double  *svN_Fixation;
    double *svWaterStressFactor;
    double *svN_StressFactor;

    double *svShootUnstressed;
    double *svN_StressCumulative;

    /* User Defined Data */
    char *userSeedingDate;
    char *userFloweringDate;
    char *userMaturityDate;
    char *userMaximumSoilCoverage;
    double *userMaximumRootingDepth;
    double *userExpectedYieldAvg;
    double *userExpectedYieldMax;
    double *userExpectedYieldMin;
    char *userPercentMoistureInYield;
    double *userFractionResidueStanding;
    double *userFractionResidueRemoved;
    double *userClippingTiming;
    double *userTranspirationMinTemperature;
    double *userTranspirationThresholdTemperature;
    double *userColdDamageMinTemperature;
    double *userColdDamageThresholdTemperature;
    double *userTemperatureBase;
    double *userTemperatureOptimum;
    double *userTemperatureMaximum;
    double *userShootPartitionInitial;
    double *userShootPartitionFinal;
    double *userRadiationUseEfficiency;
    double *userTranspirationUseEfficiency;
    double *userHIx;
    double *userHIo;        /* intercept harvest index */
    double *userHIk;
    double *userEmergenceTT;
    double *userNMaxConcentration;
    double *userNDilutionSlope;
    double *userKc;
    int *userAnnual;
    int *userLegume;
    int *userC3orC4;

    int *harvestDateFinal;
    int *harvestCount;
    char *stageGrowth;
} CropClass;
