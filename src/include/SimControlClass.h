Friend Class SimControlClass
    Public yearSpecificLastDOY() As Integer

    Private Weather_File As String                                   'Path and filename of weather file
    Private Sim_Start_Year, _
            Sim_End_Year, _
            Total_Years, _
            Years_in_Rotation As Integer
    Private Adjusted_Yields, _
            Hourly_Infiltration, _
            Automatic_Nitrogen, _
            Automatic_Phosphorus, _
            Automatic_Sulfur, _
            Crop_Daily_Output, _
            Soil_Daily_Output, _
            Nitrogen_Daily_Output, _
            Water_Daily_Output, _
            Weather_Daily_Output, _
            Residue_Daily_Output, _
            SoilCarbon_Daily_Output, _
            Annual_Soil_Output, _
            Profile_Output, _
            Season_Output As Boolean

    Public Sub New(ByVal weatherFilename As String, ByVal startYear As Integer, ByVal endYear As Integer, _
                   ByVal totalSimYears As Integer, ByVal rotationSize As Integer, ByVal adjustedYield As Boolean, _
                   ByVal hourlyInfil As Boolean, ByVal autoNitrogen As Boolean, ByVal autoPhosphorus As Boolean, _
                   ByVal autoSulfur As Boolean, ByVal dailyCrop As Boolean, ByVal dailySoil As Boolean, _
                   ByVal dailyNitrogen As Boolean, ByVal dailyWater As Boolean, ByVal dailyWeather As Boolean, _
                   ByVal dailyResidue As Boolean, ByVal dailySoilCarbon As Boolean, ByVal annualSoil As Boolean, _
                   ByVal annualSoilProfile As Boolean, ByVal seasonSummary As Boolean)

        ReDim yearSpecificLastDOY(totalSimYears)

        Sim_Start_Year = startYear
        Sim_End_Year = endYear
        Total_Years = totalSimYears
        Years_in_Rotation = rotationSize
        Adjusted_Yields = adjustedYield
        Hourly_Infiltration = hourlyInfil
        Automatic_Nitrogen = autoNitrogen
        Automatic_Phosphorus = autoPhosphorus
        Automatic_Sulfur = autoSulfur
        Weather_File = weatherFilename
        Crop_Daily_Output = dailyCrop
        Soil_Daily_Output = dailySoil
        Nitrogen_Daily_Output = dailySoil
        Water_Daily_Output = dailySoil
        Weather_Daily_Output = dailyWeather
        SoilCarbon_Daily_Output = dailySoilCarbon
        Residue_Daily_Output = dailyResidue
        Annual_Soil_Output = annualSoil
        Profile_Output = annualSoilProfile
        Season_Output = seasonSummary
    End Sub

    'Simulation Controls
    Public ReadOnly Property simStartYear() As Integer
        Get
            Return Me.Sim_Start_Year
        End Get
    End Property
    Public ReadOnly Property simEndYear() As Integer
        Get
            Return Me.Sim_End_Year
        End Get
    End Property
    Public ReadOnly Property totalYears() As Integer
        Get
            Return Me.Total_Years
        End Get
    End Property
    Public ReadOnly Property yearsInRotation() As Integer
        Get
            Return Me.Years_in_Rotation
        End Get
    End Property
    Public ReadOnly Property adjustedYields() As Boolean
        Get
            Return Me.Adjusted_Yields
        End Get
    End Property
    Public ReadOnly Property hourlyInfiltration() As Boolean
        Get
            Return Me.Hourly_Infiltration
        End Get
    End Property
    Public ReadOnly Property automaticNitrogen() As Boolean
        Get
            Return Me.Automatic_Nitrogen
        End Get
    End Property
    Public ReadOnly Property automaticPhosphorus() As Boolean
        Get
            Return Me.Automatic_Phosphorus
        End Get
    End Property
    Public ReadOnly Property automaticSulfur() As Boolean
        Get
            Return Me.Automatic_Sulfur
        End Get
    End Property
    Public ReadOnly Property weatherFile() As String
        Get
            Return Me.Weather_File
        End Get
    End Property
    Public ReadOnly Property cropDailyOutput() As Boolean
        Get
            Return Me.Crop_Daily_Output
        End Get
    End Property
    Public ReadOnly Property soilDailyOutput() As Boolean
        Get
            Return Me.Soil_Daily_Output
        End Get
    End Property
    Public ReadOnly Property nitrogenDailyOutput() As Boolean
        Get
            Return Me.Nitrogen_Daily_Output
        End Get
    End Property
    Public ReadOnly Property waterDailyOutput() As Boolean
        Get
            Return Me.Water_Daily_Output
        End Get
    End Property
    Public ReadOnly Property weatherDailyOutput() As Boolean
        Get
            Return Me.Weather_Daily_Output
        End Get
    End Property
    Public ReadOnly Property residueDailyOutput() As Boolean
        Get
            Return Me.Residue_Daily_Output
        End Get
    End Property
    Public ReadOnly Property soilCarbonDailyOutput() As Boolean
        Get
            Return Me.SoilCarbon_Daily_Output
        End Get
    End Property
    Public ReadOnly Property annualSoilOutput() As Boolean
        Get
            Return Me.Annual_Soil_Output
        End Get
    End Property
    Public ReadOnly Property profileOutput() As Boolean
        Get
            Return Me.Profile_Output
        End Get
    End Property
    Public ReadOnly Property seasonOutput() As Boolean
        Get
            Return Me.Season_Output
        End Get
    End Property

End Class
