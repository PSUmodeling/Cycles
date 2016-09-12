#include "Cycles.h"

void ReadOperation (char *filename, cropmgmt_struct *CropManagement, const comm_struct *Community, int yearsInRotation)
{
    /*
     * Read field operation description file
     * -----------------------------------------------------------------------
     * LOCAL VARIABLES
     *
     * Variable             Type        Description
     * ==========           ==========  ====================
     * operation_file	    FILE*	File pointer of fiedl operation file
     * fullname		    char*	Full file name of the field operation
     *					  file
     * cmdstr		    char[MAXSTRING]
     *					Command string
     * optstr		    char[MAXSTRING]
     *					Option argument string
     * tillage_counter	    int		Tillage operation counter
     * planting_counter	    int		Planting operatin counter
     * irrigation_counter   int		Fixed irrigation operation counter
     * fertilization_counter
     *			    int		Fixed fertilization operation counter
     * auto_irrigation_counter
     *			    int		Automatic irrigation operation counter
     * harvest_counter	    int		Forced harvest operation counter
     * tempyear		    int
     * i		    int		Loop counter
     * j		    int		Loop counter
     */
    FILE           *operation_file;
    char           *fullname;
    char            cmdstr[MAXSTRING];
    int             tillage_counter = 0;
    int             planting_counter = 0;
    int             irrigation_counter = 0;
    int             fertilization_counter = 0;
    int             auto_irrigation_counter = 0;
    int             i, j;
    plant_struct   *planting;
    tillage_struct *tillage;
    fixirr_struct  *fixirr;
    fixfert_struct *fixfert;

    /* Open field operation file */
    fullname = (char *)malloc ((strlen (filename) + 7) * sizeof (char));
    sprintf (fullname, "input/%s", filename);
    operation_file = fopen (fullname, "r");

    if (operation_file == NULL)
    {
        Cycles_printf (VL_ERROR, "ERROR: Cannot find the field operation file %s!\n", filename);
        Cycles_exit (EXIT_FAILURE);
    }
    else
        Cycles_printf (VL_NORMAL, "%-30s input/%s.\n", "Read field operation file:", filename);

    free (fullname);

    /* Read field operation file and count numbers of operations */
    FindLine (operation_file, "BOF");
    planting_counter = CountOccurance (operation_file, "PLANTING");

    FindLine (operation_file, "BOF");
    tillage_counter = CountOccurance (operation_file, "TILLAGE");
    
    FindLine (operation_file, "BOF");
    irrigation_counter = CountOccurance (operation_file, "FIXED_IRRIGATION");

    FindLine (operation_file, "BOF");
    fertilization_counter = CountOccurance (operation_file, "FIXED_FERTILIZATION");

    FindLine (operation_file, "BOF");
    auto_irrigation_counter = CountOccurance (operation_file, "AUTO_IRRIGATION");

    /* Allocate memories for field operation classes */
    CropManagement->totalCropsPerRotation = planting_counter;
    CropManagement->plantingOrder = (plant_struct *)malloc (planting_counter * sizeof (plant_struct));

    CropManagement->numFertilization = fertilization_counter;
    CropManagement->FixedFertilization = (fixfert_struct *)malloc (fertilization_counter * sizeof (fixfert_struct));

    CropManagement->numIrrigation = irrigation_counter;
    CropManagement->FixedIrrigation = (fixirr_struct *)malloc (irrigation_counter * sizeof (fixirr_struct));

    CropManagement->numTillage = tillage_counter;
    CropManagement->Tillage = (tillage_struct *)malloc (tillage_counter * sizeof (tillage_struct));

    CropManagement->numAutoIrrigation = auto_irrigation_counter;
    CropManagement->autoIrrigation = (autoirr_struct *)malloc (auto_irrigation_counter * sizeof (autoirr_struct));
    CropManagement->usingAutoIrr = 0;

    if (planting_counter)
    {
        /* Rewind to the beginning of file and read all planting operations */
        FindLine (operation_file, "BOF");

        for (i = 0; i < planting_counter; i++)
        {
            planting = &(CropManagement->plantingOrder[i]);

            FindLine (operation_file, "PLANTING");

            NextLine (operation_file, cmdstr);
            ReadKeywordInt (cmdstr, "YEAR", &planting->opYear);
            if (planting->opYear > yearsInRotation)
            {
                Cycles_printf (VL_ERROR, "ERROR: Operation year is larger than years in rotation!\n");
                Cycles_printf (VL_ERROR, "Please remove this operation and retry.\n");
            }

            NextLine (operation_file, cmdstr);
            ReadKeywordInt (cmdstr, "DOY", &planting->opDay);
            
            NextLine (operation_file, cmdstr);
            ReadKeywordStr (cmdstr, "CROP", planting->cropName);

            NextLine (operation_file, cmdstr);
            ReadKeywordInt (cmdstr, "USE_AUTO_IRR", &planting->usesAutoIrrigation);
            if (CropManagement->plantingOrder[i].usesAutoIrrigation == 0)
                CropManagement->plantingOrder[i].usesAutoIrrigation = -1;
            else
                CropManagement->usingAutoIrr = 1;

            NextLine (operation_file, cmdstr);
            ReadKeywordInt (cmdstr, "USE_AUTO_FERT", &planting->usesAutoFertilization);
            if (CropManagement->plantingOrder[i].usesAutoFertilization == 0)
                CropManagement->plantingOrder[i].usesAutoFertilization = -1;

            NextLine (operation_file, cmdstr);
            ReadKeywordDouble (cmdstr, "FRACTION", &planting->plantingDensity);

            NextLine (operation_file, cmdstr);
            ReadKeywordInt (cmdstr, "CLIPPING_START", &planting->clippingStart);
            if (planting->clippingStart > 366 || planting->clippingStart < 1)
            {
                Cycles_printf (VL_ERROR, "ERROR: Please specify valid DOY for clipping start date!\n");
                Cycles_exit (EXIT_FAILURE);
            }
            
            NextLine (operation_file, cmdstr);
            ReadKeywordInt (cmdstr, "CLIPPING_END", &planting->clippingEnd);
            if (planting->clippingEnd > 366 || planting->clippingEnd < 1)
            {
                Cycles_printf (VL_ERROR, "ERROR: Please specify valid DOY for clipping end date!\n");
                Cycles_exit (EXIT_FAILURE);
            }

            planting->status = 0;

            /* Link planting order and crop description */
            for (j = 0; j < Community->NumCrop; j++)
            {
                if (strcmp (CropManagement->plantingOrder[i].cropName, Community->Crop[j].cropName) == 0)
                {
                    CropManagement->plantingOrder[i].plantID = j;
                    break;
                }
            }
            if (j >= Community->NumCrop)
            {
                Cycles_printf (VL_ERROR, "ERROR: Cannot find the plant description of %s, please check your input file\n", CropManagement->plantingOrder[i].cropName);
                Cycles_exit (EXIT_FAILURE);
            }
        }
    }

    if (tillage_counter)
    {
        /* Rewind to the beginning of file and read all tillage operations */
        FindLine (operation_file, "BOF");

        for (i = 0; i < tillage_counter; i++)
        {
            tillage = &(CropManagement->Tillage[i]);

            FindLine (operation_file, "TILLAGE");

            NextLine (operation_file, cmdstr);
            ReadKeywordInt (cmdstr, "YEAR", &tillage->opYear);
            if (tillage->opYear > yearsInRotation)
            {
                Cycles_printf (VL_ERROR, "ERROR: Operation year is larger than years in rotation!\n");
                Cycles_printf (VL_ERROR, "Please remove this operation and retry.\n");
            }

            NextLine (operation_file, cmdstr);
            ReadKeywordInt (cmdstr, "DOY", &tillage->opDay);

            NextLine (operation_file, cmdstr);
            ReadKeywordStr (cmdstr, "TOOL", tillage->opToolName);

            NextLine (operation_file, cmdstr);
            ReadKeywordDouble (cmdstr, "DEPTH", &tillage->opDepth);

            NextLine (operation_file, cmdstr);
            ReadKeywordDouble (cmdstr, "SOIL_DISTURB_RATIO", &tillage->opSDR);

            NextLine (operation_file, cmdstr);
            ReadKeywordDouble (cmdstr, "MIXING_EFFICIENCY", &tillage->opMixingEfficiency);

            NextLine (operation_file, cmdstr);
            ReadKeywordStr (cmdstr, "CROP_NAME", tillage->cropNameT);

            /* Check if the specified crop exists */
            if (strcasecmp (tillage->cropNameT, "N/A") != 0 &&
                strcasecmp (tillage->cropNameT, "All") != 0 &&
                !CropExist (tillage->cropNameT, Community))
            {
                Cycles_printf (VL_ERROR, "ERROR: Crop name %s not recognized!\n", tillage->cropNameT);
                Cycles_exit (EXIT_FAILURE);
            }

            NextLine (operation_file, cmdstr);
            ReadKeywordDouble (cmdstr, "FRAC_THERMAL_TIME", &tillage->fractionThermalTime);

            NextLine (operation_file, cmdstr);
            ReadKeywordDouble (cmdstr, "KILL_EFFICIENCY", &tillage->killEfficiency);

            NextLine (operation_file, cmdstr);
            ReadKeywordInt (cmdstr, "GRAIN_HARVEST", &tillage->grainHarvest);

            NextLine (operation_file, cmdstr);
            ReadKeywordDouble (cmdstr, "FORAGE_HARVEST", &tillage->forageHarvest);

            tillage->status = 0;
        }
    }

    if (irrigation_counter)
    {
        /* Rewind to the beginning of file and read all irrigation
         * operations */
        FindLine (operation_file, "BOF");

        for (i = 0; i < irrigation_counter; i++)
        {
            fixirr = &(CropManagement->FixedIrrigation[i]);

            FindLine (operation_file, "FIXED_IRRIGATION");

            NextLine (operation_file, cmdstr);
            ReadKeywordInt (cmdstr, "YEAR", &fixirr->opYear);
            if (fixirr->opYear > yearsInRotation)
            {
                Cycles_printf (VL_ERROR, "ERROR: Operation year is larger than years in rotation!\n");
                Cycles_printf (VL_ERROR, "Please remove this operation and retry.\n");
            }
            
            NextLine (operation_file, cmdstr);
            ReadKeywordInt (cmdstr, "DOY", &fixirr->opDay);

            NextLine (operation_file, cmdstr);
            ReadKeywordDouble (cmdstr, "VOLUME", &fixirr->opVolume);

            fixirr->status = 0;
        }
    }

    if (fertilization_counter)
    {
        /* Rewind to the beginning of file and read all fertilization
         * operations */
        FindLine (operation_file, "BOF");

        for (i = 0; i < fertilization_counter; i++)
        {
            fixfert = &(CropManagement->FixedFertilization[i]);

            FindLine (operation_file, "FIXED_FERTILIZATION");

            NextLine (operation_file, cmdstr);
            ReadKeywordInt (cmdstr, "YEAR", &fixfert->opYear);
            if (fixfert->opYear > yearsInRotation)
            {
                Cycles_printf (VL_ERROR, "ERROR: Operation year is larger than years in rotation!\n");
                Cycles_printf (VL_ERROR, "Please remove this operation and retry.\n");
            }

            NextLine (operation_file, cmdstr);
            ReadKeywordInt (cmdstr, "DOY", &fixfert->opDay);

            NextLine (operation_file, cmdstr);
            ReadKeywordStr (cmdstr, "SOURCE", fixfert->opSource);

            NextLine (operation_file, cmdstr);
            ReadKeywordDouble (cmdstr, "MASS", &fixfert->opMass);

            NextLine (operation_file, cmdstr);
            ReadKeywordStr (cmdstr, "FORM", fixfert->opForm);

            NextLine (operation_file, cmdstr);
            ReadKeywordStr (cmdstr, "METHOD", fixfert->opMethod);

            NextLine (operation_file, cmdstr);
            ReadKeywordInt (cmdstr, "LAYER", &fixfert->opLayer);

            NextLine (operation_file, cmdstr);
            ReadKeywordDouble (cmdstr, "C_ORGANIC", &fixfert->opC_Organic);

            NextLine (operation_file, cmdstr);
            ReadKeywordDouble (cmdstr, "C_CHARCOAL", &fixfert->opC_Charcoal);

            NextLine (operation_file, cmdstr);
            ReadKeywordDouble (cmdstr, "N_ORGANIC", &fixfert->opN_Organic);

            NextLine (operation_file, cmdstr);
            ReadKeywordDouble (cmdstr, "N_CHARCOAL", &fixfert->opN_Charcoal);

            NextLine (operation_file, cmdstr);
            ReadKeywordDouble (cmdstr, "N_NH4", &fixfert->opN_NH4);

            NextLine (operation_file, cmdstr);
            ReadKeywordDouble (cmdstr, "N_NO3", &fixfert->opN_NO3);

            NextLine (operation_file, cmdstr);
            ReadKeywordDouble (cmdstr, "P_ORGANIC", &fixfert->opP_Organic);

            NextLine (operation_file, cmdstr);
            ReadKeywordDouble (cmdstr, "P_CHARCOAL", &fixfert->opP_Charcoal);

            NextLine (operation_file, cmdstr);
            ReadKeywordDouble (cmdstr, "P_INORGANIC", &fixfert->opP_Inorganic);

            NextLine (operation_file, cmdstr);
            ReadKeywordDouble (cmdstr, "K", &fixfert->opK);

            NextLine (operation_file, cmdstr);
            ReadKeywordDouble (cmdstr, "S", &fixfert->opS);

            fixfert->status = 0;

            if (fixfert->opC_Organic + fixfert->opC_Charcoal + fixfert->opN_Organic + fixfert->opN_Charcoal + fixfert->opN_NH4 + fixfert->opN_NO3 + fixfert->opP_Organic + fixfert->opP_Charcoal + fixfert->opP_Inorganic + fixfert->opK + fixfert->opS <= 1.0)
            {
                fixfert->opMass /= 1000.0;
            }
            else
            {
                Cycles_printf (VL_ERROR, "ERROR: Added fertilization fractions must be <= 1\n");
                Cycles_exit (EXIT_FAILURE);
            }
        }
    }

    if (CropManagement->usingAutoIrr)
    {
        /* Rewind to the beginning of file and read all planting operations */
        FindLine (operation_file, "BOF");

        for (i = 0; i < auto_irrigation_counter; i++)
        {
            FindLine (operation_file, "AUTO_IRRIGATION");

            NextLine (operation_file, cmdstr);
            ReadKeywordStr (cmdstr, "CROP", CropManagement->autoIrrigation[i].cropName);

            NextLine (operation_file, cmdstr);
            ReadKeywordInt (cmdstr, "START_DAY", &CropManagement->autoIrrigation[i].startDay);

            NextLine (operation_file, cmdstr);
            ReadKeywordInt (cmdstr, "STOP_DAY", &CropManagement->autoIrrigation[i].stopDay);

            NextLine (operation_file, cmdstr);
            ReadKeywordDouble (cmdstr, "WATER_DEPLETION", &CropManagement->autoIrrigation[i].waterDepletion);

            NextLine (operation_file, cmdstr);
            ReadKeywordInt (cmdstr, "LAST_SOIL_LAYER", &CropManagement->autoIrrigation[i].lastSoilLayer);
        }
    }

    /* Link plating order and auto irrigation */
    for (i = 0; i < CropManagement->totalCropsPerRotation; i++)
    {
        if (CropManagement->usingAutoIrr && CropManagement->plantingOrder[i].usesAutoIrrigation == 1)
        {
            for (j = 0; j < auto_irrigation_counter; j++)
            {
                if (strcmp (CropManagement->plantingOrder[i].cropName, CropManagement->autoIrrigation[j].cropName) == 0)
                {
                    CropManagement->plantingOrder[i].usesAutoIrrigation = j;
                    break;
                }
            }
            if (j >= auto_irrigation_counter)
            {
                Cycles_printf (VL_ERROR, "ERROR: Cannot find the description of auto irrigation for %s!\n", CropManagement->plantingOrder[i].cropName);
                Cycles_exit (EXIT_FAILURE);
            }
        }
        else
            CropManagement->plantingOrder[i].usesAutoIrrigation = -1;
    }

    fclose (operation_file);

    if (debug_mode)
    {
        PrintOperation (CropManagement->plantingOrder,
            CropManagement->totalCropsPerRotation,
            CropManagement->Tillage, CropManagement->numTillage,
            CropManagement->FixedIrrigation, CropManagement->numIrrigation,
            CropManagement->FixedFertilization, CropManagement->numFertilization);
    }
}

int CropExist (char *cropName, const comm_struct *Community)
{
    int             i;
    int             exist = 0;


    for (i = 0; i < Community->NumCrop; i++)
    {
        if (strcmp (cropName, Community->Crop[i].cropName) == 0)
        {
            exist = 1;
            break;
        }
    }

    return (exist);
}
