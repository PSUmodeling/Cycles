typedef struct WeatherClass
{
    int yr;
    int dy;

    double  locationAltitude;
    double  locationLatitude;
    double  screeningHeight;
    double  *yearlyAmplitude;
    double  *annualAverageTemperature;
    double  **wind;
    double  **ETref;
    double  **precipitation;
    double  **RHmax;
    double  **RHmin;
    double  **solarRadiation;
    double  **tMax;
    double **tMin;
    double pAtm;
} WeatherClass;
