# Lidar Processing Pipeline (LPP)

The LPP is a collection of tools developed in C/C++ and Linux script, planned to handle all the steps of lidar analysis. In the present version of LPP, only elastic lidar signal are analyzed, but is planned to manage Raman signals in the near future. 

LPP is made by 3 main tools. A first tool converts the raw data files into a single NetCDF file, including detailed information about the instrument and acquisition setup (level 0 dataset). The produced NetCDF file are then processed by another tool that applies the necessary corrections to the lidars signals and computes the cloud-mask (level 1 dataset). The final step is the elastic retrieval of aerosol properties (level 2 dataset), and for now, only elastic lidar signals are processed. The development of LPP is based on the existing analysis routines developed by individual LALINET groups, and hence takes advantage of previous efforts for algorithm comparison in the scope of the LALINET network.
The codes presented in this repository are tested on Linux Ubuntu 20.04.3 LTS, and Windows 10 using WSL.

## Overall concept of the LPP tools

The Lidar Processing Pipeline (LPP) is formed by 3 completely independent software (or modules), which communicates each other using NetCDF files. The names of each one are represented by the output of its product data level (PDL), named from 0 to 2. Each one of these modules can be executed in a Linux command line following basic rules described in this document. These modules are:
- `lidarAnalysis_PDL0`: Transforms all the raw lidar data files stored in a folder (passed as an argument) into a single NetCDF file. This output file will contain the raw lidar signals and global information about the measurement. This output is the L0 data level of LPP.
- `lidarAnalysis_PDL1`: Receive the NetCDF file produced by `lidarAnalysis_PDL0` and produce a new NetCDF defined as data level 1 (L1). This L1 file contains the same information as the data level L0 and adds L1 information. These new data are: corrected lidar files (like laser offset, bias correction, etc.), the cloud-mask product, and molecular profiles for every wavelength used. Also, all the parameters used to produce this output are stored. This output is called L1 data level of LPP.
- `lidarAnalysis_PDL2`: Receive the NetCDF file produced by `lidarAnalysis_PDL1` and produce a new NetCDF file defined as data level 2 (L2). This L2 file contains the same data of L0 and L1 and adds the optical products obtained from the elastic lidar signals. Also, all the parameters used to produce this output are stored. This output is the L2 data level of LPP.

It is important to remark the feature that the output files produced in stages 1 and 2 contains all the information of its previous stage. The new file generated adds the new information of the stage under analysis in a NetCDF's subgroup called **L*x*_Data**, being ***x*** the data level number. This can be seen in the next figure, showing the data added in L1 stage:

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

To avoid mistakes, it is preferable to use absolute paths for all the arguments passed to the modules.

In next sections, a step-to-step on how to install and run these modules are described.

