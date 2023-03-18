#!/bin/bash

sleep 1

echo ""
echo ""
echo ""
echo "Runnig lidarAnalysis_PDL1..."

# source ../Lidar_Configuration_Files/dirFile.sh
# INPUT PATHS AND FILE FOR lidarAnalysis_PDL1
# PATH_IN_PDL1="../signalsTest/Brazil/SPU/20210730/OUT/"
PATH_IN_PDL2="/mnt/Disk-1_8TB/Brazil/SPU/20210730/OUT/"
FILE_IN_PDL2="20210730_L0_L1.nc"
PATH_FILE_IN_PDL1=$PATH_IN_PDL1$FILE_IN_PDL1
# OUTPUT PATHS AND FILE FOR lidarAnalysis_PDL1
PATH_OUT_PDL1=$PATH_IN_PDL1
# FILE_OUT_PDL1="merged_20180619_PDL1.nc"
FILE_OUT_PDL1="20210730_L0_L1_L2.nc"
PATH_FILE_OUT_PDL1=$PATH_OUT_PDL1$FILE_OUT_PDL1

FILE_CONF="../Lidar_Analysis_PDL1/analysisParameters_Brazil.conf"

	cp $PATH_FILE_IN_PDL2 $PATH_FILE_OUT_PDL2
	./lidarAnalysis_PDL2 $PATH_FILE_IN_PDL1 $PATH_FILE_OUT_PDL1 $FILE_CONF

echo ""
echo ""
echo ""

exit 0
