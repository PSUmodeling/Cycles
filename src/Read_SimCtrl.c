#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#include "include/Cycles.h"

int ReadSimControl (char *project, SimControlClass *SimControl)
{
    FILE    *simctrl_file;
    char    *filename;

    printf("Read simulation control file.\n");

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
    printf("%d %d %d %d %d  %d  %d  %d  %d  %d  %d  %d  %d  %d  %d  %d  %d  %d\n", SimControl->Sim_Start_Year, SimControl->Sim_End_Year, SimControl->Years_in_Rotation, SimControl->Adjusted_Yields, SimControl->Hourly_Infiltration, SimControl->Automatic_Nitrogen, SimControl->Automatic_Phosphorus, SimControl->Automatic_Sulfur, SimControl->Weather_Daily_Output, SimControl->Crop_Daily_Output, SimControl->Residue_Daily_Output, SimControl->Water_Daily_Output, SimControl->Nitrogen_Daily_Output,  SimControl->SoilCarbon_Daily_Output, SimControl->Soil_Daily_Output, SimControl->Annual_Soil_Output, SimControl->Profile_Output, SimControl->Season_Output);  

    fclose(simctrl_file);
     
    return 0;
}
