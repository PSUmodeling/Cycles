#include "include/Cycles.h"

void SelectCropInitialPosition(CropManagementStruct *CropManagement)
{
    CropManagement->plantingIndex = -1;
    CropManagement->describedIndex = -1;

    PeekNextCrop (CropManagement);
}

void SelectNextCrop (CropManagementStruct *CropManagement)
{
    /*
     * Planting index set to next crop (if any) to be planted in the rotation
     * Described index set to previously searched value inside plantingOrder
     * Next crop in the rotation (if any)has limited values stored
     */

    if (CropManagement->plantingIndex < CropManagement->totalCropsPerRotation)
        CropManagement->plantingIndex++;
    else
        CropManagement->plantingIndex = 0;

    CropManagement->describedIndex = CropManagement->plantingOrder[CropManagement->plantingIndex].plantID;
//#ifdef _DEBUG_
//    printf ("%-25s\t%-3d\n", "*Described index", CropManagement->describedIndex);
//    printf ("(Press any key to continue ...)\n");
//    getchar ();
//#endif 
    PeekNextCrop (CropManagement);
}

void PeekNextCrop(CropManagementStruct *CropManagement)
{
    /*
     * Store the name, year, day, and type values for the next crop
     */
    int tempIndex = CropManagement->plantingIndex;
    
    if (CropManagement->totalCropsPerRotation > 0)
    {
        /* Select next crop */
        if (tempIndex < CropManagement->totalCropsPerRotation - 1)
            tempIndex++;
        else
            tempIndex = 0;

        CropManagement->nextCropSeedingYear = CropManagement->plantingOrder[tempIndex].seedingYear;
        CropManagement->nextCropSeedingDate = CropManagement->plantingOrder[tempIndex].seedingDate;
        strcpy (CropManagement->nextCropName, CropManagement->plantingOrder[tempIndex].cropName);
    }
//#ifdef _DEBUG_
//    printf ("*%-25s\t%-3d\n", "Next crop seeding year", CropManagement->nextCropSeedingYear);
//    printf ("*%-25s\t%-3d\n", "Next crop seeding date", CropManagement->nextCropSeedingDate);
//    printf ("*%-25s\t%-10s\n", "Next crop name", CropManagement->nextCropName);
//    printf ("(Press any key to continue ...)\n");
//    getchar ();
//#endif 
}
//

//


