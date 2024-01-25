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

# PATH_IN="/mnt/Disk-1_8TB/Granada/20230214/RS_20230214_0925/test/"

# PATH_IN="/mnt/Disk-1_8TB/Brazil/Manaus/2011/09/21/01/test/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/Manaus/2011/09/02/test/"
PATH_IN="/mnt/Disk-1_8TB/Brazil/SPU/20170409/day/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/SPU/20170409/night/testNight/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/SPU/20210730/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/SPU/20200914/lidar_signals/test/"

# CONFIGURATION FILES

# ! SIMULATIONS
# ! HOLGER
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Holger.conf"
# ! PAPPALARDO 2004
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Pappalardo.conf"
# ! CHILE
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_ChileSN.conf"
# ! BRAZIL
FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_SPU_PaperComparison.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_SPU.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Manaus.conf"
# ! GRANADA - ALHAMBRA
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Granada.conf"
# ! ARGENTINA
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Argentina_MiniLidar.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Argentina_AER.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Argentina_BAR.conf"
#! PILAR (CORDOBA), 2009-AGOSTO/SEPTIEMBRE
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Pilar_2009_AgoSep.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Hector.conf"
#! MULTIANGLE
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_multRam.conf"
