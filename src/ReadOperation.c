#include "Cycles.h"

void ReadOperation (char *filename, CropManagementStruct *CropManagement, const CommunityStruct *Community, int yearsInRotation)
{
    /*
     * Read field operation description file
     * -----------------------------------------------------------------------
     * LOCAL VARIABLES
     *
     * Variable             Type        Description
     * ==========           ==========  ====================
     * operation_file	    FILE*	File pointer of fiedl operation file
     * fullname		    char*	Full file name of the field operation
     *					  file
     * cmdstr		    char[MAXSTRING]
     *					Command string
     * optstr		    char[MAXSTRING]
     *					Option argument string
     * tillage_counter	    int		Tillage operation counter
     * planting_counter	    int		Planting operatin counter
     * irrigation_counter   int		Fixed irrigation operation counter
     * fertilization_counter
     *			    int		Fixed fertilization operation counter
     * auto_irrigation_counter
     *			    int		Automatic irrigation operation counter
     * harvest_counter	    int		Forced harvest operation counter
     * tempyear		    int
     * i		    int		Loop counter
     * j		    int		Loop counter
     * q		    FieldOperationStruct*
     */
    FILE           *operation_file;
    char           *fullname;
    char            cmdstr[MAXSTRING];
    char            optstr[MAXSTRING];
    int             tillage_counter = 0;
    int             planting_counter = 0;
    int             irrigation_counter = 0;
    int             fertilization_counter = 0;
    int             auto_irrigation_counter = 0;
    int             harvest_counter = 0;
    int             tempyear;
    int             i, j;
    FieldOperationStruct *q;

    /* Open field operation file */
    fullname = (char *)malloc ((strlen (filename) + 7) * sizeof (char));
    sprintf (fullname, "input/%s", filename);
    operation_file = fopen (fullname, "r");

    if (operation_file == NULL)
    {
        printf ("ERROR: Cannot find the field operation file %s!\n", filename);
        exit (1);
    }
    else
        printf ("%-30s input/%s.\n", "Read field operation file:", filename);

    free (fullname);

    /* Read field operation file and count numbers of operations */
    fgets (cmdstr, MAXSTRING, operation_file);

    while (!feof (operation_file))
    {
        if (cmdstr[0] != '#' && cmdstr[0] != '\n' && cmdstr[0] != '\0')
        {
            sscanf (cmdstr, "%s", optstr);
            if (strcasecmp ("PLANTING", optstr) == 0)
            {
                strcpy (optstr, "\0");
                fgets (cmdstr, MAXSTRING, operation_file);
                sscanf (cmdstr, "%*s %d", &tempyear);
                if (tempyear <= yearsInRotation)
                    planting_counter++;
            }
            else if (strcasecmp ("FORCED_HARVEST", optstr) == 0)
            {
                strcpy (optstr, "\0");
                fgets (cmdstr, MAXSTRING, operation_file);
                sscanf (cmdstr, "%*s %d", &tempyear);
                if (tempyear <= yearsInRotation)
                    harvest_counter++;
            }
            else if (strcasecmp ("TILLAGE", optstr) == 0)
            {
                tillage_counter++;
                strcpy (optstr, "\0");
            }
            else if (strcasecmp ("FIXED_IRRIGATION", optstr) == 0)
            {
                irrigation_counter++;
                strcpy (optstr, "\0");
            }
            else if (strcasecmp ("FIXED_FERTILIZATION", optstr) == 0)
            {
                fertilization_counter++;
                strcpy (optstr, "\0");
            }
            else if (strcasecmp ("AUTO_IRRIGATION", optstr) == 0)
            {
                auto_irrigation_counter++;
                strcpy (optstr, "\0");
            }
        }
        fgets (cmdstr, MAXSTRING, operation_file);
    }

    /* Allocate memories for field operation classes */
    CropManagement->totalCropsPerRotation = planting_counter;
    CropManagement->plantingOrder = (FieldOperationStruct *)malloc (planting_counter * sizeof (FieldOperationStruct));

    CropManagement->numHarvest = harvest_counter;
    CropManagement->ForcedHarvest = (FieldOperationStruct *)malloc (harvest_counter * sizeof (FieldOperationStruct));

    CropManagement->numFertilization = fertilization_counter;
    CropManagement->FixedFertilization = (FieldOperationStruct *)malloc (fertilization_counter * sizeof (FieldOperationStruct));

    CropManagement->numIrrigation = irrigation_counter;
    CropManagement->FixedIrrigation = (FieldOperationStruct *)malloc (irrigation_counter * sizeof (FieldOperationStruct));

    CropManagement->numTillage = tillage_counter;
    CropManagement->Tillage = (FieldOperationStruct *)malloc (tillage_counter * sizeof (FieldOperationStruct));

    CropManagement->autoIrrigation = (autoIrrigationStruct *)malloc (auto_irrigation_counter * sizeof (autoIrrigationStruct));
    CropManagement->usingAutoIrr = 0;

    if (planting_counter)
    {
        /* Rewind to the beginning of file and read all planting operations */
        rewind (operation_file);
        i = 0;

        fgets (cmdstr, MAXSTRING, operation_file);
        while (!feof (operation_file))
        {
            if (cmdstr[0] != '#' && cmdstr[0] != '\n' && cmdstr[0] != '\0')
            {
                sscanf (cmdstr, "%s", optstr);
                if (strcasecmp ("PLANTING", optstr) == 0)
                {
                    strcpy (optstr, "\0");
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %d", &tempyear);
                    if (tempyear <= yearsInRotation)
                    {
                        CropManagement->plantingOrder[i].opYear = tempyear;
                        fgets (cmdstr, MAXSTRING, operation_file);
                        sscanf (cmdstr, "%*s %d", &CropManagement->plantingOrder[i].opDay);
                        fgets (cmdstr, MAXSTRING, operation_file);
                        sscanf (cmdstr, "%*s %s", CropManagement->plantingOrder[i].cropName);
                        fgets (cmdstr, MAXSTRING, operation_file);
                        sscanf (cmdstr, "%*s %d", &CropManagement->plantingOrder[i].usesAutoIrrigation);
                        if (CropManagement->plantingOrder[i].usesAutoIrrigation == 0)
                            CropManagement->plantingOrder[i].usesAutoIrrigation = -1;
                        else
                            CropManagement->usingAutoIrr = 1;
                        fgets (cmdstr, MAXSTRING, operation_file);
                        sscanf (cmdstr, "%*s %d", &CropManagement->plantingOrder[i].usesAutoFertilization);
                        if (CropManagement->plantingOrder[i].usesAutoFertilization == 0)
                            CropManagement->plantingOrder[i].usesAutoFertilization = -1;
                        fgets (cmdstr, MAXSTRING, operation_file);
                        sscanf (cmdstr, "%*s %lf", &CropManagement->plantingOrder[i].laiFraction);

                        CropManagement->plantingOrder[i].status = 0;

                        /* Link planting order and crop description */
                        for (j = 0; j < Community->NumCrop; j++)
                        {
                            if (strcmp (CropManagement->plantingOrder[i].cropName, Community->Crop[j].cropName) == 0)
                            {
                                CropManagement->plantingOrder[i].plantID = j;
                                break;
                            }
                        }
                        if (j >= Community->NumCrop)
                        {
                            printf ("ERROR: Cannot find the plant description of %s, please check your input file\n", CropManagement->plantingOrder[i].cropName);
                            exit (1);
                        }
                        i++;
                    }
                    else
                        printf ("Planting operation in year %d is not read in because years in rotation is %d\n", tempyear, yearsInRotation);
                }
            }
            fgets (cmdstr, MAXSTRING, operation_file);
        }
    }

    if (harvest_counter)
    {
        /* Rewind to the beginning of file and read all forced harvest operations */
        rewind (operation_file);
        i = 0;

        fgets (cmdstr, MAXSTRING, operation_file);
        while (!feof (operation_file))
        {
            if (cmdstr[0] != '#' && cmdstr[0] != '\n' && cmdstr[0] != '\0')
            {
                sscanf (cmdstr, "%s", optstr);
                if (strcasecmp ("FORCED_HARVEST", optstr) == 0)
                {
                    strcpy (optstr, "\0");
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %d", &tempyear);
                    if (tempyear <= yearsInRotation)
                    {
                        CropManagement->ForcedHarvest[i].opYear = tempyear;
                        fgets (cmdstr, MAXSTRING, operation_file);
                        sscanf (cmdstr, "%*s %d", &CropManagement->ForcedHarvest[i].opDay);
                        fgets (cmdstr, MAXSTRING, operation_file);
                        sscanf (cmdstr, "%*s %s", CropManagement->ForcedHarvest[i].cropName);

                        CropManagement->ForcedHarvest[i].status = 0;

                        /* Link forced harvest and crop description */
                        for (j = 0; j < Community->NumCrop; j++)
                        {
                            if (strcmp (CropManagement->ForcedHarvest[i].cropName, Community->Crop[j].cropName) == 0)
                            {
                                CropManagement->ForcedHarvest[i].plantID = j;
                                break;
                            }
                        }
                        if (j >= Community->NumCrop)
                        {
                            printf ("ERROR: Cannot find the plant description of %s, please check your input file\n", CropManagement->ForcedHarvest[i].cropName);
                            exit (1);
                        }
                        i++;
                    }
                    else
                        printf ("Forced harvest operation in year %d is not read in because years in rotation is %d\n", tempyear, yearsInRotation);
                }
            }
            fgets (cmdstr, MAXSTRING, operation_file);
        }
    }

    if (tillage_counter)
    {
        /* Rewind to the beginning of file and read all tillage operations */
        rewind (operation_file);
        i = 0;

        fgets (cmdstr, MAXSTRING, operation_file);
        while (!feof (operation_file))
        {
            if (cmdstr[0] != '#' && cmdstr[0] != '\n' && cmdstr[0] != '\0')
            {
                sscanf (cmdstr, "%s", optstr);
                if (strcasecmp ("TILLAGE", optstr) == 0)
                {
                    strcpy (optstr, "\0");
                    q = &(CropManagement->Tillage[i]);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %d", &q->opYear);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %d", &q->opDay);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %s", q->opToolName);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %lf", &q->opDepth);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %lf", &q->opSDR);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %lf", &q->opMixingEfficiency);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %s", q->cropNameT);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %lf", &q->fractionThermalTime);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %lf", &q->killEfficiency);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %d", &q->grainHarvest);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %lf", &q->forageHarvest);

                    q->status = 0;

                    i++;
                }
            }
            fgets (cmdstr, MAXSTRING, operation_file);
        }
    }

    if (irrigation_counter)
    {
        /* Rewind to the beginning of file and read all irrigation
         * operations */
        rewind (operation_file);
        i = 0;

        fgets (cmdstr, MAXSTRING, operation_file);
        while (!feof (operation_file))
        {
            if (cmdstr[0] != '#' && cmdstr[0] != '\n' && cmdstr[0] != '\0')
            {
                sscanf (cmdstr, "%s", optstr);
                if (strcasecmp ("FIXED_IRRIGATION", optstr) == 0)
                {
                    strcpy (optstr, "\0");
                    q = &(CropManagement->FixedIrrigation[i]);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %d", &q->opYear);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %d", &q->opDay);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %lf", &q->opVolume);

                    q->status = 0;

                    i++;
                }
            }
            fgets (cmdstr, MAXSTRING, operation_file);
        }
    }

    if (fertilization_counter)
    {
        /* Rewind to the beginning of file and read all fertilization
         * operations */
        rewind (operation_file);
        i = 0;

        fgets (cmdstr, MAXSTRING, operation_file);
        while (!feof (operation_file))
        {
            if (cmdstr[0] != '#' && cmdstr[0] != '\n' && cmdstr[0] != '\0')
            {
                sscanf (cmdstr, "%s", optstr);
                if (strcasecmp ("FIXED_FERTILIZATION", optstr) == 0)
                {
                    strcpy (optstr, "\0");
                    q = &(CropManagement->FixedFertilization[i]);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %d", &q->opYear);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %d", &q->opDay);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %s", q->opSource);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %lf", &q->opMass);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %s", q->opForm);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %s", q->opMethod);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %d", &q->opLayer);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %lf", &q->opC_Organic);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %lf", &q->opC_Charcoal);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %lf", &q->opN_Organic);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %lf", &q->opN_Charcoal);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %lf", &q->opN_NH4);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %lf", &q->opN_NO3);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %lf", &q->opP_Organic);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %lf", &q->opP_Charcoal);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %lf", &q->opP_Inorganic);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %lf", &q->opK);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %lf", &q->opS);

                    q->status = 0;

                    if (q->opC_Organic + q->opC_Charcoal + q->opN_Organic + q->opN_Charcoal + q->opN_NH4 + q->opN_NO3 + q->opP_Organic + q->opP_Charcoal + q->opP_Inorganic + q->opK + q->opS <= 1.0)
                    {
                        q->opMass = q->opMass / 1000.0;
                        i++;
                    }
                    else
                    {
                        printf ("ERROR: Added fertilization fractions must be <= 1\n");
                        exit (1);
                    }
                }
            }
            fgets (cmdstr, MAXSTRING, operation_file);
        }
    }

    if (CropManagement->usingAutoIrr)
    {
        /* Rewind to the beginning of file and read all planting operations */
        rewind (operation_file);
        i = 0;

        fgets (cmdstr, MAXSTRING, operation_file);
        while (!feof (operation_file))
        {
            if (cmdstr[0] != '#' && cmdstr[0] != '\n' && cmdstr[0] != '\0')
            {
                sscanf (cmdstr, "%s", optstr);
                if (strcasecmp ("AUTO_IRRIGATION", optstr) == 0)
                {
                    strcpy (optstr, "\0");
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %s", CropManagement->autoIrrigation[i].cropName);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %d", &CropManagement->autoIrrigation[i].startDay);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %d", &CropManagement->autoIrrigation[i].stopDay);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %lf", &CropManagement->autoIrrigation[i].waterDepletion);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %d", &CropManagement->autoIrrigation[i].lastSoilLayer);
                    i++;
                }
            }
            fgets (cmdstr, MAXSTRING, operation_file);
        }
    }

    /* Link plating order and auto irrigation */
    for (i = 0; i < CropManagement->totalCropsPerRotation; i++)
    {
        if (CropManagement->usingAutoIrr && CropManagement->plantingOrder[i].usesAutoIrrigation == 1)
        {
            for (j = 0; j < auto_irrigation_counter; j++)
            {
                if (strcmp (CropManagement->plantingOrder[i].cropName, CropManagement->autoIrrigation[j].cropName) == 0)
                {
                    CropManagement->plantingOrder[i].usesAutoIrrigation = j;
                    break;
                }
            }
            if (j >= auto_irrigation_counter)
            {
                printf ("ERROR: Cannot find the description of auto irrigation for %s!\n", CropManagement->plantingOrder[i].cropName);
                exit (1);
            }
        }
        else
            CropManagement->plantingOrder[i].usesAutoIrrigation = -1;
    }

    fclose (operation_file);
}