//
//    Public ReadOnly Property Peek_CropName() As String
//        Get
//            Return nextCropName
//        End Get
//    End Property
//    Public ReadOnly Property Peek_CropSeedingDate() As Integer
//        Get
//            Return nextCropSeedingDate
//        End Get
//    End Property
//    Public ReadOnly Property Peek_CropSeedingYear() As Integer
//        Get
//            Return nextCropSeedingYear
//        End Get
//    End Property
//
//    Public ReadOnly Property CropsPerRotation() As Integer
//        Get
//            Return totalCropsPerRotation
//        End Get
//    End Property
//
//    Public ReadOnly Property userCropName() As String
//        Get
//            Return describedCrops(describedIndex, 0)
//        End Get
//    End Property
//    Public ReadOnly Property userSeedingDate() As String
//        Get
//            Return describedCrops(describedIndex, 1)
//        End Get
//    End Property
//    Public ReadOnly Property userFloweringDate() As String
//        Get
//            Return describedCrops(describedIndex, 2)
//        End Get
//    End Property
//    Public ReadOnly Property userMaturityDate() As String
//        Get
//            Return describedCrops(describedIndex, 3)
//        End Get
//    End Property
//    'Public ReadOnly Property userMaximumSoilCoverage() As String
//    '    Get
//    '        Return describedCrops(describedIndex, 4)
//    '    End Get
//    'End Property
//    'Public ReadOnly Property userMaximumRootingDepth() As Double
//    '    Get
//    '        Return describedCrops(describedIndex, 5)
//    '    End Get
//    'End Property
//    'Public ReadOnly Property userExpectedYieldAvg() As Double
//    '    Get
//    '        Return describedCrops(describedIndex, 6)
//    '    End Get
//    'End Property
//    'Public ReadOnly Property userExpectedYieldMax() As Double
//    '    Get
//    '        Return describedCrops(describedIndex, 7)
//    '    End Get
//    'End Property
//    'Public ReadOnly Property userExpectedYieldMin() As Double
//    '    Get
//    '        Return describedCrops(describedIndex, 8)
//    '    End Get
//    'End Property
//    'Public ReadOnly Property userPercentMoistureInYield() As String
//    '    Get
//    '        Return describedCrops(describedIndex, 9)
//    '    End Get
//    'End Property
//    'Public ReadOnly Property userFractionResidueStanding() As Double
//    '    Get
//    '        Return describedCrops(describedIndex, 10)
//    '    End Get
//    'End Property
//    'Public ReadOnly Property userFractionResidueRemoved() As Double
//    '    Get
//    '        Return describedCrops(describedIndex, 11)
//    '    End Get
//    'End Property
//    'Public ReadOnly Property userClippingTiming() As Double
//    '    Get
//    '        Return describedCrops(describedIndex, 12)
//    '    End Get
//    'End Property
//    'Public ReadOnly Property userTranspirationMinTemperature() As Double
//    '    Get
//    '        Return describedCrops(describedIndex, 13)
//    '    End Get
//    'End Property
//    'Public ReadOnly Property userTranspirationThresholdTemperature() As Double
//    '    Get
//    '        Return describedCrops(describedIndex, 14)
//    '    End Get
//    'End Property
//    'Public ReadOnly Property userColdDamageMinTemperature() As Double
//    '    Get
//    '        Return describedCrops(describedIndex, 15)
//    '    End Get
//    'End Property
//    'Public ReadOnly Property userColdDamageThresholdTemperature() As Double
//    '    Get
//    '        Return describedCrops(describedIndex, 16)
//    '    End Get
//    'End Property
//    Public ReadOnly Property userTemperatureBase() As Double
//        Get
//            Return describedCrops(describedIndex, 17)
//        End Get
//    End Property
//    Public ReadOnly Property userTemperatureOptimum() As Double
//        Get
//            Return describedCrops(describedIndex, 18)
//        End Get
//    End Property
//    Public ReadOnly Property userTemperatureMaximum() As Double
//        Get
//            Return describedCrops(describedIndex, 19)
//        End Get
//    End Property
//    'Public ReadOnly Property userShootPartitionInitial() As Double
//    '    Get
//    '        Return describedCrops(describedIndex, 20)
//    '    End Get
//    'End Property
//    'Public ReadOnly Property userShootPartitionFinal() As Double
//    '    Get
//    '        Return describedCrops(describedIndex, 21)
//    '    End Get
//    'End Property
//    'Public ReadOnly Property userRadiationUseEfficiency() As Double
//    '    Get
//    '        Return describedCrops(describedIndex, 22)
//    '    End Get
//    'End Property
//    'Public ReadOnly Property userTranspirationUseEfficiency() As Double
//    '    Get
//    '        Return describedCrops(describedIndex, 23)
//    '    End Get
//    'End Property
//    'Public ReadOnly Property userHIx() As Double 'maximum harvest index
//    '    Get
//    '        Return describedCrops(describedIndex, 24)
//    '    End Get
//    'End Property
//    'Public ReadOnly Property userHIo() As Double 'intercept harvest index
//    '    Get
//    '        Return describedCrops(describedIndex, 25)
//    '    End Get
//    'End Property
//    'Public ReadOnly Property userHIk() As Double
//    '    Get
//    '        Return describedCrops(describedIndex, 26)
//    '    End Get
//    'End Property
//    'Public ReadOnly Property userEmergenceTT() As Double
//    '    Get
//    '        Return describedCrops(describedIndex, 27)
//    '    End Get
//    'End Property
//    'Public ReadOnly Property userNMaxConcentration() As Double
//    '    Get
//    '        Return describedCrops(describedIndex, 28)
//    '    End Get
//    'End Property
//    'Public ReadOnly Property userNDilutionSlope() As Double
//    '    Get
//    '        Return describedCrops(describedIndex, 29)
//    '    End Get
//    'End Property
//    'Public ReadOnly Property userKc() As Double
//    '    Get
//    '        Return CStr(describedCrops(describedIndex, 30))
//    '    End Get
//    'End Property
//    'Public ReadOnly Property userAnnual() As Boolean
//    '    Get
//    '        If describedCrops(describedIndex, 31) = "Annual" Then
//    '            Return True
//    '        Else
//    '            Return False
//    '        End If
//    '    End Get
//    'End Property
//    'Public ReadOnly Property userLegume() As Boolean
//    '    Get
//    '        Return describedCrops(describedIndex, 32)
//    '    End Get
//    'End Property
//    'Public ReadOnly Property userC3orC4() As Boolean
//    '    Get
//    '        If describedCrops(describedIndex, 33) = "C3" Then
//    '            Return True
//    '        Else
//    '            Return False
//    '        End If
//    '    End Get
//    'End Property
//
//    'calcluated from user data
//    Public Property calculatedFloweringTT() As Double
//        Get
//            Return describedCrops(describedIndex, 34)
//        End Get
//        Set(ByVal value As Double)
//            describedCrops(describedIndex, 34) = value
//        End Set
//    End Property
//    Public Property calculatedMaturityTT() As Double
//        Get
//            Return describedCrops(describedIndex, 35)
//        End Get
//        Set(ByVal value As Double)
//            describedCrops(describedIndex, 35) = value
//        End Set
//    End Property
//    'Public Property calculatedSimAvgYield() As Double
//    '    Get
//    '        Return describedCrops(describedIndex, 36)
//    '    End Get
//    '    Set(ByVal value As Double)
//    '        describedCrops(describedIndex, 36) = value
//    '    End Set
//    'End Property
//    'Public Property calculatedSimMaxYield() As Double
//    '    Get
//    '        Return describedCrops(describedIndex, 37)
//    '    End Get
//    '    Set(ByVal value As Double)
//    '        describedCrops(describedIndex, 37) = value
//    '    End Set
//    'End Property
//    'Public Property calculatedSimMinYield() As Double
//    '    Get
//    '        Return describedCrops(describedIndex, 38)
//    '    End Get
//    '    Set(ByVal value As Double)
//    '        describedCrops(describedIndex, 38) = value
//    '    End Set
//    'End Property
//
//    Private ReadOnly Property usesAutoIrrigation() As Boolean
//        'rotationIndex may exceed array limits, try/catch handles this scenario
//        Get
//            Try
//                Return plantingOrder(plantingIndex, 3) > -1
//                'If plantingOrder(plantingIndex, 3) > -1 Then
//                '    Return True
//                'Else
//                '    Return False
//                'End If
//            Catch ex As Exception
//                Return False
//            End Try
//        End Get
//    End Property
//    Private ReadOnly Property usesAutoFertilization() As Boolean
//        'rotationIndex may exceed array limits, try/catch handles this scenario
//        Get
//            Try
//                Return plantingOrder(plantingIndex, 4) > -1
//                'The statement above is identical to the statement below
//                'If plantingOrder(plantingIndex, 4) > -1 Then
//                '    Return True
//                'Else
//                '    Return False
//                'End If
//            Catch ex As Exception
//                Return False
//            End Try
//        End Get
//    End Property
//    Private ReadOnly Property irrigationNum() As Integer
//        Get
//            Return plantingOrder(plantingIndex, 3)
//        End Get
//    End Property
//    Private ReadOnly Property fertilizationNum() As Integer
//        Get
//            Return plantingOrder(plantingIndex, 4)
//        End Get
//    End Property
//
//    Friend Class CropClass
//        'Instance of a crop that has been planted
//        'Instance should be deleted once the crop fallow or killed
//
//        Private opList As LinkedList
//
//        Public Sub New()
//            Me.opList = New LinkedList
//        End Sub
//
//        Public Sub SelectNextCrop()
//            'Precondition:  none
//            'Postcondition: next operation in the list selected, if any
//
//            Call Me.opList.SelectNextNode()
//        End Sub
//        Public Sub SelectFirstCrop()
//            'Precondition:  none
//            'Postcondition: first operation in the list selected, if any
//
//            Call Me.opList.SelectFirstNode()
//        End Sub
//
//        Public Sub NewCrop(ByVal cropName As String)
//            'Precondition:  none
//            'Postcondition: new crop node created
//            '               new crop node is the active node
//
//            Static cropCounter As Integer = 0
//
//            Dim tempArray(81) As Object
//            Dim pos As Integer = 1
//
//            cropCounter += 1
//
//            tempArray(0) = cropCounter
//            tempArray(1) = cropName
//
//            If Crops.usesAutoIrrigation Then
//                tempArray(2) = True
//                tempArray(3) = autoIrrigation(Crops.irrigationNum, 1)
//                tempArray(4) = autoIrrigation(Crops.irrigationNum, 2)
//                tempArray(5) = autoIrrigation(Crops.irrigationNum, 3)
//                tempArray(6) = autoIrrigation(Crops.irrigationNum, 4)
//            Else
//                tempArray(2) = False
//            End If
//
//            If Crops.usesAutoFertilization Then
//                tempArray(7) = True
//                tempArray(8) = autoFertilization(Crops.fertilizationNum, 1)
//                tempArray(9) = autoFertilization(Crops.fertilizationNum, 2)
//                tempArray(10) = autoFertilization(Crops.fertilizationNum, 3)
//                tempArray(11) = autoFertilization(Crops.fertilizationNum, 4)
//                tempArray(12) = autoFertilization(Crops.fertilizationNum, 5)
//                tempArray(13) = autoFertilization(Crops.fertilizationNum, 6)
//            Else
//                tempArray(7) = False
//            End If
//
//            For i As Integer = 1 To 39
//                tempArray(40 + i) = describedCrops(describedIndex, i)
//            Next
//
//            tempArray(79) = -1
//            tempArray(80) = 0
//            tempArray(81) = Nothing
//
//            Me.opList.InsertTail(tempArray)
//            Me.opList.SelectLastNode()
//        End Sub
//        Public Sub KillCrop(ByVal uniqueIdentifier As Integer)
//            'Precondition:  uniqueIdentifier is only found once in the list
//            'Postcondition: node containing the unique value is deleted
//
//            Me.opList.DeleteNode(uniqueIdentifier)
//        End Sub
//
void SetCropStatusToMature(CropStruct Crop);
{
    /*
     * growing and mature
     */
//            Me.opList.Data(14) = True
}
//
//        Public ReadOnly Property cropUniqueIdentifier() As String
//            Get
//                Return Me.opList.Data(0)
//            End Get
//        End Property
//        Public ReadOnly Property cropName() As String
//            Get
//                Return Me.opList.Data(1)
//            End Get
//        End Property
//
//        'User Defined Auto Irrigation
//        Public ReadOnly Property autoIrrigationUsed() As Boolean
//            Get
//                Return Me.opList.Data(2)
//            End Get
//        End Property
//        Public ReadOnly Property autoIrrigationStartDay() As Integer
//            Get
//                Return Me.opList.Data(3)
//            End Get
//        End Property
//        Public ReadOnly Property autoIrrigationStopDay() As Integer
//            Get
//                Return Me.opList.Data(4)
//            End Get
//        End Property
//        Public ReadOnly Property autoIrrigationWaterDepletion() As Double
//            Get
//                Return Me.opList.Data(5)
//            End Get
//        End Property
//        Public ReadOnly Property autoIrrigationLastSoilLayer() As Integer
//            Get
//                Return Me.opList.Data(6)
//            End Get
//        End Property
//
//        'User Defined Auto Fertilization
//        Public ReadOnly Property autoFetilizationUsed() As Boolean
//            Get
//                'Return Me.opList.Data(7)
//                Return False
//            End Get
//        End Property
//        Public ReadOnly Property autoFetilizationStartDay() As Integer
//            Get
//                Return Me.opList.Data(8)
//            End Get
//        End Property
//        Public ReadOnly Property autoFetilizationStopDay() As Integer
//            Get
//                Return Me.opList.Data(9)
//            End Get
//        End Property
//        Public ReadOnly Property autoFetilizationMass() As Double
//            Get
//                Return Me.opList.Data(10)
//            End Get
//        End Property
//        Public ReadOnly Property autoFetilizationSource() As String
//            Get
//                Return Me.opList.Data(11)
//            End Get
//        End Property
//        Public ReadOnly Property autoFetilizationForm() As String
//            Get
//                Return Me.opList.Data(12)
//            End Get
//        End Property
//        Public ReadOnly Property autoFetilizationMethod() As String
//            Get
//                Return Me.opList.Data(13)
//            End Get
//        End Property
// 
//        'Crop Status Flags
//        Public ReadOnly Property cropGrowing() As Boolean
//            Get
//                If Me.opList.Data(0) <> Nothing Then Return True
//            End Get
//        End Property
//        Public ReadOnly Property cropMature() As Boolean
//            Get
//                Return Me.opList.Data(14)
//            End Get
//        End Property
//
//        'State Variables
//        Public Property svTT_Daily() As Double
//            Get
//                Return Me.opList.Data(15)
//            End Get
//            Set(ByVal value As Double)
//                Me.opList.Data(15) = value
//            End Set
//        End Property
//        Public Property svTT_Cumulative() As Double
//            Get
//                Return Me.opList.Data(16)
//            End Get
//            Set(ByVal value As Double)
//                Me.opList.Data(16) = value
//            End Set
//        End Property
//        Public Property svRadiationInterception() As Double
//            Get
//                Return Me.opList.Data(17)
//            End Get
//            Set(ByVal value As Double)
//                Me.opList.Data(17) = value
//            End Set
//        End Property
//        Public Property svBiomass() As Double
//            Get
//                Return Me.opList.Data(18)
//            End Get
//            Set(ByVal value As Double)
//                Me.opList.Data(18) = value
//            End Set
//        End Property
//        Public Property svShoot() As Double
//            Get
//                Return Me.opList.Data(19)
//            End Get
//            Set(ByVal value As Double)
//                Me.opList.Data(19) = value
//            End Set
//        End Property
//        Public Property svRoot() As Double
//            Get
//                Return Me.opList.Data(20)
//            End Get
//            Set(ByVal value As Double)
//                Me.opList.Data(20) = value
//            End Set
//        End Property
//        Public Property svRizho() As Double
//            Get
//                Return Me.opList.Data(21)
//            End Get
//            Set(ByVal value As Double)
//                Me.opList.Data(21) = value
//            End Set
//        End Property
//        Public Property svShootDailyGrowth() As Double
//            Get
//                Return Me.opList.Data(22)
//            End Get
//            Set(ByVal value As Double)
//                Me.opList.Data(22) = value
//            End Set
//        End Property
//        Public Property svRootDailyGrowth() As Double
//            Get
//                Return Me.opList.Data(23)
//            End Get
//            Set(ByVal value As Double)
//                Me.opList.Data(23) = value
//            End Set
//        End Property
//        Public Property svRizhoDailyDeposition() As Double
//            Get
//                Return Me.opList.Data(24)
//            End Get
//            Set(ByVal value As Double)
//                Me.opList.Data(24) = value
//            End Set
//        End Property
//        Public Property svUnstressedShootDailyGrowth() As Double
//            Get
//                Return Me.opList.Data(25)
//            End Get
//            Set(ByVal value As Double)
//                Me.opList.Data(25) = value
//            End Set
//        End Property
//        Public Property svUnstressedRootDailyGrowth() As Double
//            Get
//                Return Me.opList.Data(26)
//            End Get
//            Set(ByVal value As Double)
//                Me.opList.Data(26) = value
//            End Set
//        End Property
//        Public Property svPostFloweringShootBiomass() As Double
//            Get
//                Return Me.opList.Data(27)
//            End Get
//            Set(ByVal value As Double)
//                Me.opList.Data(27) = value
//            End Set
//        End Property
//        Public Property svRootingDepth() As Double
//            Get
//                Return Me.opList.Data(28)
//            End Get
//            Set(ByVal value As Double)
//                Me.opList.Data(28) = value
//            End Set
//        End Property
//        Public Property svTranspiration() As Double
//            Get
//                Return Me.opList.Data(29)
//            End Get
//            Set(ByVal value As Double)
//                Me.opList.Data(29) = value
//            End Set
//        End Property
//        Public Property svTranspirationPotential() As Double
//            Get
//                Return Me.opList.Data(30)
//            End Get
//            Set(ByVal value As Double)
//                Me.opList.Data(30) = value
//            End Set
//        End Property
//        Public Property svN_Shoot() As Double
//            Get
//                Return Me.opList.Data(31)
//            End Get
//            Set(ByVal value As Double)
//                Me.opList.Data(31) = value
//            End Set
//        End Property
//        Public Property svN_Root() As Double
//            Get
//                Return Me.opList.Data(32)
//            End Get
//            Set(ByVal value As Double)
//                Me.opList.Data(32) = value
//            End Set
//        End Property
//        Public Property svN_Rhizo() As Double
//            Get
//                Return Me.opList.Data(33)
//            End Get
//            Set(ByVal value As Double)
//                Me.opList.Data(33) = value
//            End Set
//        End Property
//        Public Property svN_RizhoDailyDeposition() As Double
//            Get
//                Return Me.opList.Data(34)
//            End Get
//            Set(ByVal value As Double)
//                Me.opList.Data(34) = value
//            End Set
//        End Property
//        Public Property svN_AutoAdded() As Double
//            Get
//                Return Me.opList.Data(35)
//            End Get
//            Set(ByVal value As Double)
//                Me.opList.Data(35) = value
//            End Set
//        End Property
//        Public Property svN_Fixation() As Double
//            Get
//                Return Me.opList.Data(36)
//            End Get
//            Set(ByVal value As Double)
//                Me.opList.Data(36) = value
//            End Set
//        End Property
//        Public Property svWaterStressFactor() As Double
//            Get
//                Return Me.opList.Data(37)
//            End Get
//            Set(ByVal value As Double)
//                Me.opList.Data(37) = value
//            End Set
//        End Property
//        Public Property svN_StressFactor() As Double
//            Get
//                Return Me.opList.Data(38)
//            End Get
//            Set(ByVal value As Double)
//                Me.opList.Data(38) = value
//            End Set
//        End Property
//       
//
//        Public Property svShootUnstressed() As Double
//            Get
//                Return Me.opList.Data(39)
//            End Get
//            Set(ByVal value As Double)
//                Me.opList.Data(39) = value
//            End Set
//        End Property
//        Public Property svN_StressCumulative() As Double
//            Get
//                Return Me.opList.Data(40)
//            End Get
//            Set(ByVal value As Double)
//                Me.opList.Data(40) = value
//            End Set
//        End Property
//
//        'User Defined Data
//        Public ReadOnly Property userSeedingDate() As String
//            Get
//                Return Me.opList.Data(41)
//            End Get
//        End Property
//        Public ReadOnly Property userFloweringDate() As String
//            Get
//                Return Me.opList.Data(42)
//            End Get
//        End Property
//        Public ReadOnly Property userMaturityDate() As String
//            Get
//                Return Me.opList.Data(43)
//            End Get
//        End Property
//        Public ReadOnly Property userMaximumSoilCoverage() As String
//            Get
//                Return Me.opList.Data(44)
//            End Get
//        End Property
//        Public ReadOnly Property userMaximumRootingDepth() As Double
//            Get
//                Return Me.opList.Data(45)
//            End Get
//        End Property
//        Public ReadOnly Property userExpectedYieldAvg() As Double
//            Get
//                Return Me.opList.Data(46)
//            End Get
//        End Property
//        Public ReadOnly Property userExpectedYieldMax() As Double
//            Get
//                Return Me.opList.Data(47)
//            End Get
//        End Property
//        Public ReadOnly Property userExpectedYieldMin() As Double
//            Get
//                Return Me.opList.Data(48)
//            End Get
//        End Property
//        Public ReadOnly Property userPercentMoistureInYield() As String
//            Get
//                Return Me.opList.Data(49)
//            End Get
//        End Property
//        Public ReadOnly Property userFractionResidueStanding() As Double
//            Get
//                Return Me.opList.Data(50)
//            End Get
//        End Property
//        Public ReadOnly Property userFractionResidueRemoved() As Double
//            Get
//                Return Me.opList.Data(51)
//            End Get
//        End Property
//        Public ReadOnly Property userClippingTiming() As Double
//            Get
//                Return Me.opList.Data(52)
//            End Get
//        End Property
//        Public ReadOnly Property userTranspirationMinTemperature() As Double
//            Get
//                Return Me.opList.Data(53)
//            End Get
//        End Property
//        Public ReadOnly Property userTranspirationThresholdTemperature() As Double
//            Get
//                Return Me.opList.Data(54)
//            End Get
//        End Property
//        Public ReadOnly Property userColdDamageMinTemperature() As Double
//            Get
//                Return Me.opList.Data(55)
//            End Get
//        End Property
//        Public ReadOnly Property userColdDamageThresholdTemperature() As Double
//            Get
//                Return Me.opList.Data(56)
//            End Get
//        End Property
//        Public ReadOnly Property userTemperatureBase() As Double
//            Get
//                Return Me.opList.Data(57)
//            End Get
//        End Property
//        Public ReadOnly Property userTemperatureOptimum() As Double
//            Get
//                Return Me.opList.Data(58)
//            End Get
//        End Property
//        Public ReadOnly Property userTemperatureMaximum() As Double
//            Get
//                Return Me.opList.Data(59)
//            End Get
//        End Property
//        Public ReadOnly Property userShootPartitionInitial() As Double
//            Get
//                Return Me.opList.Data(60)
//            End Get
//        End Property
//        Public ReadOnly Property userShootPartitionFinal() As Double
//            Get
//                Return Me.opList.Data(61)
//            End Get
//        End Property
//        Public ReadOnly Property userRadiationUseEfficiency() As Double
//            Get
//                Return Me.opList.Data(62)
//            End Get
//        End Property
//        Public ReadOnly Property userTranspirationUseEfficiency() As Double
//            Get
//                Return Me.opList.Data(63)
//            End Get
//        End Property
//        Public ReadOnly Property userHIx() As Double 'maximum harvest index
//            Get
//                Return Me.opList.Data(64)
//            End Get
//        End Property
//        Public ReadOnly Property userHIo() As Double 'intercept harvest index
//            Get
//                Return Me.opList.Data(65)
//            End Get
//        End Property
//        Public ReadOnly Property userHIk() As Double
//            Get
//                Return Me.opList.Data(66)
//            End Get
//        End Property
//        Public ReadOnly Property userEmergenceTT() As Double
//            Get
//                Return Me.opList.Data(67)
//            End Get
//        End Property
//        Public ReadOnly Property userNMaxConcentration() As Double
//            Get
//                Return Me.opList.Data(68)
//            End Get
//        End Property
//        Public ReadOnly Property userNDilutionSlope() As Double
//            Get
//                Return Me.opList.Data(69)
//            End Get
//        End Property
//        Public ReadOnly Property userKc() As Double
//            Get
//                Return Me.opList.Data(70)
//            End Get
//        End Property
//        Public ReadOnly Property userAnnual() As Boolean
//            Get
//                If Me.opList.Data(71) = "Annual" Then
//                    Return True
//                Else
//                    Return False
//                End If
//            End Get
//        End Property
//        Public ReadOnly Property userLegume() As Boolean
//            Get
//                Return Me.opList.Data(72)
//            End Get
//        End Property
//        Public ReadOnly Property userC3orC4() As Boolean
//            Get
//                If Me.opList.Data(73) = "C3" Then
//                    Return True
//                Else
//                    Return False
//                End If
//            End Get
//        End Property
//
//        'Calculated from User Defined Data
//        Public Property calculatedFloweringTT() As Double
//            Get
//                Return Me.opList.Data(74)
//            End Get
//            Set(ByVal value As Double)
//                Me.opList.Data(74) = value
//            End Set
//        End Property
//        Public Property calculatedMaturityTT() As Double
//            Get
//                Return Me.opList.Data(75)
//            End Get
//            Set(ByVal value As Double)
//                Me.opList.Data(75) = value
//            End Set
//        End Property
//        Public Property calculatedSimAvgYield() As Double
//            Get
//                Return Me.opList.Data(76)
//            End Get
//            Set(ByVal value As Double)
//                Me.opList.Data(76) = value
//            End Set
//        End Property
//        Public Property calculatedSimMaxYield() As Double
//            Get
//                Return Me.opList.Data(77)
//            End Get
//            Set(ByVal value As Double)
//                Me.opList.Data(77) = value
//            End Set
//        End Property
//        Public Property calculatedSimMinYield() As Double
//            Get
//                Return Me.opList.Data(78)
//            End Get
//            Set(ByVal value As Double)
//                Me.opList.Data(78) = value
//            End Set
//        End Property
//
//        Public Property harvestDateFinal() As Integer
//            Get
//                Return Me.opList.Data(79)
//            End Get
//            Set(ByVal value As Integer)
//                Me.opList.Data(79) = value
//            End Set
//        End Property
//        Public Property harvestCount() As Integer
//            Get
//                Return Me.opList.Data(80)
//            End Get
//            Set(ByVal value As Integer)
//                Me.opList.Data(80) = value
//            End Set
//        End Property
//        Public Property stageGrowth() As String
//            Get
//                Return Me.opList.Data(81)
//            End Get
//            Set(ByVal value As String)
//                Me.opList.Data(81) = value
//            End Set
//        End Property
//
//
//    End Class
//
//End Module
