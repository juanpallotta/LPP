#!/bin/bash

# clear

echo ""
echo ""
echo ""

if [ $# -eq 0 ]
    then # NO LPP_Settings.sh FILE PASSED AS ARGUMENTS.
        source ./Lidar_Configuration_Files/LPP_Settings.sh
    else # A SETTINGS FILE WAS PASSED AS ARGUMENTS.
        source $1
fi

# GETTING THE ABSOLUTE PATHS TO EACH EXECUTABLE MODULE
PATH_TO_LPP=`pwd`'/'
PATH_TO_L0=`dirname $(realpath $(find . -name lidarAnalysis_PDL0.cpp))`'/'
PATH_TO_L1=`dirname $(realpath $(find . -name lidarAnalysis_PDL1.cpp))`'/'
PATH_TO_L2=`dirname $(realpath $(find . -name lidarAnalysis_PDL2.cpp))`'/'
PATH_TO_LPP_PLOT=`dirname $(realpath $(find .. -name LPP_Plots_L1.py))`'/'

if [[ -d $PATH_IN ]]
then # PATH_IN is a directory, so L0=yes must be set
    if [[ "${L0,,}" == "no" ]]
    then
        echo -e "\nPATH_IN: $PATH_IN  --> is a DIRECTORY, so L0 must be set as 'yes' in LPP_Settings.sh"
        exit 0
    else # L0="yes" and PATH_IN is a directory
        PATH_IN_LIST=`find $PATH_IN -type d -links 2`  # SEARCH FOR THE LAST SUB-FOLDERS INSIDE THE INPUT PATH
        echo -e "\nPATH_IN_LIST:\n$PATH_IN_LIST"
        if [[ "${L2,,}" == "yes" ]] && [[ "${L1,,}" == "no" ]]
        then
            L1="yes"
        fi
    fi
else # if [[ -d $PATH_IN ]] --> PATH_IN is a file --> L0 must be unchecked and either L1 or L2 must be checked

    if [[ "${L0,,}" == "yes" ]] 
    then
        echo -e "\n*WRONG CONFIGURATION*\nIf PATH_IN is a FILE --> L0 must be set as 'no' in LPP_Settings.sh.PATH_IN: $PATH_IN\n"
        exit 0
    fi
    
    if [[ "${L1,,}" == "yes" ]] || [[ "${L2,,}" == "yes" ]]
    then
        PATH_IN_LIST=$PATH_IN
        echo -e "\nPATH_IN_LIST: $PATH_IN_LIST --> is a FILE"
    fi
fi

# L0=yes
#   --> A FOLDER MUST BE PASSED AS ARGUMENT 
#   --> THE FILENAMES ARE AUTOMATICALLY GENERATED
# L0=NO AND L1=yes AND/OR L2=yes
#   --> A FILE MUST BE PASSED AS ARGUMENT 
#   --> THE FILENAME OF THE LAST DATA LEVEL IS AUTOMATICALLY GENERATED

# THE FOR LOOP ONLY WORKS WHEN L0 IS SET
echo ""
echo "Entering the loop..."
echo ""

PROCES_SENTRY=1
for paths_In in $PATH_IN_LIST
do
PROCES_SENTRY=1
    echo -e "\n Analyzing: "$paths_In" \n"
	if [[ "${L0,,}" == "yes" ]]
    then
        if [[ "$paths_In" != *"/LPP_OUT"* ]] # DISCARD ANY FOLDER WITH 'LPP_OUT' IN ITS NAME
        then
            echo -e "\n------------------------------------------------------------\n" 
            # DEFINING THE INPUTS AND OUTPUTS PATHS
            PATH_IN_L0=$paths_In"/" # paths_In IS A FOLDER
            PATH_OUT_L0=$PATH_IN_L0"LPP_OUT/"
            echo -e "\nCreating the output folder with the LPP products: "$PATH_OUT_L0
            mkdir $PATH_OUT_L0
		    PATH_FILE_OUT_L0=$PATH_OUT_L0$(basename $paths_In)"_L0.nc"

            PATH_FILE_IN_L1=$PATH_FILE_OUT_L0
            PATH_FILE_OUT_L1=${PATH_FILE_IN_L1%.*}"_L1.nc"

            PATH_FILE_IN_L2=$PATH_FILE_OUT_L1
            PATH_FILE_OUT_L2=${PATH_FILE_IN_L2%.*}"_L2.nc"
        else # if [[ "$paths_In" != *"/LPP_OUT"* ]]
            echo -e "\nFolder: $paths_In --> skipped because contain a LPP_OUT string in its path."
            PROCES_SENTRY=0
        fi # if [[ "$paths_In" != *"/LPP_OUT"* ]]
    else  # if [[ "${L0,,}" == "yes" ]]       L0=no
        if [[ "${L1,,}" == "yes" ]] 
        then    # L0=no L1=yes L2=?
            PATH_FILE_IN_L1=$paths_In # paths_In IS A FILE
            PATH_FILE_OUT_L1=${paths_In%.*}"_L1.nc"
            PATH_FILE_OUT_L1_CLOUD_DATA=${PATH_FILE_OUT_L1%.*}"_CLOUD_DATA.dat"
            echo ""
            echo PATH_FILE_IN_L1: $PATH_FILE_IN_L1
            echo PATH_FILE_OUT_L1: $PATH_FILE_OUT_L1
            echo ""
                if [[ "${L2,,}" == "yes" ]] 
                then # L0=no L1=yes L2=yes
                    PATH_FILE_IN_L2=$PATH_FILE_OUT_L1
                    PATH_FILE_OUT_L2=${PATH_FILE_IN_L2%.*}"_L2.nc"
                    echo ""
                    echo PATH_FILE_IN_L2: $PATH_FILE_IN_L2
                    echo PATH_FILE_OUT_L2: $PATH_FILE_OUT_L2
                    echo ""
                fi
        else
            if [[ "${L2,,}" == "yes" ]] 
            then # L0=no L1=no L2=yes
                PATH_FILE_IN_L2=$paths_In # paths_In IS A FILE
                PATH_FILE_OUT_L2=${PATH_FILE_IN_L2%.*}"_L2.nc"
                echo ""
                echo PATH_FILE_IN_L2: $PATH_FILE_IN_L2
                echo PATH_FILE_OUT_L2: $PATH_FILE_OUT_L2
                echo ""
            fi
        fi

	fi # if [[ "${L0,,}" == "yes" ]]


# INPUT AND OUTPUTS FILES ARE CONFIGURED PROPERLY ############################################################################
# RUN THE ACTIVE MODULES                          ############################################################################

if [ $PROCES_SENTRY == 1 ]
then
        if [[ "${L0,,}" == "yes" ]] 
        then
            echo -e "\n\nRunning PDL0: \n ./lidarAnalysis_PDL0 "$PATH_IN_L0" "$PATH_FILE_OUT_L0" "$FILE_CONF"\n\n"
            cd $PATH_TO_L0
            ./lidarAnalysis_PDL0 $PATH_IN_L0 $PATH_FILE_OUT_L0 $FILE_CONF 
        fi

        if [[ "${L1,,}" == "yes" ]]
        then
            # DELETE PREVIOUS VERSION OF THE L1 FILE (IF EXIST)
            if test -f "$PATH_FILE_OUT_L1"
            then 
                echo "Deleting previous version of data Level 1 --> ${PATH_FILE_OUT_L1}" 
                rm $PATH_FILE_OUT_L1 
                sleep 1
            fi

            # CHECK IF THE INPUT FILE EXIST
            if [[ -f $PATH_FILE_IN_L1 ]]
            then
                echo -e "\n\nRunning PDL1 \n ./lidarAnalysis_PDL1 "$PATH_FILE_IN_L1" "$PATH_FILE_OUT_L1 $FILE_CONF
                cd $PATH_TO_L1
                ./lidarAnalysis_PDL1 $PATH_FILE_IN_L1 $PATH_FILE_OUT_L1 $FILE_CONF

                echo -e "\n\nRunning make_CloudDB_LPP \n ./make_CloudDB_LPP "$PATH_FILE_OUT_L1" "$PATH_FILE_OUT_L1_CLOUD_DATA" "$FILE_CONF
                ./make_CloudDB_LPP $PATH_FILE_OUT_L1 $PATH_FILE_OUT_L1_CLOUD_DATA $FILE_CONF

                #   PLOTTING
                PATH_FILE_TO_PLOT=${PATH_TO_LPP_PLOT%.*}"LPP_Plots_L1.py"
                echo -e "\n\nGenerating plots for L1...\npython3 "$PATH_FILE_TO_PLOT" "$PATH_FILE_OUT_L1
                python3 $PATH_FILE_TO_PLOT $PATH_FILE_OUT_L1
            else # if [[ -f $PATH_FILE_IN_L1 ]]
                echo -e "L1: input file "$PATH_FILE_IN_L1 "doesn't exist. Set L0=yes in LPP_settings.sh file"
            fi # if [[ -f $PATH_FILE_IN_L1 ]]
            # rm $PATH_FILE_IN_L1 
        fi # if [[ "${L1,,}" == "yes" ]]

        if [[ "${L2,,}" == "yes" ]]
        then
            if test -f "$PATH_FILE_OUT_L2"
            then
                echo "Deleting previous version of data Level 2 --> ${PATH_FILE_OUT_L2}" 
                rm $PATH_FILE_OUT_L2 
                sleep 1
            fi # if test -f "$PATH_FILE_OUT_L2"
            # CHECK IF THE INPUT FILE EXIST
            if [[ -f $PATH_FILE_IN_L2 ]]
            then
                #!  AERONET DOWNLOADER HERE.

                echo -e "\n\nRunning PDL2 \n ./lidarAnalysis_PDL2 "$PATH_FILE_IN_L2" "$PATH_FILE_OUT_L2 $FILE_CONF
                cd $PATH_TO_L2
                ./lidarAnalysis_PDL2 $PATH_FILE_IN_L2 $PATH_FILE_OUT_L2 $FILE_CONF

                #   PLOTTING
                echo -e "\n\nGenerating plots for L2..."
                PATH_FILE_TO_PLOT=${PATH_TO_LPP_PLOT%.*}"LPP_Plots_L2.py"
                python3 $PATH_FILE_TO_PLOT $PATH_FILE_OUT_L2 0

            else # if [[ -f $PATH_FILE_IN_L2 ]]
                echo -e "\n L2: input file "$PATH_FILE_IN_L2 " doesn't exist. Set L1=yes in LPP_settings.sh file"
            fi # if [[ -f $PATH_FILE_IN_L2 ]]
            # rm $PATH_FILE_IN_L2
        fi # if [[ "${L2,,}" == "yes" ]]

echo ""
echo ------------------------------------------------------------ next file...
echo ""
echo ""
fi # if [ $PROCES_SENTRY == 1 ]

done # for paths_In in $PATH_IN_LIST do

echo ""
echo "Loop ended."
echo ""

exit 0
