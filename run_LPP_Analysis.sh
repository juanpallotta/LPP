#!/bin/bash

clear

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
PATH_TO_LPP=`pwd`
PATH_TO_L0=`dirname $(realpath $(find . -name lidarAnalysis_PDL0.cpp))`
PATH_TO_L1=`dirname $(realpath $(find . -name lidarAnalysis_PDL1.cpp))`
PATH_TO_L2=`dirname $(realpath $(find . -name lidarAnalysis_PDL2.cpp))`
PATH_TO_LPP_PLOT=`dirname $(realpath $(find .. -name LPP_Plots_L1.py))`

# SEARCH FOR THE LAST SUB-FOLDERS INSIDE THE INPUT PATH
cd $PATH_TO_L0
PATH_IN_LIST=`find $PATH_IN -type d -links 2`
if [[ "$PATH_IN_LIST" == *"/LPP_OUT"* ]] # DELETE FOLDER CALLED /LPP_OUT
then
    rm -r $PATH_IN_LIST 
    PATH_IN_LIST=`find $PATH_IN -type d -links 2`
fi

for paths_In in $PATH_IN_LIST
do
	# if [[ "$paths_In" == *"/2015"* ]] && [[ "$paths_In" != *"/OUT"* ]] && [[ "$paths_In" != *"355s"* ]]
	if [[ "$paths_In" != *"/LPP_OUT"* ]] # DISCARD ANY FOLDER CALLED /LPP_OUT
	then
        echo -e "\n------------------------------------------------------------\n" 
        # DEFINING THE INPUTS AND OUTPUTS PATHS
            PATH_OUT_L0=$paths_In"LPP_OUT/"
            echo -e "\nOutput folder with the LPP products: "$PATH_OUT_L0
            if test -d "$PATH_OUT_L0"
            then 
                    echo -e "\nThe output folder already exist...\n" 
                else
                    echo -e "\nCreating the output folder $PATH_OUT_L0 ...\n" 
                    mkdir $PATH_OUT_L0
            fi
            cd $PATH_TO_LPP
		    PATH_FILE_OUT_L0=$PATH_OUT_L0$(basename $paths_In)"_L0.nc"

            PATH_FILE_IN_L1=$PATH_FILE_OUT_L0
            PATH_FILE_OUT_L1=${PATH_FILE_IN_L1%.*}"_L1.nc"

            PATH_FILE_IN_L2=$PATH_FILE_OUT_L1
            PATH_FILE_OUT_L2=${PATH_FILE_IN_L2%.*}"_L2.nc"

        if [[ "$L0" == "yes" ]]
        then
            if test -f "$PATH_FILE_OUT_L0"
            then 
                echo "Deleting previous version of data Level 0 --> ${PATH_FILE_OUT_L0}" 
                rm $PATH_FILE_OUT_L0 
                sleep 1
            fi
    echo -e "Running PDL0: \n ./lidarAnalysis_PDL0 "$paths_In" "$PATH_FILE_OUT_L0" "$FILE_CONF
    cd $PATH_TO_L0

    ./lidarAnalysis_PDL0 $paths_In $PATH_FILE_OUT_L0 $FILE_CONF $PATH_DARK_FILES $OVERLAP_FILE

        fi

        if [[ "$L1" == "yes" ]] || [[ "$L2" == "yes" ]]
        then
            echo ------------------------------------------------------------ 
            echo ""
            echo ""
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
                echo -e "Running PDL1 \n ./lidarAnalysis_PDL1 "$PATH_FILE_IN_L1" "$PATH_FILE_OUT_L1 $FILE_CONF
    cd $PATH_TO_L1
    ./lidarAnalysis_PDL1 $PATH_FILE_IN_L1 $PATH_FILE_OUT_L1 $FILE_CONF  
            else
                echo -e "L1: input file "$PATH_FILE_IN_L1 "doesn't exist. Set L0=yes in LPP_settings.sh file"
            fi
            rm $PATH_FILE_IN_L1 
    #   PLOTTING
            echo -e "\n\nGenerating plots for L1..."
            PATH_FILE_TO_PLOT=${PATH_TO_LPP_PLOT%.*}"/LPP_Plots_L1.py"
            python3 $PATH_FILE_TO_PLOT $PATH_FILE_OUT_L1
        fi

        if [[ "$L2" == "yes" ]]
        then
            echo ------------------------------------------------------------ 
            echo ""
            echo ""
            # DELETE PREVIOUS VERSION OF THE L2 FILE (IF EXIST)
            if test -f "$PATH_FILE_OUT_L2"
            then
                echo "Deleting previous version of data Level 2 --> ${PATH_FILE_OUT_L2}" 
                rm $PATH_FILE_OUT_L2 
                sleep 1
            fi
            # CHECK IF THE INPUT FILE EXIST
            if [[ -f $PATH_FILE_IN_L2 ]]
            then

#!  AERONET DOWNLOADER HERE.

    echo -e "Running PDL2 \n ./lidarAnalysis_PDL2 "$PATH_FILE_IN_L2" "$PATH_FILE_OUT_L2 $FILE_CONF
    cd $PATH_TO_L2
    ./lidarAnalysis_PDL2 $PATH_FILE_IN_L2 $PATH_FILE_OUT_L2 $FILE_CONF
            else
                echo -e "\n L2: input file "$PATH_FILE_IN_L2 " doesn't exist. Set L1=yes in LPP_settings.sh file"
            fi
            # rm $PATH_FILE_IN_L2
    #   PLOTTING
            echo -e "\n\nGenerating plots for L2..."
            PATH_FILE_TO_PLOT=${PATH_TO_LPP_PLOT%.*}"/LPP_Plots_L2.py"
            python3 $PATH_FILE_TO_PLOT $PATH_FILE_OUT_L2 4
        fi

		echo ""
		echo ------------------------------------------------------------ next file...
		echo ""
		echo ""
	fi
done

echo ""
echo ""
echo ""

exit 0
