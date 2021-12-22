#!/bin/bash

# ---------------------------- EDIT CONFIGURATION START HERE
# DATA LEVEL TO PRODUCE IN THE RUN - "yes"/"no", LOWERCASE
L0="no"
L1="yes"
L2="no"

#* RELATIVE INPUT PATH TO THE TEST LIDAR FILES FOLDER INCLUDED IN THE REPOSITORY
# PATH_IN="../signalsTest/Brazil/Manaus/2011/10/17/"
PATH_IN="../signalsTest/Brazil/SPU/20210730/"
#* ABSOLUTE INPUT PATH
# PATH_IN="/mnt/Disk-1_8TB/Brazil/SPU/20210730/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/Manaus/2011/10/17/"

# CONFIGURATION FILES --> ABSOLUTE PATHS OR ¡¡RELATIVE TO THE FOLDER WICH IS RUNNING THE ANALYSIS!!
FILE_CONF_L0="../Lidar_Configuration_Files/analysisParameters_PDL0_Brazil.conf"
# FILE_CONF_L0="../Lidar_Configuration_Files/analysisParameters_PDL0_Argentina.conf"
FILE_CONF_L1_L2="../Lidar_Configuration_Files/analysisParameters_PDL1_2_Brazil.conf"
# FILE_CONF_L1_L2="../Lidar_Configuration_Files/analysisParameters_PDL1_2_Argentina.conf"

# ---------------------------- EDIT CONFIGURATION ENDS HERE

