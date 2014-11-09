#include "include/Cycles.h"

void PrintSimContrl (SimControlStruct SimControl)
{
    printf ("\n*Simulation Control:\n");
    printf ("*%-22s\t%-4d\n", "Simulation Start Year", SimControl.simStartYear);
    printf ("*%-22s\t%-4d\n", "Simulation End Year", SimControl.simEndYear);
    printf ("*%-22s\t%-4d\n", "Simulation Total Years", SimControl.totalYears);
    printf ("*%-22s\t%-4d\n", "Rotation Size", SimControl.yearsInRotation);
    printf ("*%-22s\t%-4d\n", "Adjusted Yields", SimControl.adjustedYields);
    printf ("*%-22s\t%-4d\n", "Hourly Infiltration", SimControl.hourlyInfiltration);
    printf ("*%-22s\t%-4d\n", "Automatic Nitrogen", SimControl.automaticNitrogen);
    printf ("*%-22s\t%-4d\n", "Automatic Phosphorus", SimControl.automaticPhosphorus);
    printf ("*%-22s\t%-4d\n", "Automatic Sulfur", SimControl.automaticSulfur);
    printf ("*%-22s\t%-4d\n", "Daily Weather Out", SimControl.weatherDailyOutput);
    printf ("*%-22s\t%-4d\n", "Daily Crop Out", SimControl.cropDailyOutput);
    printf ("*%-22s\t%-4d\n", "Daily Residue", SimControl.residueDailyOutput);
    printf ("*%-22s\t%-4d\n", "Daily Water Out", SimControl.waterDailyOutput);
    printf ("*%-22s\t%-4d\n", "Daily Nitrogen Out", SimControl.nitrogenDailyOutput);
    printf ("*%-22s\t%-4d\n", "Daily Soil Carbon Out", SimControl.soilCarbonDailyOutput);
    printf ("*%-22s\t%-4d\n", "Daily Soil Out", SimControl.soilDailyOutput);
    printf ("*%-22s\t%-4d\n", "Annual Soil Out", SimControl.annualSoilOutput);
    printf ("*%-22s\t%-4d\n", "Annual Profile Out", SimControl.profileOutput);
    printf ("*%-22s\t%-4d\n", "Season_Out", SimControl.seasonOutput);
    printf ("\n");
    printf ("(Press any key to continue ...)\n");
    getchar();
}

void PrintSoil (SoilStruct Soil)
{
    int             i;

    printf ("\n*Soil Description:\n");
    printf ("*%-12s\t%-3d\n", "Curve number", Soil.Curve_Number);
    printf ("*%-12s\t%-4.2lf\n", "Slope", Soil.Percent_Slope);
    printf ("*%-12s\t%-3d\n", "Total layers", Soil.totalLayers);
    printf ("*%-10s\t%-10s\t%-10s\t%-10s\t%-10s\t%-10s\t%-10s\t%-10s\t%-10s\t%-10s\n", "Layer", "Thickness", "Clay", "Sand", "Organic", "Buld Dens", "Field Cpct", "Wilt Pnt", "NO3", "CH4");
    for (i = 0; i < Soil.totalLayers; i++)
        printf ("*%-10d\t%-10.2lf\t%-10.2lf\t%-10.2lf\t%-10.2lf\t%-10.2lf\t%-10.2lf\t%-10.2lf\t%-10.2lf\t%-10.2lf\n", i + 1, Soil.layerThickness[i], Soil.Clay[i], Soil.Sand[i], Soil.IOM[i], Soil.BD[i], Soil.FC[i], Soil.PWP[i], Soil.NO3[i], Soil.NH4[i]);
    printf ("\n");
    printf ("(Press any key to continue ...)\n");
    getchar();
}

