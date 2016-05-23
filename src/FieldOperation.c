#ifdef _PIHM_
#include "pihm.h"
#else
#include "Cycles.h"
#endif

void FieldOperation (int rotationYear, int y, int doy,
    cropmgmt_struct *CropManagement, comm_struct *Community,
    soil_struct *Soil, residue_struct *Residue, ctrl_struct *SimControl,
    soilc_struct *SoilCarbon, weather_struct *Weather)
{
    op_struct      *plantingOrder;
    op_struct      *FixedFertilization;
    op_struct      *Tillage;
    op_struct      *FixedIrrigation;
    crop_struct    *Crop;
    double          irrigation_vol;
    int             i;
    int             operation_index;
    int             kill_all = 0;

    /*
     * Perform planting if necessary
     */
    while (IsOperationToday (CropManagement->rotationYear, doy,
            CropManagement->plantingOrder,
            CropManagement->totalCropsPerRotation, &operation_index))
    {
        plantingOrder = &CropManagement->plantingOrder[operation_index];
        PlantingCrop (Community, CropManagement, operation_index);
        if (verbose_mode)
            printf ("DOY %3.3d %-20s %s\n", doy, "Planting",
                plantingOrder->cropName);
    }
    UpdateOperationStatus (CropManagement->plantingOrder,
        CropManagement->totalCropsPerRotation);

    /*
     * Perform fertilization if necessary
     */
    while (IsOperationToday (CropManagement->rotationYear, doy,
            CropManagement->FixedFertilization,
            CropManagement->numFertilization, &operation_index))
    {
        FixedFertilization =
            &CropManagement->FixedFertilization[operation_index];
        if (verbose_mode)
            printf ("DOY %3.3d %-20s %s\n", doy, "Fixed Fertilization",
                FixedFertilization->opSource);

        ApplyFertilizer (FixedFertilization, Soil, Residue);
    }
    UpdateOperationStatus (CropManagement->FixedFertilization,
        CropManagement->numFertilization);

    /*
     * Perform tillage/harvest if necessary
     */
    while (IsOperationToday (CropManagement->rotationYear, doy,
            CropManagement->Tillage, CropManagement->numTillage,
            &operation_index))
    {
        Tillage = &(CropManagement->Tillage[operation_index]);
        if (verbose_mode)
            printf ("DOY %3.3d %-20s %s\n", doy, "Tillage",
                Tillage->opToolName);

        if (strcasecmp (Tillage->opToolName, "Kill_Crop") == 0 ||
            Tillage->grainHarvest || Tillage->forageHarvest)
        {
            if (strcasecmp (Tillage->cropNameT, "N/A") == 0 ||
                strcasecmp (Tillage->cropNameT, "All") == 0)
            {
                kill_all = 1;
            }

            for (i = 0; i < Community->NumCrop; i++)
            {
                if (Community->Crop[i].stageGrowth > NO_CROP)
                {
                    if (kill_all ||
                        strcasecmp (Tillage->cropNameT,
                            Community->Crop[i].cropName) == 0)
                    {
                        if (strcasecmp (Tillage->opToolName,
                                "Kill_Crop") == 0)
                        {
#ifdef _PIHM_
                            HarvestCrop (y, doy, &Community->Crop[i], Residue,
                                Soil, SoilCarbon);
#else
                            HarvestCrop (y, doy, SimControl->simStartYear,
                                &Community->Crop[i], Residue, Soil,
                                SoilCarbon, Weather);
#endif
                            Community->NumActiveCrop--;
                        }
                        else
                        {
                            if (Tillage->grainHarvest)
                            {
#ifdef _PIHM_
                                GrainHarvest (y, doy, &Community->Crop[i],
                                    Residue, Soil, SoilCarbon);
#else
                                GrainHarvest (y, doy,
                                    SimControl->simStartYear,
                                    &Community->Crop[i], Residue, Soil,
                                    SoilCarbon, Weather);
#endif
                            }

                            if (Tillage->forageHarvest)
                            {
                                //if (Community->Crop[i].svShoot >= Community->Crop[i].userClippingBiomassThresholdLower * (1.0 - exp (-Community->Crop[i].userPlantingDensity)))
                                {
#ifdef _PIHM_
                                    ForageAndSeedHarvest (y, doy,
                                        &Community->Crop[i], Residue, Soil,
                                        SoilCarbon);
#else
                                    ForageAndSeedHarvest (y, doy,
                                        SimControl->simStartYear,
                                        &Community->Crop[i], Residue, Soil,
                                        SoilCarbon, Weather);
#endif
                                }
                            }
                        }
                    }
                }

                UpdateCommunity (Community);
            }
        }
        else
        {
            ExecuteTillage (SoilCarbon->abgdBiomassInput, Tillage,
                CropManagement->tillageFactor, Soil, Residue);
        }
    }
    UpdateOperationStatus (CropManagement->Tillage,
        CropManagement->numTillage);

    UpdateCommunity (Community);

    /*
     * Perform irrigation if neccesary
     */
    Soil->irrigationVol = 0.0;
    while (IsOperationToday (CropManagement->rotationYear, doy,
            CropManagement->FixedIrrigation, CropManagement->numIrrigation,
            &operation_index))
    {
        FixedIrrigation = &(CropManagement->FixedIrrigation[operation_index]);
        if (verbose_mode)
            printf ("DOY %3.3d %-20s %lf\n", doy, "Irrigation",
                FixedIrrigation->opVolume);

        Soil->irrigationVol += FixedIrrigation->opVolume;
    }
    UpdateOperationStatus (CropManagement->FixedIrrigation,
        CropManagement->numIrrigation);


    /*
     * Auto irrigation
     */
    for (i = 0; i < Community->NumCrop; i++)
    {
        Crop = &Community->Crop[i];
        if (Crop->autoIrrigationUsed)
        {
            if (Crop->autoIrrigationStartDay < Crop->autoIrrigationStopDay)
            {
                if (doy >= Crop->autoIrrigationStartDay &&
                    doy <= Crop->autoIrrigationStopDay)
                {
                    irrigation_vol =
                        FindIrrigationVolume (Crop->
                        autoIrrigationLastSoilLayer,
                        Crop->autoIrrigationWaterDepletion, Soil);
                    if (irrigation_vol > 0.0)
                    {
                        if (verbose_mode)
                            printf ("DOY %3.3d %-20s %lf\n", doy,
                                "Auto Irrigation", irrigation_vol);
                    }
                    Soil->irrigationVol += irrigation_vol;
                }
            }
            else
            {
                if (doy >= Crop->autoIrrigationStartDay ||
                    doy <= Crop->autoIrrigationStopDay)
                {
                    irrigation_vol =
                        FindIrrigationVolume (Crop->
                        autoIrrigationLastSoilLayer,
                        Crop->autoIrrigationWaterDepletion, Soil);
                    if (irrigation_vol > 0.0)
                    {
                        if (verbose_mode)
                            printf ("DOY %3.3d %-20s %lf\n", doy,
                                "Auto Irrigation", irrigation_vol);
                    }
                    Soil->irrigationVol += irrigation_vol;
                }
            }
        }
    }
}

