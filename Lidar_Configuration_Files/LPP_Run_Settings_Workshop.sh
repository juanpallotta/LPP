#!/bin/bash

# ---------------------------- EDIT CONFIGURATION START HERE
# DATA LEVEL TO PRODUCE IN THE RUN - "yes"/"no"
L0="yes"
L1="yes"
L2="yes"

#* ABSOLUTE PATHS!!!
#* DO NOT USE SPACES!!!! 
# PATH_IN="/home/juan/LidarAnalysisCode/LPP/signalsTest/Brazil/SPU/20210730/"
# PATH_IN="/home/juan/LidarAnalysisCode/LPP/signalsTest/LPP_SIM/lpp_sim_L0.nc"


# PATH_IN="/mnt/Disk-1_8TB/Bolivia/20240909/data/"
PATH_IN="/mnt/Disk-1_8TB/Colombia/Medellin/20220413/RS/"
# PATH_IN="/mnt/Disk-1_8TB/Colombia/Medellin/20220927/RS/"
# PATH_IN="/mnt/Disk-1_8TB/Colombia/Medellin/20230131/"
# PATH_IN="/mnt/Disk-1_8TB/Colombia/Medellin/20230206/"
# PATH_IN="/mnt/Disk-1_8TB/Colombia/Medellin/20230224/"

# PATH_IN="/mnt/Disk-1_8TB/Brazil/SPU/20200914/lidar_signals/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/SPU/20210730/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/SPU/2017_09_04_dt/LPP_2017_09_04_dt/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/SPU/2017_09_04_nt/LPP_2017_09_04_nt/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/SPU/2017_09_25_nt/LPP_2017_09_25_nt/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/SPU/2017_09_28_dt/LPP_2017_09_28_dt/"

# CONFIGURATION FILES
# ! SIMULATIONS
# ! LPP SIMULATION
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_LPP_simulation.conf"
# ! COLOMBIA 
FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Medellin.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Cali.conf"
# ! BRAZIL
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_SPU_2017_09_04_dt.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_SPU_2017_09_04_nt.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_SPU_2017_09_25_nt.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_SPU_2017_09_28_dt.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Brazil_Rio.conf"

# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_SPU.conf"

# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Pilar.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Argentina_BAR_CALBUCO.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Argentina_NQN_CALBUCO.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Argentina_AER_CALBUCO.conf"
#! BOLIVIA
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Bolivia.conf"
