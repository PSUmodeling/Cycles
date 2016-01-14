#ifndef CYCLES_FUNC_HEADER
#define CYCLES_FUNC_HEADER

/* Crop.c */
//void            SelectCropInitialPosition (cropmgmt_struct *CropManagement);
//void            SelectNextCrop (cropmgmt_struct *CropManagement);
//void            PeekNextCrop (cropmgmt_struct *CropManagement);
//void            NewCrop (comm_struct *Community, const cropmgmt_struct *CropManagement);
void PlantingCrop (comm_struct *Community, const cropmgmt_struct *CropManagement, int plantingIndex);
void            AddCrop (crop_struct *Crop);
void            KillCrop (crop_struct *Crop);
void            UpdateCommunity (comm_struct *Community);

/* CropHarvest.c */
void            GrainHarvest (int y, int doy, int startYear, crop_struct *Crop, residue_struct *Residue, const soil_struct *Soil, soilc_struct *SoilCarbon, const weather_struct *Weather, const char *project);
void            ForageHarvest (int y, int doy, int startYear, crop_struct *Crop, residue_struct *Residue, const soil_struct *Soil, soilc_struct *SoilCarbon, const weather_struct *Weather, const char *project);
void            HarvestCrop (int y, int doy, int startYear, crop_struct *Crop, residue_struct *Residue, const soil_struct *Soil, soilc_struct *SoilCarbon, const weather_struct *Weather, const char *project);
void            DistributeRootDetritus (int y, double rootMass, double rhizoMass, double rootN, double rhizoN, const soil_struct *Soil, const crop_struct *Crop, residue_struct *Residue, soilc_struct *SoilCarbon);
double          ComputeHarvestIndex (double HIx, double HIo, double HIk, double cumulativeShoot, double cumulativePostFloweringShootBiomass);

/* CropProcess.c */
void            Processes (const int y, const int doy, const int autoNitrogen, comm_struct *Community, residue_struct *Residue, const weather_struct *Weather, soil_struct *Soil, soilc_struct *SoilCarbon);
void            CropGrowth (int y, int doy, double *DailyGrowth, const double Stage, crop_struct *Crop, residue_struct *Residue, const weather_struct *Weather);
void            CropNitrogenDemand (double N_AbgdConcReq, double N_RootConcReq, double *N_ReqAbgdGrowth, double *N_ReqRootGrowth, double *N_ReqRhizodeposition, double *N_CropDemand, crop_struct *Crop);
void            CropNitrogenConcentration (double *N_AbgdConcReq, double *N_RootConcReq, double *NaAbgd, double *NxAbgd, double *NcAbgd, double *NnAbgd, double *NxRoot, const double Stage, const crop_struct *Crop);
void            CropNitrogenStress (const double NaAbgd, const double NcAbgd, const double NnAbgd, crop_struct *Crop);
void CropNitrogenUptake (double *N_ReqAbgdGrowth, double *N_ReqRootGrowth, double *N_ReqRhizodeposition, double *NxAbgd, double *NxRoot, int autoNitrogen, double NO3supply, double NH4supply, double *NO3Uptake, double *NH4Uptake, double *N_CropDemand, comm_struct *Community, soil_struct *Soil);
void            PotentialSoluteUptakeOption2 (double *SoluteSupply, double *SoluteUptake, double Kd, int totalLayers, const double *BD, const double *dz, const double *WaterUptake, const double *Solute, const double *WC);
double          ShootBiomassPartitioning (const double Stage, const double Po, const double Pf);
void            RadiationInterception (int y, int doy, comm_struct *Community);
void            Phenology (int y, int doy, const weather_struct *Weather, comm_struct *Community);
void            ComputeColdDamage (int y, int doy, crop_struct *Crop, const weather_struct *Weather, const snow_struct *Snow, residue_struct *Residue);
double          ColdDamage (const double T, const double Crop_Tn, const double Crop_Tth);
double TemperatureFunctionGrowth (double tMax, double tOpt, double tMin, double T);

/* CropThermalTime.c */
void            ComputeThermalTime (int total_years, comm_struct *Community, weather_struct *Weather);
double          ThermalTime (double T_base, double T_op, double T_Max, double Temperature);

/* CropTranspiration.c */
void            WaterUptake (int y, int doy, comm_struct *Community, soil_struct *Soil, const weather_struct *Weather);
void            CalcRootFraction (double *fractionRootsByLayer, soil_struct *Soil, crop_struct *Crop);
double          TemperatureLimitation (double T, double T_Min, double T_Threshold);

