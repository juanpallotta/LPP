#!/bin/bash

clear

rm 	lidarAnalysis_PDL0
pkill -f lidarAnalysis_PDL0

sleep 1

echo "Building lidarAnalysis_PDL0..."

LIDAR_MAIN_FILE='lidarAnalysis_PDL0.cpp' 
LIDAR_LIBS_FILES='../libLidar/CDataLevel_1.cpp ../libLidar/CMolecularData_USStd.cpp ../libLidar/libLidar.cpp ../libLidar/lidarMathFunc.cpp'
NETCDF_LIB_LIDAR='../libLidar/CNetCDF_Lidar.cpp'
NETCDF_LIB='-L/usr/local/lib -lnetcdf -I/usr/local/include'
LIDAR_OUT='lidarAnalysis_PDL0'

g++ -g ${LIDAR_MAIN_FILE} ${LIDAR_LIBS_FILES} ${NETCDF_LIB} ${NETCDF_LIB_LIDAR} -Wall -O -o ${LIDAR_OUT}
gdb lidarAnalysis_PDL0
# set disable-randomization off
# b lidarAnalysis_PDL0.cpp:427
# r "./signalsTest/Brazil_14/" "./signalsTest/Brazil_14/OUT/merged_SCC_20110714.nc" mergingParameters.conf
# r "/mnt/Disk-1_8TB/Brazil/SPU/20210730/" "/mnt/Disk-1_8TB/Brazil/SPU/20210730/OUT/merged_LALINET_20210730.nc" mergingParameters_Brazil.conf
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