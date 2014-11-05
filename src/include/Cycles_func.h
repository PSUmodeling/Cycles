#ifndef CYCLES_FUNC_HEADER
#define CYCLES_FUNC_HEADER

/* ReadSimControl.c */
int             ReadSimControl (char *project, CyclesStruct Cycles);

/* ReadSoil.c */
int             ReadSoil (char *project, CyclesStruct Cycles);

/* ReadCrop.c */
int             ReadCrop (char *project, CyclesStruct Cycles);

/* ReadOperation.c */
int             ReadOperation (char *project, CyclesStruct Cycles);

/* ReadWeather.c */
int             ReadWeather (char *project, CyclesStruct Cycles);

/* Initialize.c */
void            Initialize (CyclesStruct Cycles);

/* Soil.c */
void            InitializeSoil (SoilClass * Soil, WeatherClass * Weather, SimControlClass * SimControl);
double          SoilWaterPotential (double SaturationWC, double AirEntryPot, double Campbell_b, double WC);
double          VolumetricWCAt33Jkg (double Clay, double Sand, double OM);
double          VolumetricWCAt1500Jkg (double Clay, double Sand, double OM);
double          SoilWaterContent (double SaturationWC, double AirEntryPot, double Campbell_b, double Water_Potential);
double          BulkDensity (double Clay, double Sand, double OM);

/* LinkedList.c */
void            InsertOperation (FieldOperationListClass * OperationList, FieldOperationClass * Operation);

/* ReferenceET.c */
double          CalculatePMET (double lat, double pAtm, double screeningHeight, double Tmax, double Tmin, double sRad, double rhMax, double rhMin, double wind, double doy);
double          SaturatedVaporPressure (double T);
double          PotentialRadiation (double Lat, int doy);
double          NetRadiation (double Pot_Rad, double Solar_Rad, double Actual_VP, double TMax, double TMin);
double          Aero_Res (double uz, double z);

/* Weather.c */
double          SatVP (double T);
void            CalculateDerivedWeather (WeatherClass * Weather, SimControlClass * SimControl);
double          annualAvgTemperature (WeatherClass * Weather, int yr);
double          annualAmplitude (WeatherClass * Weather, int yr);
double          dailyETref (WeatherClass * Weather, int yr, int dy);
double          dailyPrecipitation (WeatherClass * Weather, int yr, int dy);
double          dailyRelativeHumidityMax (WeatherClass * Weather, int yr, int dy);
double          dailyRelativeHumidityMin (WeatherClass * Weather, int yr, int dy);
double          dailySolarRadiation (WeatherClass * Weather, int yr, int dy);
double          dailyTemperatureMax (WeatherClass * Weather, int yr, int dy);
double          dailyTemperatureMin (WeatherClass * Weather, int yr, int dy);
double          dailyWindSpeed (WeatherClass * Weather, int yr, int dy);

/* SoilTemperature.c */
double          HeatCapacity (double bulkDensity, double volumetricWC);
double          HeatConductivity (double bulkDensity, double volumetricWC, double fractionClay);
double          EstimatedSoilTemperature (double nodeDepth, int doy, double annualAvgTemperature, double yearlyAmplitude, int phase, double dampingDepth);

#ifdef _DEBUG_
void            PrintSimContrl (SimControlClass SimControl);
void            PrintSoil (SoilClass Soil);
void            PrintCrop (describedCropsStruct * describedCrops, int NumCrop);
void            PrintOperation (plantingOrderStruct * plantedCrops, int NumPlanting, FieldOperationListClass TillageList, FieldOperationListClass FixedIrrigationList, FieldOperationListClass FixedFertilizationList);
void            PrintWeather (WeatherClass Weather);
#endif

#endif
