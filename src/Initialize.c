#include "include/Cycles.h"

void Initialize (CyclesStruct Cycles)
{
    /* Initialize weather variables */
    CalculateDerivedWeather (&Cycles->Weather, &Cycles->SimControl);

    /* Initialize soil variables */
    InitializeSoil (&Cycles->Soil, &Cycles->Weather, &Cycles->SimControl);
}
