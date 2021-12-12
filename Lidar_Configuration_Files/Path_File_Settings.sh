#!/bin/bash

#################################### INPUT FOLDER FOR lidarAnalysis_L0

# FILE_CONF MUST CONTAIN THE RELATIVE PATH FROM THE LIDAR_ANALYSIS_PDL0 FOLDER
FILE_CONF_L0="../Lidar_Configuration_Files/analysisParameters_PDL0_Brazil.conf"
# FILE_CONF_L0="../Lidar_Configuration_Files/analysisParameters_PDL0_Argentina.conf"

PATH_IN_L0="/mnt/Disk-1_8TB/Brazil/SPU/20210730/"
PATH_OUT_L0=$PATH_IN_L0"OUT/"
FILE_OUT_L0="20210730_L0.nc"
PATH_FILE_OUT_L0=$PATH_OUT_L0$FILE_OUT_L0


####################################  INPUT PATHS AND FILE FOR lidarAnalysis_L1

# FILE_CONF MUST CONTAIN THE RELATIVE PATH FROM THE LIDAR_ANALYSIS_PDL1 FOLDER
FILE_CONF_L1_2="../Lidar_Configuration_Files/analysisParameters_PDL1_2_Brazil.conf"
# FILE_CONF_L1_2="../Lidar_Configuration_Files/analysisParameters_PDL1_2_Argentina.conf"

PATH_IN_L1=$PATH_OUT_L0
FILE_IN_L1=$FILE_OUT_L0
PATH_FILE_IN_L1=$PATH_IN_L1$FILE_IN_L1
# OUTPUT PATHS AND FILE FOR lidarAnalysis_L0
PATH_OUT_L1=$PATH_OUT_L1
FILE_OUT_L1="20180619_L1.nc"
# FILE_OUT_L0="20110714_L1.nc"
PATH_FILE_OUT_L1=$PATH_OUT_L1$FILE_OUT_L1


#################################### INPUT PATHS AND FILE FOR lidarAnalysis_L2

# FILE_CONF MUST CONTAIN THE RELATIVE PATH FROM THE LIDAR_ANALYSIS_PDL1 FOLDER
FILE_CONF_L1_2="../Lidar_Configuration_Files/analysisParameters_PDL1_2_Brazil.conf"
# FILE_CONF_L1_2="../Lidar_Configuration_Files/analysisParameters_PDL1_2_Argentina.conf"

PATH_IN_L2=$PATH_OUT_L0
FILE_IN_L2=$FILE_OUT_L1
PATH_FILE_IN_L2=$PATH_IN_L2$FILE_IN_L2
# OUTPUT PATHS AND FILE FOR lidarAnalysis_L0
PATH_OUT_L2=$PATH_OUT_L2
FILE_OUT_L2="20180619_L2.nc"
PATH_FILE_OUT_L2=$PATH_OUT_L2$FILE_OUT_L2

