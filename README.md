Cycles
======
The model Cycles is a multi-year, multi-crop, multi-layered (soil), one-dimensional, daily or sub-daily time step simulation platform that simulates water, carbon and nitrogen balance of the soil-crop system subject to climate conditions and a large array of management constraints.

![alt text][logo]
[logo]: http://plantscience.psu.edu/research/labs/kemanian/models-and-tools/cycles/leadImage_galleryzoom "Cycles"

Cycles simulates crop growth and nutrient cycling and predicts a range of variables for agricultural yield (grain and forage) of annual and perennial crops, and nitrogen losses.
Many modules in Cycles are related to the model CropSyst (Stöckle and Nelson, 2003), and the fundamental heat and water transport equations and algorithms are adapted from Campbell (1985) *Soil Physics with Basic* landmark book.
Cycles modules are modularized and easily unplugged from the main interface and which permits efficient addition / removal of algorithms and coupling with watershed simulation models.

Cycles has unique solutions to the coupled cycling of carbon, nitrogen and water in the soil-plant-atmospheric system.
Among the original components in Cycles are new developments in carbon and nitrogen saturation theory (soil), and an old but robust implementation of stomatal optimization theory (vegetation) to simulate plant growth, water use efficiency, and the response of crops to increasing carbon dioxide.

This is the C version of Cycles.

INSTALLATION
------------
Download the code to your work directory. Enter the `Cycles` directory and run
~~~shell
$ make
~~~
to install Cycles.

RUN CYCLES
----------
The syntax is
~~~shell
$ ./Cycles [-v] [-d] <project name>
~~~
The optional -v parameter will turn on the verbose mode.
The optional -d parameter will turn on the debug mode.

INPUT FILES
-----------
The Cycles requires XX input files:
