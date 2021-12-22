#!/bin/bash

sleep 1

echo ""
echo ""
echo ""
echo "Syncking Lidar_Conversion_Tool..."

rsync -a -v ./* lidaranalysis@lidaranalysis-AORUS:/home/lidaranalysis/LidarAnalysisCode/test/signalsTest/

echo ""
echo ""
echo ""

exit 0
