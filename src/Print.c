#include "Cycles.h"

void _Cycles_printf (const char *fn, int lineno, const char *func,
    int verbosity, const char *fmt, ...)
{
    va_list         va;

    va_start (va, fmt);

    if (VL_ERROR == verbosity)
    {
        vfprintf (stderr, fmt, va);
        if (debug_mode)
        {
            fprintf (stderr, "Printed from %s", func);
            fprintf (stderr, " (%s, Line %d.\n)", fn, lineno);
        }
        fflush (stderr);
    }
    else if (verbosity <= verbose_mode)
    {
        vfprintf (stdout, fmt, va);
        if (debug_mode)
        {
            printf ("Printed from %s", func);
            printf (" (%s, Line %d.\n)", fn, lineno);
        }
        fflush (stderr);
    }

    va_end (va);
}

void InitializeOutput (const comm_struct *Community, int layers, const ctrl_struct *SimControl, print_struct *filenames)
{
    char            filename[256];
    char           *output_dir;
    FILE           **temp_output_file=NULL, *output_file;
    int             i;

    output_dir = (char *)malloc ((strlen (project) + 8) * sizeof (char));

    mkdir ("output");
    sprintf (output_dir, "output/%s", project);
    mkdir (output_dir);

    /* Initialize season output files */
    sprintf (filename, "output/%s/season.dat", project);
    output_file = fopen (filename, "w");
    fprintf (output_file, "%-10s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\n",
        "DATE", "CROP", "TOTAL BIOMASS", "ROOT BIOMASS", "GRAIN YIELD", "FORAGE YIELD", "AG RESIDUE", "HARVEST INDEX", "POTENTIAL TR", "ACTUAL TR", "SOIL EVAP", "TOTAL N", "ROOT N", "GRAIN N", "FORAGE N", "CUM. N STRESS", "N IN HARVEST", "N IN RESIDUE", "N CONCN FORAGE");
    fprintf (output_file, "%-10s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\n", "YYYY-MM-DD", "-", "Mg/ha", "Mg/ha", "Mg/ha", "Mg/ha", "Mg/ha", "Mg/Mg", "mm", "mm", "mm", "Mg/ha", "Mg/ha", "Mg/ha", "Mg/ha", "-", "kg/ha", "kg/ha", "%");
    fflush (output_file);
    fclose (output_file);

    /* Initialize daily output files */

if (SimControl->weatherDailyOutput)
{
    sprintf (filename, "output/%s/weather.dat", project);
    filenames->weather_out = fopen (filename, "w");
    temp_output_file = &filenames->weather_out;
    fprintf (*temp_output_file, "%-10s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\n", "DATE", "MIN TMP", "MAX TMP", "AVG TMP", "REFERENCE ET", "PRECIPITATION");
    fprintf (*temp_output_file, "%-10s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\n", "YYYY-MM-DD", "C", "C", "C", "mm/day", "mm");
    fflush (*temp_output_file);
    //fclose (output_file);
}

if (SimControl->cropDailyOutput)
{
    filenames->crop_out = (FILE **)malloc (Community->NumCrop * sizeof (FILE));
    for (i = 0; i < Community->NumCrop; i++)
    {
        if (Community->Crop[i].cropUsed)
        {
        sprintf (filename, "output/%s/%s.dat", project, Community->Crop[i].cropName);
        filenames->crop_out[i] = fopen (filename, "w");
        temp_output_file = &filenames->crop_out[i];

        fprintf (*temp_output_file, "%-10s\t%-15s\t%-23s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\n", " DATE", "CROP", "STAGE", "THERMAL TIME", "CUM. BIOMASS", "AG BIOMASS", "ROOT BIOMASS", "FRAC INTERCEP", "TOTAL N", "AG N", "ROOT N", "AG N CONCN", "N FIXATION", "N ADDED", "N STRESS", " WATER STRESS", "POTENTIAL TR");
        fprintf (*temp_output_file, "%-10s\t%-15s\t%-23s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\n", "YYYY-MM-DD", "-", "-", "C-day", "Mg/ha", "Mg/ha", "Mg/ha", "-", "kg/ha", "kg/ha", "kg/ha", "g/kg", "kg/ha", "kg/ha", "%", "%", "mm/day");
        fflush (*temp_output_file);
        //fclose (*temp_output_file);
        }
    }
}

if (SimControl->waterDailyOutput)
    {
    sprintf (filename, "output/%s/water.dat", project);
    filenames->water_out = fopen (filename, "w");
    temp_output_file = &filenames->water_out;
    fprintf (*temp_output_file, "%-10s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\n", "DATE", "IRRIGATION", "RUNOFF", "INFILTRATION", "DRAINAGE", "SOIL EVAP", "RES EVAP", "SNOW SUB", "TRANSPIRATION");
    fprintf (*temp_output_file, "%-10s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\n", "YYYY-MM-DD", "mm/day", "mm/day", "mm/day", "mm/day", "mm/day", "mm/day", "mm/day", "mm/day");
    fflush (*temp_output_file);
    //fclose (*temp_output_file);
    }

if (SimControl->nitrogenDailyOutput)
    {
    sprintf (filename, "output/%s/N.dat", project);
    filenames->nitrogen_out = fopen (filename, "w");
    temp_output_file = &filenames->nitrogen_out;
    fprintf (*temp_output_file, "%-10s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\n", "DATE", "ORG SOIL N", "PROF SOIL NO3", "PROF SOIL NH4", "MINERALIZATION", "IMMOBILIZATION", "NET MINERALIZ", "NH4 NITRIFICAT", "N2O FROM NITRIF", "NH4 VOLATILIZ", "NO3 DENITRIF", "N2O FROM DENIT", "NO3 LEACHING", "NH4 LEACHING");
    fprintf (*temp_output_file, "%-10s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\n", "YYYY-MM-DD", "kg N/ha", "kg N/ha", "kg N/ha", "kg N/ha", "kg N/ha", "kg N/ha", "kg N/ha", "kg N/ha", "kg N/ha", "kg N/ha", "kg N/ha", "kg N/ha", "kg N/ha");
    fflush (*temp_output_file);
    //fclose (*temp_output_file);
    }

if (SimControl->residueDailyOutput)
    {
    sprintf (filename, "output/%s/residue.dat", project);
    filenames->residue_out = fopen (filename, "w");
    temp_output_file = &filenames->residue_out;
    fprintf (*temp_output_file, "%-10s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\n", "DATE", "FRAC INTERCEP", "AG RES BIOMASS", "BG RES BIOMASS", "ROOT RES BIOMAS", "SFC MANURE C", "AG RES N", "BG RES N", "ROOT RES N", "SFC MANURE N", "STD RES MOIST", "FLAT RES MOIST");
    fprintf (*temp_output_file, "%-10s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\n", "YYYY-MM-DD", "-", "Mg/ha", "Mg/ha", "Mg/ha", "Mg/ha", "kg/ha", "kg/ha", "kg/ha", "Mg/ha", "kg/kg", "kg/kg");
    fflush (*temp_output_file);
    //fclose (*temp_output_file);
    }

if (SimControl->soilCarbonDailyOutput)
    {
    sprintf (filename, "output/%s/soilC.dat", project);
    filenames->carbon_out = fopen (filename, "w");
    temp_output_file = &filenames->carbon_out;
    fprintf (*temp_output_file, "%-10s\t%-15s\t%-15s\t%-15s\t%-15s\n", "DATE", "SOIL ORG C", "HUMIFIED C", "RES RESPIRED C", "SOM RESPIRED C");
    fprintf (*temp_output_file, "%-10s\t%-15s\t%-15s\t%-15s\t%-15s\n", "YYYY-MM-DD", "Mg/ha", "Mg/ha", "Mg/ha", "Mg/ha");
    fflush (*temp_output_file);
    //fclose (*temp_output_file);
    }


    /* Initialize soil layers file */
if (SimControl->soilDailyOutput)
{
    sprintf (filename, "output/%s/soilLayersCN.dat", project);
    filenames->layers_out = fopen (filename, "w");
    temp_output_file = &filenames->layers_out;
    fprintf (*temp_output_file, "%-10s", "DATE");
    for (i = 0; i < layers; i++)
        fprintf (*temp_output_file, "\t%-15s", "NO3");
        fprintf (*temp_output_file, "\t");
    for (i = 0; i < layers; i++)
        fprintf (*temp_output_file, "\t%-15s", "NH4");
        fprintf (*temp_output_file, "\t");
    for (i = 0; i < layers; i++)
        fprintf (*temp_output_file, "\t%-15s", "FACTOR COMP.");
        fprintf (*temp_output_file, "\t");
    fprintf (*temp_output_file, "\t%-15s", "STAND. RESID. C");
    fprintf (*temp_output_file, "\t%-15s", "FLAT RESID. C");
    fprintf (*temp_output_file, "\t%-15s", "MANURE RESID. C");
    for (i = 0; i < layers; i++)
        fprintf (*temp_output_file, "\t%-15s", "RESIDUE C");
        fprintf (*temp_output_file, "\t");
    fprintf (*temp_output_file, "\t%-15s", "STAND. RESID. C:N");
    fprintf (*temp_output_file, "\t%-15s", "FLAT RESID. C:N");
    fprintf (*temp_output_file, "\t%-15s", "MANURE RESID. C:N");
    for (i = 0; i < layers; i++)
        fprintf (*temp_output_file, "\t%-15s", "RESIDUE C:N");
        fprintf (*temp_output_file, "\t");
    for (i = 0; i < layers; i++)
        fprintf (*temp_output_file, "\t%-15s", "MIC C");
        fprintf (*temp_output_file, "\t");
    for (i = 0; i < layers; i++)
        fprintf (*temp_output_file, "\t%-15s", "MIC C:N");
        fprintf (*temp_output_file, "\t");
    for (i = 0; i < layers; i++)
        fprintf (*temp_output_file, "\t%-15s", "SOIL ORG C");
        fprintf (*temp_output_file, "\t");
    for (i = 0; i < layers; i++)
        fprintf (*temp_output_file, "\t%-15s", "SOIL ORG C:N");
        fprintf (*temp_output_file, "\t");
    for (i = 0; i < layers; i++)
        fprintf (*temp_output_file, "\t%-15s", "WATER CONTENT");
        fprintf (*temp_output_file, "\t");
    for (i = 0; i < layers; i++)
        fprintf (*temp_output_file, "\t%-15s", "THICKNESS");
        fprintf (*temp_output_file, "\t");
    for (i = 0; i < layers; i++)
        fprintf (*temp_output_file, "\t%-15s", "BULK DENSITY");
    fprintf (*temp_output_file, "\n");

    fprintf (*temp_output_file, "%-10s", "-");
    for (i = 0; i < layers; i++)
        fprintf (*temp_output_file, "\tLAYER %-9d", i + 1);
        fprintf (*temp_output_file, "\t");
    for (i = 0; i < layers; i++)
        fprintf (*temp_output_file, "\tLAYER %-9d", i + 1);
        fprintf (*temp_output_file, "\t");
    for (i = 0; i < layers; i++)
        fprintf (*temp_output_file, "\tLAYER %-9d", i + 1);
        fprintf (*temp_output_file, "\t");
    fprintf (*temp_output_file, "\t%-15s", "SURFACE");
    fprintf (*temp_output_file, "\t%-15s", "SURFACE");
    fprintf (*temp_output_file, "\t%-15s", "SURFACE");
    for (i = 0; i < layers; i++)
        fprintf (*temp_output_file, "\tLAYER %-9d", i + 1);
        fprintf (*temp_output_file, "\t");
    fprintf (*temp_output_file, "\t%-15s", "SURFACE");
    fprintf (*temp_output_file, "\t%-15s", "SURFACE");
    fprintf (*temp_output_file, "\t%-15s", "SURFACE");
    for (i = 0; i < layers; i++)
        fprintf (*temp_output_file, "\tLAYER %-9d", i + 1);
        fprintf (*temp_output_file, "\t");
    for (i = 0; i < layers; i++)
        fprintf (*temp_output_file, "\tLAYER %-9d", i + 1);
        fprintf (*temp_output_file, "\t");
    for (i = 0; i < layers; i++)
        fprintf (*temp_output_file, "\tLAYER %-9d", i + 1);
        fprintf (*temp_output_file, "\t");
    for (i = 0; i < layers; i++)
        fprintf (*temp_output_file, "\tLAYER %-9d", i + 1);
        fprintf (*temp_output_file, "\t");
    for (i = 0; i < layers; i++)
        fprintf (*temp_output_file, "\tLAYER %-9d", i + 1);
        fprintf (*temp_output_file, "\t");
    for (i = 0; i < layers; i++)
        fprintf (*temp_output_file, "\tLAYER %-9d", i + 1);
        fprintf (*temp_output_file, "\t");
    for (i = 0; i < layers; i++)
        fprintf (*temp_output_file, "\tLAYER %-9d", i + 1);
        fprintf (*temp_output_file, "\t");
    for (i = 0; i < layers; i++)
        fprintf (*temp_output_file, "\tLAYER %-9d", i + 1);
    fprintf (*temp_output_file, "\n");

    fprintf (*temp_output_file, "%-10s", "YYYY-MM-DD");
    for (i = 0; i < layers; i++)
        fprintf (*temp_output_file, "\t%-15s", "mg/kg");
        fprintf (*temp_output_file, "\t");
    for (i = 0; i < layers; i++)
        fprintf (*temp_output_file, "\t%-15s", "mg/kg");
        fprintf (*temp_output_file, "\t");
    for (i = 0; i < layers; i++)
        fprintf (*temp_output_file, "\t%-15s", "-/-");
        fprintf (*temp_output_file, "\t");
    fprintf (*temp_output_file, "\t%-15s", "Mg/ha");
    fprintf (*temp_output_file, "\t%-15s", "Mg/ha");
    fprintf (*temp_output_file, "\t%-15s", "Mg/ha");
    for (i = 0; i < layers; i++)
        fprintf (*temp_output_file, "\t%-15s", "Mg/ha");
        fprintf (*temp_output_file, "\t");
    fprintf (*temp_output_file, "\t%-15s", "g:g");
    fprintf (*temp_output_file, "\t%-15s", "g:g");
    fprintf (*temp_output_file, "\t%-15s", "g:g");
    for (i = 0; i < layers; i++)
        fprintf (*temp_output_file, "\t%-15s", "g:g");
        fprintf (*temp_output_file, "\t");
    for (i = 0; i < layers; i++)
        fprintf (*temp_output_file, "\t%-15s", "Mg/ha");
        fprintf (*temp_output_file, "\t");
    for (i = 0; i < layers; i++)
        fprintf (*temp_output_file, "\t%-15s", "g:g");
        fprintf (*temp_output_file, "\t");
    for (i = 0; i < layers; i++)
        fprintf (*temp_output_file, "\t%-15s", "Mg/ha");
        fprintf (*temp_output_file, "\t");
    for (i = 0; i < layers; i++)
        fprintf (*temp_output_file, "\t%-15s", "g:g");
        fprintf (*temp_output_file, "\t");
    for (i = 0; i < layers; i++)
        fprintf (*temp_output_file, "\t%-15s", "m3/m3");
        fprintf (*temp_output_file, "\t");
    for (i = 0; i < layers; i++)
        fprintf (*temp_output_file, "\t%-15s", "m");
        fprintf (*temp_output_file, "\t");
    for (i = 0; i < layers; i++)
        fprintf (*temp_output_file, "\t%-15s", "Mg/m3");
    fprintf (*temp_output_file, "\n");

    fflush (*temp_output_file);
    //fclose (*temp_output_file);
}


    sprintf (filename, "output/%s/summary.dat", project);
    output_file = fopen (filename, "w");
    fclose (output_file);

    /* Initialize annual output files */

//    sprintf (filename, "output/%s/annualSoilC.dat", project);
//    output_file = fopen (filename, "w");
//    fprintf (output_file, "%-7s", "YEAR");
//    for (i = 0; i < layers; i++)
//        fprintf (output_file, "\t%-15s", "THICKNESS");
//    for (i = 0; i < layers; i++)
//        fprintf (output_file, "\t%-15s", "BULK DENSITY");
//    for (i = 0; i < layers; i++)
//        fprintf (output_file, "\t%-15s", "CLAY");
//    for (i = 0; i < layers; i++)
//        fprintf (output_file, "\t%-15s", "COMP DECOMP FAC");
//    for (i = 0; i < layers; i++)
//        fprintf (output_file, "\t%-15s", "SOIL C DECOMP R");
//    for (i = 0; i < layers; i++)
//        fprintf (output_file, "\t%-15s", "INIT SOIL C");
//    for (i = 0; i < layers; i++)
//        fprintf (output_file, "\t%-15s", "SOIL C DECOMP");
//    for (i = 0; i < layers; i++)
//        fprintf (output_file, "\t%-15s", "C INPUT / LYR");
//    for (i = 0; i < layers; i++)
//        fprintf (output_file, "\t%-15s", "HUMIFIED / LYR");
//    for (i = 0; i < layers; i++)
//        fprintf (output_file, "\t%-15s", "HUMIFICAT COEF");
//    for (i = 0; i < layers; i++)
//        fprintf (output_file, "\t%-15s", "EST SAT CONCN");
//    for (i = 0; i < layers; i++)
//        fprintf (output_file, "\t%-15s", "EST SAT CONT");
//    fprintf (output_file, "\n");
//
//    fprintf (output_file, "%-7s", "-");
//    for (i = 0; i < layers; i++)
//        fprintf (output_file, "\tLAYER %-9d", i + 1);
//    for (i = 0; i < layers; i++)
//        fprintf (output_file, "\tLAYER %-9d", i + 1);
//    for (i = 0; i < layers; i++)
//        fprintf (output_file, "\tLAYER %-9d", i + 1);
//    for (i = 0; i < layers; i++)
//        fprintf (output_file, "\tLAYER %-9d", i + 1);
//    for (i = 0; i < layers; i++)
//        fprintf (output_file, "\tLAYER %-9d", i + 1);
//    for (i = 0; i < layers; i++)
//        fprintf (output_file, "\tLAYER %-9d", i + 1);
//    for (i = 0; i < layers; i++)
//        fprintf (output_file, "\tLAYER %-9d", i + 1);
//    for (i = 0; i < layers; i++)
//        fprintf (output_file, "\tLAYER %-9d", i + 1);
//    for (i = 0; i < layers; i++)
//        fprintf (output_file, "\tLAYER %-9d", i + 1);
//    for (i = 0; i < layers; i++)
//        fprintf (output_file, "\tLAYER %-9d", i + 1);
//    for (i = 0; i < layers; i++)
//        fprintf (output_file, "\tLAYER %-9d", i + 1);
//    for (i = 0; i < layers; i++)
//        fprintf (output_file, "\tLAYER %-9d", i + 1);
//    fprintf (output_file, "\n");
//
//    fprintf (output_file, "%-7s", "YYYY");
//    for (i = 0; i < layers; i++)
//        fprintf (output_file, "\t%-15s", "m");
//    for (i = 0; i < layers; i++)
//        fprintf (output_file, "\t%-15s", "Mg/m3");
//    for (i = 0; i < layers; i++)
//        fprintf (output_file, "\t%-15s", "%");
//    for (i = 0; i < layers; i++)
//        fprintf (output_file, "\t%-15s", "-");
//    for (i = 0; i < layers; i++)
//        fprintf (output_file, "\t%-15s", "1/year");
//    for (i = 0; i < layers; i++)
//        fprintf (output_file, "\t%-15s", "Mg/ha");
//    for (i = 0; i < layers; i++)
//        fprintf (output_file, "\t%-15s", "Mg C/year");
//    for (i = 0; i < layers; i++)
//        fprintf (output_file, "\t%-15s", "Mg C/year");
//    for (i = 0; i < layers; i++)
//        fprintf (output_file, "\t%-15s", "Mg C/ year");
//    for (i = 0; i < layers; i++)
//        fprintf (output_file, "\t%-15s", "-");
//    for (i = 0; i < layers; i++)
//        fprintf (output_file, "\t%-15s", "g C /kg soil");
//    for (i = 0; i < layers; i++)
//        fprintf (output_file, "\t%-15s", "Mg C/ha");
//    fprintf (output_file, "\n");
//
//    fflush (output_file);
//    fclose (output_file);
//
//
//    /* Initialize annual output files */
//    sprintf (filename, "output/%s/annualSoilCEvol.dat", project);
//    output_file = fopen (filename, "w");
//    fprintf (output_file, "%-7s\t%-15s", "YEAR", "Profile");
//    fprintf (output_file, "\t%-15s\t%-15s", "ABOVE 30 cm", "BELOW 30 cm");
//    for (i = 0; i < layers; i++)
//        fprintf (output_file, "\tLAYER %-9d", i + 1);
//    fprintf (output_file, "\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\n", "RES BIOMASS", "ROOT BIOMASS", "RES BIOMASS IN", "ROOT BIOMASS IN", "INIT PROF C", "RES C INPUT", "ROOT C INPUT", "HUMIFIED C", "RESPIRED C", "FINAL PROF C", "YEAR C DIFF", "N GROSS MINERAL", "N IMMOBIL", "N NET MINERAL", "NH4 NITRIFICAT", "N2O FROM NITRIF", "NH3 VOLATIL", "NO3 DENITRIF", "N2O FROM DENIT", "NO3 LEACHING", "NH4 LEACHING");
//
//    fprintf (output_file, "%-7s\t%-15s", "YYYY", "Mg C/ha");
//    for (i = 0; i < layers; i++)
//        fprintf (output_file, "\t%-15s", "Mg C/ha");
//    fprintf (output_file, "\t%-15s\t%-15s", "Mg C/ha", "Mg C/ha");
//
//    fprintf (output_file, "\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s", "Mg/year", "Mg/year", "Mg/year", "Mg/year", "Mg C/year", "Mg C/year", "Mg C/year", "Mg C/year", "Mg C/year", "Mg C/year", "Mg C/year", "kg N/year", "kg N/year", "kg N/year", "kg N/year", "kg N/year", "kg N/year", "kg N/year", "kg N/year", "kg N/year", "kg N/year");
//    fprintf (output_file, "\n");
//    fflush (output_file);
//    fclose (output_file);

    sprintf (filename, "output/%s/annualSoilProfileC.dat", project);
    output_file = fopen (filename, "w");
    fprintf (output_file, "%-7s", "YEAR");
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\t%-15s", "RES BIOMASS IN");
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\t%-15s", "ROOT BIOMASS IN");
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\t%-15s", "RES C DECOMP");
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\t%-15s", "ROOT C DECOMP");
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\t%-15s", "INIT C MASS");
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\t%-15s", "HUMIFIED C");
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\t%-15s", "RESPIRED C");
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\t%-15s", "FINAL C");
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\t%-15s", "C DIFF");
    fprintf (output_file, "\n");

    fprintf (output_file, "%-7s", "-");
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\tLAYER %-9d", i + 1);
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\tLAYER %-9d", i + 1);
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\tLAYER %-9d", i + 1);
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\tLAYER %-9d", i + 1);
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\tLAYER %-9d", i + 1);
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\tLAYER %-9d", i + 1);
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\tLAYER %-9d", i + 1);
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\tLAYER %-9d", i + 1);
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\tLAYER %-9d", i + 1);
    fprintf (output_file, "\n");

    fprintf (output_file, "%-7s", "YYYY");
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\t%-15s", "Mg/ha");
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\t%-15s", "Mg/ha");
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\t%-15s", "Mg/ha");
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\t%-15s", "Mg/ha");
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\t%-15s", "Mg C/year");
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\t%-15s", "Mg C/year");
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\t%-15s", "Mg C/year");
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\t%-15s", "Mg C/year");
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\t%-15s", "Mg C/year");
    fprintf (output_file, "\n");

    fflush (output_file);
    fclose (output_file);

    sprintf (filename, "output/%s/annualSOM.dat", project);
    output_file = fopen (filename, "w");
    fprintf (output_file, "%-7s", "YEAR");
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\t%-15s", "THICKNESS");
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\t%-15s", "BULK DENSITY");
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\t%-15s", "SOIL C");
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\t%-15s", "C SAT. RATIO");

    fprintf (output_file, "\n");

    fprintf (output_file, "%-7s", "-");
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\tLAYER %-9d", i + 1);
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\tLAYER %-9d", i + 1);
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\tLAYER %-9d", i + 1);
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\tLAYER %-9d", i + 1);
    fprintf (output_file, "\n");

    fprintf (output_file, "%-7s", "YYYY");
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\t%-15s", "m");
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\t%-15s", "Mg/m3");
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\t%-15s", "%");
    for (i = 0; i < layers; i++)
        fprintf (output_file, "\t%-15s", "Cs/Cx");
    fprintf (output_file, "\n");

    fflush (output_file);
    fclose (output_file);
    free (output_dir);

}

