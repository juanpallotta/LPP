/**
 * @file mergeLidarFiles.cpp
 * @author Juan V. Pallotta (juanpallotta@gmail.com)
 * @brief Main code to process lidar signals.
 * @version 0.1
 * @date 2021-06-08
 * @copyright Copyright (c) 2021
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string>
#include <vector>
#include <cstdlib>
#include <sys/stat.h>
#include <ctime>
#include <assert.h> 
#include <netcdf.h>

/* Handle errors by printing an error message and exiting with a
 * non-zero status. */
#define ERRCODE 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}

// #define NDIMS_SCC 5
// #define NVARS_SCC 15

// #define NDIMS_LALINET 5
// #define NVARS_LALINET 20

// LIDAR LIBRARY ANALISYS
#include "../libLidar/libLidar.hpp"
#include "../libLidar/CNetCDF_Lidar.hpp"

int main( int argc, char *argv[] )
{
    strcGlobalParameters    glbParam  ;
	sprintf( glbParam.FILE_PARAMETERS , "%s", argv[3] ) ;
    // ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"numEventsToAvg"      , (const char*)"int"   , (int*)&glbParam.numEventsToAvg ) ;
    ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"inputDataFileFormat" , (const char*)"string", (char*)glbParam.inputDataFileFormat  ) ;
    ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"outputDataFileFormat", (const char*)"string", (char*)glbParam.outputDataFileFormat ) ;

	string	Path_In ;
	string	Path_Out, File_Out, Path_File_Out ;

    Path_In.assign( argv[1] ) ;

    Path_File_Out.assign( argv[2] ) ;
    Path_Out.assign( Path_File_Out.substr( 0, Path_File_Out.find_last_of("/")+1 ) ) ;
	File_Out.assign( Path_File_Out.substr( Path_Out.length(), ( Path_File_Out.length() - Path_Out.length() ) ) ) ;
    if ( Path_In.back() != '/' )
        Path_In.push_back('/') ;

    printf("\n Path_In:\t%s \t", Path_In.c_str() ) ;
    // printf("\n Path_Out:\t%s", Path_Out.c_str() ) ;
    // printf("\n File_Out: \t%s", File_Out.c_str() ) ;
    printf("\n Path_File_Out: %s", Path_File_Out.c_str() ) ;
    // printf("\n Configuration File (glbParam.FILE_PARAMETERS): \t%s", glbParam.FILE_PARAMETERS ) ;
    // printf("\n Configuration File (argv[3]): \t%s", argv[3] ) ;

	char	strTimeMerged[70] ; // PathMergedFiles[70], PathFile_OUT_FULL[100], merged_NCDF_FileName[70], 
    char    **inputFilesInTime ;

    struct stat pathFileInput_stat ;
    stat( Path_In.c_str(), &pathFileInput_stat ) ;

    // if ( S_ISREG(pathFileInput_stat.st_mode) )
    // { // A *FILE* WAS PASSED AS AN ARGUMENT
    //     inputFilesInTime    = (char**) new char* [1] ;
    //     inputFilesInTime[0] = (char*)  new char [200] ;
    //     sprintf(inputFilesInTime[0], "%s", Path_In.c_str() ) ;
    //     printf("\n File to analyze: %s \n", inputFilesInTime[0]) ;
    // }
    // else
    if ( S_ISDIR(pathFileInput_stat.st_mode) )
    { // A *FOLDER* WAS PASSED AS AN ARGUMENT --> analyze all the files within the time bin set in analysisParameter.dat
        int     nFilesInInputFolder =0 ;
        DIR *d;
        struct dirent *dir;
        // GET THE NUMBER OF FILES IN THE INPUT FOLDER
        d = opendir(Path_In.c_str()) ;
        while ( (dir = readdir(d)) != NULL )
        { // GET THE NUMBER OF FILES INSIDE THE FOLDER
            if ( ( dir->d_type == DT_REG ) && ( strcmp(dir->d_name, "..") !=0 ) && ( strcmp(dir->d_name, ".") !=0 ) && ( strcmp(dir->d_name, "log.txt") !=0 ) && ( strcmp(dir->d_name, "temp.dat") !=0 ) )
                nFilesInInputFolder++;
        }
        // printf("\n nFilesInInputFolder: %d \n", nFilesInInputFolder) ;
        if ( nFilesInInputFolder ==0 )
        {
            printf("\n There are not Licel files in the folder (see dirFile.sh) \nBye...") ;
            return -1 ;
        }
        rewinddir(d) ;
        inputFilesInTime  = (char**) new char* [nFilesInInputFolder] ;
        for( int f=0 ; f<nFilesInInputFolder ; f++ )
            inputFilesInTime[f] = (char*) new char [200] ;
        // GET THE NUMBER OF FILES WITHIN THE TIME BIN SET IN analysisParameter.dat
        glbParam.nEvents = (int)getInputFilesInTimeRange((char*)Path_In.c_str(), (char**)inputFilesInTime, (strcGlobalParameters*)&glbParam ) ;
    }
    else
    {
        cout << endl << "A folder should be passed as first argument (check dirFile.sh file)" << endl ;
        return -1 ;
    }
    assert( glbParam.nEvents !=0 ); // SI SE CUMPLE, SIGUE DE LARGO.

    // glbParam.nEventsAVG = (int)floor(glbParam.nEvents /glbParam.numEventsToAvg ) ;
    glbParam.nEventsAVG = (int)glbParam.nEvents ; // ! FOR COMPATIBILITY --> ERASE

    if ( (strcmp( glbParam.inputDataFileFormat, "LICEL_FILE" ) ==0) || (strcmp( glbParam.inputDataFileFormat, "RAYMETRIC_FILE" ) ==0) )
    {
        cout << "\n\t Input data file: " << glbParam.inputDataFileFormat << endl ;
        ReadLicelGobalParameters( (char*)inputFilesInTime[0], (strcGlobalParameters*)&glbParam ) ;
    }
    else if( (strcmp( glbParam.inputDataFileFormat, "LALINET_NETCDF" ) ==0) )
        cout << "\n\t Input data file: " << glbParam.inputDataFileFormat << " still NOT implemented" << endl ;
    else if( (strcmp( glbParam.inputDataFileFormat, "SCC_NETCDF" ) ==0) )
        cout << "\n\t Input data file: " << glbParam.inputDataFileFormat << " still NOT implemented" << endl ;
    else if( (strcmp( glbParam.inputDataFileFormat, "CSV" ) ==0) )
        cout << "\n\t Input data file: " << glbParam.inputDataFileFormat << " still NOT implemented" << endl ;
    else
        cout << "\n\t Input data file: wrong value. Please, check mergingParameters.dat" << endl ;

    printf( "\n Total Events: %d \n", glbParam.nEvents ) ;

