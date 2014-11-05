#include "include/Cycles.h"

int ReadSimControl (char *project, CyclesStruct Cycles)
{
    FILE           *simctrl_file;
    char           *filename;
    SimControlClass *SimControl;

    SimControl = &Cycles->SimControl;

    printf ("Read simulation control file.\n");

    /* Open simulation control file */
    filename = (char *)malloc ((strlen (project) + 12) * sizeof (char));
    sprintf (filename, "input/%s.ctrl", project);
    simctrl_file = fopen (filename, "r");
    free (filename);

    if (simctrl_file == NULL)
    {
        printf ("\nError: Cannot find the simulation cotrol file %s!\n", filename);
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

    fclose (simctrl_file);

    SimControl->totalYears = SimControl->simEndYear - SimControl->simStartYear + 1;

    return 0;
}
