#!/bin/bash

sleep 1

echo ""
echo ""
echo ""
echo "Syncking to /Release/Lidar_Conversion_Tool..."
echo ""
echo "Syncking source code..."
rsync -a -v ./* ../../Release/Lidar_Conversion_Tool/

echo ""
echo ""
echo ""
echo "Syncking to Release/libLidar..."
rsync -a -v ../libLidar/* ../../Release/libLidar/

echo ""
echo ""
echo ""
echo "Syncking to Release/signalsTest..."
rsync -a -v ../signalsTest/* ../../Release/signalsTest/

echo ""
echo ""
echo ""
echo ""

exit 0