void PrintDailyOutput (int y, int doy, int start_year, const weather_struct *Weather, const comm_struct *Community, const soil_struct *Soil, const snow_struct *Snow, const residue_struct *Residue, const ctrl_struct *SimControl, const soilc_struct *SoilCarbon, print_struct *filenames)
{
    char            filename[256];
    FILE           **output_file=NULL;
    int             month, mday;
    int             i;
    double          sum, sumResidueC[Soil->totalLayers], sumResidueN[Soil->totalLayers];
    int             leap_year = 0;
    crop_struct     *Crop;

    if (Weather->lastDoy[y] == 366)
        leap_year = 1;

    doy2date (y + start_year, doy, &month, &mday, leap_year);

    /*
     * Print weather output
     */
if (SimControl->weatherDailyOutput)
{
    //sprintf (filename, "output/%s/weather.dat", project);
    //output_file = fopen (filename, "a");
    output_file = &filenames->weather_out;
    fprintf (*output_file, "%4.4d-%2.2d-%2.2d\t", y + start_year, month, mday);
    fprintf (*output_file, "%-15.2lf\t", Weather->tMin[y][doy - 1]);
    fprintf (*output_file, "%-15.2lf\t", Weather->tMax[y][doy - 1]);
    fprintf (*output_file, "%-15.2lf\t", 0.5 * Weather->tMin[y][doy - 1] + 0.5 * Weather->tMax[y][doy - 1]);
    fprintf (*output_file, "%-15.3lf\t", Weather->ETref[y][doy - 1]);
    fprintf (*output_file, "%-15.2lf\n", Weather->precipitation[y][doy - 1]);

    fflush (*output_file);

    if (y==(SimControl->totalYears-1) && doy==Weather->lastDoy[y])
        fclose (*output_file);
}
    /*
     * Print crop output
     */

if (SimControl->cropDailyOutput)
{
    for (i = 0; i < Community->NumCrop; i++)
    {
        if (Community->Crop[i].cropUsed)
        {
        Crop = &Community->Crop[i];
        //sprintf (filename, "output/%s/%s.dat", project, Crop->cropName);
        //output_file = fopen (filename, "a");

        output_file = &filenames->crop_out[i];

        fprintf (*output_file, "%4.4d-%2.2d-%2.2d\t", y + start_year, month, mday);
        if (Community->NumActiveCrop <= 0)
        {
            fprintf (*output_file, "%-15s\t", "FALLOW");
            fprintf (*output_file, "%-23s\t", "N/A");
        }
        else if (Crop->stageGrowth == NO_CROP)
        {
            fprintf (*output_file, "%-15s\t", "NO_CROP");
            fprintf (*output_file, "%-23s\t", "N/A");
        }
        else
        {
            fprintf (*output_file, "%-15s\t", Crop->cropName);
            switch (Crop->stageGrowth)
            {
                case NO_CROP:
                    /* Do nothing */
                    break;
                case PRE_EMERGENCE:
                    fprintf (*output_file, "%-23s\t", "PRE_EMERGENCE");
                    break;
                case VEGETATIVE_GROWTH:
                    fprintf (*output_file, "%-23s\t", "VEGETATIVE_GROWTH");
                    break;
                case PERENNIAL:
                    fprintf (*output_file, "%-23s\t", "PERENNIAL");
                    break;
                case REPRODUCTIVE_GROWTH:
                    fprintf (*output_file, "%-23s\t", "REPRODUCTIVE_GROWTH");
                    break;
                case MATURITY:
                    fprintf (*output_file, "%-23s\t", "MATURITY");
                    break;
                case CLIPPING:
                    fprintf (*output_file, "%-23s\t", "CLIPPING");
                    break;
                case PLANTING:
                    fprintf (*output_file, "%-23s\t", "PLANTING");
                    break;
            }
        }


        fprintf (*output_file, "%-15.6lf\t", Crop->svTT_Cumulative);
        fprintf (*output_file, "%-15.6lf\t", Crop->svBiomass);
        fprintf (*output_file, "%-15.6lf\t", Crop->svShoot);
        fprintf (*output_file, "%-15.6lf\t", Crop->svRoot);
        fprintf (*output_file, "%-15.6lf\t", Crop->svRadiationInterception);
        fprintf (*output_file, "%-15.6lf\t", (Crop->svN_Shoot + Crop->svN_Root) * 1000.);
        fprintf (*output_file, "%-15.6lf\t", Crop->svN_Shoot * 1000.);
        fprintf (*output_file, "%-15.6lf\t", Crop->svN_Root * 1000.);
        if (Crop->svShoot > 0.)
            fprintf (*output_file, "%-15.6lf\t", (Crop->svN_Shoot / Crop->svShoot) * 1000.);
        else
            fprintf (*output_file, "%-15.6lf\t", 0.);
        fprintf (*output_file, "%-15.6lf\t", Crop->svN_Fixation * 1000.);
        fprintf (*output_file, "%-15.6lf\t", Crop->svN_AutoAdded * 1000.);
        fprintf (*output_file, "%-15.6lf\t", Crop->svN_StressFactor);
        fprintf (*output_file, "%-15.6lf\t", Crop->svWaterStressFactor);
        fprintf (*output_file, "%-15.6lf\n", Crop->svTranspirationPotential);

        fflush (*output_file);
        if (y==(SimControl->totalYears-1) && doy==Weather->lastDoy[y])
            fclose (*output_file);
     }
   //if (y==(SimControl->totalYears-1) && doy==Weather->lastDoy[y])
   //     free (filenames->crop_out[i]);

}
if (y==(SimControl->totalYears-1) && doy==Weather->lastDoy[y])
        free (filenames->crop_out);
}
    /*
     * Print Water output
     */
if (SimControl->waterDailyOutput)
{
    sprintf (filename, "output/%s/water.dat", project);
    output_file = &filenames->water_out;

    fprintf (*output_file, "%4.4d-%2.2d-%2.2d\t", y + start_year, month, mday);
    fprintf (*output_file, "%-15.6lf\t", Soil->irrigationVol);
    fprintf (*output_file, "%-15.6lf\t", Soil->runoffVol);
    fprintf (*output_file, "%-15.6lf\t", Soil->infiltrationVol);
    fprintf (*output_file, "%-15.6lf\t", Soil->drainageVol);
    fprintf (*output_file, "%-15.6lf\t", Soil->evaporationVol);
    fprintf (*output_file, "%-15.6lf\t", Soil->residueEvaporationVol);
    fprintf (*output_file, "%-15.6lf\t", Snow->snowEvaporationVol);
    fprintf (*output_file, "%-15.6lf\n", Community->svTranspiration);

    fflush (*output_file);
    if (y==(SimControl->totalYears-1) && doy==Weather->lastDoy[y])
        fclose (*output_file);
}
    /*
     * Print Nitrogen Output
     */
if (SimControl->nitrogenDailyOutput)
{
    sprintf (filename, "output/%s/N.dat", project);
    output_file = &filenames->nitrogen_out;

    fprintf (*output_file, "%4.4d-%2.2d-%2.2d\t", y + start_year, month, mday);
    fprintf (*output_file, "%-15.6lf\t", Soil->SONProfile * 1000.);
    fprintf (*output_file, "%-15.6lf\t", Soil->NO3Profile * 1000.);
    fprintf (*output_file, "%-15.6lf\t", Soil->NH4Profile * 1000.);
    fprintf (*output_file, "%-15.6lf\t", Soil->N_Mineralization * 1000.);
    fprintf (*output_file, "%-15.6lf\t", Soil->N_Immobilization * 1000.);
    fprintf (*output_file, "%-15.6lf\t", Soil->N_NetMineralization * 1000.);
    fprintf (*output_file, "%-15.6lf\t", Soil->NH4_Nitrification * 1000.);
    fprintf (*output_file, "%-15.6lf\t", Soil->N2O_Nitrification * 1000.);
    fprintf (*output_file, "%-15.6lf\t", Soil->NH4_Volatilization * 1000.);
    fprintf (*output_file, "%-15.6lf\t", Soil->NO3_Denitrification * 1000.);
    fprintf (*output_file, "%-15.6lf\t", Soil->N2O_Denitrification * 1000.);
    fprintf (*output_file, "%-15.6lf\t", Soil->NO3Leaching * 1000.);
    fprintf (*output_file, "%-15.6lf\n", Soil->NH4Leaching * 1000.);

    fflush (*output_file);
    if (y==(SimControl->totalYears-1) && doy==Weather->lastDoy[y])
        fclose (*output_file);

}
    /*
     * Print Carbon Output
     */
if (SimControl->soilCarbonDailyOutput)
{
    sprintf (filename, "output/%s/soilC.dat", project);
    output_file = &filenames->carbon_out;

    fprintf (*output_file, "%4.4d-%2.2d-%2.2d\t", y + start_year, month, mday);
    fprintf (*output_file, "%-15.6lf\t", Soil->SOCProfile);
    fprintf (*output_file, "%-15.6lf\t", Soil->C_Humified);
    fprintf (*output_file, "%-15.6lf\t", Soil->C_ResidueRespired);
    fprintf (*output_file, "%-15.6lf\n", Soil->C_SoilRespired);

    fflush (*output_file);
    if (y==(SimControl->totalYears-1) && doy==Weather->lastDoy[y])
        fclose (*output_file);

}

    /*
     * Print residue output
     */

if (SimControl->residueDailyOutput)
{
    sprintf (filename, "output/%s/residue.dat", project);
    output_file = &filenames->residue_out;

    fprintf (*output_file, "%4.4d-%2.2d-%2.2d\t", y + start_year, month, mday);
    fprintf (*output_file, "%-15.6lf\t", Residue->residueInterception);
    fprintf (*output_file, "%-15.6lf\t", Residue->stanResidueMass + Residue->flatResidueMass);
    sum = 0.0;
    for (i = 0; i < Soil->totalLayers; i++)
        sum = sum + Residue->residueAbgd[i];
    fprintf (*output_file, "%-15.6lf\t", sum);
    sum = 0.0;
    for (i = 0; i < Soil->totalLayers; i++)
        sum = sum + Residue->residueRt[i] + Residue->residueRz[i];
    fprintf (*output_file, "%-15.6lf\t", sum);
    fprintf (*output_file, "%-15.6lf\t", Residue->manureSurfaceC);
    fprintf (*output_file, "%-15.6lf\t", Residue->stanResidueN + Residue->flatResidueN);
    sum = 0.0;
    for (i = 0; i < Soil->totalLayers; i++)
        sum = sum + Residue->residueAbgdN[i];
    fprintf (*output_file, "%-15.6lf\t", sum);
    sum = 0.0;
    for (i = 0; i < Soil->totalLayers; i++)
        sum = sum + Residue->residueRtN[i] + Residue->residueRzN[i];
    fprintf (*output_file, "%-15.6lf\t", sum);
    fprintf (*output_file, "%-15.6lf\t", Residue->manureSurfaceN);
    if (Residue->stanResidueMass > 0.0)
        fprintf (*output_file, "%-15.6lf\t", Residue->stanResidueWater / (Residue->stanResidueWater + Residue->stanResidueMass / 10.0));
    else
        fprintf (*output_file, "%-15.6lf\t", 0.0);
    if (Residue->flatResidueMass > 0.0)
        fprintf (*output_file, "%-15.6lf\n", Residue->flatResidueWater / (Residue->flatResidueWater + Residue->flatResidueMass / 10.0));
    else
        fprintf (*output_file, "%-15.6lf\n", 0.0);

    fflush (*output_file);
    if (y==(SimControl->totalYears-1) && doy==Weather->lastDoy[y])
        fclose (*output_file);
}
//Print soil layer C and N output;
if (SimControl->soilDailyOutput)
    {
    sprintf (filename, "output/%s/soilLayersCN.dat", project);
    output_file = &filenames->layers_out;

    fprintf (*output_file, "%4.4d-%2.2d-%2.2d\t", y + start_year, month, mday);
    for (i = 0; i < Soil->totalLayers; i++)
        fprintf (*output_file, "%-15.6lf\t", (Soil->NO3[i]*1000000)/(Soil->BD[i]*Soil->layerThickness[i]*10000));
        fprintf (*output_file, "\t");
    for (i = 0; i < Soil->totalLayers; i++)
        fprintf (*output_file, "%-15.6lf\t", (Soil->NH4[i]*1000000)/(Soil->BD[i]*Soil->layerThickness[i]*10000));
        fprintf (*output_file, "\t");
    for (i = 0; i < Soil->totalLayers; i++)
        fprintf (*output_file, "%-15.6lf\t", SoilCarbon->factorComposite[i]);
        fprintf (*output_file, "\t");
    fprintf (*output_file, "%-15.6lf\t", Residue->stanResidueMass*0.43);
    fprintf (*output_file, "%-15.6lf\t", Residue->flatResidueMass*0.43);
    fprintf (*output_file, "%-15.6lf\t", Residue->manureSurfaceC);
    for (i = 0; i < Soil->totalLayers; i++)
        {
        sumResidueC[i] = Residue->residueRt[i]*0.43 + Residue->residueRz[i]*0.43 + Residue->residueAbgd[i]*0.43 + Residue->manureC[i];
        sumResidueN[i] = Residue->residueRtN[i] + Residue->residueRzN[i] + Residue->residueAbgdN[i] + Residue->manureN[i];
        fprintf (*output_file, "%-15.6lf\t", sumResidueC[i]);
        }
        fprintf (*output_file, "\t");
    fprintf (*output_file, "%-15.6lf\t", Residue->stanResidueMass*0.43/Residue->stanResidueN);
    fprintf (*output_file, "%-15.6lf\t", Residue->flatResidueMass*0.43/Residue->flatResidueN);
    fprintf (*output_file, "%-15.6lf\t", Residue->manureSurfaceC/Residue->manureSurfaceN);
    for (i = 0; i < Soil->totalLayers; i++)
        fprintf (*output_file, "%-15.6lf\t", sumResidueC[i]/sumResidueN[i]);
        fprintf (*output_file, "\t");
    for (i = 0; i < Soil->totalLayers; i++)
        fprintf (*output_file, "%-15.6lf\t", Soil->MBC_Mass[i]);
        fprintf (*output_file, "\t");
    for (i = 0; i < Soil->totalLayers; i++)
        fprintf (*output_file, "%-15.6lf\t", Soil->MBC_Mass[i]/Soil->MBN_Mass[i]);
        fprintf (*output_file, "\t");
    for (i = 0; i < Soil->totalLayers; i++)
        fprintf (*output_file, "%-15.6lf\t", Soil->SOC_Mass[i]);
        fprintf (*output_file, "\t");
    for (i = 0; i < Soil->totalLayers; i++)
        fprintf (*output_file, "%-15.6lf\t", Soil->SOC_Mass[i]/Soil->SON_Mass[i]);
        fprintf (*output_file, "\t");
    for (i = 0; i < Soil->totalLayers; i++)
        fprintf (*output_file, "%-15.6lf\t", Soil->waterContent[i]);
        fprintf (*output_file, "\t");
    for (i = 0; i < Soil->totalLayers; i++)
        fprintf (*output_file, "%-15.6lf\t", Soil->layerThickness[i]);
        fprintf (*output_file, "\t");
    for (i = 0; i < Soil->totalLayers; i++)
        fprintf (*output_file, "%-15.6lf\t", Soil->BD[i]);
    fprintf(*output_file, "\n");

    fflush (*output_file);
    if (y==(SimControl->totalYears-1) && doy==Weather->lastDoy[y])
        fclose (*output_file);
    }
}

