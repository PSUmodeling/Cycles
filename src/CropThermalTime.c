#include "include/Cycles.h"

void ComputeThermalTime (int total_years, CropManagementStruct *CropManagement, WeatherStruct *Weather)
{
    /*
     * Calculate flowering and Maturity thermal time for each crop
     * in the rotation
     */

    double sumTT = 0.0;
    double sumMTTbyYear = 0.0;
    double sumFTTbyYear = 0.0;

    int cropEvents;
    int cropON;
    int lastDOY = 365; /* Leap year is not taken into account */
    int c, y, d;

    describedCropStruct *describedCrop;

    SelectCropInitialPosition(CropManagement);

    for (c = 0;  c < CropManagement->totalCropsPerRotation; c++)
    {
        SelectNextCrop (CropManagement);
        describedCrop = &(CropManagement->describedCrop[CropManagement->describedIndex]);
        if (describedCrop->calculatedMaturityTT == 0.0)
        {
            /* Computes thermal time for 1st instance of crop in each
             * rotation */
            cropEvents = 0;
            cropON = 0;

            for (y = 0; y < total_years; y++)
            {
                for (d = 1; d <= lastDOY; d++)
                {
                    if (d == describedCrop->userSeedingDate)
                        cropON = 1;
                    if (cropON)
                    {
                        sumTT = sumTT + 0.5 * (
                            ThermalTime (describedCrop->userTemperatureBase, describedCrop->userTemperatureOptimum, describedCrop->userTemperatureMaximum, Weather->tMax[y][d - 1])
                            + ThermalTime (describedCrop->userTemperatureBase, describedCrop->userTemperatureOptimum, describedCrop->userTemperatureMaximum, Weather->tMin[y][d - 1]));
                        if (d == describedCrop->userFloweringDate)
                            sumFTTbyYear = sumFTTbyYear + sumTT;
                        if (d == describedCrop->userMaturityDate)
                        {
                            cropEvents = cropEvents + 1;
                            sumMTTbyYear = sumMTTbyYear + sumTT;
                            sumTT = 0.0;
                            cropON = 0;
                        }
                    }
                } 
            } 

            /* Load flow and mat TT as crop info for the crops in the rotation
             * Order of crops in array are assigned according to order in the
             * rotation */
            if (cropEvents > 0)
            {
                describedCrop->calculatedFloweringTT = sumFTTbyYear / cropEvents;
                describedCrop->calculatedMaturityTT = sumMTTbyYear / cropEvents;
            }
            else
            {
                describedCrop->calculatedFloweringTT = 0.0;
                describedCrop->calculatedMaturityTT = 0.0;
            }
        }
        sumFTTbyYear = 0.0;
        sumMTTbyYear = 0.0;
    }
}

double ThermalTime (double T_base, double T_op, double T_Max, double Temperature)
{
    double thermal_time;

    if (Temperature <= T_base || Temperature >= T_Max)
        thermal_time = 0.0;
    else if (Temperature < T_op)
        thermal_time = Temperature - T_base;
    else
        thermal_time = (T_Max - Temperature) / (T_Max - T_op) * (T_op - T_base);

    return (thermal_time);
}
