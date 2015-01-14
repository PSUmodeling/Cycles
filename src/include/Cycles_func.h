#ifndef CYCLES_FUNC_HEADER
#define CYCLES_FUNC_HEADER

/* ReadSimControl.c */
void ReadSimControl (char *project, SimControlStruct *SimControl);

/* ReadSoil.c */
void ReadSoil (char *project, SoilStruct *Soil);

/* ReadCrop.c */
void ReadCrop (char *project, CropManagementStruct *CropManagement);

/* ReadOperation.c */
void ReadOperation (char *project, CropManagementStruct *CropManagement, int yearsInRotation);

/* ReadWeather.c */
void ReadWeather (char *project, WeatherStruct *Weather, int start_year, int total_years);

/* Initialize.c */
void Initialize (SimControlStruct *SimControl, WeatherStruct *Weather, SoilStruct *Soil, ResidueStruct *Residue, SoilCarbonStruct *SoilCarbon, CropStruct *Crop);

/* Soil.c */
void            InitializeSoil (SoilStruct * Soil, WeatherStruct * Weather, SimControlStruct * SimControl);
double          SoilWaterPotential (double SaturationWC, double AirEntryPot, double Campbell_b, double WC);
double          VolumetricWCAt33Jkg (double Clay, double Sand, double OM);
double          VolumetricWCAt1500Jkg (double Clay, double Sand, double OM);
double          SoilWaterContent (double SaturationWC, double AirEntryPot, double Campbell_b, double Water_Potential);
double          BulkDensity (double Clay, double Sand, double OM);

/* LinkedList.c */

/* ReferenceET.c */
double          CalculatePMET (double lat, double pAtm, double screeningHeight, double Tmax, double Tmin, double sRad, double rhMax, double rhMin, double wind, double doy);
double          SaturatedVaporPressure (double T);
double          PotentialRadiation (double Lat, int doy);
double          NetRadiation (double Pot_Rad, double Solar_Rad, double Actual_VP, double TMax, double TMin);
double          Aero_Res (double uz, double z);

/* Weather.c */
double          SatVP (double T);
void            CalculateDerivedWeather (WeatherStruct * Weather, int total_years);

/* SoilTemperature.c */
double          HeatCapacity (double bulkDensity, double volumetricWC);
double          HeatConductivity (double bulkDensity, double volumetricWC, double fractionClay);
double          EstimatedSoilTemperature (double nodeDepth, int doy, double annualAvgTemperature, double yearlyAmplitude, int phase, double dampingDepth);

/* Residue.c */
void InitializeResidue (ResidueStruct *Residue, int totalYears, int totalLayers);
void ComputeResidueCover (ResidueStruct *Residue);

/* CropThermalTime.c */
void ComputeThermalTime (int total_years, CropManagementStruct *CropManagement, WeatherStruct *Weather);
double ThermalTime (double T_base, double T_op, double T_Max, double Temperature);

/* Crops.c */
void SelectCropInitialPosition(CropManagementStruct *CropManagement);
void SelectNextCrop (CropManagementStruct *CropManagement);
void PeekNextCrop(CropManagementStruct *CropManagement);

/* FieldOperations.c */
void SelectNextOperation(FieldOperationStruct *FieldOperation, int NumOperation, int *operationIndex);
void SelectOperationYear (int rotationYear, FieldOperationStruct *FieldOperation, int NumOperation, int *operationIndex);
int IsOperationToday (int rotationYear, int doy, FieldOperationStruct *FieldOperation, int operationIndex);

/* SoilCarbon.c */
void InitializeSoilCarbon (SoilCarbonStruct *SoilCarbon, int totalLayers);
void ComputeFactorComposite (SoilCarbonStruct *SoilCarbon, int doy, int y, SoilStruct *Soil);
void ComputeSoilCarbonBalanceMB (SoilCarbonStruct *SoilCarbon, int y, ResidueStruct *Residue, SoilStruct *Soil, FieldOperationStruct *Tillage);
void StoreOutput (SoilCarbonStruct *SoilCarbon, int y, int totalLayers, double *SOCMass);
double Aeration(double AC);
double Moisture (double wp);
double TemperatureFunction (double T);
double MaximumAbgdHumificationFactor (double clayFraction);
double MaximumRootHumificationFactor (double clayFraction);
double MaximumRhizHumificationFactor (double clayFraction);
double MaximumManuHumificationFactor (double clayFraction);
double NitrogenMineralization (double CNDecomposing, double CNnew, double humRate, double decomposedMass);
double CNdestiny (double  NmineralConc, double CNdecomposing);
double PoolNitrogenMineralization (double NmineralConc, double CNRatioDecomposing, double humRate, double decomposedMass, double carbonConc);
double Function_CNnew (double NmineralConc, double CNDecomposingPool);

/* CropTranspiration.c */
void WaterUptake (int y, int doy, CropStruct *Crop, SoilStruct *Soil, WeatherStruct *Weather);
void CalcRootFraction (double *fractionRootsByLayer, SoilStruct *Soil As SoilClass, CropStruct Crop);
double TemperatureLimitation (double T, double T_Min, double T_Threshold);
double Aeration(double AC);

#ifdef _DEBUG_
void            PrintSimContrl (SimControlStruct SimControl);
void            PrintSoil (SoilStruct Soil);
void            PrintCrop (describedCropsStruct * describedCrops, int NumCrop);
void PrintOperation (plantingOrderStruct * plantedCrops, int NumPlanting, FieldOperationStruct *Tillage, int NumTillage, FieldOperationStruct *FixedIrrigation, int NumIrrigation, FieldOperationStruct *FixedFertilization, int NumFertilization);
void            PrintWeather (WeatherStruct Weather);
void            PrintPlantingOrder (plantingOrderStruct *plantingOrder, int totalCropsPerRotation);
#endif

#endif
