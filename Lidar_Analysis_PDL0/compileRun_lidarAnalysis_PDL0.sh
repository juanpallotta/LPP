#!/bin/bash

clear

echo ""
echo ""
echo ""

# BUILD
rm   lidarAnalysis_PDL0
./compile_lidarAnalysis_PDL0.sh

# RUN
./run_lidarAnalysis_PDL0_Brazil.sh
# ./run_lidarAnalysis_PDL0_Argentina.sh

echo ""
echo ""
echo ""
