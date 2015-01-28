#include "include/Cycles.h"

void PrintDailyOutput (int y, int doy, int start_year, const WeatherStruct *Weather, const CropStruct *Crop, const SoilStruct *Soil, const SnowStruct *Snow, const char *project)
{
    char filename[50];
    FILE *output_file;
    int month, mday;

    doy2date (y + start_year, doy, &month, &mday, 0);

    /*
     * Print weather output
     */
    sprintf (filename, "output/%s/weather.dat", project);
    output_file = fopen (filename, "a");

    fprintf (output_file, "%4.4d-%2.2d-%2.2d\t", y + start_year, month, mday);
    fprintf (output_file, "%-7.2lf\t", 0.5 * Weather->tMin[y][doy - 1] + 0.5 * Weather->tMax[y][doy - 1]);
    fprintf (output_file, "%-7.3lf\t", Weather->ETref[y][doy - 1]);
    fprintf (output_file, "%-7.2lf\n", Weather->precipitation[y][doy - 1]);

    fflush (output_file);
    fclose (output_file);

    /*
     * Print crop output
     */
    sprintf (filename, "output/%s/crop.dat", project);
    output_file = fopen (filename, "a");

    fprintf (output_file, "%4.4d-%2.2d-%2.2d\t", y + start_year, month, mday);
    if (Crop->cropUniqueIdentifier < 0)
    {
        fprintf (output_file, "%-15s\t", "Fallow");
        fprintf (output_file, "%-23s\t", "N/A");
    }
    else
    {
        fprintf (output_file, "%-15s\t", Crop->cropName);
        switch (Crop->stageGrowth)
        {
            case PRE_EMERGENCE:
                fprintf (output_file, "%-23s\t", "PRE_EMERGENCE");
                break;
            case VEGETATIVE_GROWTH:
                fprintf (output_file, "%-23s\t", "VEGETATIVE_GROWTH");
                break;
            case PERENNIAL:
                fprintf (output_file, "%-23s\t", "PERENNIAL");
                break;
            case REPRODUCTIVE_GROWTH:
                fprintf (output_file, "%-23s\t", "REPRODUCTIVE_GROWTH");
                break;
            case MATURITY:
                fprintf (output_file, "%-23s\t", "MATURITY");
                break;
            case CLIPPING:
                fprintf (output_file, "%-23s\t", "CLIPPING");
                break;
            case PLANTING:
                fprintf (output_file, "%-23s\t", "PLANTING");
                break;
        }
    }
        
    fprintf (output_file, "%-7.2lf\t", Crop->svTT_Cumulative);
    fprintf (output_file, "%-7.3lf\t", Crop->svBiomass);
    fprintf (output_file, "%-7.3lf\t", Crop->svShoot);
    fprintf (output_file, "%-7.3lf\t", Crop->svRoot);
    fprintf (output_file, "%-7.4lf\t", Crop->svRadiationInterception);
    fprintf (output_file, "%-7.2lf\t", (Crop->svN_Shoot + Crop->svN_Root) * 1000.);
    fprintf (output_file, "%-7.2lf\t", Crop->svN_Shoot * 1000.);
    fprintf (output_file, "%-7.2lf\t", Crop->svN_Root * 1000.);
    if (Crop->svShoot > 0.)
        fprintf (output_file, "%-7.2lf\t", (Crop->svN_Shoot / Crop->svShoot) * 1000.);
    else
        fprintf (output_file, "%-7.2lf\t", 0.);
    fprintf (output_file, "%-7.2lf\t", Crop->svN_Fixation * 1000.);
    fprintf (output_file, "%-7.2lf\t", Crop->svN_AutoAdded * 1000.);
    fprintf (output_file, "%-7.2lf\t", Crop->svN_StressFactor);
    fprintf (output_file, "%-7.2lf\t", Crop->svWaterStressFactor);
    fprintf (output_file, "%-7.2lf\n", Crop->svTranspirationPotential);

    fflush (output_file);
    fclose (output_file);

    /*
     * Print Water output
     */
    sprintf (filename, "output/%s/water.dat", project);
    output_file = fopen (filename, "a");

    fprintf (output_file, "%4.4d-%2.2d-%2.2d\t", y + start_year, month, mday);
    fprintf (output_file, "%-7.3lf\t", Soil->irrigationVol);
    fprintf (output_file, "%-7.3lf\t", Soil->runoffVol);
    fprintf (output_file, "%-7.3lf\t", Soil->infiltrationVol);
    fprintf (output_file, "%-7.3lf\t", Soil->drainageVol);
    fprintf (output_file, "%-7.3lf\t", Soil->evaporationVol);
    fprintf (output_file, "%-7.3lf\t", Soil->residueEvaporationVol);
    fprintf (output_file, "%-7.3lf\t", Snow->snowEvaporationVol);
    fprintf (output_file, "%-7.3lf\n", Crop->svTranspiration);

    fflush (output_file);
    fclose (output_file);

    /*
     * Print Nitrogen Output
     */
    sprintf (filename, "output/%s/N.dat", project);
    output_file = fopen (filename, "a");

    fprintf (output_file, "%4.4d-%2.2d-%2.2d\t", y + start_year, month, mday);
    fprintf (output_file, "%-7.1lf\t", Soil->SONProfile * 1000.);
    fprintf (output_file, "%-7.3lf\t", Soil->NO3Profile * 1000.);
    fprintf (output_file, "%-7.4lf\t", Soil->NH4Profile * 1000.);
    fprintf (output_file, "%-7.4lf\t", Soil->N_Mineralization * 1000.);
    fprintf (output_file, "%-7.4lf\t", Soil->N_Immobilization * 1000.);
    fprintf (output_file, "%-7.4lf\t", Soil->N_NetMineralization * 1000.);
    fprintf (output_file, "%-7.5lf\t", Soil->NH4_Nitrification * 1000.);
    fprintf (output_file, "%-7.5lf\t", Soil->N2O_Nitrification * 1000.);
    fprintf (output_file, "%-7.5lf\t", Soil->NH4_Volatilization * 1000.);
    fprintf (output_file, "%-7.5lf\t", Soil->NO3_Denitrification * 1000.);
    fprintf (output_file, "%-7.5lf\t", Soil->N2O_Denitrification * 1000.);
    fprintf (output_file, "%-7.5lf\t", Soil->NO3Leaching * 1000.);
    fprintf (output_file, "%-7.5lf\n", Soil->NH4Leaching * 1000.);

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
    fprintf (output_file, "%-15s\t", Crop->cropName);

    fprintf (output_file, "%-7.3lf\t", Crop->rcBiomass);
    fprintf (output_file, "%-7.3lf\t", Crop->rcRoot);
    fprintf (output_file, "%-7.3lf\t", Crop->rcGrainYield);
    fprintf (output_file, "%-7.3lf\t", Crop->rcForageYield);
    fprintf (output_file, "%-7.3lf\t", Crop->rcResidueBiomass);
    fprintf (output_file, "%-7.4lf\t", Crop->rcHarvestIndex);
    fprintf (output_file, "%-7.2lf\t", Crop->rcCropTranspirationPotential);
    fprintf (output_file, "%-7.2lf\t", Crop->rcCropTranspiration);
    fprintf (output_file, "%-7.2lf\t", Crop->rcSoilWaterEvaporation);
    fprintf (output_file, "%-7.3lf\t", Crop->rcTotalNitrogen);
    fprintf (output_file, "%-7.4lf\t", Crop->rcRootNitrogen);
    fprintf (output_file, "%-7.4lf\t", Crop->rcGrainNitrogenYield);
    fprintf (output_file, "%-7.3lf\t", Crop->rcForageNitrogenYield);
    fprintf (output_file, "%-7.3lf\t", Crop->rcNitrogenCumulative);
    fprintf (output_file, "\n");

    fflush (output_file);
    fclose (output_file);
} 
