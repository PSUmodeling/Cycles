#ifndef CYCLES_CONST_HEADER
#define CYCLES_CONST_HEADER

#define PI              3.14159265358979
#define MAXSTRING       1024
#define BADVAL          -999


#define STAN_RESIDUE_SA 4.0     /* Standing residue area to mass ratio
                                 * (m2/kg) */
#define FLAT_RESIDUE_SA 4.0     /* Flat residue area to mass ratio (m2/kg) */
#define STAN_RESIDUE_K  0.25    /* Standing residue extinction coefficient */
#define FLAT_RESIDUE_K  1.0     /* flat residue extinction */

#define MAXIMUM_UNDISTURBED_SOC_DECOMPOSITION_RATE 0.00015  /* (1 + 0.056) ^ (1 / 365) - 1  ' 1/day (1/5 for Urbana) */
#define MAXIMUM_RESIDUE_DECOMPOSITION_RATE  0.05    /* 1/day */
#define MAXIMUM_ROOT_DECOMPOSITION_RATE     0.05    /* 1/day */
#define MAXIMUM_RHIZO_DECOMPOSITION_RATE    0.1     /*  1/day */
#define MAXIMUM_MANURE_DECOMPOSITION_RATE   0.05    /* 1/day */
#define MAXIMUM_MICROBIAL_DECOMPOSITION_RATE 1.0    /* calculated internaly
                                                     * 1/day */
#define FRACTION_CARBON_PLANT               0.43
#define FRACTION_CARBON_RIZHO               0.43
#define FRACTION_CARBON_MANURE              0.4

#define SOC_DECOMPOSITION_POWER             0.5
#define SOC_HUMIFICATION_POWER              6.0

#endif

