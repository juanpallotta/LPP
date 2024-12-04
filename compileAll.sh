#! /bin/bash

# clear

echo ""
echo ""
echo ""
echo ""
echo "Building LPP..."
echo ""

sleep 1

echo ""
echo ""
echo ""
echo ""
echo "Building lidarAnalysisPDL0..."
echo ""
cd ./Lidar_Analysis_PDL0/
make -f makefile_lidarAnalysis_PDL0 $1

sleep 1

echo ""
echo ""
echo ""
echo ""
echo "Building lidarAnalysisPDL1..."
echo ""
cd ../Lidar_Analysis_PDL1
make -f makefile_lidarAnalysis_PDL1 $1

sleep 1

# echo ""
# echo ""
# echo ""
# echo ""
# echo "Building make_cloudDB_LPP..."
# echo ""
# make -f makefile_make_CloudDB_LPP $1

# sleep 1

echo ""
echo ""
echo ""
echo ""
echo "Building lidarAnalysisPDL2..."
echo ""
cd ../Lidar_Analysis_PDL2
make -f makefile_lidarAnalysis_PDL2 $1

echo ""
echo ""
exit 0

