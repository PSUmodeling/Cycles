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

