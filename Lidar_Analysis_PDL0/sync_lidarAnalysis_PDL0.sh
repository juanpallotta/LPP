#!/bin/bash

sleep 1

echo ""
echo ""
echo ""
echo "Syncking Lidar_Conversion_Tool..."
echo ""
echo "Syncking source code..."
rsync -a -v ./* lidaranalysis@lidaranalysis-AORUS:/home/lidaranalysis/LidarAnalysisCode/test/Lidar_Analysis_PDL0/

echo ""
echo ""
echo ""
echo "Syncking libraries..."
rsync -a -v ../libLidar/* lidaranalysis@lidaranalysis-AORUS:/home/lidaranalysis/LidarAnalysisCode/test/Lidar_Analysis_PDL0/

echo ""
echo ""
echo ""
echo ""

exit 0
