#include "Cycles.h"

int IsOperationToday (int rotationYear, int doy, FieldOperationStruct *FieldOperation, int numOperation, int *operationIndex)
{
    /*
     * Returns a true or false indicating if an operation happens on that day
     * -----------------------------------------------------------------------
     * LOCAL VARIABLES
     *
     * Variable             Type        Description
     * ==========           ==========  ====================
     * operation_today	    int		[return value]
     */
    int             operation_today;
    int             i;

    operation_today = 0;
    *operationIndex = -1;

    if  (numOperation <= 0)
    {
        operation_today = 0;
        *operationIndex = -1;
    }
    else
    {
        for (i = 0; i < numOperation; i++)
        {
            if (FieldOperation[i].status == 1)
                continue;
            else
            {
                if (rotationYear == FieldOperation[i].opYear && doy == FieldOperation[i].opDay)
                {
                    operation_today = 1;
                    FieldOperation[i].status = 1;
                    *operationIndex = i;

                    break;
                }
                else
                    break;
            }
        }
    }

    return (operation_today);
}

void UpdateOperationStatus (FieldOperationStruct *FieldOperation, int numOperation)
{
    int             i;
    int             all_performed = 1;

    if (numOperation <= 0)
    {
        /* Do nothing */
    }
    else
    {
        for (i = 0; i < numOperation; i++)
        {
            if (FieldOperation[i].status == 0)
            {
                all_performed = 0;
                break;
            }
        }

        if (all_performed)
        {
            for (i = 0 ; i < numOperation; i++)
                FieldOperation[i].status = 0;
        }
    }
}
