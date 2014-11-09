#include "include/Cycles.h"

void ReadCrop (char *project, CropManagementStruct *CropManagement)
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
    CropManagement->NumDescribedCrop = crop_counter;
    CropManagement->describedCrops = (describedCropsStruct *) malloc (crop_counter * sizeof (describedCropsStruct));

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
                sscanf (cmdstr, "%*s %s", &CropManagement->describedCrops[i].userCropName);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %d", &CropManagement->describedCrops[i].userSeedingDate);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %d", &CropManagement->describedCrops[i].userFloweringDate);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %d", &CropManagement->describedCrops[i].userMaturityDate);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrops[i].userMaximumSoilCoverage);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrops[i].userMaximumRootingDepth);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrops[i].userExpectedYieldAvg);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrops[i].userExpectedYieldMax);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrops[i].userExpectedYieldMin);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrops[i].userPercentMoistureInYield);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrops[i].userFractionResidueStanding);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrops[i].userFractionResidueRemoved);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrops[i].userClippingTiming);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrops[i].userTranspirationMinTemperature);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrops[i].userTranspirationThresholdTemperature);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrops[i].userColdDamageMinTemperature);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrops[i].userColdDamageThresholdTemperature);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrops[i].userTemperatureBase);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrops[i].userTemperatureOptimum);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrops[i].userTemperatureMaximum);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrops[i].userShootPartitionInitial);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrops[i].userShootPartitionFinal);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrops[i].userRadiationUseEfficiency);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrops[i].userTranspirationUseEfficiency);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrops[i].userHIx);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrops[i].userHIo);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrops[i].userHIk);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrops[i].userEmergenceTT);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrops[i].userNMaxConcentration);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrops[i].userNDilutionSlope);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &CropManagement->describedCrops[i].userKc);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %d", &CropManagement->describedCrops[i].userAnnual);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %d", &CropManagement->describedCrops[i].userLegume);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %d", &CropManagement->describedCrops[i].userC3orC4);

                CropManagement->describedCrops[i].userMaximumSoilCoverage = CropManagement->describedCrops[i].userMaximumSoilCoverage * 0.94 / 100.;
                CropManagement->describedCrops[i].userPercentMoistureInYield = CropManagement->describedCrops[i].userPercentMoistureInYield / 100.;
                CropManagement->describedCrops[i].userFractionResidueStanding = CropManagement->describedCrops[i].userFractionResidueStanding / 100.;
                CropManagement->describedCrops[i].userFractionResidueRemoved = CropManagement->describedCrops[i].userFractionResidueRemoved / 100.;
                if (CropManagement->describedCrops[i].userClippingTiming != BADVAL)
                    CropManagement->describedCrops[i].userClippingTiming = CropManagement->describedCrops[i].userClippingTiming / 100.;
                else
                    CropManagement->describedCrops[i].userClippingTiming = 0.;
                CropManagement->describedCrops[i].calculatedFloweringTT = 0.;
                CropManagement->describedCrops[i].calculatedMaturityTT = 0.;
                CropManagement->describedCrops[i].calculatedSimAvgYield = 0.;
                CropManagement->describedCrops[i].calculatedSimMaxYield = 0.;
                CropManagement->describedCrops[i].calculatedSimMinYield = 0.;
                i++;
            }
        }
        fgets (cmdstr, MAXSTRING, crop_file);
    }

    fclose (crop_file);
}
