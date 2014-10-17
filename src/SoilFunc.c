void SoilNew(SoilClass *Soil, int layers);
{
        Soil->totalLayers = layers;
        layers = layers + 1;

        cumulativeDepth = (double *)malloc((Soil->totalLayers + 1) * sizeof double);
        Soil->nodeDepth = (double *)malloc((layers + 1) * sizeof double);
        layerThickness = (double *)malloc((layers + 1) * sizeof double);
        Sand = (double *)malloc((layers + 1) * sizeof double);
        Clay = (double *)malloc((layers + 1) * sizeof double);
        Sand = (double *)malloc((layers + 1) * sizeof double);
        IOM = (double *)malloc((layers + 1) * sizeof double);
        NO3 = (double *)malloc((layers + 1) * sizeof double);
        NH4 = (double *)malloc((layers + 1) * sizeof double);
        BD = (double *)malloc((layers + 1) * sizeof double);
        FC = (double *)malloc((layers + 1) * sizeof double);
        PWP = (double *)malloc((layers + 1) * sizeof double);

        waterContent = (double *)malloc((layers + 1) * sizeof double);
        soilTemperature = (double *)malloc((layers + 1) * sizeof double);
        Porosity = (double *)malloc((layers + 1) * sizeof double);
        PAW = (double *)malloc((layers + 1) * sizeof double);
        FC_WaterPotential = (double *)malloc((layers + 1) * sizeof double);
        airEntryPotential = (double *)malloc((layers + 1) * sizeof double);
        B_Value = (double *)malloc((layers + 1) * sizeof double);
        M_Value = (double *)malloc((layers + 1) * sizeof double);
        SOC_Conc = (double *)malloc((layers + 1) * sizeof double);
        SOC_Mass = (double *)malloc((layers + 1) * sizeof double);
        SON_Mass = (double *)malloc((layers + 1) * sizeof double);
        MBC_Mass = (double *)malloc((layers + 1) * sizeof double);
        MBN_Mass = (double *)malloc((layers + 1) * sizeof double);
        waterUptake = (double *)malloc((layers + 1) * sizeof double);
        pH = (double *)malloc((layers + 1) * sizeof double);

        n2o = (double *)malloc((layers + 1) * sizeof double);
}


