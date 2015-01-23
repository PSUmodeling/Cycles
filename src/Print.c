#include "include/Cycles.h"

void PrintDailyOutput (int y, int doy, int start_year, const WeatherStruct *Weather, const CropStruct *Crop, const char *project)
{
    char filename[50];
    FILE *output_file;

    /*
     * Print weather output
     */
    sprintf (filename, "output/%s/weather.dat", project);
    output_file = fopen (filename, "a");

    fprintf (output_file, "%4.4d-%3.3d\t", y + start_year, doy);
    fprintf (output_file, "%-8.2lf\t", 0.5 * Weather->tMin[y][doy - 1] + 0.5 * Weather->tMax[y][doy - 1]);
    fprintf (output_file, "%-8.4lf\t", Weather->ETref[y][doy - 1]);
    fprintf (output_file, "%-8.2lf\n", Weather->precipitation[y][doy - 1]);

    fflush (output_file);
    fclose (output_file);

    /*
     * Print crop output
     */
    sprintf (filename, "output/%s/crop.dat", project);
    output_file = fopen (filename, "a");

    fprintf (output_file, "%4.4d-%3.3d\t", y + start_year, doy);
    if (Crop->cropUniqueIdentifier < 0)
    {
        fprintf (output_file, "%-8.8s\t", "Fallow");
        fprintf (output_file, "%-8.8s\t", "N/A");
    }
    else
    {
        fprintf (output_file, "%-8.8s\t", Crop->cropName);
        switch (Crop->stageGrowth)
        {
            case PRE_EMERGENCE:
                fprintf (output_file, "%-24.24s\t", "PRE_EMERGENCE");
                break;
            case VEGETATIVE_GROWTH:
                fprintf (output_file, "%-24.24s\t", "VEGETATIVE_GROWTH");
                break;
            case PERENNIAL:
                fprintf (output_file, "%-24.24s\t", "PERENNIAL");
                break;
            case REPRODUCTIVE_GROWTH:
                fprintf (output_file, "%-24.24s\t", "REPRODUCTIVE_GROWTH");
                break;
            case MATURITY:
                fprintf (output_file, "%-24.24s\t", "MATURITY");
                break;
            case CLIPPING:
                fprintf (output_file, "%-24.24s\t", "CLIPPING");
                break;
            case PLANTING:
                fprintf (output_file, "%-24.24s\t", "PLANTING");
                break;
        }
    }
        
    fprintf (output_file, "%-8.2lf\t", Crop->svTT_Cumulative);
    fprintf (output_file, "%-8.2lf\t", Crop->svBiomass);
    fprintf (output_file, "%-8.2lf\t", Crop->svShoot);
    fprintf (output_file, "%-8.2lf\t", Crop->svRoot);
    fprintf (output_file, "%-8.2lf\t", Crop->svRadiationInterception);
    fprintf (output_file, "%-8.2lf\t", (Crop->svN_Shoot + Crop->svN_Root) * 1000.);
    fprintf (output_file, "%-8.2lf\t", Crop->svN_Shoot * 1000.);
    fprintf (output_file, "%-8.2lf", Crop->svN_Root * 1000.);
    fprintf (output_file, "\n");

    fflush (output_file);
    fclose (output_file);
}

void PrintSeasonOutput (int y, int doy, int start_year, const CropStruct *Crop, const char *project)
{
    char filename[50];
    FILE *output_file;
    int month, mday;

    sprintf (filename, "output/%s/season.dat", project);
    output_file = fopen (filename, "a");

    doy2date (y + start_year, doy, &month, &mday, 0);
    fprintf (output_file, "%4.4d-%2.2d-%2.2d\t", y + start_year, month, mday);
    fprintf (output_file, "%-8.8s\t", Crop->cropName);

    fprintf (output_file, "%-8.4lf\t", Crop->rcBiomass);
    fprintf (output_file, "%-8.4lf\t", Crop->rcRoot);
    fprintf (output_file, "%-8.4lf\t", Crop->rcGrainYield);
    fprintf (output_file, "%-8.4lf\t", Crop->rcForageYield);
    fprintf (output_file, "%-8.4lf\t", Crop->rcResidueBiomass);
    fprintf (output_file, "%-8.4lf\t", Crop->rcHarvestIndex);
    fprintf (output_file, "%-8.2lf\t", Crop->rcCropTranspirationPotential);
    fprintf (output_file, "%-8.2lf\t", Crop->rcCropTranspiration);
    fprintf (output_file, "%-8.2lf\t", Crop->rcSoilWaterEvaporation);
    fprintf (output_file, "%-8.4lf\t", Crop->rcTotalNitrogen);
    fprintf (output_file, "%-8.4lf\t", Crop->rcRootNitrogen);
    fprintf (output_file, "%-8.4lf\t", Crop->rcGrainNitrogenYield);
    fprintf (output_file, "%-8.4lf\t", Crop->rcForageNitrogenYield);
    fprintf (output_file, "%-8.4lf\t", Crop->rcNitrogenCumulative);
    fprintf (output_file, "\n");

    fflush (output_file);
    fclose (output_file);
} 
