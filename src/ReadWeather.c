#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#include "include/Cycles.h"

int ReadWeather (char *project, CyclesStruct Cycles)
{
    FILE           *weather_file;
    char           *filename;
    char            cmdstr[MAXSTRING];
    char            optstr[MAXSTRING];
    int             counter = 0;
    int             i, j;
    int             year_start, year0;
    double          MaxTSum;
    double          MinTSum;
    WeatherClass   *Weather;

    Weather = &Cycles->Weather;

    printf ("Read weather file.\n");

    /* Open simulation control file */

    filename = (char *)malloc ((strlen (project) + 15) * sizeof (char));
    sprintf (filename, "input/%s.weather", project);
    weather_file = fopen (filename, "r");
    free (filename);

    if (weather_file == NULL)
    {
        printf ("\nError: Cannot find the weather file %s!\n", filename);
        exit (1);
    }

    /* Read weather file */
    fgets (cmdstr, MAXSTRING, weather_file);
    /* Read in site information and count number of weather records */
    while (!feof (weather_file))
    {
        if (cmdstr[0] != '#' && cmdstr[0] != '\n' && cmdstr[0] != '\0')
        {
            sscanf (cmdstr, "%s", optstr);
            if (strcasecmp ("LATITUDE", optstr) == 0)
                sscanf (cmdstr, "%*s %lf", &Weather->locationLatitude);
            else if (strcasecmp ("ALTITUDE", optstr) == 0)
                sscanf (cmdstr, "%*s %lf", &Weather->locationAltitude);
            else if (strcasecmp ("SCREENING_HEIGHT", optstr) == 0)
                sscanf (cmdstr, "%*s %lf", &Weather->screeningHeight);
            else if (strcasecmp ("YEAR", optstr)==0)
            {}
            else
                counter++;
        }
        fgets (cmdstr, MAXSTRING, weather_file);
    }

    printf ("%d\n", counter);

    Weather->wind           = (double *) malloc (counter * sizeof (double));
    Weather->ETref          = (double *) malloc (counter * sizeof (double));
    Weather->precipitation  = (double *) malloc (counter * sizeof (double));
    Weather->RHmax          = (double *) malloc (counter * sizeof (double));
    Weather->RHmin          = (double *) malloc (counter * sizeof (double));
    Weather->solarRadiation = (double *) malloc (counter * sizeof (double));
    Weather->tMax           = (double *) malloc (counter * sizeof (double));
    Weather->tMin           = (double *) malloc (counter * sizeof (double));
    Weather->yearlyAmplitude = (double *) malloc (counter * sizeof (double));
    Weather->annualAverageTemperature = (double *) malloc (counter * sizeof (double));
    Weather->year           = (int *) malloc (counter * sizeof (int));
    Weather->jday          = (int *) malloc (counter * sizeof (int));

    /* Rewind to the beginning of file */
    rewind (weather_file);
    fgets (cmdstr, MAXSTRING, weather_file);
    fgets (cmdstr, MAXSTRING, weather_file);
    fgets (cmdstr, MAXSTRING, weather_file);
    fgets (cmdstr, MAXSTRING, weather_file);

    for (i = 0; i < counter; i++)
    {
        fscanf (weather_file, "%d %d %lf %lf %lf %lf %lf %lf %lf", &Weather->year[i], &Weather->jday[i], &Weather->precipitation[i], &Weather->tMax[i], &Weather->tMin[i], &Weather->solarRadiation[i], &Weather->RHmax[i], &Weather->RHmin[i], &Weather->wind[i]);
//        printf("%4.4d-%3.3d %lf %lf %lf %lf %lf %lf %lf %lf %lf\n", Weather->year[i], Weather->jday[i], Weather->yearlyAmplitude[i], Weather->annualAverageTemperature[i], Weather->tMax[i], Weather->tMin[i], Weather->RHmax[i], Weather->RHmin[i], Weather->precipitation[i], Weather->solarRadiation[i], Weather->wind[i]);
        if (i == 0)
        {
            year_start = 0;
            year0 = Weather->year[i];
            MaxTSum = Weather->tMax[i];
            MinTSum = Weather->tMin[i];
        }
        else if (i == counter - 1)
        {
            i++;
            for (j = year_start; j < i; j++)
            {
                Weather->annualAverageTemperature[j] = (MaxTSum + MinTSum) / (double)(i - year_start) / 2.;
                Weather->yearlyAmplitude[j] = (MaxTSum - MinTSum) / (double)(i - year_start);
            }
        }
        else if (Weather->year[i] == year0)
        {
            MaxTSum = MaxTSum + Weather->tMax[i];
            MinTSum = MinTSum + Weather->tMin[i];
        }
        else if (Weather->year[i] > year0)
        {
            for (j = year_start; j < i; j++)
            {
                Weather->annualAverageTemperature[j] = (MaxTSum + MinTSum) / (double)(i - year_start) / 2.;
                Weather->yearlyAmplitude[j] = (MaxTSum - MinTSum) / (double)(i - year_start);
            }
            year_start = i;
            year0 = Weather->year[i];
            MaxTSum = Weather->tMax[i];
            MinTSum = Weather->tMin[i];
        }
    }
    for (i = 0; i < counter; i++)
        printf("%4.4d-%3.3d %lf %lf %lf %lf %lf %lf %lf %lf %lf\n", Weather->year[i], Weather->jday[i], Weather->yearlyAmplitude[i], Weather->annualAverageTemperature[i], Weather->tMax[i], Weather->tMin[i], Weather->RHmax[i], Weather->RHmin[i], Weather->precipitation[i], Weather->solarRadiation[i], Weather->wind[i]);
//    rewind (crop_file);
//    crop_counter = 0;
//
//    /* Read crop properties */
//    fgets (cmdstr, MAXSTRING, crop_file);
//
//    while (!feof (crop_file))
//    {
//        if (cmdstr[0] != '#' && cmdstr[0] != '\n' && cmdstr[0] != '\0')
//        {
//            sscanf (cmdstr, "%s", optstr);
//            if (strcasecmp ("NAME", optstr) == 0)
//            {
//                sscanf (cmdstr, "%*s %s", &Cycles->Crop[crop_counter].cropName);
//                fgets (cmdstr, MAXSTRING, crop_file);
//                sscanf (cmdstr, "%*s %d", &Cycles->Crop[crop_counter].userSeedingDate);
//                fgets (cmdstr, MAXSTRING, crop_file);
//                sscanf (cmdstr, "%*s %d", &Cycles->Crop[crop_counter].userFloweringDate);
//                fgets (cmdstr, MAXSTRING, crop_file);
//                sscanf (cmdstr, "%*s %d", &Cycles->Crop[crop_counter].userMaturityDate);
//                fgets (cmdstr, MAXSTRING, crop_file);
//                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userMaximumSoilCoverage);
//                fgets (cmdstr, MAXSTRING, crop_file);
//                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userMaximumRootingDepth);
//                fgets (cmdstr, MAXSTRING, crop_file);
//                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userExpectedYieldAvg);
//                fgets (cmdstr, MAXSTRING, crop_file);
//                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userExpectedYieldMax);
//                fgets (cmdstr, MAXSTRING, crop_file);
//                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userExpectedYieldMin);
//                fgets (cmdstr, MAXSTRING, crop_file);
//                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userPercentMoistureInYield);
//                fgets (cmdstr, MAXSTRING, crop_file);
//                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userFractionResidueStanding);
//                fgets (cmdstr, MAXSTRING, crop_file);
//                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userFractionResidueRemoved);
//                fgets (cmdstr, MAXSTRING, crop_file);
//                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userClippingTiming);
//                fgets (cmdstr, MAXSTRING, crop_file);
//                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userTranspirationMinTemperature);
//                fgets (cmdstr, MAXSTRING, crop_file);
//                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userTranspirationThresholdTemperature);
//                fgets (cmdstr, MAXSTRING, crop_file);
//                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userColdDamageMinTemperature);
//                fgets (cmdstr, MAXSTRING, crop_file);
//                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userColdDamageThresholdTemperature);
//                fgets (cmdstr, MAXSTRING, crop_file);
//                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userTemperatureBase);
//                fgets (cmdstr, MAXSTRING, crop_file);
//                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userTemperatureOptimum);
//                fgets (cmdstr, MAXSTRING, crop_file);
//                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userTemperatureMaximum);
//                fgets (cmdstr, MAXSTRING, crop_file);
//                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userShootPartitionInitial);
//                fgets (cmdstr, MAXSTRING, crop_file);
//                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userShootPartitionFinal);
//                fgets (cmdstr, MAXSTRING, crop_file);
//                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userRadiationUseEfficiency);
//                fgets (cmdstr, MAXSTRING, crop_file);
//                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userTranspirationUseEfficiency);
//                fgets (cmdstr, MAXSTRING, crop_file);
//                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userHIx);
//                fgets (cmdstr, MAXSTRING, crop_file);
//                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userHIo);
//                fgets (cmdstr, MAXSTRING, crop_file);
//                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userHIk);
//                fgets (cmdstr, MAXSTRING, crop_file);
//                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userEmergenceTT);
//                fgets (cmdstr, MAXSTRING, crop_file);
//                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userNMaxConcentration);
//                fgets (cmdstr, MAXSTRING, crop_file);
//                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userNDilutionSlope);
//                fgets (cmdstr, MAXSTRING, crop_file);
//                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userKc);
//                fgets (cmdstr, MAXSTRING, crop_file);
//                sscanf (cmdstr, "%*s %d", &Cycles->Crop[crop_counter].userAnnual);
//                fgets (cmdstr, MAXSTRING, crop_file);
//                sscanf (cmdstr, "%*s %d", &Cycles->Crop[crop_counter].userLegume);
//                fgets (cmdstr, MAXSTRING, crop_file);
//                sscanf (cmdstr, "%*s %d", &Cycles->Crop[crop_counter].userC3orC4);
//
//                crop_counter = crop_counter + 1;
//            }
//        }
//        fgets (cmdstr, MAXSTRING, crop_file);
//    }

    fclose (weather_file);

    return 0;
}