int IsOperationToday (int rotationYear, int doy, op_struct *FieldOperation,
    int numOperation, int *operationIndex)
{
    /*
     * Returns a true or false indicating if an operation happens on that day
     * -----------------------------------------------------------------------
     * LOCAL VARIABLES
     *
     * Variable             Type        Description
     * ==========           ==========  ====================
     * operation_today      int         [return value]
     */
    int             operation_today;
    int             i;

    operation_today = 0;
    *operationIndex = -1;

    if (numOperation <= 0)
    {
        operation_today = 0;
        *operationIndex = -1;
    }
    else
    {
        for (i = 0; i < numOperation; i++)
        {
            if (FieldOperation[i].status == 1)
                continue;
            else
            {
                if (rotationYear == FieldOperation[i].opYear &&
                    doy == FieldOperation[i].opDay)
                {
                    operation_today = 1;
                    FieldOperation[i].status = 1;
                    *operationIndex = i;

                    break;
                }
                else
                    break;
            }
        }
    }

    return (operation_today);
}

void UpdateOperationStatus (op_struct *FieldOperation, int numOperation)
{
    int             i;
    int             all_performed = 1;

    if (numOperation <= 0)
    {
        /* Do nothing */
    }
    else
    {
        for (i = 0; i < numOperation; i++)
        {
            if (FieldOperation[i].status == 0)
            {
                all_performed = 0;
                break;
            }
        }

        if (all_performed)
        {
            for (i = 0; i < numOperation; i++)
                FieldOperation[i].status = 0;
        }
    }
}
