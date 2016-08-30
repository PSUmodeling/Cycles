#ifndef CYCLES_STRUCT_HEADER
#define CYCLES_STRUCT_HEADER
/*****************************************************************************
 *
 * FILE NAME:   Cycles_struct.h
 * PURPOSE:     Defines Cycles data structure.
 *
 * Variables                Type        Description
 * ==========               ==========  ====================
 * ---------------------------------------------------------------------------
 * ctrl_struct         struct      Simulation control structure
 *  simStartYear            int         Simulation start year
 *  simEndYear              int         Simulation end year
 *  totalYears              int         Total simulation years
 *  yearsInRotation         int         Years in rotation
 *  adjustedYields          int
 *  hourlyInfiltration      int         Flag to indicate whether hourly
 *                                        infiltration is used
 *  automaticNitrogen       int         Flag to indicate whether automatic
 *                                        Nitrogen is used
 *  automaticPhosphorus     int             
 *  automaticSulfur         int         Flag to indicate
 *  cropDailyOutput         int         Flag to control daily crop output
 *  soilDailyOutput         int         Flag to control daily soil output
 *  nitrogenDailyOutput     int         Flag to control daily nitrogen output
 *  waterDailyOutput        int         Flag to control daily water output
 *  weatherDailyOutput      int         Flag to control daily weather output
 *  residueDailyOutput      int         Flag to control daily residue output
 *  soilCarbonDailyOutput   int         Flag to control daily soil carbon
 *                                        output
 *  annualSoilOutput        int         Flag to control annual soil output
 *  profileOutput           int         Flag to control profile output
 *  seasonOutput            int         Flag to control seasonal output
 * ---------------------------------------------------------------------------
 * soil_struct               struct      Soil structure
 *  totalLayers             int         Total soil layers [input]
 *  Curve_Number            double      Curve number [input]
 *  Percent_Slope           double      Slope [input]
 *  annualTemperaturePhase  double      Annual soil temperature phase
 *  dampingDepth            double*     Soil temperature damping depth (m)
 *  cumulativeDepth         double*     Depth to the bottom of layer (m)
 *  nodeDepth               double*     Depth of node (m)
 *  layerThickness          double*     Measured layer thickness (m) [input]
 *  Clay                    double*     Clay fraction [input]
 *  Sand                    double*     Sand fraction [input]
 *  IOM                     double*     Initial Organic Matter [input]
 *  NO3                     double*     Initial Nitrate (kg/ha) [input]
 *  NH4                     double*     Initial Ammonium (kg/ha) [input]
 *  BD                      double*     Bulk Density (Mg/m3)
 *  FC                      double*     Field Capacity water content [input]
 *  PWP                     double*     Permanent Wilting Point [input]
 *  Porosity                double*     Saturation water content (m3/m3)
 *  PAW                     double*     Maximum plant available water
 *  FC_WaterPotential       double*     Estimate water potential at field
 *                                        capacity
 *  airEntryPotential       double*     Calculate Air Entry Potential
 *  B_Value                 double*     Calculated "B" value
 *  M_Value                 double*     Calculated "M" value
 *  n2o                     double*     Temporary output of n2o/layer
 *  SOC_Conc                double*     (g C/kg soil)
 *  SOC_Mass                double*     Soil organic carbon (Mg/ha)
 *  SON_Mass                double*     Soil organic Nitrogen (Mg/ha)
 *  MBC_Mass                double*     Microbial biomass C (Mg/ha)
 *  MBN_Mass                double*     Microbial Biomass N (Mg/ha)
 *  SOCProfile              double      Profile soil organic C
 *  SONProfile              double      Profile soil organic N
 *  C_Humified              double      Carbon humified from residues, roots,
 *                                        rizho, and manure
 *  C_ResidueRespired       double      Carbon respired from residues, roots,
 *                                        rizho, and manure
 *  C_SoilRespired          double      Carbon respired from soil organic
 *                                        carbon only
 *  soilTemperature         double*     Soil temperature (Degree Celsius)
 *  waterContent            double*     Volumetric water content (m3/m3)
 *  waterUptake             double*     Layer water uptake
 *  pH                      double*     Soil pH
 *  evaporationVol          double      Soil evaporation (mm)
 *  residueEvaporationVol   double      Evaporation from residue (mm)
 *  infiltrationVol         double      Soil infiltration (mm)
 *  runoffVol               double      Runoff (mm)
 *  irrigationVol           double      Irrigation (mm)
 *  drainageVol             double      Drainage (mm)
 *  NO3Leaching             double      NO3 leaching (Mg N/ha)
 *  NH4Leaching             double      NH4 leaching (Mg N/ha)
 *  NO3Profile              double
 *  NH4Profile              double
 *  N_Immobilization        double
 *  N_Mineralization        double
 *  N_NetMineralization     double
 *  NH4_Nitrification       double
 *  N2O_Nitrification       double
 *  NO3_Denitrification     double
 *  N2O_Denitrification     double
 *  NH4_Volatilization      double
 *
 *
 ***************************************************************************/
