Option Explicit On
Imports System.Math

Friend Class CropHarvestClass

    Public Sub GrainHarvest(ByRef y As Integer, ByVal doy As Integer, ByVal startYear As Integer, ByRef Crop As CropClass, _
                            ByRef RealizedCrop As RealizedCropClass, ByRef Residue As ResidueClass, ByRef Soil As SoilClass, _
                            ByRef SoilCarbon As SoilCarbonClass)

        'update roots and residue biomass at harvest

        Dim HI, NHI As Double
        Dim residueMass As Double = 0
        Dim forageYield As Double = 0
        Dim retainedResidue As Double = 0
        Dim grainNitrogenYield As Double = 0
        Dim forageNitrogenYield As Double = 0

        HI = ComputeHarvestIndex(Crop.userHIx, Crop.userHIo, Crop.userHIk, Crop.svShoot, Crop.svPostFloweringShootBiomass)
        NHI = HI ^ (1 - 2 * (1 - HI) * HI)
        residueMass = Crop.svShoot * (1 - HI)
        forageYield = residueMass * Crop.userFractionResidueRemoved
        grainNitrogenYield = Crop.svN_Shoot * NHI
        forageNitrogenYield = (Crop.svN_Shoot - grainNitrogenYield) * Crop.userFractionResidueRemoved

        retainedResidue = residueMass - forageYield
        Residue.stanResidueMass += retainedResidue * Crop.userFractionResidueStanding
        Residue.flatResidueMass += retainedResidue * (1 - Crop.userFractionResidueStanding)
        Residue.stanResidueN += (Crop.svN_Shoot - grainNitrogenYield - forageNitrogenYield) * Crop.userFractionResidueStanding
        Residue.flatResidueN += (Crop.svN_Shoot - grainNitrogenYield - forageNitrogenYield) * (1 - Crop.userFractionResidueStanding)
        Residue.stanResidueWater += retainedResidue * Crop.userFractionResidueStanding / 10 * 0.5       'assume 33% residue moisture at harvest
        Residue.flatResidueWater += retainedResidue * (1 - Crop.userFractionResidueStanding) / 10 * 0.5    'assume 33% residue moisture at harvest

        'add roots of harvested annual crop to a root residue pool in each layer
        Call DistributeRootDetritus(y, Crop.svRoot, 0, Crop.svN_Root, 0, Soil, Crop, Residue, SoilCarbon)

        'yearly output variables
        Residue.yearResidueBiomass(y) += retainedResidue
        Residue.yearRootBiomass(y) += Crop.svRoot
        Residue.yearRhizodepositionBiomass(y) += Crop.svRizho

        'season outputs
        RealizedCrop.rcBiomass = Crop.svBiomass
        RealizedCrop.rcRoot = Crop.svRoot
        RealizedCrop.rcGrainYield = Crop.svShoot * HI
        RealizedCrop.rcResidueBiomass = retainedResidue
        RealizedCrop.rcForageYield = forageYield
        RealizedCrop.rcHarvestIndex = HI
        RealizedCrop.rcTotalNitrogen = Crop.svN_Shoot + Crop.svN_Root
        RealizedCrop.rcRootNitrogen = Crop.svN_Root
        RealizedCrop.rcGrainNitrogenYield = grainNitrogenYield
        RealizedCrop.rcforageNitrogenYield = forageNitrogenYield
        RealizedCrop.rcNitrogenCumulative = Crop.svN_StressCumulative

        RealizedCrop.rcYear = y + startYear - 1
        RealizedCrop.rcDoy = doy
        RealizedCrop.rcActiveStatus = False

        Call Crop.KillCrop(Crop.cropUniqueIdentifier)
    End Sub

    Public Sub ForageHarvest(ByRef y As Integer, ByVal doy As Integer, ByVal startYear As Integer, ByRef Crop As CropClass, _
                             ByRef RealizedCrop As RealizedCropClass, ByRef Residue As ResidueClass, ByRef Soil As SoilClass, _
                             ByRef SoilCarbon As SoilCarbonClass)

        Dim forageYield As Double
        Dim residueMass As Double
        Dim rootMassDead As Double
        Dim nitrogenForageYield As Double
        Dim Residue_N_Mass As Double
        Dim Root_N_Mass_Dead As Double
        Dim nitrogenStressCumulative As Double              'stored for reporting before resetting after harvest
        Const fractionalHarvestLosses As Double = 0.05
        Dim fractionalNitrogenRemoval As Double = 1 - (1 - Crop.userFractionResidueRemoved) ^ 0.7

        'with this method, forage yield is accumulated over the life of the crop for season harvest (not per year)
        'unless a new variable is created ...
        'Fractional_Nitrogen_Removal allows for N to remain in the crop to avoid harvest-induced N stress
        'the negative effect is that the removed biomass has a lower N concentration than the remaining biomass, 
        'which is likely incorrect
        'to fix this, N should be allocated to different compartments and re-allocated after a clipping event

        nitrogenStressCumulative = Crop.svN_StressCumulative

        forageYield = Crop.svShoot * Crop.userFractionResidueRemoved * (1 - fractionalHarvestLosses)
        residueMass = Crop.svShoot * Crop.userFractionResidueRemoved * fractionalHarvestLosses
        rootMassDead = Crop.svRoot * Crop.userFractionResidueRemoved
        nitrogenForageYield = Crop.svN_Shoot * fractionalNitrogenRemoval * (1 - fractionalHarvestLosses)
        Residue_N_Mass = Crop.svN_Shoot * Crop.userFractionResidueRemoved * fractionalHarvestLosses
        Root_N_Mass_Dead = Crop.svN_Root * Crop.userFractionResidueRemoved

        'add roots of clipped crop to a root residue pool in each layer
        Call DistributeRootDetritus(y, rootMassDead, 0, Root_N_Mass_Dead, 0, Soil, Crop, Residue, SoilCarbon)

        Crop.svBiomass -= (forageYield + residueMass + rootMassDead)
        Crop.svShoot -= (forageYield + residueMass)
        Crop.svRoot -= rootMassDead
        Crop.svN_Shoot -= (nitrogenForageYield + Residue_N_Mass)
        Crop.svN_Root -= Root_N_Mass_Dead
        Crop.svN_StressCumulative *= (Crop.svTT_Cumulative - Crop.userEmergenceTT) * (1 - Crop.userFractionResidueRemoved ^ 0.75) / Crop.calculatedMaturityTT 'resetting of cumulative nitrogen stress after haverst
        Crop.svTT_Cumulative = Crop.userEmergenceTT + (Crop.svTT_Cumulative - Crop.userEmergenceTT) * (1 - Crop.userFractionResidueRemoved ^ 0.75)
        Crop.svRadiationInterception = Crop.svRadiationInterception * (1 - Crop.userFractionResidueRemoved ^ 0.75)
        Crop.svShootUnstressed = Crop.svShoot 'try this approach first

        Residue.stanResidueMass += residueMass * Crop.userFractionResidueStanding
        Residue.flatResidueMass += residueMass * (1 - Crop.userFractionResidueStanding)
        Residue.stanResidueN += Residue_N_Mass * Crop.userFractionResidueStanding
        Residue.flatResidueN += Residue_N_Mass * (1 - Crop.userFractionResidueStanding)
        Residue.stanResidueWater += residueMass * Crop.userFractionResidueStanding / 10 * 0.5          'assume 33% residue moisture at harvest
        Residue.flatResidueWater += residueMass * (1 - Crop.userFractionResidueStanding) / 10 * 0.5    'assume 33% residue moisture at harvest

        'yearly output variables
        Residue.yearResidueBiomass(y) += residueMass
        Residue.yearRootBiomass(y) += rootMassDead

        'season outputs
        RealizedCrop.rcBiomass += forageYield + residueMass + rootMassDead
        RealizedCrop.rcRoot += rootMassDead
        RealizedCrop.rcForageYield += forageYield
        RealizedCrop.rcResidueBiomass += residueMass
        RealizedCrop.rcTotalNitrogen = nitrogenForageYield + Residue_N_Mass + Root_N_Mass_Dead
        RealizedCrop.rcRootNitrogen = Root_N_Mass_Dead
        RealizedCrop.rcforageNitrogenYield = nitrogenForageYield
        RealizedCrop.rcNitrogenCumulative = nitrogenStressCumulative / Crop.userClippingTiming

        RealizedCrop.rcYear = y + startYear - 1
        RealizedCrop.rcDoy = doy

    End Sub

    Public Sub KillCrop(ByRef y As Integer, ByRef doy As Integer, ByVal startYear As Integer, ByRef Crop As CropClass, _
                        ByRef RealizedCrop As RealizedCropClass, ByRef Residue As ResidueClass, ByRef Soil As SoilClass, ByRef SoilCarbon As SoilCarbonClass)

        'Precondition:  1 <= year <= totalYears; not rotation based
        '               1 <= doy <= 366
        'Postcondition: status set to Killed
        '               Final crop values based on a killed crop performed

        Dim residueMass As Double
        Dim rootMassDead As Double
        Dim Residue_N_Mass As Double
        Dim Root_N_Mass_Dead As Double

        residueMass = Crop.svShoot
        rootMassDead = Crop.svRoot
        Residue_N_Mass = Crop.svN_Shoot
        Root_N_Mass_Dead = Crop.svN_Root

        Residue.stanResidueMass += residueMass * Crop.userFractionResidueStanding
        Residue.flatResidueMass += residueMass * (1 - Crop.userFractionResidueStanding)
        Residue.stanResidueN += Residue_N_Mass * Crop.userFractionResidueStanding
        Residue.flatResidueN += Residue_N_Mass * (1 - Crop.userFractionResidueStanding)
        Residue.stanResidueWater += residueMass * Crop.userFractionResidueStanding / 10 * 0.5       'assume 33% residue moisture at harvest
        Residue.flatResidueWater += residueMass * (1 - Crop.userFractionResidueStanding) / 10 * 0.5    'assume 33% residue moisture at harvest

        'add roots of terminated crop to a root residue pool in each layer
        Call DistributeRootDetritus(y, rootMassDead, 0, Root_N_Mass_Dead, 0, Soil, Crop, Residue, SoilCarbon)

        'yearly output variables
        Residue.yearResidueBiomass(y) += residueMass
        Residue.yearRootBiomass(y) += rootMassDead
        Residue.yearRhizodepositionBiomass(y) += Crop.svRizho

        'season outputs
        RealizedCrop.rcBiomass += Crop.svBiomass
        RealizedCrop.rcRoot += Crop.svRoot
        RealizedCrop.rcResidueBiomass += residueMass

        RealizedCrop.rcYear = y + startYear - 1
        RealizedCrop.rcDoy = doy
        RealizedCrop.rcActiveStatus = False

        Call Crop.KillCrop(Crop.cropUniqueIdentifier)
    End Sub

    Public Sub SimulatedAverageYields(ByVal Crop As CropClass, ByVal RealizedCrop As RealizedCropClass, ByRef Residue As ResidueClass)
        '1=Name, 2=SumYield, 3=Number of Times the crop is in the rotation, 4= average, 5=max, 6=min

        Dim cropYieldInformation(Crops.CropsPerRotation, 6) As Object
        Dim tempYield As Double

        Crops.SelectCropInitialPosition()

        For c As Integer = 1 To Crops.CropsPerRotation
            Crops.SelectNextCrop()
            cropYieldInformation(c, 1) = Crop.cropName
            tempYield = RealizedCrop.rcGrainYield

            cropYieldInformation(c, 2) += tempYield
            cropYieldInformation(c, 3) += 1

            If cropYieldInformation(c, 3) = 1 Then
                cropYieldInformation(c, 5) = tempYield
                cropYieldInformation(c, 6) = tempYield
            Else
                If tempYield > cropYieldInformation(c, 5) Then cropYieldInformation(c, 5) = tempYield
                If tempYield < cropYieldInformation(c, 6) Then cropYieldInformation(c, 6) = tempYield
            End If
        Next

        Crops.SelectCropInitialPosition()

        For c As Integer = 1 To Crops.CropsPerRotation
            Crops.SelectNextCrop()
            If cropYieldInformation(c, 3) > 0 Then cropYieldInformation(c, 4) = cropYieldInformation(c, 2) / cropYieldInformation(c, 3)

            Crop.calculatedSimAvgYield = cropYieldInformation(c, 4)
            Crop.calculatedSimMaxYield = cropYieldInformation(c, 5)
            Crop.calculatedSimMinYield = cropYieldInformation(c, 6)
        Next
    End Sub


 
    Public Sub DistributeRootDetritus(ByVal y As Integer, ByVal rootMass As Double, ByVal rhizoMass As Double, ByVal rootN As Double, _
    ByVal rhizoN As Double, ByRef Soil As SoilClass, ByRef Crop As CropClass, ByRef Residue As ResidueClass, _
    ByRef SoilCarbon As SoilCarbonClass)

        'this subroutine distributes dead roots and rizhodeposition in soil layer
        'the function always computes the fraction of roots based on maximum root depth for the crop
        'then, it corrects the allocation using the actual rooting depth
        'this prevents an over-representation of the roots in the top layer

        'the function is the same than that for root distribution used in water uptake, but is kept in a separate sub in case parameterization changes

        'Function distribution Root=a*exp[-b*z], with b~4
        'Integral to compute mass in each layer Root_layer = a / b * (Exp(-b * z1) - Exp(-b * z2)), where z1 and z2 are the top and bottom of layer

        Const a As Double = 1
        Const b As Double = 4 'units of 1/m
        Dim rootIntegral As Double = 0
        Dim rootSum As Double

        Dim cumulativeRootingDepth As Double = 0
        Dim z1, z2 As Double
        Dim j As Integer = 0

        Dim rootDistribution(Soil.totalLayers) As Double
        Dim fractionRootsByLayer(Soil.totalLayers) As Double

        rootIntegral = a / b * (Exp(-b * 0) - Exp(-b * Crop.userMaximumRootingDepth))
        While cumulativeRootingDepth < Crop.svRootingDepth And j < Soil.totalLayers
            j += 1

            Select Case Soil.cumulativeDepth(j)
                Case Is < Crop.svRootingDepth
                    cumulativeRootingDepth = Soil.cumulativeDepth(j)
                Case Is >= Crop.svRootingDepth
                    cumulativeRootingDepth = Crop.svRootingDepth
            End Select

            z1 = Soil.cumulativeDepth(j - 1)
            z2 = cumulativeRootingDepth
            rootDistribution(j) = ((a / b) * (Exp(-b * z1) - Exp(-b * z2))) / rootIntegral
        End While



        'ensures that cumulative fractional root distribution = 1    
        rootSum = rootDistribution.Sum

        'compute input of biomass from roots to each layer
        For i As Integer = 1 To j  'exits loop on the same layer as the previous loop
            If rootMass > 0 Then
                fractionRootsByLayer(i) = rootDistribution(i) / rootSum
                Residue.residueRt(i) += fractionRootsByLayer(i) * rootMass
                Residue.residueRtN(i) += fractionRootsByLayer(i) * rootN
                SoilCarbon.rootBiomassInput(y, i) += fractionRootsByLayer(i) * rootMass
            End If

            If rhizoMass > 0 Then
                Residue.residueRz(i) += fractionRootsByLayer(i) * rhizoMass
                Residue.residueRzN(i) += fractionRootsByLayer(i) * rhizoN
                SoilCarbon.rhizBiomassInput(y, i) += fractionRootsByLayer(i) * rhizoMass
            End If
        Next
    End Sub


    'Public Sub DistributeRootDetritus(ByVal y As Integer, ByVal rootMass As Double, ByVal rhizoMass As Double, ByVal rootN As Double, _
    '                               ByVal rhizoN As Double, ByRef Soil As SoilClass, ByRef Crop As CropClass, ByRef Residue As ResidueClass, _
    '                               ByRef SoilCarbon As SoilCarbonClass)

    '    'REPLACED 04 October 2013

    '    'this subrouting distributes dead roots and rizhodeposition in soil layer
    '    'the normalized function always computes the fraction of roots based on maximum root depth for the crop
    '    'then, it corrects the allocation using the actual rooting depth
    '    'this prevents and over-representation of the roots in the top layer
    '    'when rizhodeposition or dead roots are added to the soil layers

    '    Dim cumulativeRootingDepth As Double = 0
    '    Dim fractionRootsByLayer(Soil.totalLayers) As Double
    '    Dim x1, x2 As Double
    '    Dim j As Integer = 0
    '    Dim fractionSUM As Double 'root fraction sum, used to secure that fractionSUM=1

    '    'Normalized Root Density = 1.15*exp[-11.7*NRD] + 0.022, where NRD = normalized rooting depth
    '    'Parameters of Normalized Root Density Function from Dwyer et al 19xx
    '    Const a As Double = 1.15
    '    Const b As Double = 11.7
    '    Const c As Double = 0.022
    '    Const d As Double = 0.12029 'integral of Normalized Root Distribution Function from 0 to 1 (normalized depth) = 0.12029

    '    'compute input of biomass from roots to each layer
    '    While cumulativeRootingDepth < Crop.svRootingDepth And j < Soil.totalLayers
    '        j += 1

    '        Select Case Soil.cumulativeDepth(j)
    '            Case Is < Crop.svRootingDepth
    '                cumulativeRootingDepth = Soil.cumulativeDepth(j)

    '            Case Is >= Crop.svRootingDepth
    '                cumulativeRootingDepth = Crop.svRootingDepth
    '        End Select

    '        'x1 and x2 represent the top and bottom of the soil layer i
    '        x1 = (cumulativeRootingDepth - Soil.layerThickness(j)) / Crop.userMaximumRootingDepth
    '        x2 = cumulativeRootingDepth / Crop.userMaximumRootingDepth
    '        fractionRootsByLayer(j) = (a / b * (Exp(-b * x1) - Exp(-b * x2)) + c * (x2 - x1)) / d
    '    End While

    '    fractionSUM = fractionRootsByLayer.Sum

    '    'ensures that cumulative fractional root distribution = 1
    '    For i As Integer = 1 To j  'exits loop on the same layer as the previous loop
    '        If rootMass > 0 Then
    '            fractionRootsByLayer(i) /= fractionSUM
    '            Residue.residueRt(i) += fractionRootsByLayer(i) * rootMass
    '            Residue.residueRtN(i) += fractionRootsByLayer(i) * rootN
    '            SoilCarbon.rootBiomassInput(y, i) += fractionRootsByLayer(i) * rootMass
    '        End If

    '        If rhizoMass > 0 Then
    '            Residue.residueRz(i) += fractionRootsByLayer(i) * rhizoMass
    '            Residue.residueRzN(i) += fractionRootsByLayer(i) * rhizoN
    '            SoilCarbon.rhizBiomassInput(y, i) += fractionRootsByLayer(i) * rhizoMass
    '        End If
    '    Next
    'End Sub


    Private Function ComputeHarvestIndex(ByVal HIx As Double, ByVal HIo As Double, ByVal HIk As Double, _
                                         ByVal cumulativeShoot As Double, ByVal cumulativePostFloweringShootBiomass As Double) As Double

        'update roots and residue biomass at harvest:

        Dim fg As Double                    ' fractional post-anthesis growth

        If cumulativePostFloweringShootBiomass > 0 And cumulativeShoot > 0 Then
            fg = cumulativePostFloweringShootBiomass / cumulativeShoot
            Return HIx - (HIx - HIo) * Exp(-HIk * fg)
        Else
            Return 0
        End If
    End Function

End Class
