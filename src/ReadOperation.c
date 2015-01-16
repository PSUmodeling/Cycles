#include "include/Cycles.h"

void ReadOperation (char *project, CropManagementStruct *CropManagement, int yearsInRotation)
{
    FILE           *operation_file;
    char           *filename;
    char            cmdstr[MAXSTRING];
    char            optstr[MAXSTRING];
    int             tillage_counter = 0;
    int             planting_counter = 0;
    int             irrigation_counter = 0;
    int             fertilization_counter = 0;
    int             auto_irrigation_counter = 0;
    int             tempyear;
    int             i, j;
    FieldOperationStruct *q;

    printf ("Read field operation file.\n");

    /* Open field operation file */
    filename = (char *)malloc ((strlen (project) + 17) * sizeof (char));
    sprintf (filename, "input/%s.operation", project);
    operation_file = fopen (filename, "r");
    free (filename);

    if (operation_file == NULL)
    {
        printf ("ERROR: Cannot find the field operation file %s!\n", filename);
        exit (1);
    }

    /* Read field operation file and count numbers of operations */
    fgets (cmdstr, MAXSTRING, operation_file);

    while (!feof (operation_file))
    {
        if (cmdstr[0] != '#' && cmdstr[0] != '\n' && cmdstr[0] != '\0')
        {
            sscanf (cmdstr, "%s", optstr);
            if (strcasecmp ("PLANTING", optstr) == 0)
            {
                fgets (cmdstr, MAXSTRING, operation_file);
                sscanf (cmdstr, "%*s %d", &tempyear);
                if (tempyear <= yearsInRotation)
                    planting_counter++;
            }
            else if (strcasecmp ("TILLAGE", optstr) == 0)
                tillage_counter++;
            else if (strcasecmp ("FIXED_IRRIGATION", optstr) == 0)
                irrigation_counter++;
            else if (strcasecmp ("FIXED_FERTILIZATION", optstr) == 0)
                fertilization_counter++;
            else if (strcasecmp ("AUTO_IRRIGATION", optstr) == 0)
                auto_irrigation_counter++;
        }
        fgets (cmdstr, MAXSTRING, operation_file);
    }

    /* Allocate memories for field operation classes */
#ifdef _DEBUG_
    printf ("*Field operation file contains descriptions of %d planting operations, %d tillage operations, %d fixed irrigation operations, %d fixed fertilization operations, %d auto irrigation operations.\n", planting_counter, tillage_counter, irrigation_counter, fertilization_counter, auto_irrigation_counter);
    printf ("(Press any key to continue ...)\n");
    getchar();
#endif

    CropManagement->totalCropsPerRotation = planting_counter;
    CropManagement->plantingOrder = (FieldOperationStruct *) malloc (planting_counter * sizeof (FieldOperationStruct));

    CropManagement->numFertilization = fertilization_counter;
    CropManagement->FixedFertilization = (FieldOperationStruct *) malloc (fertilization_counter * sizeof (FieldOperationStruct));

    CropManagement->numIrrigation = irrigation_counter;
    CropManagement->FixedIrrigation = (FieldOperationStruct *) malloc (irrigation_counter * sizeof (FieldOperationStruct));

    CropManagement->numTillage = tillage_counter;
    CropManagement->Tillage = (FieldOperationStruct *) malloc (tillage_counter * sizeof (FieldOperationStruct));

    CropManagement->autoIrrigation = (autoIrrigationStruct *) malloc (auto_irrigation_counter * sizeof (autoIrrigationStruct));
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
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %d", &tempyear);
                    if (tempyear <= yearsInRotation)
                    {
                        CropManagement->plantingOrder[i].opYear = tempyear;
                        fgets (cmdstr, MAXSTRING, operation_file);
                        sscanf (cmdstr, "%*s %d", &CropManagement->plantingOrder[i].opDay);
                        fgets (cmdstr, MAXSTRING, operation_file);
                        sscanf (cmdstr, "%*s %s", &CropManagement->plantingOrder[i].cropName);
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

                        /* Link planting order and crop description */
                        for (j = 0; j < CropManagement->NumDescribedCrop; j++)  
                        {
                            if (strcmp (CropManagement->plantingOrder[i].cropName, CropManagement->describedCrop[j].userCropName) == 0)
                            {
                                CropManagement->plantingOrder[i].plantID = j;
                                break;
                            }
                        }
                        if (j >= CropManagement->NumDescribedCrop)
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
                    q = &(CropManagement->Tillage[i]);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %d", &q->opYear);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %d", &q->opDay);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %s", &q->opToolName);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %lf", &q->opDepth);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %lf", &q->opSDR);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %lf", &q->opMixingEfficiency);
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
                    q = &(CropManagement->FixedIrrigation[i]);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %d", &q->opYear);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %d", &q->opDay);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %lf", &q->opVolume);
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
                    q = &(CropManagement->FixedFertilization[i]);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %d", &q->opYear);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %d", &q->opDay);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %s", &q->opSource);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %lf", &q->opMass);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %s", &q->opForm);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %s", &q->opMethod);
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
                    if (q->opC_Organic + q->opC_Charcoal + q->opN_Organic + q->opN_Charcoal + q->opN_NH4 + q->opN_NO3 + q->opP_Organic + q->opP_Charcoal + q->opP_Inorganic + q->opK + q->opS <= 1.)
                    {
                        q->opMass = q->opMass / 1000.;
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
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %s", &CropManagement->autoIrrigation[i].cropName);
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
