#include "include/Cycles.h"

void ComputeThermalTime (int total_years, CropManagementStruct *CropManagement, WeatherStruct *Weather)
{
    /*
     * Calculate flowering and Maturity thermal time for each crop
     * in the rotation
     */

    double sumTT = 0.;
    double sumMTTbyYear = 0.;
    double sumFTTbyYear = 0.;

    int cropEvents;
    int cropON;
    int lastDOY = 365; /* Leap year is not taken into account */
    int c, y, d;

    describedCropsStruct *describedCrops;

    SelectCropInitialPosition(CropManagement);

#ifdef _DEBUG_
        printf ("*%-10s%-12s%-12s\n", "Crop", "FloweringTT", "MaturityTT");
#endif
    for (c = 0;  c < CropManagement->totalCropsPerRotation; c++)
    {
        SelectNextCrop (CropManagement);
        describedCrops = &(CropManagement->describedCrops[CropManagement->describedIndex]);
        if (describedCrops->calculatedMaturityTT == 0)
        {
            /* Computes thermal time for 1st instance of crop in each
             * rotation */
            cropEvents = 0;
            cropON = 0;

            for (y = 0; y < total_years; y++)
            {
                for (d = 1; d <= lastDOY; d++)
                {
                    if (d == describedCrops->userSeedingDate)
                        cropON = 1;
                    if (cropON)
                    {
                        sumTT = sumTT + 0.5 * (
                            ThermalTime (describedCrops->userTemperatureBase, describedCrops->userTemperatureOptimum, describedCrops->userTemperatureMaximum, Weather->tMax[y][d - 1])
                            + ThermalTime (describedCrops->userTemperatureBase, describedCrops->userTemperatureOptimum, describedCrops->userTemperatureMaximum, Weather->tMin[y][d - 1]));
                        if (d == describedCrops->userFloweringDate)
                            sumFTTbyYear = sumFTTbyYear + sumTT;
                        if (d == describedCrops->userMaturityDate)
                        {
                            cropEvents = cropEvents + 1;
                            sumMTTbyYear = sumMTTbyYear + sumTT;
                            sumTT = 0.;
                            cropON = 0;
                        }
                    }
                } /* End of d loop */
            } /* End of y loop */

            /* Load flow and mat TT as crop info for the crops in the rotation
             * Order of crops in array are assigned according to order in the
             * rotation */
            if (cropEvents > 0)
            {
                describedCrops->calculatedFloweringTT = sumFTTbyYear / cropEvents;
                describedCrops->calculatedMaturityTT = sumMTTbyYear / cropEvents;
            }
            else
            {
                describedCrops->calculatedFloweringTT = 0.;
                describedCrops->calculatedMaturityTT = 0.;
            }
        }
        sumFTTbyYear = 0.;
        sumMTTbyYear = 0.;
#ifdef _DEBUG_
        printf ("*%-10s%-12.2lf%-12.2lf\n", describedCrops->userCropName, y, describedCrops->calculatedFloweringTT, describedCrops->calculatedMaturityTT);
#endif
    }
}

double ThermalTime (double T_base, double T_op, double T_Max, double Temperature)
{

    if (Temperature <= T_base || Temperature >= T_Max)
        return (0);
    else if (Temperature < T_op)
        return (Temperature - T_base);
    else
        return ((T_Max - Temperature) / (T_Max - T_op) * (T_op - T_base));
}
