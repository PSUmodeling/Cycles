# Input Files

Simulations in Cycles are driven by a series of ASCII-text input files.
However, the simulation control file (`*.ctrl`) is the only input file explicitly specified when executing Cycles in the command prompt terminal or command line.
The names of inputs files for the crop descriptions, management operations, soil profile description, and weather records are specified within the control file.
Generally speaking, the input files must be written in exactly the same format as the master template.
Users can write custom annotations within input files, however.
Any text following a `#` mark will not be read by the program.
Blank lines are also not read by the program
To create input files for a new simulation, it's easiest to make a copy of existing template files, rather than retype each file from scratch.

## Simulation Control File (\*.ctrl)

The simulation control files have a suffix of `.ctrl`.
For each simulation, the name of the control file is submitted when invoking Cycles at the command prompt and is also used as the directory name where output files are stored following each simulation.
In the control file, keyword tags are on the left and the value for each tag is separated by spaces or tabs on the right.
The control file contains information about the starting and ending years of the simulation, simulation options, including the specific output files to be written, and the names of other input files.
Simulation options are represented as on/off switches, where `0` stands for off and `1` stands for on.
The meaning of each keyword in the control file is described below.

### `SIMULATION_START_YEAR`

The year to start the simulation.
All simulations start on January 1 of the given year.
There must be records in the weather file starting from January 1 of this year.

### `SIMULATION_END_YEAR`

The year to end the simulation.
All simulations run to December 31 of the given year.
There must be records in the weather file through December 31 of this year.

### `ROTATION_SIZE`

The number of years in the crop rotation specified in the operation input file.
The specified rotation will automatically repeat itself as many times as needed over the duration of the simulation spanning from `SIMULATION_START_YEAR` to `SIMULATION_END_YEAR`.

### `CROP_FILE`

The name of the crop description file.
This file must be located in a directory titled `input` that is within the `Cycles` working directory.

### `OPERATION_FILE`

The name of the operation file.
This file must be located in a directory titled `input` that is within the `Cycles` working directory.

### `SOIL_FILE`

The name of the soil profile description file.
This file must be located in a directory titled `input` that is within the `Cycles` working directory.

### `WEATHER_FILE`

The name of the weather file.
This file must be located in a directory titled `input` that is within the `Cycles` working directory.

### `REINIT_FILE`

The name of the reinitialization file.
This file must be located in a directory titled `input` that is within the `Cycles` working directory.
Note that it cannot be left as blank, even if reinitialization is not needed.
Use `N/A` if reinitialization is not needed.

### `SOIL_LAYERS`

The number of soil layers to be used in the simulation.
The number of soil layers cannot be greater than the number of soil layers specified in the soil profile description file.

### `CO2_LEVEL`

Atmospheric CO<sub>2</sub> concentration ($\mathrm{ppm}$).
Set to `-999` to use annual CO<sub>2</sub> concentrations in the `co2.txt` file.

### `USE_REINITIALIZATION`

Set to `1` if reinitialization of carbon, nitrogen or water variables are desired on a specific day of simulation year.
A re-initialization file is required in this case.
Set to `0` if re-initialization is not needed.

### `ADJUSTED_YIELDS`

This keyword is currently not activated.
Enter `0`.

### `HYDROLOGY_OPTION`

Controls whether water infiltration and redistribution between soil layers is calculated using

- a daily cascade method (set value to `0`),
- a finite difference numerical integration at an hourly time-step (set value to `1`),
- a Crank-Nicolson finite difference numerical integration at a sub-daily time-step (set value to `2`), or
- a finite difference numerical integration at a sub-daily time-step solved by CVode (set value to `3`).

The cascade method is the most computationally efficient, but does not allow for water contents greater than field capacity or soil wetting from the bottom up.
Cascade method can miss denitrification badly, but does a good job with the water balance.

### `AUTOMATIC_NITROGEN`

Set to `0` for crops to be grown based on N available from N fertilizer additions and N cycling processes.
Set to `1` for all crops in the simulation to be grown without nitrogen limitations.
For this selection to work, the autoN flag in the planting operation of a given crop (operation file) needs to be set to `1`.
This allows running crops with and without N limitation in the same simulation.

### `AUTOMATIC_PHOSPHORUS`

Crops grow with no phosphorus limitations.
(Note: phosphorus cycling is currently not represented in the model, so this switch does not activate anything.)

### `AUTOMATIC_SULFUR`

Crops grow with no sulfur limitations.
(Note: sulfur cycling is currently not represented in the model, so this switch does not activate anything.)

### `DAILY_WEATHER_OUT`

Set to `1` to write an output file named `dailyWeather.csv` with the average temperature, reference evapotranspiration, and precipitation for each day of the simulation.

### `DAILY_CROP_OUT`

Set to `1` to write an output file named for each crop used in the rotation (e.g., `dailyMaize.csv`) with daily values for growing degree day accumulation, crop biomass production, biomass N content, crop N stress and water stress, and potential transpiration.

### `DAILY_RESIDUE_OUT`

Set to `1` to write an output file named `dailyResidue.csv` with daily values associated with the residue pools, including radiation interception by surface residues, surface residue moisture content, shoot and root residue biomass and N content in the entire soil profile, and manure C and N content at the surface and in the soil profile.

### `DAILY_WATER_OUT`

Set to `1` to write an output file named `dailyWater.csv` with daily values for the water budget, including inputs from irrigation and rainfall, and outputs of drainage, runoff, evaporation, transpiration, and sublimation.

### `DAILY_NITROGEN_OUT`

Set to `1` to write an output file named `dailyN.csv` with daily values for pools and fluxes in the N cycle, including soil profile stocks of organic N, nitrate N, and ammonium N, and rates of N mineralization and immobilization, nitrification, gaseous N losses, and N leaching losses.

### `DAILY_SOIL_CARBON_OUT`

Set to `1` to write an output file named `dailySoilC.csv` with daily values for pools and fluxes in the C cycle, including soil profile stocks of organic C, the C from decomposed organic matter that becomes humified in microbial and stabilized C pools, and C respired from decomposed residues or soil organic matter.

