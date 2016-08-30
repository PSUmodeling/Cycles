#ifdef _PIHM_
#include "pihm.h"
#else
#include "Cycles.h"
#endif

#ifdef _PIHM_
void SoluteTransport (elem_struct *elem, int numele, river_struct *riv,
    int numriv, double dt)
{
    int             i, j;
    elem_struct    *snk;
    river_struct   *river;
    river_struct   *down;
    int             k, kk;
    int             k_snk;
    int             k_river;
    double          waterInitial;
    double          soluteMassSolution;
    double          river_smc;
    double          river_bd;
    double          river_sldpth;
    const int       UP = 0;
    const int       DOWN = 1;
    const int       LEFT = 2;
    const int       RIGHT = 3;

    /* Link solute structures to soil solutes */
    for (i = 0; i < numele; i++)
    {
        for (j = 0; j < elem[i].ps.nsoil; j++)
        {
            elem[i].NO3sol.soluteMass[j] = elem[i].soil.NO3[j];
            elem[i].NH4sol.soluteMass[j] = elem[i].soil.NH4[j];
        }
    }

    /*
     * Adsortion
     */
    for (i = 0; i < numele; i++)
    {
        Adsorption (elem[i].ps.sldpth, elem[i].ws.smc, elem[i].soil.BD,
            elem[i].ps.nsoil, 0.0, &elem[i].NO3sol);
        Adsorption (elem[i].ps.sldpth, elem[i].ws.smc, elem[i].soil.BD,
            elem[i].ps.nsoil, 5.6, &elem[i].NH4sol);
    }

    for (i = 0; i < numriv; i++)
    {
        river_sldpth =
            ((riv[i].ws.stage >
                0.0) ? riv[i].ws.stage : 0.0) + riv[i].topo.zbed -
            riv[i].topo.zmin;
        river_smc =
            (((riv[i].ws.stage >
                    0.0) ? riv[i].ws.stage : 0.0) +
            riv[i].matl.porosity * (riv[i].topo.zbed -
                riv[i].topo.zmin)) / river_sldpth;
        river_bd = 0.0;         // Ignore river bed adsorption

        riv[i].NO3sol.soluteConc =
            LinearEquilibriumConcentration (0.0, river_bd, river_sldpth,
            river_smc, riv[i].NO3sol.soluteMass);
        riv[i].NH4sol.soluteConc = 
            LinearEquilibriumConcentration (5.6, river_bd, river_sldpth,
            river_smc, riv[i].NH4sol.soluteMass);

        riv[i].NO3sol.soluteMassAdsorbed = 0.0;
        riv[i].NH4sol.soluteMassAdsorbed = 0.0;
    }

    /*
     * NO3 lateral transport
     */
    for (i = 0; i < numele; i++)
    {
        for (k = 0; k < NUM_EDGE; k++)
        {
            if (elem[i].nabr[k] > 0)
            {
                if (elem[i].wf.subsurf[k] > 0.0)
                {
                    snk = elem + elem[i].nabr[k] - 1;
                    for (kk = 0; kk < NUM_EDGE; kk++)
                    {
                        if (snk->nabr[kk] == elem[i].ind)
                        {
                            k_snk = kk;
                            break;
                        }
                    }

                    Elem2ElemSolTrnsp (elem + i, snk, elem[i].wf.smflxh[k],
                        elem[i].NO3sol.soluteConc, dt,
                        elem[i].NO3sol.soluteFluxLat[k],
                        snk->NO3sol.soluteFluxLat[k_snk]);
                    Elem2ElemSolTrnsp (elem + i, snk, elem[i].wf.smflxh[k],
                        elem[i].NH4sol.soluteConc, dt,
                        elem[i].NH4sol.soluteFluxLat[k],
                        snk->NH4sol.soluteFluxLat[k_snk]);
                }
            }
            else
            {
                river = &(riv[-elem[i].nabr[k] - 1]);
                if (river->leftele == i + 1)
                {
                    k_river = LEFT;
                }
                else
                {
                    k_river = RIGHT;
                }

                Elem2RiverSolTrnsp (elem + i, river, elem[i].wf.subsurf[k],
                    elem[i].wf.smflxh[k], elem[i].NO3sol.soluteConc,
                    river->NO3sol.soluteConc, dt,
                    elem[i].NO3sol.soluteFluxLat[k],
                    &river->NO3sol.soluteFluxLat[k_river]);
                Elem2RiverSolTrnsp (elem + i, river, elem[i].wf.subsurf[k],
                    elem[i].wf.smflxh[k], elem[i].NH4sol.soluteConc,
                    river->NH4sol.soluteConc, dt,
                    elem[i].NH4sol.soluteFluxLat[k],
                    &river->NH4sol.soluteFluxLat[k_river]);
            }
        }
    }

    for (i = 0; i < numriv; i++)
    {
        if (riv[i].down > 0)
        {
            down = &riv[riv[i].down - 1];

            River2RiverSolTrnsp (riv + i, down, riv[i].wf.rivflow,
                riv[i].NO3sol.soluteConc, down->NO3sol.soluteConc, dt,
                &riv[i].NO3sol.soluteFluxLat[DOWN],
                &down->NO3sol.soluteFluxLat[UP]);
            River2RiverSolTrnsp (riv + i, down, riv[i].wf.rivflow,
                riv[i].NH4sol.soluteConc, down->NH4sol.soluteConc, dt,
                &riv[i].NH4sol.soluteFluxLat[DOWN],
                &down->NH4sol.soluteFluxLat[UP]);
        }
        else
        {
            riv[i].NO3sol.soluteFluxLat[DOWN] =
                riv[i].wf.rivflow[DOWN_CHANL2CHANL] *
                riv[i].NO3sol.soluteConc * dt * WATER_DENSITY /
                riv[i].topo.area;
            riv[i].NH4sol.soluteFluxLat[DOWN] =
                riv[i].wf.rivflow[DOWN_CHANL2CHANL] *
                riv[i].NH4sol.soluteConc * dt * WATER_DENSITY /
                riv[i].topo.area;
        }
    }

    for (i = 0; i < numele; i++)
    {
        elem[i].soil.NO3Profile = 0.0;
        elem[i].soil.NH4Profile = 0.0;

        for (k = 0; k < NUM_EDGE; k++)
        {
            elem[i].soil.NO3Leaching[k] = 0.0;
            elem[i].soil.NH4Leaching[k] = 0.0;
        }

        for (j = 0; j < elem[i].ps.nsoil; j++)
        {
            waterInitial =
                elem[i].ps.sldpth[j] * WATER_DENSITY * elem[i].ws.smc[j];

            soluteMassSolution = waterInitial * elem[i].NO3sol.soluteConc[j];

            /* Vertical transport from layer bottom */
            if (j < MAXLYR - 1)
            {
                if (elem[i].wf.smflxv[j] > 0.0)
                {
                    soluteMassSolution -=
                        elem[i].wf.smflxv[j] * elem[i].NO3sol.soluteConc[j] *
                        dt * WATER_DENSITY;
                }
                else
                {
                    soluteMassSolution -=
                        elem[i].wf.smflxv[j] * elem[i].NO3sol.soluteConc[j +
                        1] * dt * WATER_DENSITY;
                }
            }

            /* Vertical transport from layer top */
            if (j > 0)
            {
                if (elem[i].wf.smflxv[j - 1] > 0.0)
                {
                    soluteMassSolution +=
                        elem[i].wf.smflxv[j -
                        1] * elem[i].NO3sol.soluteConc[j -
                        1] * dt * WATER_DENSITY;
                }
                else
                {
                    soluteMassSolution +=
                        elem[i].wf.smflxv[j -
                        1] * elem[i].NO3sol.soluteConc[j] * dt *
                        WATER_DENSITY;
                }
            }

            /* Lateral transport */
            for (k = 0; k < NUM_EDGE; k++)
            {
                soluteMassSolution -= elem[i].NO3sol.soluteFluxLat[k][j];
                elem[i].soil.NO3Leaching[k] +=
                    elem[i].NO3sol.soluteFluxLat[k][j] * elem[i].topo.area;
            }

            elem[i].NO3sol.soluteMass[j] =
                elem[i].NO3sol.soluteMassAdsorbed[j] + soluteMassSolution;

            elem[i].NO3sol.soluteMass[j] =
                (elem[i].NO3sol.soluteMass[j] >
                0.0) ? elem[i].NO3sol.soluteMass[j] : 0.0;

            elem[i].soil.NO3[j] = elem[i].NO3sol.soluteMass[j];

            elem[i].soil.NO3Profile += elem[i].soil.NO3[j];

            /*
             * NH4
             */
            soluteMassSolution = waterInitial * elem[i].NH4sol.soluteConc[j];

            /* Vertical transport from layer bottom */
            if (j < MAXLYR - 1)
            {
                if (elem[i].wf.smflxv[j] > 0.0)
                {
                    soluteMassSolution -=
                        elem[i].wf.smflxv[j] * elem[i].NH4sol.soluteConc[j] *
                        dt * WATER_DENSITY;
                }
                else
                {
                    soluteMassSolution -=
                        elem[i].wf.smflxv[j] * elem[i].NH4sol.soluteConc[j +
                        1] * dt * WATER_DENSITY;
                }
            }

            /* Vertical transport from layer top */
            if (j > 0)
            {
                if (elem[i].wf.smflxv[j - 1] > 0.0)
                {
                    soluteMassSolution +=
                        elem[i].wf.smflxv[j - 1] *
                        elem[i].NH4sol.soluteConc[j - 1] * dt * WATER_DENSITY;
                }
                else
                {
                    soluteMassSolution +=
                        elem[i].wf.smflxv[j - 1] *
                        elem[i].NH4sol.soluteConc[j] * dt * WATER_DENSITY;
                }
            }

            /* Lateral transport */
            for (k = 0; k < NUM_EDGE; k++)
            {
                soluteMassSolution -= elem[i].NH4sol.soluteFluxLat[k][j];
                elem[i].soil.NH4Leaching[k] +=
                    elem[i].NH4sol.soluteFluxLat[k][j] * elem[i].topo.area;
            }

            elem[i].NH4sol.soluteMass[j] =
                elem[i].NH4sol.soluteMassAdsorbed[j] + soluteMassSolution;

            elem[i].NH4sol.soluteMass[j] =
                (elem[i].NH4sol.soluteMass[j] > 0.0) ?
                elem[i].NH4sol.soluteMass[j] : 0.0;

            elem[i].soil.NH4[j] = elem[i].NH4sol.soluteMass[j];

            elem[i].soil.NH4Profile += elem[i].soil.NH4[j];
        }
    }

    for (i = 0; i < numriv; i++)
    {
        waterInitial = (((riv[i].ws.stage > 0.0) ? riv[i].ws.stage : 0.0) +
            riv[i].matl.porosity * (riv[i].topo.zbed -
                riv[i].topo.zmin)) * WATER_DENSITY;
        for (k = 0; k < 4; k++)
        {
            riv[i].NO3Leaching[k] = 0.0;
            riv[i].NH4Leaching[k] = 0.0;
        }

        /* NO3 */
        soluteMassSolution = waterInitial * riv[i].NO3sol.soluteConc;

        for (k = 0; k < 4; k++)
        {
            soluteMassSolution -= riv[i].NO3sol.soluteFluxLat[k];
            riv[i].NO3Leaching[k] +=
                riv[i].NO3sol.soluteFluxLat[k] * riv[i].topo.area;
        }

        riv[i].NO3sol.soluteMass =
            riv[i].NO3sol.soluteMassAdsorbed + soluteMassSolution;

        riv[i].NO3sol.soluteMass = (riv[i].NO3sol.soluteMass > 0.0) ?
            riv[i].NO3sol.soluteMass : 0.0;

        /* NH4 */
        soluteMassSolution = waterInitial * riv[i].NH4sol.soluteConc;

        for (k = 0; k < 4; k++)
        {
            soluteMassSolution -= riv[i].NH4sol.soluteFluxLat[k];
            riv[i].NH4Leaching[k] +=
                riv[i].NH4sol.soluteFluxLat[k] * riv[i].topo.area;
        }

        riv[i].NH4sol.soluteMass =
            riv[i].NH4sol.soluteMassAdsorbed + soluteMassSolution;

        riv[i].NH4sol.soluteMass = (riv[i].NH4sol.soluteMass > 0.0) ?
            riv[i].NH4sol.soluteMass : 0.0;
    }
}

