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

    Cycles = (CyclesStruct) malloc (sizeof (*Cycles));

#ifdef _DEBUG_
        project = (char *)malloc (8*sizeof (char));
        strcpy(project, "Lebanon");
#else
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
#endif

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
    PrintSimContrl (Cycles->SimControl);
#endif

    ReadSoil (project, Cycles);
#ifdef _DEBUG_
    PrintSoil (Cycles->Soil);
#endif

    ReadCrop (project, Cycles);
#ifdef _DEBUG_
    PrintCrop (Cycles->describedCrops, Cycles->NumDescribedCrop);
#endif

    ReadOperation (project, Cycles);
#ifdef _DEBUG_
    PrintOperation (Cycles->plantedCrops, Cycles->NumPlantedCrop, Cycles->TillageList, Cycles->FixedIrrigationList, Cycles->FixedFertilizationList);
#endif

    ReadWeather (project, Cycles);
#ifdef _DEBUG_
    PrintWeather (Cycles->Weather);
    printf("%lf %lf %lf %lf %lf\n", dailyPrecipitation (&Cycles->Weather, 2001, 23), dailyRelativeHumidityMin (&Cycles->Weather, 2001, 23), dailySolarRadiation (&Cycles->Weather, 2001, 23), dailyTemperatureMin (&Cycles->Weather, 2001, 23), dailyWindSpeed (&Cycles->Weather, 2001, 23));
#endif

    Initialize (Cycles);
    return 0;
}
