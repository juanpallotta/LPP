# Lidar Processing Pipeline (LPP)

The LPP is a collection of tools developed in C/C++ and Linux script, planned to handle all the steps of lidar analysis. In the present version of LPP, only elastic lidar signal are analyzed, but it is planned to manage Raman signals in the near future. 

LPP is made by 3 main tools. The first tool converts the raw data files into a single NetCDF file, including detailed information about the instrument and acquisition setup (level 0 dataset). The produced NetCDF files are then processed by another tool that applies the necessary corrections to the lidars signals and computes the cloud-mask (level 1 dataset). The final step is the elastic retrieval of aerosol properties (level 2 dataset), and for now, only elastic lidar signals are processed. The development of LPP is based on the existing analysis routines developed by individual LALINET groups, and hence takes advantage of previous efforts for algorithm comparison within the scope of the LALINET network.
The code presented in this repository was tested on Linux Ubuntu 20.04.3 LTS, and Windows 10 using WSL.

# Overall concept of the LPP tools

The Lidar Processing Pipeline (LPP) is formed by 3 completely independent software (or modules), which communicates with each other using NetCDF files. The names of each one are represented by the output of its product data level (PDL), named from 0 to 2. Each one of these modules can be executed in a Linux command line following the basic rules described in this document. These modules are:
- `lidarAnalysis_PDL0`: Transforms all the raw lidar data files stored in a folder (passed as an argument) into a single NetCDF file. This output file will contain the raw lidar signals and global information about the measurement. This output is the L0 data level of LPP.
- `lidarAnalysis_PDL1`: Receive the NetCDF file produced by `lidarAnalysis_PDL0` and produce a new NetCDF defined as data level 1 (L1). This L1 file contains the same information as the data level L0 and adds L1 information. These new data include: corrected lidar files (like laser offset, bias correction, etc.), the cloud-mask product, and molecular profiles for every wavelength used. Also, all the parameters used to produce this output are stored. This output is called L1 data level of LPP.
- `lidarAnalysis_PDL2`: Receive the NetCDF file produced by `lidarAnalysis_PDL1` and produce a new NetCDF file defined as data level 2 (L2). This L2 file contains the same data as L0 and L1 and adds the optical products obtained from the elastic lidar signals. Also, all the parameters used to produce this output are stored. This output is the L2 data level of LPP.

It is important to remark the feature that the output files produced in stages 1 and 2 contain all the information of its previous stage. The new file generated adds the new information of the stage under analysis in a NetCDF's subgroup called **L*x*_Data**, being ***x*** the data level number. This can be seen in the next figure, showing the data added in L1 stage:

![Panoply subgroup](./Docs/Figures/sub_group_nc.png "NetCDF Subgroup")

Each lidar analysis tool must be run in a Linux terminal, following the convention:

<a name="run_module"></a>
```
$./lidarAnalysis_PDLx /Input_File_or_Folder/ /Output_File analysisParameters.conf
```

Where:
- `lidarAnalysis_PDLx`: Software module to generate the data level ***x***.
- `/Input_File_or_Folder/`: Input folder or file to produce the data Level ***x***. In the case of L0, this first parameter is the folder with the raw lidar data files in Licel or Raymetric data file format. For the rest of the data levels, the input is the NetCDF file produced in the previous stage.
- `/Output_File`: Output NetCDF filename. The output file contains the information of the input folder/file, adding the information of the new data of the level under analysis.
- `analysisParameters.conf`: Configuration file with all the variables needed for the level analysis.

To avoid possible mistakes, it is preferable to use absolute paths for all the file's arguments passed to the modules.

In the next sections, a step-by-step on how to install and run these modules are described.

