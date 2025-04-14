#!/bin/bash

# ---------------------------- EDIT CONFIGURATION START HERE
# DATA LEVEL TO PRODUCE IN THE RUN - "yes"/"no"
L0="no"
L1="yes"
L2="yes"

#* ABSOLUTE PATHS!!!
#* DO NOT USE SPACES!!!! 
# PATH_IN="/home/juan/LidarAnalysisCode/LPP/signalsTest/LPP_SIM/Vertical/lpp_sim_L0.nc"
# PATH_IN="/home/juan/LidarAnalysisCode/LPP/signalsTest/LPP_SIM/MultiAngle/lpp_sim_multiangle_L0.nc"
PATH_IN="/home/juan/LidarAnalysisCode/LPP/signalsTest/Brazil/SPU/20170928/signals/"

# PATH_IN="/mnt/Disk-1_8TB/Granada/20230214_nCh62/RS_20230214_0925/"
# PATH_IN="/mnt/Disk-1_8TB/Granada/20230830_nCh60/RS_20230830_0315/"
# PATH_IN="/mnt/Disk-1_8TB/Granada/20230830_nCh60/RS_20230830_0846/"
# PATH_IN="/mnt/Disk-1_8TB/Granada/20240916_nCh60/RS_20240916_0916/"
# PATH_IN="/mnt/Disk-1_8TB/Granada/20240603_nCh60/"

# PATH_IN="/mnt/Disk-1_8TB/Bolivia/20240826/2024_08_26_HR0935_A90_data_2CH_L0.nc"

# PATH_IN="/mnt/Disk-1_8TB/Colombia/Medellin/20220413/RS/"
# PATH_IN="/mnt/Disk-1_8TB/Colombia/Medellin/20220927/RS/"
# PATH_IN="/mnt/Disk-1_8TB/Colombia/Medellin/20230131/"
# PATH_IN="/mnt/Disk-1_8TB/Colombia/Medellin/20230206/"
# PATH_IN="/mnt/Disk-1_8TB/Colombia/Medellin/20230224/"

# PATH_IN="/mnt/Disk-1_8TB/Brazil/Manaus/2011/09/21/01/test/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/Manaus/2011/09/02/test/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/Manaus/2011/08/07/"

# PATH_IN="/mnt/Disk-1_8TB/Brazil/SPU/20200914/lidar_signals/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/SPU/20210730/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/SPU/2017_09_04_dt/LPP_2017_09_04_dt/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/SPU/2017_09_04_nt/LPP_2017_09_04_nt/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/SPU/2017_09_25_nt/LPP_2017_09_25_nt/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/SPU/2017_09_28_dt/LPP_2017_09_28_dt/"

# PATH_IN="/mnt/Disk-1_8TB/Brazil/Rio/20250307/"

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
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Medellin.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Cali.conf"
# ! BRAZIL
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_SPU_2017_09_04_dt.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_SPU_2017_09_04_nt.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_SPU_2017_09_25_nt.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_SPU_2017_09_28_dt.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Brazil_Rio.conf"

# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_SPU_PaperComparison.conf"
FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_SPU.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Manaus.conf"
# ! GRANADA - ALHAMBRA
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Granada.conf"
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
