#!/bin/bash

clear

echo ""
echo ""
echo ""

source ./Lidar_Configuration_Files/LPP_Settings.sh

# SEARCH FOR SUB-FOLDERS INSIDE THE INPUT PATH
cd ./Lidar_Analysis_PDL0/ # IF PATH_IN IS A RELATIVE PATH, IT SHOULD BE RELATIVE TO TO THE EXECUTABLE FILE. SO, PATH_IN_LIST WILL BE RELATIVE TO THE EXECUTABLES
PATH_IN_LIST=`find $PATH_IN -type d`
for paths_In in $PATH_IN_LIST
do
	# if [[ "$paths_In" == *"/2015"* ]] && [[ "$paths_In" != *"/OUT"* ]] && [[ "$paths_In" != *"355s"* ]]
	if [[ "$paths_In" != *"/LPP_OUT"* ]] # DISCARD ANY FOLDER CALLED /LPP_OUT
	then
        echo -e "\n------------------------------------------------------------\n" 
		# PATH_FILE_OUT_L0=$PATH_OUT$(basename $paths_In)"_L0.nc"
        # DEFINING THE INPUTS AND OUTPUTS PATHS
            PATH_OUT_L0=$paths_In"LPP_OUT/"
            echo -e "\nOutput folder with the LPP products: "$PATH_OUT_L0
            if test -d "$PATH_OUT_L0"
            then 
                    echo -e "\nThe output folder already exist...\n" 
                else
                    echo -e "\nCreating the output folder...\n" 
                    mkdir $PATH_OUT_L0
            fi
            cd .. # AGAIN IN ROOT FOLDER /LPP
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
            fi
            cd ./Lidar_Analysis_PDL0/
            echo "Runining lidarAnalysis_PDL0"
                ./lidarAnalysis_PDL0 $paths_In $PATH_FILE_OUT_L0 $FILE_CONF_L0
            cd ..
        fi

        if [[ "$L1" == "yes" ]] || [[ "$L2" == "yes" ]]
        then
            cd ./Lidar_Analysis_PDL1/
            echo ------------------------------------------------------------ 
            echo ""
            echo ""
            # DELETE PREVIOUS VERSION OF THE L1 FILE (IF EXIST)
                if test -f "$PATH_FILE_OUT_L1"
                then 
                    echo "Deleting previous version of data Level 1 --> ${PATH_FILE_OUT_L1}" 
                    rm $PATH_FILE_OUT_L1 
                fi
            # CHECK IF THE INPUT FILE EXIST
            if [[ -f $PATH_FILE_IN_L1 ]]
            then
                echo "Runining lidarAnalysis_PDL1"
                    ./lidarAnalysis_PDL1 $PATH_FILE_IN_L1 $PATH_FILE_OUT_L1 $FILE_CONF_L1_L2  
            else
                echo -e "L1: input file "$PATH_FILE_IN_L1 "doesn't exist. Set L0=yes in LPP_settings.sh file"
            fi
            # rm $PATH_FILE_OUT_L0
            cd ..
        fi

        if [[ "$L2" == "yes" ]]
        then
            cd ./Lidar_Analysis_PDL2/
            echo ------------------------------------------------------------ 
            echo ""
            echo ""
            # DELETE PREVIOUS VERSION OF THE L2 FILE (IF EXIST)
            if test -f "$PATH_FILE_OUT_L2"
            then
                echo "Deleting previous version of data Level 2 --> ${PATH_FILE_OUT_L2}" 
                rm $PATH_FILE_OUT_L2 
            fi
            # CHECK IF THE INPUT FILE EXIST
            if [[ -f $PATH_FILE_OUT_L0 ]]
            then
                echo "Runining lidarAnalysis_PDL2"
                    ./lidarAnalysis_PDL2 $PATH_FILE_IN_L2 $PATH_FILE_OUT_L2 $FILE_CONF_L1_L2
            else
                echo -e "\n L2: input file "$PATH_FILE_IN_L2 " doesn't exist. Set L1=yes in LPP_settings.sh file"
            fi
        # rm $PATH_FILE_OUT_L1
        cd ..
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