typedef struct ctrl_struct
{
    int             simStartYear;
    int             simEndYear;
    int             totalYears;

    int             hourlyInfiltration;
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

    char            crop_filename[128];
    char            operation_filename[128];
    char            weather_filename[128];
    char            soil_filename[128];
} ctrl_struct;

typedef struct soil_struct
{
    int             totalLayers;
    double          Curve_Number;
    double          Percent_Slope;

    double          annualTemperaturePhase;
    double          dampingDepth;

    double         *cumulativeDepth;
    double         *nodeDepth;
    double         *layerThickness;
    double         *Clay;
    double         *Sand;
    double         *IOM;
    double         *NO3;
    double         *NH4;
    double         *BD;
    double         *FC;
    double         *PWP;

    double         *Porosity;
    double         *PAW;
    double         *FC_WaterPotential;
    double         *airEntryPotential;
    double         *B_Value;
    double         *M_Value;
    double         *ksat;

    double         *n2o;

    double         *SOC_Conc;
    double         *SOC_Mass;
    double         *SON_Mass;
    double         *MBC_Mass;
    double         *MBN_Mass;
    double          SOCProfile;
    double          SONProfile;

    double          C_Humified;
    double          C_ResidueRespired;
    double          C_SoilRespired;

    double         *soilTemperature;
    double         *waterContent;
    double         *waterUptake;
    double         *pH;

    double         *latflux;

    double          evaporationVol;
    double          residueEvaporationVol;
    double          infiltrationVol;
    double          runoffVol;
    double          irrigationVol;
    double          drainageVol;
    double          NO3Leaching;
    double          NH4Leaching;

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
} soil_struct;

