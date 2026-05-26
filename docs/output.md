# Output Files

Cycles offers a number of pre-formatted output files to inspect the agroecosystem processes simulated in the model.
The following pages provide documentation for the values printed in each output file.
To obtain customized outputs not included here, please contact a member of the Cycles development team.

## dailyEnviron.csv

| Column Header                 | Unit                          | Note |
| -------------                 | ----                          | ---- |
| date                          | YYYY-MM-DD                    | |
| min_air_temperature           | $^{\circ}\mathrm{C}$          | |
| max_air_temperature           | $^{\circ}\mathrm{C}$          | |
| average_air_temperature       | $^{\circ}\mathrm{C}$          | |
| soil_temperature.X            | $^{\circ}\mathrm{C}$          | Soil temperature of layer X. |
| vapor_pressure_deficit        | $\mathrm{kPa}$                | |
| reference_evapotranspiration  | $\mathrm{mm}/\mathrm{day}$    | Reference evapotranspiration calculated using the Penman-Monteith equation. |
| precipitation                 | $\mathrm{mm}$                 | |

## daily[crop].csv

Note: [crop] in the file name will be replaced by the name of each crop planted in the rotation as specified in the operation file (e.g., `dailyMaize.csv`).

| Column Header                         | Unit                          | Note |
| -------------                         | ----                          | ---- |
| date                                  | YYYY-MM-DD                    | |
| crop                                  | -                             | |
| stage                                 | -                             | Phenological stage.|
| thermal_time                          | $^{\circ}\mathrm{C}$ day      | Cumulative thermal time since the crop was planted. |
| vernalization                         | $^{\circ}\mathrm{C}$ day      | Cumulative vernalization since the crop was planted. Reports `-999` when winter crop phenology is not activated. |
| biomass                               | $\mathrm{Mg}/\mathrm{ha}$     | Total crop biomass, sum of aboveground and roots. |
| aboveground_biomass                   | $\mathrm{Mg}/\mathrm{ha}$     | |
| grain_biomass                         | $\mathrm{Mg}/\mathrm{ha}$     | Reports `-999` when grain model is not activated. |
| reserve_biomass                       | $\mathrm{Mg}/\mathrm{ha}$     | Reserve biomass that can be re-mobilized from shoot biomass to grain. Reports `-999` when grain model is not activated. |
| root_biomass                          | $\mathrm{Mg}/\mathrm{ha}$     | |
| harvest_index                         | -                             | Reports `-999` when grain model is not activated. |
| radiation_interception                | -                             | The fraction of solar radiation intercepted by the crop canopy. |
| total_nitrogen                        | $\mathrm{kg}/\mathrm{ha}$     | The total crop N content, sum of aboveground and roots. |
| aboveground_nitrogen                  | $\mathrm{kg}/\mathrm{ha}$     | |
| root_nitrogen                         | $\mathrm{kg}/\mathrm{ha}$     | |
| aboveground_nitrogen_concentration    | $\mathrm{g}/\mathrm{kg}$      | |
| nitrogen_fixation                     | $\mathrm{kg}/\mathrm{ha}$     | Cumulative N fixation by legume crops. |
| nitrogen_auto_added                   | $\mathrm{kg}/\mathrm{ha}$     | Cumulative N added to crop biomass to sustain growth in autofertilize mode. |
| o2_factor                             | -                             | Oxygen factor fro crop growth. |
| temperature_factor                    | -                             | Temperature factor for crop growth. |
| nitrogen_stress                       | $\%$                          | |
| water stress                          | $\%$                          | |
| potential_transpiration               | $\mathrm{mm}/\mathrm{day}$    | |


### annualSoilProfileC.csv

Note: This file provides annualized measurements of various carbon pools by soil layer.

