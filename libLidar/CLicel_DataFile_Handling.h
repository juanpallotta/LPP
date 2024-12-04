
// #pragma once

#ifndef CLICEL_DATAFILE_HANDLING_H
#define CLICEL_DATAFILE_HANDLING_H

#include "libLidar.hpp" // LIDAR DATA TYPE DEFINITIONS

class CLicel_DataFile_Handling
{
public:
        CLicel_DataFile_Handling();
        ~CLicel_DataFile_Handling();

        void ReadLicel_GlobalParameters   ( char*, strcGlobalParameters* )                      ;
        void ReadLicel_Time_and_Coord     ( FILE*, strcGlobalParameters* )                      ;
        void ReadLicel_Data               ( char*, strcGlobalParameters*, strcLidarDataFile* )  ;
        void read_Licel_Header_Line       ( FILE*, int, strcGlobalParameters* )                 ;
        void check_Lidar_Files_Consistency( strcGlobalParameters *, char ** )                   ;
        int  Read_Bkg_Data_Files          ( char*, strcGlobalParameters *, double** )           ;

private:
        strcLicelDataFile lidarHeaderData ;
        int  nLineElements ;
        char strLine[500], strLine_bkp[500], *token ;
        int  indx_EL ;
        FILE *fid ;
};

#endif