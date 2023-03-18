#!/bin/bash

sleep 1

echo ""
echo ""
echo ""
echo "Runnig lidarAnalysis_PDL1..."

echo "PATH_OUT_RAW es: "
echo $PATH_OUT_RAW

# source ../Lidar_Configuration_Files/dirFile.sh
# INPUT PATHS AND FILE FOR lidarAnalysis_PDL1
# PATH_IN_PDL1="../signalsTest/Argentina/20180619/OUT/"
PATH_IN_PDL1="/mnt/Disk-1_8TB/Argentina/Aeroparque/20150504/OUT/"
FILE_IN_PDL1="AER_LALINET_20150504.nc"
PATH_FILE_IN_PDL1=$PATH_IN_PDL1$FILE_IN_PDL1

# OUTPUT PATHS AND FILE FOR lidarAnalysis_PDL1
PATH_OUT_PDL1=$PATH_IN_PDL1
FILE_OUT_PDL1="AER_LALINET_20150504_PDL1.nc"
PATH_FILE_OUT_PDL1=$PATH_OUT_PDL1$FILE_OUT_PDL1

FILE_CONF="../Lidar_Analysis_PDL1/analysisParameters_Brazil.conf"

	./lidarAnalysis_PDL1 $PATH_FILE_IN_PDL1 $PATH_FILE_OUT_PDL1 $FILE_CONF

echo ""
echo ""
echo ""

exit 0
