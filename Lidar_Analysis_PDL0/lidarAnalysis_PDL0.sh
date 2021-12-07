#!/bin/bash

echo ""
echo ""
echo ""

# clear

# source ../Lidar_Configuration_Files/Path_File_Settings.sh

# SEARCH FOR SUB-FOLDERS INSIDE THE INPUT PATH
# DISCARD ANY FOLDER CALLED /OUT
PATH_IN_LIST=`find $PATH_IN_L0 -type d`
# for paths_In in $PATH_IN_LIST
# do
# 	# if [[ "$paths_In" == *"/2015"* ]] && [[ "$paths_In" != *"/OUT"* ]] && [[ "$paths_In" != *"355s"* ]]
# 	if [[ "$paths_In" != *"/OUT"* ]]
# 	then
# 		PATH_FILE_OUT_L0=$paths_In"OUT/"$(basename $paths_In)"_L0.nc"

		# echo "paths_In --> "$paths_In
		# echo "PATH_FILE_OUT_L0 --> "$PATH_FILE_OUT_L0
		# echo "FILE_CONF_L0 --> "$FILE_CONF_L0

		echo ------------------------------------------------------------ 
		echo " EN CARPETA PDL0"
			#   ./lidarAnalysis_PDL0 $paths_In $PATH_FILE_OUT_L0 $FILE_CONF_L0
		echo ""
		echo ------------------------------------------------------------ next file...
		echo ""
		echo ""
# 	fi
# done

echo ""
echo ""
echo ""

exit 0
