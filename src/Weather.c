#include "include/Cycles.h"

//    Public Sub SelectActiveWeatherDate(ByVal year As Integer, ByVal doy As Integer)
//        If year > 0 And (doy > 0 And doy < 367) Then
//            Me.yr = year
//            Me.dy = doy
//        Else
//            MsgBox("Passed values exceed tolerances" & vbCr & _
//                   "Year: " & year & vbCr & "Day: " & doy, MsgBoxStyle.Critical, location)
//        End If
//    End Sub


double SatVP (double T)
{
    return 0.6108 * exp(17.27 * T / (T + 237.3));
}

void CalculateDerivedWeather(WeatherClass *Weather, SimControlClass *SimControl)
{
    int     i, j;
    int     year_start, year0;
    double annualMaxTemperatureSum;
    double annualMinTemperatureSum;

    Weather->atmosphericPressure = 101.325 * exp(-Weather->siteAltitude / 8200.);   /* P = 101.3 * ((293 - 0.0065 * Altitude) / 293) ^ 5.26 */

    for (i = 0; i < Weather->length; i++)
    {
        Weather->ETref[i] = CalculatePMET(Weather->siteLatitude, Weather->atmosphericPressure, Weather->screeningHeight, Weather->tMax[i], Weather->tMin[i], Weather->solarRadiation[i], Weather->RHmax[i], Weather->RHmin[i], Weather->wind[i], Weather->jday[i]);
        if (i == 0)
        {
            year_start = 0;
            year0 = Weather->year[i];
            annualMaxTemperatureSum = Weather->tMax[i];
            annualMinTemperatureSum = Weather->tMin[i];
        }
        else if (i == Weather->length - 1)
        {
            i++;
            for (j = year_start; j < i; j++)
            {
                Weather->annualAverageTemperature[j] = (annualMaxTemperatureSum + annualMinTemperatureSum) / (double)(i - year_start) / 2.;
                Weather->yearlyAmplitude[j] = (annualMaxTemperatureSum - annualMinTemperatureSum) / (double)(i - year_start);
            }
        }
        else if (Weather->year[i] == year0)
        {
            annualMaxTemperatureSum = annualMaxTemperatureSum + Weather->tMax[i];
            annualMinTemperatureSum = annualMinTemperatureSum + Weather->tMin[i];
        }
        else if (Weather->year[i] > year0)
        {
            for (j = year_start; j < i; j++)
            {
                Weather->annualAverageTemperature[j] = (annualMaxTemperatureSum + annualMinTemperatureSum) / (double)(i - year_start) / 2.;
                Weather->yearlyAmplitude[j] = (annualMaxTemperatureSum - annualMinTemperatureSum) / (double)(i - year_start);
            }
            year_start = i;
            year0 = Weather->year[i];
            annualMaxTemperatureSum = Weather->tMax[i];
            annualMinTemperatureSum = Weather->tMin[i];
        }
    }
}

double annualAvgTemperature (WeatherClass *Weather, int yr)
{
    int i;

    for (i = 0; i < Weather->length; i++)
    {
        if (Weather->year[i] == yr)
        {
            return Weather->annualAverageTemperature[i];
            break;
        }
    }

    if (i >= Weather->length)
    {
        printf ("Cannot find the annual average temperature of %4.4d\n", yr);
        exit(1);
    }
}

double annualAmplitude (WeatherClass *Weather, int yr)
{
    int i;

    for (i = 0; i < Weather->length; i++)
    {
        if (Weather->year[i] == yr)
        {
            return Weather->yearlyAmplitude[i];
            break;
        }
    }

    if (i >= Weather->length)
    {
        printf ("Cannot find the yearly temperature amplitude of %4.4d\n", yr);
        exit(1);
    }
}

double dailyETref (WeatherClass *Weather, int yr, int dy)
{
    int i;

    for (i = 0; i < Weather->length; i++)
    {
        if (Weather->year[i] == yr && Weather->jday[i] == dy)
        {
            return Weather->ETref[i];
            break;
        }
    }

    if (i >= Weather->length)
    {
        printf ("Cannot find the potential ET on the %3d day of Year %4.4d\n", dy, yr);
        exit(1);
    }
} 
double dailyPrecipitation (WeatherClass *Weather, int yr, int dy)
{
    int i;

    for (i = 0; i < Weather->length; i++)
    {
        if (Weather->year[i] == yr && Weather->jday[i] == dy)
        {
            return Weather->precipitation[i];
            break;
        }
    }

    if (i >= Weather->length)
    {
        printf ("Cannot find the precipitation on the %3d day of Year %4.4d\n", dy, yr);
        exit(1);
    }
} 

double dailyRelativeHumidityMax (WeatherClass *Weather, int yr, int dy)
{
    int i;

    for (i = 0; i < Weather->length; i++)
    {
        if (Weather->year[i] == yr && Weather->jday[i] == dy)
        {
            return Weather->RHmax[i];
            break;
        }
    }

    if (i >= Weather->length)
    {
        printf ("Cannot find the max RH on the %3d day of Year %4.4d\n", dy, yr);
        exit(1);
    }
}   

double dailyRelativeHumidityMin (WeatherClass *Weather, int yr, int dy)
{
    int i;

    for (i = 0; i < Weather->length; i++)
    {
        if (Weather->year[i] == yr && Weather->jday[i] == dy)
        {
            return Weather->RHmin[i];
            break;
        }
    }

    if (i >= Weather->length)
    {
        printf ("Cannot find the min RH on the %3d day of Year %4.4d\n", dy, yr);
        exit(1);
    }
}      

double dailySolarRadiation (WeatherClass *Weather, int yr, int dy)
{
    int i;

    for (i = 0; i < Weather->length; i++)
    {
        if (Weather->year[i] == yr && Weather->jday[i] == dy)
        {
            return Weather->solarRadiation[i];
            break;
        }
    }

    if (i >= Weather->length)
    {
        printf ("Cannot find the solar radiation on the %3d day of Year %4.4d\n", dy, yr);
        exit(1);
    }
}      

double dailyTemperatureMax (WeatherClass *Weather, int yr, int dy)
{
    int i;

    for (i = 0; i < Weather->length; i++)
    {
        if (Weather->year[i] == yr && Weather->jday[i] == dy)
        {
            return Weather->tMax[i];
            break;
        }
    }

    if (i >= Weather->length)
    {
        printf ("Cannot find the max temperature on the %3d day of Year %4.4d\n", dy, yr);
        exit(1);
    }
}      

double dailyTemperatureMin (WeatherClass *Weather, int yr, int dy)
{
    int i;

    for (i = 0; i < Weather->length; i++)
    {
        if (Weather->year[i] == yr && Weather->jday[i] == dy)
        {
            return Weather->tMin[i];
            break;
        }
    }

    if (i >= Weather->length)
    {
        printf ("Cannot find the min temperature on the %3d day of Year %4.4d\n", dy, yr);
        exit(1);
    }
}      

double dailyWindSpeed (WeatherClass *Weather, int yr, int dy)
{
    int i;

    for (i = 0; i < Weather->length; i++)
    {
        if (Weather->year[i] == yr && Weather->jday[i] == dy)
        {
            return Weather->wind[i];
            break;
        }
    }

    if (i >= Weather->length)
    {
        printf ("Cannot find the wind speed on the %3d day of Year %4.4d\n", dy, yr);
        exit(1);
    }
}      
