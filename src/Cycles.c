#include "Cycles.h"

int             verbose_mode;
int             debug_mode;
char            project[MAXSTRING];

int main (int argc, char *argv[])
{
    /*
     * Cycles main function
     * -----------------------------------------------------------------------
     * LOCAL VARIABLES
     *
     * Variable             Type        Description
     * ==========           ==========  ====================
     * y		    int
     * doy		    int
     * c		    int
     * begin_t		    time_t	Time Cycles simulation begins
     * end_t		    time_t	Time Cycles simulation ends
     * Cycles		    CyclesStruct
     */
    int             y;
    int             doy;
    int             c;
    time_t          begin_t, end_t;

    CyclesStruct    Cycles;     /* Model structure */
    char            filename[MAXSTRING];

    time (&begin_t);

    Cycles = (CyclesStruct)malloc (sizeof (*Cycles));

    printf ("\n\n");
    printf ("\t\t ######  ##    ##  ######  ##       ########  ######\n");
    printf ("\t\t##    ##  ##  ##  ##    ## ##       ##       ##    ##\n");
    printf ("\t\t##         ####   ##       ##       ##       ##\n");
    printf ("\t\t##          ##    ##       ##       ######    ######\n");
    printf ("\t\t##          ##    ##       ##       ##             ##\n");
    printf ("\t\t##    ##    ##    ##    ## ##       ##       ##    ##\n");
    printf ("\t\t ######     ##     ######  ######## ########  ######\n\n\n");

    verbose_mode = 0;

    while ((c = getopt (argc, argv, "vd")) != -1)
    {
        if (optind >= argc)
        {
            printf ("\nUsage: ./Cycles [-v] [-d] <project name>\n");
            printf ("\t-v Verbose mode\n");
            printf ("\t-d Debug mode\n");
            exit (1);
        }
        switch (c)
        {
            case 'v':
                verbose_mode = 1;
                printf ("Verbose mode turned on.\n");
                break;
            case 'd':
                debug_mode = 1;
                printf ("Debug mode turned on.\n");
                break;
            case '?':
                printf ("Option not recognisable %s\n", argv[optind]);
                break;
            default:
                break;
        }
    }

    if (optind >= argc)
    {
        printf ("ERROR: Please specify the name of project!\n");
        printf ("\nUsage: ./Cycles [-v] [-d] <project name>\n");
        printf ("\t-v Verbose mode\n");
        printf ("\t-d Ddebug mode\n");
        exit (1);
    }
    else
    {
        strcpy (project, argv[optind]);
    }

    printf ("Now running the %s simulation.\n\n", project);

    /*
     * Read input files
     */
    /* Read simulation control input file */
    sprintf (filename, "input/%s.ctrl", project);
    ReadSimControl (filename, &Cycles->SimControl);

    /* Read soil description file */
    ReadSoil (Cycles->SimControl.soil_filename, &Cycles->Soil);

    /* Read crop description file */
    ReadCrop (Cycles->SimControl.crop_filename, &Cycles->Community);

    /* Read field operation file */
    ReadOperation (Cycles->SimControl.operation_filename, &Cycles->CropManagement, &Cycles->Community, Cycles->SimControl.yearsInRotation);

    /* Read meteorological driver */
    ReadWeather (Cycles->SimControl.weather_filename, &Cycles->Weather, Cycles->SimControl.simStartYear, Cycles->SimControl.totalYears);

    /*
     * Initialize output files
     */
    InitializeOutput (&Cycles->Community, Cycles->Soil.totalLayers);

    /* 
     * Initialize model variables and parameters
     */
    Initialize (&Cycles->SimControl, &Cycles->Weather, &Cycles->Soil, &Cycles->Residue, &Cycles->SoilCarbon, &Cycles->Community, &Cycles->CropManagement, &Cycles->Snow, &Cycles->Summary);

    /* 
     * Compute crop thermal time
     */
    //ComputeThermalTime (Cycles->SimControl.totalYears, &Cycles->Community, &Cycles->Weather);

    /*
     * Daily Cycles simulation
     */
    printf ("\nSimulation running ...\n");

    for (y = 0; y < Cycles->SimControl.totalYears; y++)
    {
        printf ("Year %4d (%4d)\n", y + 1, Cycles->SimControl.simStartYear + y);

        /* Daily operations */
        for (doy = 1; doy < Cycles->Weather.lastDoy[y] + 1; doy++)
        {
            if (debug_mode)
                printf ("DOY %3.3d\n", doy);

            DailyOperations (y, doy, &Cycles->CropManagement, &Cycles->Community, &Cycles->Residue, &Cycles->SimControl, &Cycles->Snow, &Cycles->Soil, &Cycles->SoilCarbon, &Cycles->Weather, &Cycles->Summary);
            PrintDailyOutput (y, doy, Cycles->SimControl.simStartYear, &Cycles->Weather, &Cycles->Community, &Cycles->Soil, &Cycles->Snow, &Cycles->Residue);
        }
    }

    PrintSummary (&Cycles->Summary, Cycles->SimControl.totalYears);

    FreeCyclesStruct (&Cycles->CropManagement, &Cycles->Community, &Cycles->Soil, &Cycles->Weather, &Cycles->Residue, &Cycles->SoilCarbon, Cycles->SimControl.totalYears);

    free (Cycles);

    time (&end_t);

    printf ("\nSimulation time: %-d seconds.\n", (int)(end_t - begin_t));

    return (0);
}