/* DailyOperation.c */
void DailyOperations (int y, int doy, cropmgmt_struct *CropManagement, comm_struct *Community, residue_struct *Residue, ctrl_struct *SimControl, snow_struct *Snow, soil_struct *Soil, soilc_struct *SoilCarbon, weather_struct *Weather, summary_struct *Summary, const char *project);
void            GrowingCrop (int rotationYear, int y, int d, comm_struct *Community, residue_struct *Residue, const ctrl_struct *SimControl, soil_struct *Soil, soilc_struct *SoilCarbon, const weather_struct *Weather, const snow_struct *Snow, const char *project);
void CropStage (int y, int d, comm_struct *Community, const weather_struct *Weather);
double          FinalHarvestDate (int lastDoy, int d);
int             ForcedClipping (int d, comm_struct *Community);
int             ForcedMaturity (int rotationYear, int d, int lastDoy, int nextSeedingYear, int nextSeedingDate, int rotationSize);

/* Fertilization.c */
void            ApplyFertilizer (op_struct *fixedFertilization, soil_struct *Soil, residue_struct *Residue);

/* FieldOperations.c */
void FieldOperation (int rotationYear, int y, int doy, cropmgmt_struct *CropManagement, comm_struct *Community, soil_struct *Soil, residue_struct *Residue, ctrl_struct *SimControl, soilc_struct *SoilCarbon, weather_struct *Weather, const char *project);
int IsOperationToday (int rotationYear, int doy, op_struct *FieldOperation, int numOperation, int *operationIndex);
void UpdateOperationStatus (op_struct *FieldOperation, int numOperation);

/* Initialize.c */
#ifndef _CYCLES_
void            Initialize (ctrl_struct *SimControl, weather_struct *Weather, soil_struct *Soil, residue_struct *Residue, soilc_struct *SoilCarbon, comm_struct *Community, cropmgmt_struct *CropManagement, snow_struct *Snow, summary_struct *Summary);
#endif
void FreeCyclesStruct (cropmgmt_struct *CropManagement, comm_struct *Community, soil_struct *Soil, weather_struct *Weather, residue_struct *Residue, soilc_struct *SoilCarbon, int total_years);
void            FirstDOY (int *rotationYear, int yearsInRotation, int totalLayers, soilc_struct *SoilCarbon, residue_struct *Residue, const soil_struct *Soil);
void LastDOY (int y, int simStartYear, int totalLayers, soil_struct *Soil, soilc_struct *SoilCarbon, residue_struct *Residue, summary_struct *Summary, const char *project);

/* Irrigation.c */
double          FindIrrigationVolume (int opLayer, double opWaterDepletion, const soil_struct *Soil);

/* MiscFunc.c */
int             IsLeapYear (int year);
int             doy (int year, int month, int mday, int leap_year_mode);
int             t2doy (time_t * rawtime);
void            doy2date (int year, int jday, int *month, int *mday, int leap_year_mode);
int             LT (double x, double y);
int             LE (double x, double y);
int             EQ (double x, double y);
int             GE (double x, double y);
int             GT (double x, double y);

/* Print.c */
void            InitializeOutput (char *project, const comm_struct *Community, int layers);
void            PrintDailyOutput (int y, int doy, int start_year, const weather_struct *Weather, const comm_struct *Community, const soil_struct *Soil, const snow_struct *Snow, const residue_struct *Residue, const char *project);
void            PrintSeasonOutput (int y, int doy, int start_year, const weather_struct *Weather, const crop_struct *Crop, const char *project);
void            PrintAnnualOutput (int y, int start_year, const soil_struct *Soil, const soilc_struct *SoilCarbon, const char *project);
void            PrintCarbonEvolution (int y, int start_year, int total_layers, const soil_struct *Soil, const soilc_struct *SoilCarbon, const residue_struct *Residue, const char *project);
void            StoreSummary (summary_struct *Summary, const soilc_struct *SoilCarbon, const residue_struct *Residue, int totalLayers, int y);
void            PrintSummary (const summary_struct *Summary, int totalYears, const char *project);

#ifndef _CYCLES_
/* ReadCrop.c */
void            ReadCrop (char *project, comm_struct *Community);
#endif

/* ReadFunc.c */
char FirstNonWhite (char *cmdstr);
int Readable (char *cmdstr);
int FindLine (FILE * fid, char *token);
void NextLine (FILE * fid, char *cmdstr);
int CountLine (FILE * fid, int num_arg, ...);
void CheckFile (FILE * fid, char *fn);
void ReadTS (char *cmdstr, int *ftime, double *data, int nvrbl);
int CountOccurance (FILE *fid, char *token);
void ReadKeywordDouble (char *buffer, char *keyword, double *value);
void ReadKeywordInt (char *buffer, char *keyword, int *value);
void ReadKeywordTime (char *buffer, char *keyword, int *value);
void ReadKeywordStr (char *buffer, char *keyword, char *value);