void PrintCrop (describedCropsStruct * describedCrops, int NumCrop)
{
    int             i;

    printf ("\n\n*Crop description file contains descriptions of %d crop types.\n\n", NumCrop);
    printf ("(Press any key to continue ...)\n");
    getchar();

    for (i = 0; i < NumCrop; i++)
    {
        printf ("*%-38s\t%-10s\n", "NAME", describedCrops[i].userCropName);
        printf ("*%-38s\t%-3d\n", "AVERAGE SEEDING DATE", describedCrops[i].userSeedingDate);
        printf ("*%-38s\t%-3d\n", "AVERAGE 50% FLOWERING DATE", describedCrops[i].userFloweringDate);
        printf ("*%-38s\t%-3d\n", "AVERAGE MATURITY DATE", describedCrops[i].userMaturityDate);
        printf ("*%-38s\t%-9.2lf\n", "MAXIMUM_SOIL_COVERAGE", describedCrops[i].userMaximumSoilCoverage);
        printf ("*%-38s\t%-9.2lf\n", "MAXIMUM_ROOTING_DEPTH", describedCrops[i].userMaximumRootingDepth);
        printf ("*%-38s\t%-9.2lf\n", "AVERAGE_EXPECTED_YIELD", describedCrops[i].userExpectedYieldAvg);
        printf ("*%-38s\t%-9.2lf\n", "MAXIMUM_EXPECTED_YIELD", describedCrops[i].userExpectedYieldMax);
        printf ("*%-38s\t%-9.2lf\n", "MINIMUM_EXPECTED_YIELD", describedCrops[i].userExpectedYieldMin);
        printf ("*%-38s\t%-9.2lf\n", "COMMERCIAL_YIELD_MOISTURE", describedCrops[i].userPercentMoistureInYield);
        printf ("*%-38s\t%-9.2lf\n", "STANDING_RESIDUE_AT_HARVEST", describedCrops[i].userFractionResidueStanding);
        printf ("*%-38s\t%-9.2lf\n", "RESIDUE_REMOVED", describedCrops[i].userFractionResidueRemoved);
        printf ("*%-38s\t%-9.2lf\n", "HARVEST_TIMING", describedCrops[i].userClippingTiming);
        printf ("*%-38s\t%-9.2lf\n", "MIN_TEMPERATURE_FOR_TRANSPIRATION", describedCrops[i].userTranspirationMinTemperature);
        printf ("*%-38s\t%-9.2lf\n", "THRESHOLD_TEMPERATURE_FOR_TRANPIRATION", describedCrops[i].userTranspirationThresholdTemperature);
        printf ("*%-38s\t%-9.2lf\n", "MIN_TEMPERATURE_FOR_COLD_DAMAGE", describedCrops[i].userColdDamageMinTemperature);
        printf ("*%-38s\t%-9.2lf\n", "THRESHOLD_TEMPERATURE_FOR_COLD_DAMAGE", describedCrops[i].userColdDamageThresholdTemperature);
        printf ("*%-38s\t%-9.2lf\n", "BASE_TEMPERATURE_FOR_DEVELOPMENT", describedCrops[i].userTemperatureBase);
        printf ("*%-38s\t%-9.2lf\n", "OPTIMUM_TEMPERATURE_FOR_DEVELOPEMENT", describedCrops[i].userTemperatureOptimum);
        printf ("*%-38s\t%-9.2lf\n", "MAX_TEMPERATURE_FOR_DEVELOPMENT", describedCrops[i].userTemperatureMaximum);
        printf ("*%-38s\t%-9.2lf\n", "INITIAL_PARTITIONING_TO_SHOOT", describedCrops[i].userShootPartitionInitial);
        printf ("*%-38s\t%-9.2lf\n", "FINAL_PARTITIONING_TO_SHOOT", describedCrops[i].userShootPartitionFinal);
        printf ("*%-38s\t%-9.2lf\n", "RAIDATION_USE_EFFICIENCY", describedCrops[i].userRadiationUseEfficiency);
        printf ("*%-38s\t%-9.2lf\n", "TRANSPIRATION_USE_EFFICIENCY", describedCrops[i].userTranspirationUseEfficiency);
        printf ("*%-38s\t%-9.2lf\n", "MAXIMUM_HARVEST_INDEX", describedCrops[i].userHIx);
        printf ("*%-38s\t%-9.2lf\n", "MINIMUM_HARVEST_INDEX", describedCrops[i].userHIo);
        printf ("*%-38s\t%-9.2lf\n", "HARVEST_INDEX", describedCrops[i].userHIk);
        printf ("*%-38s\t%-9.2lf\n", "THERMAL_TIME_TO_EMERGENCE", describedCrops[i].userEmergenceTT);
        printf ("*%-38s\t%-9.2lf\n", "N_MAX_CONCENTRATION", describedCrops[i].userNMaxConcentration);
        printf ("*%-38s\t%-9.2lf\n", "N_DILUTION_SLOPE", describedCrops[i].userNDilutionSlope);
        printf ("*%-38s\t%-9.2lf\n", "KC", describedCrops[i].userKc);
        printf ("*%-38s\t%-3d\n", "ANNUAL", describedCrops[i].userAnnual);
        printf ("*%-38s\t%-3d\n", "LEGUME", describedCrops[i].userLegume);
        printf ("*%-38s\t%-3d\n\n", "C3", describedCrops[i].userC3orC4);
        printf ("(Press any key to continue ...)\n");
        getchar();
    }
    printf ("\n");
}

