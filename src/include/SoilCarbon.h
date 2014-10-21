#define MAXIMUM_UNDISTURBED_SOC_DECOMPOSITION_RATE  0.00015     /*(1 + 0.056) ^ (1 / 365) - 1  ' 1/day (1/5 for Urbana) */
#define MAXIMUM_RESIDUE_DECOMPOSITION_RATE          0.05        /* 1/day */
#define MAXIMUM_ROOT_DECOMPOSITION_RATE             0.05        /* 1/day */
#define MAXIMUM_RHIZO_DECOMPOSITION_RATE            0.1         /* 1/day */
#define MAXIMUM_MANURE_DECOMPOSITION_RATE           0.05        /* 1/day */
#define MAXIMUM_MICROBIAL_DECOMPOSITION_RATE        1.0         /* (calculated internaly) 1/day */

#define FRACTION_CARBON_PLANT                       0.43
#define FRACTION_CARBON_RIZHO                       0.43
#define FRACTION_CARBON_MANURE                      0.4

#define SOC_DECOMPOSITION_POWER                     0.5
#define SOC_HUMIFICATION_POWER                      6.0

typedef struct SoilCarbonClass
{
    double *factorComposite;
    double *carbonRespired;
    double **rootBiomassInput;
    double **rhizBiomassInput;
    double **abgdBiomassInput;
    double **rootCarbonInput;
    double **rhizCarbonInput;
    double **manuCarbonInput;
    double **abgdCarbonInput;
    double **carbonMassInitial;
    double **carbonMassFinal;
    double **annualDecompositionFactor;
    double **annualSoilCarbonDecompositionRate;
    double **annualCarbonInputByLayer;
    double **annualHumifiedCarbonMass;
    double **annualRespiredCarbonMass;
    double **annualRespiredResidueCarbonMass;
    double **annualHumificationCoefficient;
} SoilCarbonClass;
