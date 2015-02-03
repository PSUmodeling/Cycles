#include "include/Cycles.h"

void InitializeOutput (char *project)
{
    char            filename[50];
    char           *output_dir;
    FILE           *output_file;

    output_dir = (char *)malloc ((strlen (project) + 8) * sizeof (char));

    mkdir ("output", 0755);
    sprintf (output_dir, "output/%s", project);
    mkdir (output_dir, 0755);

    sprintf (filename, "output/%s/weather.dat", project);
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
    fprintf (output_file, "%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\n", "YEAR-DOY", "CROP", "TOTAL", "ROOT", "GRAIN", "FORAGE", "AVG", "HARVEST", "POTENT", "ACTUAL", "SOIL", "TOTAL N", "ROOT N", "GRAIN N", "FORAGE N", "CUMULAT N");
    fprintf (output_file, "%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\n", "", "", "BIOMASS", "BIOMASS", "YIELD", "YIELD", "RESIDUE", "INDEX", "TRANSP", "TRANSP", "EVAP", "", "", "", "", "STRESS");
    fprintf (output_file, "%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\n", "YEAR-DOY", "N/A", "Mg/ha", "Mg/ha", "Mg/ha", "Mg/ha", "Mg/ha", "Mg/Mg", "mm", "mm", "mm", "Mg/ha", "Mg/ha", "Mg/ha", "Mg/ha", "");
    fclose (output_file);

    sprintf (filename, "output/%s/water.dat", project);
    output_file = fopen (filename, "w");
    fprintf (output_file, "%-10s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\n", "DATE", "IRRIG", "RUNOFF", "INFILT", "DRAIN", "SOIL E", "RES E", "SNOW S", "TRANSP");
    fprintf (output_file, "%-10s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\n", "YYYY-MM-DD", "mm/day", "mm/day", "mm/day", "mm/day", "mm/day", "mm/day", "mm/day", "mm/day");
    fclose (output_file);

    sprintf (filename, "output/%s/N.dat", project);
    output_file = fopen (filename, "w");
    fclose (output_file);

    sprintf (filename, "output/%s/residue.dat", project);
    output_file = fopen (filename, "w");
    fclose (output_file);

    sprintf (filename, "output/%s/C.dat", project);
    output_file = fopen (filename, "w");
    fclose (output_file);

    free (output_dir);
}

void Initialize (SimControlStruct *SimControl, WeatherStruct *Weather, SoilStruct *Soil, ResidueStruct *Residue, SoilCarbonStruct *SoilCarbon, CropStruct *Crop, CropManagementStruct *CropManagement, SnowStruct *Snow)
{
    int             i, j;
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
    CropManagement->tillageFactor = (double *)malloc (Soil->totalLayers * sizeof (double));

    /* Initialize snow structure */
    Snow->Snow = 0.;
}

void FreeCyclesStruct (CyclesStruct Cycles, int total_years)
{
    int             i;

    free (Cycles->CropManagement.tillageFactor);
    free (Cycles->CropManagement.describedCrop);
    free (Cycles->CropManagement.plantingOrder);
    free (Cycles->CropManagement.FixedFertilization);
    free (Cycles->CropManagement.FixedIrrigation);
    free (Cycles->CropManagement.Tillage);
    free (Cycles->CropManagement.autoIrrigation);

    free (Cycles->Soil.layerThickness);
    free (Cycles->Soil.Clay);
    free (Cycles->Soil.Sand);
    free (Cycles->Soil.IOM);
    free (Cycles->Soil.BD);
    free (Cycles->Soil.FC);
    free (Cycles->Soil.PWP);
    free (Cycles->Soil.NO3);
    free (Cycles->Soil.NH4);
    free (Cycles->Soil.nodeDepth);
    free (Cycles->Soil.cumulativeDepth);
    free (Cycles->Soil.waterContent);
    free (Cycles->Soil.soilTemperature);
    free (Cycles->Soil.Porosity);
    free (Cycles->Soil.PAW);
    free (Cycles->Soil.FC_WaterPotential);
    free (Cycles->Soil.airEntryPotential);
    free (Cycles->Soil.B_Value);
    free (Cycles->Soil.M_Value);
    free (Cycles->Soil.SOC_Conc);
    free (Cycles->Soil.SOC_Mass);
    free (Cycles->Soil.SON_Mass);
    free (Cycles->Soil.MBC_Mass);
    free (Cycles->Soil.MBN_Mass);
    free (Cycles->Soil.waterUptake);
    free (Cycles->Soil.pH);
    free (Cycles->Soil.n2o);

    for (i = 0; i < total_years; i++)
    {
        free (Cycles->Weather.wind[i]);
        free (Cycles->Weather.ETref[i]);
        free (Cycles->Weather.precipitation[i]);
        free (Cycles->Weather.RHmax[i]);
        free (Cycles->Weather.RHmin[i]);
        free (Cycles->Weather.solarRadiation[i]);
        free (Cycles->Weather.tMax[i]);
        free (Cycles->Weather.tMin[i]);
    }
    free (Cycles->Weather.wind);
    free (Cycles->Weather.ETref);
    free (Cycles->Weather.precipitation);
    free (Cycles->Weather.RHmax);
    free (Cycles->Weather.RHmin);
    free (Cycles->Weather.solarRadiation);
    free (Cycles->Weather.tMax);
    free (Cycles->Weather.tMin);
    free (Cycles->Weather.yearlyAmplitude);
    free (Cycles->Weather.annualAverageTemperature);

    free (Cycles->Residue.residueAbgd);
    free (Cycles->Residue.residueRt);
    free (Cycles->Residue.residueRz);
    free (Cycles->Residue.residueAbgdN);
    free (Cycles->Residue.residueRtN);
    free (Cycles->Residue.residueRzN);
    free (Cycles->Residue.manureC);
    free (Cycles->Residue.manureN);

    free (Cycles->SoilCarbon.factorComposite);
    free (Cycles->SoilCarbon.carbonRespired);
    free (Cycles->SoilCarbon.rootBiomassInput);
    free (Cycles->SoilCarbon.rhizBiomassInput);
    free (Cycles->SoilCarbon.abgdBiomassInput);
    free (Cycles->SoilCarbon.rootCarbonInput);
    free (Cycles->SoilCarbon.rhizCarbonInput);
    free (Cycles->SoilCarbon.manuCarbonInput);
    free (Cycles->SoilCarbon.abgdCarbonInput);
    free (Cycles->SoilCarbon.carbonMassInitial);
    free (Cycles->SoilCarbon.carbonMassFinal);
    free (Cycles->SoilCarbon.annualDecompositionFactor);
    free (Cycles->SoilCarbon.annualSoilCarbonDecompositionRate);
    free (Cycles->SoilCarbon.annualCarbonInputByLayer);
    free (Cycles->SoilCarbon.annualHumifiedCarbonMass);
    free (Cycles->SoilCarbon.annualRespiredCarbonMass);
    free (Cycles->SoilCarbon.annualRespiredResidueCarbonMass);
    free (Cycles->SoilCarbon.annualHumificationCoefficient);
}
