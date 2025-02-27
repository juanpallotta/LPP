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
echo "Building LPP L0..."
echo ""
cd ./LPP_L0/
make -f makefile_lpp0 $1

sleep 1

echo ""
echo ""
echo ""
echo ""
echo "Building LPP L1..."
echo ""
cd ../LPP_L1/
make -f makefile_lpp1 $1

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
echo "Building LPP L2..."
echo ""
cd ../LPP_L2/
make -f makefile_lpp2 $1

echo ""
echo ""
exit 0