void PrintSeasonOutput (int y, int doy, int start_year, const weather_struct *Weather, const crop_struct *Crop)
{
    char            filename[256];
    FILE           *output_file;
    int             month, mday;
    int             leap_year = 0;

    if (Weather->lastDoy[y] == 366)
        leap_year = 1;



//    NextLine (simctrl_file, cmdstr);
//    ReadKeywordInt (cmdstr, "ANNUAL_SOIL_OUT", &SimControl->annualSoilOutput);
//
//    NextLine (simctrl_file, cmdstr);
//    ReadKeywordInt (cmdstr, "ANNUAL_PROFILE_OUT", &SimControl->profileOutput);
//
//    NextLine (simctrl_file, cmdstr);
//    ReadKeywordInt (cmdstr, "SEASON_OUT", &SimControl->seasonOutput);


    sprintf (filename, "output/%s/season.dat", project);
    output_file = fopen (filename, "a");

    doy2date (y + start_year, doy, &month, &mday, leap_year);
    fprintf (output_file, "%4.4d-%2.2d-%2.2d\t", y + start_year, month, mday);
    fprintf (output_file, "%-15s\t", Crop->cropName);

    fprintf (output_file, "%-15.6lf\t", Crop->rcBiomass);
    fprintf (output_file, "%-15.6lf\t", Crop->rcRoot);
    fprintf (output_file, "%-15.6lf\t", Crop->rcGrainYield);
    fprintf (output_file, "%-15.6lf\t", Crop->rcForageYield);
    fprintf (output_file, "%-15.6lf\t", Crop->rcResidueBiomass);
    fprintf (output_file, "%-15.6lf\t", Crop->rcHarvestIndex);
    fprintf (output_file, "%-15.6lf\t", Crop->rcCropTranspirationPotential);
    fprintf (output_file, "%-15.6lf\t", Crop->rcCropTranspiration);
    fprintf (output_file, "%-15.6lf\t", Crop->rcSoilWaterEvaporation);
    fprintf (output_file, "%-15.6lf\t", Crop->rcTotalNitrogen);
    fprintf (output_file, "%-15.6lf\t", Crop->rcRootNitrogen);
    fprintf (output_file, "%-15.6lf\t", Crop->rcGrainNitrogenYield);
    fprintf (output_file, "%-15.6lf\t", Crop->rcForageNitrogenYield);
    fprintf (output_file, "%-15.6lf\t", Crop->rcNitrogenCumulative);
    fprintf (output_file, "%-15.6lf\t", Crop->rcNitrogenInHarvest);
    fprintf (output_file, "%-15.6lf\t", Crop->rcNitrogenInResidue);
    fprintf (output_file, "%-15.6lf\t", Crop->rcNitrogenForageConc);
    fprintf (output_file, "\n");

    fflush (output_file);
    fclose (output_file);
}