void PrintOperation (plantingOrderStruct * plantedCrops, int NumPlanting, FieldOperationListStruct TillageList, FieldOperationListStruct FixedIrrigationList, FieldOperationListStruct FixedFertilizationList)
{
    int             i;
    FieldOperationStruct *p;

    printf ("\n*Planting:\n");
    for (i = 0; i < NumPlanting; i++)
    {
        printf ("*%-18s\t%-3d\n", "YEAR", plantedCrops[i].seedingYear);
        printf ("*%-18s\t%-3d\n", "DOY", plantedCrops[i].seedingDate);
        printf ("*%-18s\t%-10s\n", "CROP", plantedCrops[i].cropName);
        printf ("*%-18s\t%-3d\n", "AUTO IRRIGATION", plantedCrops[i].usesAutoIrrigation);
        printf ("*%-18s\t%-3d\n", "AUTO FERTILIAZTION", plantedCrops[i].usesAutoFertilization);
        printf ("*%-18s\t%-3d\n", "PLANT ID", plantedCrops[i].plantID);
    }
    printf ("\n");
    printf ("(Press any key to continue ...)\n");
    getchar();

    printf ("*Tillage:\n");
    p = TillageList.firstOperation;
    while (p != NULL)
    {
        printf ("*%-18s\t%-3d\n", "YEAR", p->opYear);
        printf ("*%-18s\t%-3d\n", "DOY", p->opDay);
        printf ("*%-18s\t%-128s\n", "TOOL", p->opToolName);
        printf ("*%-18s\t%-4.2lf\n", "DEPTH", p->opDepth);
        printf ("*%-18s\t%-4.2lf\n", "SOIL DISTURB RATIO", p->opSDR);
        printf ("*%-18s\t%-4.2lf\n", "MIXING EFFICIENCY", p->opMixingEfficiency);
        printf ("\n");
        p = p->NextOperation;
    }
    printf ("\n");
    printf ("(Press any key to continue ...)\n");
    getchar();

    printf ("*Fixed Irrigation:\n");
    p = FixedIrrigationList.firstOperation;
    while (p != NULL)
    {
        printf ("*%-18s\t%-3d\n", "YEAR", p->opYear);
        printf ("*%-18s\t%-3d\n", "DOY", p->opDay);
        printf ("*%-18s\t%-4.2lf\n", "VOLUME", p->opVolume);
        printf ("\n");
        p = p->NextOperation;
    }
    printf ("\n");
    printf ("(Press any key to continue ...)\n");
    getchar();

    printf ("*Fixed Fertilization:\n");
    p = FixedFertilizationList.firstOperation;
    while (p != NULL)
    {
        printf ("*%-18s\t%-3d\n", "YEAR", p->opYear);
        printf ("*%-18s\t%-3d\n", "DOY", p->opDay);
        printf ("*%-18s\t%-128s\n", "SOURCE", p->opSource);
        printf ("*%-18s\t%-6.2lf\n", "MASS", p->opMass);
        printf ("*%-18s\t%-128s\n", "FORM", p->opForm);
        printf ("*%-18s\t%-128s\n", "METHOD", p->opMethod);
        printf ("*%-18s\t%-3d\n", "DEPTH", p->opLayer);
        printf ("*%-18s\t%-6.2lf\n", "C_ORGANIC", p->opC_Organic);
        printf ("*%-18s\t%-6.2lf\n", "C_CHARCOAL", p->opC_Charcoal);
        printf ("*%-18s\t%-6.2lf\n", "N_ORGANIC", p->opN_Organic);
        printf ("*%-18s\t%-6.2lf\n", "N_CHARCOAL", p->opN_Charcoal);
        printf ("*%-18s\t%-6.2lf\n", "N_NH4", p->opN_NH4);
        printf ("*%-18s\t%-6.2lf\n", "N_NO3", p->opN_NO3);
        printf ("*%-18s\t%-6.2lf\n", "P_ORGANIC", p->opP_Organic);
        printf ("*%-18s\t%-6.2lf\n", "P_CHARCOOAL", p->opP_Charcoal);
        printf ("*%-18s\t%-6.2lf\n", "P_INORGANIC", p->opP_Inorganic);
        printf ("*%-18s\t%-6.2lf\n", "K", p->opK);
        printf ("*%-18s\t%-6.2lf\n", "S", p->opS);
        printf ("\n");
        p = p->NextOperation;
    }
    printf ("\n");
    printf ("(Press any key to continue ...)\n");
    getchar ();
}

void PrintWeather (WeatherStruct Weather)
{
    printf ("\n*Weather:\n");
    printf ("*%-16s\t%-6.2lf\n", "LATITUDE", Weather.siteLatitude);
    printf ("*%-16s\t%-6.2lf\n", "ALTITUDE", Weather.siteAltitude);
    printf ("*%-16s\t%-6.2lf\n", "SCREENING_HEIGHT", Weather.screeningHeight);
    printf ("*Weather record %d\n", Weather.length);
    printf ("\n");
    printf ("(Press any key to continue ...)\n");
    getchar ();
}