| Column Header                 | Unit                          | Note |
| -------------                 | -----                         | ---- |
| year                          | YYYY                          | |
| residue_biomass_added.X       | $\mathrm{Mg}/\mathrm{ha}$     | Aboveground crop residue biomass added to the residue pool of layer X. |
| root_biomass_added.X          | $\mathrm{Mg}/\mathrm{ha}$     | Crop root and rhizodeposit biomass added to the residue pools of layer X. |
| residue_carbon_decomposed.X   | $\mathrm{Mg}/\mathrm{ha}$     | Quantity of C from the aboveground crop residue pool decomposed in layer X (Layer 1 includes decomposed surface residues, standing and flattened). |
| root_carbon_decomposed.X      | $\mathrm{Mg}/\mathrm{ha}$     | Quantity of C from the crop root and rhizodeposit residue pools decomposed in layer X. |
| initial_carbon.X              | $\mathrm{Mg C}/\mathrm{year}$ | The pool size of stabilized soil C in layer X at the beginning of the year. |
| humified_carbon.X             | $\mathrm{Mg C}/\mathrm{year}$ | Quantity of decomposed C added to microbial and stabilized C pools in layer X. |
| respired_carbon.X             | $\mathrm{Mg C}/\mathrm{year}$ | Quantity of C respired from the decomposition of microbial biomass and stabilized soil C in layer X. |
| final_carbon.X                | $\mathrm{Mg C}/\mathrm{year}$ | The pool size of stabilized soil C in layer X at the end of the year. |
| carbon_change.X               | $\mathrm{Mg C}/\mathrm{year}$ | The change in the stabilized soil C pool size of layer X between the beginning and end of the year. |


### annualSOM.csv

Note: This file provides annualized measurements of the carbon concentration and saturation ratio by soil layer.
One column will be created for each layer in the soil profile for the variables where the column heading indicates `LAYER 1..x`.

| Column Header             | Unit                          | Note |
| -------------             | -----                         | ---- |
| year                      | YYYY                          | |
| soil_thickness.X          | $\mathrm{m}$                  | Thickness of soil layer X. |
| bulk_density.X            | $\mathrm{Mg}/\mathrm{m}^3$    | Bulk density of soil layer X. |
| soil_carbon.X             | $\%$                          | Carbon concentration of soil layer X, only including the stabilized soil C pool. |
| carbon_saturation_ratio.X | $\mathrm{g}/\mathrm{g}$       | The saturation ratio of soil layer X, defined as the C content (or concentration) of the stabilized soil C pool divided by the maximum capacity for C stabilization. |


### annualN.csv

Note: This file provides annualized measurements of nitrogen fluxes for the sum of all layers in the soil profile.

| Column Header         | Unit                      | Note |
| -------------         | ----                      | ---- |
| year                  | YYYY                      | |
| fertilization         | $\mathrm{kg}/\mathrm{ha}$ | Total mass of N added as fertilizer. |
| auto_added            | $\mathrm{kg}/\mathrm{ha}$ | Total mass of N added to crop biomass to sustain growth in autofertilize mode. |
| fixation              | $\mathrm{kg}/\mathrm{ha}$ | Total N fixation by legume crops. |
| NO3_leaching          | $\mathrm{kg}/\mathrm{ha}$ | Nitrate leaching in drainage water at the bottom of the soil profile. |
| NH4_leaching          | $\mathrm{kg}/\mathrm{ha}$ | Ammonium leaching in drainage water at the bottom of the soil profile. |
| NO3_bypass            | $\mathrm{kg}/\mathrm{ha}$ | Nitrate leaching in drainage water due to horizontal bypass flow. |
| NH4_bypass            | $\mathrm{kg}/\mathrm{ha}$ | Ammonium leaching in drainage water due to horizontal bypass flow. |
| denitrification       | $\mathrm{kg}/\mathrm{ha}$ | Denitrification of nitrate. |
| nitrification         | $\mathrm{kg}/\mathrm{ha}$ | Nitrification of ammonium. |
| volatilization        | $\mathrm{kg}/\mathrm{ha}$ | Ammonia volatilization. |
| N2O_emission          | $\mathrm{kg}/\mathrm{ha}$ | Total nitrous oxide emissions from nitrification and denitrification. |
| nitrogen_in_harvest   | $\mathrm{kg}/\mathrm{ha}$ | N content in removed biomass, which is the sum of grain, forage, and/or removed residues. |

### dailySoilC.csv

Note: Results in this file are for the sum of all layers in the soil profile, including surface residues.

| Column Header                         | Unit                      | Note |
| --------------                        | ----------                | ----------- |
| date                                  | YYYY-MM-DD                | |
| soil_organic_carbon                   | $\mathrm{Mg}/\mathrm{ha}$ | The sum of microbial biomass C and stabilized soil C pools. |
| humified_carbon                       | $\mathrm{Mg}/\mathrm{ha}$ | The C from all decomposed substrates, including residues, microbial biomass C, and stabilized soil C that becomes humified in microbial biomass C or stabilized soil C pools. |
| residue_respired_carbon               | $\mathrm{Mg}/\mathrm{ha}$ | The C from all decomposed residues, including crop residues, manure, and rhizodeposits, that is respired by microbes. |
| soil_organic_matter_respired_carbon   | $\mathrm{Mg}/\mathrm{ha}$ | The C from decomposed microbial biomass C and stabilized soil C that is respired by microbes. |

### dailyN.csv

Note: Results in this file are for the sum of all layers in the soil profile, including surface residues.

| Column Header             | Unit                          | Note |
| -------------             | ----                          | ---- |
| date                      | YYYY-MM-DD                    | |
| soil_organic_nitrogen     | $\mathrm{kg N}/\mathrm{ha}$   | The sum of microbial biomass N and stabilized soil organic N pools. |
| profile_soil_NO3          | $\mathrm{kg N}/\mathrm{ha}$   | |
| profile_soil_NH4          | $\mathrm{kg N}/\mathrm{ha}$   | |
| mineralization            | $\mathrm{kg N}/\mathrm{ha}$   | Gross N mineralization from all decomposing organic substrates. |
| immobilization            | $\mathrm{kg N}/\mathrm{ha}$   | Gross N immobilization in microbial biomass or stabilized soil organic N pools. |
| net_mineralization        | $\mathrm{kg N}/\mathrm{ha}$   | |
| nitrification             | $\mathrm{kg N}/\mathrm{ha}$   | |
| N2O_from_nitrification    | $\mathrm{kg N}/\mathrm{ha}$   | |
| volatilization            | $\mathrm{kg N}/\mathrm{ha}$   | |
| denitrification           | $\mathrm{kg N}/\mathrm{ha}$   | |
| N2O_from_denitrification  | $\mathrm{kg N}/\mathrm{ha}$   | |
| NO3_leaching              | $\mathrm{kg N}/\mathrm{ha}$   | Nitrate leaching in drainage water at the bottom of the soil profile. |
| NH4_leaching              | $\mathrm{kg N}/\mathrm{ha}$   | Ammonium leaching in drainage water at the bottom of the soil profile. |
| NO3_bypass                | $\mathrm{kg N}/\mathrm{ha}$   | Nitrate leaching in drainage water due to horizontal bypass flow. |
| NH4_bypass                | $\mathrm{kg N}/\mathrm{ha}$   | Ammonium leaching in drainage water due to horizontal bypass flow. |


### dailyResidue.csv

Note: Results in this file are for the sum of all layers in the soil profile, including surface residues.

