#!/bin/bash

# ---------------------------- EDIT CONFIGURATION START HERE
# DATA LEVEL TO PRODUCE IN THE RUN - "yes"/"no"
L0="yes"
L1="yes"
L2="yes"

#* ABSOLUTE PATHS!!!
#* DO NOT USE SPACES!!!! 
# PATH_IN="/home/juan/LidarAnalysisCode/LPP/signalsTest/Holger_Sim/Original/"
# PATH_IN="/home/juan/LidarAnalysisCode/LPP/signalsTest/Pappalardo_2004/"

# PATH_IN="/mnt/Disk-1_8TB/Argentina/Aeroparque/2023/20231104/"
# PATH_IN="/mnt/Disk-1_8TB/Argentina/Aeroparque/2023/20231104/"
# PATH_IN="/mnt/Disk-1_8TB/Argentina/Aeroparque/miniLidar/20230728/"
# PATH_IN="/mnt/Disk-1_8TB/Argentina/Aeroparque/miniLidar/20230727/"
# PATH_IN="/mnt/Disk-1_8TB/Argentina/Hector/20230914/data/"

PATH_IN="/mnt/Disk-1_8TB/Granada/20230214/RS_20230214_0925/test/"

# PATH_IN="/mnt/Disk-1_8TB/Chile/20190507/"

# PATH_IN="/mnt/Disk-1_8TB/Brazil/Manaus/2011/9/21/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/Manaus/2011/8/03/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/SPU/20210730/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/SPU/20200914/lidar_signals/"

# CONFIGURATION FILES

# ! SIMULATIONS
# ! HOLGER
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Holger.conf"
# ! PAPPALARDO 2004
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Pappalardo.conf"
# ! CHILE
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_ChileSN.conf"
# ! BRAZIL
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_SPU.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Manaus.conf"
# ! GRANADA - ALHAMBRA
FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Granada.conf"
# ! ARGENTINA
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Argentina_MiniLidar.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Argentina_AER.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Argentina_BAR.conf"
#! PILAR (CORDOBA), 2009-AGOSTO/SEPTIEMBRE
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Pilar_2009_AgoSep.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Hector.conf"
#! MULTIANGLE
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_multRam.conf"