void PrintAnnualOutput (int y, int start_year, const soil_struct *Soil, const soilc_struct *SoilCarbon)
{
    char            filename[256];
    FILE           *output_file;
    int             i;

    sprintf (filename, "output/%s/annualSOM.dat", project);
    output_file = fopen (filename, "a");

    fprintf (output_file, "%4.4d", y + start_year);
    for (i = 0; i < Soil->totalLayers; i++)
        fprintf (output_file, "\t%-15.6lf", Soil->layerThickness[i]);
    for (i = 0; i < Soil->totalLayers; i++)
        fprintf (output_file, "\t%-15.6lf", Soil->BD[i]);
    for (i = 0; i < Soil->totalLayers; i++)
        fprintf (output_file, "\t%-15.6lf", Soil->SOC_Mass[i] / Soil->layerThickness[i] / Soil->BD[i] / 100.0);
    for (i = 0; i < Soil->totalLayers; i++)
        fprintf (output_file, "\t%-15.6lf", (Soil->SOC_Mass[i] / Soil->layerThickness[i] / Soil->BD[i] / 100.0)/(2.11 + 0.0375 * Soil->Clay[i] * 100.0));

    fprintf (output_file, "\n");
    fflush (output_file);
    fclose (output_file);

//    sprintf (filename, "output/%s/annualSoilC.dat", project);
//    output_file = fopen (filename, "a");
//
//    fprintf (output_file, "%4.4d", y + start_year);
//    for (i = 0; i < Soil->totalLayers; i++)
//        fprintf (output_file, "\t%-15.6lf", Soil->layerThickness[i]);
//    for (i = 0; i < Soil->totalLayers; i++)
//        fprintf (output_file, "\t%-15.6lf", Soil->BD[i]);
//    for (i = 0; i < Soil->totalLayers; i++)
//        fprintf (output_file, "\t%-15.6lf", Soil->Clay[i] * 100.0);
//    for (i = 0; i < Soil->totalLayers; i++)
//        fprintf (output_file, "\t%-15.6lf", SoilCarbon->annualDecompositionFactor[i]);
//    for (i = 0; i < Soil->totalLayers; i++)
//        fprintf (output_file, "\t%-15.6lf", SoilCarbon->annualSoilCarbonDecompositionRate[i]);
//    for (i = 0; i < Soil->totalLayers; i++)
//        fprintf (output_file, "\t%-15.6lf", Soil->SOC_Mass[i]);
//    for (i = 0; i < Soil->totalLayers; i++)
//        fprintf (output_file, "\t%-15.6lf", SoilCarbon->annualRespiredCarbonMass[i]);
//    for (i = 0; i < Soil->totalLayers; i++)
//        fprintf (output_file, "\t%-15.6lf", SoilCarbon->annualCarbonInputByLayer[i]);
//    for (i = 0; i < Soil->totalLayers; i++)
//        fprintf (output_file, "\t%-15.6lf", SoilCarbon->annualHumifiedCarbonMass[i]);
//    for (i = 0; i < Soil->totalLayers; i++)
//    {
//        if (SoilCarbon->annualCarbonInputByLayer[i] > 0)
//            fprintf (output_file, "\t%-15.6lf", SoilCarbon->annualHumifiedCarbonMass[i] / SoilCarbon->annualCarbonInputByLayer[i]);
//        else
//            fprintf (output_file, "\t%-15.6lf", 0.0);
//    }
//    for (i = 0; i < Soil->totalLayers; i++)
//        fprintf (output_file, "\t%-15.6lf", 10.0 * (2.11 + 0.0375 * Soil->Clay[i] * 100.0));
//    for (i = 0; i < Soil->totalLayers; i++)
//        fprintf (output_file, "\t%-15.6lf", 10000.0 * Soil->layerThickness[i] * Soil->BD[i] * (10.0 * (2.11 + 0.0375 * Soil->Clay[i] * 100.0)) / 1000.0);
//
//    fprintf (output_file, "\n");
//    fflush (output_file);
//    fclose (output_file);


    sprintf (filename, "output/%s/annualSoilProfileC.dat", project);
    output_file = fopen (filename, "a");

    fprintf (output_file, "%4.4d", y + start_year);
    for (i = 0; i < Soil->totalLayers; i++)
        fprintf (output_file, "\t%-15.6lf", SoilCarbon->abgdBiomassInput[i]);
    for (i = 0; i < Soil->totalLayers; i++)
        fprintf (output_file, "\t%-15.6lf", SoilCarbon->rootBiomassInput[i] + SoilCarbon->rhizBiomassInput[i]);
    for (i = 0; i < Soil->totalLayers; i++)
        fprintf (output_file, "\t%-15.6lf", SoilCarbon->abgdCarbonInput[i]);
    for (i = 0; i < Soil->totalLayers; i++)
        fprintf (output_file, "\t%-15.6lf", SoilCarbon->rootCarbonInput[i] + SoilCarbon->rhizCarbonInput[i]);
    for (i = 0; i < Soil->totalLayers; i++)
        fprintf (output_file, "\t%-15.6lf", SoilCarbon->carbonMassInitial[i]);
    for (i = 0; i < Soil->totalLayers; i++)
        fprintf (output_file, "\t%-15.6lf", SoilCarbon->annualHumifiedCarbonMass[i]);
    for (i = 0; i < Soil->totalLayers; i++)
        fprintf (output_file, "\t%-15.6lf", SoilCarbon->annualRespiredCarbonMass[i]);
    for (i = 0; i < Soil->totalLayers; i++)
        fprintf (output_file, "\t%-15.6lf", SoilCarbon->carbonMassFinal[i]);
    for (i = 0; i < Soil->totalLayers; i++)
        fprintf (output_file, "\t%-15.6lf", SoilCarbon->carbonMassFinal[i] - SoilCarbon->carbonMassInitial[i]);
    fprintf (output_file, "\n");
    fflush (output_file);
    fclose (output_file);
}

