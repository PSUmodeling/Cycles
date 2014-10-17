Option Explicit On

Friend Class CropThermalTimeClass

    Public Sub ComputeThermalTime(ByVal lastDOY() As Integer, ByVal totalYears As Integer, ByRef Crop As CropClass, ByVal Weather As WeatherClass)
        'Precondition:  Weather stored previously
        '               Crop stored previously
        '               last day of year values stored previously
        '               total years calculated previously
        'Postcondition: Flowering and Maturity thermal time for each crop in the rotation is stored

        Dim sumTT, sumMTTbyYear, sumFTTbyYear As Double
        Dim cropEvents As Integer
        Dim cropON As Boolean

        Crops.SelectCropInitialPosition()

        For c As Integer = 1 To Crops.CropsPerRotation
            Crops.SelectNextCrop()

            If Crops.calculatedMaturityTT = 0 Then ' computes thermal time for 1st instance of crop in each rotation
                cropEvents = 0
                cropON = False

                For y As Integer = 1 To totalYears
                    For d As Integer = 1 To lastDOY(y)
                        If d = Crops.userSeedingDate Then cropON = True

                        If cropON Then
                            Call Weather.SelectActiveWeatherDate(y, d)

                            sumTT += 0.5 * _
                                     (ThermalTime(Crops.userTemperatureBase, Crops.userTemperatureOptimum, Crops.userTemperatureMaximum, Weather.dailyTemperatureMax) + _
                                      ThermalTime(Crops.userTemperatureBase, Crops.userTemperatureOptimum, Crops.userTemperatureMaximum, Weather.dailyTemperatureMin))

                            If d = CInt(Crops.userFloweringDate) Then sumFTTbyYear += sumTT

                            If d = CInt(Crops.userMaturityDate) Then
                                cropEvents += 1
                                sumMTTbyYear += sumTT
                                sumTT = 0
                                cropON = False
                            End If
                        End If
                    Next
                Next

                'load flow and mat TT as crop info for the crops in the rotation:
                'order of crops in array are assigned according to order in the rotation
                If cropEvents > 0 Then
                    Crops.calculatedFloweringTT = sumFTTbyYear / cropEvents
                    Crops.calculatedMaturityTT = sumMTTbyYear / cropEvents
                Else
                    Crops.calculatedFloweringTT = 0
                    Crops.calculatedMaturityTT = 0
                End If
            End If

            sumFTTbyYear = 0
            sumMTTbyYear = 0
        Next

    End Sub

    Private Function ThermalTime(ByVal T_base As Double, ByVal T_op As Double, ByVal T_Max As Double, ByVal Temperature As Double) As Double
        If Temperature <= T_base Or Temperature >= T_Max Then
            Return 0
        ElseIf Temperature < T_op Then
            Return Temperature - T_base
        Else
            Return (T_Max - Temperature) / (T_Max - T_op) * (T_op - T_base)
        End If
    End Function

End Class