## Installation:
Download or clone the repository from GitHub: `www.github.com/juanpallotta/LPP`. You will find:
- `/libLidar`: C/C++ lidar libraries source code.
- `/Lidar_Analysis_L0`: C/C++ sources code of `lidarAnalysis_PDL0` to produce level 0 (L0) data products.
- `/Lidar_Analysis_L1`: C/C++ sources code of `lidarAnalysis_PDL1` to produce level 1 (L1) data products.
- `/Lidar_Analysis_L2`: C/C++ sources code of `lidarAnalysis_PDL2` to produce level 2 (L2) data products.
- `/Lidar_Configuration_Files`: Contain the configuration files (`.conf`) for each module. Also, the settings file for an automatic run (see later [section](#Automatizing_LPP) about the automatization of the modules).
- `/signalsTest`: Lidar test files to test this code. You will find files from Buenos Aires, Argentina (pure Licel datatype files) and Brazil: Sao Paulo (`Brazil/SPU/` folder, Licel data type files) and Manaus ( `Brazil/Manaus` folder, Raymetric datatype files).
- `install_Lidar_Dependencies.sh`: Linux shell-script to install the basic software/libraries needed to compile and run the LPP software.
- `/compile_All.sh`: Linux shell script to compile all the modules.
- `/run_LPP_Analysis.sh`: Linux shell script to run the whole chain automatically. Main settings for automatic run are configured in its setting file (`/Lidar_Configuration_Files/LPP_Settings.sh`). More about the automatization of all modules in [Automatizing LPP](#Automatizing_LPP) section of this README file.
- `README.md`: This file.

## Installing dependencies:
There are a few and very basic things to be installed prior to the run of LPP. To do this job, just run the Linux shell script named `install_Lidar_Dependencies.sh`.
It is a simple Linux shell script to install the basics packages (make, g++, and NetCDF libraries). You will be asked for administrator credentials.
Remember to set `install_Lidar_Dependencies.sh` with executable attributes: `chmod +x install_Lidar_Dependencies.sh`.

## Building the code:
To compile all the modules, just run the Linux shell script named `compile_All.sh`. This is a simple Linux script that makes the executables of each module inside their folders. Remember to set `compile_All.sh` with executable attributes: `chmod +x compile_All.sh`.
At the moment, the compiler output will show some warnings. All of them will be cleared up in the future versions.

## Configuring and running LPP modules:
The behavior of each module is based on the parameters written in its configuration file, passed as the [third argument](#run_module). In this repository, they are stored in the `/Lidar_Configuration_Files`. These are text-based files and have the variables needed for the module, having to follow only 4 main rules:
1. Comments are defined by "`#`" character. You are free to comment on anything to make the run more understandable. The configuration files included in this repository have many comments to explain each variables. Also, if a variable accepts a vector of values, all of them are commented.
2. Variables definition has to follow the convention `VAR_NAME = VALUE`, and a <u>**minimum of 1 space character has to be placed before and after the "`=`" character**</u>. The variables data type can be integer, float, double, or string.
3. Some variables have to be set as vectors. Each element must be separated with the character "`:`", for instance: `VAR_NAME = VALUE1 : VALUE2 : VALUE3` <u>**and a minimum of 1 space character has to be placed before and after the "`:`" character**</u>. The number of elements depends on the variable, and how LPP has it implemented. In order to minimize the mistakes related to this, please, read the comments in the lines before the variable definition. In case that the number of elements doesn't meet the right values, LPP will show a warning and exit the execution.
4. The configuration file could be the same for all the modules, or use one file for all of them, as far it contains the variables needed for the run. These variables are described in the next sections.

The following sections describe each module, how to configure it, and how to run it. We highly encourage you to run the examples shown and play with its variables to feel comfortable with the uses of the modules. Then, use your own input files.

### `lidarAnalysis_PDL0`: Converting raw lidar files in a single NetCDF file
This module is used to merge the raw lidars files located in a folder (passed as first argument), into a single NetCDF file (path and filename passed as a second argument). The configuration file is passed as third argument, and its going to be described in this section.

An example of how to run this module using the sample signals included in this repository is shown below:

    ./lidarAnalysis_PDL0 ../signalTest/Brazil/SPU/20210730/ ../signalTest/Brazil/SPU/20210730/LPP_OUT/20210730_L0.nc ../Lidar_Configuration_Files/analysisParameters_PDL0_Brazil.conf

Where:
- `lidarAnalysis_PDL0`: Executable file of the L0 module.
- `../signalTest/Brazil/SPU/20210730/`: Input folder with the raw-lidar files in Licel or Raymetric data format. Since Licel files have no defined extension, there is no way to distinguish them from the rest of the files. Because of that <u>**it is important that nothing but raw lidar data files must be in this input folder**</u>. 
- `../signalTest/Brazil/SPU/20210730/LPP_OUT/20210730_L0.nc`: Output path, **including the file name** of the output NetCDF file, in this example `20210730_L0.nc`. If the output file needs to be placed in a subfolder, generate it manually before running (in this example `LPP_OUT/`).
- `../Lidar_Configuration_Files/analysisParameters_PDL0_Brazil.conf`: Configuration file with the variables needed for the merging.

Be careful if relative path is used: it must be relative to the executable file. In this example, the executable file is `lidarAnalysis_PDL0` and is in the folder `Lidar_Analysis_L0/`.

An example of how L0 configuration file should look is seen in the next code:

```bash
#############################################################
# CONFIGURATION FILE FOR LIDAR CONVERSION TOOL 
# RAW LIDAR DATA FILES TO NETCDF FILE - DATA LEVEL 0 (L0)
#############################################################

# DATE-TIME RANGE TO ANALYZE INSIDE THE FOLDER PASSED AS ARGUMENT TO lidarAnalysis_PDL0
# IF minTime = maxTime --> ALL FILES INSIDE THE FOLDER WILL BE ANALYZED
minTime = 2021/07/30-00:00:00
maxTime = 2021/07/30-00:00:00

# INPUT DATAFILE FORMAT
inputDataFileFormat = LICEL_FILE
# inputDataFileFormat = RAYMETRIC_FILE
# inputDataFileFormat = LALINET_NETCDF -- NOT IMPLEMENTET YET
# inputDataFileFormat = SCC_NETCDF -- NOT IMPLEMENTET YET
# inputDataFileFormat = CSV -- NOT IMPLEMENTET YET

# OUTPUT NETCDF DATAFILE FORMAT --> outDataFileFormat
outputDataFileFormat = LALINET_NETCDF
# outputDataFileFormat = SCC_NETCDF
```

At the moment, configuration files for L0 data level only need a few of basic inputs. 
* `minTime` and `maxTime`: Time bin to analyze inside the input folder. In case to analyze all the files inside the folder, these two variables have to be set equals. Pay attention to the format: `YYYY/MM/DD-HH:MM:SS`.
* `inputDataFileFormat`: At the moment, only Licel (`LICEL_FILE`) or Raymetric (`RAYMETRIC_FILE`) data type file are accepted. As can be seen, there are planned more input data types formats to convert. 
* `outputDataFileFormat`: The output datatype accepted are: LALINET (`LALINET_NETCDF`) and Sigle Calculus Chain (`SCC_NETCDF`) datatype file. 

A description of LALINET data type and its name conventions, can be see in later section in this document ([LALINET data type format](#LALINET_data_type_format)).

If `SCC_NETCDF` is selected, the output file generated can be used as an input for the SCC platform (https://www.earlinet.org/index.php?id=281). More info about SCC data file format and its name convention is described in the web page of the project (https://docs.scc.imaa.cnr.it/en/latest/file_formats/netcdf_file.html). It is also important to mention that if this output is selected, later modules of LPP (`lidarAnalysis_PDL1` and `lidarAnalysis_PDL2`) cannot be used.

In order to proceed without mistakes, it highly recommended uncomment the proper line in the configuration files included in this repository. Also, its worth mentioning that this inputs allows only one valid entry for each variable, so carefully check that only one line of each variable is uncommented.


### `lidarAnalysis_PDL1`: Producing data level 1 products: lidar signals corrections and cloud-mask

This module receive the NetCDF file produced by the previous module (`lidarAnalysis_PDL0`) as a first parameter (in our example, `../signalTest/Brazil/SPU/20210730/LPP_OUT/20210730_L0.nc`). This module will accept the input file while it is in the LALINET NetCDF format, this mean, the variable `outputDataFileFormat = LALINET_NETCDF` should be set in the configuration file of L0 module.

An example of how to run this module can be:

    ./lidarAnalysis_PDL1 ../signalTest/Brazil/SPU/20210730/LPP_OUT/20210730_L0.nc ../signalTest/Brazil/SPU/20210730/LPP_OUT/20210730_L0_L1.nc ../Lidar_Configuration_Files/analysisParameters_PDL1_2_Brazil.conf

The configuration file (in this case, `../Lidar_Configuration_Files/analysisParameters_PDL1_2_Brazil.conf`) could be the same file used in module L0, or use another one. The only important thing here is the variables included in the file passed as thrid argument.

The sample files included in this repository uses one file for `lidarAnalysis_PDL0`, and another for modules `lidarAnalysis_PDL1` and `lidarAnalysis_PDL2`.

An example of a configuration file and its variables for L1 data level retrieval could be:

```bash

##########################################################################################
# PARAMETERS FOR lidarAnalysis_PDL1
##########################################################################################

# LASER-BIN-OFFSET OR TRIGGER-DELAY OR ZERO-BIN
# VECTOR TYPE: ONE PER CHANNEL
# MANAUS
# indxOffset = 11 : 11 : 11 : 11 : 11 
# SAO PAULO
indxOffset = 2 : 3 : 7 : 5 : 8 : 2 : 9 : 2 : 2 : 2 : 2 : 2 

# NUMBER OF FILES (EVENTS) THAT WILL BE AVERAGED INTO A SINGLE LIDAR SIGNAL
numEventsToAvg_PDL1 = 10

# WAVELENGHT USED FOR PDL1. INDEXES STARTING FROM 0
# SP
indxWL_PDL1 = 2
# MANAUS
# indxWL_PDL1 = 0

# MOLECULAR REFERENCES
Temperature_at_Lidar_Station = 25.0
Pressure_at_Lidar_Station = 940.0

# INITIAL RANGE OF ANALYSIS: rInitSig
# END RANGE OF ANALYSIS: rEndSig
rInitSig = 1100
rEndSig = 25000

# NUMBER OF BINS USED FOR BACKGROUND CALCULATION (TAKEN FROM THE TAIL OF THE LIDAR SIGNAL)
nBinsBkg = 1000

# CLOUD DETECTION PARAMETERS
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

A description of each of this parameters are:

* `indxOffset`: The number of bins to remove from the beginning of the lidar track recorded due to the laser offset. This parameter should be a vector, with the number of elements equal to the number of channels in the file.

* `numEventsToAvg_PDL1`: Time averaging for L1 data level products. This parameters tell to `lidarAnalysis_PDL1` the numbers of lidar profiles to average and producing one merged profile. After this, the `time` dimension in the NetCDF file from the L1 data products will be reduced by `numEventsToAvg_PDL1` times.

* `indxWL_PDL1`: An index (starting from 0) of the channel to use in cloud-mask production. It is recommended to use an elastic lidar channel and the highest wavelength in the file.

* `Temperature_at_Lidar_Station`: Temperature at ground level (in Celsius).
* `Pressure_at_Lidar_Station`: Pressure at ground level (in hPa)

* `rInitSig` : Initial range of the analysis (in meters from the lidar). It is prefereable to use the first point where the full overlap is achieved.
* `rEndSig `: End range of analysis (in meters).

* `nBinsBkg = 1000`: Number fo bins used for background removal. 

* **Cloud-mask retrieval parameters:** These parameters are required for the cloud detection algorithm. We strongly recommend using the values set in the files included in this repository. This algorithm is robust enough to work with a wide range of lidar signals using this setup, detecting clouds and plumes under a variety of conditions.

```
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

<!-- ### `lidarAnalysis_PDL2`. Producing data level 2 products: aerosol optical parameters

    ./lidarAnalysis_PDL2 ../signalTest/Brazil/SPU/20210730/LPP_OUT/20210730__L0_L1.nc ../signalTest/Brazil/SPU/20210730/LPP_OUT/20210730_L0_L1_L2.nc ../Lidar_Configuration_Files/analysisParameters_PDL1_2_Brazil.conf -->


## <a name="Automatizing_LPP"></a>Automatizing LPP

In order to run all the LPP modules automatically, this repository contains a Linux script to do this task. The name of this script is `run_LPP_Analysis.sh`, and you can find it at the root folder of this repository. It uses a general configuration file named located in the configuration folder `Lidar_Configuration_Files/LPP_Settings.sh`.

With this script, the outputs and input paths and filenames of every module are automatically generated and passed as argument for every `lidarAnalysis_PDLx`. The rules for performing this task are explained in this section.

This automatization process use another configuration file, named `Lidar_Configuration_Files/LPP_Settings.sh`, where the data set level and the input and output paths are set. An example of this file can be seen below:

```bash
#!/bin/bash

# DATA LEVEL TO PRODUCE IN THE RUN. 
L0="yes"
L1="yes"
L2="no"

# ABSOLUTE INPUT PATH
# PATH_IN="../signalsTest/Brazil/Manaus/2011/10/17/"
PATH_IN="../signalsTest/Brazil/SPU/20210730/"

FILE_CONF_L0="../Lidar_Configuration_Files/analysisParameters_PDL0_Brazil.conf"
FILE_CONF_L1_L2="../Lidar_Configuration_Files/analysisParameters_PDL1_2_Brazil.conf"

```

As can be seen, a few lines are needed to run the automatic mode. These are:
* `L0`, `L1` and `L2`: Data level to process. By setting `"yes"` or `"no"` at the variables `Lx`, you can control the run of each module.
* `PATH_IN/`: Main path of the raw lidar data, in wich only lidar data files (Licel or Raymetric data format) must be stored. This is the starting folder for searching raw lidar files. A folder named `LPP_OUT/` will be created automatically in the last subfolder found with raw lidar files. The `LPP_OUT` folder will be used to store the NetCDF output files produced by each LPP module.
Because there are different ways to store the data files and their folder structures, two examples will be shown and how the folder/files are generated automatically. 
* `FILE_CONF_L0` and `FILE_CONF_L0_L1`: Path to the configuration file of each module.

In order to explain how the paths are generated automatically, two examples are shown. First one is used in Sao Paulo and Argentinean lidars, where the lidar files produced in a day are stored in a single folder with the full date in its name. In the next figure, an example is shown:

![SPU_files](./Docs/Figures/SPU_lidar_files.png "SPU lidar files")

It can be seen that inside a single folder you can find all the files for that day. In this situation, the script `run_LPP_Analysis.sh` will create the folder `LPP_OUT/` automatically and the output files of each modules will save the data in it. If we take a look inside the folder `/LPP_OUT` it can be seen the NetCDF files created by the modules `lidar_Analysis_PDL0` and `lidar_Analysis_PDL1`:

![SPU_output_folder_files](./Docs/Figures/SPU_files_LPP_folder.png "LPP output folder")

In the case of Manaus lidars, the structure of the folder for each measurement can be seen in next figure:

![Manaus_output_folder_files](./Docs/Figures/Manaus_lidar_files.png "LPP output folder")

In this case, the files are stored in a folder with the number of the day, and the data of the month and year are in a higher order folders. In this scenario, the script `run_LPP_Analysis.sh` will create the `/LPP_OUT` folder in the last subfolder, in this case, in `/17/LPP_OUT`. Inside of it, the output files of each module are found:

![SPU_output_folder_files](./Docs/Figures/Manaus_files_LPP_folder.png "LPP output folder")


As can be seen, there are also rules for creating the NetCDF files name. `lidar_Analysis_PDL0` will create the L0 data level file using the name of the last subfolder with lidar files, adding `_L0.nc`. The rest of the modules, will add `_L1` and `_L2` to the input filename to produce the output filename. It can be seen in last figure, where `lidar_Analysis_PDL0` produce `17_L0.nc` and  `lidar_Analysis_PDL1` produce `17_L0_L1.nc`. 


## <a name="LALINET_data_type_format"></a> LALINET Data Type Format
