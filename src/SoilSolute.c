#include "include/Cycles.h"

void SoluteTransport (int totalLayers, double Sol_Kd, double WInConc, double *leachate, const double *WFlux, double *soluteMass, const double *BD, const double *thickness, const double *porosity, const double *WCinitial)
{
    int             i;
    //double          C[totalLayers]; /* solute concentration at end of equilibrium, kg solute / m3 */
    double          soluteConc; /* solute concentration, current layer, kg solute / m3 */
    double          soluteFlow[totalLayers + 1];    /* solute flow, kg solute per time step */
    double          waterInitial;   /* initial layer soil water storage, kg / m2 */
    double          ratio;      /* ratio of flux out of a layer to the layer porosity (or effective porosity) */
    double          soluteMassAdsorbed; /* kg/m2 */
    double          soluteMassSolution; /* kg/m2 */

    soluteFlow[0] = WInConc * WFlux[0];

    for (i = 0; i < totalLayers; i++)
    {
        soluteConc = 0.0;
        soluteMassAdsorbed = 0.0;
        waterInitial = thickness[i] * WATER_DENSITY * WCinitial[i];

        if (soluteMass[i] > 0.0)
            soluteConc = LinearEquilibriumConcentration (Sol_Kd, BD[i], thickness[i], WCinitial[i], soluteMass[i]);

        soluteMassAdsorbed = soluteMass[i] - waterInitial * soluteConc;

        if (WFlux[i + 1] > 0.0 + 1e-6)
        {
            /* note: the 0.67 is temporary, likely not needed, and if needed
             * use a relationship with B */
            ratio = 0.67 * WFlux[i + 1] / (porosity[i] * thickness[i] * WATER_DENSITY);
            soluteMassSolution = (soluteFlow[i] / ratio + (waterInitial * soluteConc - soluteFlow[i] / ratio) * exp (-ratio));
            soluteFlow[i + 1] = waterInitial * soluteConc + soluteFlow[i] - soluteMassSolution;
            if (i == totalLayers - 1)
                *leachate = soluteFlow[i + 1];
        }
        else
        {
            soluteMassSolution = waterInitial * soluteConc + soluteFlow[i];
            soluteFlow[i + 1] = 0.0;
        }

        /* this equilibrium can cause a mass balance error if "mass" is based
         * on concentration at the end ignoring initial mass, not using it */
        //SoluteMass(i) = LinearEquilibriumSoluteMass(Sol_Kd, BD(i), Thickness(i), WCFinal, C(i))
        soluteMass[i] = soluteMassAdsorbed + soluteMassSolution;
    }
}

void SoluteTransportEvaporation (int totalLayers, double Sol_Kd, const double *WFlux, double *soluteMass, const double *BD, const double *thickness, const double *porosity, double *WCinitial)
{
    /* this is a rough attempt to move solutes upward due to evaporation */
    int             i;
    double          soluteConc; /* solute concentration, current layer, kg solute / m3 */
    double          waterInitial;   /* initial layer soil water storage, kg / m2 */
    double          soluteFlux[totalLayers];

    for (i = 1; i < totalLayers; i++)
    {
        waterInitial = thickness[i] * WATER_DENSITY * WCinitial[i];

        if (soluteMass[i] > 0.0)
            soluteConc = LinearEquilibriumConcentration (Sol_Kd, BD[i], thickness[i], WCinitial[i], soluteMass[i]);

        else
            soluteConc = 0.0;

        soluteFlux[i] = WFlux[i] * soluteConc;
    }

    for (i = totalLayers - 2; i >= 0; i--)
    {
        if (soluteFlux[i + 1] > 0)
        {
            soluteMass[i + 1] -= soluteFlux[i + 1];
            soluteMass[i] += soluteFlux[i + 1];
        }
    }
}

double LinearEquilibriumConcentration (double Kd, double bulkDensity, double layerThickness, double waterContent, double soluteMass)
{
    double          soilBufferPower;

    /* soil buffer power, m3/m3
     * soil bulk density, kg/m3
     * kd is the slope of the adsortion isotherm, m3/kg
     * solute equilibrium concentration, kg solute / m3 */
    bulkDensity *= 1000.0;       /* convert Mg/m3 to kg/m3 */
    soilBufferPower = Kd * bulkDensity + waterContent;

    return (soluteMass / (soilBufferPower * layerThickness * WATER_DENSITY));
}

double LinearEquilibriumSoluteMass (double Kd, double bulkDensity, double layerThickness, double waterContent, double concentration)
{
    /* returns the total solute mass based on concentration in the solution */
    double          soilBufferPower;

    /* soil buffer power, m3/m3
     * soil bulk density, kg/m3
     * kd is the slope of the adsortion isotherm, m3/kg
     * solute equilibrium concentration, kg solute / m3
     * solute mass, kg solute / m3 */
    bulkDensity *= 1000.0;       /* convert Mg/m3 to kg/m3 */
    soilBufferPower = Kd * bulkDensity + waterContent;

    return (concentration * (soilBufferPower * layerThickness * WATER_DENSITY));
}
