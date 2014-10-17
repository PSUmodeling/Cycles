Option Explicit On

Friend Class SoilEvaporationClass

    Public Sub Evaporation(ByRef Soil As SoilClass, ByVal Crop As CropClass, ByVal Residue As ResidueClass, ByVal ETo As Double, ByVal SnowCover As Double)

        Dim SoilSolute As New SoilSoluteClass

        Dim EvaporativeDemand As Double                 'mm/day
        Dim WC_AirDry As Double                         'm3/m3
        Dim layerTop(Soil.totalLayers) As Double        'm
        Dim layerBottom(Soil.totalLayers) As Double     'm
        Dim layerMidpoint As Double                     'm
        Dim WaterAvailable As Double                    'layer available water for evaporation, mm
        Dim WaterSupply As Double                       'layer water supply, mm
        Dim DepthLimitation As Double                   'factor that limits evaporable water based on depth
        Dim WaterContentLimitation As Double            'factor that limits evaporable water based on WC
        Dim Evaporation As Double                       'evaporation by layer, mm
        Dim WCi() As Double = Soil.waterContent         'copy initial soil water content
        Dim EvapFlux(Soil.totalLayers) As Double        'store flux from each layer to compute solute transport

        Soil.evaporationVol = 0

        'it uses the maximum cover of either flat residues or snow to calculate radiation reaching the soil surface
        If (1 - Residue.flatResidueTau) >= SnowCover Then
            EvaporativeDemand = (1 - Residue.residueInterception) * (1 - Crop.svRadiationInterception) * ETo
        Else
            EvaporativeDemand = Residue.stanResidueTau * (1 - SnowCover) * (1 - Crop.svRadiationInterception) * ETo
        End If

        For i As Integer = 1 To Soil.totalLayers
            If i > 1 Then
                layerBottom(i) = layerBottom(i - 1) + Soil.layerThickness(i)
                layerTop(i) = layerBottom(i - 1)
            Else
                layerBottom(i) = Soil.layerThickness(i)
                layerTop(i) = 0
            End If

            layerMidpoint = 0.5 * (layerTop(i) + layerBottom(i))
            WC_AirDry = Soil.PWP(i) / 3                                         'an approximation to air dry
            WaterAvailable = (Soil.waterContent(i) - WC_AirDry) * Soil.layerThickness(i) * DailyClass.WATER_DENSITY

            DepthLimitation = 1 / 3 _
                    * (Depth_Limitation_To_Evaporation(layerTop(i)) _
                    + Depth_Limitation_To_Evaporation(layerMidpoint) _
                    + Depth_Limitation_To_Evaporation(layerBottom(i)))

            WaterContentLimitation = Water_Content_Limitation_To_Evaporation(Soil.FC(i), WC_AirDry, Soil.waterContent(i))
            WaterSupply = WaterAvailable * DepthLimitation * WaterContentLimitation

            If WaterSupply > EvaporativeDemand Then
                Evaporation = EvaporativeDemand
            Else
                Evaporation = WaterSupply
            End If

            EvaporativeDemand -= Evaporation
            Soil.evaporationVol += Evaporation
            Soil.waterContent(i) -= Evaporation / (Soil.layerThickness(i) * DailyClass.WATER_DENSITY)
            EvapFlux(i) = Evaporation

            If EvaporativeDemand = 0 Then Exit For

        Next

        'try changing retention constant to slow down nitrate upward movement, try 1 (defaul is 0 for nitrate downward in liquid flow)
        If Soil.evaporationVol > 0 Then
            Call SoilSolute.SoluteTransportEvaporation(Soil.totalLayers, 1, EvapFlux, Soil.NO3, Soil.BD, Soil.layerThickness, Soil.Porosity, WCi)
            Call SoilSolute.SoluteTransportEvaporation(Soil.totalLayers, 5.6, EvapFlux, Soil.NH4, Soil.BD, Soil.layerThickness, Soil.Porosity, WCi)
        End If

        SoilSolute = Nothing
    End Sub

    Private Function Depth_Limitation_To_Evaporation(ByVal Depth As Double) As Double
        Return 1 / (1 + (Depth / 0.04) ^ 3.5)
    End Function

    Private Function Water_Content_Limitation_To_Evaporation(ByVal FC As Double, ByVal WC_AirDry As Double, ByVal WC As Double) As Double
        Return ((WC - WC_AirDry) / (FC - WC_AirDry)) ^ 3
    End Function

End Class