### `DAILY_SOIL_LYR_CN_OUT`

Set to `1` to write an output file named `dailySoilLayersCN.csv` with daily values for C and N pool sizes by soil layer.
Included are nitrate and ammonium concentrations, the composite environmental factor regulating decomposition rates, residue C and C:N ratio, microbial biomass C and C:N ratio, stabilized organic C and C:N ratio, soil water content, and soil layer thickness and bulk density.

### `ANNUAL_SOIL_OUT`

Set to `1` to write an output file named `annualSOM.csv` with annual values for soil carbon pool sizes and carbon saturation ratios by soil layer.

### `ANNUAL_PROFILE_OUT`

Set to `1` to write an output file named `annualSoilProfileC.csv` with annual values of carbon pools by soil layer.

### `ANNUAL_NFLUX_OUT`

Set to `1` to write and output file named `annualN.csv` with annual values of nitrogen fluxes, including fertilization, fixation, leaching, denitrification, nitrification, and volatilization.

## Weather File (\*.weather)

The weather files typically have a suffix of `.weather`, but any suffix or file naming convention can be used so long as the name of the weather file listed in the control file exactly matches the name of the weather file to be used in the input directory.

Weather files contain three lines at the beginning with the keyword tags `LATITUDE`, `ALTITUDE`, and `SCREENING_HEIGHT`, each followed by a tab-delimited value.
Altitude and screening height values should be entered in meters.
Screening height is the height of the weather station instruments above the land surface.
Following these three lines is a row of tab-delimited column headers in the order `YEAR`, `DOY`, `PP`, `TX`, `TN`, `SOLAR`, `RHX`, `RHN`, `WIND`, with daily values for each weather variable listed in rows below.
`YEAR` and `DOY` are the year and numerical day of the year, respectively, for each daily weather record.
`PP` is precipitation in $\mathrm{mm}/\mathrm{day}$.
`TX` and `TN` are the maximum and minimum daily temperatures in $^{\circ}\mathrm{C}$.
`SOLAR` is the daily solar radiation in $\mathrm{MJ}/\mathrm{m}^{2}/\mathrm{day}$.
`RHX` and `RHN` are the maximum and minimum relative humidity in $\%$.
`WIND` is the average wind speed in $\mathrm{m}/\mathrm{s}$.

## Soil File (\*.soil)

Similar to the weather input file, soil files typically have a suffix of `.soil`, but any naming convention can be used as long as it matches the soil file name listed in the control file.

The soil file starts with two lines at the beginning with the keyword tags `CURVE_NUMBER` and `SLOPE`, with each keyword followed by a value.

The `CURVE_NUMBER` refers to the United States Department of Agriculture Natural Resources Conservation Service Curve Number method to calculate runoff from watersheds.
The curve number is used in an equation to predict the volume of water from precipitation or irrigation that is necessary to generate runoff from a field.
Lower values indicate faster infiltration rates, meaning that a greater volume of precipitation is needed to generate runoff.
The exact value to use in a soil input file for a given simulation depends on the soil texture and crop rotation characteristics, as described in the NRCS technical report TR-55, "[Urban Hydrology for Small Watersheds](https://www.nrcs.usda.gov/Internet/FSE_DOCUMENTS/stelprdb1044171.pdf)".
Table 2.2 in the document suggests curve numbers to use for different hydrologic soil groups (A, B, C, or D) under different crop rotations.
If the hydrologic soil group is not known for a soil, Appendix A of TR-55 provides qualitative descriptions of the different hydrologic groups and assigns a hydrologic soil group rating to different soil texture classes (i.e., loams, silt loams, sandy loams, etc.).
The curve number rating will affect the water budget in a simulation, which can affect crop yields and other processes, so its value should be given due consideration, especially if you are attempting to compare simulation outputs with results from a real field experiment.

The `SLOPE` value is in $\%$, or units of rise per $100$ units of run.

Below the first three keyword tags is a row of column headers for soil properties in the order `LAYER`, `THICK`, `CLAY`, `SAND`, `SOC`, `BD`, `FC`, `PWP`, `SON`, `NO3`, `NH4`, `ROCK`, `BYP_H`, `BYP_V`, and `pH`.
The values of each soil property are listed below in a separate row for each soil layer.

The variable `LAYER` is simply the layer number, starting at `1` and increasing by one unit for each additional layer.

`THICK` is the thickness of each soil layer in meters.
Because of the important processes occurring in the topsoil layers, such as gas and energy exchange with the atmosphere and incorporation of crop residues, it is necessary to have the topsoil layers simulated with greater depth resolution than the subsoils.
An optimal depth for the first soil layer is $0.05~\mathrm{m}$, with subsequent topsoil layers entered in increments between $0.05~\mathrm{m}$ and $0.10~\mathrm{m}$ depth.
Below the topsoil layers, we suggest that soil layers be defined based on depth segments corresponding to changes in soil horizonation for the soil type to be simulated.

The keyword tags `CLAY` and `SAND` are the clay particle size fraction and sand particle size fraction of each soil layer in $\%$.

`SOC` is the soil organic carbon concentration of each layer in $\%$.
The organic carbon concentration of each layer is used to calculate N concentration to initialize the stabilized soil C and N pools when the simulation is started, using a soil organic C:N ratio of $10:1$.
The microbial biomass C and N pools are also initialized as $3\%$ of the pool sizes of stabilized soil C and N.

`BD` is bulk density in $\mathrm{Mg}/\mathrm{m}^3$, and `FC` and `PWP` are the field capacity and permanent wilting point volumetric water contents, respectively, in $\mathrm{m}^3/\mathrm{m}^3$.
If any of these values are not known for a soil layer, you can enter `-999` and the model will estimate the values using pedotransfer functions.

`SON` is the soil organic nitrogen mass in kg/ha for each soil layer.
You can also enter `-999` for `SON`, in which case the model starts with a fixed C:N ratio of $9$.

`NO3` and `NH4` are the nitrate and ammonium masses in $\mathrm{kg}/\mathrm{ha}$ for each soil layer (important: the values are absolute masses, not concentrations).

