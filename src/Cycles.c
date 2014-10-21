#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#include "include/Cycles.h"

int main (int argc, char *argv[])
{
    CyclesStruct        Cycles;

    char *project;
    Cycles = (CyclesStruct) malloc (sizeof *Cycles);

    if (argc < 2)
    {
        printf("Error: Please specify the name of project!\n");
        exit (0);
    }
    else
    {
        /* Get user specified project in command line */
        project = (char *)malloc ((strlen (argv[1]) + 1)* sizeof (char));
        strcpy (project, argv[1]);
    }

    system ("clear");
    printf ("\n\n");
    printf ("\t\t ######  ##    ##  ######  ##       ########  ######\n"
            "\t\t##    ##  ##  ##  ##    ## ##       ##       ##    ##\n"
            "\t\t##         ####   ##       ##       ##       ##\n"
            "\t\t##          ##    ##       ##       ######    ######\n"  
            "\t\t##          ##    ##       ##       ##             ##\n"
            "\t\t##    ##    ##    ##    ## ##       ##       ##    ##\n"
            "\t\t ######     ##     ######  ######## ########  ######\n\n\n");
    printf("\t\t Copyright(c)2010-2015 PSU / WSU All rights reserved\n\n\n");

    ReadSimControl (project, &Cycles->SimControl);
    ReadSoil (project, &Cycles->Soil);
    ReadCrop (project, Cycles->Crop);
    ReadOperation (project, &Cycles->FieldOperation);

    return 0;
}
