#ifndef CYCLES_STRUCT_HEADER
#define CYCLES_STRUCT_HEADER

typedef struct SimControlStruct
{
    int            *yearSpecificLastDOY;

    int             simStartYear;
    int             simEndYear;
    int             totalYears;
    int             yearsInRotation;

    int             adjustedYields;
    int             hourlyInfiltration;
    int             automaticNitrogen;
    int             automaticPhosphorus;
    int             automaticSulfur;
    int             cropDailyOutput;
    int             soilDailyOutput;
    int             nitrogenDailyOutput;
    int             waterDailyOutput;
    int             weatherDailyOutput;
    int             residueDailyOutput;
    int             soilCarbonDailyOutput;
    int             annualSoilOutput;
    int             profileOutput;
    int             seasonOutput;
} SimControlStruct;

typedef struct SoilStruct
{
    int             totalLayers;
    int             Curve_Number;
    double          Percent_Slope;

    int             annualTemperaturePhase;
    double          dampingDepth;

    double         *cumulativeDepth;    /* Depth to the bottom of layer (m) */
    double         *nodeDepth;          /* Depth of node (m) */
    double         *layerThickness;     /* Measured layer thickness (m) */
    double         *Clay;               /* Clay fraction */
    double         *Sand;               /* Sand fraction */
    double         *IOM;                /* Initial Organic Matter */
    double         *NO3;                /* Nitrate (kg/ha) */
    double         *NH4;                /* Ammonium (kg/ha) */
    double         *BD;                 /* Bulk Density (Mg/m3) */
    double         *FC;                 /* Field Capacity water content */
    double         *PWP;                /* Permanent Wilting Point */
    double         *Porosity;           /* Saturation water content (m3/m3) */
    double         *PAW;                /* Maximum plant available water */
    double         *FC_WaterPotential;  /* Estimate water potential at field
                                         * capacity */
    double         *airEntryPotential;  /* Calculate Air Entry Potential */
    double         *B_Value;            /* Calculated "B" value */
    double         *M_Value;            /* Calculated "M" value */

    double         *n2o;                /* Temporary output of n2o/layer */

    double         *SOC_Conc;           /* g C / kg soil */
    double         *SOC_Mass;           /* Soil Organic Carbon (Mg/ha)
                                         * factor 0.58 converts SOM to SOC */
    double         *SON_Mass;           /* Soil Organic Nitrogen (Mg/ha) */
    double         *MBC_Mass;           /* Microbial Biomass C (Mg/ha) */
    double         *MBN_Mass;           /* Microbial Biomass N (Mg/ha) */
    double          SOCProfile;
    double          SONProfile;

    double          C_Humified;         /* Carbon humified from residues,
                                         * roots, rizho, and manure */
    double          C_ResidueRespired;  /* Carbon respired from residues,
                                         * roots, rizho, and manure */
    double          C_SoilRespired;     /* Carbon respired from soil organic
                                         * carbon only */

    double         *soilTemperature;    /* Celsius */
    double         *waterContent;       /* Volumetric water content (m3/m3) */
    double         *waterUptake;        /* Layer water uptake  */
    double         *pH;

    double          evaporationVol;     /* (mm) */
    double          residueEvaporationVol;  /* (mm) */
    double          infiltrationVol;    /* (mm) */
    double          runoffVol;          /* (mm) */
    double          irrigationVol;      /* (mm) */
    double          drainageVol;        /* (mm) */
    double          NO3Leaching;        /* (Mg N/ha) */
    double          NH4Leaching;        /* (Mg N/ha) */

    double          NO3Profile;
    double          NH4Profile;
    double          N_Immobilization;
    double          N_Mineralization;
    double          N_NetMineralization;
    double          NH4_Nitrification;
    double          N2O_Nitrification;
    double          NO3_Denitrification;
    double          N2O_Denitrification;
    double          NH4_Volatilization;
} SoilStruct;

typedef struct plantingOrderStruct
{
    int             seedingYear;
    int             seedingDate;
    char            cropName[128];
    int             usesAutoIrrigation;
    int             usesAutoFertilization;
    int             plantID;
} plantingOrderStruct;

typedef struct describedCropsStruct
{
    /* User Defined Data */
    char            userCropName[128];
    int             userSeedingDate;
    int             userFloweringDate;
    int             userMaturityDate;
    double          userMaximumSoilCoverage;
    double          userMaximumRootingDepth;
    double          userExpectedYieldAvg;
    double          userExpectedYieldMax;
    double          userExpectedYieldMin;
    double          userPercentMoistureInYield;
    double          userFractionResidueStanding;
    double          userFractionResidueRemoved;
    double          userClippingTiming;
    double          userTranspirationMinTemperature;
    double          userTranspirationThresholdTemperature;
    double          userColdDamageMinTemperature;
    double          userColdDamageThresholdTemperature;
    double          userTemperatureBase;
    double          userTemperatureOptimum;
    double          userTemperatureMaximum;
    double          userShootPartitionInitial;
    double          userShootPartitionFinal;
    double          userRadiationUseEfficiency;
    double          userTranspirationUseEfficiency;
    double          userHIx;
    double          userHIo;    /* intercept harvest index */
    double          userHIk;
    double          userEmergenceTT;
    double          userNMaxConcentration;
    double          userNDilutionSlope;
    double          userKc;
    int             userAnnual;
    int             userLegume;
    int             userC3orC4;
    double          calculatedFloweringTT;
    double          calculatedMaturityTT;
    double          calculatedSimAvgYield;
    double          calculatedSimMaxYield;
    double          calculatedSimMinYield;
} describedCropsStruct;

