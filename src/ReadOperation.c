#include "include/Cycles.h"

int ReadOperation (char *project, CyclesStruct Cycles)
{
    FILE           *operation_file;
    char           *filename;
    char            cmdstr[MAXSTRING];
    char            optstr[MAXSTRING];
    char            tempchar[128];
    int             tillage_counter = 0;
    int             planting_counter = 0;
    int             irrigation_counter = 0;
    int             fertilization_counter = 0;
    int             i;
    FieldOperationClass *q;

    printf ("Read field operation file.\n");

    /* Open simulation control file */
    filename = (char *)malloc ((strlen (project) + 17) * sizeof (char));
    sprintf (filename, "input/%s.operation", project);
    operation_file = fopen (filename, "r");
    free (filename);

    if (operation_file == NULL)
    {
        printf ("\nError: Cannot find the field operation file %s!\n", filename);
        exit (1);
    }

    /* Read field operation file and count numbers of operations */
    fgets (cmdstr, MAXSTRING, operation_file);

    while (!feof (operation_file))
    {
        if (cmdstr[0] != '#' && cmdstr[0] != '\n' && cmdstr[0] != '\0')
        {
            sscanf (cmdstr, "%s", optstr);
            if (strcasecmp ("TILLAGE", optstr) == 0)
                tillage_counter++;
            else if (strcasecmp ("PLANTING", optstr) == 0)
                planting_counter++;
            else if (strcasecmp ("IRRIGATION", optstr) == 0)
                irrigation_counter++;
            else if (strcasecmp ("FERTILIZATION", optstr) == 0)
                fertilization_counter++;
            else if (strcasecmp ("AUTO_IRRIGATION", optstr) == 0)
                Cycles->usingAutoIrr = 1;
        }
        fgets (cmdstr, MAXSTRING, operation_file);
    }

    /* Allocate memories for field operation classes */
#ifdef _DEBUG_
    printf ("  Field operation file contains descriptions of %d planting operations, %d tillage operations, %d fixed irrigation operations, and %d fixed fertilization operations.\n", planting_counter, tillage_counter, irrigation_counter, fertilization_counter);
#endif

    Cycles->NumPlantedCrop = planting_counter;

    Cycles->plantedCrops = (plantingOrderStruct *) malloc (planting_counter * sizeof (plantingOrderStruct));

    /* Rewind to the beginning of file and read all planting operations */
    rewind (operation_file);
    planting_counter = 0;

    fgets (cmdstr, MAXSTRING, operation_file);
    while (!feof (operation_file))
    {
        if (cmdstr[0] != '#' && cmdstr[0] != '\n' && cmdstr[0] != '\0')
        {
            sscanf (cmdstr, "%s", optstr);
            if (strcasecmp ("PLANTING", optstr) == 0)
            {
                fgets (cmdstr, MAXSTRING, operation_file);
                sscanf (cmdstr, "%*s %d", &Cycles->plantedCrops[planting_counter].seedingYear);
                fgets (cmdstr, MAXSTRING, operation_file);
                sscanf (cmdstr, "%*s %d", &Cycles->plantedCrops[planting_counter].seedingDate);
                fgets (cmdstr, MAXSTRING, operation_file);
                sscanf (cmdstr, "%*s %s", &Cycles->plantedCrops[planting_counter].cropName);
                fgets (cmdstr, MAXSTRING, operation_file);
                sscanf (cmdstr, "%*s %d", &Cycles->plantedCrops[planting_counter].usesAutoIrrigation);
                fgets (cmdstr, MAXSTRING, operation_file);
                sscanf (cmdstr, "%*s %d", &Cycles->plantedCrops[planting_counter].usesAutoFertilization);
            }
        }
        fgets (cmdstr, MAXSTRING, operation_file);
    }

    if (tillage_counter)
    {
        /* Rewind to the beginning of file and read all tillage operations */
        rewind (operation_file);

        fgets (cmdstr, MAXSTRING, operation_file);
        while (!feof (operation_file))
        {
            if (cmdstr[0] != '#' && cmdstr[0] != '\n' && cmdstr[0] != '\0')
            {
                sscanf (cmdstr, "%s", optstr);
                if (strcasecmp ("TILLAGE", optstr) == 0)
                {
                    q = (FieldOperationClass *) malloc (sizeof (FieldOperationClass));
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
                    InsertOperation (&Cycles->TillageList, q);
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

        fgets (cmdstr, MAXSTRING, operation_file);
        while (!feof (operation_file))
        {
            if (cmdstr[0] != '#' && cmdstr[0] != '\n' && cmdstr[0] != '\0')
            {
                sscanf (cmdstr, "%s", optstr);
                if (strcasecmp ("IRRIGATION", optstr) == 0)
                {
                    q = (FieldOperationClass *) malloc (sizeof (FieldOperationClass));
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %d", &q->opYear);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %d", &q->opDay);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %lf", &q->opVolume);
                    InsertOperation (&Cycles->FixedIrrigationList, q);
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

        fgets (cmdstr, MAXSTRING, operation_file);
        while (!feof (operation_file))
        {
            if (cmdstr[0] != '#' && cmdstr[0] != '\n' && cmdstr[0] != '\0')
            {
                sscanf (cmdstr, "%s", optstr);
                if (strcasecmp ("FERTILIZATION", optstr) == 0)
                {
                    q = (FieldOperationClass *) malloc (sizeof (FieldOperationClass));
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
                    InsertOperation (&Cycles->FixedFertilizationList, q);
                }
            }
            fgets (cmdstr, MAXSTRING, operation_file);
        }
    }

    fclose (operation_file);

    return 0;
}
