# Lidar Processing Pipeline (LPP)

The LPP is a collection of tools developed in C/C++ and Linux script, planned to handle all the steps of lidar analysis. A first tool converts the raw data files into a single NetCDF file, including detailed information about the instrument and acquisition setup (level 0 dataset). The produced files are then processed by another tool that applies the necessary corrections and computes the cloud-mask (level 1 dataset). The final step is the elastic retrieval of aerosol properties (level 2 dataset). The development of LPP is based on the existing analysis routines developed by individual LALINET groups, and hence takes advantage of previous efforts for algorithm comparison in the scope of the LALINET network
The codes presented in this repository are tested on Linux Ubuntu 20.04.3 LTS, and Windows 10 using WSL.

## Overall concept of the LPP tools

The Lidar Processing Pipeline (LPP) is formed by 3 completely independent modules. The names of each one are represented by the output of its product data level (PDL), named from 0 to 2. Each one of these modules can be executed in a command line following the rules described in this document. These modules are:
- `lidarAnalysis_PDL0`: Transforms all the raw lidar data files stored in a folder (passed as an argument) to a single NetCDF file. This output file will contain the raw lidar signals and global information about the measurement. The variable's name follows the name convention of the Single Calculus Chain (SCC) platform. This output is the L0 data level of LPP.
- `lidarAnalysis_PDL1`: Receive the NetCDF file produced by `lidarAnalysis_PDL0` and produce a new NetCDF defined as data level 1 (L1). This L1 file contain the corrected lidar files (like laser offset, bias correction, etc.), the cloud-mask product and molecular profiles for every wavelength used. Also, all the parameters used to produce this output are stored. This output is called L1 data level of LPP.
- `lidarAnalysis_PDL2`: Receive the NetCDF file produced by `lidarAnalysis_PDL1` and produce a new NetCDF file defined as data level 2 (L2). This L2 file contain the optical products obtained from the elastic lidar signals contained in the NetCDF input file. Also, all the parameters used to produce this output are stored. This output is the L2 data level of LPP.

Each lidar analysis tool must be run in a Linux terminal, following the convention:

   $./lidarAnalysis_PDLx /Input_File_or_Folder/ /Output_File analysisParameters.conf

Where:
- `lidarAnalysis_PDLx`: Software module to generate the data level ***x***.
- `/Input_File_or_Folder/`: Input folder or file to produce the data Level ***x***. In the case of L0, this first parameter is the folder with the raw lidar data files in Licel or Raymetric data file format. For the rest of the data levels, the input is the NetCDF file produced in the previous stage.
- `/Output_File`: Output NetCDF filename. The output file contains the information of the input folder/file, adding the information of the new data of the level under analysis.
- `analysisParameters.conf`: Configuration file with all the variables needed for the level analysis.

In order to avoid mistakes, it is preferable to use absolute paths for all the arguments passed to the modules.

An important feature of the output files produced in the stages 1 and 2 is that these outputs contains all the information of its previous stage. The new file generated add the new information of the stage under analysis in a NetCDF's sub-group called **L*x*_Data**, being ***x*** the data level number. This can be seen in the next figure, showing the data added in L1 stage:

![Panoply subgroup](./Docs/Figures/sub_group_nc.png "NetCDF Subgroup")

In next sections, a step-to-step on how to install and run these modules are described.

