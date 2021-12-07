#!/bin/bash

clear

sleep 1

echo "Building LidarAnalysis PDL1..."

LIDAR_MAIN_FILE='lidarAnalysis_PDL1.cpp' 
LIDAR_LIBS_FILES='../libLidar/CDataLevel_1.cpp ../libLidar/CNetCDF_Lidar.cpp ../libLidar/CMolecularData_USStd.cpp ../libLidar/libLidar.cpp ../libLidar/lidarMathFunc.cpp'
NETCDF_LIB='-L/usr/local/lib -lnetcdf -I/usr/local/include'
LIDAR_OUT=lidarAnalysis_PDL1

g++ -g ${LIDAR_MAIN_FILE} ${LIDAR_LIBS_FILES} ${NETCDF_LIB} -Wall -O -o ${LIDAR_OUT}
gdb lidarAnalysis_PDL1
# set disable-randomization off
# b lidarAnalysis_PDL1.cpp:82
# r ../signalsTest/Brazil/SPU/20210730/OUT/20210730_L0.nc ../signalsTest/Brazil/SPU/20210730/OUT/20210730_L0_L1.nc analysisParameters_Brazil.conf 
# r /mnt/Disk-1_8TB/Brazil/SPU/20210730/OUT/20210730_L0.nc /mnt/Disk-1_8TB/Brazil/SPU/20210730/OUT/20210730_L0_L1.nc analysisParameters_Brazil.conf 
# bt

exit 0

# GDB offers a big list of commands, however the following commands are the ones used most frequently:

# -----> bt - Prints a stack trace
# -----> p var - Prints the current value of the variable "var"

# b main - Puts a breakpoint at the beginning of the program
# b [filename]:[linenumber]
# b - Puts a breakpoint at the current line
# b N - Puts a breakpoint at line N
# b +N - Puts a breakpoint N lines down from the current line
# b fn - Puts a breakpoint at the beginning of function "fn"
# d N - Deletes breakpoint number N
# info break - list breakpoints
# r - Runs the program until a breakpoint or error
# c - Continues running the program until the next breakpoint or error
# f - Runs until the current function is finished
# s - Runs the next line of the program
# s N - Runs the next N lines of the program
# n - Like s, but it does not step into functions
# u N - Runs until you get N lines in front of the current line
# u - Goes up a level in the stack
# d - Goes down a level in the stack
# q - Quits gdb