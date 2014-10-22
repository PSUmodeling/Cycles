#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#include "include/Cycles.h"

int ReadCrop (char *project, CyclesStruct Cycles)
{
    FILE           *crop_file;
    char           *filename;
    char            cmdstr[MAXSTRING];
    char            optstr[MAXSTRING];
    int             crop_counter = 0;
    int             i;

    printf ("Read crop description file.\n");

    /* Open simulation control file */

    filename = (char *)malloc ((strlen (project) + 12) * sizeof (char));
    sprintf (filename, "input/%s.crop", project);
    crop_file = fopen (filename, "r");
    free (filename);

    if (crop_file == NULL)
    {
        printf ("\nError: Cannot find the crop description file %s!\n", filename);
        exit (1);
    }

    /* Read crop description file */
    /* First count how many crop types are there in the description file */
    fgets (cmdstr, MAXSTRING, crop_file);

    while (!feof (crop_file))
    {
        if (cmdstr[0] != '#' && cmdstr[0] != '\n' && cmdstr[0] != '\0')
        {
            sscanf (cmdstr, "%s", optstr);
            if (strcasecmp ("NAME", optstr) == 0)
            {
                crop_counter = crop_counter + 1;
            }
        }
        fgets (cmdstr, MAXSTRING, crop_file);
    }

    /* Allocate memories for Crop classes */
    printf ("  Crop description file contains descriptions of %d crop types.\n", crop_counter);

    Cycles->Crop = (CropClass *) malloc (crop_counter * sizeof (CropClass));

    /* Rewind to the beginning of file and reset crop_counter */
    rewind (crop_file);
    crop_counter = 0;

    /* Read crop properties */
    fgets (cmdstr, MAXSTRING, crop_file);

    while (!feof (crop_file))
    {
        if (cmdstr[0] != '#' && cmdstr[0] != '\n' && cmdstr[0] != '\0')
        {
            sscanf (cmdstr, "%s", optstr);
            if (strcasecmp ("NAME", optstr) == 0)
            {
                sscanf (cmdstr, "%*s %s", &Cycles->Crop[crop_counter].cropName);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %d", &Cycles->Crop[crop_counter].userSeedingDate);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %d", &Cycles->Crop[crop_counter].userFloweringDate);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %d", &Cycles->Crop[crop_counter].userMaturityDate);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userMaximumSoilCoverage);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userMaximumRootingDepth);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userExpectedYieldAvg);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userExpectedYieldMax);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userExpectedYieldMin);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userPercentMoistureInYield);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userFractionResidueStanding);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userFractionResidueRemoved);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userClippingTiming);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userTranspirationMinTemperature);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userTranspirationThresholdTemperature);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userColdDamageMinTemperature);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userColdDamageThresholdTemperature);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userTemperatureBase);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userTemperatureOptimum);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userTemperatureMaximum);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userShootPartitionInitial);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userShootPartitionFinal);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userRadiationUseEfficiency);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userTranspirationUseEfficiency);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userHIx);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userHIo);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userHIk);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userEmergenceTT);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userNMaxConcentration);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userNDilutionSlope);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Cycles->Crop[crop_counter].userKc);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %d", &Cycles->Crop[crop_counter].userAnnual);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %d", &Cycles->Crop[crop_counter].userLegume);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %d", &Cycles->Crop[crop_counter].userC3orC4);

                crop_counter = crop_counter + 1;
            }
        }
        fgets (cmdstr, MAXSTRING, crop_file);
    }

    fclose (crop_file);

    return 0;
}
