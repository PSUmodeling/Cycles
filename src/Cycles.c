#include "Cycles.h"

int             verbose_mode;
int             debug_mode;

int main (int argc, char *argv[])
{
    int             rotationYear;
    int             nextSeedingDate;
    int             nextSeedingYear;
    int             y;
    int             doy;
    int             i;
    int             c;
    clock_t         begin, end;

    CyclesStruct    Cycles;     /* Model structure */
    char           *project;    /* Name of simulation */

    begin = clock ();

    Cycles = (CyclesStruct) malloc (sizeof (*Cycles));

    //system ("clear");
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
        project = (char *)malloc ((strlen (argv[optind]) + 1) * sizeof (char));
        strcpy (project, argv[optind]);
    }

    printf ("Now running the %s simulation.\n\n", project);

    InitializeOutput (project);

    /* Read simulation control input file */
    ReadSimControl (project, &Cycles->SimControl);
    if (debug_mode)
        PrintSimContrl (Cycles->SimControl);

    /* Read soil description file */
    ReadSoil (Cycles->SimControl.soil_filename, &Cycles->Soil);
    if (debug_mode)
        PrintSoil (Cycles->Soil);

    /* Read crop description file */
    ReadCrop (Cycles->SimControl.crop_filename, &Cycles->CropManagement);
    if (debug_mode)
        PrintCrop (Cycles->CropManagement.describedCrop, Cycles->CropManagement.NumDescribedCrop);

    /* Read field operation file */
    ReadOperation (Cycles->SimControl.operation_filename, &Cycles->CropManagement, Cycles->SimControl.yearsInRotation);
    if (debug_mode)
        PrintOperation (Cycles->CropManagement.plantingOrder, Cycles->CropManagement.totalCropsPerRotation, Cycles->CropManagement.Tillage, Cycles->CropManagement.numTillage, Cycles->CropManagement.FixedIrrigation, Cycles->CropManagement.numIrrigation, Cycles->CropManagement.FixedFertilization, Cycles->CropManagement.numFertilization);

    /* Read meteorological driver */
    ReadWeather (Cycles->SimControl.weather_filename, &Cycles->Weather, Cycles->SimControl.simStartYear, Cycles->SimControl.totalYears);
    if (debug_mode)
        PrintWeather (Cycles->Weather);

    /* Initialize model variables and parameters */
    Initialize (&Cycles->SimControl, &Cycles->Weather, &Cycles->Soil, &Cycles->Residue, &Cycles->SoilCarbon, &Cycles->Crop, &Cycles->CropManagement, &Cycles->Snow);

    /* Compute crop thermal time */
    printf ("Compute crop thermal time.\n");
    ComputeThermalTime (Cycles->SimControl.totalYears, &Cycles->CropManagement, &Cycles->Weather);

    SelectCropInitialPosition (&Cycles->CropManagement);

    if (Cycles->CropManagement.totalCropsPerRotation > 0)
    {
        nextSeedingYear = Cycles->CropManagement.nextCropSeedingYear;
        nextSeedingDate = Cycles->CropManagement.nextCropSeedingDate;
    }
    else
    {
        nextSeedingYear = 0;
        nextSeedingDate = 0;
    }
    if (debug_mode)
        printf ("*Next seeding year is %-4d, next seeding date is %3d\n", nextSeedingYear, nextSeedingDate);

    rotationYear = 0;

    printf ("\nSimulation running ...\n");

    for (y = 0; y < Cycles->SimControl.totalYears; y++)
    {
        printf ("Year %4d\n", y + 1);
        if (rotationYear < Cycles->SimControl.yearsInRotation)
            rotationYear++;
        else
            rotationYear = 1;
        if (debug_mode)
            printf ("*%-15s = %-d\n", "Rotation year", rotationYear);

        SelectOperationYear (rotationYear, Cycles->CropManagement.Tillage, Cycles->CropManagement.numTillage, &Cycles->CropManagement.tillageIndex);
        SelectOperationYear (rotationYear, Cycles->CropManagement.FixedIrrigation, Cycles->CropManagement.numIrrigation, &Cycles->CropManagement.irrigationIndex);
        SelectOperationYear (rotationYear, Cycles->CropManagement.FixedFertilization, Cycles->CropManagement.numFertilization, &Cycles->CropManagement.fertilizationIndex);

        for (i = 0; i < Cycles->Soil.totalLayers; i++)
        {
            Cycles->SoilCarbon.carbonMassInitial[i] = Cycles->Soil.SOC_Mass[i];
            Cycles->SoilCarbon.carbonMassFinal[i] = 0.0;
            Cycles->SoilCarbon.annualHumifiedCarbonMass[i] = 0.0;
            Cycles->SoilCarbon.annualRespiredCarbonMass[i] = 0.0;
        }

        /* Daily operations */
        for (doy = 1; doy < 366; doy++)
        {
            if (debug_mode)
                printf ("DOY %3.3d\n", doy);
            DailyOperations (rotationYear, y, doy, &nextSeedingYear, &nextSeedingDate, &Cycles->CropManagement, &Cycles->Crop, &Cycles->Residue, &Cycles->SimControl, &Cycles->Snow, &Cycles->Soil, &Cycles->SoilCarbon, &Cycles->Weather, project);
            PrintDailyOutput (y, doy, Cycles->SimControl.simStartYear, &Cycles->Weather, &Cycles->Crop, &Cycles->Soil, &Cycles->Snow, &Cycles->Residue, project);
        }
    }

    FreeCyclesStruct (Cycles, Cycles->SimControl.totalYears);
    free (project);
    free (Cycles);

    end = clock ();

    printf ("\nSimulation time: %-lf seconds.\n", (double)(end - begin) / CLOCKS_PER_SEC);

    return (0);
}
