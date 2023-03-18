#!/bin/bash

clear

# COMPILE

echo ""
echo ""
echo ""

# rm      lidarAnalysis_PDL1
make -f makefileLidarAnalysis_PDL2

sleep 3

# RUN

# ./runLidarAnalysis_PDL1.sh
./shBulkAnalysis_PDL2.sh

echo ""
echo ""
echo ""
