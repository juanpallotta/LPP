#!/bin/bash

# ---------------------------- EDIT CONFIGURATION START HERE
# DATA LEVEL TO PRODUCE IN THE RUN - "yes"/"no"
L0="yes"
L1="yes"
L2="yes"

#* ABSOLUTE PATHS!!!
#* DO NOT USE SPACES!!!! 
# PATH_IN="/home/juan/LidarAnalysisCode/LPP/signalsTest/LPP_SIM/Vertical/lpp_sim_L0.nc"
PATH_IN="/home/juan/LidarAnalysisCode/LPP/signalsTest/Brazil/SPU/20170928/signals/"
# PATH_IN="/home/juan/LidarAnalysisCode/LPP/signalsTest/Argentina/20240930/"
# PATH_IN="/home/juan/LidarAnalysisCode/LPP/signalsTest/Argentina/20241002/"

# PATH_IN="/home/juan/LidarAnalysisCode/LPP/signalsTest/LPP_SIM/MultiAngle/lpp_sim_multiangle_L0.nc"

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
FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_SPU.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Brazil_Rio.conf"
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
