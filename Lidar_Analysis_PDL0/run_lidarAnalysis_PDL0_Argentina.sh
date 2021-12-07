#!/bin/bash

pkill -f mergeLidarFiles

sleep 1

echo ""
echo ""
echo ""
echo "Merging Lidar files..."

# source ../Lidar_Configuration_Files/dirFile.sh
# PATH_IN_RAW="../signalsTest/Argentina/20180619/"
# PATH_OUT_RAW="../signalsTest/Argentina/20180619/OUT/"
PATH_IN_RAW="/mnt/Disk-1_8TB/Argentina/Aeroparque/20150503/"
PATH_OUT_RAW="/mnt/Disk-1_8TB/Argentina/Aeroparque/20150503/OUT/"

FILE_OUT_MERGED="AER_LALIINET_20150503.nc"
PATH_FILE_OUT_MERGED=$PATH_OUT_RAW$FILE_OUT_MERGED

FILE_CONF="../Lidar_Configuration_Files/analysisParameters_PDL0_Argentina.conf"

	./mergeLidarFiles $PATH_IN_RAW $PATH_FILE_OUT_MERGED $FILE_CONF

echo ""
echo ""
echo ""

exit 0
