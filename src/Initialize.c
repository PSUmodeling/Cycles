#include "include/Cycles.h"

void InitializeOutput (char *project)
{
    char filename[50];
    char *output_dir;
    FILE    *output_file;

    output_dir = (char *) malloc ((strlen(project) + 8) * sizeof (char));

    sprintf (output_dir, "output/%s", project);
    mkdir (output_dir, 0755);

    sprintf (filename, "output/%s/weather.dat", project);
    output_file = fopen (filename, "w");
    fprintf (output_file, "%-8s\t%-8s\t%-8s\t%-8s\n", "YEAR-DOY", "T_MEAN", "ET_REF", "PRECIP");
    fprintf (output_file, "%-8s\t%-8s\t%-8s\t%-8s\n", "YEAR-DOY", "C", "MM/DAY", "MM");
    fclose (output_file);

    sprintf (filename, "output/%s/crop.dat", project);
    output_file = fopen (filename, "w");
    fprintf (output_file, "%-8s\t%-8s\t%-8s\t%-8s\n", "YEAR-DOY", "CROP", "STAGE", "PRECIP");
    fprintf (output_file, "%-8s\t%-8s\t%-8s\t%-8s\n", "YEAR-DOY", "N/A", "N/A", "MM");
    fclose (output_file);

    free (output_dir);
}

void Initialize (SimControlStruct *SimControl, WeatherStruct *Weather, SoilStruct *Soil, ResidueStruct *Residue, SoilCarbonStruct *SoilCarbon, CropStruct *Crop, CropManagementStruct *CropManagement)
{
    int i, j;
    /* Initialize weather variables */
    CalculateDerivedWeather (Weather, SimControl->totalYears);

    /* Initialize soil variables */
    InitializeSoil (Soil, Weather, SimControl);

    /* Initialize residue */
    InitializeResidue (Residue, SimControl->totalYears, Soil->totalLayers);

    /* Initialize soil carbon */
    InitializeSoilCarbon (SoilCarbon, Soil->totalLayers);

    ///* Initialize crops */
    Crop->cropUniqueIdentifier = -1;

    /* Initialize tillage factors */
    CropManagement->tillageFactor = (double *) malloc (Soil->totalLayers * sizeof (double));
}