`ROCK` is the fraction of coarse fragment (rock) in each soil layer, expressed in $\mathrm{m}^3/\mathrm{m}^3$.

`BYP_H` and `BYP_V` are the fractions of horizontal and vertical bypass flows in each soil layer.

`pH` is the soil pH of each layer.


## Crop Description File (\*.crop)

The crop description file defines physiological and management parameters that control the growth and harvest of crops used in the simulation.
Each crop used in the simulation must have an entry in the crop description file.
Each crop entry begins with the keyword tag `NAME` and is followed by the keywords listed in exactly the order below.

### `NAME`

The name of the crop.
This entry is used as the reference for crop management practices listed in the operation file, such as planting and harvesting.

### `THERMAL_TIME_TO_EMERGENCE`

The thermal time accumulates between when a crop is planted and when it emerges from the soil and begins intercepting radiation, transpiring, accruing biomass, etc., in growing $^{\circ}\mathrm{C}$ days.

### `FLOWERING_TT`

The thermal time to flowering, in growing $^{\circ}\mathrm{C}$ days, calculated using the base, optimum, and maximum temperatures for development listed in `BASE_TEMPERATURE_FOR_DEVELOPMENT`, `OPTIMUM_TEMPERATURE_FOR_DEVELOPEMENT`, and `MAX_TEMPERATURE_FOR_DEVELOPMENT`.

### `MATURITY_TT`

The thermal time to maturity in growing $^{\circ}\mathrm{C}$ days.

### `BASE_TEMPERATURE_FOR_DEVELOPMENT`

The base temperature ($T_{\mathrm{min}}$, $^{\circ}\mathrm{C}$) for phenological development and accumulation of thermal time.
Air temperatures below this value will not add to cumulative thermal time.
Air temperatures above this value and below `OPTIMUM_TEMPERATURE_FOR_DEVELOPMENT` will accumulate thermal time according to $T_\mathrm{air} – T_\mathrm{min}$.

### `OPTIMUM_TEMPERATURE_FOR_DEVELOPMENT`

When air temperatures are above this value ($T_{\mathrm{opt}}$, degree C) and below `MAX_TEMPERATURE_FOR_DEVELOPMENT` ($T_\mathrm{max}$), the accumulation of thermal time is calculated as

$$\frac{T_{\mathrm{max}} - T_{\mathrm{air}}}{T_{\mathrm{max}} - T_{\mathrm{opt}}}\left(T_\mathrm{opt} - T_\mathrm{min}\right)$$

### `MAX_TEMPERATURE_FOR_DEVELOPMENT`

Air temperatures above this value ($T_{\mathrm{max}}$, $^{\circ}\mathrm{C}$) will not add to cumulative thermal time.

### `PHOTOPERIOD_MIN`

Minimum daylength (hours) threshold for pre-flowering thermal time accumulation calculation.
Part of winter crop phenology parameters.

### `PHOTOPERIOD_MAX`

Minimum daylength (hours) threshold for pre-flowering thermal time accumulation calculation.
Part of winter crop phenology parameters.

### `THERMAL_TIME_VERNALIZATION`

The thermal time to completing vernalization, in growing $^{\circ}\mathrm{C}$ days.
Used to calculate the vernalization factor which affects pre-flowering thermal time accumulation based on vernalization progress.
Calculated using the base, optimum, and maximum temperatures for vernalization listed in `VERNALIZATION_TEMPERATURE_THRESHOLD_LOWER`, `VERNALIZATION_TEMPERATURE_THRESHOLD_OPTIMAL`, and `VERNALIZATION_TEMPERATURE_THRESHOLD_UPPER`.
Part of winter crop phenology parameters.

### `THERMAL_TIME_FLOWERING_AT_PMAX`

Thermal time to flowering, in growing $^{\circ}\mathrm{C}$ days.
Calculated using the default base, optimum, and maximum temperatures for development listed in `BASE_TEMPERATURE_FOR_DEVELOPMENT`, `OPTIMUM_TEMPERATURE_FOR_DEVELOPEMENT`, and `MAX_TEMPERATURE_FOR_DEVELOPMENT`.
Part of winter crop phenology parameters.

### `THERMAL_TIME_GRAIN_FILLING`

Thermal time post flowering to grain fill completion, in growing $^{\circ}\mathrm{C}$ days.
Calculated using the base, optimum, and maximum temperatures for post-flowering development listed in `TEMPERATURE_POST_FLOWERING_LOWER`, `TEMPERATURE_POST_FLOWERING_OPTIMAL`, and `TEMPERATURE_POST_FLOWERING_UPPER`.
Part of winter crop phenology parameters.

### `VERNALIZATION_TEMPERATURE_THRESHOLD_LOWER`

The base temperature ($T_{v,\mathrm{min}}$, $^{\circ}\mathrm{C}$) for accumulation of vernalization thermal time.
Air temperatures below this value will not add to cumulative thermal time.
Air temperatures above this value and below `VERNALIZATION_TEMPERATURE_THRESHOLD_OPTIMAL` will accumulate thermal time according to $T_\mathrm{air} – T_{v,\mathrm{min}}$.
Part of winter crop phenology parameters.

### `VERNALIZATION_TEMPERATURE_THRESHOLD_OPTIMAL`

When air temperatures are above this value ($T_{v,\mathrm{opt}}$, $^{\circ}\mathrm{C}$) and below `VERNALIZATION_TEMPERATURE_THRESHOLD_UPPER` ($T_{v,\mathrm{max}}$), the accumulation of vernalization thermal time is calculated as

$$\frac{T_{v,\mathrm{max}} - T_{\mathrm{air}}}{T_{v,\mathrm{max}} - T_{v,\mathrm{opt}}}\left(T_{v,\mathrm{opt}} - T_{v,\mathrm{min}}\right)$$

Part of winter crop phenology parameters.

### `VERNALIZATION_TEMPERATURE_THRESHOLD_UPPER`

Air temperatures above this value ($T_{v,\mathrm{max}}$, $^{\circ}\mathrm{C}$) will not add to cumulative vernalization thermal time.
Part of winter crop phenology parameters.

