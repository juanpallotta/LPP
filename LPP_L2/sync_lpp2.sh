#!/bin/bash

sleep 1

echo ""
echo ""
echo ""
echo "Syncking Lidar_Analysis_PDL1..."

echo ""
echo "Syncking source code..."
# rsync -a -v ./* lidaranalysis@lidaranalysis-AORUS:/home/lidaranalysis/LidarAnalysisCode/test/Lidar_Analysis_PDL1/
rsync -a -v ./* lidaranalysis@192.168.0.9:/home/lidaranalysis/LidarAnalysisCode/test/Lidar_Analysis_PDL1/

echo ""
echo ""
echo ""
echo "Syncking libraries..."
#rsync -a -v ../libLidar/* lidaranalysis@lidaranalysis-AORUS:/home/lidaranalysis/LidarAnalysisCode/test/libLidar/
rsync -a -v ../libLidar/* lidaranalysis@192.168.0.9:/home/lidaranalysis/LidarAnalysisCode/test/libLidar/

echo ""
echo ""
echo ""
echo ""

exit 0
