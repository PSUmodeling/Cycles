#ifndef CYCLES_HEADER
#define CYCLES_HEADER

#define MAXSTRING 1024
typedef struct SimControlClass
{
    int *yearSpecificLastDOY;

    int Sim_Start_Year;
    int Sim_End_Year;
    int Total_Years;
    int Years_in_Rotation;

    int Adjusted_Yields;
    int Hourly_Infiltration;
    int Automatic_Nitrogen;
    int Automatic_Phosphorus;
    int Automatic_Sulfur;
    int Crop_Daily_Output;
    int Soil_Daily_Output;
    int Nitrogen_Daily_Output;
    int Water_Daily_Output;
    int Weather_Daily_Output;
    int Residue_Daily_Output;
    int SoilCarbon_Daily_Output;
    int Annual_Soil_Output;
    int Profile_Output;
    int Season_Output;
} SimControlClass;

typedef struct SoilClass_type
{
    //double *Sand;

    int totalLayers;
    int Curve_Number;
    double Percent_Slope;

    int annualTemperaturePhase;
    int dampingDepth;

    double *cumulativeDepth;                /* depth, in meters, to the bottom of that layer */
    double *nodeDepth;
    double *layerThickness;                 /* Measured in Meters */
    double *Clay;                            /* Clay fraction, reads clay % from spreadsheet */
    double *Sand;
    double *IOM;                             /* Initial Organic Matter */
    double *NO3;                             /* Nitrate(kg/ha) */
    double *NH4;                             /* Ammonium(kg/ha) */
    double *BD;                              /* Bulk Density (Mg/m3) */
    double *FC;                              /* Field Capacity water content */
    double *PWP;                             /* Permanent Wilting Point */
    double *Porosity;                                /* Saturation water content (m3/m3) */
    double *PAW;                             /* Maximum plant available water */
    double *FC_WaterPotential;               /* Estimate water potential at field capacity */
    double *airEntryPotential;               /* Calculate Air Entry Potential */
    double *B_Value;                         /* Calculated "B" value */
    double *M_Value;                         /* Calculated "M" value */

    double *n2o;                             /* temporary output of n2o per layer */

    double *SOC_Conc;                                /* g C / kg soil */
    double *SOC_Mass;                                /* Soil Organic Carbon, Mg/ha, factor 0.58 converts SOM to SOC */
    double *SON_Mass;                                /* Soil Organic Nitrogen, Mg/ha */
    double *MBC_Mass;                                /* Microbial Biomass Carbon, Mg/ha */
    double *MBN_Mass;                                /* Microbial Biomass Nitrogen, Mg/ha */
    double SOCProfile;
    double SONProfile;

    double C_Humified;                      /* Carbon humified from residues, roots, rizho, and manure */
    double C_ResidueRespired;               /* Carbon respired from residues, roots, rizho, and manure */
    double C_SoilRespired;                  /* Carbon respired from soil organic carbon only */

    double *soilTemperature;                             /* Celsius */
    double *waterContent;                               /* Volumetric water content, m3/m3 */
    double *waterUptake;                                /* layer water uptake  */
    double *pH;

    double evaporationVol;                              /* mm of water */
    double residueEvaporationVol;                       /* mm of water */
    double infiltrationVol;                             /* mm of water */
    double runoffVol;                                   /* mm of water */
    double irrigationVol;                               /* mm of water */
    double drainageVol;                                 /* mm of water */
    double NO3Leaching;                                 /* Mg N/ha */
    double NH4Leaching;                                 /* Mg N/ha */

    double NO3Profile;
    double NH4Profile;
    double N_Immobilization;
    double N_Mineralization;
    double N_NetMineralization;
    double NH4_Nitrification;
    double N2O_Nitrification;
    double NO3_Denitrification;
    double N2O_Denitrification;
    double NH4_Volatilization;
} SoilClass;

typedef struct CyclesStruct
{
    SimControlClass SimControl;
    SoilClass        Soil;
} *CyclesStruct;

/* Declare Cycles functions */
int ReadSimControl (char *project, SimControlClass *SimControl);
int ReadSoil (char *project, SoilClass *Soil);

#endif
