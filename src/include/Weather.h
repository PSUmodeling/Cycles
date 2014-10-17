#ifndef WEATHER_HEADER
    #define WEATHER_HEADER
//class SnowClass;

class WeatherClass
{
    friend class SnowClass;
public:
    WeatherClass();
    WeatherClass(double Prcp, double Tmax, double Tmin, double ET);
private:
    double  dailyPrecipitation; 
    double  dailyTemperatureMax;
    double  dailyTemperatureMin;
    double  dailyETref;
};

inline WeatherClass::WeatherClass(double Prcp, double Tmax, double Tmin, double ET)
{
    dailyPrecipitation = Prcp;
    dailyTemperatureMax = Tmax;
    dailyTemperatureMin = Tmin;
    dailyETref = ET;
}
#endif
