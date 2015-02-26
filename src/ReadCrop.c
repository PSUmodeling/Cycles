#include "Cycles.h"

void ReadCrop (char *filename, CropManagementStruct *CropManagement)
{
    FILE           *crop_file;
    char           *fullname;
    char            cmdstr[MAXSTRING];
    char            optstr[MAXSTRING];
    int             crop_counter = 0;
    int             i;

    /* Open simulation control file */

    fullname = (char *)malloc ((strlen (filename) + 7) * sizeof (char));
    sprintf (fullname, "input/%s", filename);
    crop_file = fopen (fullname, "r");

    if (crop_file == NULL)
    {
        printf ("\nError: Cannot find the crop description file %s!\n", filename);
        exit (1);
    }
    else
	printf ("Read crop description file: %s.\n", filename);

    free (fullname);

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
                strcpy (optstr, "\0");
                crop_counter = crop_counter + 1;
            }
        }
        fgets (cmdstr, MAXSTRING, crop_file);
    }

    /* Allocate memories for Crop classes */
    CropManagement->NumDescribedCrop = crop_counter;
    CropManagement->describedCrop = (describedCropStruct *) malloc (crop_counter * sizeof (describedCropStruct));

    /* Rewind to the beginning of file and reset crop_counter */
    rewind (crop_file);
    i = 0;

    /* Read crop properties */
    fgets (cmdstr, MAXSTRING, crop_file);

    while (!feof (crop_file))
    {
        if (cmdstr[0] != '#' && cmdstr[0] != '\n' && cmdstr[0] != '\0')
        {
            sscanf (cmdstr, "%s", optstr);
            if (strcasecmp ("NAME", optstr) == 0)
            {
                strcpy (optstr, "\0");
                sscanf (cmdstr, "%*s %s", &CropManagement->describedCrop[i].userCropName);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %d", &CropManagement->describedCrop[i].userSeedingDate);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %d", &CropManagement->describedCrop[i].userFloweringDate);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %d", &CropManagement->describedCrop[i].userMaturityDate);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrop[i].userMaximumSoilCoverage);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrop[i].userMaximumRootingDepth);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrop[i].userExpectedYieldAvg);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrop[i].userExpectedYieldMax);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrop[i].userExpectedYieldMin);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrop[i].userPercentMoistureInYield);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrop[i].userFractionResidueStanding);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrop[i].userFractionResidueRemoved);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrop[i].userClippingTiming);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrop[i].userTranspirationMinTemperature);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrop[i].userTranspirationThresholdTemperature);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrop[i].userColdDamageMinTemperature);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrop[i].userColdDamageThresholdTemperature);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrop[i].userTemperatureBase);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrop[i].userTemperatureOptimum);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrop[i].userTemperatureMaximum);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrop[i].userShootPartitionInitial);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrop[i].userShootPartitionFinal);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrop[i].userRadiationUseEfficiency);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrop[i].userTranspirationUseEfficiency);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrop[i].userHIx);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrop[i].userHIo);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrop[i].userHIk);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrop[i].userEmergenceTT);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrop[i].userNMaxConcentration);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrop[i].userNDilutionSlope);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrop[i].userKc);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %d", &CropManagement->describedCrop[i].userAnnual);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %d", &CropManagement->describedCrop[i].userLegume);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %d", &CropManagement->describedCrop[i].userC3orC4);

                /* Convert units */
                CropManagement->describedCrop[i].userMaximumSoilCoverage = CropManagement->describedCrop[i].userMaximumSoilCoverage * 0.94 / 100.0;
                CropManagement->describedCrop[i].userPercentMoistureInYield = CropManagement->describedCrop[i].userPercentMoistureInYield / 100.0;
                CropManagement->describedCrop[i].userFractionResidueStanding = CropManagement->describedCrop[i].userFractionResidueStanding / 100.0;
                CropManagement->describedCrop[i].userFractionResidueRemoved = CropManagement->describedCrop[i].userFractionResidueRemoved / 100.0;
                if (CropManagement->describedCrop[i].userClippingTiming != BADVAL)
                    CropManagement->describedCrop[i].userClippingTiming = CropManagement->describedCrop[i].userClippingTiming / 100.0;
                else
                    CropManagement->describedCrop[i].userClippingTiming = 0.0;
                CropManagement->describedCrop[i].calculatedFloweringTT = 0.0;
                CropManagement->describedCrop[i].calculatedMaturityTT = 0.0;
                CropManagement->describedCrop[i].calculatedSimAvgYield = 0.0;
                CropManagement->describedCrop[i].calculatedSimMaxYield = 0.0;
                CropManagement->describedCrop[i].calculatedSimMinYield = 0.0;
                i++;
            }
        }
        fgets (cmdstr, MAXSTRING, crop_file);
    }

    fclose (crop_file);
}
