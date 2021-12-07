#!/bin/bash

pkill -f lidarAnalysis_PDL0

sleep 1

echo ""
echo ""
echo ""
echo "Merging Lidar files..."

# FOR SAO PAULO LIDAR SIGNALS, RUN THIS FIRST INSIDE THE LIDAR FILES FOLDER.
# sudo sed -i 's/Sao Paul/SaoPaul/g' *.*

# source ../Lidar_Configuration_Files/dirFile.sh
PATH_IN_RAW="/mnt/Disk-1_8TB/Brazil/SPU/20210730/"
# PATH_IN_RAW="../signalsTest/Brazil/SPU/20210730/"

PATH_OUT_RAW="/mnt/Disk-1_8TB/Brazil/SPU/20210730/OUT/"
# PATH_OUT_RAW="../signalsTest/Brazil/SPU/20210730/OUT/"
# FILE_OUT_MERGED="merged_SCC_20200805.nc"
FILE_OUT_MERGED="merged_LALINET_20210730.nc"
# FILE_OUT_MERGED="merged_LALINET_20210730.nc"
PATH_FILE_OUT_MERGED=$PATH_OUT_RAW$FILE_OUT_MERGED

FILE_CONF="../Lidar_Configuration_Files/analysisParameters_PDL0_Brazil.conf"

	./lidarAnalysis_PDL0 $PATH_IN_RAW $PATH_FILE_OUT_MERGED $FILE_CONF

echo ""
echo ""
echo ""

exit 0
