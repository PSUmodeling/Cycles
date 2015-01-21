#include "include/Cycles.h"

int main (int argc, char *argv[])
{
    int rotationYear;
    int nextSeedingDate;
    int nextSeedingYear;
    int y;
    int doy;
    int i;

    CyclesStruct    Cycles;     /* Model structure */
    char           *project;    /* Name of simulation */

    Cycles = (CyclesStruct) malloc (sizeof (*Cycles));

#ifdef _DEBUG_
    project = (char *)malloc (5 * sizeof (char));
    strcpy (project, "Demo");
#else
    if (argc < 2)
    {
        printf ("ERROR: Please specify the name of project!\n");
        exit (1);
    }
    else
    {
        /* Get user specified project in command line */
        project = (char *)malloc ((strlen (argv[1]) + 1) * sizeof (char));
        strcpy (project, argv[1]);
    }
#endif

    system ("clear");
    printf ("\n\n");
    printf ("\t\t ######  ##    ##  ######  ##       ########  ######\n");
    printf ("\t\t##    ##  ##  ##  ##    ## ##       ##       ##    ##\n");
    printf ("\t\t##         ####   ##       ##       ##       ##\n");
    printf ("\t\t##          ##    ##       ##       ######    ######\n");
    printf ("\t\t##          ##    ##       ##       ##             ##\n");
    printf ("\t\t##    ##    ##    ##    ## ##       ##       ##    ##\n");
    printf ("\t\t ######     ##     ######  ######## ########  ######\n\n\n");

    printf ("Now running the %s simulation.\n\n", project);

    /* Create output directory */
    if (0 == (mkdir ("output", 0755)))
        printf (" Output directory was created.\n\n");

    /* Read simulation control input file */
    ReadSimControl (project, &Cycles->SimControl);
#ifdef _DEBUG_
    PrintSimContrl (Cycles->SimControl);
#endif

    /* Read soil description file */
    ReadSoil (project, &Cycles->Soil);
#ifdef _DEBUG_
    PrintSoil (Cycles->Soil);
#endif

    /* Read crop description file */
    ReadCrop (project, &Cycles->CropManagement);
#ifdef _DEBUG_
    PrintCrop (Cycles->CropManagement.describedCrop, Cycles->CropManagement.NumDescribedCrop);
#endif

    /* Read field operation file */
    ReadOperation (project, &Cycles->CropManagement, Cycles->SimControl.yearsInRotation);
#ifdef _DEBUG_
    PrintOperation (Cycles->CropManagement.plantingOrder, Cycles->CropManagement.totalCropsPerRotation, Cycles->CropManagement.Tillage, Cycles->CropManagement.numTillage, Cycles->CropManagement.FixedIrrigation, Cycles->CropManagement.numIrrigation, Cycles->CropManagement.FixedFertilization, Cycles->CropManagement.numFertilization);
#endif

    /* Read meteorological driver */
    ReadWeather (project, &Cycles->Weather, Cycles->SimControl.simStartYear, Cycles->SimControl.totalYears);
#ifdef _DEBUG_
    PrintWeather (Cycles->Weather);
#endif

    /* Initialize model variables and parameters */
    Initialize (&Cycles->SimControl, &Cycles->Weather, &Cycles->Soil, &Cycles->Residue, &Cycles->SoilCarbon, &Cycles->Crop, &Cycles->CropManagement);

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
#ifdef _DEBUG_
    printf ("*Next seeding year is %-4d, next seeding date is %3d\n", nextSeedingYear, nextSeedingDate);
#endif

    rotationYear = 0;

    printf ("\nSimulation running ...\n");

    for (y = 0; y < Cycles->SimControl.totalYears; y++)
    {
        printf ("Year %4d\n", y + 1);
        if (rotationYear < Cycles->SimControl.yearsInRotation)
            rotationYear++;
        else
            rotationYear = 1;
#ifdef _DEBUG_
        printf ("*%-15s = %-d\n", "Rotation year", rotationYear);
#endif

        SelectOperationYear (rotationYear, Cycles->CropManagement.Tillage, Cycles->CropManagement.numTillage, &Cycles->CropManagement.tillageIndex);
#ifdef _DEBUG_
//        printf ("*%-30s = %-d\n", "Active tillage index", Cycles->CropManagement.tillageIndex);
#endif
        SelectOperationYear (rotationYear, Cycles->CropManagement.FixedIrrigation, Cycles->CropManagement.numIrrigation, &Cycles->CropManagement.irrigationIndex);
#ifdef _DEBUG_
//        printf ("*%-30s = %-d\n", "Active irriggation index", Cycles->CropManagement.irrigationIndex);
#endif
        SelectOperationYear (rotationYear, Cycles->CropManagement.FixedFertilization, Cycles->CropManagement.numFertilization, &Cycles->CropManagement.fertilizationIndex);
#ifdef _DEBUG_
//        printf ("*%-30s = %-d\n", "Active fertilization index", Cycles->CropManagement.fertilizationIndex);
#endif

        for (i = 0; i < Cycles->Soil.totalLayers; i++)
        {
            Cycles->SoilCarbon.carbonMassInitial[i] = Cycles->Soil.SOC_Mass[i];
            Cycles->SoilCarbon.carbonMassFinal[i] = 0.;
            Cycles->SoilCarbon.annualHumifiedCarbonMass[i] = 0.;
            Cycles->SoilCarbon.annualRespiredCarbonMass[i] = 0.;
        } 

        /* Daily operations */
        for (doy = 1; doy < 366; doy++)
        {
            DailyOperations (rotationYear, y, doy, &nextSeedingYear, &nextSeedingDate, &Cycles->CropManagement, &Cycles->Crop, &Cycles->Residue, &Cycles->SimControl, &Cycles->Snow, &Cycles->Soil, &Cycles->SoilCarbon, &Cycles->Weather);
        }
    }
    return 0;
}