### `TEMPERATURE_POST_FLOWERING_LOWER`

The base temperature ($T_{p,\mathrm{min}}$, $^{\circ}\mathrm{C}$) for phenological development and accumulation of thermal time.
Air temperatures below this value will not add to cumulative post-flowering thermal time.
Air temperatures above this value and below `TEMPERATURE_POST_FLOWERING_OPTIMAL` will accumulate thermal time according to $T_\mathrm{air} – T_{p,\mathrm{min}}$.
Part of winter crop phenology parameters.

### `TEMPERATURE_POST_FLOWERING_OPTIMAL`

When air temperatures are above this value ($^{\circ}\mathrm{C}$) and below `TEMPERATURE_POST_FLOWERING_UPPER` ($T_{p,\mathrm{max}}$), the accumulation of post-flowering thermal time is calculated as

$$\frac{T_{p,\mathrm{max}} - T_{\mathrm{air}}}{T_{p,\mathrm{max}} - T_{p,\mathrm{opt}}}\left(T_{p,\mathrm{opt}} - T_{p,\mathrm{min}}\right)$$

Part of winter crop phenology parameters.

### `TEMPERATURE_POST_FLOWERING_UPPER`

Air temperatures above this value ($T_{p,\mathrm{max}}$, $^{\circ}\mathrm{C}$) will not add to cumulative post-flowering thermal time.
Part of winter crop phenology parameters.

### `DEVERNALIZATION_TEMPERATURE`

The air temperature threshold ($T_d$, $^{\circ}\mathrm{C}$) above which vernalization progress is reversed.
When the air temperature exceeds this value, $T_{\mathrm{air}} - T_d$ is subtracted from the accumulated vernalization thermal time for that day.
Part of winter crop phenology parameters.

### `INITIAL_PARTITIONING_TO_SHOOT`

The fraction of growth partitioned to aboveground biomass (as opposed to roots) at the beginning of crop phenological development (seedling emergence).
Values can range from $0$ to $1$.
Perennials have lower values than annuals.

### `FINAL_PARTITIONING_TO_SHOOT`

The fraction of growth partitioned to aboveground biomass (as opposed to roots) at the end of crop phenological development (physiological maturity). Values can range from $0$ to $1$.
Perennials have lower values than annuals.

### `MAXIMUM_HARVEST_INDEX`

The maximum harvest index possible ($H_{\mathrm{max}}$), used in the calculation of actual harvest index described below.
It is an asymptotic value that can be much higher than the actual maximum HI observed (e.g., $0.8$ for corn).
Use default values if needed.

### `MINIMUM_HARVEST_INDEX`

The minimum harvest index possible ($H_{\mathrm{min}}$), used in the calculation of actual harvest index described below.
It has some semblance to the fraction of water soluble carbohydrates at flowering or in the first week after flowering in grain crops.
Values range from $0$ to $0.3$.

### `HARVEST_INDEX_SLOPE_MULTIPLIER`

A slope multiplier ($m$) between $0.01$ and $1$ used in the calculation of harvest index.
The harvest index is calculated based on the proportion of aboveground crop biomass growth occurring post-anthesis relative to total aboveground growth over the entire duration of crop development ($f_G$) as follows:

$$\mathrm{HI} = H_{\mathrm{max}} - \left(H_{\mathrm{max}} - H_{\mathrm{min}}\right) \exp\left(-H_k f_G\right)$$

where $H_k = m\left(1 - H_{\mathrm{min}}\right) / \left(H_{\mathrm{max}} - H_{\mathrm{min}}\right)$.

### `N_MAX_CONCENTRATION_GRAIN`

The maximum allowable grain N concentration ($\%$), used in nitrogen harvest index calculation.

### `N_MIN_CONCENTRATION_GRAIN`

The minimum allowable grain N concentration ($\%$), used in nitrogen harvest index calculation.

### `N_MAX_CONCENTRATION_STRAW`

The maximum allowable straw N concentration at harvest ($\%$), used in nitrogen harvest index calculation.

### `N_MIN_CONCENTRATION_STRAW`

The minimum allowable straw N concentration at harvest ($\%$), used in nitrogen harvest index calculation.

### `N_PARTITIONING_FACTOR`

An empirical constant in N partitioning (-) used in nitrogen harvest index calculation.

### `MAXIMUM_ROOTING_DEPTH`

The maximum depth to which crop roots can grow in meters.


### `RADIATION_USE_EFFICIENCY`

The radiation use efficiency ($\mathrm{RUE}$) of the crop in $\mathrm{g}/\mathrm{MJ}$.
Units in $\mathrm{MJ}$ of solar radiation (not PAR).

### `TRANSPIRATION_USE_EFFICIENCY`

The transpiration use efficiency ($\mathrm{TUE}$) of the crop in $\mathrm{g}/\mathrm{kg}$ of water when the $\mathrm{VPD} = 1 \mathrm{~kPa}$.


### `MIN_TEMPERATURE_FOR_TRANSPIRATION`

The air temperature ($T_{t,\mathrm{min}}$, daily average, $^{\circ}\mathrm{C}$) below which transpiration by the plant virtually ceases (transpiration is $1\%$ of potential transpiration).
This value can constrain crop growth at low temperatures because crop growth is calculated as the minimum of $\mathrm{TUE}\times T$ or $\mathrm{RUE}\times R_i$,
where $T$ is the transpiration rate and $R_i$ is the intercepted radiation.
Therefore, if transpiration does not occur, plant biomass accrual does not occur.

### `THRESHOLD_TEMPERATURE_FOR_TRANSPIRATION`

The threshold temperature ($T_{t,\mathrm{thresh}}$, daily average, $^{\circ}\mathrm{C}$) below which potential transpiration is multiplied by a linear temperature reduction factor, where the reduction factor is calculated as:

$$\frac{T_\mathrm{air} - T_{t,\mathrm{min}}}{T_{t,\mathrm{thresh}} - T_{t,\mathrm{min}}}$$

### `KC`

