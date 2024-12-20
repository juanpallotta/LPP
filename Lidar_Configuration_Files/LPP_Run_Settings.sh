#!/bin/bash

# ---------------------------- EDIT CONFIGURATION START HERE
# DATA LEVEL TO PRODUCE IN THE RUN - "yes"/"no"
L0="yes"
L1="yes"
L2="yes"

#* ABSOLUTE PATHS!!!
#* DO NOT USE SPACES!!!! 
# PATH_IN="/home/juan/LidarAnalysisCode/LPP/signalsTest/Brazil/SPU/20210730/"
# PATH_IN="/home/juan/LidarAnalysisCode/LPP/signalsTest/Holger_Sim/Original/"
# PATH_IN="/home/juan/LidarAnalysisCode/LPP/signalsTest/Pappalardo_2004/"

# PATH_IN="/mnt/Disk-1_8TB/Granada/20230214/RS_20230214_0925/"
# PATH_IN="/mnt/Disk-1_8TB/Granada/20230830/RS_20230830_0315/"
# PATH_IN="/mnt/Disk-1_8TB/Granada/20240916/RS_20240916_0916/"

# PATH_IN="/mnt/Disk-1_8TB/Colombia/Medellin/20220413/RS/"
# PATH_IN="/mnt/Disk-1_8TB/Colombia/Medellin/20220927/RS/"
# PATH_IN="/mnt/Disk-1_8TB/Colombia/Medellin/20230131/"
# PATH_IN="/mnt/Disk-1_8TB/Colombia/Medellin/20230206/"
# PATH_IN="/mnt/Disk-1_8TB/Colombia/Medellin/20230224/"

# PATH_IN="/mnt/Disk-1_8TB/Bolivia/20240906/data/"

# PATH_IN="/mnt/Disk-1_8TB/Brazil/Manaus/2011/09/21/01/test/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/Manaus/2011/09/02/test/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/Manaus/2011/08/15/"

# PATH_IN="/mnt/Disk-1_8TB/Brazil/SPU/2017_09_04_dt/LPP_2017_09_04_dt/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/SPU/2017_09_04_nt/LPP_2017_09_04_nt/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/SPU/2017_09_25_nt/LPP_2017_09_25_nt/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/SPU/2017_09_28_dt/LPP_2017_09_28_dt/"

# PATH_IN="/mnt/Disk-1_8TB/Brazil/SPU/20170904/day/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/SPU/2017_09_28_dt/LPP_2017_09_28_dt/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/SPU/20170409/night/testNight/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/SPU/20210730/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/SPU/20200914/lidar_signals_20200914/"

# PATH_IN="/mnt/Disk-1_8TB/Argentina/Cordoba/20230802/"
# PATH_IN="/mnt/Disk-1_8TB/Argentina/Cordoba/20190920/"
PATH_IN="/mnt/Disk-1_8TB/Argentina/Cordoba/0_Paper_Celeste/20190930/"
# PATH_IN="/mnt/Disk-1_8TB/Argentina/Bariloche/20150504/"

# PATH_IN="/mnt/Disk-1_8TB/Argentina/Calbuco/BAR/20150426/"
# PATH_IN="/mnt/Disk-1_8TB/Argentina/Calbuco/NQN/20150424/"
# PATH_IN="/mnt/Disk-1_8TB/Argentina/Calbuco/AER/20150421/"

# CONFIGURATION FILES
# ! SIMULATIONS
# ! HOLGER
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Holger.conf"
# ! PAPPALARDO 2004
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Pappalardo.conf"
# ! CHILE
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_ChileSN.conf"
# ! COLOMBIA 
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Medellin.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Cali.conf"
# ! BRAZIL
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_SPU_2017_09_04_dt.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_SPU_2017_09_04_nt.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_SPU_2017_09_25_nt.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_SPU_2017_09_28_dt.conf"

# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_SPU_PaperComparison.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_SPU.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Manaus.conf"
# ! GRANADA - ALHAMBRA
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Granada.conf"
# FILE_CONF="/mnt/c/Users/jpall/MEGA_LPP/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Granada.conf"
# ! ARGENTINA
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Argentina_MiniLidar.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Argentina_AER.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Argentina_BAR.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Argentina_COMR.conf"
FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Pilar_2019_AgoSep.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Pilar.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Hector.conf"

# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Argentina_BAR_CALBUCO.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Argentina_NQN_CALBUCO.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Argentina_AER_CALBUCO.conf"
#! BOLIVIA
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Bolivia.conf"
#! MULTIANGLE
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_multRam.conf"