| Column Header                 | Unit                      | Note |
| ---------------               | ----                      | ---- |
| date                          | YYYY-MM-DD                | |
| radiation_interception        | -                         | The fraction of solar radiation intercepted by standing and flattened crop residues on the soil surface. |
| aboveground_residue_biomass   | $\mathrm{Mg}/\mathrm{ha}$ | The biomass of standing and flattened crop residues remaining on the soil surface. |
| belowground_residue_biomass   | $\mathrm{Mg}/\mathrm{ha}$ | The remaining biomass of crop residue shoots that were incorporated into the soil profile. |
| root_residue_biomass          | $\mathrm{Mg}/\mathrm{ha}$ | The remaining biomass of crop residue roots and rhizodeposits in the soil profile. |
| surface_manure_carbon         | $\mathrm{Mg}/\mathrm{ha}$ | The C content of manure remaining on the soil surface. |
| surface_ferment_carbon        | $\mathrm{Mg}/\mathrm{ha}$ | The C content of manure remaining on the soil surface. |
| aboveground_residue_nitrogen  | $\mathrm{kg}/\mathrm{ha}$ | The N content of standing and flattened crop residues remaining on the soil surface. |
| belowground_residue_nitrogen  | $\mathrm{kg}/\mathrm{ha}$ | The remaining N content of crop residue shoots that were incorporated into the soil profile. |
| root_residue_nitrogen         | $\mathrm{kg}/\mathrm{ha}$ | The remaining N content of crop residue roots and rhizodeposits in the soil profile. |
| surface_manure_nitrogen       | $\mathrm{Mg}/\mathrm{ha}$ | The N content of manure remaining on the soil surface. |
| surface_ferment_nitrogen      | $\mathrm{Mg}/\mathrm{ha}$ | The N content of manure remaining on the soil surface. |
| standing_residue_moisture     | $\mathrm{kg}/\mathrm{kg}$ | Standing surface residue moisture content defined as: $m_w / \left(m_w + m_r\right)$, where $m_w$ and $m_r$ are mass of water and residue, respectively. |
| flat_residue_moisture         | $\mathrm{kg}/\mathrm{kg}$ | Flattened surface residue moisture content defined as: $m_w / \left(m_w + m_r\right)$. |


### dailyWater.csv

| Column Header             | Unit                          | Note |
| -------------             | ----                          | ---- |
| date                      | YYYY-MM-DD                    | |
| irrigation                | $\mathrm{mm}/\mathrm{day}$    | Water added as irrigation. |
| runoff                    | $\mathrm{mm}/\mathrm{day}$    | Water runoff from the soil surface. |
| infiltration              | $\mathrm{mm}/\mathrm{day}$    | Water infiltration into the soil. |
| drainage                  | $\mathrm{mm}/\mathrm{day}$    | Water drainage from the bottom of the soil profile. |
| bypass                    | $\mathrm{mm}/\mathrm{day}$    | Water drainage due to horizontal bypass flow. |
| soil_evaporation          | $\mathrm{mm}/\mathrm{day}$    | Evaporation from the soil surface. |
| residue_evaporation       | $\mathrm{mm}/\mathrm{day}$    | Evaporation from the surface residue. |
| snow_sublimation          | $\mathrm{mm}/\mathrm{day}$    | Sublimation from snow cover. |
| transpiration             | $\mathrm{mm}/\mathrm{day}$    | Transpiration by the crop canopy. |
| soil_moisture_content.X   | $\mathrm{m}^3/\mathrm{m}^3$   | Soil moisture content of layer X. |

 
### harvest.csv

The `harvest.csv` file provides information about each crop harvest.

| Column Header                     | Unit                      | Note |
| --------------                    | ----                      | ---- |
| date                              | YYYY-MM-DD                | |
| crop                              | -                         | |
| planting_date                     | -                         | |
| total_biomass                     | $\mathrm{Mg}/\mathrm{ha}$ | Total biomass (roots + aboveground) accumulated by the crop at harvest. |
| root_biomass                      | $\mathrm{Mg}/\mathrm{ha}$ | Root biomass accumulated by the crop at harvest. |
| grain_yield                       | $\mathrm{Mg}/\mathrm{ha}$ | |
| forage_yield                      | $\mathrm{Mg}/\mathrm{ha}$ | Forage yield or removed residue biomass at harvest. |
| aboveground_residue               | $\mathrm{Mg}/\mathrm{ha}$ | Aboveground residue biomass left in the field at harvest. |
| harvest_index                     | $\mathrm{Mg}/\mathrm{Mg}$ | Fraction of aboveground biomass harvested as grain. |
| potential_transpiration           | \mathrm{mm}               | Potential water transpiration of the crop under non-limiting soil moisture conditions. |
| actual_transpiration              | \mathrm{mm}               | Actual water transpiration of the crop given soil moisture conditions in the simulation. |
| irrigation                        | \mathrm{mm}               | Water added as irrigation. |
| total_nitrogen                    | $\mathrm{Mg}/\mathrm{ha}$ | Total biomass N (roots + aboveground) content accumulated by the crop at harvest. |
| root_nitrogen                     | $\mathrm{Mg}/\mathrm{ha}$ | Root biomass N accumulated by the crop at harvest. |
| grain_nitrogen                    | $\mathrm{Mg}/\mathrm{ha}$ | |
| forage_nitrogen                   | $\mathrm{Mg}/\mathrm{ha}$ | Forage or removed residue N content at harvest. |
| cumulative_nitrogen_stress        | -                         | Cumulative N stress over the duration of crop growth, calculated as the sum of each daily N stress weighted by the daily fraction of thermal time accumulated to maturity. |
| nitrogen_in_harvest               | $\mathrm{kg}/\mathrm{ha}$ | N content in removed biomass, which is the sum of grain, forage, and/or removed residues. |
| nitrogen_in_residue               | $\mathrm{kg}/\mathrm{ha}$ | N content in residues left in the field at harvest. |
| nitrogen_concentration_in_forage  | $\%$                      | The N concentration in forage or removed residues at harvest. |
| nitrogen_fixation                 | $\mathrm{kg}/\mathrm{ha}$ | Cumulative N fixation by legume crops. |
| nitrogen_auto_added               | $\mathrm{kg}/\mathrm{ha}$ | Cumulative N added to crop biomass to sustain growth in autofertilize mode. |

 
### dailySoilLayersCN.csv