The crop transpiration coefficient, used to convert reference evapotranspiration based on environmental conditions into crop transpiration.


### `LWP_STRESS_ONSET`

The leaf water potential ($\mathrm{J}/\mathrm{kg}$) threshold for the onset of stress, below which stomatal closure begins, reducing transpiration rates.

### `LWP_WILTING_POINT`

The leaf water potential ($\mathrm{J}/\mathrm{kg}$) at wilting point, where transpiration stops.

### `TRANSPIRATION_MAX`

The maximum crop transpiration rate in $\mathrm{mm}/\mathrm{day}$.


### `MIN_TEMPERATURE_FOR_COLD_DAMAGE`

The minimum temperature ($T_{c,\mathrm{min}}$, $^{\circ}\mathrm{C}$) used in the calculation of the cold damage factor described below.
If the daily minimum temperature is below this value, the cold damage factor is set to $0.99$.

### `THRESHOLD_TEMPERATURE_FOR_COLD_DAMAGE`

The threshold temperature ($T_{c,\mathrm{thresh}}$, $^{\circ}\mathrm{C}$) used in the calculation of the cold damage factor.
If the daily minimum temperature falls below this threshold value, a cold damage factor ($f_c$) is calculated as:

$$1 - \frac{T_\mathrm{air} - T_{c,\mathrm{min}}}{T_{c,\mathrm{thresh}} - T_{c,\mathrm{min}}}$$

The cold damage factor reduces the aboveground crop biomass (moving it to the surface residue pools), reduces radiation interception by the canopy, and delays phenology development.
Aboveground biomass and radiation interception is reduced by a factor calculated as $1 – f_c^3$.
As an example, a daily minimum temperature below the `MIN_TEMPERATURE_FOR_COLD_DAMAGE` will cause the existing aboveground biomass and radiation interception to be multiplied by $1-0.99^3=0.03$, with the $97\%$ of existing biomass that was killed added to the residue pools.
So for a crop with an aboveground biomass of $1000 \mathrm{~kg}/\mathrm{ha}$, a day in the simulation with a minimum temperature below the `MIN_TEMPERATURE_FOR_COLD_DAMAGE` will reduce the aboveground biomass to $30\mathrm{~kg}/\mathrm{ha}$ and add $970\mathrm{~kg}/\mathrm{ha}$ of biomass to the residue pools.

Important Note:  The `THRESHOLD_TEMPERATURE_FOR_COLD_DAMAGE` value can also trigger a grain harvest of a growing crop.
If the crop is an annual species and the cumulative thermal time of crop development is greater than the `FLOWERING_TT` value, the crop will be automatically harvested as a grain crop.

### `N_MAX_CONCENTRATION`

The maximum N concentration in crop tissue ($\mathrm{g}/\mathrm{g}$), a feature that regulates the N requirements for growth through the N dilution curve.

### `N_DILUTION_SLOPE`

A parameter that controls the slope of the N dilution curve, which regulates crop N requirements as biomass accrues.


### `ANNUAL`

Whether the crop is annual (`1`) or perennial (`0`).

### `LEGUME`

Whether the crop is a legume (`1`) or a non-legume (`0`).

### `C3`

Whether the crop has a C3 photosynthetic pathway (`1`) or a C4 pathway (`0`).



## Management Operations File (\*.operation)

The management operation file sets the daily management operations to be used in a simulated crop rotation.
Each operation is indicated by one of five possible headers, `PLANTING`, `TILLAGE`, `FIXED_FERTILIZATION`, `FIXED_IRRIGATION`, or `AUTO_IRRIGATION`.
For each operation type, subsequent keywords tags followed by a tab-delimited value are required in a specific order.
Each operation includes an entry for the `YEAR`, and `DOY` (numerical day of the year) in which the operation is to be performed.
`YEAR` refers to a generic year in the rotation cycle, starting with year 1, rather than an actual calendar year.
The rotation will start in the calendar year specified in the control file under `SIMULATION_START_YEAR` and will repeat itself as many times as possible until the simulation reaches the end of the calendar year specified under `SIMULATION_END_YEAR` in the control file.
In this manner, the rotation in an operation file can be started in any calendar year specified by the control file and can run for as many rotation cycles as desired, given that a weather file with a sufficient time span is available.

It is important that entries in the operation file are listed in chronological order by `YEAR` and `DOY`.
If operations are listed out of order, they will be read into the management array in the wrong order and may not be executed with the proper timing in the simulation.

### `PLANTING`

The `PLANTING` operation plants a crop.
The following keyword tags are needed after the `PLANTING` header.

#### `YEAR`

The year in the rotation in which the planting operation is to be performed.

#### `DOY`

The numerical day of the year (0 to 365 or 366) on which the planting operation is to be performed.
If conditional planting is activated (i.e., `MIN_SMC` and `MIN_SOIL_TEMP` are not `-999`), this is the beginning of the planting window.

#### `END_DOY`

The last numerical day of the year (0 to 365 or 366) on which the planting operation is to be performed when conditional planting is activated.
Set the value to `-999` to disable conditional planting.

#### `CROP`

The name of the crop to be planted.
This must match the name of one of the crops described in the crop description file (`*.crop`).
If the automatic irrigation feature is activated below, the crop name here must match the crop name in the `AUTO_IRRIGATION` operation entry.

#### `MAX_SMC`

Maximum soil moisture (fraction between PWP and FC) allowed to perform planting.
A value of $0$ represents PWP and a value of $1$ represents FC.
Set the value to `-999` to disable maximum soil moisture control on conditional planting.

#### `MIN_SMC`

Minimum soil moisture (fraction between PWP and FC) required to perform planting.
A value of $0$ represents PWP and a value of $1$ represents FC.
Set the value to `-999` to disable minimum soil moisture control on conditional planting.

#### `MAX_SOIL_TEMP`

Maximum 7-day moving average soil temperature required to perform planting in $^{\circ}\mathrm{C}$.
Set the value to `-999` to disable maximum soil temperature control on conditional planting.

#### `MIN_SOIL_TEMP`

