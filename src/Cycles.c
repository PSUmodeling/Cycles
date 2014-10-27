#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#include "include/Cycles.h"

int main (int argc, char *argv[])
{
    CyclesStruct    Cycles;
#ifdef _DEBUG_
    int             crop_counter;
    int             i;
    FieldOperationClass *p;
#endif

    char           *project;

    Cycles = (CyclesStruct) malloc (sizeof *Cycles);

    if (argc < 2)
    {
        printf ("Error: Please specify the name of project!\n");
        exit (0);
    }
    else
    {
        /*
         * Get user specified project in command line 
         */
        project = (char *)malloc ((strlen (argv[1]) + 1) * sizeof (char));
        strcpy (project, argv[1]);
    }

    system ("clear");
    printf ("\n\n");
    printf ("\t\t ######  ##    ##  ######  ##       ########  ######\n");
    printf ("\t\t##    ##  ##  ##  ##    ## ##       ##       ##    ##\n");
    printf ("\t\t##         ####   ##       ##       ##       ##\n");
    printf ("\t\t##          ##    ##       ##       ######    ######\n");
    printf ("\t\t##          ##    ##       ##       ##             ##\n");
    printf ("\t\t##    ##    ##    ##    ## ##       ##       ##    ##\n");
    printf ("\t\t ######     ##     ######  ######## ########  ######\n\n\n");
    printf ("\t\t Copyright(c)2010-2015 PSU / WSU All rights reserved\n\n\n");

    ReadSimControl (project, Cycles);
#ifdef _DEBUG_
    printf ("%d %d\n", Cycles->SimControl.Sim_Start_Year, Cycles->SimControl.Season_Output);
#endif
    ReadSoil (project, Cycles);
#ifdef _DEBUG_
    printf ("%lf\n", Cycles->Soil.layerThickness[Cycles->Soil.totalLayers - 1]);
#endif
    ReadCrop (project, Cycles);
#ifdef _DEBUG_
    for (crop_counter = 0; crop_counter < 3; crop_counter++)
    {
        printf ("%s\t", Cycles->Crop[crop_counter].cropName);
        printf ("%d\t", Cycles->Crop[crop_counter].userSeedingDate);
        printf ("%d\n", Cycles->Crop[crop_counter].userC3orC4);
    }
#endif
    ReadOperation (project, Cycles);
#ifdef _DEBUG_
    printf ("Number of field operations: %d\n", Cycles->FieldOperation.n);
    printf ("%d %d\n", Cycles->FieldOperation.FirstOperation->opType, Cycles->FieldOperation.FirstOperation->opDay);
    p = Cycles->FieldOperation.FirstOperation->NextOperation;
    for (i = 0; i < Cycles->FieldOperation.n; i++)
    {
        printf ("%d %d\n", p->opType, p->opDay);
        p = p->NextOperation;
    }
#endif
    ReadWeather (project, Cycles);

    return 0;
}
