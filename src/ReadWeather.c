#include "Cycles.h"

void ReadWeather (char *filename, WeatherStruct *Weather, int start_year, int total_years)
{
    FILE           *weather_file;
    char           *fullname;
    char            cmdstr[MAXSTRING];
    char            optstr[MAXSTRING];
    int             y, doy;
    int             temp_year, temp_doy;
    char            start_year_str[5];

    /* Open simulation control file */

    fullname = (char *)malloc ((strlen (filename) + 7) * sizeof (char));
    sprintf (fullname, "input/%s", filename);
    weather_file = fopen (fullname, "r");

    if (weather_file == NULL)
    {
        printf ("\nError: Cannot find the weather file %s!\n", filename);
        exit (1);
    }
    else
	printf ("Read weather file: %s.\n", filename);

    free (fullname);

    /* Allocate */
    Weather->wind = (double **)malloc (total_years * sizeof (double *));
    Weather->ETref = (double **)malloc (total_years * sizeof (double *));
    Weather->precipitation = (double **)malloc (total_years * sizeof (double *));
    Weather->RHmax = (double **)malloc (total_years * sizeof (double *));
    Weather->RHmin = (double **)malloc (total_years * sizeof (double *));
    Weather->solarRadiation = (double **)malloc (total_years * sizeof (double *));
    Weather->tMax = (double **)malloc (total_years * sizeof (double *));
    Weather->tMin = (double **)malloc (total_years * sizeof (double *));
    Weather->yearlyAmplitude = (double *)malloc (total_years * sizeof (double));
    Weather->annualAverageTemperature = (double *)malloc (total_years * sizeof (double));
    Weather->lastDoy = (int *)malloc (total_years * sizeof (int));
    for (y = 0; y < total_years; y++)
    {
        Weather->wind[y] = (double *)malloc (366 * sizeof (double));
        Weather->ETref[y] = (double *)malloc (366 * sizeof (double));
        Weather->precipitation[y] = (double *)malloc (366 * sizeof (double));
        Weather->RHmax[y] = (double *)malloc (366 * sizeof (double));
        Weather->RHmin[y] = (double *)malloc (366 * sizeof (double));
        Weather->solarRadiation[y] = (double *)malloc (366 * sizeof (double));
        Weather->tMax[y] = (double *)malloc (366 * sizeof (double));
        Weather->tMin[y] = (double *)malloc (366 * sizeof (double));
    }

    sprintf (start_year_str, "%4.4d", start_year);
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
            //else if (strcasecmp ("YEAR", optstr) == 0)
            else if (strcasecmp (start_year_str, optstr) == 0)
            {
                for (y = 0; y < total_years; y++)
                {
                    for (doy = 1; doy < 367; doy++)
                    {
                        sscanf (cmdstr, "%d %d %*lf %*lf %*lf %*lf %*lf %*lf %*lf", &temp_year, &temp_doy);
			if (temp_year == y + start_year && temp_doy == doy)
			{
			    sscanf (cmdstr, "%*d %*d %lf %lf %lf %lf %lf %lf %lf", &Weather->precipitation[y][doy - 1], &Weather->tMax[y][doy - 1], &Weather->tMin[y][doy - 1], &Weather->solarRadiation[y][doy - 1], &Weather->RHmax[y][doy - 1], &Weather->RHmin[y][doy - 1], &Weather->wind[y][doy - 1]);
			    if (doy == 366)
				Weather->lastDoy[y] = 366;
			    fgets (cmdstr, MAXSTRING, weather_file);
			}
			else if (doy == 366 && temp_year == y + start_year + 1 && temp_doy == 1)
			    Weather->lastDoy[y] = 365;
			else if (doy == 366 && feof (weather_file))
			    Weather->lastDoy[y] = 365;
			else
			{
                            printf ("ERROR: Please check your weather input file near YEAR: %4.4d, DOY: %-d, expecting %4.4d-%-d, eof status %d\n", temp_year, temp_doy, y, doy, feof (weather_file));
                            exit (1);
			}
		    }
		}
		break;
	    }
        }
        fgets (cmdstr, MAXSTRING, weather_file);
    }

    fclose (weather_file);
}
