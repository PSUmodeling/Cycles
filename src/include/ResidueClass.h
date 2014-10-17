Option Explicit On
Imports System.Math

Friend Class ResidueClass
    Private Const STAN_RESIDUE_SA As Double = 4                 ' standing residue area to mass ratio, m2/kg
    Private Const FLAT_RESIDUE_SA As Double = 4                 ' flat residue area to mass ratio, m2/kg
    Private Const STAN_RESIDUE_K As Double = 0.25               ' standing residue extinction coefficient
    Private Const FLAT_RESIDUE_K As Double = 1.0                ' flat residue extinction 

    Public residueInterception, _
           stanResidueTau, _
           flatResidueTau, _
           stanResidueMass, _
           flatResidueMass, _
           stanResidueN, _
           flatResidueN, _
           manureSurfaceC, _
           manureSurfaceN As Double

    Public stanResidueWater, _
           flatResidueWater As Double                           'mm of water

    Public residueAbgd(), _
           residueRt(), _
           residueRz(), _
           residueAbgdN(), _
           residueRtN(), _
           residueRzN(), _
           yearResidueBiomass(), _
           yearResidueHarvested(), _
           yearRootBiomass(), _
           yearRhizodepositionBiomass(), _
           manureC(), _
           manureN() As Double                                  'Mg/ha

    Public Sub New(ByVal totalYears As Integer, ByVal totalLayers As Integer)
        ReDim Me.residueAbgd(totalLayers)
        ReDim Me.residueRt(totalLayers)
        ReDim Me.residueRz(totalLayers)
        ReDim Me.residueAbgdN(totalLayers)
        ReDim Me.residueRtN(totalLayers)
        ReDim Me.residueRzN(totalLayers)
        ReDim Me.manureC(totalLayers)
        ReDim Me.manureN(totalLayers)

        ReDim Me.yearResidueBiomass(totalYears)
        ReDim Me.yearResidueHarvested(totalYears)
        ReDim Me.yearRootBiomass(totalYears)
        ReDim Me.yearRhizodepositionBiomass(totalYears)

        Me.residueInterception = 0
        Me.stanResidueTau = 1
        Me.flatResidueTau = 1
        Me.stanResidueMass = 0
        Me.flatResidueMass = 0
        Me.stanResidueN = 0
        Me.flatResidueN = 0
        Me.stanResidueWater = 0
        Me.flatResidueWater = 0
    End Sub

    Public Sub ComputeResidueCover()
        'Precondition:  
        'Postcondition: 

        Dim stanResidueAI As Double                             'standing residue area index, m2 residue / m2 ground
        Dim flatResidueAI As Double                             'flat residue area index, m2 residue / m2 ground

        stanResidueAI = STAN_RESIDUE_SA * stanResidueMass * 0.1 ' factor 0.1 converts from Mg/ha to kg/m2
        flatResidueAI = FLAT_RESIDUE_SA * flatResidueMass * 0.1 ' factor 0.1 converts from Mg/ha to kg/m2
        stanResidueTau = Exp(-STAN_RESIDUE_K * stanResidueAI)
        flatResidueTau = Exp(-FLAT_RESIDUE_K * flatResidueAI)
        residueInterception = (1 - stanResidueTau) + stanResidueTau * (1 - flatResidueTau)
    End Sub

    Public Sub ResidueWetting(ByRef Soil As SoilClass)
        'Precondition:  
        'Postcondition: 

        Const residueMaxWaterConcentration As Double = 3.3      'kg water / kg residue
        Dim flatResidueWaterDeficit As Double                   'mm, water need to saturate residue
        Dim standingResidueWaterDeficit As Double               'mm, water need to saturate residue
        Dim waterWettingResidue As Double                       'mm, amount of water interceptable by residue
        Dim waterRetainedResidue As Double                      'mm, water retained in residue and discounted for infiltration

        flatResidueWaterDeficit = residueMaxWaterConcentration * Me.flatResidueMass / 10 - Me.flatResidueWater   '10 converts residue from Mg/ha to kg/m2
        standingResidueWaterDeficit = residueMaxWaterConcentration * Me.stanResidueMass / 10 - Me.stanResidueWater   '10 converts residue from Mg/ha to kg/m2
        waterWettingResidue = Soil.infiltrationVol * Me.residueInterception

        'wet flat residue first
        If waterWettingResidue > flatResidueWaterDeficit Then
            Me.flatResidueWater += flatResidueWaterDeficit
            waterRetainedResidue += flatResidueWaterDeficit
            waterWettingResidue -= flatResidueWaterDeficit
        Else
            Me.flatResidueWater += waterWettingResidue
            waterRetainedResidue += waterWettingResidue
            waterWettingResidue -= waterWettingResidue
        End If

        If waterWettingResidue > standingResidueWaterDeficit Then
            Me.stanResidueWater += standingResidueWaterDeficit
            waterRetainedResidue += standingResidueWaterDeficit
            waterWettingResidue -= standingResidueWaterDeficit
        Else
            Me.stanResidueWater += waterWettingResidue
            waterRetainedResidue += waterWettingResidue
            waterWettingResidue -= waterWettingResidue
        End If

        Soil.infiltrationVol -= waterRetainedResidue
    End Sub

    Public Sub ResidueEvaporation(ByRef Soil As SoilClass, ByVal Crop As CropClass, ByVal ETo As Double, ByVal snowCover As Double)
        'Precondition:  
        'Postcondition: 

        Const residueMaxWaterConcentration As Double = 3.3      'kg water / kg residue
        Dim flatEvapFactor, standingEvapFactor, flatEvap, standingEvap, residueEvapDemand, xx As Double

        Soil.residueEvaporationVol = 0
        If Me.stanResidueWater > 0 Or Me.flatResidueWater > 0 Then

            Soil.residueEvaporationVol = 0
            residueEvapDemand = Me.residueInterception * (1 - snowCover) * (1 - Crop.svRadiationInterception) * ETo
            standingEvapFactor = (Me.stanResidueWater / (residueMaxWaterConcentration * Me.stanResidueMass / 10)) ^ 2   '10 converts residue from Mg/ha to kg/m2
            flatEvapFactor = (Me.flatResidueWater / (residueMaxWaterConcentration * Me.flatResidueMass / 10)) ^ 2               '10 converts residue from Mg/ha to kg/m2

            'dry standing residue first
            xx = residueEvapDemand * standingEvapFactor
            If Me.stanResidueWater >= xx Then
                standingEvap = xx
            Else
                standingEvap = Me.stanResidueWater
            End If
            residueEvapDemand -= standingEvap

            xx = residueEvapDemand * flatEvapFactor
            If Me.flatResidueWater >= xx Then
                flatEvap = xx
            Else
                flatEvap = Me.flatResidueWater
            End If

            Soil.residueEvaporationVol = standingEvap + flatEvap
            Me.stanResidueWater -= standingEvap
            Me.flatResidueWater -= flatEvap

        End If

    End Sub

End Class
