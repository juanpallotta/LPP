#!/bin/bash

clear
pkill -f augerLidarAnalysis_TAM

source dirFile.sh
LIDAR_FILES=`find $DATA_DIR/ -type f | grep .root`

for file in $LIDAR_FILES
do
	echo ------------------------------------------------------------ $file
	./augerLidarAnalysis_TAM "$file" "analysisParametersLongRange.dat" bulk
	echo ------------------------------------------------------------ $file OK, next file...

done
		site=ch
		./shLidarVSclf.sh $DATA_DIR $site

# OCTAVE
# find $DATA_DIR/ -type f | grep clf_ > clf_files.dat
# find $DATA_DIR/ -type f | grep .root_OUT.txt > lidar_files.dat

# octave clf_lidar_time.m

exit 0
