#!/bin/bash

# ---------------------------- EDIT CONFIGURATION START HERE
# DATA LEVEL TO PRODUCE IN THE RUN - "yes"/"no", LOWERCASE
L0="yes"
L1="yes"
L2="no"

#* RELATIVE INPUT PATH TO THE TEST LIDAR FILES FOLDER INCLUDED IN THE REPOSITORY
#* DO NOT USE SPACES!!!! 
# PATH_IN="../signalsTest/Brazil/Manaus/"
# PATH_IN="../signalsTest/Brazil/SPU/20210730/"
PATH_IN="/mnt/Disk-1_8TB/Argentina/Aeroparque/volcan_Toga_AER/"

# CONFIGURATION FILES --> RELATIVE TO THE FOLDER WICH IS RUNNING THE ANALYSIS!!
# BRAZIL
# FILE_CONF_L0="../Lidar_Configuration_Files/analysisParameters_PDL0_Brazil.conf"
# FILE_CONF_L1_L2="../Lidar_Configuration_Files/analysisParameters_PDL1_2_Brazil.conf"

# ARGENTINA
FILE_CONF_L0="../Lidar_Configuration_Files/analysisParameters_PDL0_Argentina.conf"
FILE_CONF_L1_L2="../Lidar_Configuration_Files/analysisParameters_PDL1_2_Argentina.conf"


