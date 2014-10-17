Option Explicit On
Imports System.Math

Friend Class SoilCarbonClass
    Private Const MAXIMUM_UNDISTURBED_SOC_DECOMPOSITION_RATE As Double = 0.00015  '(1 + 0.056) ^ (1 / 365) - 1  ' 1/day (1/5 for Urbana)
    Private Const MAXIMUM_RESIDUE_DECOMPOSITION_RATE As Double = 0.05                  ' 1/day
    Private Const MAXIMUM_ROOT_DECOMPOSITION_RATE As Double = 0.05                     ' 1/day
    Private Const MAXIMUM_RHIZO_DECOMPOSITION_RATE As Double = 0.1                     ' 1/day
    Private Const MAXIMUM_MANURE_DECOMPOSITION_RATE As Double = 0.05                   ' 1/day
    Private Const MAXIMUM_MICROBIAL_DECOMPOSITION_RATE As Double = 1                   ' (calculated internaly) 1/day

    Private Const FRACTION_CARBON_PLANT As Double = 0.43
    Private Const FRACTION_CARBON_RIZHO As Double = 0.43
    Private Const FRACTION_CARBON_MANURE As Double = 0.4

    Private Const SOC_DECOMPOSITION_POWER As Double = 0.5
    Private Const SOC_HUMIFICATION_POWER As Double = 6

    Public factorComposite(), _
           carbonRespired(), _
           rootBiomassInput(,), _
           rhizBiomassInput(,), _
           abgdBiomassInput(,), _
           rootCarbonInput(,), _
           rhizCarbonInput(,), _
           manuCarbonInput(,), _
           abgdCarbonInput(,), _
           carbonMassInitial(,), _
           carbonMassFinal(,), _
           annualDecompositionFactor(,), _
           annualSoilCarbonDecompositionRate(,), _
           annualCarbonInputByLayer(,), _
           annualHumifiedCarbonMass(,), _
           annualRespiredCarbonMass(,), _
           annualRespiredResidueCarbonMass(,), _
           annualHumificationCoefficient(,) As Double

    Public Sub New(ByVal totalYears As Integer, ByVal totalLayers As Integer)
        ReDim factorComposite(totalLayers)
        ReDim carbonRespired(totalLayers)
        ReDim rootBiomassInput(totalYears, totalLayers)
        ReDim rhizBiomassInput(totalYears, totalLayers)
        ReDim abgdBiomassInput(totalYears, totalLayers)
        ReDim rootCarbonInput(totalYears, totalLayers)
        ReDim rhizCarbonInput(totalYears, totalLayers)
        ReDim manuCarbonInput(totalYears, totalLayers)
        ReDim abgdCarbonInput(totalYears, totalLayers)
        ReDim carbonMassInitial(totalYears, totalLayers)
        ReDim carbonMassFinal(totalYears, totalLayers)
        ReDim annualDecompositionFactor(totalYears, totalLayers)
        ReDim annualSoilCarbonDecompositionRate(totalYears, totalLayers)
        ReDim annualCarbonInputByLayer(totalYears, totalLayers)
        ReDim annualHumifiedCarbonMass(totalYears, totalLayers)
        ReDim annualRespiredCarbonMass(totalYears, totalLayers)
        ReDim annualRespiredResidueCarbonMass(totalYears, totalLayers)
        ReDim annualHumificationCoefficient(totalYears, totalLayers)
    End Sub

    Public Sub ComputeFactorComposite(ByVal doy As Integer, ByVal y As Integer, ByVal Soil As SoilClass)

        Static avg(Soil.totalLayers) As Double
        Dim waterPotential As Double        'J/kg
        Dim airContent As Double            'm3/m3
        Dim factorMoisture As Double        'unitless
        Dim factorTemperature As Double     'unitless
        Dim factorAeration As Double = 1    'cumulative aeration factor accounts empirically for air content of layers above that considered in the calculations

        For i As Integer = 1 To Soil.totalLayers
            waterPotential = Soil.SoilWaterPotential(Soil.Porosity(i), Soil.airEntryPotential(i), Soil.B_Value(i), Soil.waterContent(i))
            airContent = Soil.Porosity(i) - Soil.waterContent(i)
            factorMoisture = Moisture(waterPotential)
            factorTemperature = TemperatureFunction(Soil.soilTemperature(i))
            factorAeration *= Aeration(airContent)
            factorComposite(i) = factorMoisture * factorAeration * factorTemperature

            avg(i) += factorComposite(i)

            If doy = 365 Then
                annualDecompositionFactor(y, i) = avg(i) / 365
                avg(i) = 0
            End If
        Next
    End Sub

    Public Sub ComputeSoilCarbonBalanceMB(ByRef y As Integer, ByRef Residue As ResidueClass, ByRef Soil As SoilClass, ByRef Tillage As TillageClass)

        Dim socDecompositionRate, _
            micrDecompositionRate, _
            humifiedCarbon, _
            humifiedNitrogen As Double

        Dim abgdHumificationFactor, _
            rootHumificationFactor, _
            rhizHumificationFactor, _
            manuHumificationFactor, _
            micrHumificationFactor As Double

        Dim soilMass, _
            satSOCConc As Double                  'g C kg soil

        Dim humificationAdjustmentBySOC, _
            decompositionAdjustmentBySOC As Double

        Dim contactFractionFlat As Double           'fraction of surface residues subject to decomposition
        Dim contactFractionStan As Double           'fraction of surface residues subject to decomposition

        Dim xx0, xx1, xx2, xx3, xx4, xx5, xx6, xx7, xx8, xx9 As Double    'residue mass decomposition (xx1-5), manure carbon decomposition (xx6-7), and organic matter and microbial pool decomposition (xx8 and xx9)- Mg/ha/day
        Dim nm0, nm1, nm2, nm3, nm4, nm5, nm6, nm7, nm8, nm9 As Double    'residue nitrogen net mineralization
        Dim nr0, nr1, nr2, nr3, nr4, nr5, nr6, nr7, nr8, nr9 As Double    'residue nitrogen removed by decomposition from each residue pool
        Dim nh0, nh1, nh2, nh3, nh4, nh5, nh6, nh7, nh8, nh9 As Double    'residue nitrogen transfered to microbial biomass (nh1-8) or from microbial biomass to SOM  

        Dim xxPartialSum As Double

        Dim NMineralization, _
            NImmobilization, _
            NNetMineralization As Double

        Dim stanCNRatio, _
            flatCNRatio, _
            abgdCNRatio, _
            rootCNRatio, _
            rhizCNRatio, _
            smanCNRatio, _
            manuCNRatio, _
            micrCNRatio, _
            somCNratio As Double                        'sman = surface manure, manu = manure

        Dim CNnew As Double                             'CN of newly formed organic matter or microbial biomass
        Dim NMineralConcentration As Double             'g N-NO3 g soil
        Dim NMineral As Double                          'sum of NO3 and NH4 (be careful with units)
        Dim NH4_Fraction As Double                      'fraction of NH4 in the sum NO3 + NH4
        Dim decompReductionFactor As Double

        Dim NInitial, NFinal As Double
        'Dim SOCINITIAL As Double 'temporary

        NInitial = 0
        NFinal = 0
        NInitial = Residue.stanResidueN + Residue.flatResidueN + Residue.manureSurfaceN

        contactFractionFlat = 0
        contactFractionStan = 0

        Soil.N_Immobilization = 0
        Soil.N_Mineralization = 0
        Soil.N_NetMineralization = 0
        Soil.SOCProfile = 0
        Soil.SONProfile = 0
        Soil.C_Humified = 0
        Soil.C_ResidueRespired = 0
        Soil.C_SoilRespired = 0

        For i As Integer = 1 To Soil.totalLayers
            NInitial += Soil.SON_Mass(i) + Soil.MBN_Mass(i) + Soil.NO3(i) + Soil.NH4(i) + Residue.residueAbgdN(i) + Residue.residueRtN(i) + Residue.residueRzN(i) + Residue.manureN(i)
            'SOCINITIAL = Soil.SOC_Mass(i) 'temporary

            socDecompositionRate = 0
            micrDecompositionRate = 0
            humifiedCarbon = 0
            humifiedNitrogen = 0
            NMineralization = 0
            NImmobilization = 0
            NNetMineralization = 0

            CNnew = 0                   'CN of destiny pool (microbial biomass) calculated before each microbial attack; for microbial biomass it equal CN microbial biomass
            stanCNRatio = 0             'CN ratio of standing residues
            flatCNRatio = 0             'CN ratio of standing residues
            abgdCNRatio = 0             'CN ratio of aboveground residues (in each soil layer)
            rootCNRatio = 0             'CN ratio of root residues
            rhizCNRatio = 0             'CN ratio of rizhodeposition
            smanCNRatio = 0             'CN ratio of surface manure
            manuCNRatio = 0             'CN ratio of manure
            micrCNRatio = 0             'CN ratio of microbial biomass

            xx0 = 0 : xx1 = 0 : xx2 = 0 : xx3 = 0 : xx4 = 0 : xx5 = 0 : xx6 = 0 : xx7 = 0 : xx8 = 0 : xx9 = 0
            nm0 = 0 : nm1 = 0 : nm2 = 0 : nm3 = 0 : nm4 = 0 : nm5 = 0 : nm6 = 0 : nm7 = 0 : nm8 = 0 : nm9 = 0
            nr0 = 0 : nr1 = 0 : nr2 = 0 : nr3 = 0 : nr4 = 0 : nr5 = 0 : nr6 = 0 : nr7 = 0 : nr8 = 0 : nr9 = 0
            nh0 = 0 : nh1 = 0 : nh2 = 0 : nh3 = 0 : nh4 = 0 : nh5 = 0 : nh6 = 0 : nh7 = 0 : nh8 = 0 : nh9 = 0

            'compute auxiliar variables
            soilMass = 10000 * Soil.BD(i) * Soil.layerThickness(i) '10000 converts from Mg/m2 to Mg/ha
            NMineral = Soil.NO3(i) + Soil.NH4(i)
            NH4_Fraction = Soil.NH4(i) / NMineral
            NMineralConcentration = Soil.NO3(i) / soilMass
            satSOCConc = 21.1 + 0.375 * Soil.Clay(i) * 100

            'compute C/N ratios
            If i = 1 Then
                If Residue.stanResidueMass > 0 Then stanCNRatio = Residue.stanResidueMass * FRACTION_CARBON_PLANT / Residue.stanResidueN
                If Residue.flatResidueMass > 0 Then flatCNRatio = Residue.flatResidueMass * FRACTION_CARBON_PLANT / Residue.flatResidueN
                If Residue.manureSurfaceC > 0 Then smanCNRatio = Residue.manureSurfaceC / Residue.manureSurfaceN
            End If

            somCNratio = Soil.SOC_Mass(i) / Soil.SON_Mass(i)
            micrCNRatio = Soil.MBC_Mass(i) / Soil.MBN_Mass(i)
            If Residue.residueAbgd(i) > 0 Then abgdCNRatio = Residue.residueAbgd(i) * FRACTION_CARBON_PLANT / Residue.residueAbgdN(i)
            If Residue.residueRt(i) > 0 Then rootCNRatio = Residue.residueRt(i) * FRACTION_CARBON_PLANT / Residue.residueRtN(i)
            If Residue.residueRz(i) > 0 Then rhizCNRatio = Residue.residueRz(i) * FRACTION_CARBON_RIZHO / Residue.residueRzN(i)
            If Residue.manureC(i) > 0 Then manuCNRatio = Residue.manureC(i) / Residue.manureN(i)

            'humification
            humificationAdjustmentBySOC = 1 - (Soil.SOC_Conc(i) / satSOCConc) ^ SOC_HUMIFICATION_POWER 'humification reduction when C conc approaches saturation
            humificationAdjustmentBySOC = Math.Max(0, humificationAdjustmentBySOC)

            'abgdHumificationFactor = Math.Sqrt(MaximumAbgdHumificationFactor(Soil.Clay(i))) * humificationAdjustmentBySOC
            'rootHumificationFactor = Math.Sqrt(MaximumRootHumificationFactor(Soil.Clay(i))) * humificationAdjustmentBySOC
            'rhizHumificationFactor = Math.Sqrt(MaximumRhizHumificationFactor(Soil.Clay(i))) * humificationAdjustmentBySOC
            'manuHumificationFactor = Math.Sqrt(MaximumManuHumificationFactor(Soil.Clay(i))) * humificationAdjustmentBySOC
            'micrHumificationFactor = abgdHumificationFactor 'temporarily assigned abgd humification, then checked if it can be higher if manure is decomposing, but never lower

            abgdHumificationFactor = Math.Sqrt(MaximumAbgdHumificationFactor(Soil.Clay(i)) * humificationAdjustmentBySOC)
            rootHumificationFactor = Math.Sqrt(MaximumRootHumificationFactor(Soil.Clay(i)) * humificationAdjustmentBySOC)
            rhizHumificationFactor = Math.Sqrt(MaximumRhizHumificationFactor(Soil.Clay(i)) * humificationAdjustmentBySOC)
            manuHumificationFactor = Math.Sqrt(MaximumManuHumificationFactor(Soil.Clay(i)) * humificationAdjustmentBySOC)
            micrHumificationFactor = abgdHumificationFactor 'temporarily assigned abgd humification, then checked if it can be higher if manure is decomposing, but never lower

            'RESIDUE AND MANURE DECOMPOSITION
            If i = 1 Then
                contactFractionStan = Residue.stanResidueTau ^ Exp(-1.5 / Math.Sqrt(1 - Residue.stanResidueTau))
                contactFractionFlat = Residue.flatResidueTau ^ Exp(-1.5 / Math.Sqrt(1 - Residue.flatResidueTau))
                xx2 = factorComposite(i) * MAXIMUM_RESIDUE_DECOMPOSITION_RATE * contactFractionStan * Residue.stanResidueMass
                xx3 = factorComposite(i) * MAXIMUM_RESIDUE_DECOMPOSITION_RATE * contactFractionFlat * Residue.flatResidueMass
                xx6 = factorComposite(i) * MAXIMUM_MANURE_DECOMPOSITION_RATE * Residue.manureSurfaceC
            End If

            xx1 = factorComposite(i) * MAXIMUM_RESIDUE_DECOMPOSITION_RATE * Residue.residueAbgd(i)
            xx4 = factorComposite(i) * MAXIMUM_ROOT_DECOMPOSITION_RATE * Residue.residueRt(i)
            xx5 = factorComposite(i) * MAXIMUM_RHIZO_DECOMPOSITION_RATE * Residue.residueRz(i)
            xx7 = factorComposite(i) * MAXIMUM_MANURE_DECOMPOSITION_RATE * Residue.manureC(i)

            'inorganic N limitation for decomposition
            'if decomposition > 0 then compute net N mineralization and accumulate negatives
            If i = 1 Then
                If xx2 > 0 Then CNnew = CNdestiny(NMineralConcentration, stanCNRatio) : nm2 = NitrogenMineralization(stanCNRatio, CNnew, abgdHumificationFactor, xx2 * FRACTION_CARBON_PLANT)
                If xx3 > 0 Then CNnew = CNdestiny(NMineralConcentration, flatCNRatio) : nm3 = NitrogenMineralization(flatCNRatio, CNnew, abgdHumificationFactor, xx3 * FRACTION_CARBON_PLANT)
                If xx6 > 0 Then CNnew = CNdestiny(NMineralConcentration, smanCNRatio) : nm6 = NitrogenMineralization(smanCNRatio, CNnew, manuHumificationFactor, xx6)

                If nm2 < 0 Then nm0 += nm2
                If nm3 < 0 Then nm0 += nm3
                If nm6 < 0 Then nm0 += nm6
            End If

            If xx1 > 0 Then CNnew = CNdestiny(NMineralConcentration, abgdCNRatio) : nm1 = NitrogenMineralization(abgdCNRatio, CNnew, abgdHumificationFactor, xx1 * FRACTION_CARBON_PLANT)
            If xx4 > 0 Then CNnew = CNdestiny(NMineralConcentration, rootCNRatio) : nm4 = NitrogenMineralization(rootCNRatio, CNnew, rootHumificationFactor, xx4 * FRACTION_CARBON_PLANT)
            If xx5 > 0 Then CNnew = CNdestiny(NMineralConcentration, rhizCNRatio) : nm5 = NitrogenMineralization(rhizCNRatio, CNnew, rhizHumificationFactor, xx5 * FRACTION_CARBON_RIZHO)
            If xx7 > 0 Then CNnew = CNdestiny(NMineralConcentration, manuCNRatio) : nm7 = NitrogenMineralization(manuCNRatio, CNnew, manuHumificationFactor, xx7)

            If nm1 < 0 Then nm0 += nm1
            If nm4 < 0 Then nm0 += nm4
            If nm5 < 0 Then nm0 += nm5
            If nm7 < 0 Then nm0 += nm7


            If (-nm0) > NMineral Then
                decompReductionFactor = NMineral / (-nm0)
            Else
                decompReductionFactor = 1
            End If

            If decompReductionFactor < 1 Then
                'adjust actual decomposition as a function on mineral N availability
                If nm1 < 0 Then xx1 *= decompReductionFactor
                If nm2 < 0 Then xx2 *= decompReductionFactor
                If nm3 < 0 Then xx3 *= decompReductionFactor
                If nm4 < 0 Then xx4 *= decompReductionFactor
                If nm5 < 0 Then xx5 *= decompReductionFactor
                If nm6 < 0 Then xx6 *= decompReductionFactor
                If nm7 < 0 Then xx7 *= decompReductionFactor

                'recalculate net mineralization only for pools with adjusted decomposition
                If nm1 < 0 Then CNnew = CNdestiny(NMineralConcentration, abgdCNRatio) : nm1 = NitrogenMineralization(abgdCNRatio, CNnew, abgdHumificationFactor, xx1 * FRACTION_CARBON_PLANT)
                If nm2 < 0 Then CNnew = CNdestiny(NMineralConcentration, stanCNRatio) : nm2 = NitrogenMineralization(stanCNRatio, CNnew, abgdHumificationFactor, xx2 * FRACTION_CARBON_PLANT)
                If nm3 < 0 Then CNnew = CNdestiny(NMineralConcentration, flatCNRatio) : nm3 = NitrogenMineralization(flatCNRatio, CNnew, abgdHumificationFactor, xx3 * FRACTION_CARBON_PLANT)
                If nm4 < 0 Then CNnew = CNdestiny(NMineralConcentration, rootCNRatio) : nm4 = NitrogenMineralization(rootCNRatio, CNnew, rootHumificationFactor, xx4 * FRACTION_CARBON_PLANT)
                If nm5 < 0 Then CNnew = CNdestiny(NMineralConcentration, rhizCNRatio) : nm5 = NitrogenMineralization(rhizCNRatio, CNnew, rhizHumificationFactor, xx5 * FRACTION_CARBON_RIZHO)
                If nm6 < 0 Then CNnew = CNdestiny(NMineralConcentration, smanCNRatio) : nm6 = NitrogenMineralization(smanCNRatio, CNnew, manuHumificationFactor, xx6)
                If nm7 < 0 Then CNnew = CNdestiny(NMineralConcentration, manuCNRatio) : nm7 = NitrogenMineralization(manuCNRatio, CNnew, manuHumificationFactor, xx7)
            End If

            'calculate weighted C retention efficiency from microbial biomass to soil organic matter - 16 April 2014
            xxPartialSum = xx1 + xx2 + xx3 + xx4 + xx5 + xx6 + xx7
            If xxPartialSum > 0 Then
                micrHumificationFactor = Math.Max(micrHumificationFactor, _
                                        (1 / xxPartialSum) * (abgdHumificationFactor * (xx1 + xx2 + xx3) _
                                        + rootHumificationFactor * xx4 _
                                        + rhizHumificationFactor * xx5 _
                                        + manuHumificationFactor * (xx6 + xx7)))
            End If

            'SOC decomposition and SON mineralization
            decompositionAdjustmentBySOC = Math.Min(1, 1 - 1 / (1 + ((Soil.SOC_Conc(i) / satSOCConc) / 0.22) ^ 3.0))
            socDecompositionRate = factorComposite(i) * (1 + Tillage.tillageFactor(i)) * MAXIMUM_UNDISTURBED_SOC_DECOMPOSITION_RATE * decompositionAdjustmentBySOC / (1 - micrHumificationFactor ^ 2)
            xx8 = socDecompositionRate * Soil.SOC_Mass(i)
            If xx8 > 0 Then CNnew = CNdestiny(NMineralConcentration, somCNratio) : nm8 = NitrogenMineralization(somCNratio, CNnew, micrHumificationFactor, xx8)

            'microbial biomass decomposition and N mineralization - 16 April 2014
            Dim aux1, aux2, aux3 As Double
            aux1 = (Soil.MBC_Mass(i) / Soil.SOC_Mass(i)) / 0.03
            aux2 = Exp(10 * (aux1 - 1)) ' acceleration of microbial turnover if > 3% of SOC
            aux3 = aux2 * MAXIMUM_UNDISTURBED_SOC_DECOMPOSITION_RATE * decompositionAdjustmentBySOC / (1 - micrHumificationFactor ^ 2) * (0.97 / 0.03) * (1 / micrHumificationFactor) ' Charlie's steady state km so that Cm = 0.03 of organic carbon km = 0.97ks / 0.03(ex(1-Cs/Cx))1/2. Also notice ks acceleration from apparent to actual turnover ks/(1-e^2)
            micrDecompositionRate = factorComposite(i) * (1 + Tillage.tillageFactor(i)) * aux3
            xx9 = micrDecompositionRate * Soil.MBC_Mass(i)
            If xx9 > 0 Then nm9 = NitrogenMineralization(micrCNRatio, micrCNRatio, micrHumificationFactor, xx9)

            'calculate N removal from decomposing pools
            If xx1 > 0 Then nr1 = xx1 * FRACTION_CARBON_PLANT / abgdCNRatio
            If xx2 > 0 Then nr2 = xx2 * FRACTION_CARBON_PLANT / stanCNRatio
            If xx3 > 0 Then nr3 = xx3 * FRACTION_CARBON_PLANT / flatCNRatio
            If xx4 > 0 Then nr4 = xx4 * FRACTION_CARBON_PLANT / rootCNRatio
            If xx5 > 0 Then nr5 = xx5 * FRACTION_CARBON_RIZHO / rhizCNRatio
            If xx6 > 0 Then nr6 = xx6 / smanCNRatio
            If xx7 > 0 Then nr7 = xx7 / manuCNRatio
            If xx8 > 0 Then nr8 = xx8 / somCNratio
            If xx9 > 0 Then nr9 = xx9 / micrCNRatio

            'calculate N contribution (N humification) to microbial pool of each decomposing pool
            If nm1 > 0 Then nh1 = nr1 - nm1 Else nh1 = nr1
            If nm2 > 0 Then nh2 = nr2 - nm2 Else nh2 = nr2
            If nm3 > 0 Then nh3 = nr3 - nm3 Else nh3 = nr3
            If nm4 > 0 Then nh4 = nr4 - nm4 Else nh4 = nr4
            If nm5 > 0 Then nh5 = nr5 - nm5 Else nh5 = nr5
            If nm6 > 0 Then nh6 = nr6 - nm6 Else nh6 = nr6
            If nm7 > 0 Then nh7 = nr7 - nm7 Else nh7 = nr7
            If nm8 > 0 Then nh8 = nr8 - nm8 Else nh8 = nr8
            If nm9 > 0 Then nh9 = nr9 - nm9 Else nh9 = nr9

            'calculate total residue, manure, and som carbon tansfer to microbial pool - 16 April 2014 
            humifiedCarbon = abgdHumificationFactor * FRACTION_CARBON_PLANT * (xx1 + xx2 + xx3) _
                             + rootHumificationFactor * FRACTION_CARBON_PLANT * xx4 _
                             + rhizHumificationFactor * FRACTION_CARBON_RIZHO * xx5 _
                             + manuHumificationFactor * (xx6 + xx7) _
                             + micrHumificationFactor * xx8

            'calculate total residue, manure, and som nitrogen transfer to microbial pool - 16 April 2014
            humifiedNitrogen = nh1 + nh2 + nh3 + nh4 + nh5 + nh6 + nh7 + nh8

            'accumulate N mineralization, immobilization, and net mineralization
            If nm1 > 0 Then NMineralization += nm1 Else NImmobilization += nm1
            If nm2 > 0 Then NMineralization += nm2 Else NImmobilization += nm2
            If nm3 > 0 Then NMineralization += nm3 Else NImmobilization += nm3
            If nm4 > 0 Then NMineralization += nm4 Else NImmobilization += nm4
            If nm5 > 0 Then NMineralization += nm5 Else NImmobilization += nm5
            If nm6 > 0 Then NMineralization += nm6 Else NImmobilization += nm6
            If nm7 > 0 Then NMineralization += nm7 Else NImmobilization += nm7
            If nm8 > 0 Then NMineralization += nm8 Else NImmobilization += nm8
            If nm9 > 0 Then NMineralization += nm9 Else NImmobilization += nm9

            NNetMineralization = NMineralization + NImmobilization

            'UPDATE POOLS (N immbilization is negative)
            Soil.NO3(i) += NImmobilization * (1 - NH4_Fraction)
            Soil.NH4(i) += NImmobilization * NH4_Fraction + NMineralization

            If i = 1 Then
                Residue.stanResidueWater *= (1 - xx2 / (Residue.stanResidueMass + 0.0000000001))
                Residue.flatResidueWater *= (1 - xx3 / (Residue.flatResidueMass + 0.0000000001))
                Residue.stanResidueMass -= xx2
                Residue.flatResidueMass -= xx3
                Residue.manureSurfaceC -= xx6
                Residue.stanResidueN -= nr2
                Residue.flatResidueN -= nr3
                Residue.manureSurfaceN -= nr6
            End If

            Residue.residueAbgd(i) -= xx1
            Residue.residueRt(i) -= xx4
            Residue.residueRz(i) -= xx5
            Residue.manureC(i) -= xx7
            Residue.residueAbgdN(i) -= nr1
            Residue.residueRtN(i) -= nr4
            Residue.residueRzN(i) -= nr5
            Residue.manureN(i) -= nr7

            Soil.SOC_Mass(i) += xx9 * micrHumificationFactor - xx8
            Soil.SON_Mass(i) += nh9 - nr8
            Soil.SOC_Conc(i) = Soil.SOC_Mass(i) * 1000 / soilMass

            Soil.MBC_Mass(i) += humifiedCarbon - xx9
            Soil.MBN_Mass(i) += humifiedNitrogen + (-NImmobilization) - nr9

            carbonRespired(i) = (1 - abgdHumificationFactor) * FRACTION_CARBON_PLANT * (xx1 + xx2 + xx3) _
                                + (1 - rootHumificationFactor) * FRACTION_CARBON_PLANT * xx4 _
                                + (1 - rhizHumificationFactor) * FRACTION_CARBON_RIZHO * xx5 _
                                + (1 - manuHumificationFactor) * (xx6 + xx7) _
                                + (1 - micrHumificationFactor) * (xx8 + xx9)

            'for OUTPUT
            annualSoilCarbonDecompositionRate(y, i) += SOCDecompositionRate
            annualRespiredCarbonMass(y, i) += (1 - micrHumificationFactor) * (xx8 + xx9)                                'excludes residue and manure
            annualRespiredResidueCarbonMass(y, i) += carbonRespired(i) - (1 - micrHumificationFactor) * (xx8 + xx9)     'residue, roots and manure only
            abgdCarbonInput(y, i) += FRACTION_CARBON_PLANT * (xx1 + xx2 + xx3)
            rootCarbonInput(y, i) += FRACTION_CARBON_PLANT * xx4
            rhizCarbonInput(y, i) += FRACTION_CARBON_RIZHO * xx5
            manuCarbonInput(y, i) += (xx6 + xx7)

            annualCarbonInputByLayer(y, i) = abgdCarbonInput(y, i) + rootCarbonInput(y, i) + rhizCarbonInput(y, i) + manuCarbonInput(y, i)
            annualHumifiedCarbonMass(y, i) += xx9 * micrHumificationFactor + humifiedCarbon                 'C that goes to SOC and C that goes to microbial mass

            Soil.N_Immobilization += NImmobilization
            Soil.N_Mineralization += NMineralization
            Soil.N_NetMineralization += NNetMineralization
            Soil.SOCProfile += Soil.SOC_Mass(i) + Soil.MBC_Mass(i)
            Soil.SONProfile += Soil.SON_Mass(i) + Soil.MBN_Mass(i)
            Soil.C_Humified += xx9 * micrHumificationFactor + humifiedCarbon                                'C that goes to SOC and C that goes to microbial mass
            Soil.C_ResidueRespired += carbonRespired(i) - (1 - micrHumificationFactor) * (xx8 + xx9)        'residues, roots and manure
            Soil.C_SoilRespired += (1 - micrHumificationFactor) * (xx8 + xx9)

            NFinal += Soil.SON_Mass(i) + Soil.MBN_Mass(i) + Soil.NO3(i) + Soil.NH4(i) + Residue.residueAbgdN(i) + Residue.residueRtN(i) + Residue.residueRzN(i) + Residue.manureN(i)
        Next

        NFinal += Residue.stanResidueN + Residue.flatResidueN + Residue.manureSurfaceN

        If Abs(NFinal - NInitial) > 0.00001 Then Stop

    End Sub

    Public Sub ComputeSoilCarbonBalance(ByRef y As Integer, ByRef Residue As ResidueClass, ByRef Soil As SoilClass, ByRef Tillage As TillageClass)

        Dim apparentSOCDecompositionRate, _
            apparentSOCDecomposition, _
            SON_Mineralization, _
            humifiedCarbon, _
            humifiedNitrogen As Double

        Dim abgdHumificationFactor, _
            rootHumificationFactor, _
            rhizHumificationFactor, _
            manuHumificationFactor, _
            soilMass, _
            satSOCConc As Double                  'g C kg soil

        Dim humificationAdjustmentBySOC, _
            decompositionAdjustmentBySOC As Double

        Dim contactFractionFlat As Double           'fraction of surface residues subject to decomposition
        Dim contactFractionStan As Double           'fraction of surface residues subject to decomposition

        Dim xx1, xx2, xx3, xx4, xx5, xx6, xx7 As Double         'residue mass decomposition (xx1-5) and manure carbon decomposition (xx6-7), Mg/ha/day
        Dim nm1, nm2, nm3, nm4, nm5, nm6, nm7, nm8 As Double    'residue nitrogen net mineralization
        Dim nh1, nh2, nh3, nh4, nh5, nh6, nh7 As Double         'residue nitrogen humified or retained in organic matter
        Dim nr1, nr2, nr3, nr4, nr5, nr6, nr7 As Double         'residue nitrogen removed by decomposition from each residue pool

        Dim NMineralization, _
            NImmobilization, _
            NNetMineralization As Double

        Dim stanCNRatio, _
            flatCNRatio, _
            abgdCNRatio, _
            rootCNRatio, _
            rhizCNRatio, _
            smanCNRatio, _
            manuCNRatio, _
            CN_SOM As Double                        'sman = surface manure, manu = manure

        Dim NO3_Concentration As Double             'g N-NO3 g soil
        Dim NMineral As Double                      'sum of NO3 and NH4 (be careful with units)
        Dim NH4_Fraction As Double                  'fraction of NH4 in the sum NO3 + NH4
        Dim decompReductionFactor As Double

        Dim NInitial, NFinal As Double
        'Dim SOCINITIAL As Double 'temporary

        NInitial = 0
        NFinal = 0
        NInitial = Residue.stanResidueN + Residue.flatResidueN + Residue.manureSurfaceN

        contactFractionFlat = 0
        contactFractionStan = 0

        Soil.N_Immobilization = 0
        Soil.N_Mineralization = 0
        Soil.N_NetMineralization = 0
        Soil.SOCProfile = 0
        Soil.SONProfile = 0
        Soil.C_Humified = 0
        Soil.C_ResidueRespired = 0
        Soil.C_SoilRespired = 0

        For i As Integer = 1 To Soil.totalLayers
            NInitial += Soil.SON_Mass(i) + Soil.NO3(i) + Soil.NH4(i) + Residue.residueAbgdN(i) + Residue.residueRtN(i) + Residue.residueRzN(i) + Residue.manureN(i)
            'SOCINITIAL = Soil.SOC_Mass(i) 'temporary

            apparentSOCDecompositionRate = 0
            apparentSOCDecomposition = 0
            SON_Mineralization = 0
            humifiedCarbon = 0
            humifiedNitrogen = 0

            NNetMineralization = 0
            NMineralization = 0
            NImmobilization = 0

            stanCNRatio = 0             'CN ratio of standing residues
            flatCNRatio = 0             'CN ratio of standing residues
            abgdCNRatio = 0             'CN ratio of aboveground residues (in each soil layer)
            rootCNRatio = 0             'CN ratio of root residues
            rhizCNRatio = 0             'CN ratio of rizhodeposition
            smanCNRatio = 0             'CN ratio of surface manure
            manuCNRatio = 0             'CN ratio of manure

            xx1 = 0 : xx2 = 0 : xx3 = 0 : xx4 = 0 : xx5 = 0 : xx6 = 0 : xx7 = 0
            nm1 = 0 : nm2 = 0 : nm3 = 0 : nm4 = 0 : nm5 = 0 : nm6 = 0 : nm7 = 0 : nm8 = 0
            nr1 = 0 : nr2 = 0 : nr3 = 0 : nr4 = 0 : nr5 = 0 : nr6 = 0 : nr7 = 0
            nh1 = 0 : nh2 = 0 : nh3 = 0 : nh4 = 0 : nh5 = 0 : nh6 = 0 : nh7 = 0

            soilMass = 10000 * Soil.BD(i) * Soil.layerThickness(i) '10000 converts from Mg/m2 to Mg/ha
            NO3_Concentration = Soil.NO3(i) / soilMass
            satSOCConc = 21.1 + 0.375 * Soil.Clay(i) * 100

            'compute C/N ratios
            If i = 1 Then
                If Residue.stanResidueMass > 0 Then stanCNRatio = Residue.stanResidueMass * FRACTION_CARBON_PLANT / Residue.stanResidueN
                If Residue.flatResidueMass > 0 Then flatCNRatio = Residue.flatResidueMass * FRACTION_CARBON_PLANT / Residue.flatResidueN
                If Residue.manureSurfaceC > 0 Then smanCNRatio = Residue.manureSurfaceC / Residue.manureSurfaceN
            End If

            CN_SOM = Soil.SOC_Mass(i) / Soil.SON_Mass(i)
            If Residue.residueAbgd(i) > 0 Then abgdCNRatio = Residue.residueAbgd(i) * FRACTION_CARBON_PLANT / Residue.residueAbgdN(i)
            If Residue.residueRt(i) > 0 Then rootCNRatio = Residue.residueRt(i) * FRACTION_CARBON_PLANT / Residue.residueRtN(i)
            If Residue.residueRz(i) > 0 Then rhizCNRatio = Residue.residueRz(i) * FRACTION_CARBON_RIZHO / Residue.residueRzN(i)
            If Residue.manureC(i) > 0 Then manuCNRatio = Residue.manureC(i) / Residue.manureN(i)

            'humification
            humificationAdjustmentBySOC = 1 - (Soil.SOC_Conc(i) / satSOCConc) ^ SOC_HUMIFICATION_POWER 'humification reduction when C conc approaches saturation
            If humificationAdjustmentBySOC < 0 Then humificationAdjustmentBySOC = 0

            abgdHumificationFactor = MaximumAbgdHumificationFactor(Soil.Clay(i)) * humificationAdjustmentBySOC
            rootHumificationFactor = MaximumRootHumificationFactor(Soil.Clay(i)) * humificationAdjustmentBySOC
            rhizHumificationFactor = MaximumRhizHumificationFactor(Soil.Clay(i)) * humificationAdjustmentBySOC
            manuHumificationFactor = MaximumManuHumificationFactor(Soil.Clay(i)) * humificationAdjustmentBySOC

            'RESIDUE AND MANURE DECOMPOSITION
            If i = 1 Then
                contactFractionStan = Residue.stanResidueTau ^ Exp(-1.5 / Math.Sqrt(1 - Residue.stanResidueTau))
                contactFractionFlat = Residue.flatResidueTau ^ Exp(-1.5 / Math.Sqrt(1 - Residue.flatResidueTau))
                xx2 = factorComposite(i) * MAXIMUM_RESIDUE_DECOMPOSITION_RATE * contactFractionStan * Residue.stanResidueMass
                xx3 = factorComposite(i) * MAXIMUM_RESIDUE_DECOMPOSITION_RATE * contactFractionFlat * Residue.flatResidueMass
                xx6 = factorComposite(i) * MAXIMUM_MANURE_DECOMPOSITION_RATE * Residue.manureSurfaceC
            End If

            xx1 = factorComposite(i) * MAXIMUM_RESIDUE_DECOMPOSITION_RATE * Residue.residueAbgd(i)
            xx4 = factorComposite(i) * MAXIMUM_ROOT_DECOMPOSITION_RATE * Residue.residueRt(i)
            xx5 = factorComposite(i) * MAXIMUM_RHIZO_DECOMPOSITION_RATE * Residue.residueRz(i)
            xx7 = factorComposite(i) * MAXIMUM_MANURE_DECOMPOSITION_RATE * Residue.manureC(i)

            'inorganic N limitation for decomposition
            'if decomposition > 0 then compute net N mineralization and accumulate negatives
            If i = 1 Then
                If xx2 > 0 Then nm2 = PoolNitrogenMineralization(NO3_Concentration, stanCNRatio, abgdHumificationFactor, xx2, FRACTION_CARBON_PLANT)
                If xx3 > 0 Then nm3 = PoolNitrogenMineralization(NO3_Concentration, flatCNRatio, abgdHumificationFactor, xx3, FRACTION_CARBON_PLANT)
                If xx6 > 0 Then nm6 = PoolNitrogenMineralization(NO3_Concentration, smanCNRatio, manuHumificationFactor, xx6, 1)

                If nm2 < 0 Then nm8 += nm2
                If nm3 < 0 Then nm8 += nm3
                If nm6 < 0 Then nm8 += nm6
            End If

            If xx1 > 0 Then nm1 = PoolNitrogenMineralization(NO3_Concentration, abgdCNRatio, abgdHumificationFactor, xx1, FRACTION_CARBON_PLANT)
            If xx4 > 0 Then nm4 = PoolNitrogenMineralization(NO3_Concentration, rootCNRatio, rootHumificationFactor, xx4, FRACTION_CARBON_PLANT)
            If xx5 > 0 Then nm5 = PoolNitrogenMineralization(NO3_Concentration, rhizCNRatio, rhizHumificationFactor, xx5, FRACTION_CARBON_RIZHO)
            If xx7 > 0 Then nm7 = PoolNitrogenMineralization(NO3_Concentration, manuCNRatio, manuHumificationFactor, xx7, 1)

            If nm1 < 0 Then nm8 += nm1
            If nm4 < 0 Then nm8 += nm4
            If nm5 < 0 Then nm8 += nm5
            If nm7 < 0 Then nm8 += nm7

            NMineral = Soil.NO3(i) + Soil.NH4(i)
            NH4_Fraction = Soil.NH4(i) / NMineral
            If (-nm8) > NMineral Then
                decompReductionFactor = NMineral / (-nm8)
            Else
                decompReductionFactor = 1
            End If

            If decompReductionFactor < 1 Then
                'adjust actual decomposition as a function on mineral N availability
                If nm1 < 0 Then xx1 *= decompReductionFactor
                If nm2 < 0 Then xx2 *= decompReductionFactor
                If nm3 < 0 Then xx3 *= decompReductionFactor
                If nm4 < 0 Then xx4 *= decompReductionFactor
                If nm5 < 0 Then xx5 *= decompReductionFactor
                If nm6 < 0 Then xx6 *= decompReductionFactor
                If nm7 < 0 Then xx7 *= decompReductionFactor

                'recalculate net mineralization only for pools with adjusted decomposition
                If nm1 < 0 Then nm1 = PoolNitrogenMineralization(NO3_Concentration, abgdCNRatio, abgdHumificationFactor, xx1, FRACTION_CARBON_PLANT)
                If nm2 < 0 Then nm2 = PoolNitrogenMineralization(NO3_Concentration, stanCNRatio, abgdHumificationFactor, xx2, FRACTION_CARBON_PLANT)
                If nm3 < 0 Then nm3 = PoolNitrogenMineralization(NO3_Concentration, flatCNRatio, abgdHumificationFactor, xx3, FRACTION_CARBON_PLANT)
                If nm4 < 0 Then nm4 = PoolNitrogenMineralization(NO3_Concentration, rootCNRatio, rootHumificationFactor, xx4, FRACTION_CARBON_PLANT)
                If nm5 < 0 Then nm5 = PoolNitrogenMineralization(NO3_Concentration, rhizCNRatio, rhizHumificationFactor, xx5, FRACTION_CARBON_RIZHO)
                If nm6 < 0 Then nm6 = PoolNitrogenMineralization(NO3_Concentration, smanCNRatio, manuHumificationFactor, xx6, 1)
                If nm7 < 0 Then nm7 = PoolNitrogenMineralization(NO3_Concentration, manuCNRatio, manuHumificationFactor, xx7, 1)
            End If

            'calculate N removal from decomposing pools except SON
            If xx1 > 0 Then nr1 = xx1 * FRACTION_CARBON_PLANT / abgdCNRatio
            If xx2 > 0 Then nr2 = xx2 * FRACTION_CARBON_PLANT / stanCNRatio
            If xx3 > 0 Then nr3 = xx3 * FRACTION_CARBON_PLANT / flatCNRatio
            If xx4 > 0 Then nr4 = xx4 * FRACTION_CARBON_PLANT / rootCNRatio
            If xx5 > 0 Then nr5 = xx5 * FRACTION_CARBON_RIZHO / rhizCNRatio
            If xx6 > 0 Then nr6 = xx6 / smanCNRatio
            If xx7 > 0 Then nr7 = xx7 / manuCNRatio

            'calculate N contribution (N humification) to SON of each decomposing pool
            If nm1 > 0 Then nh1 = nr1 - nm1 Else nh1 = nr1
            If nm2 > 0 Then nh2 = nr2 - nm2 Else nh2 = nr2
            If nm3 > 0 Then nh3 = nr3 - nm3 Else nh3 = nr3
            If nm4 > 0 Then nh4 = nr4 - nm4 Else nh4 = nr4
            If nm5 > 0 Then nh5 = nr5 - nm5 Else nh5 = nr5
            If nm6 > 0 Then nh6 = nr6 - nm6 Else nh6 = nr6
            If nm7 > 0 Then nh7 = nr7 - nm7 Else nh7 = nr7

            'calculate total residue and manure carbon humification
            humifiedCarbon = abgdHumificationFactor * FRACTION_CARBON_PLANT * (xx1 + xx2 + xx3) _
                             + rootHumificationFactor * FRACTION_CARBON_PLANT * xx4 _
                             + rhizHumificationFactor * FRACTION_CARBON_RIZHO * xx5 _
                             + manuHumificationFactor * (xx6 + xx7)

            'calculate total residue and manure nitrogen humification
            humifiedNitrogen = nh1 + nh2 + nh3 + nh4 + nh5 + nh6 + nh7

            'SOC DECOMPOSITION
            'decompositionAdjustmentBySOC = (Soil.SOC_Concentration(i) / satSOCConc) ^ SOC_DECOMPOSITION_POWER
            decompositionAdjustmentBySOC = 1 - 1 / (1 + ((Soil.SOC_Conc(i) / satSOCConc) / 0.22) ^ 3.0)
            If decompositionAdjustmentBySOC > 1 Then decompositionAdjustmentBySOC = 1
            apparentSOCDecompositionRate = factorComposite(i) * (1 + Tillage.tillageFactor(i)) * MAXIMUM_UNDISTURBED_SOC_DECOMPOSITION_RATE * decompositionAdjustmentBySOC
            apparentSOCDecomposition = apparentSOCDecompositionRate * Soil.SOC_Mass(i)
            SON_Mineralization = apparentSOCDecomposition / CN_SOM
            'End If

            'accumulate N mineralization, immobilization, and net mineralization
            If nm1 > 0 Then NMineralization += nm1 Else NImmobilization += nm1
            If nm2 > 0 Then NMineralization += nm2 Else NImmobilization += nm2
            If nm3 > 0 Then NMineralization += nm3 Else NImmobilization += nm3
            If nm4 > 0 Then NMineralization += nm4 Else NImmobilization += nm4
            If nm5 > 0 Then NMineralization += nm5 Else NImmobilization += nm5
            If nm6 > 0 Then NMineralization += nm6 Else NImmobilization += nm6
            If nm7 > 0 Then NMineralization += nm7 Else NImmobilization += nm7

            NMineralization += SON_Mineralization
            NNetMineralization = NMineralization + NImmobilization

            'UPDATE POOLS (N immbilization is negative)
            Soil.NO3(i) += NImmobilization * (1 - NH4_Fraction)
            Soil.NH4(i) += NImmobilization * NH4_Fraction + NMineralization

            If i = 1 Then
                Residue.stanResidueWater *= (1 - xx2 / (Residue.stanResidueMass + 0.0000000001))
                Residue.flatResidueWater *= (1 - xx3 / (Residue.flatResidueMass + 0.0000000001))
                Residue.stanResidueMass -= xx2
                Residue.flatResidueMass -= xx3
                Residue.manureSurfaceC -= xx6
                Residue.stanResidueN -= nr2
                Residue.flatResidueN -= nr3
                Residue.manureSurfaceN -= nr6
            End If

            Residue.residueAbgd(i) -= xx1
            Residue.residueRt(i) -= xx4
            Residue.residueRz(i) -= xx5
            Residue.manureC(i) -= xx7
            Residue.residueAbgdN(i) -= nr1
            Residue.residueRtN(i) -= nr4
            Residue.residueRzN(i) -= nr5
            Residue.manureN(i) -= nr7

            Soil.SOC_Mass(i) += humifiedCarbon - apparentSOCDecomposition
            Soil.SON_Mass(i) += humifiedNitrogen + (-NImmobilization) - SON_Mineralization
            Soil.SOC_Conc(i) = Soil.SOC_Mass(i) * 1000 / soilMass
            carbonRespired(i) = apparentSOCDecomposition _
                                + (1 - abgdHumificationFactor) * FRACTION_CARBON_PLANT * (xx1 + xx2 + xx3) _
                                + (1 - rootHumificationFactor) * FRACTION_CARBON_PLANT * xx4 _
                                + (1 - rhizHumificationFactor) * FRACTION_CARBON_RIZHO * xx5 _
                                + (1 - manuHumificationFactor) * (xx6 + xx7)

            'for OUTPUT
            annualSoilCarbonDecompositionRate(y, i) += apparentSOCDecompositionRate
            annualRespiredCarbonMass(y, i) += apparentSOCDecomposition                              'excludes residue and manure
            annualRespiredResidueCarbonMass(y, i) += carbonRespired(i) - apparentSOCDecomposition   'residue and manure only
            abgdCarbonInput(y, i) += FRACTION_CARBON_PLANT * (xx1 + xx2 + xx3)
            rootCarbonInput(y, i) += FRACTION_CARBON_PLANT * xx4
            rhizCarbonInput(y, i) += FRACTION_CARBON_RIZHO * xx5
            manuCarbonInput(y, i) += (xx6 + xx7)

            annualCarbonInputByLayer(y, i) = abgdCarbonInput(y, i) + rootCarbonInput(y, i) + rhizCarbonInput(y, i) + manuCarbonInput(y, i)
            annualHumifiedCarbonMass(y, i) += humifiedCarbon

            Soil.N_Immobilization += NImmobilization
            Soil.N_Mineralization += NMineralization
            Soil.N_NetMineralization += NNetMineralization
            Soil.SOCProfile += Soil.SOC_Mass(i)
            Soil.SONProfile += Soil.SON_Mass(i)
            Soil.C_Humified += humifiedCarbon
            Soil.C_ResidueRespired += (carbonRespired(i) - apparentSOCDecomposition)                'includes residue, roots, and manure
            Soil.C_SoilRespired += apparentSOCDecomposition


            NFinal += Soil.SON_Mass(i) + Soil.NO3(i) + Soil.NH4(i) + Residue.residueAbgdN(i) + Residue.residueRtN(i) + Residue.residueRzN(i) + Residue.manureN(i)
        Next

        NFinal += Residue.stanResidueN + Residue.flatResidueN + Residue.manureSurfaceN

        If Abs(NFinal - NInitial) > 0.00001 Then Stop


    End Sub

    Public Sub StoreOutput(ByVal y As Integer, ByVal totalLayers As Integer, ByVal SOCMass() As Double)
        'Dim soilCarbonInitial As Double = 0
        'Dim soilCarbonFinal As Double = 0
        'Dim carbonBalance As Double = 0
        'Dim humifiedInputCarbon As Double = 0
        'Dim decomposedSoilCarbon As Double = 0

        For i As Integer = 1 To totalLayers
            'soilCarbonInitial += carbonMassInitial(y, i)
            'soilCarbonFinal += SOC_Mass(i)
            'humifiedInputCarbon += annualHumifiedCarbonMass(y, i)
            'decomposedSoilCarbon += annualRespiredCarbonMass(y, i)
            carbonMassFinal(y, i) = SideSubs.Formatting(SOCMass(i))
        Next

        'carbonBalance = (soilCarbonFinal - soilCarbonInitial) - (humifiedInputCarbon - decomposedSoilCarbon)
    End Sub


    Private Function Aeration(ByVal AC As Double) As Double
        'AC = soil air content
        Const A1 As Double = 0.05
        Const A2 As Integer = 4

        Return 1 - 0.6 / (1 + (AC / A1) ^ A2)
    End Function

    Private Function Moisture(ByVal wp As Double) As Double
        ' WP = soil water potential
        Const M1 As Integer = -600
        Const M2 As Integer = 3

        Return 1 / (1 + (wp / M1) ^ M2)
    End Function

    Private Function TemperatureFunction(ByVal T As Double) As Double
        Dim temp As Double
        Dim Q As Double

        Const tMin As Double = -5.0
        Const tOpt As Double = 35.0
        Const tMax As Double = 50.0

        If T < 0 Or T > tMax Then
            temp = 0
        Else
            Q = (tMin - tOpt) / (tOpt - tMax)
            temp = ((T - tMin) ^ (Q) * (tMax - T)) / ((tOpt - tMin) ^ (Q) * (tMax - tOpt))
            If temp > 1 Then temp = 1.0
        End If

        Return temp
    End Function

    Private Function MaximumAbgdHumificationFactor(ByRef clayFraction As Double) As Double
        Return 0.092 + 0.104 * (1 - Exp(-5.5 * clayFraction))
    End Function

    Private Function MaximumRootHumificationFactor(ByRef clayFraction As Double) As Double
        Return 0.092 + 0.104 * (1 - Exp(-5.5 * clayFraction))
    End Function

    Private Function MaximumRhizHumificationFactor(ByRef clayFraction As Double) As Double
        Return 0.0# + 0.084 * (1 - Exp(-5.5 * clayFraction))
    End Function

    Private Function MaximumManuHumificationFactor(ByRef clayFraction As Double) As Double
        Return 0.15 + 0.25 * (1 - Exp(-5.5 * clayFraction))
    End Function

    Private Function NitrogenMineralization(ByVal CNDecomposing As Double, ByVal CNnew As Double, ByVal humRate As Double, ByVal decomposedMass As Double) As Double
        'Dim newCN As Double 'CN of new organic matter (humified residue)
        Return decomposedMass * (1 / CNDecomposing - humRate / CNnew)
    End Function

    Private Function CNdestiny(ByVal NmineralConc As Double, ByVal CNdecomposing As Double) As Double
        'returns CN ratio of newly formed microbial biomass based on CN or decomposing residue and N mineral in soil
        'same function that for one-pool model, but applied to microbial biomass

        Dim YY2 As Double
        'NmineralConc = mineral N concentration g/g
        'YY1 = input-dependent constant to correct CN ratio = 110 (fix this to accomodate manure)
        'YY2 = correction factor based on CN of decomposing pool
        NmineralConc = NmineralConc + 0.0000001
        YY2 = 5.5 * (1 - 1 / (1 + (CNdecomposing / 110) ^ 3))
        Return 8.5 + YY2 * (0.5 + 0.5 / (1 + (NmineralConc / 0.000008) ^ 3))
    End Function

    Private Function PoolNitrogenMineralization(ByVal NmineralConc As Double, ByVal CNRatioDecomposing As Double, ByVal humRate As Double, _
                                                ByVal decomposedMass As Double, Optional ByVal carbonConc As Double = 1) As Double
        Dim newCN As Double 'CN of new organic matter (humified residue)

        decomposedMass *= carbonConc
        newCN = Function_CNnew(NmineralConc, CNRatioDecomposing)
        Return decomposedMass * (1 / CNRatioDecomposing - humRate / newCN)
    End Function

    Private Function Function_CNnew(ByVal NmineralConc As Double, ByVal CNDecomposingPool As Double) As Double
        'returns CN ratio of newly formed organic matter based on CN or decomposing residue and N mineral in soil

        Dim YY2 As Double
        'NmineralConc = mineral N concentration g/g
        'YY1 = input-dependent constant to correct CN ratio = 110 (fix this to accomodate manure)
        'YY2 = correction factor based on CN of decomposing pool
        NmineralConc = NmineralConc + 0.0000001
        YY2 = 5.5 * (1 - 1 / (1 + (CNDecomposingPool / 110) ^ 3))
        Return 8.5 + YY2 * (0.5 + 0.5 / (1 + (NmineralConc / 0.000008) ^ 3))
    End Function

End Class
