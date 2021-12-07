#!/bin/bash

clear

sleep 1

echo "Building lidarAnalysis_PDL0..."

make -f makefile_lidarAnalysis_PDL0

# LIDAR_MAIN_FILE='mergeLidarFiles.cpp' 
# LIDAR_LIBS_FILES='../libLidar/lidarDataTypes.hpp ../libLidar/libLidar.hpp ../libLidar/libLidar.cpp ../libLidar/lidarMathFunc.hpp ../libLidar/lidarMathFunc.cpp'
# NETCDF_LIB_LIDAR='../libLidar/CNetCDF_LIDAR.hpp ../libLidar/CNetCDF_LIDAR.cpp'
# NETCDF_LIB='-L/usr/local/lib -lnetcdf -I/usr/local/include'
# LIDAR_OUT='mergeLidarFiles'

# g++ ${LIDAR_MAIN_FILE} ${LIDAR_LIBS_FILES} ${NETCDF_LIB_LIDAR} ${NETCDF_LIB} -Wall -o ${LIDAR_OUT}

exit 0
