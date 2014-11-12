#include "include/Cycles.h"

int main (int argc, char *argv[])
{
    int rotationYear;
    int nextSeedingDate;
    int nextSeedingYear;
    int y;

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
    printf ("\t\t Copyright(c)2010-2015 PSU / WSU All rights reserved\n\n\n");

    printf ("Now running the %s simulation.\n\n", project);

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
    PrintCrop (Cycles->CropManagement.describedCrops, Cycles->CropManagement.NumDescribedCrop);
#endif


    /* Read field operation file */
    ReadOperation (project, &Cycles->CropManagement, Cycles->SimControl.yearsInRotation);
#ifdef _DEBUG_
    PrintOperation (Cycles->CropManagement.plantingOrder, Cycles->CropManagement.totalCropsPerRotation, Cycles->CropManagement.Tillage, Cycles->CropManagement.numTillage, Cycles->CropManagement.FixedIrrigation, Cycles->CropManagement.numIrrigation, Cycles->CropManagement.FixedFertilization, Cycles->CropManagement.numFertilization);
#endif

    /* Read meteorological driver */
    ReadWeather (project, &Cycles->Weather);
#ifdef _DEBUG_
    PrintWeather (Cycles->Weather);
#endif

    /* Initialize model variables and parameters */
    Initialize (&Cycles->SimControl, &Cycles->Weather, &Cycles->Soil, &Cycles->Residue);

    /* Compute crop thermal time */
    printf ("Compute crop thermal time.\n");
    ComputeThermalTime (Cycles->SimControl.simStartYear, Cycles->SimControl.simEndYear, &Cycles->CropManagement, &Cycles->Weather);

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

    for (y = Cycles->SimControl.simStartYear; y <= Cycles->SimControl.simEndYear; y++)
    {
        printf ("Year %4.4d\n", y);
        if (rotationYear < Cycles->SimControl.yearsInRotation)
            rotationYear++;
        else
            rotationYear = 1;
#ifdef _DEBUG_
        printf ("*%-30s = %-d\n", "Rotation year", rotationYear);
#endif

        SelectOperationYear (rotationYear, Cycles->CropManagement.Tillage, Cycles->CropManagement.numTillage, &Cycles->CropManagement.tillageIndex);
#ifdef _DEBUG_
        printf ("*%-30s = %-d\n", "Active tillage index", Cycles->CropManagement.tillageIndex);
#endif
        SelectOperationYear (rotationYear, Cycles->CropManagement.FixedIrrigation, Cycles->CropManagement.numIrrigation, &Cycles->CropManagement.irrigationIndex);
#ifdef _DEBUG_
        printf ("*%-30s = %-d\n", "Active irriggation index", Cycles->CropManagement.irrigationIndex);
#endif
        SelectOperationYear (rotationYear, Cycles->CropManagement.FixedFertilization, Cycles->CropManagement.numFertilization, &Cycles->CropManagement.fertilizationIndex);
#ifdef _DEBUG_
        printf ("*%-30s = %-d\n", "Active fertilization index", Cycles->CropManagement.fertilizationIndex);
#endif

    }
    return 0;
}
