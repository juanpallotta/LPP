#!/bin/bash

echo ""
echo ""
echo ""

clear

# source ../Lidar_Configuration_Files/dirFile.sh
# DATA_DIR="/mnt/Disk-1_8TB/Argentina/Aeroparque"
DATA_DIR="/mnt/Disk-1_8TB/Brazil/SPU/20210730"
# DATA_DIR="/mnt/Disk-1_8TB/Brazil/Manaus/2011/10/24/"
PATH_IN_RAW=`find $DATA_DIR -type d`

FILE_CONF="../Lidar_Configuration_Files/analysisParameters_PDL0_Brazil.conf"
# FILE_CONF="../Lidar_Configuration_Files/analysisParameters_PDL0_Argentina.conf"

for paths_In in $PATH_IN_RAW
do
	# if [[ "$paths_In" == *"/2015"* ]] && [[ "$paths_In" != *"/OUT"* ]] && [[ "$paths_In" != *"355s"* ]]
	if [[ "$paths_In" != *"/OUT"* ]]
	then
		PATH_FILE_OUT_MERGED=$paths_In"/OUT/"$(basename $paths_In)"_L0.nc"

		echo ------------------------------------------------------------ 
		echo ""
			  ./lidarAnalysis_PDL0 $paths_In $PATH_FILE_OUT_MERGED $FILE_CONF
		echo ""
		echo ------------------------------------------------------------ next file...
		echo ""
		echo ""
	fi
done

echo ""
echo ""
echo ""

exit 0