## Installation:
Download the latest version from `www.github.com/juanpallotta/LPP`. You will find 7 folders and 2 files:
- `/libLidar`: C/C++ lidar libraries source code.
- `/Lidar_Analysis_L0`: C/C++ sources code of `lidarAnalysis_PDL0` to produce level 0 data products.
- `/Lidar_Analysis_L1`: C/C++ sources code of `lidarAnalysis_PDL1` to produce level 1 data products.
- `/Lidar_Analysis_L2`: C/C++ sources code of `lidarAnalysis_PDL2` to produce level 2 data products.
- `/Lidar_Configuration_Files`: Contain the configuration files (`.conf`) of each module.
- `/signalsTest`: Lidar test files to test this code. You will find files from Argentina (pure Licel datatype files) and Brazil (Raymetric datatype files).
- `install_Lidar_Dependencies.sh`: a Linux shell-script to install the basic software/libraries needed to compile and run the LPP software.
- `/compile_All.sh`: Linux shell script to compile all the modules.
- `/run_LPP_Analysis`: Linux shell script to run the whole chain automatically, following the rules saved in the configuration files `/Lidar_Configuration_Files/LPP_Settings.sh`. More about the automatization of all modules in section [Automatizing LPP](#Automatizing_LPP) of this README file.
- `README.md`: This file.

## Installing dependencies:
There are and very basics things to be installed prior to the run. To solve this problem, run the Linux shell script named: `install_Lidar_Dependencies.sh`.
It is a simple Linux shell script to install the basics packages (make, g++, and NetCDF libraries). You will be asked for administrator credentials. Before running it, remember to check if the executable attributes are set. If not the case, make it executable with the command:

   chmod +x install_Lidar_Dependencies.sh

This advisor is also applicable to any other script in the project.

## Building:
To compile all the modules, just run the Linux shell script named: `compile_All.sh`. This is a simple Linux script which enter the folders of each module and run the make command passing the makefile as argument.
At the moment, the compiler output will show some warnings. All of them will be cleared up in the future versions.


_____________________________________________________________________________


## Configuring and running LPP modules:
The behavior of each module is based on how is set the configuration file passed as the third argument. This file must have the variables needed for the module, following few rules.
In this section, how to configure each module will be described. This is done by setting the variables in the configuration file (extension `.conf`, located in the folder `/Lidar_Configuration_Files`). Then, this file should be passed as a third argument to the module.

<u>Configuration file:</u>
In these files, all the parameters needed for the analysis must be set, and only 2 main ideas have to be taken into account:
1. Comments are defined by "`#`" character.
2. Variables definition has to follow the convention `VAR_NAME = VALUE`, and a minimum of 1 space character has to be placed before and after the `=` character. The variables can be integers, float, double, or string data format.
3. Some variables have to be set as vectors, separating each element be the character "`:`", for instance: `VAR_NAME = VALUE1 : VALUE2 : VALUE3`. The number of elements depends on the variable, and how LPP has it implemented. In order to minimize the mistakes related to this, read the comments in the previous lines of the variable definition. In case that the numbers of elements doesn't meet the right values, LPP will show a warning and exit the execution.
4. The configuration file could be the same for all the modules, as far it contains the variables needed for the run.



### `lidarAnalysis_PDL0`: Converting raw lidar files in a single NetCDF file
This module is used to merge the raw lidars files located in a folder (passed as first argument), into a single NetCDF file (passed as a second argument). The configuration file is passed as third argument.

An example of how to run this module can be:

    ./lidarAnalysis_PDL0 ../signalTest/Brazil/SPU/20210730/ ../signalTest/Brazil/SPU/20210730/LPP_OUT/20210730_L0.nc ../Lidar_Configuration_Files/analysisParameters_PDL0_Brazil.conf

Where:
- `lidarAnalysis_PDL0`: Executable file of the L0 module.
- `../signalTest/Brazil/SPU/20210730/`: Input folder with the raw-lidar files in Licel or Raymetric data format.
- `../signalTest/Brazil/SPU/20210730/LPP_OUT/20210730_L0.nc`: Output path, **INCLUDING THE OUTPUT FILE NAME** of the output NetCDF file.
- `../Lidar_Configuration_Files/analysisParameters_PDL0_Brazil.conf`: Configuration file with the variables needed for the merging.

An example of how L0 configuration file should look is seen in the next code:

```bash
####################################################
# CONFIGURATION FILE FOR LIDAR CONVERSION TOOL 
# RAW LIDAR DATA FILES TO NETCDF FILE - L0
####################################################

# DATE-TIME RANGE TO ANALYZE INSIDE THE FOLDER PASSED AS ARGUMENT TO lidar_Analysis_pDL0
# IF minTime = maxTime --> ALL FILES INSIDE THE FOLDER WILL BE ANALYZED
minTime = 2021/07/30-00:00:00
maxTime = 2021/07/30-00:00:00

# INPUT DATAFILE FORMAT
# inputDataFileFormat = LICEL_FILE
inputDataFileFormat = RAYMETRIC_FILE
# inputDataFileFormat = LALINET_NETCDF -- NOT IMPLEMENTET YET
# inputDataFileFormat = SCC_NETCDF -- NOT IMPLEMENTET YET
# inputDataFileFormat = CSV -- NOT IMPLEMENTET YET

# OUTPUT NETCDF DATAFILE FORMAT --> outDataFileFormat
outputDataFileFormat = LALINET_NETCDF
# outputDataFileFormat = SCC_NETCDF

```

At the moment, configuration files for L0 retrieval only need a few of basic data. The first one set are related to the time bin to analyze inside the folder (`minTime` and `maxTime`). In case to analyze all the files, these two variables have to be set equals. 

The other two variables are related to the data type format used in the input and output data files. The two options are Licel based files like: pure Licel data file format and Raymetric data file format. As can be seen, there are planned more input data types formats to convert. 

Related to the output data file format, two types can be set: LALINET or SCC output files. A description of LALINET data type and its name conventions, can be see in latter section in this document ([LALINET data type format](#LALINET_data_type_format)). The SCC data type is described in the web page of the project (https://docs.scc.imaa.cnr.it/en/latest/file_formats/netcdf_file.html). This module can be use to feed the SCC platform, but no for use it in the next modules of LPP.


In order to proceed without mistakes, it highly recommended uncomment the proper line in the configuration files included in this repository. Also, its worth mentioning that this inputs allows only one valid entry for each variable, so carefully check that only one line of each variable is uncommented.


### `lidarAnalysis_PDL1`: Producing data level 1 products: lidar signals corrections and cloud-mask

This module receive the NetCDF file produced by the previous module as a first parameter. This module will accept the input file while it is in the LALINET NetCDF format, this mean, the variable `outputDataFileFormat = LALINET_NETCDF` should be set in the configuration file of L0 module.

An example of how to run this module can be:

    ./lidarAnalysis_PDL1 ../signalTest/Brazil/SPU/20210730/LPP_OUT/20210730_L0.nc ../signalTest/Brazil/SPU/20210730/LPP_OUT/20210730_L0_L1.nc ../Lidar_Configuration_Files/analysisParameters_PDL1_2_Brazil.conf

The configuration file (in this case, `../Lidar_Configuration_Files/analysisParameters_PDL1_2_Brazil.conf`) could be the same file used in module L0, or use another one. The sample files included in this repository uses the same file for modules 

<!-- ### `lidarAnalysis_PDL2`. Producing data level 2 products: aerosol optical parameters

    ./lidarAnalysis_PDL2 ../signalTest/Brazil/SPU/20210730/LPP_OUT/20210730__L0_L1.nc ../signalTest/Brazil/SPU/20210730/LPP_OUT/20210730_L0_L1_L2.nc ../Lidar_Configuration_Files/analysisParameters_PDL1_2_Brazil.conf -->


## <a name="Automatizing_LPP"></a>Automatizing LPP

In order to run all the LPP modules automatically, this repository contains a Linux script to do this task. The name of this script is `run_LPP_Analysis.sh`, and you can find it at the root folder of this repository. It uses a general configuration file named `LPP_Settings.sh` located in the folder `/Lidar_Configuration_Files/`.

The main task of this script is to call every module, producing the inputs and outputs paths automatically. The convention's names of these strings are explained in this section. This script will analyze all the sub-folders starting from the main folders configured in the setting file `LPP_Settings.sh`, and generate the names of the input and output NetCDF files automatically. An example of the configuration file is show below:

```bash {.line-numbers}
#!/bin/bash

# DATA LEVEL TO PRODUCE IN THE RUN. 
L0="yes"
L1="yes"
L2="no"

# ABSOLUTE INPUT PATH
PATH_IN="/mnt/Disk-1_8TB/Brazil/SPU/20210730/"

FILE_CONF_L0="../Lidar_Configuration_Files/analysisParameters_PDL0_Brazil.conf"
FILE_CONF_L1_L2="../Lidar_Configuration_Files/analysisParameters_PDL1_2_Brazil.conf"

```

As can be seen, a few lines are needed to run the automatic mode. These are:
* `L0`, `L1` and `L2`: Data level to process. By setting `"yes"` or `"no"` at the variables `Lx`, the run of the module can be controlled.
* `PATH_IN`: Main path of the raw lidar data, in wich only lidar data file in the accepted data file formats must be stored. This is the starting point for searching for raw lidar data. A new folder named *LPP_OUT* will be created automatically once the script has searched to the last subfolder with raw lidar files. The `LPP_OUT` folder will be used to store the NetCDF files produced by LPP of the files stored at the mother folder of `LPP_OUT`.
Because there are different ways to store the data files and their folder structures, two examples will be shown and how the folder/files are generated automatically. 

First one is like Sao Paulo lidars, where the lidar files produced in a day are stored in a single folder with the date in its name. In the next figure, an example is shown:

![SPU_files](./Docs/Figures/SPU_lidar_files.png "SPU lidar files")

In the example of the last figure, the folder `LPP_OUT` can be seen were the output files of LPP. They can be seen in the next figure:

![SPU_output_folder_files](./Docs/Figures/SPU_files_LPP_folder.png "LPP output folder")


In this last figure, the name convention automatically generated by the script `run_LPP_Analysis.sh` can be seen. For this case, only `L0` and `L1` were generated, and the names of each file contain the name of the upper folder of `LPP_OUT`.






## <a name="LALINET_data_type_format"></a> LALINET Data Type Format
