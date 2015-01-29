#include "include/Cycles.h"

void ReadSoil (char *project, SoilStruct *Soil)
{
    FILE           *soil_file;
    char           *filename;
    char            cmdstr[MAXSTRING];
    int             i;

    printf ("Read soil initialization file.\n");

    /* Open soil file */
    filename = (char *)malloc ((strlen (project) + 12) * sizeof (char));
    sprintf (filename, "input/%s.soil", project);
    soil_file = fopen (filename, "r");
    free (filename);

    if (soil_file == NULL)
    {
        printf ("\nERROR: Cannot find the soil file %s!\n", filename);
        exit (1);
    }

    /* Read soil file */
    fgets (cmdstr, MAXSTRING, soil_file);
    sscanf (cmdstr, "%*s %lf", &Soil->Curve_Number);
    fgets (cmdstr, MAXSTRING, soil_file);
    sscanf (cmdstr, "%*s %lf", &Soil->Percent_Slope);
    Soil->Percent_Slope = Soil->Percent_Slope / 100.0;
    fgets (cmdstr, MAXSTRING, soil_file);
    sscanf (cmdstr, "%*s %d", &Soil->totalLayers);

    /* Allocate memories for soil class */
    Soil->layerThickness = (double *)malloc (Soil->totalLayers * sizeof (double));
    Soil->Clay = (double *)malloc (Soil->totalLayers * sizeof (double));
    Soil->Sand = (double *)malloc (Soil->totalLayers * sizeof (double));
    Soil->IOM = (double *)malloc (Soil->totalLayers * sizeof (double));
    Soil->BD = (double *)malloc (Soil->totalLayers * sizeof (double));
    Soil->FC = (double *)malloc (Soil->totalLayers * sizeof (double));
    Soil->PWP = (double *)malloc (Soil->totalLayers * sizeof (double));
    Soil->NO3 = (double *)malloc (Soil->totalLayers * sizeof (double));
    Soil->NH4 = (double *)malloc (Soil->totalLayers * sizeof (double));

    fgets (cmdstr, MAXSTRING, soil_file);   /* skip header line */
    for (i = 0; i < Soil->totalLayers; i++)
    {
        fgets (cmdstr, MAXSTRING, soil_file);
        sscanf (cmdstr, "%*d %lg %lf %lf %lf %lf %lf %lf %lf %lf", &Soil->layerThickness[i], &Soil->Clay[i], &Soil->Sand[i], &Soil->IOM[i], &Soil->BD[i], &Soil->FC[i], &Soil->PWP[i], &Soil->NO3[i], &Soil->NH4[i]);
    }

    fclose (soil_file);
}