int StoreInputData(SoilClass *Soil, int manualBD; int manualFC, int manualPWP, double *LT, double *clayContent, double *sandContent, double organicMatter, double *Nitrate(), double *Ammonium, double *Bulk_Density, double *fieldCapacity, double *PermWiltingPoint, double percentSlope, double curveNumber)
{
    /*
     * Precondition:  inputs arrays must be zero based
     * Postcondition: stored arrays are 1 based
     */

    double WC33;        /* volumetric water content at 33 J/kg */
    double WC1500;      /* volumetric water content at 1500 J/kg */
    char *Location;

    Soil->Curve_Number = curveNumber;
    Soil->Percent_Slope = percentSlope;

    /*
     * Cannot remove error handling for PWP from soilclass without significant
     * code duplication as PWP is compared after calculation
     */

    Soil->cumulativeDepth[0] = 0;

    Soil->nodeDepth[0] = SideSubs.Formatting(-0.5 * Soil->layerThickness[1]);

    for (i = 0; i < Soil->totalLayers; i++)
    {
        Soil->layerThickness[i] = LT(i - 1)
            Clay(i) = clayContent(i - 1) / 100
            Sand(i) = sandContent(i - 1) / 100
            IOM(i) = organicMatter(i - 1)
            NO3(i) = Nitrate(i - 1) / 1000
            NH4(i) = Ammonium(i - 1) / 1000
            BD(i) = Bulk_Density(i - 1)
            FC(i) = fieldCapacity(i - 1)
            PWP(i) = PermWiltingPoint(i - 1)
            Soil->nodeDepth(i) = SideSubs.Formatting(cumulativeDepth(i - 1) + layerThickness(i) / 2)
            cumulativeDepth(i) = SideSubs.Formatting(cumulativeDepth(i - 1) + layerThickness(i))
    }

        Soil->nodeDepth(Soil->totalLayers + 1) = SideSubs.Formatting(cumulativeDepth(Soil->totalLayers) + layerThickness(Soil->totalLayers) / 2)

        Location = "Error: Soil Sheet"

        'computes hydraulic properties
        For i As Integer = 1 To Soil->totalLayers                                    'Uses user input to switch between user or calculated data
            If Not manualBD Then BD(i) = BulkDensity(Clay(i), Sand(i), IOM(i)) 'Bulk Density switch

            Porosity(i) = 1 - BD(i) / 2.65
            WC33 = VolumetricWCAt33Jkg(Clay(i), Sand(i), IOM(i))
            WC1500 = VolumetricWCAt1500Jkg(Clay(i), Sand(i), IOM(i))
            B_Value(i) = (Log(1500) - Log(33)) / (Log(WC33) - Log(WC1500))
            airEntryPotential(i) = -33 * (WC33 / Porosity(i)) ^ B_Value(i)
            M_Value(i) = 2 * B_Value(i) + 3

            If manualFC Then                                                'Field Capacity switch
                FC_WaterPotential(i) = SoilWaterPotential(Porosity(i), airEntryPotential(i), B_Value(i), FC(i))
            Else
                FC_WaterPotential(i) = -0.35088 * Clay(i) * 100 - 28.947
                FC(i) = SoilWaterContent(Porosity(i), airEntryPotential(i), B_Value(i), FC_WaterPotential(i))
            End If

            If Not manualPWP Then                                           'Permanent Wilting Point switch
                PWP(i) = SoilWaterContent(Porosity(i), airEntryPotential(i), B_Value(i), -1500)
            End If

            If PWP(i) >= FC(i) Then
                MsgBox("Permanent Wilting Point must be less than Field Capacity." & vbCr & "Layer " & i, vbOKOnly, Location)
                Return False
            End If

        Next

        For i As Integer = 1 To Soil->totalLayers                                 'Initializes variables dependent on previous loop
            SOC_Conc(i) = IOM(i) * 10 * 0.58
            SOC_Mass(i) = IOM(i) / 100 * 0.58 * layerThickness(i) * BD(i) * 10000
            SON_Mass(i) = SOC_Mass(i) / 10                                  'Initializes with CN ratio = 11
            MBC_Mass(i) = 0.03 * SOC_Mass(i)                                'Initializes as 3% of SOC_Mass, but "added" C
            MBN_Mass(i) = MBC_Mass(i) / 10                                  'Initializes with CN ratio = 10
            PAW(i) = FC(i) - PWP(i)
            waterContent(i) = (FC(i) + PWP(i)) / 2
        Next

        Return True
    End Function

    Public Function SoilWaterPotential(ByVal SaturationWC As Double, ByVal AirEntryPot As Double, ByVal Campbell_b As Double, ByVal WC As Double) As Double
        Return AirEntryPot * (WC / SaturationWC) ^ (-Campbell_b)
    End Function

    'Public Function XConc(ByVal NewX As Double, ByVal OldX As Double, ByVal NewMass As Double, ByVal OldMass As Double) As Double
    '    XConc = (NewX + OldX) / (NewMass + OldMass)
    'End Function

    Private Function VolumetricWCAt33Jkg(ByVal Clay As Double, ByVal Sand As Double, ByVal OM As Double) As Double
        ' Saxton and Rawls 2006 SSSAJ 70:1569-1578 Eq 2 (r2 = 0.63)
        ' clay and sand fractional, OM as % (original paper says % for everything, results make no sense)
        Dim x1 As Double

        x1 = 0.299 - 0.251 * Sand + 0.195 * Clay + 0.011 * OM + 0.006 * Sand * OM - 0.027 * Clay * OM + 0.452 * Sand * Clay
        Return -0.015 + 0.636 * x1 + 1.283 * x1 ^ 2
    End Function

    Private Function VolumetricWCAt1500Jkg(ByVal Clay As Double, ByVal Sand As Double, ByVal OM As Double) As Double
        ' Saxton and Rawls 2006 SSSAJ 70:1569-1578 Eq 1 (r2 = 0.86)
        ' clay and sand fractional, OM as % (original paper says % for everything, results make no sense)
        Dim x1 As Double

        x1 = 0.031 - 0.024 * Sand + 0.487 * Clay + 0.006 * OM + 0.005 * Sand * OM - 0.013 * Clay * OM + 0.068 * Sand * Clay
        Return -0.02 + 1.14 * x1
    End Function

    Private Function SoilWaterContent(ByVal SaturationWC As Double, ByVal AirEntryPot As Double, ByVal Campbell_b As Double, _
            ByVal Water_Potential As Double) As Double
        Return SaturationWC * (Water_Potential / AirEntryPot) ^ (-1 / Campbell_b)
    End Function

    Private Function BulkDensity(ByVal Clay As Double, ByVal Sand As Double, ByVal OM As Double) As Double
        ' Saxton and Rawls 2006 SSSAJ 70:1569-1578 Eq 6,5 (r2 = 0.29) really poor fit
        ' clay and sand fractional, OM as % (original paper says % for everything, results make no sense)
        ' note:  X2 is Eq 3, supposedly representing moisture from FC to Saturation; however, porosity is further adjusted by sand, an inconsistency
        Dim x1 As Double, x2 As Double, FC As Double, Porosity As Double

        x1 = 0.078 + 0.278 * Sand + 0.034 * Clay + 0.022 * OM - 0.018 * Sand * OM - 0.027 * Clay * OM - 0.584 * Sand * Clay
        x2 = -0.107 + 1.636 * x1
        FC = VolumetricWCAt33Jkg(Clay, Sand, OM)
        Porosity = 0.043 + FC + x2 - 0.097 * Sand
        Return (1 - Porosity) * 2.65
    End Function

End Class

