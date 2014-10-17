Option Explicit On
Imports System.Threading

Friend Class MainClass
    '               Author & Programmer: Armen R. Kemanian - Penn State University
    '               Author & Programmer: Claudio O. Stockle - Washington State University
    '               Programmer: Shawn Quisenberry - AgriLIFE Research (Dec/2009)

    Friend Const VERSION As String = "BETA 4.0.0"               'year, month, day, daily revision 

    Private AutoFertilization As AutoFertilizationClass
    Private AutoIrrigation As AutoIrrigationClass
    Private Crop As CropClass
    Private CropHarvest As CropHarvestClass
    Private FixedFertilization As FixedFertilizationClass
    Private FixedIrrigation As FixedIrrigationClass
    Private RealizedCrop As RealizedCropClass
    Private Residue As ResidueClass
    Private SimControl As SimControlClass
    Private Snow As SnowClass
    Private Soil As SoilClass
    Private SoilCarbon As SoilCarbonClass
    Private SoilTemperature As SoilTemperatureClass
    Private ThermalTime As CropThermalTimeClass
    Private Tillage As TillageClass
    Private Weather As WeatherClass
    Private Outputs As OutputsClass


    Friend Sub Main(ByVal myExcel As Excel.Application, ByVal myWorkbook As Excel.Workbook, Optional ByVal importedFile As String = Nothing)
        'Precondition:  Excel has been started
        '               Results workbook created
        '               Inputs are within expected value ranges

        Dim Daily As New DailyClass
        Dim backWorker As Thread = Nothing
        Dim rotationYear, nextSeedingDate, nextSeedingYear As Integer

        MainForm.RuntimeStatus("Simulation initializing")

        If Initialize(myExcel, myWorkbook, importedFile) Then
            ThermalTime.ComputeThermalTime(SimControl.yearSpecificLastDOY, SimControl.totalYears, Crop, Weather)

            Crops.SelectCropInitialPosition()

            If Crops.CropsPerRotation > 0 Then
                nextSeedingYear = Crops.Peek_CropSeedingYear
                nextSeedingDate = Crops.Peek_CropSeedingDate
            Else
                nextSeedingYear = 0
                nextSeedingDate = 0
            End If

            rotationYear = 0
            MainForm.RuntimeStatus("Simulation running    ")
            MainForm.startProgressBar(SimControl.totalYears)

            For y As Integer = 1 To SimControl.totalYears
                If rotationYear < SimControl.yearsInRotation Then
                    rotationYear += 1
                Else
                    rotationYear = 1
                End If

                Call Tillage.SelectOperationYear(rotationYear)
                Call FixedIrrigation.SelectOperationYear(rotationYear)
                Call FixedFertilization.SelectOperationYear(rotationYear)

                For i As Integer = 1 To Soil.totalLayers
                    SoilCarbon.carbonMassInitial(y, i) = Soil.SOC_Mass(i)
                    SoilCarbon.carbonMassFinal(y, i) = 0
                    SoilCarbon.annualHumifiedCarbonMass(y, i) = 0
                    SoilCarbon.annualRespiredCarbonMass(y, i) = 0
                Next

                For doy As Integer = 1 To SimControl.yearSpecificLastDOY(y)
                    Call Daily.DailyOperations(rotationYear, y, doy, nextSeedingYear, nextSeedingDate, AutoIrrigation, Crop, FixedFertilization, FixedIrrigation, RealizedCrop, Residue, SimControl, Snow, Soil, SoilCarbon, SoilTemperature, Tillage, Weather)
                    Call Outputs.StoreDailyOutputs(y, doy, SimControl.yearSpecificLastDOY(y), Crop, Soil, Residue, Weather, SoilCarbon, Snow)
                Next

                Call SoilCarbon.StoreOutput(y, Soil.totalLayers, Soil.SOC_Mass)
                Call Outputs.StoreAnnualSoilOutput(y, Soil, SoilCarbon)
                Call Outputs.StoreCarbonEvolutionOutput(y, Soil, SoilCarbon, Residue)

                Call Delay(backWorker) 'this delay let's the backworker finish the printing before starting another printing batch

                Call Outputs.CopyForPrinting()
                backWorker = New Thread(AddressOf Outputs.PrintAnnualResults) : backWorker.Start()
                Call MainForm.updateProgressBar()
            Next

            Call Delay(backWorker)

            'Call RealizedCrop.msgboxOpList()

            Call MainForm.stopProgressBar()
            MainForm.RuntimeStatus("Writing results to file")

            Call CropHarvest.SimulatedAverageYields(Crop, RealizedCrop, Residue)
            Call Outputs.PrintSummaryResults(SimControl, SoilCarbon, Residue, Soil, RealizedCrop)
        End If

        AutoFertilization = Nothing
        AutoIrrigation = Nothing
        Crop = Nothing
        CropHarvest = Nothing
        Daily = Nothing
        FixedFertilization = Nothing
        FixedIrrigation = Nothing
        Outputs = Nothing
        RealizedCrop = Nothing
        Residue = Nothing
        SimControl = Nothing
        Snow = Nothing
        Soil = Nothing
        SoilCarbon = Nothing
        SoilTemperature = Nothing
        ThermalTime = Nothing
        Tillage = Nothing
        Weather = Nothing
    End Sub

    Private Function Initialize(ByVal myExcel As Excel.Application, ByRef myWorkbook As Excel.Workbook, ByVal importedFile As String) As Boolean
        'Precondition:  Source information formatted and located properly
        '               Classes are declared
        'Postcondition: Classes initialized
        '               Source information read and stored in appropriate classes
        '               Weather file read

        Dim PMET As New ReferenceETClass

        Dim tillList(,) As Object = Nothing
        Dim fixedFertList(,) As Object = Nothing
        Dim autoFertList(,) As Object = Nothing
        Dim fixedIrrList(,) As Object = Nothing
        Dim autoIrrList(,) As Object = Nothing
        Dim plantingOrder(,) As Object = Nothing
        Dim cropDescriptions(,) As Object = Nothing
        Dim weatherFile As String = Nothing
        Dim layerThickness() As Double = Nothing
        Dim Clay() As Double = Nothing
        Dim Sand() As Double = Nothing
        Dim IOM() As Double = Nothing
        Dim NO3() As Double = Nothing
        Dim NH4() As Double = Nothing
        Dim bulkDensity() As Double = Nothing
        Dim FC() As Double = Nothing
        Dim PWP() As Double = Nothing
        Dim usingTill, usingFixedFert, usingAutoFert, usingFixedIrr, usingAutoIrr, dailyCrop, _
            dailyWater, dailySoil, dailyNitrogen, dailyWeather, dailyResidue, dailySoilCarbon, _
            annualSoil, annualProfile, seasonOuts, adjustedYields, hourlyInfil, autoNitrogen, _
            autoPhosphorus, autoSulfur, manualBD, manualFC, manualPWP As Boolean
        Dim startYear, endYear, rotationSize, totalYears, totalLayers, selectedLayer As Integer
        Dim slope, curve As Double

        'arrays are zero based
        If importedFile = Nothing Then
            Dim Import As New MainForm.formRead
            Dim Export As New ImportExport.xlsWrite(VERSION)

            Import.SimulationControls(weatherFile, startYear, endYear, rotationSize, adjustedYields, hourlyInfil, _
                                      autoNitrogen, autoPhosphorus, autoSulfur, dailyCrop, dailySoil, dailyNitrogen, _
                                      dailyWater, dailyWeather, dailyResidue, dailySoilCarbon, annualSoil, annualProfile, seasonOuts)
            Import.SoilDescription(manualBD, manualFC, manualPWP, layerThickness, Clay, Sand, IOM, NO3, NH4, _
                                   bulkDensity, FC, PWP, totalLayers, selectedLayer, slope, curve)
            Import.CropDescriptions(cropDescriptions)
            Import.PlantingOrder(plantingOrder)
            Import.Tillage(tillList, usingTill)
            Import.FixedIrrigation(fixedIrrList, usingFixedIrr)
            Import.AutoIrrigation(autoIrrList, usingAutoIrr)
            Import.FixedFertilization(fixedFertList, usingFixedFert)
            Import.AutoFertilization(autoFertList, usingAutoFert)

            Export.SimulationControls(myWorkbook, weatherFile, startYear, endYear, rotationSize, adjustedYields, hourlyInfil, _
                                      autoNitrogen, autoPhosphorus, autoSulfur, dailyCrop, dailySoil, dailyNitrogen, dailyWater, _
                                      dailyWeather, dailyResidue, dailySoilCarbon, annualSoil, annualProfile, seasonOuts)
            Export.SoilDescription(myWorkbook, manualBD, manualFC, manualPWP, layerThickness, Clay, Sand, IOM, NO3, NH4, _
                                   bulkDensity, FC, PWP, totalLayers, selectedLayer, slope, curve)
            Export.CropDescriptions(myWorkbook, cropDescriptions)
            Export.PlantingOrder(myWorkbook, plantingOrder)
            Export.Tillage(myWorkbook, tillList, usingTill)
            Export.FixedIrrigation(myWorkbook, fixedIrrList, usingFixedIrr)
            Export.AutoIrrigation(myWorkbook, autoIrrList, usingAutoIrr)
            Export.FixedFertilization(myWorkbook, fixedFertList, usingFixedFert)
            Export.AutoFertilization(myWorkbook, autoFertList, usingAutoFert)

            Import = Nothing
            Export = Nothing
        Else
            Dim Import As New ArmenCustomIO.xlsRead
            Dim Export As New ArmenCustomIO.xlsWrite(VERSION)

            If Not Import.OpenSource(myExcel, importedFile) Then
                Import = Nothing
                Export = Nothing
                Return False
            End If

            Import.SimulationControls(weatherFile, startYear, endYear, rotationSize, adjustedYields, hourlyInfil, autoNitrogen, _
                                      autoPhosphorus, autoSulfur, dailyCrop, dailySoil, dailyNitrogen, dailyWater, dailyWeather, _
                                      dailyResidue, dailySoilCarbon, annualSoil, annualProfile, seasonOuts)
            Import.SoilDescription(manualBD, manualFC, manualPWP, layerThickness, Clay, Sand, IOM, NO3, NH4, _
                                   bulkDensity, FC, PWP, totalLayers, selectedLayer, slope, curve)
            Import.CropDescriptions(cropDescriptions)
            Import.PlantingOrder(plantingOrder)
            Import.Tillage(tillList, usingTill)
            Import.FixedIrrigation(fixedIrrList, usingFixedIrr)
            Import.AutoIrrigation(autoIrrList, usingAutoIrr)
            Import.FixedFertilization(fixedFertList, usingFixedFert)
            Import.AutoFertilization(autoFertList, usingAutoFert)
            Import.CloseSource()

            Export.SimulationControls(myWorkbook, weatherFile, startYear, endYear, rotationSize, adjustedYields, hourlyInfil, _
                                      autoNitrogen, autoPhosphorus, autoSulfur, dailyCrop, dailySoil, dailyNitrogen, dailyWater, _
                                      dailyWeather, dailyResidue, dailySoilCarbon, annualSoil, annualProfile, seasonOuts)
            Export.SoilDescription(myWorkbook, manualBD, manualFC, manualPWP, layerThickness, Clay, Sand, IOM, NO3, NH4, _
                                   bulkDensity, FC, PWP, totalLayers, selectedLayer, slope, curve)
            Export.CropDescriptions(myWorkbook, cropDescriptions)
            Export.PlantingOrder(myWorkbook, plantingOrder)
            Export.Tillage(myWorkbook, tillList, usingTill)
            Export.FixedIrrigation(myWorkbook, fixedIrrList, usingFixedIrr)
            Export.AutoIrrigation(myWorkbook, autoIrrList, usingAutoIrr)
            Export.FixedFertilization(myWorkbook, fixedFertList, usingFixedFert)
            Export.AutoFertilization(myWorkbook, autoFertList, usingAutoFert)

            Import = Nothing
            Export = Nothing
        End If

        CreateResultSheets(myWorkbook, dailyCrop, dailySoil, dailyNitrogen, dailyWater, dailyWeather, dailyResidue, dailySoilCarbon, annualSoil, annualProfile, seasonOuts)

        totalYears = endYear - startYear + 1

        AutoFertilization = New AutoFertilizationClass
        AutoIrrigation = New AutoIrrigationClass
        Crop = New CropClass
        CropHarvest = New CropHarvestClass
        FixedFertilization = New FixedFertilizationClass()
        FixedIrrigation = New FixedIrrigationClass()
        RealizedCrop = New RealizedCropClass
        Residue = New ResidueClass(totalYears, selectedLayer + 1)
        SimControl = New SimControlClass(weatherFile, startYear, endYear, totalYears, rotationSize, adjustedYields, hourlyInfil, _
                                         autoNitrogen, autoPhosphorus, autoSulfur, dailyCrop, dailySoil, dailyNitrogen, dailyWater, _
                                         dailyWeather, dailyResidue, dailySoilCarbon, annualSoil, annualProfile, seasonOuts)
        Snow = New SnowClass
        Soil = New SoilClass(selectedLayer)
        SoilCarbon = New SoilCarbonClass(totalYears, selectedLayer + 1)
        SoilTemperature = New SoilTemperatureClass
        ThermalTime = New CropThermalTimeClass
        Tillage = New TillageClass(selectedLayer)
        Weather = New WeatherClass(totalYears)
        Outputs = New OutputsClass(totalYears, selectedLayer, startYear, dailyCrop, dailySoil, dailyNitrogen, dailyWater, _
                                   dailyWeather, dailyResidue, dailySoilCarbon, annualSoil, annualProfile, seasonOuts, myWorkbook)

        '========== Store parameters ==========
        Call Soil.StoreInputData(manualBD, manualFC, manualPWP, layerThickness, Clay, Sand, IOM, NO3, NH4, bulkDensity, FC, PWP, slope, curve)
        Crops.StoreInputData(plantingOrder, cropDescriptions, rotationSize, autoIrrList, usingAutoIrr, autoFertList, usingAutoFert)

        If usingTill Then
            For i As Integer = 0 To tillList.GetUpperBound(0)
                Tillage.AddTillageOperation(tillList(i, 0), tillList(i, 1), tillList(i, 2), tillList(i, 3), tillList(i, 4), tillList(i, 5))
            Next
        End If

        If usingFixedIrr Then
            For i As Integer = 0 To fixedIrrList.GetUpperBound(0)
                FixedIrrigation.AddIrrigationOperation(fixedIrrList(i, 0), fixedIrrList(i, 1), fixedIrrList(i, 2))
            Next
        End If

        If usingFixedFert Then
            For i As Integer = 0 To fixedFertList.GetUpperBound(0)
                FixedFertilization.AddFertilizerOperation(fixedFertList(i, 0), fixedFertList(i, 1), fixedFertList(i, 2), fixedFertList(i, 3), _
                                                          fixedFertList(i, 4), fixedFertList(i, 5), Math.Min(CInt(fixedFertList(i, 6)), selectedLayer), _
                                                          fixedFertList(i, 7), fixedFertList(i, 8), fixedFertList(i, 9), fixedFertList(i, 10), _
                                                          fixedFertList(i, 11), fixedFertList(i, 12), fixedFertList(i, 13), fixedFertList(i, 14), _
                                                          fixedFertList(i, 15), fixedFertList(i, 16), fixedFertList(i, 17))
            Next
        End If

        Weather.ReadExcelWeatherFile(myExcel, weatherFile, totalYears, rotationSize, startYear, endYear, SimControl.yearSpecificLastDOY)
        Weather.CalculateDerivedWeather(PMET, SimControl)

        Soil.dampingDepth = 2

        If Weather.siteLatitude >= 0 Then
            Soil.annualTemperaturePhase = 100
        Else
            Soil.annualTemperaturePhase = 280
        End If

        'initializes soil temperature in first day of simulation
        For i As Integer = 1 To (Soil.totalLayers + 1)
            Soil.soilTemperature(i) = SoilTemperature.EstimatedSoilTemperature(Soil.nodeDepth(i), 1, Weather.annualAvgTemperature, _
                                      Weather.annualAmplitude, Soil.annualTemperaturePhase, Soil.dampingDepth)
        Next

        PMET = Nothing

        Return True
    End Function

    Private Sub Delay(ByVal backworker As Thread)
        Dim dblWaitTil As Date
        Const delayTime As Double = 10

        If backworker IsNot Nothing Then
            Do While backworker.ThreadState = ThreadState.Running
                dblWaitTil = Now.AddMilliseconds(delayTime)
                Do Until Now > dblWaitTil
                    System.Windows.Forms.Application.DoEvents()
                Loop
            Loop
        End If
    End Sub

End Class
