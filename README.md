
# Lidar Processing Pipeline Manual (tested on Linux Ubuntu 20.04.3 LTS, and Windows 10 using WSL).

The Lidar Processing Pipeline (LPP) is formed by 3 modules, each one completely independent of the other. The names of each module are represented by the output of its product data level, named from 0 to 2. Each one of these modules can be executed independently as a tool, following the rules described in this document. These modules are:
- `lidarAnalysis_PDL0`: Transforms the raw lidar data files to a single NetCDF file containing the raw lidar signals and global information about the measurement. The variable's name follows the name convention of the Single Calculus Chain (SCC) platform. This output is the L0 data level of LPP.
- `lidarAnalysis_PDL1`: Receive the NetCDF file produced in the previous stage (L0) and produce a new NetCDF file with all the corrections applied to the lidar signals, like laser offset and bias correction. Also, the cloud mask is obtained and stored with the molecular profiles. This output is the L1 data level of LPP.
- `lidarAnalysis_PDL2`: Receive the NetCDF file produced in the previous stage (L1) and produce a new NetCDF file with all the optical products retrieval applied to the elastic lidar signals contained in the NetCDF input file. This output is the L2 data level of LPP.

Each runs in a Linux terminal, following the next convention:

    user@lidarAnalysis:~$./lidarAnalysis_PDLx /Input_File_or_Folder/ /Output_File analysisParameters.conf

Where:
- `lidarAnalysis_PDLx`: Sofware module to generate the data level ***x***.
- `/Input_File_or_Folder/`: Input folder or file to produce the data Level ***x***. In the case of L0, this first parameter is the folder with the lidar data files in the Licel or Raymetric data file format. For the rest of the data levels, the input is the NetCDF file produced in the previous stage.
- `/Output_File`: Output NetCDF filename. The output file contains the information of the input folder/file, adding the information of the new data of the level under analysis.
- `analysisParameters.conf`: Configuration file with all the variables needed for the analysis of level analysis. 

It is important to mention that the output files produced in the levels 1 and 2 contains all the information of the previous stage (input data file, first parameter). The new file generated add the new information in the NetCDF file in a sub-group called **L*x*_Data**, being ***x*** the data level number. This can be seen in next figure, showing the data added in L1 stage:

![Panoply subgroup](./Docs/Figures/sub_group_nc.png "NetCDF Subgroup")

How to install and run these modules are described in the next sections.

## Installation:
Download the lastest version from `www.github.com/juanpallotta/LPP`. You will find 7 folders and 2 files:
- `/libLidar`: C/C++ lidar libraries source code.
- `/Lidar_Analysis_L0`: C/C++ sources code of `lidarAnalysis_PDL0` to produce level 0 data products.
- `/Lidar_Analysis_L1`: C/C++ sources code of `lidarAnalysis_PDL1` to produce level 1 data products.
- `/Lidar_Analysis_L2`: C/C++ sources code of `lidarAnalysis_PDL2` to produce level 2 data products.
- `/Lidar_Configuration_Files`: Contain the configuration (`.conf`) files of each module. 
- `/signalsTest`: Lidar test files to test this code. You will find files from Argentina (pure Licel datatype files) and Brazil (Raymetric datatype files).
install_Lidar_Dependencies.sh: a Linux shell-script to install the basic software/libraries needed to compile and run the LPP software.
- `/compile_All.sh`: Linux shell script to compile all the modules.
- `/run_LPP_Analysis`: Linux shell script to run the whole chain automatically, following the rules saved in the configuration files.
- `README.md`: This file.

## Installing dependencies:
Run the Linux shell script named: `install_Lidar_Dependencies.sh`. 
It is a simple Linux shell script to install the basics packages (make, g++, and NetCDF libraries). You will be asked for administrator credentials. Before running it, remember to check if the executable attributes is set. If not the case, make it executable with the command:

    chmod +x install_Lidar_Dependencies.sh

This advisor is also applicable to any other script in the project.

## Building:
To compile and run all the modules, just run the Linux shell script named: `compile_All.sh`.
At the moment, the compiler output will show some warnings, please, forget them for now).

## Configuring and running LPP modules:
In this section, how to configure each module will be described. This is done by setting the variables in the configuration file (extention `.conf`, located in the folder `/Lidar_Configuration_Files`).

### `lidarAnalysis_PDL0`: Converting raw lidar files in a single NetCDF file:
This module is used to merge the raw lidar into a single NetCDF file. An example of how to run this module using the example files included in this release can be:

    ./lidarAnalysis_PDL0 ../signalTest/Brazil/SPU/20210730/ ../signalTest/Brazil/SPU/20210730/LPP_OUT/20210730_L0.nc analysisParameters_PDL0_Brazil.conf 

Where:
- `lidarAnalysis_PDL0`: Executable file.
- `../signalTest/Brazil/SPU/20210730/`: Input folder with the raw-lidar files in Licel or Raymetric data format.
- `../signalTest/Brazil/SPU/20210730/LPP_OUT/20210730_L0.nc`: Output path **INCLUDING THE OUTPUT FILE NAME** of the output NetCDF file.
- `analysisParameters_PDL0_Brazil.conf`: Configuration file with the variables needed for the merging.

In the next section, a description of the configuration file is described.


SEGUIR ACA
------------------------


#### `analysisParameters_PDL0_Brazil.conf`:
In this file, the merging parameters are set, and only 2 main ideas have to be taken into account:
1. Comments are defined by `#` character.
2. Variables definition has to follow the convention `VAR_NAME = VALUE`, and a minimun of 1 space character has to be placed before and after the `=` character. The variables can be integers, float, double or string data format.

        ####################################################
        # CONFIGURATION FILE FOR LIDAR CONVERSION TOOL
        ####################################################
        # DATE-TIME RANGE TO ANALYZE INSIDE THE FOLDER PASSED AS ARGUMENT TO mergeLidarFiles
        # minTime = 2020/08/05-00:00:00
        # maxTime = 2020/08/05-01:00:00
        # minTime = 2018/06/19-00:00:00
        # maxTime = 2018/06/19-03:00:00
        minTime = 2011/07/14-00:00:00
        maxTime = 2011/07/14-03:00:00

        # INPUT DATAFILE FORMAT --> inputDataFileFormat
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


## Automatization of the whole process


In order to run this code in a more convenient way, there is a Linux script to configure and run it. The name of this script is runMergeLidarFiles.sh, and its shelf-explanatory. Here I copy an example of this script:

