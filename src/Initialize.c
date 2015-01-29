#include "include/Cycles.h"

void InitializeOutput (char *project)
{
    char filename[50];
    char *output_dir;
    FILE    *output_file;

    output_dir = (char *) malloc ((strlen(project) + 8) * sizeof (char));

    mkdir ("output", 0755);
    sprintf (output_dir, "output/%s", project);
    mkdir (output_dir, 0755);

    output_file = fopen (filename, "w");
    fprintf (output_file, "%-10s\t%-7s\t%-7s\t%-7s\n", "DATE", "T_MEAN", "ET_REF", "PRECIP");
    fprintf (output_file, "%-10s\t%-7s\t%-7s\t%-7s\n", "YYYY-MM-DD", "C", "MM/DAY", "MM");
    fclose (output_file);

    sprintf (filename, "output/%s/crop.dat", project);
    output_file = fopen (filename, "w");
    fprintf (output_file, "%-10s\t%-15s\t%-23s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\n", " DATE", " CROP", " STAGE", "THERMAL", "CUMULAT", "  ABG", " ROOT", " FRACT", " TOTAL", "  ABG", " ROOT", " ABG N", "   N", "N ADDED", "   N", " WATER", "POTENT");
    fprintf (output_file, "%-10s\t%-15s\t%-23s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\n", "", "", "", " TIME", "BIOMASS", "BIOMASS", "BIOMASS", "INTERCP", "   N", "   N", "   N", "CONCENT", "  FIX", "", "STRESS", "STRESS", "TRANSP");
    fprintf (output_file, "%-10s\t%-15s\t%-23s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\n", "YYYY-MM-DD", "-", "-", "C-DAY", "Mg/ha", "Mg/ha", "Mg/ha", "-", "kg/ha", "kg/ha", "kg/ha", "g/kg", "kg/ha", "kg/ha", "%", "%", "mm/day");
    fclose (output_file);

    sprintf (filename, "output/%s/season.dat", project);
    output_file = fopen (filename, "w");
    fprintf (output_file, "%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\n", "YEAR-DOY", "CROP", "TOTAL", "ROOT", "GRAIN", "FORAGE", "AVG", "HARVEST", "POTENT", "ACTUAL", "SOIL", "TOTAL N", "ROOT N", "GRAIN N", "FORAGE N", "CUMULAT N");
    fprintf (output_file, "%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\n", "", "", "BIOMASS", "BIOMASS", "YIELD", "YIELD", "RESIDUE", "INDEX", "TRANSP", "TRANSP", "EVAP", "", "", "", "", "STRESS");
    fprintf (output_file, "%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\t%-8.8s\n", "YEAR-DOY", "N/A", "Mg/ha", "Mg/ha", "Mg/ha", "Mg/ha", "Mg/ha", "Mg/Mg", "mm", "mm", "mm", "Mg/ha", "Mg/ha", "Mg/ha", "Mg/ha", "");
    fclose (output_file);

    sprintf (filename, "output/%s/water.dat", project);
    output_file = fopen (filename, "w");
    fprintf (output_file, "%-10s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\n", "DATE", "IRRIG", "RUNOFF", "INFILT", "DRAIN", "SOIL E", "RES E", "SNOW S", "TRANSP");
    fprintf (output_file, "%-10s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\n", "YYYY-MM-DD", "mm/day", "mm/day", "mm/day", "mm/day", "mm/day", "mm/day", "mm/day", "mm/day");
    fclose (output_file);

    sprintf (filename, "output/%s/N.dat", project);
    output_file = fopen (filename, "w");
    fclose (output_file);

    free (output_dir);
}

void Initialize (SimControlStruct *SimControl, WeatherStruct *Weather, SoilStruct *Soil, ResidueStruct *Residue, SoilCarbonStruct *SoilCarbon, CropStruct *Crop, CropManagementStruct *CropManagement, SnowStruct *Snow)
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

    /* Initialize snow structure */
    Snow->Snow = 0.;
}
