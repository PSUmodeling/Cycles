#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#include "include/Cycles.h"

int ReadOperation (char *project, CyclesStruct Cycles)
{
    FILE           *operation_file;
    char           *filename;
    char            cmdstr[MAXSTRING];
    char            optstr[MAXSTRING];
    char            tempchar[128];
    FieldOperationClass *p, *q;

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

    /* Read field operation file */
    /* Skip header line */
    fgets (cmdstr, MAXSTRING, operation_file);

    /* Read first field operation */
    if (!feof (operation_file))
    {
        fgets (cmdstr, MAXSTRING, operation_file);
        q = (FieldOperationClass *) malloc (sizeof (FieldOperationClass));
        Cycles->FieldOperation.FirstOperation = q;
        sscanf (cmdstr, "%d %d %s", &q->opYear, &q->opDay, &tempchar);

        /* Read remaining parameters according to operation type */
        if (strcasecmp ("PLANTING", tempchar) == 0)
        {
            q->opType = OP_PLANTING;
            sscanf (cmdstr, "%*d %*d %*s %s", &q->cropName);
        }
        else if (strcasecmp ("TILLAGE", tempchar) == 0)
        {
            q->opType = OP_TILLAGE;
            sscanf (cmdstr, "%*d %*d %*s %s %lf %lf %lf", &q->opToolName, &q->opDepth, &q->opSDR, &q->opMixingEfficiency);
        }
        else if (strcasecmp ("IRRIGATION", tempchar) == 0)
        {
            q->opType = OP_IRRIGATION;
            sscanf (cmdstr, "%*d %*d %*s %lf", &q->volume);
        }
        else if (strcasecmp ("FERTILIZATION", tempchar) == 0)
        {
            q->opType = OP_FERTILIZATION;
            sscanf (cmdstr, "%*d %*d %*s %s %lf %s %s %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", &q->opsource, &q->opMass, &q->opForm, &q->opMethod, &q->opLayer, &q->opC_Organic, &q->opC_Charcoal, &q->opN_Organic, &q->opN_Charcoal, &q->opN_NH4, &q->opN_NO3, &q->opP_Organic, &q->opP_Charcoal, &q->opP_Inorganic, &q->opK, &q->opS);
        }
        q->NextOperation = q;
        Cycles->FieldOperation.n = 1;

    }

    /* Read remaining operations */
    fgets (cmdstr, MAXSTRING, operation_file);
    while (!feof (operation_file))
    {
        p = (FieldOperationClass *) malloc (sizeof (FieldOperationClass));
        sscanf (cmdstr, "%d %d %s", &p->opYear, &p->opDay, &tempchar);

        /* Read remaining parameters according to operation type */
        if (strcasecmp ("PLANTING", tempchar) == 0)
        {
            p->opType = OP_PLANTING;
            sscanf (cmdstr, "%*d %*d %*s %s", &p->cropName);
        }
        else if (strcasecmp ("TILLAGE", tempchar) == 0)
        {
            p->opType = OP_TILLAGE;
            sscanf (cmdstr, "%*d %*d %*s %s %lf %lf %lf", &p->opToolName, &p->opDepth, &p->opSDR, &p->opMixingEfficiency);
        }
        else if (strcasecmp ("IRRIGATION", tempchar) == 0)
        {
            p->opType = OP_IRRIGATION;
            sscanf (cmdstr, "%*d %*d %*s %lf", &p->volume);
        }
        else if (strcasecmp ("FERTILIZATION", tempchar) == 0)
        {
            p->opType = OP_FERTILIZATION;
            sscanf (cmdstr, "%*d %*d %*s %s %lf %s %s %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", &p->opsource, &p->opMass, &p->opForm, &p->opMethod, &p->opLayer, &p->opC_Organic, &p->opC_Charcoal, &p->opN_Organic, &p->opN_Charcoal, &p->opN_NH4, &p->opN_NO3, &p->opP_Organic, &p->opP_Charcoal, &p->opP_Inorganic, &p->opK, &p->opS);
        }
        else
        {
            printf ("Error: Operation tempchar cannot be recoginzed!\n");
            exit (1);
        }
        //        printf("%s opType %d %d\n", tempchar, p->opType, OP_TILLAGE);
        fgets (cmdstr, MAXSTRING, operation_file);
        p->NextOperation = q->NextOperation;
        q->NextOperation = p;
        q = p;
        Cycles->FieldOperation.n = Cycles->FieldOperation.n + 1;
    }

    fclose (operation_file);


    return 0;
}
