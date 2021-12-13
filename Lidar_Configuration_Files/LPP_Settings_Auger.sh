#!/bin/bash

# SHELL SCRIPT EXECUTED IN run_Auger_Analysis.sh NEEDED TO LOAD VARIABLES

# ---------------------------- EDIT CONFIGURATION START HERE
# DATA LEVEL TO PRODUCE IN THE RUN
export L0="yes"
export L1="no"
export L2="no"

# INPUT AND OUTPUT ABSOLUTE PATHS!!!!
PATH_IN="/mnt/Disk-1_8TB/Auger/LidarData/CO/2007/01/09/"
# PATH_IN='/home/juan/MEGAsync/procesamiento/LidarAnalysisCode/LPP/signalsTest/Auger/LidarData/CO/2007/01/09/'

# CONFIGURATION FILES --> ¡¡RELATIVE TO THE FOLDER WICH IS RUNNING THE ANALYSIS!!
# FILE_CONF_L0="../Lidar_Configuration_Files/analysisParameters_LL.conf"
# FILE_CONF_L0="../Lidar_Configuration_Files/analysisParameters_LM.conf"
# FILE_CONF_L0="../Lidar_Configuration_Files/analysisParameters_LA.conf"
FILE_CONF_L0="../Lidar_Configuration_Files/analysisParameters_Auger_CO_L0.conf"
# FILE_CONF_L1_L2="../Lidar_Configuration_Files/analysisParameters_Auger_LL_L1_L2.conf"
# FILE_CONF_L1_L2="../Lidar_Configuration_Files/analysisParameters_Auger_LM_L1_L2.conf"
# FILE_CONF_L1_L2="../Lidar_Configuration_Files/analysisParameters_Auger_LA_L1_L2.conf"
FILE_CONF_L1_L2="../Lidar_Configuration_Files/analysisParameters_Auger_CO_L1_L2.conf"

# ---------------------------- EDIT CONFIGURATION ENDS HERE