Minimum 7-day moving average soil temperature required to perform planting in $^{\circ}\mathrm{C}$.
Set the value to `-999` to disable minimum soil temperature control on conditional planting.

#### `USE_AUTO_IRR`

Whether or not the crop should be grown with automatic irrigation.
Yes = `1`, No = `0`.
If automatic irrigation is activated, there must be an associated `AUTO_IRRIGATION` entry in the operation file with the same crop name as is used in the `CROP` keyword here.

#### `USE_AUTO_FERT`

Whether or not the crop should be grown with automatic fertilization (growth unrestricted by nutrient limitations).
Yes = `1`, No = `0`.

#### `DENSITY`

The planting density ($0$ to $1$), where $1$ is a monoculture planting rate and $< 1$ indicates a proportionally reduced seeding rate.
This feature allows for the planting of multiple species in a plant community at different initial densities.

#### `CLIPPING_START`

The numerical day of the year on which the clipping time window begins.
Set the value to `-999` to disable automated harvesting of the corresponding crop.
A value of `-999`, however, cannot disable scheduled harvests.

#### `CLIPPING_END`

The numerical day of the year on which the clipping time window ends.

#### `MAXIMUM_SOIL_COVERAGE`

The maximum crop cover in $\%$ is a proxy for light interception potential given by row spacing and other features.
This value can be set to less than $100\%$ if the crop canopy does not have the potential to provide full cover, or if the crop is planted with a row spacing such that there will be bare ground present between crop rows (e.g., horticultural production or dry areas).

#### `STANDING_RESIDUE_AT_HARVEST`

The percent of aboveground crop residues remaining in the field that are left in the standing position (i.e., in minimal contact with the soil surface and therefore decomposing at a slower rate).
The reminder of the reside will lay flat on the ground.

#### `RESIDUE_REMOVED`

For crops harvested as grain, the percent of non-grain crop biomass that is removed with the harvested grain.
For crops harvested as forages or grazed through the clipping controls, the percent of harvestable aboveground plant biomass that is removed by the harvest.
For clipping events, this value also controls the fraction of root biomass that is killed at each clipping event and the extent to which the thermal time of crop development is set back at each clipping event.

Interactions: The harvestable aboveground plant biomass is the biomass quantity above the `CLIPPING_BIOMASS_THRESHOLD_LOWER` value.

#### `CLIPPING_BIOMASS_THRESHOLD_LOWER`

The aboveground plant biomass threshold (in $\mathrm{Mg}/\mathrm{ha}$), which remains un-harvestable during clipping events.
This value represents the mass of living plant biomass that would remain in the field after a clipping event with a `RESIDUE_REMOVED` value of $100$.

#### `CLIPPING_BIOMASS_THRESHOLD_UPPER`

The aboveground plant biomass threshold (in $\mathrm{Mg}/\mathrm{ha}$) that will trigger a clipping event or forage harvest.

Interactions: Clipping will only be allowed if the clipping date window for the crop, which is defined in the operation file, is open.
The `HARVEST_TIMING` value also controls clipping events, and clipping will occur when either the biomass threshold or the `HARVEST TIMING` threshold is reached, whichever occurs first.
The destination of clippings, either returned to the soil surface, harvested, or grazed is controlled by the `CLIPPING_BIOMASS_DESTINY` option.
The percent of aboveground biomass that is clipped is controlled by the `RESIDUE_REMOVED` value.

#### `CLIPPING_BIOMASS_DESTINY`

This value controls the destiny of biomass cut by clipping events, with options of `REMOVE`, `RETURN`, or `GRAZING`.
`REMOVE` will treat the clipped biomass as a harvested crop.
`RETURN` will treat the clipped biomass as a residue returned to the soil surface, with the allocation to standing versus flattened residue pools controlled by the `STANDING_RESIDUE_AT_HARVEST` value.
`GRAZING` will treat the clipped biomass as being consumed by livestock, with $50\%$ of the carbon in clipped biomass returned to the soil surface manure C pool and $50\%$ of the carbon respired by livestock as CO<sub>2</sub>, and with $50\%$ of the nitrogen in clipped biomass returned to the soil surface in the manure N pool and $50\%$ of the nitrogen returned as urine to the NH<sub>4</sub><sup>+</sup> pool of the first soil layer.

#### `HARVEST_TIMING`

The percent of thermal time to crop maturity that will trigger a clipping event or grain harvest.
As an example, if `HARVEST_TIMING` is set to $90$, and the `MATURITY_TT` is $1000$, the crop will be clipped when the cumulative thermal time of crop development reaches $900$ growing $^{\circ}\mathrm{C}$ days.
If the harvest timing is less than $100$, the crop will be clipped as a forage and will continue to regrow.
If the harvest timing is greater than $100$, the crop will have reached physiological maturity and will be harvested as a grain crop and killed after harvested.
A value of `-999` can be entered to trigger a default value of $101$, which will result in the crop being harvested for grain on the first day after it reaches physiological maturity.

Interactions: The `CLIPPING_BIOMASS_THRESHOLD_UPPER` value also controls if and when a crop will be clipped.
If the `HARVEST_TIMING` value is greater than 100, indicating a desire to harvest the crop for grain, but the `CLIPPING_BIOMASS_THRESHOLD_UPPER` value is set within a range of biomass production levels that will be achieved during growth of the crop, the crop will get clipped as a forage.
Therefore, when programming a crop to be grown as a grain crop, the `CLIPPING_BIOMASS_THRESHOLD_UPPER` value should be set to a very high value, such as `999`, so that clipping will not be triggered before physiological maturity is reached.

#### `KILL_AFTER_HARVEST`

Whether to kill the crop after automatic grain or forage harvests.
Note that crops will not be killed after scheduled harvests in the operation files.
To kill the crops after scheduled harvests, `KILL_CROP` tillage operations need to be scheduled.

### `TILLAGE`

The tillage operation can cause soil disturbances of varying intensity, for instance ranging from the slight disturbance of disc openers on a no-till planter to full-scale inversion tillage.
Tillage operations can also be used to harvest or kill planted crops.
The following keyword tags are needed after the `TILLAGE` header.

