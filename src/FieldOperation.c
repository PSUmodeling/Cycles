#include "Cycles.h"

void SelectNextOperation (int NumOperation, int *operationIndex)
{
    /*
     * Select next operation in the list, if any
     */
    (*operationIndex)++;
    if (*operationIndex >= NumOperation)
        *operationIndex = -1;
}

void SelectOperationYear (int rotationYear, const FieldOperationStruct *FieldOperation, int NumOperation, int *operationIndex)
{
    if (NumOperation == 0)
    {
        *operationIndex = -1;
    }
    else
    {
        if (rotationYear > 0)
        {
            *operationIndex = 0;
            while (*operationIndex >= 0)
            {
                if (FieldOperation[*operationIndex].opYear < rotationYear)
                    SelectNextOperation (NumOperation, operationIndex);
                else
                    break;
            }
        }
        else
        {
            printf ("ERROR: Field operations attempting to be passed an invalid year: %d", rotationYear);
            exit (1);
        }
    }
}

int IsOperationToday (int rotationYear, int doy, const FieldOperationStruct *FieldOperation, int operationIndex)
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

    if (operationIndex == -1)
        operation_today = 0;
    else
    {
        if (rotationYear == FieldOperation[operationIndex].opYear && doy == FieldOperation[operationIndex].opDay)
            operation_today = 1;
        else
            operation_today = 0;
    }

    return (operation_today);
}
