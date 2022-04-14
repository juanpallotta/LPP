#!/bin/bash

# ---------------------------- EDIT CONFIGURATION START HERE
# DATA LEVEL TO PRODUCE IN THE RUN - "yes"/"no", LOWERCASE
L0="yes"
L1="yes"
L2="yes"

#* ABSOLUTE PATHS!!!
#* DO NOT USE SPACES!!!! 
# PATH_IN="/home/juan/LidarAnalysisCode/LPP/signalsTest/Holger_Sim/bkg1e0/"

# PATH_IN="/mnt/Disk-1_8TB/Brazil/SPU/20210730/"
# PATH_IN="../signalsTest/Brazil/Manaus/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/Manaus/2011/10/17/"
# PATH_IN="/mnt/Disk-1_8TB/Argentina/Aeroparque/20220301/"
PATH_IN="/mnt/Disk-1_8TB/Argentina/Cordoba/20190928/test/"

# NOISE FILE OBTAINED WITH THE TELESCOPE COVERED AND THE LASER FIRING.
# ! IF THERE IS NO NOISE FILE: THE FILENAME MUST BE SET AS: "-"
# ! IF THERE IS    NOISE FILE: THE FILENAME MUST CONTAIN THE STRING 'bkg' IN SOME PART OF ITS NAME!!!
PATH_DARK_FILE="-"
# PATH_DARK_FILE="/mnt/Disk-1_8TB/Argentina/Aeroparque/bkg_20220307"

# CONFIGURATION FILES --> RELATIVE TO THE FOLDER WICH IS RUNNING THE ANALYSIS!!
# HOLGER SIMULATION
# FILE_CONF_L0="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_PDL0_Holger.conf"
# FILE_CONF_L1_L2="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_PDL1_2_Holger.conf"
# BRAZIL
# FILE_CONF_L0="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_PDL0_Brazil.conf"
# FILE_CONF_L1_L2="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_PDL1_2_Brazil.conf"
# ARGENTINA
FILE_CONF_L0="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_PDL0_Argentina.conf"
FILE_CONF_L1_L2="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_PDL1_2_Argentina.conf"
