#include "include/Cycles.h"

int ReadWeather (char *project, CyclesStruct Cycles)
{
    FILE           *weather_file;
    char           *filename;
    char            cmdstr[MAXSTRING];
    char            optstr[MAXSTRING];
    int             counter = 0;
    int             i, j;
    WeatherClass   *Weather;

    Weather = &Cycles->Weather;

    printf ("Read weather file.\n");

    /* Open simulation control file */

    filename = (char *)malloc ((strlen (project) + 15) * sizeof (char));
    sprintf (filename, "input/%s.weather", project);
    weather_file = fopen (filename, "r");

    if (weather_file == NULL)
    {
        printf ("\nError: Cannot find the weather file %s!\n", filename);
        exit (1);
    }

    free (filename);

    /* Read weather file */
    fgets (cmdstr, MAXSTRING, weather_file);

    /* Read in site information and count number of weather records */
    while (!feof (weather_file))
    {
        if (cmdstr[0] != '#' && cmdstr[0] != '\n' && cmdstr[0] != '\0')
        {
            sscanf (cmdstr, "%s", optstr);
            if (strcasecmp ("LATITUDE", optstr) == 0)
                sscanf (cmdstr, "%*s %lf", &Weather->siteLatitude);
            else if (strcasecmp ("ALTITUDE", optstr) == 0)
                sscanf (cmdstr, "%*s %lf", &Weather->siteAltitude);
            else if (strcasecmp ("SCREENING_HEIGHT", optstr) == 0)
                sscanf (cmdstr, "%*s %lf", &Weather->screeningHeight);
            else if (strcasecmp ("YEAR", optstr) == 0)
            {
            }
            else
                counter++;
        }
        fgets (cmdstr, MAXSTRING, weather_file);
    }


    Weather->length = counter;

    /* Allocate memories for weather forcing */
    Weather->wind = (double *)malloc (counter * sizeof (double));
    Weather->ETref = (double *)malloc (counter * sizeof (double));
    Weather->precipitation = (double *)malloc (counter * sizeof (double));
    Weather->RHmax = (double *)malloc (counter * sizeof (double));
    Weather->RHmin = (double *)malloc (counter * sizeof (double));
    Weather->solarRadiation = (double *)malloc (counter * sizeof (double));
    Weather->tMax = (double *)malloc (counter * sizeof (double));
    Weather->tMin = (double *)malloc (counter * sizeof (double));
    Weather->yearlyAmplitude = (double *)malloc (counter * sizeof (double));
    Weather->annualAverageTemperature = (double *)malloc (counter * sizeof (double));
    Weather->year = (int *)malloc (counter * sizeof (int));
    Weather->jday = (int *)malloc (counter * sizeof (int));

    /* Rewind to the beginning of file and read weather records */
    rewind (weather_file);
    fgets (cmdstr, MAXSTRING, weather_file);
    fgets (cmdstr, MAXSTRING, weather_file);
    fgets (cmdstr, MAXSTRING, weather_file);
    fgets (cmdstr, MAXSTRING, weather_file);

    for (i = 0; i < counter; i++)
    {
        fscanf (weather_file, "%d %d %lf %lf %lf %lf %lf %lf %lf", &Weather->year[i], &Weather->jday[i], &Weather->precipitation[i], &Weather->tMax[i], &Weather->tMin[i], &Weather->solarRadiation[i], &Weather->RHmax[i], &Weather->RHmin[i], &Weather->wind[i]);
        //        printf("%4.4d-%3.3d %lf %lf %lf %lf %lf %lf %lf %lf %lf\n", Weather->year[i], Weather->jday[i], Weather->yearlyAmplitude[i], Weather->annualAverageTemperature[i], Weather->tMax[i], Weather->tMin[i], Weather->RHmax[i], Weather->RHmin[i], Weather->precipitation[i], Weather->solarRadiation[i], Weather->wind[i]);
    }

    fclose (weather_file);

    return 0;
}
