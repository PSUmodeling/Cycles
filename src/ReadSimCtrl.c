#include "Cycles.h"

void ReadSimControl (char *project, SimControlStruct *SimControl)
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
    char           *filename;

    /* Open simulation control file */
    filename = (char *)malloc ((strlen (project) + 12) * sizeof (char));
    sprintf (filename, "input/%s.ctrl", project);
    simctrl_file = fopen (filename, "r");
    printf ("Read simulation control file: %s.\n", filename);
    free (filename);

    if (simctrl_file == NULL)
    {
        printf ("ERROR: Cannot find the simulation control file %s!\n", filename);
        exit (1);
    }

    /* Read simulation control file */
    fscanf (simctrl_file, "%*s %d", &SimControl->simStartYear);
    fscanf (simctrl_file, "%*s %d", &SimControl->simEndYear);
    fscanf (simctrl_file, "%*s %d", &SimControl->yearsInRotation);
    fscanf (simctrl_file, "%*s %d", &SimControl->adjustedYields);
    fscanf (simctrl_file, "%*s %d", &SimControl->hourlyInfiltration);
    fscanf (simctrl_file, "%*s %d", &SimControl->automaticNitrogen);
    fscanf (simctrl_file, "%*s %d", &SimControl->automaticPhosphorus);
    fscanf (simctrl_file, "%*s %d", &SimControl->automaticSulfur);
    fscanf (simctrl_file, "%*s %d", &SimControl->weatherDailyOutput);
    fscanf (simctrl_file, "%*s %d", &SimControl->cropDailyOutput);
    fscanf (simctrl_file, "%*s %d", &SimControl->residueDailyOutput);
    fscanf (simctrl_file, "%*s %d", &SimControl->waterDailyOutput);
    fscanf (simctrl_file, "%*s %d", &SimControl->nitrogenDailyOutput);
    fscanf (simctrl_file, "%*s %d", &SimControl->soilCarbonDailyOutput);
    fscanf (simctrl_file, "%*s %d", &SimControl->soilDailyOutput);
    fscanf (simctrl_file, "%*s %d", &SimControl->annualSoilOutput);
    fscanf (simctrl_file, "%*s %d", &SimControl->profileOutput);
    fscanf (simctrl_file, "%*s %d", &SimControl->seasonOutput);
    fscanf (simctrl_file, "%*s %s", SimControl->crop_filename);
    fscanf (simctrl_file, "%*s %s", SimControl->operation_filename);
    fscanf (simctrl_file, "%*s %s", SimControl->soil_filename);
    fscanf (simctrl_file, "%*s %s", SimControl->weather_filename);

    fclose (simctrl_file);

    SimControl->totalYears = SimControl->simEndYear - SimControl->simStartYear + 1;
}
