
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

<u>Configuration file data format:</u>
In these files, all the parameters needed for the analysis must be set, and only 2 main ideas have to be taken into account:
1. Comments are defined by "`#`" character.
2. Variables definition has to follow the convention `VAR_NAME = VALUE`, and a minimum of 1 space character has to be placed before and after the `=` character. The variables can be integers, float, double, or string data format.
3. Some variables have to be set as vectors, separating each element be the character "`:`", for instance: `VAR_NAME = VALUE1 : VALUE2 : VALUE3`. The number of elements depends on the variable, and how LPP has it implemented. In order to minimize the mistakes related to this, read the comments in the previous lines of the variable definition. In case that the numbers of elements doesn't meet the right values, LPP will show a warning and exit the execution.
4. The configuration file could be the same for all the modules, as far it contains the variables needed for the run.



### `lidarAnalysis_PDL0`: Converting raw lidar files in a single NetCDF file:
This module is used to merge the raw lidars files located in a folder (passed as first argument), into a single NetCDF file (passed as a second argument). The configuration file is passed as third argument.

An example of how to run this module can be:

    ./lidarAnalysis_PDL0 ../signalTest/Brazil/SPU/20210730/ ../signalTest/Brazil/SPU/20210730/LPP_OUT/20210730_L0.nc analysisParameters_PDL0_Brazil.conf

Where:
- `lidarAnalysis_PDL0`: Executable file of the L0 module.
- `../signalTest/Brazil/SPU/20210730/`: Input folder with the raw-lidar files in Licel or Raymetric data format.
- `../signalTest/Brazil/SPU/20210730/LPP_OUT/20210730_L0.nc`: Output path, **INCLUDING THE OUTPUT FILE NAME** of the output NetCDF file.
- `analysisParameters_PDL0_Brazil.conf`: Configuration file with the variables needed for the merging.

An example of how L0 configuration file should look is seen in the next code:

```bash {.line-numbers}
####################################################
# CONFIGURATION FILE FOR LIDAR CONVERSION TOOL
####################################################
# DATE-TIME RANGE TO ANALYZE INSIDE THE FOLDER PASSED AS ARGUMENT TO mergeLidarFiles
minTime = 2011/07/14-00:00:00
maxTime = 2011/07/14-03:00:00

# INPUT DATAFILE FORMAT
# inputDataFileFormat = LICEL_FILE
inputDataFileFormat = RAYMETRIC_FILE
# inputDataFileFormat = LALINET_NETCDF -- NOT IMPLEMENTET YET
# inputDataFileFormat = SCC_NETCDF -- NOT IMPLEMENTET YET
# inputDataFileFormat = CSV -- NOT IMPLEMENTET YET

# OUTPUT NETCDF DATAFILE FORMAT --> outDataFileFormat
outputDataFileFormat = LALINET_NETCDF
# outputDataFileFormat = SCC_NETCDF

# NUMBER OF FILES (EVENTS) THAT WILL BE MERGED (AVERAGED) INTO A SINGLE LIDAR SIGNAL
numEventsToAvg = 10

# MOLECULAR REFERENCES
Molecular_Calc = 0
Temperature_at_Lidar_Station = 25.0
Pressure_at_Lidar_Station = 940.0

# ATTRIBUTES (UNDER DEVELOPMENT...)
# ATT_global_TEXT = 
# ATT_global_DOUBLE = 
# ATT_global_INT = 

# ATT_Raw_Lidar_Data_TEXT =
# ATT_Raw_Lidar_Data_DOUBLE =
# ATT_Raw_Lidar_Data_INT =

Time range of analysis:
minTime = 2011/07/14-00:00:00
maxTime = 2011/07/14-03:00:00
```

The code will search inside the input folder passed as the first argument (PATH_IN_RAW in the runMergeLidarFiles.sh script) for files inside this time limit. All files inside these limits will be merged in the NetCDF output file.

Input datafile format:
   # INPUT DATAFILE FORMAT --> inputDataFileFormat
   # inputDataFileFormat = LICEL_FILE
   inputDataFileFormat = RAYMETRIC_FILE
   # inputDataFileFormat = LALINET_NETCDF -- NOT IMPLEMENTET YET
   # inputDataFileFormat = SCC_NETCDF -- NOT IMPLEMENTET YET
   # inputDataFileFormat = CSV -- NOT IMPLEMENTET YET

Uncomment the proper line in order to the right data file format to analyze. In this version, only LICEL_FILE and RAYMETRIC_FILE are accepted.

   Output datafile format:
   # OUTPUT NETCDF DATAFILE FORMAT --> outDataFileFormat
   outputDataFileFormat = LALINET_NETCDF
   # outputDataFileFormat = SCC_NETCDF


Uncomment the proper line in order to the right data file format to analyze. In this version, only LALINET_NETCDF and SCC_NETCDF are accepted.
Files to merge (average )
   # NUMBER OF FILES (EVENTS) THAT WILL BE MERGED (AVERAGED) INTO A SINGLE LIDAR SIGNAL
   numEventsToAvg = 10

In case you want to average a set of lidar files, it can be done in this step, by setting numEventsToAvg to the preferred number. In case of not wanting to average lidar files, just set this value to 1.

   Molecular reference:
   # MOLECULAR REFERENCES
   Molecular_Calc = 0
   Temperature_at_Lidar_Station = 25.0
   Pressure_at_Lidar_Station = 940.0

Till this version, these are the variables that need to be stored in the SCC-NetCDF output file.



### `lidarAnalysis_PDL1`. Producing data level 1 products: lidar signals corrections and cloud-mask

   ./lidarAnalysis_PDL1 ../signalTest/Brazil/SPU/20210730/LPP_OUT/20210730_L0.nc ../signalTest/Brazil/SPU/20210730/LPP_OUT/20210730_L0_L1.nc analysisParameters_PDL1_2_Brazil.conf


### `lidarAnalysis_PDL2`. Producing data level 2 products: aerosol optical parameters

   ./lidarAnalysis_PDL2 ../signalTest/Brazil/SPU/20210730/LPP_OUT/20210730__L0_L1.nc ../signalTest/Brazil/SPU/20210730/LPP_OUT/20210730_L0_L1_L2.nc analysisParameters_PDL1_2_Brazil.conf


## <a name="Automatizing_LPP"></a>Automatizing LPP


In order to run this code in a more convenient way, there is a Linux script to configure and run it. The name of this script is runMergeLidarFiles.sh, and its shelf-explanatory. Here I copy an example of this script:




