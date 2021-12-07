#!/bin/bash

echo ""
echo ""
echo ""

# clear

# source ../Lidar_Configuration_Files/dirFile.sh
# DATA_DIR="/mnt/Disk-1_8TB/Argentina/Aeroparque"
# DATA_DIR="/mnt/Disk-1_8TB/Brazil/Manaus/2011/10/24/"
DATA_DIR="/mnt/Disk-1_8TB/Brazil/SPU/20210730/"
LIDAR_FILES_IN=`find $DATA_DIR -type f -name "*.nc"`
FILE_CONF="../Lidar_Configuration_Files/analysisParameters_PDL1_2_Brazil.conf"
# FILE_CONF="../Lidar_Configuration_Files/analysisParameters_PDL1_2_Argentina.conf"

for files_In_PDL1 in $LIDAR_FILES_IN
do
	# if [[ "$files_In_PDL1" == *"20150503"* ]] && [[ "$files_In_PDL1" != *"_PDL1.nc"* ]]  # ARGENTINA
	if [[ "$files_In_PDL1" != *"_L1.nc"* ]] && [[ "$files_In_PDL1" != *"_L2.nc"* ]] 
	then
		echo ""
		echo ""
		echo "_____________________________________________________________________________________________________________________________"
		echo ""
		echo ""
		
		PATH_FILE_OUT_PDL1=${files_In_PDL1%.*}"_L1.nc"

		# DELETE PREVIOUS VERSION OF THE L1 FILE
	    if test -f "$PATH_FILE_OUT_PDL1"
	    then
			echo "Deleting previous version of data Level 1 --> ${PATH_FILE_OUT_PDL1}" 
			rm $PATH_FILE_OUT_PDL1 
	    fi

		# CONFIGURATION FILE
		INPUT_DIR=`dirname $files_In_PDL1`
		IS_CONF_FILE=`find $INPUT_DIR -type f -name "*.conf"`
		if [[ $IS_CONF_FILE != "" ]]
		then
			FILE_CONF=$IS_CONF_FILE
		fi

		# RUNNING THE SOFTWARE
		echo ""
		echo ""
		echo "Analyzing --> $files_In_PDL1 using configuration file --> $FILE_CONF" 
		echo ""
			./lidarAnalysis_PDL1 $files_In_PDL1 $PATH_FILE_OUT_PDL1 $FILE_CONF  
		echo ""
		echo "PATH_FILE_OUT_PDL1 --> $PATH_FILE_OUT_PDL1"
		echo ""
		echo "------------------------------------------------------------ Done, next file..."
		echo ""
		echo ""
	fi
done

echo ""
echo ""
echo ""

exit 0