void Adsorption (const double *sldpth, const double *sh2o, const double *bd,
    int nsoil, double Sol_Kd, solute_struct *solute)
{
    int             j;
    double          waterInitial;

    for (j = 0; j < MAXLYR; j++)
    {
        solute->soluteConc[j] = 0.0;
        solute->soluteMassAdsorbed[j] = 0.0;
    }

    for (j = 0; j < nsoil; j++)
    {
        waterInitial = sldpth[j] * WATER_DENSITY * sh2o[j];

        if (solute->soluteMass[j] > 0.0)
        {
            solute->soluteConc[j] =
                LinearEquilibriumConcentration (Sol_Kd, bd[j], sldpth[j],
                sh2o[j], solute->soluteMass[j]);
        }

        solute->soluteMassAdsorbed[j] =
            solute->soluteMass[j] - waterInitial * solute->soluteConc[j];
        solute->soluteMassAdsorbed[j] =
            (solute->soluteMassAdsorbed[j] <
            0.0) ? 0.0 : solute->soluteMassAdsorbed[j];
    }
}

void Elem2ElemSolTrnsp (const elem_struct *src, const elem_struct *snk,
    double *smflx, const double *conc, double dt, double *flux_sol_src,
    double *flux_sol_snk)
{
    int             i, j;
    double          srcdpth_tot = 0.0;
    double          snkdpth_tot = 0.0;
    double          srcdpth[MAXLYR];
    double          snkdpth[MAXLYR];
    double          eq_depth[MAXLYR];

    for (i = 0; i < src->ps.nsoil; i++)
    {
        srcdpth_tot += src->ps.satdpth[i];
        srcdpth[i] = src->ps.satdpth[i];
    }

    for (j = 0; j < snk->ps.nsoil; j++)
    {
        snkdpth_tot += snk->ps.satdpth[j];
        snkdpth[j] = snk->ps.satdpth[j];
    }

    for (j = 0; j < snk->ps.nsoil; j++)
    {
        eq_depth[j] = srcdpth_tot / snkdpth_tot * snkdpth[j];
    }

    for (j = 0; j < MAXLYR; j++)
    {
        flux_sol_src[j] = 0.0;
        flux_sol_snk[j] = 0.0;
    }

    i = 0;
    j = 0;

    while (j < snk->ps.nsoil && i < src->ps.nsoil)
    {
        if (eq_depth[j] <= 0.0)
        {
            j++;
        }
        else if (srcdpth[i] < eq_depth[j])
        {
            flux_sol_src[i] +=
                (srcdpth[i] >
                0.0) ? srcdpth[i] / src->ps.satdpth[i] * smflx[i] * conc[i] *
                dt * WATER_DENSITY / src->topo.area : 0.0;
            flux_sol_snk[j] -=
                (srcdpth[i] >
                0.0) ? srcdpth[i] / src->ps.satdpth[i] * smflx[i] * conc[i] *
                dt * WATER_DENSITY / snk->topo.area : 0.0;
            eq_depth[j] -= srcdpth[i];
            i++;
        }
        else
        {
            flux_sol_src[i] +=
                eq_depth[j] / src->ps.satdpth[i] * smflx[i] * conc[i] * dt *
                WATER_DENSITY / src->topo.area;
            flux_sol_snk[j] -=
                eq_depth[j] / src->ps.satdpth[i] * smflx[i] * conc[i] * dt *
                WATER_DENSITY / snk->topo.area;
            srcdpth[i] -= eq_depth[j];
            j++;
        }
    }
}

