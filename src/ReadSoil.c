#include "Cycles.h"

void ReadSoil (char *filename, SoilStruct *Soil)
{
    /*
     * Read soil description file
     * -----------------------------------------------------------------------
     * LOCAL VARIABLES
     *
     * Variable             Type        Description
     * ==========           ==========  ====================
     * soil_file	    FILE*	File pointer of soil description file
     * fullname		    char*	Full file name of the soil description
     *					  file
     * cmdstr		    char[MAXSTRING]
     *					Command string
     * i		    int		Loop counter
     */
    FILE           *soil_file;
    char           *fullname;
    char            cmdstr[MAXSTRING];
    int             i;

    /* Open soil file */
    fullname = (char *)malloc ((strlen (filename) + 7) * sizeof (char));
    sprintf (fullname, "input/%s", filename);
    soil_file = fopen (fullname, "r");

    if (soil_file == NULL)
    {
        printf ("\nERROR: Cannot find the soil file %s!\n", filename);
        exit (1);
    }
    else
        printf ("%-30s input/%s.\n", "Read soil initialization file:", filename);

    free (fullname);

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

        if (Soil->Clay[i] > 100.0 || Soil->Clay[i] < 0.0)
        {
            printf ("ERROR: Clay out of range (0--100) in %s!\n", filename);
            exit (1);
        }
        if (Soil->Sand[i] > 100.0 || Soil->Sand[i] < 0.0)
        {
            printf ("ERROR: Sand out of range (0--100) in %s!\n", filename);
            exit (1);
        }
        if (Soil->IOM[i] > 100.0 || Soil->IOM[i] < 0.0)
        {
            printf ("ERROR: IOM out of range (0--100) in %s!\n", filename);
            exit (1);
        }
        if (Soil->FC[i] > 1.0 || (Soil->FC[i] < 0.0 && (int)Soil->FC[i] != -999))
        {
            printf ("ERROR: Field capacity out of range (0--1.0) in %s!\n", filename);
            exit (1);
        }
        if (Soil->PWP[i] > 1.0 || (Soil->PWP[i] < 0.0 && (int)Soil->PWP[i] != -999))
        {
            printf ("ERROR: Permanent wilting point out of range (0--1.0) in %s!\n", filename);
            exit (1);
        }
    }

    fclose (soil_file);
}