Note: This file provides outputs needed to calculate selected C and N pool sizes or elemental concentrations by soil layer.

| Column Header             | Unit                          | Description |
| -------------             | ----                          | ----------- |
| date                      | YYYY-MM-DD                    | |
| NO3.X                     | $\mathrm{mg}/\mathrm{kg}$     | Nitrate concentration of layer X. |
| NH4.X                     | $\mathrm{mg}/\mathrm{kg}$     | Ammonium concentration of layer X. |
| composite_factor.X        | $\mathrm{-}/\mathrm{-}$       | The composite environmental factor downregulating the maximum decomposition rate of each C pool in layer X; a function of temperature, moisture, and aeration. |
| standing_residue_carbon   | $\mathrm{Mg}/\mathrm{ha}$     | The C content of standing residue on the soil surface. |
| flat_residue_carbon       | $\mathrm{Mg}/\mathrm{ha}$     | The C content of flattened residue on the soil surface. |
| manure_residue_carbon     | $\mathrm{Mg}/\mathrm{ha}$     | The C content of manure on the soil surface. |
| ferment_residue_carbon    | $\mathrm{Mg}/\mathrm{ha}$     | The C content of manure on the soil surface. |
| residue_carbon.X          | $\mathrm{Mg}/\mathrm{ha}$     | The total C content of residues in layer X, including aboveground crop residues, root residues and rhizodeposits, and manure. |
| standing_residue_C:N      | $\mathrm{g}/\mathrm{g}$       | The C:N ratio of standing residue on the soil surface. |
| flat_residue_C:N          | $\mathrm{g}/\mathrm{g}$       | The C:N ratio of flattened residue on the soil surface. |
| manure_residue_C:N        | $\mathrm{g}/\mathrm{g}$       | The C:N ratio of manure on the soil surface. |
| ferment_residue_C:N       | $\mathrm{g}/\mathrm{g}$       | The C:N ratio of manure on the soil surface. |
| residue_C:N.X             | $\mathrm{g}/\mathrm{g}$       | The C:N ratio of residues in layer X, including aboveground crop residues, root residues and rhizodeposits, and manure. |
| microbial_carbon.X        | $\mathrm{Mg}/\mathrm{ha}$     | The C content of microbial biomass in layer X. |
| microbial_C:N.X           | $\mathrm{g}/\mathrm{g}$       | The C:N ratio of microbial biomass in layer X. |
| soil_organic_carbon.X     | $\mathrm{Mg}/\mathrm{ha}$     | The C content of stabilized soil organic matter in layer X. |
| soil_organic_C:N          | $\mathrm{g}/\mathrm{g}$       | The C:N ratio of stabilized soil organic matter in layer X. |
| soil_thickness.X          | $\mathrm{m}                   | The thickness of layer X. |
| bulk_density.X            | $\mathrm{Mg}/\mathrm{m}^3$    | The bulk density of layer X. |
 