#include "include/Cycles.h"

int ReadOperation (char *project, CyclesStruct Cycles)
{
    FILE           *operation_file;
    char           *filename;
    char            cmdstr[MAXSTRING];
    char            optstr[MAXSTRING];
    char            tempchar[128];
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

    Cycles->NumOp = 0;
    /* Read field operation file */
    fgets (cmdstr, MAXSTRING, operation_file);

    while (!feof (operation_file))
    {
        if (cmdstr[0] != '#' && cmdstr[0] != '\n' && cmdstr[0] != '\0')
        {
            sscanf (cmdstr, "%s", optstr);
            if (strcasecmp ("YEAR", optstr) == 0)
            {
                Cycles->NumOp = Cycles->NumOp + 1;
            }
        }
        fgets (cmdstr, MAXSTRING, operation_file);
    }

    /* Allocate memories for field operation classes */
    printf ("  Field operation file contains descriptions of %d field operations.\n", Cycles->NumOp);

    Cycles->FieldOperation = (FieldOperationClass *) malloc (Cycles->NumOp * sizeof (FieldOperationClass));

    /* Rewind to the beginning of file */
    rewind (operation_file);

    fgets (cmdstr, MAXSTRING, operation_file);
    while (!feof (operation_file))
    {
        if (cmdstr[0] != '#' && cmdstr[0] != '\n' && cmdstr[0] != '\0')
        {
            sscanf (cmdstr, "%s", optstr);
            if (strcasecmp ("YEAR", optstr) == 0)
            {
                q = &Cycles->FieldOperation[i];
                sscanf (cmdstr, "%*s %d", &q->opYear);
                fgets (cmdstr, MAXSTRING, operation_file);
                sscanf (cmdstr, "%*s %d", &q->opDay);
                fgets (cmdstr, MAXSTRING, operation_file);
                sscanf (cmdstr, "%*s %s", &tempchar);
                
                /* Read remaining parameters according to operation type */
                if (strcasecmp ("PLANTING", tempchar) == 0)
                {
                    q->opType = OP_PLANTING;
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %s", &q->cropName);
                    sscanf (cmdstr, "%*s %d", &q->usingAutoIrr);
                    sscanf (cmdstr, "%*s %d", &q->usingAutoFert);
                }
                else if (strcasecmp ("TILLAGE", tempchar) == 0)
                {
                    q->opType = OP_TILLAGE;
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %s", &q->opToolName);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %lf", &q->opDepth);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %lf", &q->opSDR);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %lf", &q->opMixingEfficiency);
                }
                else if (strcasecmp ("IRRIGATION", tempchar) == 0)
                {
                    q->opType = OP_IRRIGATION;
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %lf", &q->volume);
                }
                else if (strcasecmp ("FERTILIZATION", tempchar) == 0)
                {
                    q->opType = OP_FERTILIZATION;
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %s", &q->opsource);
                    fgets (cmdstr, MAXSTRING, operation_file);
                    sscanf (cmdstr, "%*s %lf",&q->opMass);
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
                }
            }
            i++;
        }
        fgets (cmdstr, MAXSTRING, operation_file);
    }


    fclose (operation_file);


    return 0;
}