typedef struct crop_struct
{
    /* Instance of the crop that is being planted */
    char            cropName[128];

    /* User Defined Auto Irrigation */
    int             autoIrrigationUsed;
    int             autoIrrigationStartDay;
    int             autoIrrigationStopDay;
    double          autoIrrigationWaterDepletion;
    int             autoIrrigationLastSoilLayer;

    /* User Defined Auto Fertilization */
    int             autoFertilizationUsed;
    int             autoFertilizationStartDay;
    int             autoFertilizationStopDay;
    double          autoFertilizationMass;
    char            autoFertilizationSource;
    char            autoFertilizationForm;
    int             autoFertilizationMethod;

    /* Crop Status Flags */
    int             cropGrowing;
    int             cropMature;

    /* State Variables */
    double          svTT_Daily;
    double          svTT_Cumulative;
    double          svRadiationInterception;
    double          svBiomass;
    double          svShoot;
    double          svRoot;
    double          svRizho;
    double          svShootDailyGrowth;
    double          svRootDailyGrowth;
    double          svRizhoDailyDeposition;
    double          svUnstressedShootDailyGrowth;
    double          svUnstressedRootDailyGrowth;
    double          svPostFloweringShootBiomass;
    double          svRootingDepth;
    double          svTranspiration;
    double          svTranspirationPotential;
    double          svN_Shoot;
    double          svN_Root;
    double          svN_Rhizo;
    double          svN_RizhoDailyDeposition;
    double          svN_AutoAdded;
    double          svN_Fixation;
    double          svWaterStressFactor;
    double          svN_StressFactor;

    double          svShootUnstressed;
    double          svN_StressCumulative;

    double          svRadiationInterception_nc;
    
    double          userMaximumSoilCoverage;
    double          userMaximumRootingDepth;
    double          userExpectedYieldAvg;
    double          userExpectedYieldMax;
    double          userExpectedYieldMin;
    double          userPercentMoistureInYield;
    double          userFractionResidueStanding;
    double          userFractionResidueRemoved;
    double          userClippingBiomassThresholdUpper;
    double          userClippingBiomassThresholdLower;
    double          userClippingTiming;
    int             userClippingDestiny;
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
    double          userExtinctionCoefficient;

    double          userPlantingDensity;

    int             userClippingStart;
    int             userClippingEnd;

    double          userFloweringTT;
    double          userMaturityTT;
    double          calculatedSimAvgYield;
    double          calculatedSimMaxYield;
    double          calculatedSimMinYield;
    double          LWP_StressOnset;
    double          LWP_WiltingPoint;
    double          transpirationMax;

    int             harvestDateFinal;
    int             harvestCount;
    enum stage      stageGrowth;

    double          rcForageYield;
    double          rcGrainYield;
    double          rcBiomass;
    double          rcRoot;
    double          rcResidueBiomass;
    double          rcCropTranspiration;
    double          rcCropTranspirationPotential;
    double          rcSoilWaterEvaporation;
    double          rcHarvestIndex;
    double          rcTotalNitrogen;
    double          rcRootNitrogen;
    double          rcGrainNitrogenYield;
    double          rcForageNitrogenYield;
    double          rcNitrogenCumulative;
    double          rcNitrogenInHarvest;
    double          rcNitrogenInResidue;
    double          rcNitrogenForageConc;
} crop_struct;

typedef struct comm_struct
{
    /* State Variables */
    double          svRadiationInterception;
    double          svBiomass;
    double          svShoot;
    double          svRoot;
    double          svRizho;
    double          svShootDailyGrowth;
    double          svRootDailyGrowth;
    double          svRizhoDailyDeposition;
    double          svRootingDepth; /* maximum */
    double          svTranspiration;
    double          svTranspirationPotential;
    double          svN_Shoot;
    double          svN_Root;
    double          svN_Rhizo;
    double          svN_RizhoDailyDeposition;
    double          svN_AutoAdded;
    double          svN_Fixation;
    double          svWaterStressFactor;
    double          svN_StressFactor;

    crop_struct    *Crop;
    int             NumCrop;
    int             NumActiveCrop;
} comm_struct;

typedef struct plant_struct
{
    /* Planting */
    int             opYear;
    int             opDay;
    int             status;
    char            cropName[128];
    int             usesAutoIrrigation;
    int             usesAutoFertilization;
    int             plantID;
    double          plantingDensity;
    int             clippingStart;
    int             clippingEnd;
} plant_struct;

typedef struct tillage_struct
{
    /* Tillage */
    int             opYear;
    int             opDay;
    int             status;
    char            opToolName[MAXSTRING];
    double          opDepth;
    double          opSDR;
    double          opMixingEfficiency;
    char            cropNameT[128];
    double          fractionThermalTime;
    double          killEfficiency;
    int             grainHarvest;
    double          forageHarvest;
} tillage_struct;

typedef struct fixirr_struct
{
    /* Fixed Irrigation */
    int             opYear;
    int             opDay;
    int             status;
    double          opVolume;
} fixirr_struct;

typedef struct fixfert_struct
{
    /* Fixed Fertilization */
    int             opYear;
    int             opDay;
    int             status;
    char            opSource[MAXSTRING];
    double          opMass;
    char            opForm[MAXSTRING];
    char            opMethod[MAXSTRING];
    int             opLayer;    /* Starting from 1 */
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
} fixfert_struct;

typedef struct autoirr_struct
{
    char            cropName[128];
    int             startDay;
    int             stopDay;
    double          waterDepletion;
    int             lastSoilLayer;
} autoirr_struct;
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

