#!/bin/bash

# ---------------------------- EDIT CONFIGURATION START HERE
# DATA LEVEL TO PRODUCE IN THE RUN - "yes"/"no"
L0="yes"
L1="yes"
L2="yes"

#* ABSOLUTE PATHS!!!
#* DO NOT USE SPACES!!!! 
# PATH_IN="/mnt/Disk-1_8TB/Argentina/multiangulo/20151105/ramVert/"
# PATH_IN="/mnt/Disk-1_8TB/Argentina/Aeroparque/2022/20221102/"
# PATH_IN="/mnt/Disk-1_8TB/Argentina/Bariloche/20150504/"
# PATH_IN="/mnt/Disk-1_8TB/Argentina/Hector/20221206_sinAmp/data/LPP_OUT/data_L0_L1.nc"

# PATH_IN="/mnt/Disk-1_8TB/Brazil/Manaus/2011/9/22/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/SPU/20210730/LPP_OUT/20210730_L0.nc"
PATH_IN="/mnt/Disk-1_8TB/Brazil/SPU/20200914/lidar_signals/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/SPU/20200914/lidar_signals/LPP_OUT/lidar_signals_L0_L1.nc"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/SPU/20200914/lidar_signals/LPP_OUT/lidar_signals_L0_L1.nc"

# CONFIGURATION FILES
# ! HOLGER SIMULATION
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Holger.conf"
# ! BRAZIL
FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_SPU.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Manaus.conf"
# ! ARGENTINA
# BARILOCHE
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Argentina_BAR.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Argentina.conf"
# PILAR (CORDOBA), 2009-AGOSTO/SEPTIEMBRE
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Pilar_2009_AgoSep.conf"
# ! HECTOR
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Hector.conf"
#! MULTIANGLE
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_multRam.conf"
