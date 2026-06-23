
# Skills

You are a programmer who is tasked with automating the Lidar Processing Pipeline (LPP) data retrieval and processing workflow. More information about LPP and how it work, read the README.md at [Lidar Processing Pipeline](https://github.com/juanpallotta/LPP/blob/main/README.md).

You are also a Linux expert and are proficient in shell scripting. You are able to read and understand configuration files, and determine the correct parameters to update based on the instructions provided. You are also capable of interpreting the complex and often unclear instructions provided by the user to create a working script that fulfills the user's request.

## Task

Create a robust Linux script that automates the Lidar Processing Pipeline (LPP) analysis workflow. The script must dynamically update parameters for each data entry and execute the LPP analysis. The automation should be driven by the file structure within the "PATH_IN" directory, set as is described in the configuration file (see [conf_lpp_bulk.conf](conf_lpp_bulk.conf)). It looks like this:

```bash
PATH_TO_LPP = /home/juan/LPP
PATH_IN = /mnt/c/lidarData/SPU/test/
FILE_CONF = /home/juan/LPP/Lidar_Configuration_Files/analysisParameters_SPU.conf
Path_To_Molecular_File = /mnt/c/lidarData/SPU/Radiosonde/
PATH_TO_OUTPUT = /mnt/c/lidarData/SPU/Results/
```

This file is already created in this directory as [conf_lpp_bulk.conf](conf_lpp_bulk.conf) with the right file paths for this specific scenario. This file accepts comments by using the character `#`.

The function of each path is described bellow:

- PATH_TO_LPP --> Path to the Lidar Processing Pipeline (LPP) main folder. This folder must contain the `run_LPP_Analysis.sh` and also the `/Lidar_Configuration_Files` folder which contains the `LPP_Run_Settings.sh` and `analysisParameters_XXX.conf` (the complete path to this last file is pointed by `FILE_CONF`) files.

- PATH_IN --> Path to the data folder. This folder should contain sub-folders of data named with the date `YYYYMMDD`, where `YYYY` is the year, `MM` is the month, and `DD` is the day. Inside this folder, there should be a `data` folder which contains the data files and a `dark_current` folder which contains the dark files.

- FILE_CONF --> Path to the configuratio file for the LPP run (`.conf` file). This file contains the parameters for the LPP analysis.
  
- Path_To_Molecular_File --> Path to the radiosonde folder. This folder should contain radiosonde files. The Radiosonde filenames are formatted like: rad_XXXXX_YYYYMMDDHHnn.csv, where the digits `XXXXX` correspond to the station ID (does not matter for the analysis), the following 8 digits `YYYYMMDD` correspond to the date (YYYYMMDD), and the last 2 digits `nn` correspond to the hour in which the radiosonde was launched (you don't needed for this analysis). If you find a radiosonde file for the same day but for a different hour, take the first one you found.

- PATH_TO_OUTPUT --> Path to the results folder. This folder should contain the results of the LPP analysis. Create inside this folder one subfolders for each day you find in PATH_IN, the name of the subfolder should be the same as the date folder.

## Instructions for the Linux script you must develop

The Linux script must modify 2 configuration files:

1. File `PATH_TO_LPP/Lidar_Configuration_Files/LPP_Run_Settings.sh`
2. File `FILE_CONF`

Using the paths provided in the [conf_lpp_bulk.conf](conf_lpp_bulk.conf) file.

For the first file (`PATH_TO_LPP/Lidar_Configuration_Files/LPP_Run_Settings.sh`), the script must search for the followings variables and updated based on:

1. Variables:

```bash
L0="yes"
L1="yes"
L2="yes"
```

Must be replaced by its values in the file `conf_lpp_bulk.conf`. If one of these variables does not exist in the file `conf_lpp_bulk.conf`, the script must create it and set its value as "yes".

2. Variable `PATH_IN` --> must be replaced by one of the paths inside folder `PATH_IN/YYYYMMDD/data/`
3. Variable `FILE_CONF` --> must be replaced by variable `FILE_CONF` located in the file `conf_lpp_bulk.conf`.

For the second file (`FILE_CONF`), the script must update the following parameters:

1. `PATH_DARK_FILES` --> taken from the dark files folder filename `PATH_IN/YYYYMMDD/dark_current/`.
2. `Path_To_Molecular_File` --> taken from the Radiosonde folder (`Path_To_Molecular_File/rad_XXXXX_YYYYMMDDHHnn.csv`).

Once you modify all these parameters in the 2 mentioned files, LPP should be executed by running the script `run_LPP_Analysis.sh` located in the main folder (`PATH_TO_LPP`). When the script finishes, you will find the folder `LPP_OUT` inside the folder `PATH_IN/YYYYMMDD/data`, and the folder `LPP_OUT` must be moved to the `PATH_TO_OUTPUT` folder (defined in the `conf_lpp_bulk.conf` file). Additionally, rename the folders within the results folder to indicate the date like this: YYYYMMDD_LPP_OUT_WLindex. Use the variable `indxWL_PDL2` located in the file `PATH_TO_PARAM` as an indicator of which index channel the data was processed for.
For example, if indxWL_PDL2 = 0 and the data was retrieved from a data folder named 20200107, the resulting output folder should be called 20200107_LPP_OUT_00 (use 2 digits for the index channel). If indxWL_PDL2 = 2 and the data was retrieved from a data folder named 20200107, the resulting output folder should be called 20200107_LPP_OUT_02.

Once you moved the `LPP_OUT` folder, you must delete all the folders inside the `PATH_IN/YYYYMMDD/data` folder.

This process must be repeated for each day found in `PATH_IN` folder.

If some error occured during the execution, save an Markdown file something about the issue (e.g., no radiosonde file, some of the modules of LPP quit its execution suddently, etc).
