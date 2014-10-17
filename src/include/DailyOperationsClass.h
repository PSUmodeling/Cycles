Friend Class DailyClass

    Friend Const WATER_DENSITY As Integer = 1000                'kg/m3

    Public Sub DailyOperations(ByVal rotationYear As Integer, ByVal y As Integer, ByVal doy As Integer, _
                               ByRef nextSeedingYear As Integer, ByRef nextSeedingDate As Integer, ByRef AutoIrrigation As AutoIrrigationClass, ByRef Crop As CropClass, _
                               ByRef FixedFertilization As FixedFertilizationClass, ByRef FixedIrrigation As FixedIrrigationClass, _
                               ByRef RealizedCrop As RealizedCropClass, ByRef Residue As ResidueClass, ByRef SimControl As SimControlClass, _
                               ByRef Snow As SnowClass, ByRef Soil As SoilClass, ByRef SoilCarbon As SoilCarbonClass, _
                               ByRef SoilTemperature As SoilTemperatureClass, ByRef Tillage As TillageClass, ByRef Weather As WeatherClass)

        'passed classes have been initialized
 
        Dim CropHarvest As New CropHarvestClass
        Dim SoilEvaporation As New SoilEvaporationClass
        Dim SoilInfiltration As New SoilInfiltrationClass
        Dim SoilNitrogen As New SoilNitrogenClass

        Call Weather.SelectActiveWeatherDate(y, doy)

        ' If doy = 205 Then Stop

        If Crop.cropGrowing Then
            Call GrowingCrop(rotationYear, y, doy, nextSeedingYear, nextSeedingDate, Crop, CropHarvest, RealizedCrop, Residue, SimControl, Soil, SoilCarbon, Weather, Snow)
        ElseIf doy = nextSeedingDate And rotationYear = nextSeedingYear Then
            Call PlantingCrop(doy, nextSeedingYear, nextSeedingDate, Crop, RealizedCrop)
        End If

        Do While FixedFertilization.IsOperationToday(rotationYear, doy)
            Call Fertilization.ApplyFertilizer(FixedFertilization, Soil, Residue)
            Call FixedFertilization.SelectNextOperation()
        Loop

        Do While Tillage.IsOperationToday(rotationYear, doy)
            If Tillage.opToolName <> "Kill Crop" Then
                Call Tillage.ExecuteTillage(y, SoilCarbon.abgdBiomassInput, Tillage, Soil, Residue)
            ElseIf Crop.cropGrowing Then
                Call CropHarvest.KillCrop(y, doy, SimControl.simStartYear, Crop, RealizedCrop, Residue, Soil, SoilCarbon)
            End If
            Call Tillage.SelectNextOperation()
        Loop


        Call Residue.ComputeResidueCover()
        Call Tillage.TillageFactorSettling(Soil.totalLayers, Soil.waterContent, Soil.Porosity)
        Call Snow.SnowProcesses(y, doy, Weather, Residue.stanResidueTau, Crop.svRadiationInterception)
        Call SoilInfiltration.Redistribution(y, doy, Weather.dailyPrecipitation, Snow.snowFall, Snow.snowMelt, SimControl.hourlyInfiltration, Crop, Soil, Residue, FixedIrrigation, AutoIrrigation)
        Call Residue.ResidueEvaporation(Soil, Crop, Weather.dailyETref, Snow.snowCover)
        Call SoilEvaporation.Evaporation(Soil, Crop, Residue, Weather.dailyETref, Snow.snowCover)
        Call SoilTemperature.Temperature(y, doy, Snow.snowCover, Crop.svRadiationInterception, Soil, Weather, Residue)
        Call SoilCarbon.ComputeFactorComposite(doy, y, Soil)
        'Call SoilCarbon.ComputeSoilCarbonBalance(y, Residue, Soil, Tillage)
        Call SoilCarbon.ComputeSoilCarbonBalanceMB(y, Residue, Soil, Tillage)
        Call SoilNitrogen.NitrogenTransformation(Soil, Crop, Residue, Weather, SoilCarbon)

        'here is a good place to reset soil conditions based on external soil analysis
        'by calling a text or excel file likely located in same folder as input file
        'needs year, date, and soil layer much like recal in CropSyt

        CropHarvest = Nothing
        SoilEvaporation = Nothing
        SoilInfiltration = Nothing
        SoilNitrogen = Nothing
    End Sub

    'Old Version saved 14.3.25 when new Maize Silage system put in place

    'Private Sub GrowingCrop(ByVal rotationYear As Integer, ByVal y As Integer, ByVal d As Integer, _
    '                        ByRef nextSeedingYear As Integer, ByRef nextSeedingDate As Integer, ByRef Crop As CropClass, ByRef CropHarvest As CropHarvestClass, _
    '                        ByRef RealizedCrop As RealizedCropClass, ByRef Residue As ResidueClass, ByVal SimControl As SimControlClass, _
    '                        ByRef Soil As SoilClass, ByRef SoilCarbon As SoilCarbonClass, ByVal Weather As WeatherClass, ByVal Snow As SnowClass)

    '    'Precondition:  Crop is planted
    '    '               1 <= rotationYear <= rotationSize
    '    '               1 <= y <= totalYears
    '    '               1 <= d <= 366
    '    'Postcondition: Processes that only occur while a crop is growing are performed
    '    '               Any needed harvests/forages have occured
    '    '               Final Harvest date set once crop maturity achieved

    '    Dim CropProcesses As New CropProcessesClass
    '    Dim CropTranspiration As New CropTranspirationClass

    '    Dim forcedHarvest As Boolean = ForcedMaturity(rotationYear, d, nextSeedingYear, nextSeedingDate, SimControl.yearsInRotation)

    '    Select Case Crop.svTT_Cumulative
    '        Case Is < Crop.userEmergenceTT
    '            Crop.stageGrowth = "PreEmergence"

    '        Case Is < Crop.calculatedFloweringTT
    '            If Crop.userAnnual Then
    '                Crop.stageGrowth = "Vegetative Growth"
    '            Else
    '                Crop.stageGrowth = "Perennial"
    '            End If

    '        Case Is < Crop.calculatedMaturityTT
    '            If Crop.userAnnual Then
    '                Crop.stageGrowth = "Reproductive Growth"
    '            Else
    '                Crop.stageGrowth = "Perennial"
    '            End If

    '        Case Is >= Crop.calculatedMaturityTT
    '            Crop.stageGrowth = "Maturity"

    '            If Crop.harvestDateFinal < 1 Then
    '                Call Crop.SetCropStatusToMature()
    '                Call SetFinalHarvestDate(SimControl.yearSpecificLastDOY(y), d, Crop.harvestDateFinal)
    '            End If

    '    End Select

    '    Call CropProcesses.Phenology(y, d, Weather, Crop)
    '    Call CropProcesses.RadiationInterception(y, d, Weather, Crop)
    '    Call CropTranspiration.WaterUptake(Crop, Soil, Weather)
    '    Call CropProcesses.Processes(y, d, SimControl.automaticNitrogen, Crop, CropHarvest, Residue, Weather, Soil, SoilCarbon)

    '    If Weather.dailyTemperatureMin < Crop.userColdDamageThresholdTemperature Then
    '        If Crop.userAnnual And Crop.svTT_Cumulative > Crop.calculatedFloweringTT Then
    '            Call CropHarvest.GrainHarvest(y, d, SimControl.simStartYear, Crop, RealizedCrop, Residue, Soil, SoilCarbon)
    '        Else
    '            'Call CropProcesses.ComputeColdDamage(y, d, Crop, RealizedCrop, Weather, Snow, Residue)
    '        End If
    '    End If

    '    If Crop.cropUniqueIdentifier < 0 Then Stop

    '    If d = Crop.harvestDateFinal Or forcedHarvest Then
    '        If Crop.userAnnual Then
    '            Call CropHarvest.GrainHarvest(y, d, SimControl.simStartYear, Crop, RealizedCrop, Residue, Soil, SoilCarbon)
    '        Else
    '            Call CropHarvest.ForageHarvest(y, d, SimControl.simStartYear, Crop, RealizedCrop, Residue, Soil, SoilCarbon)
    '            Call CropHarvest.KillCrop(y, d, SimControl.simStartYear, Crop, RealizedCrop, Residue, Soil, SoilCarbon)
    '        End If

    '    ElseIf Crop.userClippingTiming > 0 Then
    '        If Crop.userClippingTiming <= Crop.svTT_Cumulative / Crop.calculatedMaturityTT Then
    '            If (Crop.harvestCount <= 2 And Crop.userAnnual) Or Not Crop.userAnnual Then
    '                Call CropHarvest.ForageHarvest(y, d, SimControl.simStartYear, Crop, RealizedCrop, Residue, Soil, SoilCarbon)
    '                Call RealizedCrop.AddCrop(Crop.cropUniqueIdentifier, Crop.cropName)
    '                Crop.stageGrowth = "Clipping"
    '                Crop.harvestCount += 1

    '                ' For sorghum biomass paper
    '                ' If Crop.userAnnual Then Call CropHarvest.KillCrop(y, d, SimControl.simStartYear, Crop, RealizedCrop, Residue, Soil, SoilCarbon) 'this is test
    '            End If
    '        End If

    '    End If

    '    RealizedCrop.rcCropTranspirationPotential += Crop.svTranspirationPotential
    '    RealizedCrop.rcCropTranspiration += Crop.svTranspiration
    '    RealizedCrop.rcSoilWaterEvaporation += Soil.evaporationVol

    '    CropTranspiration = Nothing
    '    CropProcesses = Nothing

    'End Sub

    Private Sub GrowingCrop(ByVal rotationYear As Integer, ByVal y As Integer, ByVal d As Integer, _
                            ByRef nextSeedingYear As Integer, ByRef nextSeedingDate As Integer, ByRef Crop As CropClass, ByRef CropHarvest As CropHarvestClass, _
                            ByRef RealizedCrop As RealizedCropClass, ByRef Residue As ResidueClass, ByVal SimControl As SimControlClass, _
                            ByRef Soil As SoilClass, ByRef SoilCarbon As SoilCarbonClass, ByVal Weather As WeatherClass, ByVal Snow As SnowClass)

        'Precondition:  Crop is planted
        '               1 <= rotationYear <= rotationSize
        '               1 <= y <= totalYears
        '               1 <= d <= 366
        'Postcondition: Processes that only occur while a crop is growing are performed
        '               Any needed harvests/forages have occured
        '               Final Harvest date set once crop maturity achieved

        Dim CropProcesses As New CropProcessesClass
        Dim CropTranspiration As New CropTranspirationClass

        Dim forcedHarvest As Boolean = ForcedMaturity(rotationYear, d, nextSeedingYear, nextSeedingDate, SimControl.yearsInRotation)

        Select Case Crop.svTT_Cumulative
            Case Is < Crop.userEmergenceTT
                Crop.stageGrowth = "PreEmergence"

            Case Is < Crop.calculatedFloweringTT
                If Crop.userAnnual Then
                    Crop.stageGrowth = "Vegetative Growth"
                Else
                    Crop.stageGrowth = "Perennial"
                End If

            Case Is < Crop.calculatedMaturityTT
                If Crop.userAnnual Then
                    Crop.stageGrowth = "Reproductive Growth"
                Else
                    Crop.stageGrowth = "Perennial"
                End If

            Case Is >= Crop.calculatedMaturityTT
                Crop.stageGrowth = "Maturity"

                If Crop.harvestDateFinal < 1 Then
                    Call Crop.SetCropStatusToMature()
                    Call SetFinalHarvestDate(SimControl.yearSpecificLastDOY(y), d, Crop.harvestDateFinal)
                End If

        End Select

        Call CropProcesses.Phenology(y, d, Weather, Crop)
        Call CropProcesses.RadiationInterception(y, d, Weather, Crop)
        Call CropTranspiration.WaterUptake(Crop, Soil, Weather)
        Call CropProcesses.Processes(y, d, SimControl.automaticNitrogen, Crop, CropHarvest, Residue, Weather, Soil, SoilCarbon)

        If Weather.dailyTemperatureMin < Crop.userColdDamageThresholdTemperature Then
            If Crop.userAnnual And Crop.svTT_Cumulative > Crop.calculatedFloweringTT Then
                Call CropHarvest.GrainHarvest(y, d, SimControl.simStartYear, Crop, RealizedCrop, Residue, Soil, SoilCarbon)
            Else
                Call CropProcesses.ComputeColdDamage(y, d, Crop, RealizedCrop, Weather, Snow, Residue)
            End If
        End If

        If Crop.cropUniqueIdentifier < 0 Then Stop

        If d = Crop.harvestDateFinal Or forcedHarvest Then
            If Crop.userAnnual Then
                Call CropHarvest.GrainHarvest(y, d, SimControl.simStartYear, Crop, RealizedCrop, Residue, Soil, SoilCarbon)
            Else
                Call CropHarvest.ForageHarvest(y, d, SimControl.simStartYear, Crop, RealizedCrop, Residue, Soil, SoilCarbon)
                Call CropHarvest.KillCrop(y, d, SimControl.simStartYear, Crop, RealizedCrop, Residue, Soil, SoilCarbon)
            End If

        ElseIf Crop.userClippingTiming > 0 Then
            If Crop.userClippingTiming <= Crop.svTT_Cumulative / Crop.calculatedMaturityTT Then
                If (Crop.harvestCount < 3 And Crop.userAnnual) Or Not Crop.userAnnual Then
                    '2014.03.25 three lines below customized for Mitch and Amanda 
                    'to allow for a unique harvest and automatic kill of maize sileage and rye bioenergy
                    If Crop.cropName = "Maize Silage MH" Or Crop.cropName = "Rye Bioenergy AR" Or Crop.cropName = "Willow AK" Then
                        Call CropHarvest.GrainHarvest(y, d, SimControl.simStartYear, Crop, RealizedCrop, Residue, Soil, SoilCarbon)
                    Else
                        Call CropHarvest.ForageHarvest(y, d, SimControl.simStartYear, Crop, RealizedCrop, Residue, Soil, SoilCarbon)
                        Call RealizedCrop.AddCrop(Crop.cropUniqueIdentifier, Crop.cropName)
                        Crop.stageGrowth = "Clipping"
                        Crop.harvestCount += 1
                    End If

                    'For sorghum biomass paper
                    'If Crop.userAnnual Then Call CropHarvest.KillCrop(y, d, SimControl.simStartYear, Crop, RealizedCrop, Residue, Soil, SoilCarbon)
                End If
            End If

        End If

        RealizedCrop.rcCropTranspirationPotential += Crop.svTranspirationPotential
        RealizedCrop.rcCropTranspiration += Crop.svTranspiration
        RealizedCrop.rcSoilWaterEvaporation += Soil.evaporationVol

        CropTranspiration = Nothing
        CropProcesses = Nothing

    End Sub


    Private Sub PlantingCrop(ByVal doy As Integer, ByRef nextSeedingYear As Integer, ByRef nextSeedingDate As Integer, ByRef Crop As CropClass, ByRef RealizedCrop As RealizedCropClass)

        'Precondition:  Crop is not growing
        'Postcondition: new realized crop is created
        '               next crop in the rotation selected
        '               status set to growing
        '               harvestDate is reset to an unreachable date
        '               dateSet is set to false

        Crops.SelectNextCrop()

        Call Crop.NewCrop(Crops.userCropName)
        Call RealizedCrop.AddCrop(Crop.cropUniqueIdentifier, Crop.cropName)

        nextSeedingDate = Crops.Peek_CropSeedingDate
        nextSeedingYear = Crops.Peek_CropSeedingYear

        Crop.stageGrowth = "Planting"
    End Sub

    Private Sub SetFinalHarvestDate(ByVal lastDoy As Integer, ByVal d As Integer, ByRef harvestDate As Integer)
        'Precondition:  1 <= d <= 366
        '               365 <= lastDoy <= 366
        'Postcondition: 1 <= harvestDate <= 366

        harvestDate = d + 10

        If harvestDate > lastDoy Then
            harvestDate -= lastDoy
        End If
    End Sub

    Private Function ForcedMaturity(ByVal rotationYear As Integer, ByVal d As Integer, ByVal nextSeedingYear As Integer, _
                                    ByVal nextSeedingDate As Integer, ByVal rotationSize As Integer) As Boolean
        'Precondition:  1 <= rotationYear <= rotationSize
        '               1 <= d <= 366
        'Postcondition: Returns true if planted crop is within saftey margin of days of the next crop to be planted

        Dim margin As Integer = 10
        Dim nextRotationYear As Integer = rotationYear

        If nextRotationYear < rotationSize Then
            nextRotationYear += 1
        Else
            nextRotationYear = 1
        End If

        If rotationYear = nextSeedingYear Then
            If d < nextSeedingDate And (d + margin) >= nextSeedingDate Then
                Return True
            End If

        ElseIf nextRotationYear = nextSeedingYear Then
            If d >= (365 + nextSeedingDate - margin) Then
                Return True
            End If

        End If

        Return False
    End Function

End Class