void Elem2RiverSolTrnsp (const elem_struct *elem, const river_struct *riv,
    double gwflux, double *smflx, const double *elem_conc,
    double riv_conc, double dt, double *flux_sol_elem,
    double *flux_sol_riv)
{
    int             k;
    double          flux_sol_total;

    flux_sol_total = 0.0;

    for (k = 0; k < elem->ps.nsoil; k++)
    {
        if (gwflux > 0.0)
        {
            flux_sol_elem[k] =
                smflx[k] * elem_conc[k] * dt * WATER_DENSITY /
                elem->topo.area;
        }
        else
        {
            flux_sol_elem[k] =
                smflx[k] * riv_conc * dt * WATER_DENSITY / elem->topo.area;
        }

        flux_sol_total -= flux_sol_elem[k] * elem->topo.area;
    }

    *flux_sol_riv = flux_sol_total / riv->topo.area;
}

void River2RiverSolTrnsp (river_struct *riv, const river_struct *down,
    double *fluxriv, double riv_conc, double down_conc,
    double dt, double *flux_sol_riv, double *flux_sol_down)
{
    double          flux_total;
    flux_total =
        riv->wf.rivflow[DOWN_CHANL2CHANL] + riv->wf.rivflow[DOWN_AQUIF2AQUIF];

    if (flux_total > 0.0)
    {
        *flux_sol_riv =
            flux_total * riv_conc * dt * WATER_DENSITY / riv->topo.area;
        *flux_sol_down =
            -flux_total * riv_conc * dt * WATER_DENSITY / down->topo.area;
    }
    else
    {
        *flux_sol_riv =
            flux_total * down_conc * dt * WATER_DENSITY / riv->topo.area;
        *flux_sol_down =
            -flux_total * down_conc * dt * WATER_DENSITY / down->topo.area;
    }
}
#else
void SoluteTransport (int totalLayers, double Sol_Kd, double WInConc,
    double *leachate, const double *WFlux, double *soluteMass,
    const double *BD, const double *thickness, const double *porosity,
    const double *WCinitial)
{
    /*
     * 
     * -----------------------------------------------------------------------
     * LOCAL VARIABLES
     *
     * Variable             Type        Description
     * ==========           ==========  ====================
     * i                    int         Loop counter
     * soluteConc           double      Solute concentration, current layer
     *                                    [kg solute / m3]
     * soluteFlow           double[]    Solute flow [kg solute/time step]
     * waterInitial         double      Initial layer soil water storage
     *                                    [kg/m2]
     * ratio                double      Ratio of flux out of a layer to the
     *                                    layer porosity (or effective
     *                                    porosity)
     * soluteMassAdsorbed   double      [kg/m2]
     * soluteMassSolution   double      [kg/m2]
     */

    int             i;
    double          soluteConc;
    double          soluteFlow[totalLayers + 1];
    double          waterInitial;
    double          ratio;
    double          soluteMassAdsorbed;
    double          soluteMassSolution;

    soluteFlow[0] = WInConc * WFlux[0];

    for (i = 0; i < totalLayers; i++)
    {
        soluteConc = 0.0;
        soluteMassAdsorbed = 0.0;
        waterInitial = thickness[i] * WATER_DENSITY * WCinitial[i];

        if (soluteMass[i] > 0.0)
            soluteConc =
                LinearEquilibriumConcentration (Sol_Kd, BD[i], thickness[i],
                WCinitial[i], soluteMass[i]);

        soluteMassAdsorbed = soluteMass[i] - waterInitial * soluteConc;

        if (WFlux[i + 1] > 0.0)
        {
            /* Note: the 0.67 is temporary, likely not needed, and if needed
             * use a relationship with B */
            ratio =
                0.67 * WFlux[i +
                1] / (porosity[i] * thickness[i] * WATER_DENSITY);
            soluteMassSolution =
                (soluteFlow[i] / ratio + (waterInitial * soluteConc -
                    soluteFlow[i] / ratio) * exp (-ratio));
            soluteFlow[i + 1] =
                waterInitial * soluteConc + soluteFlow[i] -
                soluteMassSolution;
            if (i == totalLayers - 1)
                *leachate = soluteFlow[i + 1];
        }
        else
        {
            soluteMassSolution = waterInitial * soluteConc + soluteFlow[i];
            soluteFlow[i + 1] = 0.0;
        }

        /* This equilibrium can cause a mass balance error if "mass" is based
         * on concentration at the end ignoring initial mass, not using it */
        //SoluteMass(i) = LinearEquilibriumSoluteMass(Sol_Kd, BD(i), Thickness(i), WCFinal, C(i))
        soluteMass[i] = soluteMassAdsorbed + soluteMassSolution;
    }
}
#endif

