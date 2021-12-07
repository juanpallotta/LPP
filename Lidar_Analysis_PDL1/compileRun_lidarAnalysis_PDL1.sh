#!/bin/bash

clear

# COMPILE

echo ""
echo ""
echo ""

# rm      lidarAnalysis_PDL1
make -f makefile_lidarAnalysis_PDL1

# RUN

# ./runLidarAnalysis_PDL1.sh
./shBulkAnalysis_PDL1.sh

echo ""
echo ""
echo ""
