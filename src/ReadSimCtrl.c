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
    fscanf (simctrl_file, "%*s %d", &SimControl->Sim_Start_Year);
    fscanf (simctrl_file, "%*s %d", &SimControl->Sim_End_Year);
    fscanf (simctrl_file, "%*s %d", &SimControl->Years_in_Rotation);
    fscanf (simctrl_file, "%*s %d", &SimControl->Adjusted_Yields);
    fscanf (simctrl_file, "%*s %d", &SimControl->Hourly_Infiltration);
    fscanf (simctrl_file, "%*s %d", &SimControl->Automatic_Nitrogen);
    fscanf (simctrl_file, "%*s %d", &SimControl->Automatic_Phosphorus);
    fscanf (simctrl_file, "%*s %d", &SimControl->Automatic_Sulfur);
    fscanf (simctrl_file, "%*s %d", &SimControl->Weather_Daily_Output);
    fscanf (simctrl_file, "%*s %d", &SimControl->Crop_Daily_Output);
    fscanf (simctrl_file, "%*s %d", &SimControl->Residue_Daily_Output);
    fscanf (simctrl_file, "%*s %d", &SimControl->Water_Daily_Output);
    fscanf (simctrl_file, "%*s %d", &SimControl->Nitrogen_Daily_Output);
    fscanf (simctrl_file, "%*s %d", &SimControl->SoilCarbon_Daily_Output);
    fscanf (simctrl_file, "%*s %d", &SimControl->Soil_Daily_Output);
    fscanf (simctrl_file, "%*s %d", &SimControl->Annual_Soil_Output);
    fscanf (simctrl_file, "%*s %d", &SimControl->Profile_Output);
    fscanf (simctrl_file, "%*s %d", &SimControl->Season_Output);

    fclose (simctrl_file);

    return 0;
}
