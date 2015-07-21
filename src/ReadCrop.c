#include "Cycles.h"

void ReadCrop (char *filename, CommunityStruct *Community)
{
    /*
     * Read crop description file
     * -----------------------------------------------------------------------
     * LOCAL VARIABLES
     *
     * Variable             Type        Description
     * ==========           ==========  ====================
     * crop_file	    FILE*	File pointer of crop description file
     * fullname		    char*	Full file name of the crop description
     *					  file
     * cmdstr		    char[MAXSTRING]
     *					Command string
     * optstr		    char[MAXSTRING]
     *					Option argument string
     * crop_counter	    int		Crop counter
     * i		    int		Loop counter
     */
    FILE           *crop_file;
    char           *fullname;
    char            cmdstr[MAXSTRING];
    char            optstr[MAXSTRING];
    int             crop_counter = 0;
    int             i;
    CropStruct     *Crop;

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
        printf ("%-30s input/%s.\n", "Read crop description file:", filename);

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
    Community->NumCrop = crop_counter;
    Community->Crop = (CropStruct *)malloc (crop_counter * sizeof (CropStruct));

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
                Crop = &(Community->Crop[i]);
                sscanf (cmdstr, "%*s %s", Crop->cropName);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %d", &Crop->userSeedingDate);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %d", &Crop->userFloweringDate);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %d", &Crop->userMaturityDate);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Crop->userMaximumSoilCoverage);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Crop->userMaximumRootingDepth);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Crop->userExpectedYieldAvg);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Crop->userExpectedYieldMax);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Crop->userExpectedYieldMin);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Crop->userPercentMoistureInYield);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Crop->userFractionResidueStanding);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Crop->userFractionResidueRemoved);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Crop->userClippingBiomassThreshold);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Crop->userClippingTiming);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Crop->userTranspirationMinTemperature);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Crop->userTranspirationThresholdTemperature);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Crop->userColdDamageMinTemperature);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Crop->userColdDamageThresholdTemperature);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Crop->userTemperatureBase);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Crop->userTemperatureOptimum);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Crop->userTemperatureMaximum);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Crop->userShootPartitionInitial);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Crop->userShootPartitionFinal);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Crop->userRadiationUseEfficiency);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Crop->userTranspirationUseEfficiency);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Crop->userHIx);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Crop->userHIo);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Crop->userHIk);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Crop->userEmergenceTT);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Crop->userNMaxConcentration);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Crop->userNDilutionSlope);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Crop->userKc);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %d", &Crop->userAnnual);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %d", &Crop->userLegume);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %d", &Crop->userC3orC4);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Crop->LWP_StressOnset);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Crop->LWP_WiltingPoint);
                fgets (cmdstr, MAXSTRING, crop_file);
                sscanf (cmdstr, "%*s %lf", &Crop->transpirationMax);

                /* Convert units */
                Crop->userMaximumSoilCoverage = Crop->userMaximumSoilCoverage * 0.94 / 100.0;
                Crop->userPercentMoistureInYield = Crop->userPercentMoistureInYield / 100.0;
                Crop->userFractionResidueStanding = Crop->userFractionResidueStanding / 100.0;
                Crop->userFractionResidueRemoved = Crop->userFractionResidueRemoved / 100.0;
                if (Crop->userClippingTiming != BADVAL)
                    Crop->userClippingTiming = Crop->userClippingTiming / 100.0;
                else
                    Crop->userClippingTiming = 0.0;

                Crop->calculatedFloweringTT = 0.0;
                Crop->calculatedMaturityTT = 0.0;
                Crop->calculatedSimAvgYield = 0.0;
                Crop->calculatedSimMaxYield = 0.0;
                Crop->calculatedSimMinYield = 0.0;
                i++;
            }
        }
        fgets (cmdstr, MAXSTRING, crop_file);
    }

    fclose (crop_file);
}
