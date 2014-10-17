Option Explicit On
Imports System.Math

Friend Class WeatherClass
    Private ReadOnly location As String = MainForm.ProductName & ": Weather File"

    Private yr, _
            dy As Integer                       'internal index

    Private locationAltitude, _
            locationLatitude, _
            screeningHeight, _
            yearlyAmplitude(), _
            annualAverageTemperature(), _
            wind(,), _
            ETref(,), _
            precipitation(,), _
            RHmax(,), _
            RHmin(,), _
            solarRadiation(,), _
            tMax(,), _
            tMin(,), _
            pAtm As Double

    Public Sub New(ByVal totalYears As Integer)
        ReDim Me.wind(totalYears, 366)
        ReDim Me.annualAverageTemperature(totalYears)
        ReDim Me.ETref(totalYears, 366)
        ReDim Me.precipitation(totalYears, 366)
        ReDim Me.RHmax(totalYears, 366)
        ReDim Me.RHmin(totalYears, 366)
        ReDim Me.solarRadiation(totalYears, 366)
        ReDim Me.tMax(totalYears, 366)
        ReDim Me.tMin(totalYears, 366)
        ReDim Me.yearlyAmplitude(totalYears)
    End Sub

    Public Sub SelectActiveWeatherDate(ByVal year As Integer, ByVal doy As Integer)
        If year > 0 And (doy > 0 And doy < 367) Then
            Me.yr = year
            Me.dy = doy
        Else
            MsgBox("Passed values exceed tolerances" & vbCr & _
                   "Year: " & year & vbCr & "Day: " & doy, MsgBoxStyle.Critical, location)
        End If
    End Sub

    Public Function ReadExcelWeatherFile(ByRef myExcel As Excel.Application, ByVal weatherFile As String, ByVal totalYears As Integer, _
                                         ByVal yearsInRotation As Integer, ByVal simStartYear As Integer, ByVal simEndYear As Integer, _
                                         ByRef lastDOY() As Integer) As Boolean

        Dim y, d, Day, Next_Day, year, Next_Year As Integer
        Dim noError As Boolean = True
        Dim Found As Boolean
        Dim Error_Message As String
        Dim Sheet As Excel.Worksheet
        Dim startRow As Integer = 7
        Dim dateArray(,), valueArray(,) As Object
        Dim weatherBook As Excel.Workbook

        ReDim dateArray(0, 0)
        ReDim valueArray(0, 0)

        ReDim precipitation(totalYears, 366)
        ReDim tMax(totalYears, 366)
        ReDim tMin(totalYears, 366)
        ReDim solarRadiation(totalYears, 366)
        ReDim RHmax(totalYears, 366)
        ReDim RHmin(totalYears, 366)
        ReDim wind(totalYears, 366)

        Try
            weatherBook = myExcel.Workbooks.Open(weatherFile) 'open spreadsheet from disk
        Catch ex As Exception
            MsgBox(Err.Description, MsgBoxStyle.Exclamation, location)
            Return False
        End Try

        Sheet = weatherBook.Sheets.Item("Weather") 'get 1st sheet from workbook
        Error_Message = "Weather file contains improperly formatted data." & vbCr & vbCr
        Found = False

        Me.locationLatitude = Sheet.Range("C2").Value
        Me.locationAltitude = Sheet.Range("E2").Value
        Me.screeningHeight = Sheet.Range("G2").Value

        Do  'Finds Simulation_Start_Year/Day
            dateArray = Sheet.Range(Sheet.Cells(startRow, 1), Sheet.Cells(startRow + 366, 2)).Value

            For d = 1 To 366
                If dateArray(d, 1) = simStartYear And dateArray(d, 2) = 1 Then
                    Exit Do
                Else
                    startRow = startRow + 1
                End If
            Next d
        Loop Until Sheet.Cells(startRow, 1).value = Nothing

        If Sheet.Cells(startRow, 1).value <> Nothing Then    'if a blank cell in the year position was not encountered...
            Do  'Begin Weatherfile error checking
                dateArray = Sheet.Range(Sheet.Cells(startRow, 1), Sheet.Cells(startRow + 366, 2)).Value
                valueArray = Sheet.Range(Sheet.Cells(startRow, 3), Sheet.Cells(startRow + 366, 9)).Value

                For d = 1 To 366
                    Day = dateArray(d, 2)

                    If Day = 1 Then
                        year = dateArray(d, 1)
                        y = year - simStartYear + 1
                    End If

                    Next_Day = dateArray(d + 1, 2)
                    Next_Year = dateArray(d + 1, 1)

                    precipitation(y, Day) = valueArray(d, 1)                 'Precipitation
                    tMax(y, Day) = valueArray(d, 2)                          'Temperature Max
                    tMin(y, Day) = valueArray(d, 3)                          'Temperature Min
                    solarRadiation(y, Day) = valueArray(d, 4)                'Solar Radiation
                    RHmax(y, Day) = valueArray(d, 5)                         'Relative Humidity Max
                    RHmin(y, Day) = valueArray(d, 6)                         'Relative Humidity Min
                    wind(y, Day) = valueArray(d, 7)                          'Wind Speed

                    If precipitation(y, Day) < 0 Or precipitation(y, Day) > 1000 Then Found = True '  0 =< x >= 1000
                    If tMax(y, Day) < -50 Or tMax(y, Day) > 50 Then Found = True '-50 =< x >= 50
                    If tMin(y, Day) < -50 Or tMin(y, Day) > 50 Or tMin(y, Day) > tMax(y, Day) Then Found = True '-50 =< x >= 50; x =< y
                    If solarRadiation(y, Day) < 0 Or solarRadiation(y, Day) > 50 Then Found = True '  0 =< x >= 50
                    If RHmax(y, Day) < 0 Or RHmax(y, Day) > 105 Then Found = True '  0 =< x >= 105
                    If RHmin(y, Day) < 0 Or RHmin(y, Day) > 105 Or RHmin(y, Day) > RHmax(y, Day) Then Found = True '  0 =< x >= 105; x =< y
                    If wind(y, Day) < 0 Or wind(y, Day) > 110 Then Found = True '  0 =< x >= 110

                    'day = 1-364 or 1-365 with 366 valid, compare current day to next day
                    If (Day >= 1 And Day < 365) Or (Day = 365 And Next_Day = 366) Then
                        'year cannot change, next day should be sequential
                        If (year <> Next_Year) Or (Day + 1 <> Next_Day) Then
                            Found = True
                            Exit Do
                        End If

                        'day is last day of year (365 or 366), compare current day to next day
                    ElseIf (Day = 365 And Next_Day <> 366) Or (Day = 366) Then
                        'not end of file
                        If Next_Day <> Nothing And Next_Year <> Nothing Then
                            'next year is sequential and next day resets to 1
                            If Next_Day = 1 And Next_Year = year + 1 Then
                                lastDOY(y) = Day
                                Exit For
                                'next year is not sequential
                            ElseIf (Next_Year <> year + 1) Or (Next_Day <> 1) Then
                                Found = True
                                Exit Do
                            End If
                            'end of file
                        ElseIf Next_Day = Nothing And Next_Year = Nothing Then
                            lastDOY(y) = Day
                            Exit For
                        End If

                        'Day < 1 or Next_Day > 366
                    Else
                        Found = True
                        Exit Do
                    End If
                Next d

                startRow = startRow + d
            Loop Until Next_Year > simEndYear Or Next_Year = Nothing
        Else
            MsgBox("Data missing or corrupted on or after row " & d & ".", MsgBoxStyle.Exclamation, location)
            Return False
        End If

        If noError Then
            If Found Then
                MsgBox("Data missing or corrupted on or after row " & d & ".", MsgBoxStyle.Exclamation, location)
                Return False
            End If
        End If

        weatherBook.Close()
        Return noError
    End Function

    Public Function SatVP(ByVal T As Double) As Double
        Return 0.6108 * Exp(17.27 * T / (T + 237.3))
    End Function

    Public Sub CalculateDerivedWeather(ByRef PMET As ReferenceETClass, ByVal SimControl As SimControlClass)
        Dim annualMaxTemperatureSum As Double
        Dim annualMinTemperatureSum As Double

        Me.pAtm = 101.325 * Exp(-Me.locationAltitude / 8200) 'P = 101.3 * ((293 - 0.0065 * Altitude) / 293) ^ 5.26

        For y As Integer = 1 To SimControl.totalYears       'Initialize yearly information for soil temperature
            annualMaxTemperatureSum = 0
            annualMinTemperatureSum = 0

            For d As Integer = 1 To SimControl.yearSpecificLastDOY(y)
                Me.ETref(y, d) = PMET.CalculatePMET(Me.locationLatitude, Me.pAtm, Me.screeningHeight, Me.tMax(y, d), _
                                                 Me.tMin(y, d), Me.solarRadiation(y, d), Me.RHmax(y, d), Me.RHmin(y, d), Me.wind(y, d), d)
                annualMaxTemperatureSum += Me.tMax(y, d)
                annualMinTemperatureSum += Me.tMin(y, d)
            Next

            Me.annualAverageTemperature(y) = (annualMaxTemperatureSum + annualMinTemperatureSum) / (SimControl.yearSpecificLastDOY(y) * 2)
            Me.yearlyAmplitude(y) = (annualMaxTemperatureSum - annualMinTemperatureSum) / SimControl.yearSpecificLastDOY(y)
        Next
    End Sub

    Public ReadOnly Property annualAvgTemperature() As Double
        Get
            Return Me.annualAverageTemperature(Me.yr)
        End Get
    End Property
    Public ReadOnly Property annualAmplitude() As Double
        Get
            Return Me.yearlyAmplitude(Me.yr)
        End Get
    End Property

    Public ReadOnly Property atmosphericPressure() As Double
        Get
            Return Me.pAtm
        End Get
    End Property
    Public ReadOnly Property siteLatitude() As Double
        Get
            Return Me.locationLatitude
        End Get
    End Property

    Public ReadOnly Property dailyETref() As Double
        Get
            Return Me.ETref(Me.yr, Me.dy)
        End Get
    End Property
    Public ReadOnly Property dailyPrecipitation() As Double
        Get
            Return Me.precipitation(Me.yr, Me.dy)
        End Get
    End Property
    Public ReadOnly Property dailyRelativeHumidityMax() As Double
        Get
            Return Me.RHmax(Me.yr, Me.dy)
        End Get
    End Property
    Public ReadOnly Property dailyRelativeHumidityMin() As Double
        Get
            Return Me.RHmin(Me.yr, Me.dy)
        End Get
    End Property
    Public ReadOnly Property dailySolarRadiation() As Double
        Get
            Return Me.solarRadiation(Me.yr, Me.dy)
        End Get
    End Property
    Public ReadOnly Property dailyTemperatureMax() As Double
        Get
            Return Me.tMax(Me.yr, Me.dy)
        End Get
    End Property
    Public ReadOnly Property dailyTemperatureMin() As Double
        Get
            Return Me.tMin(Me.yr, Me.dy)
        End Get
    End Property
    Public ReadOnly Property dailyWindSpeed() As Double
        Get
            Return Me.wind(Me.yr, Me.dy)
        End Get
    End Property

    Public ReadOnly Property currentDoy() As Integer
        Get
            Return Me.dy
        End Get
    End Property
    Public ReadOnly Property currentYear() As Integer
        Get
            Return Me.yr
        End Get
    End Property

End Class