typedef struct CropStruct
{
    /* Instance of a crop that has been planted
     * Instance should be deleted once the crop fallow or killed */

    char            cropName[128];

    /* User Defined Auto Irrigation */
    int             autoIrrigationUsed;
    int             autoIrrigationStartDay;
    int             autoIrrigationStopDay;
    double          autoIrrigationWaterDepletion;
    int             autoIrrigationLastSoilLayer;

    /* User Defined Auto Fertilization */
    int             autoFetilizationUsed;
    int             autoFetilizationStartDay;
    int             autoFetilizationStopDay;
    double          autoFetilizationMass;
    char            autoFetilizationSource;
    char            autoFetilizationForm;
    int             autoFetilizationMethod;

    /* Crop Status Flags */
    int            *cropGrowing;
    int            *cropMature;

    /* State Variables */
    double         *svTT_Daily;
    double         *svTT_Cumulative;
    double         *svRadiationInterception;
    double         *svBiomass;
    double         *svShoot;
    double         *svRoot;
    double         *svRizho;
    double         *svShootDailyGrowth;
    double         *svRootDailyGrowth;
    double         *svRizhoDailyDeposition;
    double         *svUnstressedShootDailyGrowth;
    double         *svUnstressedRootDailyGrowth;
    double         *svPostFloweringShootBiomass;
    double         *svRootingDepth;
    double         *svTranspiration;
    double         *svTranspirationPotential;
    double         *svN_Shoot;
    double         *svN_Root;
    double         *svN_Rhizo;
    double         *svN_RizhoDailyDeposition;
    double         *svN_AutoAdded;
    double         *svN_Fixation;
    double         *svWaterStressFactor;
    double         *svN_StressFactor;

    double         *svShootUnstressed;
    double         *svN_StressCumulative;

    int            *harvestDateFinal;
    int            *harvestCount;
    char           *stageGrowth;
} CropStruct;

typedef struct FieldOperationStruct
{
    int             opYear;
    int             opDay;

    /* Tillage */
    char            opToolName[MAXSTRING];
    double          opDepth;
    double          opSDR;
    double          opMixingEfficiency;

    /* Fixed Irrigation */
    double          opVolume;

    /* Fixed Fertilization */
    char            opSource[MAXSTRING];
    double          opMass;
    char            opForm[MAXSTRING];
    char            opMethod[MAXSTRING];
    int             opLayer;
    double          opC_Organic;
    double          opC_Charcoal;
    double          opN_Organic;
    double          opN_Charcoal;
    double          opN_NH4;
    double          opN_NO3;
    double          opP_Organic;
    double          opP_Charcoal;
    double          opP_Inorganic;
    double          opK;
    double          opS;

    struct FieldOperationStruct *NextOperation;
} FieldOperationStruct;

typedef struct autoIrrigationStruct
{
    char            cropName[128];
    int             startDay;
    int             stopDay;
    double          waterDepletion;
    double          lastSoilLayer;
} autoIrrigationStruct;
//
//typedef struct autoFertilizationStruct
//{
//    char            cropName[128];
//    int             startDay;
//    int             stopDay;
//    double          mass;
//    char            source[MAXSTRING];
//    char            form[MAXSTRING];
//    char            method[MAXSTRING];
//} autoFertilizationStruct;

typedef struct FieldOperationListStruct
{
    FieldOperationStruct *firstOperation;
    //    FieldOperationStruct *lastOperation;
    //    FieldOperationStruct *currentOperation;
} FieldOperationListStruct;

typedef struct CropManagementStruct
{
    FieldOperationStruct *FixedFertilization;
    FieldOperationStruct *FixedIrrigation;
    FieldOperationStruct *Tillage;

    FieldOperationListStruct FixedFertilizationList;
    FieldOperationListStruct FixedIrrigationList;
    FieldOperationListStruct TillageList;

    plantingOrderStruct *plantingOrder;
    int             plantingIndex;

    describedCropsStruct *describedCrops;
    int             NumDescribedCrop;
    int             describedIndex;

    autoIrrigationStruct *autoIrrigation;
    int             usingAutoIrr;
    int             usingAutoFert;

    int             totalCropsPerRotation;

    char            nextCropName[128];
    int             nextCropSeedingDate;
    int             nextCropSeedingYear;
} CropManagementStruct;

typedef struct ResidueStruct
{
    double          residueInterception;
    double          stanResidueTau;
    double          flatResidueTau;
    double          stanResidueMass;
    double          flatResidueMass;
    double          stanResidueN;
    double          flatResidueN;
    double          manureSurfaceC;
    double          manureSurfaceN;

    double          stanResidueWater;
    double          flatResidueWater;   /* (mm) */

    double          *residueAbgd;
    double          *residueRt;
    double          *residueRz;
    double          *residueAbgdN;
    double          *residueRtN;
    double          *residueRzN;
    double          *yearResidueBiomass;
    double          *yearResidueHarvested;
    double          *yearRootBiomass;
    double          *yearRhizodepositionBiomass;
    double          *manureC;
    double          *manureN;           /* Mg/ha */
} ResidueStruct;

typedef struct WeatherStruct
{
    double          siteAltitude;
    double          siteLatitude;
    double          screeningHeight;
    int             length;
    int            *year;
    int            *jday;
    double         *time;
    double         *yearlyAmplitude;
    double         *annualAverageTemperature;
    double         *wind;
    double         *ETref;
    double         *precipitation;
    double         *RHmax;
    double         *RHmin;
    double         *solarRadiation;
    double         *tMax;
    double         *tMin;
    double          atmosphericPressure;
} WeatherStruct;

typedef struct CyclesStruct
{
    SimControlStruct SimControl;
    SoilStruct       Soil;
    CropManagementStruct CropManagement;
    CropStruct      *Crop;
    ResidueStruct    Residue;
    WeatherStruct    Weather;
} *CyclesStruct;

#endif