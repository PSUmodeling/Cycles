# Cycles: An Agroecosystems Simulation Model
# *User Reference Guide*

Prepared by Charles White, Yuning Shi, and Armen Kemanian

## Contents
[Introduction to Using Cycles](#introduction-to-using-cycles)

[Input Files](#input-files)

- [Simulation Control File](#simulation-control-file-ctrl)
- [Weather File](#weather-file-weather)
- [Soil File](#soil-file-soil)
- [Crop Description File](#crop-description-file-crop)
- [Management Operations File](#management-operations-file-operation)
- [Re-initialization File](#reinitialization-file-reinit)
- [Multi-mode File](#multi-mode-file)

[Output Files](#output-files)

[Bug Report Guidelines](#bug-report-guidelines)

## Introduction to Using Cycles

Cycles is a daily time-step agro-ecosystem model that simulates the biophysical processes and management practices occurring within cropping systems and other land uses.
Processes include fluxes in the water and energy balance, the coupled cycling of carbon and nitrogen, and plant growth.
The model can simulate a wide range of agricultural management practices such as tillage, organic and inorganic nutrient additions, annual and perennial crops, crop harvests as grain or forages, polycultures and relay cropping, grazing, and irrigation.
Crop growth is represented with a generalizable framework such that a nearly limitless variety of agricultural crop species can be specified by the user.

Cycles is written in C and the source code is released from the [Cycles GitHub repository](https://github.com/PSUmodeling/Cycles).
The [release page](https://github.com/PSUmodeling/Cycles/releases) contains compiled executables for different operating systems.
Text-based input files that specify the simulation control parameters, a soil profile description, crop descriptions, the sequence of management operations, and weather drive each user-defined simulation.
An optional re-initialization file can be used to reset model variables to desired values as described in the re-initialization file.
Outputs for various pools and fluxes in the agro-ecosystem are written to tab-delimited text files that can be opened by most spreadsheet programs.

To get started running the model, download the package based on your operating system from the [release page](https://github.com/PSUmodeling/Cycles/releases) under the "Assets" menu and decompress the package to a working directory of your choice.
For Windows users, please use `Cycles_win_vXXX.zip`, Mac users `Cycles_macos_vXXX.zip`, and Unix users `Cycles_debian_vXXX.zip`.

The input directory is where you store the various input files needed to drive each simulation.
Each simulation needs a control file (\*.ctrl), an operation file (\*.operation), a soil profile description file (\*.soil), a crop description file (\*.crop), and a weather file (\*.weather).
A re-initialization file (\*.reinit) is optional.
Each simulation you run should have a uniquely-named control file, but it is possible to share a single operation file, soil description file, crop description file, or weather file across multiple simulations.

Each of the input files is described in more detail below, but assuming that the input files are prepared and located in the `input` directory, Cycles is launched as follows.

1. Navigate to the `Cycles` working directory.

2. Once you are in the directory, type the following into the command line and hit enter:

   ```shell
   ./Cycles <simulation name>
   ```

   or, in Windows:

   ```shell
   Cycles_win.exe <simulation name>
   ```

   The string `<simulation name>` should correspond to the file name of the simulation control file, not including the `.ctrl` suffix.
   For instance, the command used to run the simulation specified by the `ContinuousCorn.ctrl` file is

   ```shell
   ./Cycles ContinuousCorn
   ```

   or in Windows,

   ```shell
   Cycles_win.exe ContinuousCorn
   ```

   You can also specify whether to run the model in brief mode, verbose mode, or debugging mode, by adding `-b`, `-v` or `-d` parameters, respectively, as in:

   ```shell
   ./Cycles [-bvd] <simulation name>
   ```

   or, in Windows:

   ```shell
   Cycles_win.exe [-bvd] <simulation name>
   ```

   Brief mode has minimal screen output.
   Verbose mode has verbose screen output for users to understand the simulation processes.
   Debugging mode is the most useful in Unix operating systems, stopping the simulation when NAN or overflow errors occur.

3. Cycles includes a "multi-mode" feature that enables users to run batch simulations.
   In multi-mode, a master control file (or [multi-mode file](#multi-mode-file)) in the input folder is required.
   To start batch simulation as described in the multi-mode file:

   ```shell
   ./Cycles -m <multi-mode file name>
   ```

   or, in Windows:

   ```shell
   Cycles_win.exe -m <multi-mode file name>
   ```

4. Cycles includes a spin-up feature that enables users to run the model to equilibrium.
   In spin-up mode, the model recycles the weather forcing and operations between the specified simulation start year and end year, until the model reaches equilibrium (i.e., the change in total soil organic carbon before and after the simulation is below 1%).
   The spin-up is followed by a simulation starting from equilibrium.
   No output file is written during the spin-up.
   Only the last simulation from equilibrium produces model output.
   The spin-up simulation also generates a `<simulation name>.ss` file in the `input` directory, which contains the soil conditions at equilibrium, and can be used to drive other simulations.
   To start a simulation with spin-up:

   ```shell
   ./Cycles -s <simulation name>
   ```

   or, in Windows:

   ```shell
   Cycles_win.exe -s <simulation name>
   ```

5. Cycles includes a "baseline simulation" mode.
   The baseline simulation generates a re-initialization file at specified day of year (DOY) when it runs.
   The generated re-initialization file can be used to for other simulations, resetting model variables to the "baseline" values.
   To start a baseline simulation that generates re-initialization for a specified DOY:

   ```shell
   ./Cycles -l <DOY> <simulation name>
   ```
   or, in Windows:

   ```shell
   Cycles_win.exe -l <DOY> <simulation name>
   ```

   The baseline simulation can also spin-up.
   Only the equilibrium simulation will generate re-initialization variables.

Outputs from the model are written to files located in a subdirectory named `output`.
If the `output` subdirectory does not already exist within your working directory, the program will create it.
To keep your outputs organized, the program creates a new subdirectory within the `output` directory with the same name as the simulation control file.
In baseline mode the generated re-initialization file can be found in the output directory as `reinit.dat`.
If you run the same simulation control file multiple times, the program will **overwrite** output files in the folder each time.

**Windows users:** It is important to note that if any of the output files are open in a spreadsheet software when a new simulation is run, those files will not be overwritten successfully.
To save an existing output file within the `output` subdirectory and prevent the results from being overwritten by subsequent simulation runs, you can manually change the name of the file in Windows Explorer, for instance by appending a keyword of your choice to the filename.

The remainder of the manual will describe the structure of the input files needed to run a simulation and provide documentation of the results printed to the various output files.

[(Back to top)](#contents)

## Input Files

Simulations in Cycles are driven by a series of ASCII-text input files.
However, the simulation control file (`*.ctrl`) is the only input file explicitly specified when executing Cycles in the command prompt terminal or command line.
The names of inputs files for the crop descriptions, management operations, soil profile description, and weather records are specified within the control file.
Generally speaking, the input files must be written in exactly the same format as the master template.
Users can write custom annotations within input files, however.
Any text following a \# mark will not be read by the program.
Blank lines are also not read by the program
To create input files for a new simulation, it’s easiest to make a copy of existing template files, rather than retype each file from scratch.

[(Back to top)](#contents)

### Simulation Control File (\*.ctrl)

The simulation control files have a suffix of `.ctrl`.
For each simulation, the name of the control file is submitted when invoking Cycles at the command prompt and is also used as the directory name where output files are stored following each simulation.
In the control file, keyword tags are on the left and the value for each tag is separated by spaces or tabs on the right.
The control file contains information about the starting and ending years of the simulation, simulation options, including the specific output files to be written, and the names of other input files.
Simulation options are represented as on/off switches, where `0` stands for off and `1` stands for on.
The meaning of each keyword in the control file is described below.

#### `SIMULATION_START_YEAR`

The year to start the simulation.
All simulations start on January 1 of the given year.
There must be records in the weather file starting from January 1 of this year.

#### `SIMULATION_END_YEAR`

The year to end the simulation.
All simulations run to December 31 of the given year.
There must be records in the weather file through December 31 of this year.

#### `ROTATION_SIZE`

The number of years in the crop rotation specified in the operation input file.
The specified rotation will automatically repeat itself as many times as needed over the duration of the simulation spanning from `SIMULATION_START_YEAR` to `SIMULATION_END_YEAR`.

#### `USE_REINITIALIZATION`

Set to `1` if reinitialization of carbon, nitrogen or water variables are desired on a specific day of simulation year.
A re-initialization file is required in this case.
Set to `0` if re-initialization is not needed.

#### `ADJUSTED_YIELDS`

This keyword is currently not activated.
Enter `0`.

#### `HOURLY_INFILTRATION`

Controls whether water infiltration and redistribution between soil layers is calculated using the daily cascade method (set value to `0`) or a finite difference numerical integration at an hourly time-step (set value to `1`).
The cascade method is the most computationally efficient, but does not allow for water contents greater than field capacity or soil wetting from the bottom up.
Cascade method can miss denitrification badly, but do a good job with the water balance.

#### `AUTOMATIC_NITROGEN`

Set to `1` for all crops in the simulation to be grown without nitrogen limitations.
Set to `0` for crops to be grown based on N available from N cycling processes.

#### `AUTOMATIC_PHOSPHORUS`

Crops grow with no phosphorus limitations.
(Note: phosphorus cycling is currently not represented in the model, so this switch does not activate anything.)

#### `AUTOMATIC_SULFUR`

Crops grow with no sulfur limitations.
(Note: sulfur cycling is currently not represented in the model, so this switch does not activate anything.)

#### `DAILY_WEATHER_OUT`

Set to `1` to write an output file named `weather.dat` with the average temperature, reference evapotranspiration, and precipitation for each day of the simulation.

#### `DAILY_CROP_OUT`

Set to `1` to write an output file named for each crop used in the rotation (e.g., `Maize.dat`) with daily values for growing degree day accumulation, crop biomass production, biomass N content, crop N stress and water stress, and potential transpiration.

#### `DAILY_RESIDUE_OUT`

Set to `1` to write an output file named `residue.dat` with daily values associated with the residue pools, including radiation interception by surface residues, surface residue moisture content, shoot and root residue biomass and N content in the entire soil profile, and manure C and N content at the surface and in the soil profile.

#### `DAILY_WATER_OUT`

Set to `1` to write an output file named `water.dat` with daily values for the water budget, including inputs from irrigation and rainfall, and outputs of drainage, runoff, evaporation, transpiration, and sublimation.

#### `DAILY_NITROGEN_OUT`

Set to `1` to write an output file named `N.dat` with daily values for pools and fluxes in the N cycle, including soil profile stocks of organic N, nitrate N, and ammonium N, and rates of mineralization and immobilization, nitrification, gaseous N losses, and leaching losses.

#### `DAILY_SOIL_CARBON_OUT`

Set to `1` to write an output file named `soilC.dat` with daily values for pools and fluxes in the C cycle, including soil profile stocks of organic C, the C from decomposed organic matter that becomes humified in microbial and stabilized C pools, and C respired from decomposed residues or soil organic matter.

#### `DAILY_SOIL_LYR_CN_OUT`

Set to `1` to write an output file named `soilLayersCN.dat` with daily values for C and N pool sizes by soil layer.
Included are nitrate and ammonium concentrations, the composite environmental factor regulating decomposition rates, residue C and C:N ratio, microbial biomass C and C:N ratio, stabilized organic C and C:N ratio, soil water content, and soil layer thickness and bulk density.

#### `ANNUAL_SOIL_OUT`

Set to `1` to write an output file named `annualSOM.dat` with annual values for soil carbon pool sizes and carbon saturation ratios by soil layer.

#### `ANNUAL_PROFILE_OUT`

Set to `1` to write an output file named `annualSoilProfileC.dat` with annual values of carbon pools by soil layer.

#### `ANNUAL_NFLUX_OUT`

Set to `1` to write and output file named `annualN.dat` with annual values of nitrogen fluxes, including fertilization, fixation, leaching, denitrification, nitrification, and volatilization.

#### `CROP_FILE`

The name of the crop description file.
This file must be located in a directory titled `input` that is within the `Cycles` working directory.

#### `OPERATION_FILE`

The name of the operation file.
This file must be located in a directory titled `input` that is within the `Cycles` working directory.

#### `SOIL_FILE`

The name of the soil profile description file.
This file must be located in a directory titled `input` that is within the `Cycles` working directory.

#### `WEATHER_FILE`

The name of the weather file.
This file must be located in a directory titled `input` that is within the `Cycles` working directory.

#### `REINIT_FILE`

The name of the reinitialization file.
This file must be located in a directory titled `input` that is within the `Cycles` working directory.
Note that it cannot be left as blank, even if reinitialization is not needed.
Use `N/A` if reinitialization is not needed.

[(Back to top)](#contents)

### Weather File (\*.weather)

The weather files typically have a suffix of `.weather`, but any suffix or file naming convention can be used so long as the name of the weather file listed in the control file exactly matches the name of the weather file to be used in the input directory.

Weather files contain three lines at the beginning with the keyword tags `LATITUDE`, `ALTITUDE`, and `SCREENING_HEIGHT`, each followed by a tab-delimited value.
Altitude and screening height values should be entered in meters.
Screening height is the height of the weather station instruments above the land surface.
Following these three lines is a row of tab-delimited column headers in the order `YEAR`, `DOY`, `PP`, `TX`, `TN`, `SOLAR`, `RHX`, `RHN`, `WIND`, with daily values for each weather variable listed in rows below.
`YEAR` and `DOY` are the year and numerical day of the year, respectively, for each daily weather record.
`PP` is precipitation in mm day<sup>-1</sup>.
`TX` and `TN` are the maximum and minimum daily temperatures in degrees Celsius.
`SOLAR` is the daily solar radiation in MJ m<sup>-2</sup> day<sup>-1</sup>.
`RHX` and `RHN` are the maximum and minimum relative humidity in %.
`WIND` is the average wind speed in m s<sup>-1</sup>.

[(Back to top)](#contents)

### Soil File (\*.soil)

Similar to the weather input file, soil files typically have a suffix of `.soil`, but any naming convention can be used as long as it matches the soil file name listed in the control file.

The soil file starts with three lines at the beginning with the keyword tags `CURVE_NUMBER`, `SLOPE`, and `TOTAL_LAYERS`, with each keyword followed by a tab-delimited value.

The `CURVE_NUMBER` refers to the United States Department of Agriculture Natural Resources Conservation Service Curve Number method to calculate runoff from watersheds.
The curve number is used in an equation to predict the volume of water from precipitation or irrigation that is necessary to generate runoff from a field.
Lower values indicate faster infiltration rates, meaning that a greater volume of precipitation is needed to generate runoff.
The exact value to use in a soil input file for a given simulation depends on the soil texture and crop rotation characteristics, as described in the NRCS technical report TR-55, "[Urban Hydrology for Small Watersheds](https://www.nrcs.usda.gov/Internet/FSE_DOCUMENTS/stelprdb1044171.pdf)".
Table 2.2 in the document suggests curve numbers to use for different hydrologic soil groups (A, B, C, or D) under different crop rotations.
If the hydrologic soil group is not known for a soil, Appendix A of TR-55 provides qualitative descriptions of the different hydrologic groups and assigns a hydrologic soil group rating to different soil texture classes (i.e., loams, silt loams, sandy loams, etc.).
The curve number rating will affect the water budget in a simulation, which can affect crop yields and other processes, so its value should be given due consideration, especially if you are attempting to compare simulation outputs with results from a real field experiment.

The `SLOPE` value is in %, or units of rise per 100 units of run.
The `TOTAL_LAYERS` value is simply the total number of soil layers described in the profile.

Below the first three keyword tags is a row of column headers for soil properties in the order `LAYER`, `THICK`, `CLAY`, `SAND`, `ORGANIC`, `BD`, `FC`, `PWP`, `NO3`, `NH4`, `BYP_H`, and `BYP_V`.
The values of each soil property are listed below in a separate row for each soil layer.
The variable `LAYER` is simply the layer number, starting at `1` and increasing by one unit for each additional layer.
`THICK` is the thickness of each soil layer in meters.
Because of the important processes occurring in the topsoil layers, such as gas and energy exchange with the atmosphere and incorporation of crop residues, it is necessary to have the topsoil layers simulated with greater depth resolution than the subsoils.
An optimal depth for the first soil layer is 0.05 m, with subsequent topsoil layers entered in increments between 0.05 m and 0.10 m depth.
Below the topsoil layers, we suggest that soil layers be defined based on depth segments corresponding to changes in soil horizonation for the soil type to be simulated.

The keyword tags `CLAY` and `SAND` are the clay particle size fraction and sand particle size fraction of each soil layer in %.

`ORGANIC` is the organic matter concentration of each layer in %.
The organic matter concentration of each layer is converted to C and N concentration to initialize the stabilized soil C and N pools when the simulation is started, using a conversion of 0.58 %C per % organic matter and a soil organic matter C:N ratio of 10:1.
The microbial biomass C and N pools are also initialized as 3% of the pool sizes of stabilized soil C and N.

`BD` is bulk density in Mg/m<sup>3</sup>, and `FC` and `PWP` are the field capacity and permanent wilting point volumetric water contents, respectively, in m<sup>3</sup>/m<sup>3</sup>.
If any of these values are not known for a soil layer, you can enter -999 and the model will estimate the values using pedotransfer functions.

`NO3` and `NH4` are the nitrate and ammonium masses in kg/ha for each soil layer (important: the values are absolute masses, not concentrations).

`BYP_H` and `BYP_V` are the fractions of horizontal and vertical bypass flows in each soil layer.

[(Back to top)](#contents)

### Crop Description File (\*.crop)

The crop description file defines physiological and management parameters that control the growth and harvest of crops used in the simulation.
Each crop used in the simulation must have an entry in the crop description file.
Each crop entry begins with the keyword tag `NAME` and is followed by the keywords listed in exactly the order below.

#### `NAME`

The name of the crop.
This entry is used as the reference for crop management practices listed in the operation file, such as planting and harvesting.

#### `FLOWERING_TT`

The thermal time to flowering, in growing degree C days, calculated using the base, optimum, and maximum temperatures for development listed in `BASE_TEMPERATURE_FOR_DEVELOPMENT`, `OPTIMUM_TEMPERATURE_FOR_DEVELOPEMENT`, and `MAX_TEMPERATURE_FOR_DEVELOPMENT`.

#### `MATURITY_TT`

The thermal time to maturity in growing degree C days.

#### `MAXIMUM_SOIL_COVERAGE`

The maximum crop cover in % is a proxy for light interception potential given by row spacing and other features.
This value can be set to less than 100% if the crop canopy does not have the potential to provide full cover, or if the crop is planted with a row spacing such that there will be bare ground present between crop rows (e.g., horticultural production or dry areas).

#### `MAXIMUM_ROOTING_DEPTH`

The maximum depth to which crop roots can grow in meters.

#### `AVERAGE_EXPECTED_YIELD`

This parameter is not currently used.
Leave at `1.0`.

#### `MAXIMUM_EXPECTED_YIELD`

This parameter is not currently used.
Leave at `1.0`.

#### `MINIMUM_EXPECTED_YIELD`

This parameter is not currently used.
Leave at `1.0`.

#### `COMMERCIAL_YIELD_MOISTURE`

This parameter is not currently used.
Leave at `0.0`.

#### `STANDING_RESIDUE_AT_HARVEST`

The percent of aboveground crop residues remaining in the field that are left in the standing position (i.e., in minimal contact with the soil surface and therefore decomposing at a slower rate).
The reminder of the reside will lay flat on the ground.

#### `RESIDUE_REMOVED`

For crops harvested as grain, the percent of non-grain crop biomass that is removed with the harvested grain.
For crops harvested as forages or grazed through the clipping controls, the percent of harvestable aboveground plant biomass that is removed by the harvest.
For clipping events, this value also controls the fraction of root biomass that is killed at each clipping event and the extent to which the thermal time of crop development is set back at each clipping event.

Interactions: The harvestable aboveground plant biomass is the biomass quantity above the `CLIPPING_BIOMASS_THRESHOLD_LOWER` value.

#### `CLIPPING_BIOMASS_THRESHOLD_UPPER`

The aboveground plant biomass threshold (in Mg/ha) that will trigger a clipping event or forage harvest.

Interactions: Clipping will only be allowed if the clipping date window for the crop, which is defined in the operation file, is open.
The `HARVEST_TIMING` value also controls clipping events, and clipping will occur when either the biomass threshold or the `HARVEST TIMING` threshold is reached, whichever occurs first.
The destination of clippings, either returned to the soil surface, harvested, or grazed is controlled by the `CLIPPING_BIOMASS_DESTINY` option.
The percent of aboveground biomass that is clipped is controlled by the `RESIDUE_REMOVED` value.

#### `CLIPPING_BIOMASS_THRESHOLD_LOWER`

The aboveground plant biomass threshold (in Mg/ha), which remains un-harvestable during clipping events.
This value represents the mass of living plant biomass that would remain in the field after a clipping event with a `RESIDUE_REMOVED` value of 100.

#### `HARVEST_TIMING`

The percent of thermal time to crop maturity that will trigger a clipping event or grain harvest.
As an example, if `HARVEST_TIMING` is set to 90, and the `MATURITY_TT` is 1000, the crop will be clipped when the cumulative thermal time of crop development reaches 900 growing degree days.
If the harvest timing is less than 100, the crop will be clipped as a forage and will continue to regrow.
If the harvest timing is greater than 100, the crop will have reached physiological maturity and will be harvested as a grain crop and killed after harvested.
A value of `-999` can be entered to trigger a default value of 101, which will result in the crop being harvested for grain on the first day after it reaches physiological maturity.

Interactions: The `CLIPPING_BIOMASS_THRESHOLD_UPPER` value also controls if and when a crop will be clipped.
If the `HARVEST_TIMING` value is greater than 100, indicating a desire to harvest the crop for grain, but the `CLIPPING_BIOMASS_THRESHOLD_UPPER` value is set within a range of biomass production levels that will be achieved during growth of the crop, the crop will get clipped as a forage.
Therefore, when programming a crop to be grown as a grain crop, the `CLIPPING_BIOMASS_THRESHOLD_UPPER` value should be set to a very high value, such as `999`, so that clipping will not be triggered before physiological maturity is reached.

#### `KILL_AFTER_HARVEST`

Whether to kill the crop after automatic grain or forage harvests.
Note that crops will not be killed after scheduled harvests in the operation files.
To kill the crops after scheduled harvests, `KILL_CROP` tillage operations need to be scheduled.

#### `CLIPPING_BIOMASS_DESTINY`

This value controls the destiny of biomass cut by clipping events, with options of `REMOVE`, `RETURN`, or `GRAZING`.
`REMOVE` will treat the clipped biomass as a harvested crop.
`RETURN` will treat the clipped biomass as a residue returned to the soil surface, with the allocation to standing versus flattened residue pools controlled by the `STANDING_RESIDUE_AT_HARVEST` value.
`GRAZING` will treat the clipped biomass as being consumed by livestock, with 50% of the carbon in clipped biomass returned to the soil surface manure C pool and 50% of the carbon respired by livestock as CO2, and with 50% of the nitrogen in clipped biomass returned to the soil surface in the manure N pool and 50% of the nitrogen returned as urine to the NH4+ pool of the first soil layer.

#### `MIN_TEMPERATURE_FOR_TRANSPIRATION`

The air temperature (daily average, degree C) below which transpiration by the plant virtually ceases (transpiration is 1% of potential transpiration).
This value can constrain crop growth at low temperatures because crop growth is calculated as the minimum of `Transpiration Use Efficiency * Transpiration` or `Radiation Use Efficiency * Radiation Interception`.
Therefore, if transpiration does not occur, plant biomass accrual does not occur.

#### `THRESHOLD_TEMPERATURE_FOR_TRANSPIRATION`

The threshold temperature (daily average, degree C) below which potential transpiration is multiplied by a linear temperature reduction factor, where the reduction factor is calculated as:
`(daily average temperature – MIN_TEMPERATURE_FOR_TRANSPIRATION)/ (THRESHOLD_TEMPERATURE_FOR_TRANSPIRATION – MIN_TEMPERATURE_FOR_TRANSPIRATION)`.

#### `MIN_TEMPERATURE_FOR_COLD_DAMAGE`

The minimum temperature (degree C) used in the calculation of the cold damage factor described below.
If the daily minimum temperature is below this value, the cold damage factor is set to 0.99.

#### `THRESHOLD_TEMPERATURE_FOR_COLD_DAMAGE`

The threshold temperature (degree C) used in the calculation of the cold damage factor.
If the daily minimum temperature falls below this threshold value, a cold damage factor is calculated as:

`1 – (daily minimum temperature – MIN_TEMPERATURE_FOR_COLD_DAMAGE) / (THRESHOLD_TEMPERATURE_FOR_COLD_DAMAGE - MIN_TEMPERATURE_FOR_COLD_DAMAGE)`

The cold damage factor reduces the aboveground crop biomass (moving it to the surface residue pools), reduces radiation interception by the canopy, and delays phenology development.
Aboveground biomass and radiation interception is reduced by a factor calculated as `1 – cold damage factor^3`.
As an example, a daily minimum temperature below the `MIN_TEMPERATURE_FOR_COLD_DAMAGE` will cause the existing aboveground biomass and radiation interception to be multiplied by 0.03 (1-0.99<sup>3</sup>), with the 97% of existing biomass that was killed added to the residue pools.
So for a crop with an aboveground biomass of 1000 kg/ha, a day in the simulation with a minimum temperature below the `MIN_TEMPERATURE_FOR_COLD_DAMAGE` will reduce the aboveground biomass to 30 kg/ha and add 970 kg/ha of biomass to the residue pools.

Important Note:  The `THRESHOLD_TEMPERATURE_FOR_COLD_DAMAGE` value can also trigger a grain harvest of a growing crop.
If the crop is an annual species and the cumulative thermal time of crop development is greater than the `FLOWERING_TT` value, the crop will be automatically harvested as a grain crop.

#### `BASE_TEMPERATURE_FOR_DEVELOPMENT`

The base temperature (degree C) for phenological development and accumulation of thermal time.
Air temperatures below this value will not add to cumulative thermal time.
Air temperatures above this value and below `OPTIMUM_TEMPERATURE_FOR_DEVELOPMENT` will accumulate thermal time according to `air temperature – BASE_TEMPERATURE_FOR_DEVELOPMENT`.

#### `OPTIMUM_TEMPERATURE_FOR_DEVELOPMENT`

When air temperatures are above this value (degree C) and below `MAX_TEMPERATURE_FOR_DEVELOPMENT`, the accumulation of thermal time is calculated as `(MAX_TEMPERATURE_FOR_DEVELOPMENT – air temperature) / (MAX_TEMPERATURE_FOR_DEVELOPMENT - OPTIMUM_TEMPERATURE_FOR_DEVELOPMENT) * (OPTIMUM_TEMPERATURE_FOR_DEVELOPMENT – BASE_TEMPERATURE_FOR_DEVELOPMENT)`

#### `MAX_TEMPERATURE_FOR_DEVELOPMENT`

Air temperatures above this value (degree C) will not add to cumulative thermal time.

#### `INITIAL_PARTITIONING_TO_SHOOT`

The fraction of growth partitioned to aboveground biomass (as opposed to roots) at the beginning of crop phenological development (seedling emergence).
Values can range from 0 to 1.
Perennials have lower values than annuals.

#### `FINAL_PARTITIONING_TO_SHOOT`

The fraction of growth partitioned to aboveground biomass (as opposed to roots) at the end of crop phenological development (physiological maturity). Values can range from 0 to 1.
Perennials have lower values than annuals.

#### `RADIATION_USE_EFFICIENCY`

The radiation use efficiency of the crop in g/MJ.
Units in MJ of solar radiation (not PAR).

#### `TRANSPIRATION_USE_EFFICIENCY`

The transpiration use efficiency of the crop in g/kg of water when the VPD = 1 kPa..

#### `MAXIMUM_HARVEST_INDEX`

The maximum harvest index possible (H<sub>x</sub>), used in the calculation of actual harvest index described below.
It is an asymptotic value that can be much higher than the actual maximum HI observed (e.g., 0.8 for corn).
Use default values if needed.

#### `MINIMUM_HARVEST_INDEX`

The minimum harvest index possible (H<sub>n</sub>), used in the calculation of actual harvest index described below.
It has some semblance to the fraction of water soluble carbohydrates at flowering or in the first week after flowering in grain crops.
Values range from 0 to 0.3.

#### `HARVEST_INDEX`

A coefficient (H<sub>k</sub>), used in the calculation of actual harvest index.
The actual harvest index is calculated based on the proportion of aboveground crop biomass growth occurring post-anthesis relative to total aboveground growth over the entire duration of crop development (fG) as follows:

`HI actual = Hx - (Hx - Hn) * exp(-Hk * fG)`.

A quick estimate is `k = (1 - Hn) / (Hx - Hn)` (never above this value).

#### `THERMAL_TIME_TO_EMERGENCE`

The thermal time accumulates between when a crop is planted and when it emerges from the soil and begins intercepting radiation, transpiring, accruing biomass, etc.

#### `N_MAX_CONCENTRATION`

The maximum N concentration in crop tissue (g/g), a feature that regulates the N requirements for growth through the N dilution curve.

#### `N_DILUTION_SLOPE`

A parameter that controls the slope of the N dilution curve, which regulates crop N requirements as biomass accrues.

#### `KC`

The crop transpiration coefficient, used to convert reference evapotranspiration based on environmental conditions into crop transpiration.

#### `ANNUAL`

Whether the crop is annual (value = 1) or perennial (value = 0).

#### `LEGUME`

Whether the crop is a legume (value = 1) or a non-legume (value = 0).

#### `C3`

Whether the crop has a C3 photosynthetic pathway (value = 1) or a C4 pathway (value = 0).

#### `LWP_STRESS_ONSET`

The leaf water potential (J/kg) threshold for the onset of stress, below which stomatal closure begins, reducing transpiration rates.

#### `LWP_WILTING_POINT`

The leaf water potential (J/kg) at wilting point, where transpiration stops.

#### `TRANSPIRATION_MAX`

The maximum crop transpiration rate in mm/day.

[(Back to top)](#contents)

### Management Operations File (\*.operation)

The management operation file sets the daily management operations to be used in a simulated crop rotation.
Each operation is indicated by one of five possible headers, `PLANTING`, `TILLAGE`, `FIXED_FERTILIZATION`, `FIXED_IRRIGATION`, or `AUTO_IRRIGATION`.
For each operation type, subsequent keywords tags followed by a tab-delimited value are required in a specific order.
Each operation includes an entry for the `YEAR`, and `DOY` (numerical day of the year) in which the operation is to be performed.
`YEAR` refers to a generic year in the rotation cycle, starting with year 1, rather than an actual calendar year.
The rotation will start in the calendar year specified in the control file under `SIMULATION_START_YEAR` and will repeat itself as many times as possible until the simulation reaches the end of the calendar year specified under `SIMULATION_END_YEAR` in the control file.
In this manner, the rotation in an operation file can be started in any calendar year specified by the control file and can run for as many rotation cycles as desired, given that a weather file with a sufficient time span is available.

It is important that entries in the operation file are listed in chronological order by `YEAR` and `DOY`.
If operations are listed out of order, they will be read into the management array in the wrong order and may not be executed with the proper timing in the simulation.

#### `PLANTING`

The PLANTING operation plants a crop.
The following keyword tags are needed after the PLANTING header.

##### `YEAR`

The year in the rotation in which the planting operation is to be performed.

##### `DOY`

The numerical day of the year (0 to 365 or 366) on which the planting operation is to be performed.
If conditional planting is activated (i.e., `MIN_SMC` and `MIN_SOIL_TEMP` are not `-999`), this is the beginning of the planting window.

##### `END_DOY`

The last numerical day of the year (0 to 365 or 366) on which the planting operation is to be performed when conditional planting is activated.
Set the value to `-999` to disable conditional planting.

##### `MAX_SMC`

Maximum volumetric soil moisture allowed to perform planting in m<sup>3</sup> m<sup>-3</sup>.
Set the value to `-999` to disable maximum soil moisture control on conditional planting.

##### `MIN_SMC`

Minimum volumetric soil moisture required to perform planting in m<sup>3</sup> m<sup>-3</sup>.
Keyword `FC` can be used for the `MAX_SMC` value, which represents field capacity.
Set the value to `-999` to disable minimum soil moisture control on conditional planting.

##### `MIN_SOIL_TEMP`

Minimum soil temperature required to perform planting in degree Celsius.
Keyword `PWP` can be used for the `MIN_SMC` value, which represents permanent wilting point.
Set the value to `-999` to disable soil temperature control on conditional planting.

##### `CROP`

The name of the crop to be planted.
This must match the name of one of the crops described in the crop description file (\*.crop).
If the automatic irrigation feature is activated below, the crop name here must match the crop name in the `AUTO_IRRIGATION` operation entry.

##### `USE_AUTO_IRR`

Whether or not the crop should be grown with automatic irrigation.
Yes = 1, No = 0.
If automatic irrigation is activated, there must be an associated `AUTO_IRRIGATION` entry in the operation file with the same crop name as is used in the `CROP` keyword here.

##### `USE_AUTO_FERT`

Whether or not the crop should be grown with automatic fertilization (growth unrestricted by nutrient limitations).
Yes = 1, No = 0.

##### `FRACTION`

The planting density (0 to 1), where 1 is a monoculture planting rate and < 1 indicates a proportionally reduced seeding rate.
This feature allows for the planting of multiple species in a plant community at different initial densities.

##### `CLIPPING_START`

The numerical day of the year on which the clipping time window begins.
Set the value to `-999` to disable harvesting of the corresponding crop.

##### `CLIPPING_END`

The numerical day of the year on which the clipping time window ends.

#### `TILLAGE`

The tillage operation can cause soil disturbances of varying intensity, for instance ranging from the slight disturbance of disc openers on a no-till planter to full-scale inversion tillage.
Tillage operations can also be used to harvest or kill planted crops.
The following keyword tags are needed after the `TILLAGE` header.

##### `YEAR`

The year in the rotation in which the tillage operation is to be performed.

##### `DOY`

The numerical day of the year (0 to 365) on which the tillage operation is to be performed.

##### `TOOL`

A descriptive name of the tillage tool described by the operation entry.
This can be any user defined string of text and does not affect the simulation, with some exceptions.
The string `Kill_Crop` is used to kill a crop without harvesting any residues;
the string `Grain_Harvest` is used to perform a grain harvest;
and the string `Forage_Harvest` is used to perform a forage harvest.
Note that scheduled grain harvests do not kill crops.
To kill a crop after a harvest, a `Kill_Crop` tillage operation should be used.

##### `DEPTH`

The soil depth (in meters) to which the tillage operation will cause mixing and disturbance induced increases in microbial respiration.

##### `SOIL_DISTURB_RATIO`

A relative value that indicates the extent to which the tillage operation speeds up decomposition rate of the soil microbial biomass and stabilized soil C pools.

##### `MIXING_EFFICIENCY`

A factor used to control the extent to which soil layers and residue pools mix with each other during a tillage event (range is 0 to 1).
The `MIXING_EFFICIENCY` value specifies the mass fraction of the soil mineral and organic pools in each soil layer within the depth range of the operation that will be mixed together and redeposited in each soil layer.
Pools that are mixed include clay and sand fractions, stabilized soil C and N, microbial biomass C and N, crop residue biomass and N, manure C and N, nitrate and ammonium, water content, and surface residues.
The mixing efficiency will also flatten a fraction of the standing crop residue equal to the square root of the mixing efficiency.

##### `CROP_NAME`

If the TILLAGE operation is being used to harvest or kill a crop, this tag indicates the crop name to be harvested or killed.
To harvest or kill all the crops in a planted community, use `All` or `N/A`.
To harvest a single crop, use the crop name exactly as written in the `PLANTING` operation and crop description file.

##### `FRAC_THERMAL_TIME`

This keyword is not activated in the current version of Cycles, so leave the value as 0.
In future versions, this keyword will be used to program tillage events at specific times relative to crop phenological development.

##### `KILL_EFFICIENCY`

This keyword is not activated in the current version of Cycles, so leave the value as 0.
In future versions, this keyword will be used to generate an incomplete kill of a crop in order to simulate the effect of competition from a crop previous crop that was not completely killed.

#### `FIXED_FERTILIZATION`

The fixed fertilization operation is used to apply organic and inorganic soil fertility inputs in a simulation.
The following keyword tags are needed after the `FIXED_FERTILIZATION` header.

##### `YEAR`

The year in the rotation in which the fixed fertilization operation is to be performed.

##### `DOY`

The numerical day of the year (0 to 365) on which the tillage operation is to be performed.

##### `SOURCE`

A descriptive name of the fertility input described by the entry.
This can be any user defined string of text and does not affect the simulation.

##### `MASS`

The total mass of the input in kg/ha.

##### `FORM`

This keyword is not currently activated.
Enter any string.

##### `METHOD`

This keyword is not currently activated.
Enter any string.

##### `LAYER`

The soil layer to which the fertilizer input is added.
To specify surface applied fertilizer applications, use a value of 0.
Organic components of surface applied fertilizer inputs will be added to the surface manure C and N pools, while inorganic components of the surface applied inputs will be added to the nitrate or ammonium pools of the first soil layer.

##### `C_ORGANIC`

The fraction of input mass that is organic C.
This fraction will be added to the manure C pool.

##### `C_CHARCOAL`

This keyword is not currently activated.
Use 0.

##### `N_ORGANIC`

The fraction of input mass that is organic N.
This fraction will be added to the manure N pool.

##### `N_CHARCOAL`

This keyword is not currently activated.
Use 0.

##### `N_NH4`

The fraction of input mass that is ammonium N.
This fraction will be added to the ammonium pool.

##### `N_NO3`

The fraction of input mass that is nitrate N.
This fraction will be added to the nitrate pool.

##### `P_ORGANIC`

This keyword is not currently activated.
Use 0.

##### `P_CHARCOAL`

This keyword is not currently activated.
Use 0.

##### `P_INORGANIC`

This keyword is not currently activated.
Use 0.

##### `K`

This keyword is not currently activated.
Use 0.

##### `S`

This keyword is not currently activated.
Use 0.

#### `FIXED_IRRIGATION`

The fixed irrigation operation is used to provide a one-time application of water to simulate an irrigation event.
The following keyword tags are needed after the `FIXED_IRRIGATION` header.

##### `YEAR`

The year in the rotation in which the tillage operation is to be performed.

##### `DOY`

The numerical day of the year (0 to 365) on which the tillage operation is to be performed.

##### `VOLUME`

The volume of water to be added in mm.
Irrigation water is added to the soil surface and is combined with precipitation and snowmelt in calculating the amount of water infiltration versus runoff.

#### `AUTO_IRRIGATION`

The automatic irrigation operation is used to provide irrigation events triggered by specific water depletion levels.
The automatic irrigation event will calculate the quantity of water to add that is needed to bring the soil layers within and above the `LAST_SOIL_LAYER` to field capacity.
These operation entries are only read into the simulation controls if one or more crops have the `USE_AUTO_IRR` keyword set to 1 in a `PLANTING` operation entry.
The following keyword tags are needed after the `AUTO_IRRIGATION` header.

##### `CROP`

The crop that the `AUTO_IRRIGATION` operation applies to.
This entry must match the name entered in the `CROP` keyword of the `PLANTING` operation.

##### `START_DAY`

The first numerical day of the year at which auto irrigation events can be triggered.

##### `STOP_DAY`

The last numerical day of the year at which auto irrigation events can be triggered.

##### `WATER_DEPLETION`

The fraction of the plant available water capacity that can be depleted before triggering an automatic irrigation event.
The fraction of plant available water depleted is integrated across the soil layers including and above the layer specified by `LAST_SOIL_LAYER`.
Example: entering a value of 0.05 means that an auto irrigation event will be triggered when the plant available water content (volumetric water content above the permanent wilting point but not greater than field capacity) falls below 95% of the plant available water capacity (the volumetric water content between permanent wilting point water content and field capacity water content).

##### `LAST_SOIL_LAYER`

The bottom soil layer that will be used in the determination of auto irrigation requirements and volumes.

[(Back to top)](#contents)

### Reinitialization File (\*.reinit)

Re-initialization files are optional, and typically have a suffix of `.reinit`, but any naming convention can be used as long as it matches the reinitialization file name listed in the control file.
When reinitialization option is used, the model variables of choice will be replaced by the values specified in the reinitialization file.

The reinitialization file starts with a row specifying `YEAR`, and `DOY`.
`YEAR` and `DOY` set the simulation year and the numerical day of the year that reinitialization occurs.
The re-reinitialization variables are organized in a table-like fashion.
The header lines describe the names of the variables, and the values follow.
Currently, the following options are available:
 SMC (m3/m3),
 NO3 (kg/ha),
 NH4 (kg/ha),
 SOC (Mg/ha),
 SON (Mg/ha),
 RESIDUEABGD (Mg/ha),
 RESIDUERT (Mg/ha),
 RESIDUERZ (Mg/ha),
 RESIDUEABGDN (Mg/ha),
 RESIDUERTN (Mg/ha),
 RESIDUERZN (Mg/ha),
 MANUREC (Mg/ha),
 MANUREN (Mg/ha), and
 SATURATION (100%)
of different soil layers; and
 STANRESIDUEMASS (Mg/ha),
 FLATRESIDUEMASS (Mg/ha),
 STANRESIDUEN (Mg/ha),
 FLATRESIDUEN (Mg/ha),
 MANURESURFACEC (Mg/ha),
 MANURESURFACEN (Mg/ha),
 STANRESIDUEWATER (mm),
 FLATRESIDUEWATER (mm), and
 INFILTRATION (mm/day).

 YEAR | yyyy |  DOY | xxx
--- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | ---| --- | --- | --- | ---
STANRESIDUEMASS | FLATRESIDUEMASS | STANRESIDUEN | FLATRESIDUEN | MANURESURFACEC | MANURESURFACEN | INFILTRATION
...| | | | | |
LAYER | SMC | NO3 | NH4 | SOC | SON | MBC | MBN | RESIDUEABGD | RESIDUERT | RESIDUERZ | RESIDUEABGDN | RESIDUERTN | RESIDUERZN | MANUREC | MANUREN | SATURATION
1 | ... | | | | | | | | | | | | | | |
2 | ... | | | | | | | | | | | | | | |
3 | ... | | | | | | | | | | | | | | |
... | ... | | | | | | | | | | | | | | |


 The structure described above can repeat to specify re-initialization for as many dates as needed.
 To disable the re-initialization of certain variables, use `-999` as their values.

[(Back to top)](#contents)

### Multi-Mode File

Multi-mode files can be used to perform batch simulations, when combined with the `-m` option of the Cycles executable.
They serve as multiple simulation control files, with each line driving one simulation.
The `SIM_CODE` column specifies the name of the output subdirectory for the corresponding simulation.
Purposes of all other input variables are the same as in simulation control files.
Note that all outputs are turned on in multi-mode.

[(Back to top)](#contents)

## Output Files

Cycles offers a number of pre-formatted output files to inspect the agroecosystem processes simulated in the model.
The following pages provide documentation for the values printed in each output file.
To obtain customized outputs not included here, please contact a member of the Cycles development team.

### Weather.dat

| Column Heading    | Units         | Description |
| ----------------- | ------------- | ----------- |
| DATE              | YYYY-MM-DD    | Calendar date of the simulation. |
| MIN TMP           | C             | Minimum temperature. |
| MAX TMP           | C             | Maximum temperature. |
| AVG TMP           | C             | Average temperature. |
| REFERENCE ET      | mm/day        | Reference evapotranspiration calculated using the Penman-Monteith equation. |
| PRECIPITATION     | mm            | Precipitation. |

[(Back to top)](#contents)

### [crop].dat

Note: [crop] in the file name will be replaced by the name of each crop planted in the rotation as specified in the operation file (e.g., Maize.dat).

| Column Header    | Units            | Description |
| ------------- | ------------- | ----------- |
| DATE          | YYYY-MM-DD    | Calendar date in the simulation. |
| CROP          | -             | The crop name. |
| STAGE         | -             | The phenological stage.|
| THERMAL TIME  | C-day         | The cumulative thermal time since the crop was planted. |
| CUM. BIOMASS  | Mg/ha         | The cumulative total crop biomass, sum of aboveground and roots. |
| AG BIOMASS    | Mg/ha         | Aboveground dry biomass. |
| ROOT BIOMASS  | Mg/ha         | Crop root dry biomass. |
| FRAC INTERCEP | -             | The fraction of solar radiation intercepted by the crop canopy. |
| TOTAL N       | kg/ha         | The total crop N content, sum of aboveground and roots. |
| AG N          | kg/ha         | Aboveground biomass N content. |
| ROOT N        | kg/ha         | Crop root N content. |
| AG N CONCN    | g/kg          | Aboveground biomass N concentration. |
| N FIXATION    | kg/ha         | Cumulative N fixation by legume crops. |
| N ADDED       | kg/ha         | Cumulative N added to crop biomass to sustain growth in autofertilize mode. |
| N STRESS      | %             | Daily N stress value of the crop. |
| WATER STRESS  | %             | Daily water stress value of the crop. |
| POTENTIAL TR  | mm/day        | Potential transpiration of the crop. |

[(Back to top)](#contents)

### annualSoilProfileC.dat

Note: This file provides annualized measurements of various carbon pools by soil layer.  One column will be created for each layer in the soil profile for the variables where the column heading indicates `LAYER 1..x`.

| Column Header     |               | Units       | Description |
| ----------------- | ------------- | ----------- | ----------- |
| YEAR              | -             | YYYY        | Year of the simulation. |
| RES BIOMASS IN    | LAYER 1..x    | Mg/ha       | Aboveground crop residue biomass added to the residue pool during the year. |
| ROOT BIOMASS IN   | LAYER 1..x    | Mg/ha       | Crop root and rhizodeposit biomass added to the residue pools during the year. |
| RES C DECOMP      | LAYER 1..x    | Mg/ha       | Quantity of C from the aboveground crop residue pool decomposed during the year. (Layer 1 includes decomposed surface residues, standing and flattened) |
| ROOT C DECOMP     | LAYER 1..x    | Mg/ha       | Quantity of C from the crop root and rhizodeposit residue pools decomposed during the year. |
| INIT C MASS       | LAYER 1..x    | Mg C/year   | The pool size of stabilized soil C at the beginning of the year. |
| HUMIFIED C        | LAYER 1..x    | Mg C/year   | Quantity of decomposed C added to microbial and stabilized C pools. |
| RESPIRED C        | LAYER 1..x    | Mg C/year   | Quantity of C respired from the decomposition of microbial biomass and stabilized soil C. |
| FINAL C           | LAYER 1..x    | Mg C/year   | The pool size of stabilized soil C at the end of the year. |
| C DIFF            | LAYER 1..x    | Mg C/year   | The change in the stabilized soil C pool size between the beginning and end of the year. |

[(Back to top)](#contents)

### AnnualSOM.dat

Note: This file provides annualized measurements of the carbon concentration and saturation ratio by soil layer.
One column will be created for each layer in the soil profile for the variables where the column heading indicates `LAYER 1..x`.

| Column Heading    |               | Units | Description |
| ----------------- | ------------- | ----- | ----------- |
| YEAR              | -             | YYYY  | Year of the simulation. |
| THICKNESS         | LAYER 1..x    | m     | Thickness of the soil layer. |
| BULK DENSITY      | LAYER 1..x    | Mg/m3 | Bulk density of the soil layer. |
| SOIL C            | LAYER 1..x    | %     | Carbon concentration of the soil layer, only including the stabilized soil C pool. |
| C SAT. RATIO      | LAYER 1..x    | Cs/Cx | The saturation ratio of the soil layer, defined as the C content (or concentration) of the stabilized soil C pool (Cs) divided by the maximum capacity for C stabilization (Cx). |

[(Back to top)](#contents)

### annualN.dat

Note: This file provides annualized measurements of nitrogen fluxes for the sum of all layers in the soil profile.

| Column Heading  | Units | Description |
| ----------------| ------| ----------- |
| YEAR            | YYYY  | Year of the simulation. |
| FERTILIZATION   | kg/ha | Total mass of N added as fertilizer. |
| AUTO ADDED      | kg/ha | Total mass of N added to crop biomass to sustain growth in autofertilize mode. |
| FIXATION        | kg/ha | Total N fixation by legume crops. |
| NO3 LEACHING    | kg/ha | Nitrate leaching in drainage water at the bottom of the soil profile. |
| NH4 LEACHING    | kg/ha | Ammonium leaching in drainage water at the bottom of the soil profile. |
| NO3 BYPASS      | kg/ha | Nitrate leaching in drainage water due to horizontal bypass flow. |
| NH4 BYPASS      | kg/ha | Ammonium leaching in drainage water due to horizontal bypass flow. |
| DENITRIFICATION | kg/ha | Denitrification of nitrate. |
| NITRIFICATION   | kg/ha | Nitrification of ammonium. |
| VOLATILIZATION  | kg/ha | Ammonia volatilization. |
| N2O EMISSION    | kg/ha | Total nitrous oxide emissions from nitrification and denitrification. |
| N IN HARVEST    | kg/ha | N content in removed biomass, which is the sum of grain, forage, and/or removed residues. |

[(Back to top)](#contents)

### soilC.dat

Note: Results in this file are for the sum of all layers in the soil profile, including surface residues.

| Column Header     | Units         | Description |
| ----------------- | ------------- | ----------- |
| DATE              | YYYY-MM-DD    | Calendar date in the simulation. |
| SOIL ORG C        | Mg/ha         | The sum of microbial biomass C and stabilized soil C pools. |
| HUMIFIED C        | Mg/ha         | The C from all decomposed substrates, including residues, microbial biomass C, and stabilized soil C that becomes humified in microbial biomass C or stabilized soil C pools. |
| RES RESPIRED C    | Mg/ha         | The C from all decomposed residues, including crop residues, manure, and rhizodeposits, that is respired by microbes. |
| SOM RESPIRED C    | Mg/ha         | The C from decomposed microbial biomass C and stabilized soil C that is respired by microbes. |

[(Back to top)](#contents)

### N.dat

Note: Results in this file are for the sum of all layers in the soil profile, including surface residues.

| Column Header     | Units         | Description |
| ----------------- | ------------- | ----------- |
| DATE              | YYYY-MM-DD    | Calendar date in the simulation. |
| ORG SOIL N        | kg N/ha       | The sum of microbial biomass N and stabilized soil organic N pools. |
| PROF SOIL NO3     | kg N/ha       | Soil profile nitrate-N content. |
| PROF SOIL NH4     | kg N/ha       | Soil profile ammonium-N content. |
| MINERALIZATION    | kg N/ha       | Gross N mineralization from all decomposing organic substrates. |
| IMMOBILIZATION    | kg N/ha       | Gross N immobilization in microbial biomass or stabilized soil organic N pools. |
| NET MINERALIZ     | kg N/ha       | Net N mineralization. |
| NH4 NITRIFICAT    | kg N/ha       | Nitrification of ammonium. |
| N2O FROM NITRIF   | kg N/ha       | Nitrous oxide emissions from nitrification. |
| NH4 VOLATILIZ     | kg N/ha       | Ammonia volatilization. |
| NO3 DENITRIF      | kg N/ha       | Denitrification of nitrate. |
| N2O FROM DENIT    | kg N/ha       | Nitrous oxide emissions from denitrification. |
| NO3 LEACHING      | kg N/ha       | Nitrate leaching in drainage water at the bottom of the soil profile. |
| NH4 LEACHING      | kg N/ha       | Ammonium leaching in drainage water at the bottom of the soil profile. |
| NO3 BYPASS        | kg N/ha       | Nitrate leaching in drainage water due to horizontal bypass flow. |
| NH4 BYPASS        | kg N/ha       | Ammonium leaching in drainage water due to horizontal bypass flow. |

[(Back to top)](#contents)

### Residue.dat

Note: Results in this file are for the sum of all layers in the soil profile, including surface residues.

| Column Header     | Units         | Description |
| ----------------- | ------------- | ----------- |
| DATE              | YYYY-MM-DD    | Calendar date in the simulation. |
| FRAC INTERCEP     | -             | The fraction of solar radiation intercepted by standing and flattened crop residues on the soil surface. |
| AG RES BIOMASS    | Mg/ha         | The biomass of standing and flattened crop residues remaining on the soil surface. |
| BG RES BIOMASS    | Mg/ha         | The remaining biomass of crop residue shoots that were incorporated into the soil profile. |
| ROOT RES BIOMAS   | Mg/ha         | The remaining biomass of crop residue roots and rhizodeposits in the soil profile. |
| SFC MANURE C      | Mg/ha         | The C content of manure remaining on the soil surface. |
| AG RES N          | kg/ha         | The N content of standing and flattened crop residues remaining on the soil surface. |
| BG RES N          | kg/ha         | The remaining N content of crop residue shoots that were incorporated into the soil profile. |
| ROOT RES N        | kg/ha         | The remaining N content of crop residue roots and rhizodeposits in the soil profile. |
| SFC MANURE N      | Mg/ha         | The N content of manure remaining on the soil surface. |
| STD RES MOIST     | kg/kg         | Standing surface residue moisture content defined as: `water mass/(water mass+residue dry mass)` |
| FLAT RES MOIST    | kg/kg         | Flattened surface residue moisture content defined as: `water mass/(water mass+residue dry mass)` |

[(Back to top)](#contents)

### Water.dat

| Column Header     |               | Units         | Description |
| ----------------- | --------------| ------------- | ----------- |
| DATE              | -             | YYYY-MM-DD    | Calendar date in the simulation. |
| IRRIGATION        | -             | mm/day        | Water added as irrigation. |
| RUNOFF            | -             | mm/day        | Water runoff from the soil surface. |
| INFILTRATION      | -             | mm/day        | Water infiltration into the soil. |
| DRAINAGE          | -             | mm/day        | Water drainage from the bottom of the soil profile. |
| BYPASS            | -             | mm/day        | Water drainage due to horizontal bypass flow. |
| SOIL EVAP         | -             | mm/day        | Evaporation from the soil surface. |
| RES EVAP          | -             | mm/day        | Evaporation from the surface residue. |
| SNOW SUB          | -             | mm/day        | Sublimation from snow cover. |
| TRANSPIRATION     | -             | mm/day        | Transpiration by the crop canopy. |
| SMC               | LAYER 1..x    | m3/m3         | Soil moisture content. |

[(Back to top)](#contents)
 
### Season.dat

The season.dat file provides information about each crop harvest.

| Column Header     | Units         | Description |
| ----------------- | ------------- | ----------- |
| DATE              | YYYY-MM-DD    | Calendar date in the simulation. |
| CROP              | -             | Crop that was harvested. |
| PLANT DATE        | -             | Calendar date when the crop was planted. |
| TOTAL BIOMASS     | Mg/ha         | Total biomass (roots + aboveground) accumulated by the crop at harvest. |
| ROOT BIOMASS      | Mg/ha         | Root biomass accumulated by the crop at harvest. |
| GRAIN YIELD       | Mg/ha         | Grain yield at harvest. |
| FORAGE YIELD      | Mg/ha         | Forage yield or removed residue biomass at harvest. |
| AG RESIDUE        | Mg/ha         | Aboveground residue biomass left in the field at harvest. |
| HARVEST INDEX     | Mg/Mg         | Fraction of aboveground biomass harvested as grain. |
| POTENTIAL TR      | mm            | Potential water transpiration of the crop under non-limiting soil moisture conditions. |
| ACTUAL TR         | mm            | Actual water transpiration of the crop given soil moisture conditions in the simulation. |
| SOIL EVAP         | mm            | Water evaporation from the soil. |
| TOTAL N           | Mg/ha         | Total biomass N (roots + aboveground) content accumulated by the crop at harvest. |
| ROOT N            | Mg/ha         | Root biomass N accumulated by the crop at harvest. |
| GRAIN N           | Mg/ha         | Grain N content at harvest. |
| FORAGE N          | Mg/ha         | Forage or removed residue N content at harvest. |
| CUM. N STRESS     | -             | Cumulative N stress over the duration of crop growth, calculated as the sum of each daily N stress weighted by the daily fraction of thermal time accumulated to maturity. |
| N IN HARVEST      | kg/ha         | N content in removed biomass, which is the sum of grain, forage, and/or removed residues. |
| N IN RESIDUE      | kg/ha         | N content in residues left in the field at harvest. |
| N CONCN FORAGE    | %             | The N concentration in forage or removed residues at harvest. |
| N FIXATION        | kg/ha         | Cumulative N fixation by legume crops. |
| N ADDED           | kg/ha         | Cumulative N added to crop biomass to sustain growth in autofertilize mode. |

[(Back to top)](#contents)
 
### soilLayersCN.dat

Note: This file provides outputs needed to calculate selected C and N pool sizes or elemental concentrations by soil layer.
One column will be created for each layer in the soil profile for the variables where the column heading indicates `LAYER 1..x`.

| Column Heading    |               | Units        | Description |
| ----------------- | ------------- | ------------ | ----------- |
| DATE              | -             | YYYY-MM-DD   | Calendar date of the simulation. |
| NO3               | LAYER 1..x    | mg/kg        | Nitrate concentration. |
| NH4               | LAYER 1..x    | mg/kg        | Ammonium concentration. |
| FACTOR COMP.      | LAYER 1..x    | -/-          | The composite environmental factor downregulating the maximum decomposition rate of each C pool; a function of temperature, moisture, and aeration in each soil layer. |
| STAND RESID C     | SURFACE       | Mg/ha        | The C content of standing residue on the soil surface. |
| FLAT RESID C      | SURFACE       | Mg/ha        | The C content of flattened residue on the soil surface. |
| MANURE RESID C    | SURFACE       | Mg/ha        | The C content of manure on the soil surface. |
| RESIDUE C         | LAYER 1..x    | Mg/ha        | The total C content of residues in each soil layer, including aboveground crop residues, root residues and rhizodeposits, and manure. |
| STAND RESID C:N   | SURFACE       | g:g          | The C:N ratio of standing residue on the soil surface. |
| FLAT RESID C:N    | SURFACE       | g:g          | The C:N ratio of flattened residue on the soil surface. |
| MANURE RES C:N    | SURFACE       | g:g          | The C:N ratio of manure on the soil surface. |
| RESIDUE C:N       | LAYER 1..x    | g:g          | The C:N ratio of residues in each soil layer, including aboveground crop residues, root residues and rhizodeposits, and manure. |
| MIC C             | LAYER 1..x    | Mg/ha        | The C content of microbial biomass in each soil layer. |
| MIC C:N           | LAYER 1..x    | g:g          | The C:N ratio of microbial biomass in each soil layer. |
| SOIL ORG C        | LAYER 1..x    | Mg/ha        | The C content of stabilized soil organic matter in each soil layer. |
| SOIL ORG C:N      | LAYER 1..x    | g:g          | The C:N ratio of stabilized soil organic matter in each soil layer. |
| WATER CONTENT     | LAYER 1..x    | m3/m3        | The volumetric water content in each soil layer. |
| THICKNESS         | LAYER 1..x    | m            | The thickness of each soil layer. |
| BULK DENSITY      | LAYER 1..x    | Mg/m3        | The bulk density of each soil layer. |

[(Back to top)](#contents)
 
### Summary.dat

The summary file provides a summarized output of total C inputs over the duration of the simulation and average annual rates for N cycling processes.

| Column Header     | Units         | Description |
| ----------------- | ------------- | ----------- |
| INIT PROF C       | Mg C          | The C content in the stabilized soil C pool at the beginning of the simulation. |
| FINAL PROF C      | Mg C          | The C content in the stabilized soil C pool at the end of the simulation. |
| PROF C DIFF       | Mg C          | The change in C content of the stabilized soil C pool from the beginning to end of the simulation. |
| RES C INPUT       | Mg C          | The total quantity of aboveground crop residues decomposed over the duration of the simulation. |
| ROOT C INPUT      | Mg C          | The total quantity of roots and rhizodeposits decomposed over the duration of the simulation. |
| HUMIFIED C        | Mg C          | The total quantity of decomposed C added to microbial and soil C pools over the duration of the simulation. |
| RESP SOIL C       | Mg C          | The total quantity of C respired during decomposition of the microbial biomass and stabilized soil C pools over the duration of the simulation. |
| RESP RES C        | Mg/ha         | The total quantity of C respired during decomposition of the residue pools over the duration of the simulation. |
| RETAINED RES      | Mg/ha         | The total quantity of aboveground crop residues retained in the field over the duration of the simulation. |
| PRODUCED ROOT     | Mg/ha         | The total quantity of roots and rhizodeposits produced over the duration of the simulation. |
| SOIL C CHG/YR     | kg C          | The difference in the stabilized soil C pool size from the beginning to the end of the simulation divided by the number of years of the simulation. |
| AVG GROSS N MIN   | kg N/yr       | Average gross N mineralization rate over the duration of the simulation. |
| AVG N IMMOB       | kg N/yr       | Average gross N immobilization rate over the duration of the simulation. |
| AVG NET N MIN     | kg N/yr       | Average net N mineralization rate over the duration of the simulation. |
| AVG NH4 NITRIF    | kg N/yr       | Average nitrification rate over the duration of the simulation. |
| AVG N2O FR NIT    | kg N/yr       | Average nitrous oxide emission rate from nitrification over the duration of the simulation. |
| AVG NH3 VOLATIL   | kg N/yr       | Average ammonia volatilization rate over the duration of the experiment. |
| AVG NO3 DENIT     | kg N/yr       | Average denitrification rate over the duration of the simulation. |
| AVG N2O FR DENI   | kg N/yr       | Average nitrous oxide emission rate from denitrification over the duration of the simulation. |
| AVG NO3 LEACH     | kg N/yr       | Average nitrate leaching rate over the duration of the simulation. |
| AVG NH4 LEACH     | kg N/yr       | Average ammonium leaching rate over the duration of the simulation. |
| AVG TOT N2O EMI   | kg N/yr       | Average total nitrous oxide emissions over the duration of the simulation. |

[(Back to top)](#contents)

## Bug Report Guidelines

All bug reports and feature requests **must** be submitted as issues at the [Cycles Issues page](https://github.com/PSUmodeling/Cycles/issues).

Before reporting the bug, please download the latest version of Cycles at the [Cycles release page](https://github.com/PSUmodeling/Cycles/releases) and test if the bug has already been fixed.

When reporting a bug, describe the bug briefly in the title of the issue.
In the comment section, include the Cycles version you have tested.
To check the version of Cycles you are using, please run

```shell
./Cycles -V
```

or, in Windows

```shell
Cycles_win.exe -V
```

Please describe how to reproduce the bug, what is the expected result, and what is the actual result.
Include important information such as in which output file, on what simulation day does the bug appear.
If the bug is specific to your simulation, please attach your input files to the issue.

The Cycles team will get back to you immediately.
Meanwhile check if your question has been already posted and resolved.

[(Back to top)](#contents)
