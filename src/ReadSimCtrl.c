#include "Cycles.h"

void ReadSimControl (char *filename, ctrl_struct *SimControl, cropmgmt_struct *CropManagement)
{
    /*
     * Read simulation control file
     * -----------------------------------------------------------------------
     * LOCAL VARIABLES
     *
     * Variable             Type        Description
     * ==========           ==========  ====================
     * simctrl_file	    FILE*	File pointer of simulation control
     *					  file
     * filename		    char*	Simulation control file name
     */
    FILE           *simctrl_file;
    char            cmdstr[MAXSTRING];

    /* Open simulation control file */
    simctrl_file = fopen (filename, "r");
    Cycles_printf (VL_NORMAL, "%-30s %s.\n", "Read simulation control file:", filename);

    if (simctrl_file == NULL)
    {
        Cycles_printf (VL_ERROR, "ERROR: Cannot find the simulation control file %s!\n", filename);
        Cycles_exit (EXIT_FAILURE);
    }

    /* Read simulation control file */
    FindLine (simctrl_file, "BOF");

    NextLine (simctrl_file, cmdstr);
    ReadKeywordInt (cmdstr, "SIMULATION_START_YEAR", &SimControl->simStartYear);

    NextLine (simctrl_file, cmdstr);
    ReadKeywordInt (cmdstr, "SIMULATION_END_YEAR", &SimControl->simEndYear);

    NextLine (simctrl_file, cmdstr);
    ReadKeywordInt (cmdstr, "ROTATION_SIZE", &CropManagement->yearsInRotation);

    NextLine (simctrl_file, cmdstr);
    ReadKeywordInt (cmdstr, "ADJUSTED_YIELDS", &CropManagement->adjustedYields);

    NextLine (simctrl_file, cmdstr);
    ReadKeywordInt (cmdstr, "HOURLY_INFILTRATION", &SimControl->hourlyInfiltration);

    NextLine (simctrl_file, cmdstr);
    ReadKeywordInt (cmdstr, "AUTOMATIC_NITROGEN", &CropManagement->automaticNitrogen);

    NextLine (simctrl_file, cmdstr);
    ReadKeywordInt (cmdstr, "AUTOMATIC_PHOSPHORUS", &CropManagement->automaticPhosphorus);

    NextLine (simctrl_file, cmdstr);
    ReadKeywordInt (cmdstr, "AUTOMATIC_SULFUR", &CropManagement->automaticSulfur);

    NextLine (simctrl_file, cmdstr);
    ReadKeywordInt (cmdstr, "DAILY_WEATHER_OUT", &SimControl->weatherDailyOutput);

    NextLine (simctrl_file, cmdstr);
    ReadKeywordInt (cmdstr, "DAILY_CROP_OUT", &SimControl->cropDailyOutput);

    NextLine (simctrl_file, cmdstr);
    ReadKeywordInt (cmdstr, "DAILY_RESIDUE_OUT", &SimControl->residueDailyOutput);

    NextLine (simctrl_file, cmdstr);
    ReadKeywordInt (cmdstr, "DAILY_WATER_OUT", &SimControl->waterDailyOutput);

    NextLine (simctrl_file, cmdstr);
    ReadKeywordInt (cmdstr, "DAILY_NITROGEN_OUT", &SimControl->nitrogenDailyOutput);

    NextLine (simctrl_file, cmdstr);
    ReadKeywordInt (cmdstr, "DAILY_SOIL_CARBON_OUT", &SimControl->soilCarbonDailyOutput);

    NextLine (simctrl_file, cmdstr);
    ReadKeywordInt (cmdstr, "DAILY_SOIL_LYR_CN_OUT", &SimControl->soilDailyOutput);

    NextLine (simctrl_file, cmdstr);
    ReadKeywordInt (cmdstr, "ANNUAL_SOIL_OUT", &SimControl->annualSoilOutput);

    NextLine (simctrl_file, cmdstr);
    ReadKeywordInt (cmdstr, "ANNUAL_PROFILE_OUT", &SimControl->profileOutput);

    NextLine (simctrl_file, cmdstr);
    ReadKeywordInt (cmdstr, "SEASON_OUT", &SimControl->seasonOutput);

    NextLine (simctrl_file, cmdstr);
    ReadKeywordStr (cmdstr, "CROP_FILE", SimControl->crop_filename);

    NextLine (simctrl_file, cmdstr);
    ReadKeywordStr (cmdstr, "OPERATION_FILE", SimControl->operation_filename);

    NextLine (simctrl_file, cmdstr);
    ReadKeywordStr (cmdstr, "SOIL_FILE", SimControl->soil_filename);

    NextLine (simctrl_file, cmdstr);
    ReadKeywordStr (cmdstr, "WEATHER_FILE", SimControl->weather_filename);

    fclose (simctrl_file);

    SimControl->totalYears = SimControl->simEndYear - SimControl->simStartYear + 1;

    if (debug_mode)
    {
        PrintSimContrl (*SimControl);
    }
}
