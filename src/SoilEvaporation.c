#include "include/Cycles.h"

void Evaporation (SoilStruct *Soil, CropStruct *Crop, ResidueStruct *Residue, double ETo, double SnowCover)
{
    int             i;
    double          EvaporativeDemand;  /* mm/day */
    double          WC_AirDry;  /* m3/m3 */
    double          layerTop[Soil->totalLayers];    /* m */
    double          layerBottom[Soil->totalLayers]; /* m */
    double          layerMidpoint;  /* m */
    double          WaterAvailable; /* layer available water for evaporation, mm */
    double          WaterSupply;    /* layer water supply, mm */
    double          DepthLimitation;    /* factor that limits evaporable water based on depth */
    double          WaterContentLimitation; /* factor that limits evaporable water based on WC */
    double          Evaporation;    /* evaporation by layer, mm */
    double          WCi[Soil->totalLayers]; /* copy initial soil water content */
    double          EvapFlux[Soil->totalLayers];    /* store flux from each layer to compute solute transport */

    for (i = 0; i < Soil->totalLayers; i++)
        WCi[i] = Soil->waterContent[i];

    Soil->evaporationVol = 0.;

    /* It uses the maximum cover of either flat residues or snow to calculate
     * radiation reaching the soil surface */
    if (1. - Residue->flatResidueTau >= SnowCover)
        EvaporativeDemand = (1. - Residue->residueInterception) * (1. - Crop->svRadiationInterception) * ETo;
    else
        EvaporativeDemand = Residue->stanResidueTau * (1. - SnowCover) * (1. - Crop->svRadiationInterception) * ETo;

#ifdef _DEBUG_
    printf ("Evaporation:\n");
    printf ("EvaporativeDemand = %lf, residueInterception = %lf, svRadiationInterception = %lf, ETo = %lf, \n", EvaporativeDemand, Residue->residueInterception, Crop->svRadiationInterception, ETo);
#endif

    for (i = 0; i < Soil->totalLayers; i++)
    {
        if (i > 0)
        {
            layerBottom[i] = layerBottom[i - 1] + Soil->layerThickness[i];
            layerTop[i] = layerBottom[i - 1];
        }
        else
        {
            layerBottom[i] = Soil->layerThickness[i];
            layerTop[i] = 0.;
        }

        layerMidpoint = 0.5 * (layerTop[i] + layerBottom[i]);
        WC_AirDry = Soil->PWP[i] / 3.;  /* an approximation to air dry */
        WaterAvailable = (Soil->waterContent[i] - WC_AirDry) * Soil->layerThickness[i] * WATER_DENSITY;

        DepthLimitation = 1. / 3. * (Depth_Limitation_To_Evaporation (layerTop[i]) + Depth_Limitation_To_Evaporation (layerMidpoint) + Depth_Limitation_To_Evaporation (layerBottom[i]));
        WaterContentLimitation = Water_Content_Limitation_To_Evaporation (Soil->FC[i], WC_AirDry, Soil->waterContent[i]);
        WaterSupply = WaterAvailable * DepthLimitation * WaterContentLimitation;

        if (WaterSupply > EvaporativeDemand)
            Evaporation = EvaporativeDemand;
        else
            Evaporation = WaterSupply;

        EvaporativeDemand -= Evaporation;
        Soil->evaporationVol += Evaporation;
        Soil->waterContent[i] -= Evaporation / (Soil->layerThickness[i] * WATER_DENSITY);
        EvapFlux[i] = Evaporation;

#ifdef _DEBUG_
        printf ("waterContent[%d] = %lf\n", i + 1, Soil->waterContent[i]);
#endif

        if (EvaporativeDemand == 0.)
            break;
    }
#ifdef _DEBUG_
    printf ("evaporationVol = %lf\n", Soil->evaporationVol);
#endif

    /* Try changing retention constant to slow down nitrate upward movement,
     * try 1 (defaul is 0 for nitrate downward in liquid flow) */
    if (Soil->evaporationVol > 0.)
    {
        SoluteTransportEvaporation (Soil->totalLayers, 1, EvapFlux, Soil->NO3, Soil->BD, Soil->layerThickness, Soil->Porosity, WCi);
        SoluteTransportEvaporation (Soil->totalLayers, 5.6, EvapFlux, Soil->NH4, Soil->BD, Soil->layerThickness, Soil->Porosity, WCi);
    }
}

double Depth_Limitation_To_Evaporation (double Depth)
{
    return (1. / (1. + pow (Depth / 0.04, 3.5)));
}

double Water_Content_Limitation_To_Evaporation (double FC, double WC_AirDry, double WC)
{
    return (pow ((WC - WC_AirDry) / (FC - WC_AirDry), 3));
}
