# Cycles: An Agroecosystems Simulation Model<br>User Reference Guide
<div style="text-align: center;"><p>
Prepared by Charles M. White, Yuning Shi, Emmy Wwang, and Armen R. Kemanian
</p></div>


Cycles is a daily time-step agroecosystem model that simulates the biophysical processes and management practices occurring in a conceptual field that subtends a sequence of crops and operations or cropping system.
Processes include fluxes in the water and energy balance and the coupled cycling of carbon and nitrogen including vegetation growth. The model can simulate a wide range of agricultural management practices such as tillage, inorganic and organic nutrient additions, annual and perennial crops, crop harvests as grain or forages, polycultures and relay cropping, grazing, and irrigation.
Crop growth is represented with a generalizable framework such that a nearly limitless variety of agricultural crop species can be specified by the user.

Cycles is written in C and the executables for different operating systems are released from the [Cycles GitHub repository release page](https://github.com/PSUmodeling/Cycles/releases).
Text-based input files that specify the simulation control parameters, a soil profile description, crop descriptions, the sequence of management operations, and weather drive each user-defined simulation.
An optional re-initialization file can be used to reset model variables to desired values as described in the re-initialization file.
An optional calibration file can be used in calibration mode to provide an interface for calibrating hard-coded model variables.
Outputs for various pools and fluxes in the agro-ecosystem are written to tab-delimited text files that can be opened by most spreadsheet programs.

## Getting Started

To get started running the model, download the package based on your operating system from the [release page](https://github.com/PSUmodeling/Cycles/releases) under the "Assets" menu and decompress the package to a working directory of your choice.
For Windows users, please use `Cycles_win_XXX.zip`, Mac users `Cycles_macos_XXX.zip`, and Linux/Unix users `Cycles_debian_XXX.zip`.

The input directory is where you store the various input files needed to drive each simulation.
Each simulation needs a control file (`*.ctrl`), an operation file (`*.operation`), a soil profile description file (`*.soil`), a crop description file (`*.crop`), and a weather file (`*.weather`).
A re-initialization file (`*.reinit`) and a calibration file (`*.nudge`) are optional.
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

Outputs from the model are written to files located in a subdirectory named `output`.
If the `output` subdirectory does not already exist within your working directory, the program will create it.
To keep your outputs organized, the program creates a new subdirectory within the `output` directory with the same name as the simulation control file.
If you run the same simulation control file multiple times, the program will **overwrite** output files in the folder each time, unless the `-e` option is used to customize the output file extension.

**Windows users:** It is important to note that if any of the output files are open in a spreadsheet software when a new simulation is run, those files will not be overwritten successfully.
To save an existing output file within the `output` subdirectory and prevent the results from being overwritten by subsequent simulation runs, you can manually change the name of the file in Windows Explorer, for instance by appending a keyword of your choice to the filename.

## Spin-up Mode

Cycles includes a spin-up feature that enables users to run the model to equilibrium.
In spin-up mode, the model recycles the weather forcing and operations between the specified simulation start year and end year, until the model reaches equilibrium (i.e., the change in total soil organic carbon before and after the simulation is below 1%).
The spin-up is followed by a simulation starting from equilibrium.
No output file is written during the spin-up.
Only the last simulation from equilibrium produces model output.
The spin-up simulation also generates a `<simulation name>_ss` file in the `input` directory, which contains the soil conditions at equilibrium, and can be used to drive other simulations.
To start a simulation with spin-up:

```shell
./Cycles -s <simulation name>
```

or, in Windows:

```shell
Cycles_win.exe -s <simulation name>
```

## Grain Growth Model

Cycles includes a grain growth model.
To use the grain growth model:

```shell
./Cycles -g <simulation name>
```

or, in Windows:

```shell
Cycles_win.exe -g <simulation name>
```

## Nudge (Calibration) Mode

Cycles includes a "nudge" (calibration) mode.
In calibration mode, a [calibration file](input.md#calibration-file-nudge) named `<simulation name>.nudge` in the input folder is required.
The calibration multipliers and parameters are used to nudge the hard-coded parameter values in Cycles.
To run Cycles in calibration mode:

```shell
./Cycles -n <simulation name>
```

or, in Windows:
```shell
Cycles_win.exe -n <simulation name>
```

## Break and Resume a Simulation

Cycles includes a feature to break and resume a simulation, which can be coupled with decision support systems to run the model in real-time.
There are two ways to break a simulation:

The first is by harvesting all planted crops that have been scheduled:

```shell
./Cycles -b <simulation name>
```

or, in Windows:

```shell
Cycles_win.exe -b <simulation name>
```

The second is by reaching a specified date in the simulation:

```shell
./Cycles -bYYYYDOY <simulation name>
```

or, in Windows:

```shell
Cycles_win.exe -bYYYYDOY <simulation name>
```

Note that there is no space between the `-b` and the date, which is specified in the format of `YYYYDOY`, where `YYYY` is the year and `DOY` is the day of year (1-365 or 366).

To resume a simulation that has been broken, use the following command:

```shell
./Cycles -r <simulation name>
```

or, in Windows:

```shell
Cycles_win.exe -r <simulation name>
```

Between the break and resume commands, the control file, soil description file, and crop description file should not be modified.
But the operation file and weather file can be appended to change the management practices and weather forcing for the remainder of the simulation.

## Cycles-utils Python Package

A [`Cycles-utils` Python package](https://github.com/PSUmodeling/Cycles-utils) has been developed to facilitate Cycles agroecosystem model simulations.
This package provides a number of tools for users to prepare Cycles simulation input files, run Cycles simulations, and post-process Cycles simulation results.
For basic usage examples, please refer to this [Jupyter notebook](https://github.com/PSUmodeling/Cycles/blob/master/cycles-utils.ipynb).
For more complex usage examples that uses , please refer to this [Colab notebook](https://colab.research.google.com/drive/1d8TFfZR_t6eo-Lgh8ZdR382RUcAXlawf?usp=sharing).
