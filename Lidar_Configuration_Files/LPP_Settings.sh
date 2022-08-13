#!/bin/bash

# ---------------------------- EDIT CONFIGURATION START HERE
# DATA LEVEL TO PRODUCE IN THE RUN - "yes"/"no", LOWERCASE
L0="yes"
L1="yes"
L2="yes"

#* ABSOLUTE PATHS!!!
#* DO NOT USE SPACES!!!! 
# PATH_IN="/home/juan/LidarAnalysisCode/LPP/signalsTest/Holger_Sim/bkg1e0/"

# PATH_IN="/mnt/Disk-1_8TB/Argentina/Cordoba/Paper_Celeste/20190924/"
# PATH_IN="/mnt/Disk-1_8TB/Argentina/Aeroparque/20220811/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/Manaus/2011/9/02/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/Manaus/2011/9/21/"
PATH_IN="/mnt/Disk-1_8TB/Brazil/SPU/20200914/lidar_signals/"
# PATH_IN="/mnt/Disk-1_8TB/Argentina/Hector/2022/07/13/data/"

# NOISE FILE OBTAINED WITH THE TELESCOPE COVERED AND THE LASER FIRING.
# ! IF THERE IS NO NOISE FILE: THE FILENAME MUST BE SET AS: "-"
# ! IF THERE IS    NOISE FILE: THE FILENAME MUST CONTAIN THE STRING 'bkg' IN SOME PART OF ITS NAME!!!
# PATH_DARK_FILE="-"
PATH_DARK_FILE="/mnt/Disk-1_8TB/Brazil/SPU/20200914/dark_current/bkg_s2091411.394544"
# PATH_DARK_FILE="/mnt/Disk-1_8TB/Argentina/Hector/2022/07/13/bkg_cero_13_07_2022_2212.licel"

# CONFIGURATION FILES --> RELATIVE TO THE FOLDER WICH IS RUNNING THE ANALYSIS!!
# HOLGER SIMULATION
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Holger.conf"
# BRAZIL
FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Brazil.conf"
# ARGENTINA
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Argentina.conf"
# PILAR (CORDOBA), 2009-AGOSTO/SEPTIEMBRE
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Pilar_2009_AgoSep.conf"
# HECTOR
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Hector.conf"