#ifndef _CYCLES_
/* ReadOperation.c */
void            ReadOperation (char *project, cropmgmt_struct *CropManagement, const comm_struct *Community, int yearsInRotation);

/* ReadSimControl.c */
void            ReadSimControl (char *project, ctrl_struct *SimControl);

/* ReadSoil.c */
void            ReadSoil (char *project, soil_struct *Soil);

/* ReadWeather.c */
void            ReadWeather (char *project, weather_struct *Weather, int start_year, int total_years);
#endif

int CropExist (char *cropName, const comm_struct *Community);

/* ReferenceET.c */
double          CalculatePMET (double lat, double pAtm, double screeningHeight, double Tmax, double Tmin, double sRad, double rhMax, double rhMin, double wind, double doy);
double          SaturatedVaporPressure (double T);
double          PotentialRadiation (double Lat, int doy);
double          NetRadiation (double Pot_Rad, double Solar_Rad, double Actual_VP, double TMax, double TMin);
double          Aero_Res (double uz, double z);

/* Residue.c */
void            InitializeResidue (residue_struct *Residue, int totalYears, int totalLayers);
void            ComputeResidueCover (residue_struct *Residue);
void            ResidueWetting (residue_struct *Residue, soil_struct *Soil);
void            ResidueEvaporation (residue_struct *Residue, soil_struct *Soil, const comm_struct *Community, double ETo, double snowCover);

/* Snow.c */
void            SnowProcesses (snow_struct *Snow, int y, int doy, weather_struct *Weather, double TauStandingRes, double CropInterception);
void            CalculateSnowFall (snow_struct *Snow, double Tavg, double PP);
void            CalculateSnowMelt (snow_struct *Snow, double Tavg, double Tx, double Tn);
void            CalculateSnowEvaporation (snow_struct *Snow, double TauStandingRes, double CropInterception, double ETo);
double          CalculateSnowCover (snow_struct *Snow);

/* Soil.c */
#ifdef _CYCLES_
void InitializeSoil (soil_struct *Soil, weather_struct *Weather, ctrl_struct *SimControl, const soil_struct *soil);
#else
void            InitializeSoil (soil_struct *Soil, weather_struct *Weather, ctrl_struct *SimControl);
#endif
#ifdef _CYCLES_
double SoilWaterPotential (double porosity, double thetar, double alpha, double beta, double water_content);
#else
double          SoilWaterPotential (double SaturationWC, double AirEntryPot, double Campbell_b, double WC);
#endif
double          VolumetricWCAt33Jkg (double Clay, double Sand, double OM);
double          VolumetricWCAt1500Jkg (double Clay, double Sand, double OM);
#ifdef _CYCLES_
double SoilWaterContent (double thetas, double thetar, double alpha, double beta, double psi);
#else
double          SoilWaterContent (double SaturationWC, double AirEntryPot, double Campbell_b, double Water_Potential);
#endif
double          BulkDensity (double Clay, double Sand, double OM);

/* SoilCarbon.c */
void            InitializeSoilCarbon (soilc_struct *SoilCarbon, int totalLayers);
void            ComputeFactorComposite (soilc_struct *SoilCarbon, int doy, int y, int last_doy, soil_struct *Soil);
void            ComputeSoilCarbonBalanceMB (soilc_struct *SoilCarbon, int y, residue_struct *Residue, soil_struct *Soil, double *tillageFactor);
void            ComputeSoilCarbonBalance (soilc_struct *SoilCarbon, int y, residue_struct *Residue, soil_struct *Soil, double *tillageFactor);
void            StoreOutput (soilc_struct *SoilCarbon, int y, int totalLayers, double *SOCMass);
double          Aeration (double AC);
double          Moisture (double wp);
double          TemperatureFunction (double T);
double          MaximumAbgdHumificationFactor (double clayFraction);
double          MaximumRootHumificationFactor (double clayFraction);
double          MaximumRhizHumificationFactor (double clayFraction);
double          MaximumManuHumificationFactor (double clayFraction);
double          NitrogenMineralization (double CNDecomposing, double CNnew, double humRate, double decomposedMass);
double          CNdestiny (double NmineralConc, double CNdecomposing);
double          PoolNitrogenMineralization (double NmineralConc, double CNRatioDecomposing, double humRate, double decomposedMass, double carbonConc);
double          Function_CNnew (double NmineralConc, double CNDecomposingPool);

/* SoilEvaporation.c */
void            Evaporation (soil_struct *Soil, const comm_struct *Community, residue_struct *Residue, double ETo, double SnowCover);
double          Depth_Limitation_To_Evaporation (double Depth);
double          Water_Content_Limitation_To_Evaporation (double FC, double WC_AirDry, double WC);