#### `YEAR`

The year in the rotation in which the tillage operation is to be performed.

#### `DOY`

The numerical day of the year (0 to 365) on which the tillage operation is to be performed.

#### `TOOL`

A descriptive name of the tillage tool described by the operation entry.
The `TOOL` must be a valid tool described in the `input/tillage_tools.txt`, with some exceptions:
The string `Kill_Crop` is used to kill a crop without harvesting any residues;
the string `Grain_Harvest` is used to perform a grain harvest;
the string `Forage_Harvest` is used to perform a forage harvest;
and the string `Burn_Residue` is used to burn residue that is aboveground.
Note that scheduled grain harvests do not kill crops, and scheduled harvests are performed even if `CLIPPING_START` is set to `-999`.
When burning residue, the `KILL_EFFICIENCY` parameter is used to determine the fraction burned.

To kill a crop after a scheduled harvest, a `Kill_Crop` tillage operation should be used.

In the `input/tillage_tools.txt` file, the `TOOL` name is followed by a set of parameters that describe the tillage tool:

`DEPTH`:
The soil depth (in meters) to which the tillage operation will cause mixing and disturbance induced increases in microbial respiration.

`SOIL_DISTURB_RATIO`:
A relative value that indicates the extent to which the tillage operation speeds up decomposition rate of the soil microbial biomass and stabilized soil C pools.

`MIXING_EFFICIENCY`:
A factor used to control the extent to which soil layers and residue pools mix with each other during a tillage event (range is $0$ to $1$).
The `MIXING_EFFICIENCY` value specifies the mass fraction of the soil mineral and organic pools in each soil layer within the depth range of the operation that will be mixed together and redeposited in each soil layer.
Pools that are mixed include clay and sand fractions, stabilized soil C and N, microbial biomass C and N, crop residue biomass and N, manure C and N, nitrate and ammonium, water content, and surface residues.
The mixing efficiency will also flatten a fraction of the standing crop residue equal to the square root of the mixing efficiency.

#### `CROP_NAME`

If the `TILLAGE` operation is being used to harvest or kill a crop, this tag indicates the crop name to be harvested or killed.
To kill all the crops in a planted community, use `All` or `N/A`.
To harvest a crop, use the crop name exactly as written in the `PLANTING` operation and crop description file.

#### `FRAC_THERMAL_TIME`

This keyword is not activated in the current version of Cycles, so leave the value as `0`.
In future versions, this keyword will be used to program tillage events at specific times relative to crop phenological development.

#### `KILL_EFFICIENCY`

This keyword is not activated in the current version of Cycles, except for the `Burn_Residue` option..
When using the `Burn_Residue` option, the `KILL_EFFICIENCY` value specifies the fraction of aboveground residue that is burned.
In future versions, this keyword will be used to generate an incomplete kill of a crop in order to simulate the effect of competition from a crop previous crop that was not completely killed.

### `FIXED_FERTILIZATION`

The fixed fertilization operation is used to apply organic and inorganic soil fertility inputs in a simulation.
The following keyword tags are needed after the `FIXED_FERTILIZATION` header.

#### `YEAR`

The year in the rotation in which the fixed fertilization operation is to be performed.

#### `DOY`

The numerical day of the year (0 to 365) on which the tillage operation is to be performed.

#### `SOURCE`

A descriptive name of the fertility input described by the entry.
The `SOURCE` must be a valid fertilizer described in the `input/fertilizers.txt`.

In the `input/fertilizers.txt` file, the `FERTILIZER` name is followed by a set of parameters that describe the fertilizer:

`C_ORGANIC`:
The fraction of input mass that is organic C.
This fraction will be added to the manure C pool.

`C_CHARCOAL`:
This keyword is not currently activated.

`N_ORGANIC`:
The fraction of input mass that is organic N.
This fraction will be added to the manure N pool.

`N_CHARCOAL`:
This keyword is not currently activated.

`N_NH4`:
The fraction of input mass that is ammonium N.
This fraction will be added to the ammonium pool.

`N_NO3`:
The fraction of input mass that is nitrate N.
This fraction will be added to the nitrate pool.

`P_ORGANIC`:
This keyword is not currently activated.

`P_CHARCOAL`:
This keyword is not currently activated.

`P_INORGANIC`:
This keyword is not currently activated.

`K`:
This keyword is not currently activated.

`S`:
This keyword is not currently activated.

#### `MASS`

The total mass of the input in $\mathrm{kg}/\mathrm{ha}$.

#### `FORM`

This keyword is not currently activated.
Enter any string.

#### `METHOD`

This keyword is not currently activated.
Enter any string.

#### `DEPTH`

The soil depth (in meters) to which the fertilizer input is added.
To specify surface applied fertilizer applications, use a value of $0$.
Organic components of surface applied fertilizer inputs will be added to the surface manure C and N pools, while inorganic components of the surface applied inputs will be added to the nitrate or ammonium pools of the first soil layer.

### `FIXED_IRRIGATION`

The fixed irrigation operation is used to provide a one-time application of water to simulate an irrigation event.
The following keyword tags are needed after the `FIXED_IRRIGATION` header.

#### `YEAR`

The year in the rotation in which the tillage operation is to be performed.

#### `DOY`

The numerical day of the year (0 to 365) on which the tillage operation is to be performed.

#### `VOLUME`

The volume of water to be added in $\mathrm{mm}$.
Irrigation water is added to the soil surface and is combined with precipitation and snowmelt in calculating the amount of water infiltration versus runoff.

### `AUTO_IRRIGATION`

The automatic irrigation operation is used to provide irrigation events triggered by specific water depletion levels.
The automatic irrigation event will calculate the quantity of water to add that is needed to bring the soil layers within and above the `DEPTH` to field capacity.
These operation entries are only read into the simulation controls if one or more crops have the `USE_AUTO_IRR` keyword set to `1` in a `PLANTING` operation entry.
The following keyword tags are needed after the `AUTO_IRRIGATION` header.

#### `CROP`

The crop that the `AUTO_IRRIGATION` operation applies to.
This entry must match the name entered in the `CROP` keyword of the `PLANTING` operation.

