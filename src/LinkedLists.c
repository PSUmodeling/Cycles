#include "include/Cycles.h"

void InsertOperation (FieldOperationListStruct * OperationList, FieldOperationStruct * Operation)
{
    FieldOperationStruct *p;

    p = OperationList->firstOperation;

    if (p == NULL)
    {
        OperationList->firstOperation = Operation;
        Operation->NextOperation = NULL;
        if (OperationList->firstOperation == NULL)
            printf ("Insert failed\n");
    }
    else
    {
        while (p->NextOperation != NULL)
        {
            p = p->NextOperation;
        }
        p->NextOperation = Operation;
        Operation->NextOperation = NULL;
    }
}
