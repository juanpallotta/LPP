#!/bin/bash

# ---------------------------- EDIT CONFIGURATION START HERE
# DATA LEVEL TO PRODUCE IN THE RUN - "yes"/"no"
L0="yes"
L1="yes"
L2="yes"

#* ABSOLUTE PATHS!!!
#* DO NOT USE SPACES!!!! 

# PATH_IN="/home/juan/LidarAnalysisCode/LPP/signalsTest/LPP_SIM/MultiAngle/lpp_sim_multiangle_L0.nc"

# PATH_IN="/mnt/Disk-1_8TB/Potenza/licel_raw/"
# PATH_IN="/mnt/Disk-1_8TB/Argentina/Aeroparque/2024/20240810/"
# PATH_IN="/mnt/Disk-1_8TB/Argentina/Aeroparque/miniLidar/20230728/" 
# PATH_IN="/mnt/Disk-1_8TB/Argentina/Cordoba/2024/20241010/" 
# PATH_IN="/mnt/Disk-1_8TB/Bolivia/20240925/2024_09_25_HR0924_A90_data_2CH_L0.nc"
PATH_IN="/mnt/Disk-1_8TB/Colombia/Medellin/20220714/"
# PATH_IN="/mnt/Disk-1_8TB/Granada/20230830_nCh60/RS_20230830_0315/"
# PATH_IN="/mnt/Disk-1_8TB/Granada/20240916_nCh60/RS_20240916_0916/"

# CONFIGURATION FILES
# ! SIMULATIONS
# ! LPP SIMULATION
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_LPP_simulation.conf"
# ! HOLGER
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Holger.conf"
# ! PAPPALARDO 2004
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Pappalardo.conf"
# ! CHILE
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_ChileSN.conf"
# ! COLOMBIA 
FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Medellin.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Cali.conf"
# ! BRAZIL
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_SPU.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Brazil_Rio.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Manaus.conf"
# ! GRANADA - ALHAMBRA
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Granada.conf"
# ! POTENZA - MUSA
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Potenza.conf"
# ! ARGENTINA
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Argentina_MiniLidar.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Argentina_AER.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Argentina_BAR.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Argentina_COMR.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Pilar_2019_AgoSep.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Pilar.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Hector.conf"

# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Argentina_BAR_CALBUCO.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Argentina_NQN_CALBUCO.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Argentina_AER_CALBUCO.conf"
#! BOLIVIA
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Bolivia.conf"
#! MULTIANGLE
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_multRam.conf"
