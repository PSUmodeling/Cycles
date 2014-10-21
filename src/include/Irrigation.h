Option Explicit On

Friend Module Irrigation

    Public Class FixedIrrigationClass
        Inherits FieldOperationsBaseClass

        Public Sub New()
            MyBase.New()
        End Sub

        Public Sub AddIrrigationOperation(ByVal year As Integer, ByVal day As Integer, ByVal volume As Double)
            'Precondition:  none
            'Postcondition: adds operations to OperationParameters sorted by year then day

            If year > 0 And day > 0 And day <= 365 And volume > 0 Then
                Call Me.AddOperation(year, day, volume)
            End If
        End Sub

        Public ReadOnly Property opVolume() As Double
            Get
                Return Me.opList.Data(2)
            End Get
        End Property

        'DEBUG ONLY
        Public Sub msgboxOpList()
            Dim line As String = Nothing

            If Not Me.opList.IsEmpty Then
                Call Me.SelectFirstOperation()

                Do Until Me.opYear <= 0
                    line = line & "Year: " & Format(Me.opList.Data(0), "###0") & "   " & _
                                  "Day: " & Format(Me.opList.Data(1), "###0") & "   " & _
                                  "Volume: " & Format(Me.opList.Data(2), "#0.00") & "   " & vbCr
                    SelectNextOperation()
                Loop

                MsgBox(line, MsgBoxStyle.OkOnly, "DEBUG: Fixed Irrigation")
            End If
        End Sub

    End Class

    Public Class AutoIrrigationClass

        Public Function FindIrrigationVolume(ByVal opLayer As Integer, ByVal opWaterDepletion As Double, ByVal Soil As SoilClass) As Double

            'This sub calculates the volume of an automatic irrigation
            Dim depletionZonePAW As Double = 0
            Dim depletionZoneWater As Double = 0
            Dim tempVal As Double

            If opLayer > Soil.totalLayers Then opLayer = Soil.totalLayers

            For i As Integer = 1 To opLayer
                If Soil.waterContent(i) >= Soil.PWP(i) Then
                    depletionZonePAW += Soil.PAW(i) * Soil.layerThickness(i)
                    depletionZoneWater += (Soil.waterContent(i) - Soil.PWP(i)) * Soil.layerThickness(i)
                Else
                    depletionZonePAW += (Soil.FC(i) - Soil.waterContent(i)) * Soil.layerThickness(i) 'this adds deficit below PWP
                End If
            Next

            If (depletionZoneWater + Soil.irrigationVol) < (depletionZonePAW * (1 - opWaterDepletion)) Then
                tempVal = (depletionZonePAW - depletionZoneWater) * 1000 - Soil.irrigationVol  'convert m to mm
            End If

            Return tempVal
        End Function

    End Class

End Module