
#pragma once

#ifndef CLICEL_DATAFILE_HANDLING_H
#define CLICEL_DATAFILE_HANDLING_H

#include "libLidar.hpp" // LIDAR DATA TYPE DEFINITIONS

class CLicel_DataFile_Handling
{
public:
    CLicel_DataFile_Handling();
    ~CLicel_DataFile_Handling();

    void ReadLicelTime_and_Coord( FILE*, strcGlobalParameters* ) ;
    void ReadLicelGobalParameters( char*, strcGlobalParameters* ) ;
    void ReadLicelData( char*, strcGlobalParameters*, strcLidarDataFile* ) ;
    void check_Lidar_Files_Consistency( strcGlobalParameters *, char ** ) ;

private:


};

#endif