# Installation:
Download or clone the repository from GitHub: `www.github.com/juanpallotta/LPP`. You will find:
- `/libLidar`: C/C++ lidar libraries source code.
- `/Lidar_Analysis_L0`: C/C++ sources code of `lidarAnalysis_PDL0` to produce level 0 (L0) data products.
- `/Lidar_Analysis_L1`: C/C++ sources code of `lidarAnalysis_PDL1` to produce level 1 (L1) data products.
- `/Lidar_Analysis_L2`: C/C++ sources code of `lidarAnalysis_PDL2` to produce level 2 (L2) data products.
- `/Lidar_Configuration_Files`: Contain the configuration files (`.conf`) for each module. Also, the settings file for an automatic run (see later [section](#Automatizing_LPP) about the LPP automation).
- `/signalsTest`: Lidar test files to test this code. You will find files from Buenos Aires, Argentina (pure Licel datatype files) and Brazil: Sao Paulo (folder `Brazil/SPU/`, Licel data type files) and Manaus (folder `Brazil/Manaus`, Raymetric datatype files).
- `install_Lidar_Dependencies.sh`: Linux shell-script to install the basic software/libraries needed to compile and run the LPP software.
- `/compile_All.sh`: Linux shell script to compile all the modules.
- `/run_LPP_Analysis.sh`: Linux shell script to run the whole chain automatically. The main settings for automatic run are configured in its setting file (`/Lidar_Configuration_Files/LPP_Settings.sh`). More about the automatization of all modules in [Automatizing LPP](#Automatizing_LPP) section of this README file.
- `README.md`: This file.

# Setting up the code
## Installing dependencies:
There are a few pre-requisites to be installed prior to the run of LPP. To do this job, just run the Linux shell script named `install_Lidar_Dependencies.sh`.
It is a simple Linux shell script to install the basics packages (make, g++, and NetCDF libraries). You will be asked for administrator credentials.
Remember to set `install_Lidar_Dependencies.sh` with executable attributes: `chmod +x install_Lidar_Dependencies.sh`.

## Building the code:
To compile all the modules, just run the Linux shell script named `compile_All.sh`. This is a simple Linux script that produces the executables of each module inside their folders. Remember to set `compile_All.sh` with executable attributes prior run it: `chmod +x compile_All.sh`.
At the moment, the compiler output will show some warnings. All of them will be solved in future versions.

## Configuring and running LPP modules:
The behavior of each module is based on the parameters written in its configuration file, passed as the [third argument](#run_module). They are stored in the `/Lidar_Configuration_Files` folder of this repository. These are text-based files and have the variables needed for the module, having to follow only 4 main rules:
1. Comments are defined by "`#`" character. You are free to comment on anything to make the run more understandable for you. The configuration files included in this repository have many comments to explain each variable.
2. Variables definition has to follow the convention `VAR_NAME = VALUE`, and a <u>**minimum of 1 space character has to be placed before and after the "`=`" character**</u>. The variables data type can be integer, float, double, or string. Also, in some cases, a variable can be an array of values, in this case, a comment in the line before warn about this.
3. Some variables have to be set as vectors. Each element must be separated by the character "`:`", for instance: `VAR_NAME = VALUE1 : VALUE2 : VALUE3` <u>**and a minimum of 1 space character has to be placed before and after the "`:`" character**</u>. The number of elements depends on the variable, and how LPP has it implemented. In order to minimize the mistakes related to this, please, read the comments in the lines before the variable definition. In case that the number of elements doesn't meet the right values, LPP will show a warning and exit the execution.
4. The configuration file could be the same for all the modules, or use one file for all of them, as far as it contains the variables needed for the run. These variables are described in this document.

The following sections describe each module, how to configure it, and how to run it. We highly encourage you to run the examples shown and play with its variables to feel comfortable with the uses of the modules. Then, use your own input files.

# <a name="configuring_PDL0"></a> `lidarAnalysis_PDL0`: Converting raw lidar files in a single NetCDF file
This module is used to merge the raw lidar files located in a folder (passed as first argument), into a single NetCDF file (path and filename passed as a second argument). The configuration file is passed as the third argument, and it's going to be described in this section.

An example of how to run this module using the sample signals included in this repository is shown below:

    ./lidarAnalysis_PDL0 
    /mnt/Disk-1_8TB/Brazil/SPU/20210730/ 
    /mnt/Disk-1_8TB/Brazil/SPU/20210730/LPP_OUT/20210730_L0.nc 
    /home/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_PDL0_Brazil.conf

Where:
- `lidarAnalysis_PDL0`: Executable file of the L0 module.
- `/mnt/Disk-1_8TB/Brazil/SPU/20210730/`: Absolute input folder path with the raw-lidar files in Licel or Raymetric data format. Since Licel files have no defined extension, <u>**it is critical that nothing but raw lidar data files must be in this input folder**</u>. 
- `/mnt/Disk-1_8TB/Brazil/SPU/20210730/LPP_OUT/20210730_L0.nc`: Absolute output path, <u>**including the file name**</u> of the output NetCDF file, in this example `20210730_L0.nc`. If the output file needs to be placed in a subfolder, generate it manually before running (in this example `/LPP_OUT/`).
- `/home/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_PDL0_Brazil.conf`: Absolute path to the configuration file containing the variables needed for the merging.

It is highly recommentable to use absolute paths to avoid errors in the execution.

In this first LPP version, L0 configuration file looks like the next code:

```bash
#############################################################
# CONFIGURATION FILE FOR LIDAR CONVERSION TOOL 
# RAW LIDAR DATA FILES TO NETCDF FILE - DATA LEVEL 0 (L0)
#############################################################

# INPUT DATAFILE FORMAT
inputDataFileFormat = LICEL_FILE
# inputDataFileFormat = RAYMETRIC_FILE

# OUTPUT NETCDF DATAFILE FORMAT --> outDataFileFormat
outputDataFileFormat = LALINET_NETCDF
# outputDataFileFormat = SCC_NETCDF
```

L0 data levels configuration file only need a few of basic inputs:
* `inputDataFileFormat`: At the moment, only Licel (`LICEL_FILE` option) or Raymetric (`RAYMETRIC_FILE` option) data type file are accepted. There is planned to accept more input data types formats in the future. 
* `outputDataFileFormat`: The output data types accepted are: LALINET (`LALINET_NETCDF`) and Single Calculus Chain (`SCC_NETCDF`) data type files. If `SCC_NETCDF` is selected, higher modules of LPP (`lidarAnalysis_PDL1` and `lidarAnalysis_PDL2`) can not be executed due to the different names conventions of the variables inside the file. A detailed description of LALINET data type can be seen in later sections of this document ([LALINET data type format](#LALINET_data_type_format)).

If `SCC_NETCDF` is selected, the output file generated can be used as an input for the SCC platform (https://www.earlinet.org/index.php?id=281). More info about SCC data file format and its name convention is described in the web page of the project (https://docs.scc.imaa.cnr.it/en/latest/file_formats/netcdf_file.html). 

In order to proceed without making mistakes, we highly suggest to uncomment the proper line in the configuration files included in this repository. Also, its worth mentioning that this inputs allows only one valid entry for each variable, so check carefully if only one line of each variable is enabled (uncommented).


# <a name="configuring_PDL1"></a> `lidarAnalysis_PDL1`: Producing data level 1 products: lidar signals corrections and cloud-mask

This module receives the NetCDF file produced by the previous module (`lidarAnalysis_PDL0`) as it first parameter (in our example, `/mnt/Disk-1_8TB/Brazil/SPU/20210730/LPP_OUT/20210730_L0.nc`). This module will accept the input file while it is in the LALINET NetCDF format; this means, the variable `outputDataFileFormat = LALINET_NETCDF` should be set in the L0's module configuration file (`analysisParameters_PDL0_Brazil.conf` in this example).

An example of how to run this module can be:

    ./lidarAnalysis_PDL1 
    /mnt/Disk-1_8TB/Brazil/SPU/20210730/LPP_OUT/20210730_L0.nc 
    /mnt/Disk-1_8TB/Brazil/SPU/20210730/LPP_OUT/20210730_L0_L1.nc 
    /home/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_PDL1_2_Brazil.conf

The configuration file (in this case, `/home/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_PDL1_2_Brazil.conf`) could be the same file used in module L0, or use another one. The only thing that matters are the defined variables inside the file passed as a third argument.

The sample files included in this repository use one configuration file for `lidarAnalysis_PDL0`, and another for the modules `lidarAnalysis_PDL1` and `lidarAnalysis_PDL2`.

An example of the variables contained in a configuration file for L1 module is:

```bash

##########################################################################################
# PARAMETERS FOR lidarAnalysis_PDL1
##########################################################################################

# LASER-BIN-OFFSET OR TRIGGER-DELAY OR ZERO-BIN
# ARRAY: ONE PER CHANNEL
indxOffset = 2 : 3 : 7 : 5 : 8 : 2 : 9 : 2 : 2 : 2 : 2 : 2 

# NUMBER OF FILES (EVENTS) THAT WILL BE AVERAGED INTO A SINGLE LIDAR SIGNAL
numEventsToAvg_PDL1 = 10

# WAVELENGHT USED FOR PRODUCT DATA LEVEL 1 (PDL1). INDEXES STARTING FROM 0
# SP
indxWL_PDL1 = 2

# MOLECULAR DATA
# ABSOLUTE PATH TO THE RADIOSOUNDING/MODEL FILE. THIS FILE MUST BE FORMATTED:
# - COMMA SEPARATED
# - RANGE UNIT: METERS
# - TEMPERATURE UNIT: KELVIN
# - PRESSURE UNIT: HECTO PASCALS
Path_To_Molecular_File = ./US-StdA_DB_CEILAP.csv
# SETTINGS OF THE COLUMN INDEX (0-2) OF THE PARAMETERS: RANGE-TEMPERATURE-PRESSURE IN THE FILE:
Range_column_index_in_File = 0
Temp_column_index_in_File  = 1
Pres_column_index_in_File  = 2

Temperature_at_Lidar_Station = 25.0
Pressure_at_Lidar_Station = 940.0

# INITIAL RANGE OF ANALYSIS: rInitSig
# END RANGE OF ANALYSIS: rEndSig
rInitSig = 400
rEndSig = 25000

# NUMBER OF BINS USED FOR BACKGROUND CALCULATION (TAKEN FROM THE TAIL OF THE LIDAR SIGNAL)
nBinsBkg = 1000

# CLOUD-MASK RETRIEVAL PARAMETERS
AVG_CLOUD_DETECTION = 101
stepScanCloud = 1
nScanMax = 5000
errFactor = 2.0
thresholdFactor = 5.0
CLOUD_MIN_THICK = 5

errScanCheckFactor = 1.0
errCloudCheckFactor = 0.0
DELTA_RANGE_LIM_BINS = 100

```

Below is a description of each of these parameters:

* `indxOffset`: The number of bins to remove from the beginning of the lidar track recorded due to the laser offset. This parameter should be an array with one element for each channel acquired.

* `numEventsToAvg_PDL1`: Time averaging for L1 data level products. This parameters tell to `lidarAnalysis_PDL1` the numbers of lidar profiles to average producing one merged profile. After this, the `time` dimension in the NetCDF file for the L1 data products will be reduced by `numEventsToAvg_PDL1` times.

* `indxWL_PDL1`: An index (starting from 0) of the channel to use in cloud-mask production. It is recommended to use an elastic lidar channel and the highest wavelength in the file for better cloud discrimination.

* `Path_To_Molecular_File`: Path to the plain-text, comma-sepparated file containing the radiosonde/model data. This file must contain the altitude, temperature and pressure of the radiosonde/model used for the computation of the alpha and beta molecular profiles. The units of these variables are described in the comments of the setting file: altitude in m, temperature in K and pressure in hPa. This repository has a sample file of US standard model (file `/LPP/Lidar_Analysis_PDL1/MolData/US-StdA_DB_CEILAP.csv`). The radiosonde/model file must be headerless, and the column order of each parameter (height, temperature and pressure) are described in the following variables (`Range_column_index_in_File`, `Temp_column_index_in_File` and `Pres_column_index_in_File`).
* `Range_column_index_in_File`: Column index of the radiosonde/model file containing the range values. For this example, index 0 (first column).
* `Temp_column_index_in_File`: Column index of the radiosonde/model file containing temperature values. For this example, index 1 (second column).
* `Pres_column_index_in_File`: Column index of the radiosonde/model file containing the pressure values. For this example, index 2 (third column).
  
The first lines of the `US-StdA_DB_CEILAP.csv` file contained in this repository are shown in the next lines (data contained in columns indexes 3 to 7 are not taken into account):

```
0,288.15,1013.27,0,0,1033.81,1.2247
200,286.85,989.48,0,0,1009.26,1.2013
400,285.55,966.13,0,0,985.22,1.1783
600,284.25,943.24,0,0,961.68,1.1557
800,282.95,920.78,0,0,938.62,1.1333
1000,281.65,898.77,0,0,916.05,1.1113
...
```
* `Temperature_at_Lidar_Station`: Temperature at ground level (in Celsius).
* `Pressure_at_Lidar_Station`: Pressure at ground level (in hPa)

* `rInitSig`: Initial range of the analysis (in meters from the lidar line of sight). It is preferable to set this value to the first point where the full overlap is achieved.
* `rEndSig`: End range of analysis (in meters from the lidar line of sight). It is preferable to set this value to the last processable point in the acquired signal.

* `nBinsBkg`: Number of bins used for background removal. 

* **Cloud-mask retrieval parameters:** These parameters are required for the cloud detection algorithm. We strongly recommend using the values set in the files included in this repository. The algorithm used is robust enough to work with a wide range of elastic-lidar signals using this setup.

```
AVG_CLOUD_DETECTION = 101
stepScanCloud = 1
nScanMax = 5000
errFactor = 2.0
thresholdFactor = 5.0
CLOUD_MIN_THICK = 5

```

# <a name="configuring_PDL2"></a> `lidarAnalysis_PDL2`. Producing data level 2 products: aerosol optical parameters
This module receives the NetCDF file produced by the previous module (`lidarAnalysis_PDL1`) as a first parameter (in our example, `/mnt/Disk-1_8TB/Brazil/SPU/20210730/LPP_OUT/20210730_L0_L1.nc`). This module will accept the input file while it is in the LALINET NetCDF format; this means, the variable `outputDataFileFormat = LALINET_NETCDF` should be set in the configuration file of L0 module.

An example of how to run this module can be:

    ./lidarAnalysis_PDL2
    /mnt/Disk-1_8TB/Brazil/SPU/20210730/LPP_OUT/20210730_L0_L1.n
    /mnt/Disk-1_8TB/Brazil/SPU/20210730/LPP_OUT/20210730_L0_L1_L2.nc 
    /home/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_PDL1_2_Brazil.conf

The configuration file (in this case, `/home/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_PDL1_2_Brazil.conf`) is the same file used in module L1 and also contains the variables needed for the optical products retrieval (L2).

An example of a configuration file and its variables for L2 data-level retrieval could be:

```bash

##########################################################################################
# PARAMETERS FOR lidarAnalysis_PDL2
##########################################################################################

# NUMBER OF FILES (EVENTS) THAT WILL BE MERGED (AVERAGED) INTO A SINGLE LIDAR SIGNAL
numEventsToAvg_PDL2 = 18

# NUMBER OF POINTS USED FOR A SMOOTHING SLIDING WINDOWS TO APPLY TO THE RCLS BEFORE FERNALD'S INVERSION
avg_Points_Fernald = 51

# FERNALD INVERSION PARAMETER
# LR: COULD BE AN ARRAY OR A SINGLE VALUE
LR = 50 : 60 : 70 : 80
# INDEX OF THE CHANNEL TO BE INVERTED (INDEXED FROM 0)
indxWL_PDL2 = 0

# REFERENCE HEIGHT FOR LIDAR SIGNAL (ABOVE SEA LEVEL)
heightRef_Inversion_ASL = 10000
# HALF NUMBER OF POINTS (BINS) TO AVERAGE AROUND THE REFERENCE HEIGHT DURING THE NORMALIZATION OF THE LIDAR SIGNAL.
avg_Half_Points_Fernald_Ref = 15

```

A description of each of these parameters is described below:

* `numEventsToAvg_PDL2`: Number of lidar signals to average. It is the equivalent to time averaging but using the numbers of adjacents profiles.
* `avg_Points_Fernald`: Numbers of points of the moving average filter used for the spatial smoothing of the signal.
* `LR`: Lidar ratio used for the inversion. It can be more than one value, in wich each elements must be sepparated by `:`, with a space before and after `:` (see sample code in the previous lines: `LR = 50 : 60 : 70 : 80`).
* `indxWL_PDL2`: Index of the channel used for the inversion (starting at 0). This first version, only one channel can be accepted as input, and the aerosol optical profiles of extinction and backscattering will have dimensions of `time`, `LRs` and `points`.
* `heightRef_Inversion_ASL`: Altitude above sea level used as reference height used to normalize the lidar signal.
* `avg_Half_Points_Fernald_Ref`: In order to obtain the value of the lidar signal to normalize it, an average of +-`avg_Half_Points_Fernald_Ref` points around the reference heigh are used.


# <a name="Automatizing_LPP"></a>Automatizing LPP

In order to run all the LPP modules automatically, this repository contains a Linux script to do this task. The name of this script is `run_LPP_Analysis.sh`, and you can find it in the root folder of this repository. It uses a general configuration file `LPP_Settings.sh` located in the configuration folder `Lidar_Configuration_Files/`.

With this script, the outputs and input paths and file names of every module are automatically generated and passed as arguments for every `lidarAnalysis_PDLx` module. The rules used in the automatic filename generation are explained in this section.

In the configuration file (`Lidar_Configuration_Files/LPP_Settings.sh`), the outputs data set levels and the input and output paths are set. An example of this file can be seen below:

```bash
#!/bin/bash

# DATA LEVEL TO PRODUCE IN THE RUN. 
L0="yes"
L1="yes"
L2="no"

# ABSOLUTE INPUT PATH
PATH_IN="/mnt/Disk-1_8TB//Brazil/SPU/20210730/"

FILE_CONF_L0="/home/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_PDL0_Brazil.conf"
FILE_CONF_L1_L2="/home/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_PDL1_2_Brazil.conf"

```

As can be seen, a few lines are needed to run the automatic mode. These are:
* `L0`, `L1` and `L2`: Data level to process. By setting `"yes"` or `"no"` at the variables `Lx`, you can control the run of each module.
* `PATH_IN/`: Main path of the raw lidar data, in wich only lidar data files (Licel or Raymetric data format) must be stored. This is the starting folder for searching raw lidar files. A folder named `/LPP_OUT/` will be created automatically in the last subfolder found with raw lidar files. The `/LPP_OUT/` folder will be used to store the NetCDF output files produced by each LPP module.
Because there are different ways to store the data files and their folder structures, two examples will be shown and how the folder/files are generated automatically. 
* `FILE_CONF_L0` and `FILE_CONF_L0_L1`: Path to the configuration file of each module.

In order to explain how paths are generated automatically, two examples are shown. The first one is used in Sao Paulo and Argentinean lidars, where the lidar files produced in a day are stored in a single folder with the full date in its name. In the next figure, an example is shown:

![SPU_files](./Docs/Figures/SPU_lidar_files.png "SPU lidar files")

It can be seen that inside a single folder you can find all the files for that day. In this situation, the script `run_LPP_Analysis.sh` will create the folder `/LPP_OUT/` automatically and the output files of each modules will save the data in it. If we take a look inside the folder `/LPP_OUT` it can be seen the NetCDF files created by the modules `lidar_Analysis_PDL0` and `lidar_Analysis_PDL1`:

![SPU_output_folder_files](./Docs/Figures/SPU_files_LPP_folder.png "LPP output folder")

In the case of Manaus lidars, the structure of the folder for each measurement can be seen in the next figure:

![Manaus_output_folder_files](./Docs/Figures/Manaus_lidar_files.png "LPP output folder")

In this case, the files are stored in a folder with the number of the day, and the data of the month and year are in a higher order folder. In this scenario, the script `run_LPP_Analysis.sh` will create the `/LPP_OUT` folder in the last subfolder, in this case, in `/17/LPP_OUT`. Inside of it, the output files of each module are found:

![SPU_output_folder_files](./Docs/Figures/Manaus_files_LPP_folder.png "LPP output folder")


As can be seen, there are also rules for creating the NetCDF files name. `lidar_Analysis_PDL0` will create the L0 data level file using the name of the last subfolder with lidar files, adding `_L0.nc` at the end. The rest of the modules, will add `_L1` and `_L2` to the input filename to produce the output filename. It can be seen in the last figure, where `lidar_Analysis_PDL0` produces `17_L0.nc` and  `lidar_Analysis_PDL1` produce `17_L0_L1.nc`. 


## <a name="LALINET_data_type_format"></a> LALINET NetCDF's Data Type Format File

As was mentioned before, each LPP module has its own NetCDF file output produced by processing the information passed as the input argument (first parameter) according to the configuration file (third parameter). We describe here the status of the first version of LPP output files, but this will be upgraded as new capabilities are added.
The description of the dimensions, variables and global parameters are described in the next sections.

### NetCDF's File Produced for Data Level 0
This file contains the raw lidar data extracted from the input files, with general information contained in the header. 
<!-- As was explained in the section related to the product data level 0 ([PDL0](#configuring_PDL0)), extra information can be added to this file as global.--> 

#### Dimensions
In this version, 3 dimensions are defined:
```
time
channels
points
```
For the data level 0, the dimension `time` contains the number of lidar files located in the input folder passed as first argument to `lidarAnalysis_PDL0`.
`channels` dimension contains the number of channels contained in the lidar input files. <u>**Important Note:</u> all the lidar files stored in the input folder must have the same number of channels.** `lidarAnalysis_PDL0` considers that all the files contained in the input folder passed as first argument have the same hardware characteristics. If some changes in the hardware are made during a measurement, please, save them in another folder and analyze them in another run.
`points` dimension is the number of bins of all lidar tracks recorded.

#### Variables
The variables of the L0 are the data stored in the headers of the lidar files (in Licel/Raymetric datatype format).
The variables are listed below (in alphabetical order), with the dimensions used in parentheses:

* `Accumulated_Pulses(channels)`: Number of laser pulses accumulated in each channel.
* `ADC_Bits(channels)`: Number of bits for the ADC electronic.
* `Azimuth(time)`: .
* `DAQ_Range(channels)`:
* `Laser_Source(channels)`:
* `Number_Of_Bins(channels)`:
* `PMT_Voltage(channels)`:
* `Polarization(channels)`:
* `Raw_Data_Start_Time(time)`:
* `Raw_Data_Stop_Time(time)`:
* `Raw_Lidar_Data(time, channels, points)`:
* `Wavelengths(channels)`:
* `Zenith(time)`:

**Global attributes**

* `Site_Name`: String containing the lidar's site name.
* `Altitude_meter_asl`: Double data type containing the altitude of the lidar site.
* `Latitude_degrees_north`: Double data type with the latitude of the lidar site.
* `Longitude_degrees_east`: Double data type with the longitude of the lidar site.
* `Range_Resolution`: Double data type with the range resolution in meters.
* `Laser_Frec_1`: Double data type with the laser 1 repetition rate. 
* `Laser_Frec_2`: Double data type with the laser 2 repetition rate. 

### NetCDF's File Produced for Data Level 1

### NetCDF's File Produced for Data Level 2



