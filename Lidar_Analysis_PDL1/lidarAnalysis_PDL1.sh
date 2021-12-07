#!/bin/bash

echo ""
echo ""
echo ""

# clear

# source ../Lidar_Configuration_Files/Path_File_Settings.sh

LIDAR_FILES_IN=`find $PATH_IN_L1 -type f -name "*.nc"`
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

		# DELETE PREVIOUS VERSION OF THE L1 FILE
		PATH_FILE_OUT_PDL1=${files_In_PDL1%.*}"_L1.nc"
	    if test -f "$PATH_FILE_OUT_PDL1"
	    then
			echo "Deleting previous version of data Level 1 --> ${PATH_FILE_OUT_PDL1}" 
			rm $PATH_FILE_OUT_PDL1 
	    fi

		# IF THERE IS A CONFIGURATION FILE INSIDE THE FOLDER PATH_IN_L1, IS SET AS FILE_CONF_L1_2
		INPUT_DIR=`dirname $files_In_PDL1`
		IS_CONF_FILE=`find $INPUT_DIR -type f -name "*.conf"`
		if [[ $IS_CONF_FILE != "" ]]
		then
			FILE_CONF_L1_2=$IS_CONF_FILE
		fi

		# RUNNING THE SOFTWARE
		echo ""
		echo ""
		echo "Analyzing --> $files_In_PDL1 using configuration file --> $FILE_CONF_L1_2" 
		echo ""
			# ./lidarAnalysis_PDL1 $files_In_PDL1 $PATH_FILE_OUT_PDL1 $FILE_CONF_L1_2  
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