// * LICEL FILE READOUT ////////////////////////////////////////////////////////////////////////////////////
    // Average_In_Time_Lidar_Profiles( (strcGlobalParameters*)&glbParam ) ;

    strcLidarDataFile	*dataFile    = (strcLidarDataFile*) new strcLidarDataFile[ glbParam.nEvents ] ;
    GetMem_DataFile( (strcLidarDataFile*)dataFile, (strcGlobalParameters*)&glbParam ) ;
    string  timeVec_str[glbParam.nEvents] ;

    double  ***dataToSave = (double***) new double**[glbParam.nEvents];
    for ( int e=0 ; e <glbParam.nEvents ; e++ )
    {
        dataToSave[e] = (double**) new double*[glbParam.nCh] ;
        for ( int c=0 ; c <glbParam.nCh ; c++ )
        {
            dataToSave[e][c] = (double*) new double[glbParam.nBinsRaw] ;
            for(int b =0; b <glbParam.nBinsRaw; b++)
                dataToSave[e][c][b] = (double)0;
        }
    }

    struct tm   *tmFile_start = (struct tm*) new struct tm [1] ;    tmFile_start->tm_isdst = 0 ;
    struct tm   *tmFile_stop  = (struct tm*) new struct tm [1] ;    tmFile_stop->tm_isdst  = 0 ;
    int     *Raw_Data_Start_Time = (int*) new int [glbParam.nEvents] ;
    int     *Raw_Data_Stop_Time  = (int*) new int [glbParam.nEvents] ;
    string  Raw_Data_Start_Time_str[glbParam.nEvents], Raw_Data_Stop_Time_str[glbParam.nEvents] ;
    char	dumpChar = '\0' ;

