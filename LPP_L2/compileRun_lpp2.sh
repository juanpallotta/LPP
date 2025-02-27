#!/bin/bash

clear

# COMPILE

echo ""
echo ""
echo ""

# rm      lpp1
make -f makefilelpp2

sleep 3

# RUN

# ./runlpp1.sh
./shBulkAnalysis_PDL2.sh

echo ""
echo ""
echo ""
