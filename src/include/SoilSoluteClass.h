Option Explicit On

Friend Class SoilSoluteClass
    Private Const WD As Double = 1000 'kg/m3

    'March 13 2013 version
    'Public Sub SoluteTransport(ByVal totalLayers As Integer, ByVal Sol_Kd As Double, ByVal WInConc As Double, ByRef leachate As Double, _
    '                            ByVal WFlux() As Double, ByRef soluteMass() As Double, ByVal BD() As Double, ByVal thickness() As Double, _
    '                            ByVal porosity() As Double, ByVal WCinitial() As Double)

    '    Dim C(totalLayers) As Double            'solute concentration at end of equilibrium, kg solute / m3
    '    Dim soluteConc As Double                'solute concentration, current layer, kg solute / m3
    '    Dim soluteFlow(totalLayers) As Double   'solute flow, kg solute per time step
    '    Dim waterInitial As Double              'initial layer soil water storage, kg / m2
    '    'Dim WaterFinal As Double                'final layer soil water storage, kg / m2
    '    'Dim WCFinal As Double                   'temporary variable to recalculate solute mass
    '    Dim R As Double                         'ratio of flux out of a layer to the layer porosity (or effective porosity)

    '    Dim soluteMassAdsorbed As Double        'kg/m2
    '    Dim soluteMassSolution As Double        'kg/m2

    '    soluteFlow(0) = WInConc * WFlux(0)

    '    For i As Integer = 1 To totalLayers

    '        waterInitial = thickness(i) * WD * WCinitial(i)
    '        'WaterFinal = WaterInitial + WFlux(i - 1) - WFlux(i)
    '        'WCFinal = WaterFinal / (Thickness(i) * WD)

    '        If soluteMass(i) > 0 Then
    '            soluteConc = LinearEquilibriumConcentration(Sol_Kd, BD(i), thickness(i), WCinitial(i), soluteMass(i))
    '        Else
    '            soluteConc = 0
    '        End If

    '        soluteMassAdsorbed = soluteMass(i) - waterInitial * soluteConc

    '        If WFlux(i) > 0 Then
    '            'note: the 0.67 is temporary, likely not needed, and if needed use a relationship with B
    '            R = 0.67 * WFlux(i) / (porosity(i) * thickness(i) * WD)
    '            soluteMassSolution = (soluteFlow(i - 1) / R + (waterInitial * soluteConc - soluteFlow(i - 1) / R) * Math.Exp(-R))
    '            soluteFlow(i) = waterInitial * soluteConc + soluteFlow(i - 1) - soluteMassSolution
    '            If i = totalLayers Then leachate = soluteFlow(i)
    '        Else
    '            soluteMassSolution = waterInitial * soluteConc + soluteFlow(i - 1)
    '            soluteFlow(i) = 0
    '        End If
    '        'this equilibrium can cause a mass balance error if "mass" is based on concentration at the end ignoring initial mass, not using it 
    '        'SoluteMass(i) = LinearEquilibriumSoluteMass(Sol_Kd, BD(i), Thickness(i), WCFinal, C(i))
    '        soluteMass(i) = soluteMassAdsorbed + soluteMassSolution
    '    Next

    'End Sub


    Public Sub SoluteTransport(ByVal totalLayers As Integer, ByVal Sol_Kd As Double, ByVal WInConc As Double, ByRef leachate As Double, _
                               ByVal WFlux() As Double, ByRef soluteMass() As Double, ByVal BD() As Double, ByVal thickness() As Double, _
                               ByVal porosity() As Double, ByVal WCinitial() As Double)

        Dim C(totalLayers) As Double            'solute concentration at end of equilibrium, kg solute / m3
        Dim soluteConc As Double                'solute concentration, current layer, kg solute / m3
        Dim soluteFlow(totalLayers) As Double   'solute flow, kg solute per time step
        Dim waterInitial As Double              'initial layer soil water storage, kg / m2
        'Dim WaterFinal As Double                'final layer soil water storage, kg / m2
        'Dim WCFinal As Double                   'temporary variable to recalculate solute mass
        Dim R As Double                         'ratio of flux out of a layer to the layer porosity (or effective porosity)

        Dim soluteMassAdsorbed As Double        'kg/m2
        Dim soluteMassSolution As Double        'kg/m2

        soluteFlow(0) = WInConc * WFlux(0)

        For i As Integer = 1 To totalLayers
            soluteConc = 0
            soluteMassAdsorbed = 0
            waterInitial = thickness(i) * WD * WCinitial(i)
            'WaterFinal = WaterInitial + WFlux(i - 1) - WFlux(i)
            'WCFinal = WaterFinal / (Thickness(i) * WD)

            If soluteMass(i) > 0 Then soluteConc = LinearEquilibriumConcentration(Sol_Kd, BD(i), thickness(i), WCinitial(i), soluteMass(i))
 
            soluteMassAdsorbed = soluteMass(i) - waterInitial * soluteConc

            If WFlux(i) > 0 Then
                'note: the 0.67 is temporary, likely not needed, and if needed use a relationship with B
                R = 0.67 * WFlux(i) / (porosity(i) * thickness(i) * WD)
                soluteMassSolution = (soluteFlow(i - 1) / R + (waterInitial * soluteConc - soluteFlow(i - 1) / R) * Math.Exp(-R))
                soluteFlow(i) = waterInitial * soluteConc + soluteFlow(i - 1) - soluteMassSolution
                If i = totalLayers Then leachate = soluteFlow(i)
            Else
                soluteMassSolution = waterInitial * soluteConc + soluteFlow(i - 1)
                soluteFlow(i) = 0
            End If
            'this equilibrium can cause a mass balance error if "mass" is based on concentration at the end ignoring initial mass, not using it 
            'SoluteMass(i) = LinearEquilibriumSoluteMass(Sol_Kd, BD(i), Thickness(i), WCFinal, C(i))
            soluteMass(i) = soluteMassAdsorbed + soluteMassSolution
        Next

    End Sub
    Public Sub SoluteTransportEvaporation(ByVal totalLayers As Integer, ByVal Sol_Kd As Double, ByVal WFlux() As Double, _
                                          ByRef soluteMass() As Double, ByVal BD() As Double, ByVal thickness() As Double, _
                                          ByVal porosity() As Double, ByVal WCinitial() As Double)

        'this is a rough attempt to move solutes upward due to evaporation

        Dim soluteConc As Double                'solute concentration, current layer, kg solute / m3
        Dim waterInitial As Double              'initial layer soil water storage, kg / m2
        Dim soluteFlux(totalLayers) As Double

        For i As Integer = 2 To totalLayers
            waterInitial = thickness(i) * WD * WCinitial(i)

            If soluteMass(i) > 0 Then
                soluteConc = LinearEquilibriumConcentration(Sol_Kd, BD(i), thickness(i), WCinitial(i), soluteMass(i))
            Else
                soluteConc = 0
            End If

            soluteFlux(i) = WFlux(i) * soluteConc
        Next

        For i As Integer = totalLayers - 1 To 1 Step -1
            If soluteFlux(i + 1) > 0 Then
                soluteMass(i + 1) -= soluteFlux(i + 1)
                soluteMass(i) += soluteFlux(i + 1)
            End If
        Next

    End Sub

    'Private Sub SoluteExplicitTransport(ByVal infilt As Double, ByVal infiltConc As Double, ByVal dt As Double, ByVal snl As Integer, _
    '                                    ByVal vl As Integer, ByVal kQ As Double, ByRef soluteMass() As Double, ByVal BD() As Double, _
    '                                    ByVal layerThickness() As Double, ByVal WC() As Double, ByVal jl() As Double, _
    '                                    ByVal WTableSimulated As Boolean, ByRef soluteLeaching As Double)

    '    'vl is the number o layers passed INCLUDING (as the VLth layer) the one with the water table 

    '    Dim m As Integer                                        'm is the number of layers, last node for the solution, 
    '    Dim WTSimulated As Boolean
    '    Dim DXX(vl + 1) As Double                               'vector of known values
    '    Dim Ep(vl + 1) As Double                                'flux control variable to operate upwards(upwards flux to the node i is NEGATIVE, entering flux from below) or downward (flux downwards from the node i is POSITIVE, leaving the node towards the below node) fluxes
    '    Dim Ap(vl + 1) As Double                                'flux control variable to operate upwards(upwards flux to the node i is NEGATIVE, entering flux from below) or downward (flux downwards from the node i is POSITIVE, leaving the node towards the below node) fluxes
    '    Dim fw(vl + 1) As Double                                'water flux kg H2O/m2/s
    '    Dim Actual_fw(vl + 1) As Double                         'corrected layer flux to avoid negative concentrations
    '    Dim Sol_S(vl + 1) As Double                             'Solute concentration in soil mass kg solute/kg SOIL
    '    Dim Sol_C(vl + 1) As Double                             'Solute concentration in soil solution kg solute/kg water
    '    Dim Sol_CN(vl + 1) As Double                            'New solute concentration kg solute/kg water
    '    Dim dz(vl + 1) As Double                                'depth interval or thickness
    '    Dim SoilW(vl + 1) As Double                             'gravimetric water content kg H20/kg soil
    '    Dim Q(vl + 1) As Double                                 'layer variable, obtained from manipulation of the balance equation with S=(kQ+w)C, Q has units of flux

    '    Dim f As Double = 0                                     'f=0 is forward or totaly based on concentrations at the end of the time step
    '    Dim g As Double = 1 - f                                 'explicit or implicit degree for the numerical solution,

    '    Dim SoluteMassInitial As Double = 0                     'local mass balance control variables
    '    Dim SoluteMassFinal As Double = 0
    '    Dim SoluteMassLeached As Double = 0
    '    Dim SoluteMassBalance As Double = 0

    '    If vl < snl Then
    '        m = vl - 1 'last node in the solution is the one above water table, therefore vl is the vadose zone layers including the one with the water table
    '    Else
    '        m = vl
    '    End If

    '    For i As Integer = 1 To vl                              'load water flux and flux solute conditions for each layer; water table is a boundary conditions
    '        SoluteMassInitial += soluteMass(i)
    '        dz(i) = layerThickness(i)
    '        'SoilW(i) = WC(i) / BD(i)                            'to get kg water/kg soil from volumetric water content
    '        SoilW(i) = (jl(i) / 1000 / dz(i) + WC(i)) / BD(i)   'converts volumetric to gravimetric; uses the sum of flux + water content at the end of the time step
    '        fw(i) = jl(i) / dt                                  'to get kg m-2 s-1 (jl is in mm)
    '        Sol_S(i) = soluteMass(i) / (1000 * BD(i) * dz(i))   'kg solute / kg soil (layer solute mass in kg/m2)
    '        Sol_C(i) = Sol_S(i) / (kQ + SoilW(i))               'kg solute / kg water, using linear equilibrium isotherm equation
    '    Next

    '    dz(m + 1) = dz(m)                                       'check this
    '    If vl < 1 Then m = 1 'which condition is this? Armen

    '    'set up layer constants
    '    For i As Integer = 1 To m
    '        'DeltaZ(i) = (dz(i + 1) + dz(i)) / 2                 'what is this?
    '        Q(i) = 1000 * BD(i) * dz(i) * (kQ + SoilW(i)) / dt
    '        If jl(i) >= 0 Then Ap(i) = 1 : Ep(i) = 0
    '        If jl(i) < 0 Then Ap(i) = 0 : Ep(i) = 1
    '    Next

    '    Sol_CN(0) = 0
    '    fw(0) = infilt / dt                                     'upper boundary conditions
    '    Sol_C(0) = infiltConc                                   'upper boundary conditions, if rain then water salt=0 

    '    If jl(0) >= 0 Then                                      'upper boundary condition; infiltration > 0
    '        Ep(0) = 0                                           'infiltration in
    '        Ap(0) = 1
    '    Else
    '        Ep(0) = 1                                           'no infiltration
    '        Ap(0) = 0
    '    End If

    '    If WTSimulated Then
    '        Sol_CN(m + 1) = Sol_C(vl)                           'lower boundary conditions, salt concentration from the layer with water table
    '    Else
    '        Sol_CN(m + 1) = Sol_C(m)                            'lower boundary conditions
    '        Sol_C(m + 1) = Sol_C(m)                             'lower boundary conditions
    '    End If

    '    'set maximums layer fluxes to available equivalent-flux potentials to avoid negative concentration (in cases of very high fluxes)
    '    For i As Integer = 1 To m
    '        Actual_fw(i) = fw(i)

    '        If fw(i) > 0 And Q(i) < fw(i) Then
    '            Actual_fw(i) = Q(i)                             'layer flux is downward, Q is used to check and correct layer flux
    '        ElseIf fw(i) <= 0 And Q(i + 1) < fw(i) Then
    '            Actual_fw(i) = Q(i + 1)                         'layer flux is upward, layer+1 (Q i+1) is used to check and correct layer flux
    '        End If
    '    Next

    '    'set up matrix (in case of a non explicit solution is implemented)
    '    For i As Integer = 1 To m
    '        DXX(i) = Sol_C(i) * (Q(i) + Actual_fw(i - 1) * Ep(i - 1) * g - Actual_fw(i) * Ap(i) * g) _
    '                  + Actual_fw(i - 1) * Ap(i - 1) * g * Sol_C(i - 1) _
    '                  - Actual_fw(i) * Ep(i) * g * Sol_C(i + 1)
    '    Next

    '    'calculate solute leaching
    '    If fw(m) > 0 Then
    '        soluteLeaching += Sol_C(m) * Actual_fw(m) * dt
    '    Else
    '        soluteLeaching += Sol_C(m + 1) * Actual_fw(m) * dt 'upwards movement of solute from layer m+1 to m
    '    End If

    '    SoluteMassLeached = soluteLeaching                      'kg solute/m2

    '    For i As Integer = 1 To m
    '        Sol_CN(i) = DXX(i) / Q(i)                           'to get concentration, this is the balance equation with f=0
    '        Sol_C(i) = Sol_CN(i)
    '        Sol_S(i) = Sol_C(i) * (kQ + SoilW(i))               'to get kg solute/kg soil, HERE THE LINEAR EQUILIBRIUM ISOTHERM EQUATION
    '        soluteMass(i) = Sol_S(i) * (1000 * BD(i) * dz(i))   'to get back solute mass in kg/m2 to update the byref input
    '        SoluteMassFinal += soluteMass(i)
    '    Next

    '    SoluteMassBalance = SoluteMassInitial - (SoluteMassFinal + SoluteMassLeached)
    '    If Math.Abs(SoluteMassBalance) > 0.000001 Then Stop 'error in kg/m2 (about 100 g/ha)
    'End Sub

    Private Function LinearEquilibriumConcentration(ByVal Kd As Double, ByVal bulkDensity As Double, ByVal layerThickness As Double, _
                                                    ByVal waterContent As Double, ByVal soluteMass As Double) As Double

        Dim soilBufferPower As Double

        'soil buffer power, m3/m3
        'soil bulk density, kg/m3
        'kd is the slope of the adsortion isotherm, m3/kg
        'solute equilibrium concentration, kg solute / m3

        bulkDensity *= 1000 'convert Mg/m3 to kg/m3
        soilBufferPower = Kd * bulkDensity + waterContent

        Return soluteMass / (soilBufferPower * layerThickness * DailyClass.WATER_DENSITY)
    End Function

    Private Function LinearEquilibriumSoluteMass(ByVal Kd As Double, ByVal bulkDensity As Double, ByVal layerThickness As Double, _
                                                 ByVal waterContent As Double, ByVal concentration As Double) As Double
        'returns the total solute mass based on concentration in the solution
        Dim soilBufferPower As Double

        'soil buffer power, m3/m3
        'soil bulk density, kg/m3
        'kd is the slope of the adsortion isotherm, m3/kg
        'solute equilibrium concentration, kg solute / m3
        'solute mass, kg solute / m3

        bulkDensity *= 1000 'convert Mg/m3 to kg/m3
        soilBufferPower = Kd * bulkDensity + waterContent

        Return concentration * (soilBufferPower * layerThickness * DailyClass.WATER_DENSITY)
    End Function

End Class
