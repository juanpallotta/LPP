#!/bin/bash

pkill -f lidarAnalysis_PDL0

sleep 1

echo ""
echo ""
echo ""
echo "Merging Lidar files..."

# FOR SAO PAULO LIDAR SIGNALS, RUN THIS FIRST INSIDE THE LIDAR FILES FOLDER.
# sudo sed -i 's/Sao Paul/SaoPaul/g' *.*

# PATH_IN_RAW="../signalsTest/Brazil/20110714/"
PATH_IN_RAW="../signalsTest/Brazil/SPU/20210730/"
# PATH_IN_RAW="../signalsTest/Argentina/20200805/"
# PATH_OUT_RAW="../signalsTest/Brazil/20110714/OUT/"
PATH_OUT_RAW="../signalsTest/Brazil/SPU/20210730/OUT/"
# PATH_OUT_RAW="../signalsTest/Argentina/20200805/OUT/"
# FILE_OUT_MERGED="merged_LALINET_20200805.nc"
# FILE_OUT_MERGED="merged_SCC_20200805.nc"
# FILE_OUT_MERGED="merged_LALINET_20110714.nc"
FILE_OUT_MERGED="merged_LALINET_20210730.nc"
PATH_FILE_OUT_MERGED=$PATH_OUT_RAW$FILE_OUT_MERGED

	./lidarAnalysis_PDL0 $PATH_IN_RAW $PATH_FILE_OUT_MERGED "mergingParameters_Argentina.conf"

echo ""
echo ""
echo ""

exit 0
