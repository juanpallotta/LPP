#!/bin/bash

pkill -f mergeLidarFiles

sleep 1

echo ""
echo ""
echo ""
echo "Runnig lidarAnalysis_PDL1..."

# source ../Lidar_Configuration_Files/dirFile.sh
# INPUT PATHS AND FILE FOR lidarAnalysis_PDL1
# PATH_IN_PDL1="../signalsTest/Brazil/SPU/20210730/OUT/"
PATH_IN_PDL1="/mnt/Disk-1_8TB/Brazil/SPU/20210730/OUT/"
FILE_IN_PDL1="20210730_L0.nc"
PATH_FILE_IN_PDL1=$PATH_IN_PDL1$FILE_IN_PDL1
# OUTPUT PATHS AND FILE FOR lidarAnalysis_PDL1
PATH_OUT_PDL1=$PATH_IN_PDL1
# FILE_OUT_PDL1="merged_20180619_PDL1.nc"
FILE_OUT_PDL1="20210730_L0_L1.nc"
PATH_FILE_OUT_PDL1=$PATH_OUT_PDL1$FILE_OUT_PDL1

FILE_CONF="../Lidar_Configuration_Files/analysisParameters_PDL1_2_Brazil.conf"

	cp $PATH_FILE_IN_PDL1 $PATH_FILE_OUT_PDL1
	./lidarAnalysis_PDL1 $PATH_FILE_IN_PDL1 $PATH_FILE_OUT_PDL1 $FILE_CONF

echo ""
echo ""
echo ""

exit 0
