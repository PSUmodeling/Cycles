#include "include/Cycles.h"

int ReadSoil (char *project, CyclesStruct Cycles)
{
    FILE           *soil_file;
    char           *filename;
    char            cmdstr[MAXSTRING];
    int             iLayer;
    SoilClass      *Soil;

    Soil = &Cycles->Soil;

    printf ("Read soil initialization file.\n");

    /* Open soil file */
    filename = (char *)malloc ((strlen (project) + 12) * sizeof (char));
    sprintf (filename, "input/%s.soil", project);
    soil_file = fopen (filename, "r");
    free (filename);

    if (soil_file == NULL)
    {
        printf ("\nError: Cannot find the soil file %s!\n", filename);
        exit (1);
    }

    /* Read simulation control file */
    fgets (cmdstr, MAXSTRING, soil_file);
    sscanf (cmdstr, "%*s %d", &Soil->Curve_Number);
    fgets (cmdstr, MAXSTRING, soil_file);
    sscanf (cmdstr, "%*s %lf", &Soil->Percent_Slope);
    Soil->Percent_Slope = Soil->Percent_Slope / 100.;
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
    for (iLayer = 0; iLayer < Soil->totalLayers; iLayer++)
    {
        fgets (cmdstr, MAXSTRING, soil_file);
        sscanf (cmdstr, "%*d %lf %lf %lf %lf %lf %lf %lf %lf %lf", &Soil->layerThickness[iLayer], &Soil->Clay[iLayer], &Soil->Sand[iLayer], &Soil->IOM[iLayer], &Soil->BD[iLayer], &Soil->FC[iLayer], &Soil->PWP[iLayer], &Soil->NO3[iLayer], &Soil->NH4[iLayer]);
    }

    fclose (soil_file);

    return 0;
}