/* SoilInfiltration.c */
void            Redistribution (int y, int doy, double precipitation, double snowFall, double snowMelt, int hourlyInfiltration, const comm_struct *Community, soil_struct *Soil, residue_struct *Residue);
void            CascadeRedistribution (soil_struct *Soil);
void            SubDailyRedistribution (soil_struct *Soil);
void            CurveNumber (soil_struct *Soil);
double          K_Sat (double WCATSAT, double WCAT33, double b);
double          AverageHydraulicConductance (double WCSAT, double WCFC, double aep, double b, double SWP1, double SWP2, double ks);
double          Numerator (double aep, double b, double SWP);
double          Denominator (double aep, double b, double SWP);

/* SoilNitrogen.c */
void            NitrogenTransformation (int y, int doy, soil_struct *Soil, const comm_struct *Community, const residue_struct *Residue, const weather_struct *Weather, const soilc_struct *SoilCarbon);
void            Nitrification (double *Profile_N_Nitrified, double *Profile_N2O_Nitrified, soil_struct *Soil, const soilc_struct *SoilCarbon);
void            Denitrification (double *Profile_N_Denitrified, double *Profile_N2O_Denitrified, soil_struct *Soil, const soilc_struct *SoilCarbon);
void            Volatilization (int y, int doy, double *Profile_NH4_Volatilization, soil_struct *Soil, const comm_struct *Community, const residue_struct *Residue, const weather_struct *Weather);
double          N2OFractionNitrification (double air);
double          pHFunction (double pH);
double          VolatilizationDepthFunction (double depth);
double          AirMolarDensity (double T, double P);
double          BoundaryLayerConductance (double RI, double RM, double WS, double AMD);

/* SoilSolute.c */
void            SoluteTransport (int totalLayers, double Sol_Kd, double WInConc, double *leachate, const double *WFlux, double *soluteMass, const double *BD, const double *thickness, const double *porosity, const double *WCinitial);
void            SoluteTransportEvaporation (int totalLayers, double Sol_Kd, const double *WFlux, double *soluteMass, const double *BD, const double *thickness, const double *porosity, double *WCinitial);
double          LinearEquilibriumConcentration (double Kd, double bulkDensity, double layerThickness, double waterContent, double soluteMass);
double          LinearEquilibriumSoluteMass (double Kd, double bulkDensity, double layerThickness, double waterContent, double concentration);

/* SoilTemperature.c */
void            Temperature (int y, int doy, double snowCover, double cropInterception, soil_struct *Soil, weather_struct *Weather, residue_struct *Residue);
double          HeatCapacity (double bulkDensity, double volumetricWC);
double          HeatConductivity (double bulkDensity, double volumetricWC, double fractionClay);
double          EstimatedSoilTemperature (double nodeDepth, int doy, double annualAvgTemperature, double yearlyAmplitude, int phase, double dampingDepth);

/* Tillage.c */
void            ExecuteTillage (double *abgdBiomassInput, const op_struct *Tillage, double *tillageFactor, soil_struct *Soil, residue_struct *Residue);
void            TillageFactorSettling (double *tillageFactor, int totalLayers, const double *waterContent, const double *Porosity);
double          Fraction (double a, double b, double c, double d, double f);
void            ComputeTillageFactor (const op_struct *Tillage, double *tillageFactor, const soil_struct *Soil, const double *soilLayerBottom, double toolDepth);
double          ComputeTextureFactor (double Clay);

/* Weather.c */
double          SatVP (double T);
void            CalculateDerivedWeather (weather_struct *Weather, int total_years);

void            PrintSimContrl (ctrl_struct SimControl);
void            PrintSoil (soil_struct Soil);
void            PrintCrop (comm_struct Community);
void            PrintOperation (op_struct *plantedCrops, int NumPlanting, op_struct *Tillage, int NumTillage, op_struct *FixedIrrigation, int NumIrrigation, op_struct *FixedFertilization, int NumFertilization);
void            PrintWeather (weather_struct Weather);

#ifdef _CYCLES_
/* cycles_func.c */
void DailyVar (int t, int start_time, pihm_struct pihm);

void CyclesRead (char *simulation, CyclesStruct cycles, pihm_struct pihm);
void CyclesInit (CyclesStruct cycles, pihm_struct pihm);
void ReadSimControl (char *simulation, ctrl_struct *SimControl, const pihm_struct pihm);
void ReadSoilInit (char *simulation, soil_struct *Soil);
void ReadCrop (char *simulation, comm_struct *community);
void ReadOperation (char *filename, cropmgmt_struct *CropManagement, const comm_struct *Community, int yearsInRotation);
void ReadWeather (char *filename, weather_struct *Weather, int start_year, int total_years);
void DailyCycles (CyclesStruct cycles, pihm_struct pihm, int t, char *project);
#endif

#endif
