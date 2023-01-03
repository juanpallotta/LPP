#!/bin/bash

clear

sleep 1

echo "Building lidarAnalysisPDL1..."

make -f makefile_lidarAnalysis_PDL1_Debug

gdb lidarAnalysis_PDL1

# set disable-randomization off
# b lidarAnalysis_PDL0.cpp:233
# r "/mnt/Disk-1_8TB/Brazil/SPU/20200914/lidar_signals/LPP_OUT/lidar_signals_L0.nc" "/mnt/Disk-1_8TB/Brazil/SPU/20200914/lidar_signals/LPP_OUT/lidar_signals_L0_L1.nc" "../Lidar_Configuration_Files/analysisParameters_SPU.conf"

# Auger
# r "/home/juan/MEGAsync/procesamiento/LidarAnalysisCode/Auger_Data_Conversion_Tool/signalsTest/Auger/CO/2007/07/20/LPP_OUT/lidar-ch-20070720-044305-R16418.root_L0.nc" "/home/juan/MEGAsync/procesamiento/LidarAnalysisCode/Auger_Data_Conversion_Tool/signalsTest/Auger/CO/2007/07/20/LPP_OUT/lidar-ch-20070720-044305-R16418.root_L0_L1.nc" "/home/juan/MEGAsync/procesamiento/LidarAnalysisCode/Auger_Data_Conversion_Tool/Lidar_Configuration_Files/analysisParameters_Auger_CO_L1_L2.conf"

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