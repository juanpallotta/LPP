#!/bin/bash

echo ""
echo ""
echo ""

clear
pkill -f lidarAnalysis

source dirFile.sh

if [ -f $fileORfolder ]
then
	echo $fileORfolder "----> is a file"
	EXT="${fileORfolder##*.}"
		if [ "$EXT" = "nc" ]
		then
			echo "NetCDF file passed as an argument"
			echo "Run MergeTimeNetCDF"
			echo "Run lidarAnalysisNetCDF"
		else
			echo "Licel file passed as an argument ---> lidarAnalysisLicel"
		fi

elif [ -d $fileORfolder ]
then
	echo $fileORfolder "----> is a directory"
	echo "All files in the folder must have the same file format."

	LIDAR_FILES=$(find $DATA_DIR -maxdepth 1 -type f)
	for file in $LIDAR_FILES
	do
		EXT="${fileORfolder##*.}"
			if [ "$EXT" = "nc" ]
			then
				echo "NetCDF file in a directory ---> lidarAnalysisNetCDF"
			else
				echo "Licel file " $file " in a directory ---> mergeLidarLicelFiles --> loop through merged files ourside this loop"
			fi
	done
else
  echo $fileORfolder is neither a file nor directory
fi

echo ""
echo ""
echo ""

exit 0
