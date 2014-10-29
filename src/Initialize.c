#include "include/Cycles.h"

void Initialize (CyclesStruct Cycles)
{

//    CalculateDerivedWeather ();

    /* Initialize soil vairables */
    InitializeSoil (&Cycles->Soil, &Cycles->Weather);
}