// MAIN LOOP ACROSS THE CLUSTER FILES ////////////////////////////////////////////////////////////////////////////
    for ( int fC=0 ; fC <glbParam.nEvents ; fC++ )
    {
        glbParam.event_analyzed = fC;
        printf("\n File Nº %d: %s \n", fC, inputFilesInTime[fC] ) ;
        // avgTime_num =0 ;
            if( (strcmp( glbParam.inputDataFileFormat, "LICEL_FILE" ) ==0) || (strcmp( glbParam.inputDataFileFormat, "RAYMETRIC_FILE" ) ==0) )
                ReadLicelData ( (char*)inputFilesInTime[fC], (strcGlobalParameters*)&glbParam, (strcLidarDataFile*)&dataFile[fC] ) ;
            sscanf( glbParam.StartDate, "%2d%2d%4d", &tmFile_start->tm_mday, &tmFile_start->tm_mon, &tmFile_start->tm_year  ) ;
            sscanf( glbParam.StartTime, "%2d%2d%2d", &tmFile_start->tm_hour, &tmFile_start->tm_min, &tmFile_start->tm_sec   ) ;
            sscanf( glbParam.StopDate, "%2d%2d%4d" , &tmFile_stop->tm_mday , &tmFile_stop->tm_mon , &tmFile_stop->tm_year   ) ;
            sscanf( glbParam.StopTime, "%2d%2d%2d" , &tmFile_stop->tm_hour , &tmFile_stop->tm_min , &tmFile_stop->tm_sec    ) ;

                sprintf( strTimeMerged, "%s%s", glbParam.StartDate, glbParam.StartTime ) ;
                Raw_Data_Start_Time_str[fC].assign(strTimeMerged) ;
                Raw_Data_Start_Time[fC] = (int)mktime( (tm*)tmFile_start ) ;

                sprintf( strTimeMerged, "%s%s", glbParam.StopDate, glbParam.StopTime ) ;
                Raw_Data_Stop_Time_str[fC].assign(strTimeMerged) ;
                Raw_Data_Stop_Time[fC] = (int)mktime( (tm*)tmFile_stop ) ;

                tmFile_start->tm_year = 1900 + tmFile_start->tm_year ;

                mkdir  ( Path_Out.c_str(), 0777 ) ;
                sprintf( strTimeMerged, "%c%04d%01x%02d%02d.%02d%02d00.dat", dumpChar, tmFile_start->tm_year, tmFile_start->tm_mon, tmFile_start->tm_mday, tmFile_start->tm_hour, tmFile_start->tm_min, tmFile_start->tm_sec ) ;
                sprintf( glbParam.fileName, "%s", Path_File_Out.c_str() ) ;
                sprintf( strTimeMerged, "%04d%02d%02d%02d%02d%02d", tmFile_start->tm_year, tmFile_start->tm_mon, tmFile_start->tm_mday, tmFile_start->tm_hour, tmFile_start->tm_min, tmFile_start->tm_sec ) ;
                timeVec_str[fC].assign(strTimeMerged) ;
                sprintf( strTimeMerged, "%s%s", glbParam.StopDate, glbParam.StopTime ) ;

                cout << endl << "Raw_Data_Start_Time[fC]: " << Raw_Data_Start_Time[fC] ;
                cout << endl << "Raw_Data_Stop_Time[fC]: "  << Raw_Data_Stop_Time[fC] << endl ;
                cout << endl << "Raw_Data_Start_Time_str[fC]: " << Raw_Data_Start_Time_str[fC] ;
                cout << endl << "Raw_Data_Stop_Time_str[fC]: "  << Raw_Data_Stop_Time_str[fC] << endl ;
                    for ( int c=0 ; c <glbParam.nCh ; c++ )
                    {
                        for ( int b=0 ; b <glbParam.nBinsRaw ; b++ )
                        {
                                dataToSave[fC][c][b] = (double) dataFile[fC].db_ADC[c][b] ;
                        }
                    }
    } // for ( int fC=0 ; fC <glbParam.nEventsAVG ; fC++ )

// NETCDF FILE STUFF
    CNetCDF_Lidar   oNCL = CNetCDF_Lidar() ;

    if ( strcmp(glbParam.outputDataFileFormat, "SCC_NETCDF") ==0 )
    {
        cout << endl << "\tOutput datafile: SCC_NETCDF" << endl ;
        oNCL.Save_SCC_NCDF_Format( (string)Path_File_Out, (strcGlobalParameters*)&glbParam, (double***)dataToSave, 
                                   (int*)Raw_Data_Start_Time, (string*)Raw_Data_Start_Time_str, (int*)Raw_Data_Stop_Time, (string*)Raw_Data_Stop_Time_str ) ;
    }
    else if ( strcmp(glbParam.outputDataFileFormat, "LALINET_NETCDF") ==0 )
    {
        cout << endl << "\tOutput datafile: LALINET_NETCDF" << endl ;
        oNCL.Save_LALINET_NCDF_PDL0( (string)Path_File_Out, (strcGlobalParameters*)&glbParam, (double***)dataToSave,
                                       (int*)Raw_Data_Start_Time, (int*)Raw_Data_Stop_Time ) ;
    }

    // if ( (retval = nc_close(ncid)) )
    //     ERR(retval);

    printf("\n\n*** SUCCESS writing example file %s!\n", Path_File_Out.c_str());

	return 0 ;
}