void PrintCarbonEvolution (int y, int start_year, int total_layers, const soil_struct *Soil, const soilc_struct *SoilCarbon, const residue_struct *Residue)
{
    char            filename[256];
    FILE           *output_file;
    int             i;

    double          SOC_sum;
    double          SOC_Mass_Depth1, SOC_Mass_Depth2;
    double          layerSplit;
    double          sumProfile[18];

    sprintf (filename, "output/%s/annualSoilCEvol.dat", project);
    output_file = fopen (filename, "a");

    for (i = 0; i < 18; i++)
        sumProfile[i] = 0.0;

    SOC_Mass_Depth1 = 0.0;
    SOC_Mass_Depth2 = 0.0;
    layerSplit = 0.3;

    for (i = 0; i < total_layers; i++)
    {
        sumProfile[0] += SoilCarbon->abgdBiomassInput[i];
        sumProfile[1] += SoilCarbon->rootBiomassInput[i] + SoilCarbon->rhizCarbonInput[i];
        sumProfile[2] += SoilCarbon->abgdCarbonInput[i];
        sumProfile[3] += SoilCarbon->rootCarbonInput[i] + SoilCarbon->rhizCarbonInput[i];
        sumProfile[4] += SoilCarbon->carbonMassInitial[i];
        sumProfile[5] += SoilCarbon->annualHumifiedCarbonMass[i];
        sumProfile[6] += SoilCarbon->annualRespiredCarbonMass[i];
        sumProfile[7] += SoilCarbon->carbonMassFinal[i];

        sumProfile[8] += SoilCarbon->annualNmineralization[i];
        sumProfile[9] += SoilCarbon->annualNImmobilization[i];
        sumProfile[10] += SoilCarbon->annualNNetMineralization[i];

        //soilCarbonEvolutionVars(y, 2 + i) = SoilCarbon.carbonMassFinal(y, i) 'Updated_SOC_Mass(i)

        if (Soil->cumulativeDepth[i] <= layerSplit)
            SOC_Mass_Depth1 += Soil->SOC_Mass[i];
        else if (Soil->cumulativeDepth[i] > layerSplit && Soil->cumulativeDepth[i] < layerSplit + (i == total_layers - 1 ? 0 : Soil->layerThickness[i + 1]))
        {
            SOC_Mass_Depth1 += Soil->SOC_Mass[i] * (layerSplit - (Soil->cumulativeDepth[i] - Soil->layerThickness[i])) / Soil->layerThickness[i];
            SOC_Mass_Depth2 += Soil->SOC_Mass[i] * (Soil->cumulativeDepth[i] - layerSplit) / Soil->layerThickness[i];
        }
        else
            SOC_Mass_Depth2 += Soil->SOC_Mass[i];
    }

    sumProfile[11] = SoilCarbon->annualAmmoniumNitrification;
    sumProfile[12] = SoilCarbon->annualNitrousOxidefromNitrification;
    sumProfile[13] = SoilCarbon->annualAmmoniaVolatilization;
    sumProfile[14] = SoilCarbon->annualNO3Denitrification;
    sumProfile[15] = SoilCarbon->annualNitrousOxidefromDenitrification;
    sumProfile[16] = SoilCarbon->annualNitrateLeaching;
    sumProfile[17] = SoilCarbon->annualAmmoniumLeaching;

    fprintf (output_file, "%4.4d", y + start_year);
    SOC_sum = 0.0;
    for (i = 0; i < total_layers; i++)
        SOC_sum = SOC_sum + Soil->SOC_Mass[i];
    fprintf (output_file, "\t%-15.6lf", SOC_sum);
    fprintf (output_file, "\t%-15.6lf\t%-15.6lf", SOC_Mass_Depth1, SOC_Mass_Depth2);
    for (i = 0; i < Soil->totalLayers; i++)
        fprintf (output_file, "\t%-15.6lf", Soil->SOC_Mass[i]);
    fprintf (output_file, "\t%-15.6lf", Residue->yearResidueBiomass);
    fprintf (output_file, "\t%-15.6lf", Residue->yearRootBiomass);
    fprintf (output_file, "\t%-15.6lf", sumProfile[0]);
    fprintf (output_file, "\t%-15.6lf", sumProfile[1]);
    fprintf (output_file, "\t%-15.6lf", sumProfile[4]);
    fprintf (output_file, "\t%-15.6lf", sumProfile[2]);
    fprintf (output_file, "\t%-15.6lf", sumProfile[3]);
    fprintf (output_file, "\t%-15.6lf", sumProfile[5]);
    fprintf (output_file, "\t%-15.6lf", sumProfile[6]);
    fprintf (output_file, "\t%-15.6lf", sumProfile[7]);
    fprintf (output_file, "\t%-15.6lf", sumProfile[7] - sumProfile[4]);
    fprintf (output_file, "\t%-15.6lf", sumProfile[8]);
    fprintf (output_file, "\t%-15.6lf", sumProfile[9]);
    fprintf (output_file, "\t%-15.6lf", sumProfile[10]);
    fprintf (output_file, "\t%-15.6lf", sumProfile[11]);
    fprintf (output_file, "\t%-15.6lf", sumProfile[12]);
    fprintf (output_file, "\t%-15.6lf", sumProfile[13]);
    fprintf (output_file, "\t%-15.6lf", sumProfile[14]);
    fprintf (output_file, "\t%-15.6lf", sumProfile[15]);
    fprintf (output_file, "\t%-15.6lf", sumProfile[16]);
    fprintf (output_file, "\t%-15.6lf", sumProfile[17]);

    for (i = 0; i < Soil->totalLayers; i++)
        fprintf (output_file, "\t%-15.6lf", 1000.0 * Soil->SOC_Mass[i] /
            Soil->BD[i] / 10000.0 / Soil->layerThickness[i]);

    fprintf (output_file, "\n");
    fflush (output_file);
    fclose (output_file);
}