void SoluteTransportEvaporation (int totalLayers, double Sol_Kd,
    const double *WFlux, double *soluteMass, const double *BD,
    const double *thickness, const double *porosity, double *WCinitial)
{
    /*
     * This is a rough attempt to move solutes upward due to evaporation
     * -----------------------------------------------------------------------
     * LOCAL VARIABLES
     *
     * Variable             Type        Description
     * ==========           ==========  ====================
     * i                    int
     * soluteConc           double      Solute concentration, current layer
     *                                    [kg solute/m3]
     * waterInitial         double      Initial layer soil water storage
     *                                    [kg/m2]
     * soluteFlux           double[]
     */

    int             i;
    double          soluteConc;
    double          waterInitial;
    double          soluteFlux[totalLayers];

    for (i = 1; i < totalLayers; i++)
    {
        waterInitial = thickness[i] * WATER_DENSITY * WCinitial[i];

        if (soluteMass[i] > 0.0)
            soluteConc =
                LinearEquilibriumConcentration (Sol_Kd, BD[i], thickness[i],
                WCinitial[i], soluteMass[i]);

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

/*****************************************************************************
 * FUNCTION NAME:   LinearEquilibriumConcentration
 *
 * ARGUMENT LIST
 *
 * Argument             Type        IO  Description
 * ==========           ==========  ==  ====================
 * Kd                   double      I   Slope of the adsortion isotherm
 *                                        [m3/kg]
 * bulkDensity          double      I   Soil bulk density [kg/m3]
 * layerThickness       double      I
 * waterContent         double      I
 * soluteMass           double      I
 *
 * RETURN VALUE: double (solute equilibrium concentration [kg solute/m3])
 ****************************************************************************/
double LinearEquilibriumConcentration (double Kd, double bulkDensity,
    double layerThickness, double waterContent, double soluteMass)
{
    /* LOCAL VARIABLES
     *
     * Variable             Type        Description
     * ==========           ==========  ====================
     * soilBufferPower      double      [m3/m3]
     */
    double          soilBufferPower;

    bulkDensity *= 1000.0;      /* Convert Mg/m3 to kg/m3 */
    soilBufferPower = Kd * bulkDensity + waterContent;

    return (soluteMass / (soilBufferPower * layerThickness * WATER_DENSITY));
}

/*****************************************************************************
 * FUNCTION NAME:   LinearEquilibriumSoluteMass
 *
 * ARGUMENT LIST
 *
 * Argument             Type        IO  Description
 * ==========           ==========  ==  ====================
 * Kd                   double      I   Slope of the adsortion isotherm
 *                                        [m3/kg]
 * bulkDensity          double      I   Soil bulk density [kg/m3]
 * layerThickness       double      I
 * waterContent         double      I
 * concentration        double      I   Solute equilibrium concentration
 *                                        [kg solute / m3]
 *
 * RETURN VALUE: double (total solute mass based on concentration in the
 *   solution)
 ****************************************************************************/
double LinearEquilibriumSoluteMass (double Kd, double bulkDensity,
    double layerThickness, double waterContent, double concentration)
{
    /* LOCAL VARIABLES
     *
     * Variable             Type        Description
     * ==========           ==========  ====================
     * soilBufferPower      double      [m3/m3]
     */

    double          soilBufferPower;

    bulkDensity *= 1000.0;      /* convert Mg/m3 to kg/m3 */
    soilBufferPower = Kd * bulkDensity + waterContent;

    return (concentration * (soilBufferPower * layerThickness *
            WATER_DENSITY));
}
