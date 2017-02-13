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
    plant_struct      *plantingOrder;
    fixfert_struct      *FixedFertilization;
    tillage_struct      *Tillage;
    fixirr_struct      *FixedIrrigation;
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
            CropManagement->totalCropsPerRotation, &operation_index, PLANT_OP))
    {
        plantingOrder = &CropManagement->plantingOrder[operation_index];
        PlantingCrop (Community, CropManagement, operation_index);
        Cycles_printf (VL_VERBOSE,
            "DOY %3.3d %-20s %s\n", doy, "Planting", plantingOrder->cropName);
    }
    UpdateOperationStatus (CropManagement->plantingOrder,
        CropManagement->totalCropsPerRotation, PLANT_OP);

    /*
     * Perform fertilization if necessary
     */
    while (IsOperationToday (CropManagement->rotationYear, doy,
            CropManagement->FixedFertilization,
            CropManagement->numFertilization, &operation_index, FIXFERT_OP))
    {
        FixedFertilization =
            &CropManagement->FixedFertilization[operation_index];
        Cycles_printf (VL_VERBOSE,
            "DOY %3.3d %-20s %s\n", doy, "Fixed Fertilization",
            FixedFertilization->opSource);

        ApplyFertilizer (FixedFertilization, Soil, Residue);
    }
    UpdateOperationStatus (CropManagement->FixedFertilization,
        CropManagement->numFertilization, FIXFERT_OP);

    /*
     * Perform tillage/harvest if necessary
     */
    while (IsOperationToday (CropManagement->rotationYear, doy,
            CropManagement->Tillage, CropManagement->numTillage,
            &operation_index, TILLAGE_OP))
    {
        Tillage = &(CropManagement->Tillage[operation_index]);
        Cycles_printf (VL_VERBOSE,
            "DOY %3.3d %-20s %s\n", doy, "Tillage",
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
        CropManagement->numTillage, TILLAGE_OP);

    UpdateCommunity (Community);

    /*
     * Perform irrigation if neccesary
     */
    Soil->irrigationVol = 0.0;
    while (IsOperationToday (CropManagement->rotationYear, doy,
            CropManagement->FixedIrrigation, CropManagement->numIrrigation,
            &operation_index, FIXIRR_OP))
    {
        FixedIrrigation = &(CropManagement->FixedIrrigation[operation_index]);
        Cycles_printf (VL_VERBOSE,
            "DOY %3.3d %-20s %lf\n", doy, "Irrigation",
            FixedIrrigation->opVolume);

        Soil->irrigationVol += FixedIrrigation->opVolume;
    }
    UpdateOperationStatus (CropManagement->FixedIrrigation,
        CropManagement->numIrrigation, FIXIRR_OP);


    /*
     * Auto irrigation
     */
    for (i = 0; i < Community->NumCrop; i++)
    {
        Crop = &Community->Crop[i];
        if (Crop->stageGrowth > NO_CROP)
        {

        if (Crop->autoIrrigationUsed)
        {
            if (Crop->autoIrrigationStartDay < Crop->autoIrrigationStopDay)
            {
                if (doy >= Crop->autoIrrigationStartDay &&
                    doy <= Crop->autoIrrigationStopDay)
                {
                    irrigation_vol =
                        FindIrrigationVolume
                        (Crop->autoIrrigationLastSoilLayer,
                        Crop->autoIrrigationWaterDepletion, Soil);

                    if (irrigation_vol > 0.0)
                    {
                        Cycles_printf (VL_VERBOSE,
                            "DOY %3.3d %-20s %lf\n", doy,
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
                        FindIrrigationVolume
                        (Crop->autoIrrigationLastSoilLayer,
                        Crop->autoIrrigationWaterDepletion, Soil);


                    if (irrigation_vol > 0.0)
                    {
                        Cycles_printf (VL_VERBOSE,
                            "DOY %3.3d %-20s %lf\n", doy,
                            "Auto Irrigation", irrigation_vol);
                    }
                    Soil->irrigationVol += irrigation_vol;
                }
            }
        }
        }
    }
}

int IsOperationToday (int rotationYear, int doy, void *FieldOperation,
    int numOperation, int *operationIndex, int op_type)
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
    int            *status;
    int             opYear;
    int             opDay;

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
            switch (op_type)
            {
                case PLANT_OP:
                    status = &(((plant_struct *)FieldOperation)[i].status);
                    opYear = ((plant_struct *)FieldOperation)[i].opYear;
                    opDay = ((plant_struct *)FieldOperation)[i].opDay;
                    break;
                case TILLAGE_OP:
                    status = &(((tillage_struct *)FieldOperation)[i].status);
                    opYear = ((tillage_struct *)FieldOperation)[i].opYear;
                    opDay = ((tillage_struct *)FieldOperation)[i].opDay;
                    break;
                case FIXIRR_OP:
                    status = &(((fixirr_struct *)FieldOperation)[i].status);
                    opYear = ((fixirr_struct *)FieldOperation)[i].opYear;
                    opDay = ((fixirr_struct *)FieldOperation)[i].opDay;
                    break;
                case FIXFERT_OP:
                    status = &(((fixfert_struct *)FieldOperation)[i].status);
                    opYear = ((fixfert_struct *)FieldOperation)[i].opYear;
                    opDay = ((fixfert_struct *)FieldOperation)[i].opDay;
                    break;
                default:
                    break;
            }

            if (*status == 1)
            {
                continue;
            }
            else
            {
                if (rotationYear == opYear && doy == opDay)
                {
                    operation_today = 1;
                    *status = 1;
                    *operationIndex = i;
                    break;
                }
                else
                {
                    break;
                }
            }
        }
    }

    return (operation_today);
}

void UpdateOperationStatus (void *FieldOperation, int numOperation, int op_type)
{
    int             i;
    int             all_performed = 1;
    int            *status;

    if (numOperation <= 0)
    {
        /* Do nothing */
    }
    else
    {
        for (i = 0; i < numOperation; i++)
        {
            switch (op_type)
            {
                case PLANT_OP:
                    status = &(((plant_struct *)FieldOperation)[i].status);
                    break;
                case TILLAGE_OP:
                    status = &(((tillage_struct *)FieldOperation)[i].status);
                    break;
                case FIXIRR_OP:
                    status = &(((fixirr_struct *)FieldOperation)[i].status);
                    break;
                case FIXFERT_OP:
                    status = &(((fixfert_struct *)FieldOperation)[i].status);
                    break;
                default:
                    break;
            }

            if (0 == *status)
            {
                all_performed = 0;
                break;
            }
        }

        if (all_performed)
        {
            for (i = 0; i < numOperation; i++)
            {
                switch (op_type)
                {
                    case PLANT_OP:
                        status = &(((plant_struct *)FieldOperation)[i].status);
                        break;
                    case TILLAGE_OP:
                        status = &(((tillage_struct *)FieldOperation)[i].status);
                        break;
                    case FIXIRR_OP:
                        status = &(((fixirr_struct *)FieldOperation)[i].status);
                        break;
                    case FIXFERT_OP:
                        status = &(((fixfert_struct *)FieldOperation)[i].status);
                        break;
                    default:
                        break;
                }

                *status = 0;
            }
        }
    }
}
