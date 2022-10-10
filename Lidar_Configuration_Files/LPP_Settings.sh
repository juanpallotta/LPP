#!/bin/bash

# ---------------------------- EDIT CONFIGURATION START HERE
# DATA LEVEL TO PRODUCE IN THE RUN - "yes"/"no", LOWERCASE
L0="yes"
L1="yes"
L2="yes"

#* ABSOLUTE PATHS!!!
#* DO NOT USE SPACES!!!! 
# PATH_IN="/home/juan/LidarAnalysisCode/LPP/signalsTest/Holger_Sim/bkg1e4/"
# PATH_IN="/mnt/Disk-1_8TB/Argentina/Cordoba/Paper_Celeste/20190927/"
# PATH_IN="/mnt/Disk-1_8TB/Argentina/Aeroparque/20220817/signals/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/Manaus/2011/8/14/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/Manaus/dark_Current_Files/Dark_Current_15h42/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/SPU/20200914/lidar_signals/"
PATH_IN="/mnt/Disk-1_8TB/Argentina/Hector/202208/01/data/"

# NOISE FILE OBTAINED WITH THE TELESCOPE COVERED AND THE LASER FIRING.
# ! IF THERE IS NO NOISE FILE: PATH_DARK_FILE MUST BE SET AS: "-"
# ! IF THERE IS    NOISE FILE: PATH_DARK_FILE MUST CONTAIN THE STRING 'bkg' IN SOME PART OF ITS NAME!!!
# PATH_DARK_FILES="-"
# PATH_DARK_FILES="/mnt/Disk-1_8TB/Argentina/Cordoba/Paper_Celeste/bkg_noise/2700/"
# PATH_DARK_FILES="/mnt/Disk-1_8TB/Argentina/Aeroparque/20220817/bkg_noise/"
# PATH_DARK_FILES="/mnt/Disk-1_8TB/Brazil/SPU/20200914/dark_current_2/"
# PATH_DARK_FILES="/mnt/Disk-1_8TB/Brazil/Manaus/dark_Current_Files/Dark_Current_15h57/"
PATH_DARK_FILES="/mnt/Disk-1_8TB/Argentina/Hector/202208/01/noise/"

# OVERLAP FILE.
# ! IF THERE IS NO OVERLAP FILE: VERLAP_FILE MUST BE SET AS: "-"
# ! IF THERE IS    OVERLAP FILE: OVERLAP_FILE MUST BE SET WITH THE FULL PATH
OVERLAP_FILE="-"
# OVERLAP_FILE="/mnt/Disk-1_8TB/Brazil/SPU/Overlap_Files/overlap_SPU.csv"
# OVERLAP_FILE="/mnt/Disk-1_8TB/Brazil/Manaus/Overlap_Files/overlap_narrow_4000p.csv"

# CONFIGURATION FILES --> RELATIVE TO THE FOLDER WICH IS RUNNING THE ANALYSIS!!
# ! HOLGER SIMULATION
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Holger.conf"
# ! BRAZIL
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_SPU.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Manaus.conf"
# ! ARGENTINA
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Argentina.conf"
# PILAR (CORDOBA), 2009-AGOSTO/SEPTIEMBRE
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Pilar_2009_AgoSep.conf"
# ! HECTOR
FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Hector.conf"
