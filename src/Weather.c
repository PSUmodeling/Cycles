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

    Weather->pAtm = 101.325 * exp(-Weather->locationAltitude / 8200.);   /* P = 101.3 * ((293 - 0.0065 * Altitude) / 293) ^ 5.26 */

    for (i = 0; i < Weather->length; i++)
    {
        Weather->ETref[i] = CalculatePMET(Weather->locationLatitude, Weather->pAtm, Weather->screeningHeight, Weather->tMax[i], Weather->tMin[i], Weather->solarRadiation[i], Weather->RHmax[i], Weather->RHmin[i], Weather->wind[i], Weather->jday[i]);
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