typedef struct cropmgmt_struct
{
    int             yearsInRotation;
    int             adjustedYields;
    int             automaticNitrogen;
    int             automaticPhosphorus;
    int             automaticSulfur;

    int             rotationYear;

    fixfert_struct *FixedFertilization;
    int             numFertilization;

    fixirr_struct  *FixedIrrigation;
    int             numIrrigation;

    tillage_struct *Tillage;
    int             numTillage;
    double         *tillageFactor;

    plant_struct   *plantingOrder;
    int             totalCropsPerRotation;

    autoirr_struct *autoIrrigation;
    int             numAutoIrrigation;

    int             usingAutoIrr;
    int             usingAutoFert;
} cropmgmt_struct;

typedef struct snow_struct
{
    double          Snow;
    double          snowFall;
    double          snowMelt;
    double          snowCover;
    double          snowEvaporationVol;
} snow_struct;

typedef struct residue_struct
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

    double         *residueAbgd;
    double         *residueRt;
    double         *residueRz;
    double         *residueAbgdN;
    double         *residueRtN;
    double         *residueRzN;
    double          yearResidueBiomass;
    double          yearResidueHarvested;
    double          yearRootBiomass;
    double          yearRhizodepositionBiomass;
    double         *manureC;
    double         *manureN;    /* Mg/ha */
} residue_struct;

typedef struct soilc_struct
{
    double         *factorComposite;
    double         *carbonRespired;
    double         *rootBiomassInput;
    double         *rhizBiomassInput;
    double         *abgdBiomassInput;
    double         *rootCarbonInput;
    double         *rhizCarbonInput;
    double         *manuCarbonInput;
    double         *abgdCarbonInput;
    double         *carbonMassInitial;
    double         *carbonMassFinal;
    double         *annualDecompositionFactor;
    double         *annualSoilCarbonDecompositionRate;
    double         *annualCarbonInputByLayer;
    double         *annualHumifiedCarbonMass;
    double         *annualRespiredCarbonMass;
    double         *annualRespiredResidueCarbonMass;
    double         *annualHumificationCoefficient;
    double         *annualNmineralization;
    double         *annualNImmobilization;
    double         *annualNNetMineralization;
    double          annualAmmoniumNitrification;
    double          annualNitrousOxidefromNitrification;
    double          annualAmmoniaVolatilization;
    double          annualNO3Denitrification;
    double          annualNitrousOxidefromDenitrification;
    double          annualNitrateLeaching;
    double          annualAmmoniumLeaching;
} soilc_struct;

typedef struct weather_struct
{
    double          siteAltitude;
    double          siteLatitude;
    double          screeningHeight;
    int             length;
    double         *yearlyAmplitude;
    double         *annualAverageTemperature;
    int            *lastDoy;
    double        **wind;
    double        **ETref;
    double        **precipitation;
    double        **RHmax;
    double        **RHmin;
    double        **solarRadiation;
    double        **tMax;
    double        **tMin;
    double          atmosphericPressure;
} weather_struct;

typedef struct print_struct
{
    char	    var_name[16];
    char	    unit[16];
    double	   *print_var;
} print_struct;

typedef struct summary_struct
{
    double          abgd_c_input;
    double          root_c_input;
    double          residue_biomass;
    double          produced_root;
    double          residue_resp;
    double          hum;
    double          soil_resp;
    double          n_mineralization;
    double          n_immobilization;
    double          n_net_mineralization;
    double          nh4_nitrification;
    double          n2o_from_nitrification;
    double          nh3_volatilization;
    double          no3_denirification;
    double          n2o_from_denitrification;
    double          no3_leaching;
    double          nh4_leaching;
    double          initial_soc;
    double          final_soc;
} summary_struct;

typedef struct CyclesStruct
{
    ctrl_struct     SimControl;

    soil_struct      Soil;
    cropmgmt_struct CropManagement;
    comm_struct     Community;
    residue_struct  Residue;
    soilc_struct    SoilCarbon;
    weather_struct  Weather;
    snow_struct     Snow;
    summary_struct  Summary;

    print_struct   *daily_output;
    print_struct   *annual_output;
} *CyclesStruct;
#endif