#### `START_DAY`

The first numerical day of the year at which auto irrigation events can be triggered.

#### `STOP_DAY`

The last numerical day of the year at which auto irrigation events can be triggered.

#### `WATER_DEPLETION`

The fraction of the plant available water capacity that can be depleted before triggering an automatic irrigation event.
The fraction of plant available water depleted is integrated across the soil layers including and above the layer specified by `LAST_SOIL_LAYER`.
Example: entering a value of 0.05 means that an auto irrigation event will be triggered when the plant available water content (volumetric water content above the permanent wilting point but not greater than field capacity) falls below 95% of the plant available water capacity (the volumetric water content between permanent wilting point water content and field capacity water content).

#### `DEPTH`

The lowest depth that will be used in the determination of auto irrigation requirements and volumes.


## Reinitialization File (\*.reinit)

Re-initialization files are optional, and typically have a suffix of `.reinit`, but any naming convention can be used as long as it matches the reinitialization file name listed in the control file.
When reinitialization option is used, the model variables of choice will be replaced by the values specified in the reinitialization file.

The reinitialization file starts with a row specifying `YEAR`, and `DOY`.
`YEAR` and `DOY` set the simulation year and the numerical day of the year that reinitialization occurs.
The re-reinitialization variables are organized in a table-like fashion.
The header lines describe the names of the variables, and the values follow.
Currently, the following options are available:
`SMC` ($\mathrm{m}^3/\mathrm{m}^3$),
`NO3` ($\mathrm{kg}/\mathrm{ha}$),
`NH4` ($\mathrm{kg}/\mathrm{ha}$),
`SOC` ($\mathrm{Mg}/\mathrm{ha}$),
`SON` ($\mathrm{Mg}/\mathrm{ha}$),
`MBC` ($\mathrm{Mg}/\mathrm{ha}$),
`MBN` ($\mathrm{Mg}/\mathrm{ha}$),
`RESIDUEABGDC` ($\mathrm{Mg}/\mathrm{ha}$),
`RESIDUERTC` ($\mathrm{Mg}/\mathrm{ha}$),
`RESIDUERZC` ($\mathrm{Mg}/\mathrm{ha}$),
`RESIDUEABGDN` ($\mathrm{Mg}/\mathrm{ha}$),
`RESIDUERTN` ($\mathrm{Mg}/\mathrm{ha}$),
`RESIDUERZN` ($\mathrm{Mg}/\mathrm{ha}$),
`MANUREC` ($\mathrm{Mg}/\mathrm{ha}$),
`MANUREN` ($\mathrm{Mg}/\mathrm{ha}$), and
`FERMENTC` ($\mathrm{Mg}/\mathrm{ha}$),
`FERMENTN` ($\mathrm{Mg}/\mathrm{ha}$), and
`SATURATION` ($\mathrm{100\%}$)
of different soil layers; and
`STANRESIDUEC` ($\mathrm{Mg}/\mathrm{ha}$),
`FLATRESIDUEC` ($\mathrm{Mg}/\mathrm{ha}$),
`STANRESIDUEN` ($\mathrm{Mg}/\mathrm{ha}$),
`FLATRESIDUEN` ($\mathrm{Mg}/\mathrm{ha}$),
`MANURESURFACEC` ($\mathrm{Mg}/\mathrm{ha}$),
`MANURESURFACEN` ($\mathrm{Mg}/\mathrm{ha}$),
`FERMENTSURFACEC` ($\mathrm{Mg}/\mathrm{ha}$),
`FERMENTSURFACEN` ($\mathrm{Mg}/\mathrm{ha}$),
`STANRESIDUEWATER` ($\mathrm{mm}$),
`FLATRESIDUEWATER` ($\mathrm{mm}$), and
`INFILTRATION` ($\mathrm{mm}/\mathrm{day}$).

Please refer to the `input/CornSoyWheatPasture.reinit` file for an example of a reinitialization file.
The structure can repeat to specify re-initialization for as many dates as needed.
To disable the re-initialization of certain variables, use `-999` as their values.

## Calibration File (\*.nudge)

Calibration files are optional, and are only required when running Cycles in the calibration mode (with the `-n` command line option).
When the calibration mode is used, a calibration file named `<simulation name>.nudge` should be provided in the `input` directory.

The calibration files contain calibration multipliers for 12 model parameters, and values of two model parameters.
When running in the calibration mode, the corresponding parameters will be multiplied by the calibration multipliers.
A calibration multiplier of 1 means the default hard-coded parameter value will be used.

Currently, the following options are available for calibration multiplier:
`SOC_DECOMP_RATE` (soil organic carbon decomposition rate),
`RESIDUE_DECOMP_RATE` (residue decomposition rate),
`ROOT_DECOMP_RATE` (root decomposition rate),
`RHIZO_DECOMP_RATE` (rhizome decomposition rate),
`MANURE_DECOMP_RATE` (manure decomposition rate),
`FERMENT_DECOMP_RATE` (ferment decomposition rate),
`MICROB_DECOMP_RATE` (microbe decomposition rate),
`SOC_HUMIF_POWER` (soil organic carbon humification exponent),
`NITRIF_RATE` (nitrification rate),
`POT_DENITRIF_RATE` (potential denitrification rate),
`DENITRIF_HALF_RATE` (half saturation constant for denitrification),
`DECOMP_HALF_RESP` (decomposition half response to saturation, default value 0.22),
`DECOMP_RESP_POWER` (decomposition exponential response to saturation, default value 3.0).
`ROOT_PROGRESSION` (rooting depth progression rate).
`RADIATION_USE_EFFICIENCY` (crop-specific radiation use efficiency).

Parameter values for `KD_NO3` (adsorption coefficient for NO<sub>3</sub><sup>-</sup>, default value $0.0\mathrm{~cm}^3/\mathrm{g}$) and `KD_NH4` (adsorption coefficient for NH<sub>4</sub><sup>+</sup>, default value $5.6\mathrm{~cm}^3/\mathrm{g}$) should also be provided in calibration files.