void PrintSummary (const summary_struct *Summary, int totalYears)
{
    char            filename[256];
    FILE           *output_file;

    sprintf (filename, "output/%s/summary.dat", project);
    output_file = fopen (filename, "a");

    printf ("\nSoil Carbon Summary:\n");
    printf ("%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\n", "INIT PROF C", "FINAL PROF C", "PROF C DIFF", "RES C INPUT", "ROOT C INPUT", "HUMIFIED C", "RESP SOIL C", "RESP RES C", "RETAINED RES", "PRODUCED ROOT", "SOIL C CHG/YR");
    printf ("%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\n", "Mg C", "Mg C", "Mg C", "Mg C", "Mg C", "Mg C", "Mg C", "Mg/ha", "Mg/ha", "Mg/ha", "kg C");
    printf ("%-15.6lf\t%-15.6lf\t%-15.6lf\t%-15.6lf\t%-15.6lf\t%-15.6lf\t%-15.6lf\t%-15.6lf\t%-15.6lf\t%-15.6lf\t%-15.6lf\n", Summary->initial_soc, Summary->final_soc, Summary->final_soc - Summary->initial_soc, Summary->abgd_c_input, Summary->root_c_input, Summary->hum, Summary->soil_resp, Summary->residue_resp, Summary->residue_biomass, Summary->produced_root, (Summary->final_soc - Summary->initial_soc) / (double) totalYears * 1000.0);

    fprintf (output_file, "%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\n", "INIT PROF C", "FINAL PROF C", "PROF C DIFF", "RES C INPUT", "ROOT C INPUT", "HUMIFIED C", "RESP SOIL C", "RESP RES C", "RETAINED RES", "PRODUCED ROOT", "SOIL C CHG/YR");
    fprintf (output_file, "%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\n", "Mg C", "Mg C", "Mg C", "Mg C", "Mg C", "Mg C", "Mg C", "Mg/ha", "Mg/ha", "Mg/ha", "kg C");
    fprintf (output_file, "%-15.6lf\t%-15.6lf\t%-15.6lf\t%-15.6lf\t%-15.6lf\t%-15.6lf\t%-15.6lf\t%-15.6lf\t%-15.6lf\t%-15.6lf\t%-15.6lf\n", Summary->initial_soc, Summary->final_soc, Summary->final_soc - Summary->initial_soc, Summary->abgd_c_input, Summary->root_c_input, Summary->hum, Summary->soil_resp, Summary->residue_resp, Summary->residue_biomass, Summary->produced_root, (Summary->final_soc - Summary->initial_soc) / (double) totalYears * 1000.0);

    printf ("\n");
    printf ("%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\n", "AVG GROSS N MIN", "AVG N IMMOB", "AVG NET N MIN", "AVG NH4 NITRIF", "AVG N2O FR NIT", "AVG NH3 VOLATIL", "AVG NO3 DENIT", "AVG N2O FR DENI", "AVG NO3 LEACH", "AVG NH4 LEACH", "AVG TOT N2O EMI");
    printf ("%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\n", "kg N/yr", "kg N/yr", "kg N/yr", "kg N/yr", "kg N/yr", "kg N/yr", "kg N/yr", "kg N/yr", "kg N/yr", "kg N/yr", "kg N/yr");
    printf ("%-15.6lf\t%-15.6lf\t%-15.6lf\t%-15.6lf\t%-15.6lf\t%-15.6lf\t%-15.6lf\t%-15.6lf\t%-15.6lf\t%-15.6lf\t%-15.6lf\n", Summary->n_mineralization / (double) totalYears, Summary->n_immobilization / (double) totalYears, Summary->n_net_mineralization / (double) totalYears, Summary->nh4_nitrification / (double) totalYears, Summary->n2o_from_nitrification / (double) totalYears, Summary->nh3_volatilization / (double) totalYears, Summary->no3_denirification / (double) totalYears, Summary->n2o_from_denitrification / (double) totalYears, Summary->no3_leaching / (double) totalYears, Summary->nh4_leaching / (double) totalYears, (Summary->n2o_from_nitrification + Summary->n2o_from_denitrification) / (double) totalYears);

    fprintf (output_file, "\n");
    fprintf (output_file, "%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\n", "AVG GROSS N MIN", "AVG N IMMOB", "AVG NET N MIN", "AVG NH4 NITRIF", "AVG N2O FR NIT", "AVG NH3 VOLATIL", "AVG NO3 DENIT", "AVG N2O FR DENI", "AVG NO3 LEACH", "AVG NH4 LEACH", "AVG TOT N2O EMI");
    fprintf (output_file, "%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s\n", "kg N/yr", "kg N/yr", "kg N/yr", "kg N/yr", "kg N/yr", "kg N/yr", "kg N/yr", "kg N/yr", "kg N/yr", "kg N/yr", "kg N/yr");
    fprintf (output_file, "%-15.6lf\t%-15.6lf\t%-15.6lf\t%-15.6lf\t%-15.6lf\t%-15.6lf\t%-15.6lf\t%-15.6lf\t%-15.6lf\t%-15.6lf\t%-15.6lf\n", Summary->n_mineralization / (double) totalYears, Summary->n_immobilization / (double) totalYears, Summary->n_net_mineralization / (double) totalYears, Summary->nh4_nitrification / (double) totalYears, Summary->n2o_from_nitrification / (double) totalYears, Summary->nh3_volatilization / (double) totalYears, Summary->no3_denirification / (double) totalYears, Summary->n2o_from_denitrification / (double) totalYears, Summary->no3_leaching / (double) totalYears, Summary->nh4_leaching / (double) totalYears, (Summary->n2o_from_nitrification + Summary->n2o_from_denitrification) / (double) totalYears);
    fflush (output_file);
    fclose (output_file);
}
