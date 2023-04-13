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
# PATH_IN="/mnt/Disk-1_8TB/Argentina/multiangulo/20151105/ramVert/"
# PATH_IN="/mnt/Disk-1_8TB/Argentina/Aeroparque/2015/20150426/"
# PATH_IN="/mnt/Disk-1_8TB/Argentina/Bariloche/20150504/"
# PATH_IN="/mnt/Disk-1_8TB/Argentina/Cordoba/Paper_Celeste/20190930/"
PATH_IN="/mnt/Disk-1_8TB/Argentina/Hector/20230411/signal/"

# PATH_IN="/mnt/Disk-1_8TB/Brazil/Manaus/2011/9/21/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/Manaus/2011/8/14/"
# PATH_IN="/mnt/Disk-1_8TB/Brazil/SPU/20210730/"

# CONFIGURATION FILES
# ! SIMULATIONS
# ! HOLGER
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Holger.conf"
# ! PAPPALARDO 2004
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Pappalardo.conf"
# ! BRAZIL
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_SPU.conf"
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Manaus.conf"
# ! ARGENTINA
# BARILOCHE
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Argentina_AER.conf"
# PILAR (CORDOBA), 2009-AGOSTO/SEPTIEMBRE
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Pilar_2009_AgoSep.conf"
# ! HECTOR
FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_Hector.conf"
#! MULTIANGLE
# FILE_CONF="/home/juan/LidarAnalysisCode/LPP/Lidar_Configuration_Files/analysisParameters_multRam.conf"
