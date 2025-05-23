
/**
 * @author Juan V. Pallotta (juanpallotta@gmail.com)
 * @brief Code to process data level 0 lidar signals.
 * @date 2021-06-08
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

// LIDAR LIBRARY ANALISYS
#include "../libLidar/libLidar.hpp"
#include "../libLidar/CNetCDF_Lidar.hpp"
#include "../libLidar/CLicel_DataFile_Handling.h"

int main( int argc, char *argv[] )
{
    printf("\n\n---- lidarAnalisys_PDL0 (START) -----------------------------------------------------------------------------\n\n") ;

    strcGlobalParameters    glbParam  ;
	sprintf( glbParam.FILE_PARAMETERS, "%s", argv[3] ) ;
    
    ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"inputDataFileFormat"      , (const char*)"string", (char*)glbParam.inputDataFileFormat  ) ;
    ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"outputDataFileFormat"     , (const char*)"string", (char*)glbParam.outputDataFileFormat ) ;
    // ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"Path_To_Global_Parameters", (const char*)"string", (char*)glbParam.Measurement_Att_File ) ;

	string	Path_In ;
	string	Path_Out, File_Out, Path_File_Out ;

    Path_In.assign( argv[1] ) ;
    Path_File_Out.assign( argv[2] ) ;

    Path_Out.assign( Path_File_Out.substr( 0, Path_File_Out.find_last_of("/")+1 ) ) ;
	File_Out.assign( Path_File_Out.substr( Path_Out.length(), ( Path_File_Out.length() - Path_Out.length() ) ) ) ;
    if ( Path_In.back() != '/' )
        Path_In.push_back('/') ;

    printf("\n lpp0: Path_In:\t%s \t", Path_In.c_str() ) ;
    printf("\n lpp0: Path_File_Out: %s", Path_File_Out.c_str() ) ;

	char	strTimeMerged[70] ; // PathMergedFiles[70], PathFile_OUT_FULL[100], merged_NCDF_FileName[70], 
    char    **inputFilesInTime ;

    struct stat pathFileInput_stat ;
    stat( Path_In.c_str(), &pathFileInput_stat ) ;

    ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"Time_Zone" , (const char*)"float", (float*)&glbParam.Time_Zone ) ;

    if ( S_ISDIR(pathFileInput_stat.st_mode) )
    { // A *FOLDER* WAS PASSED AS AN ARGUMENT --> analyze all the files within the time bin set in analysisParameter.dat
        int     nFilesInInputFolder =0 ;
        DIR *   d;
        struct  dirent *dir;
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
            printf("\n There are not Licel files in the folder \nBye...") ;
            return -1 ;
        }
        rewinddir(d) ;
        inputFilesInTime  = (char**) new char* [nFilesInInputFolder] ;
        for( int f=0 ; f<nFilesInInputFolder ; f++ )
            inputFilesInTime[f] = (char*) new char [200] ;
        // GET THE NUMBER OF FILES WITHIN THE TIME BIN SET IN analysisParameter.dat
        glbParam.nEvents = (int)getInputFilesInTimeRange( (char*)Path_In.c_str(), (char**)inputFilesInTime, (strcGlobalParameters*)&glbParam ) ;
     // TO-DO: glbParam.nEvents = (int)getInputFilesInZenithRange((char*)Path_In.c_str(), (char**)inputFilesInTime, (strcGlobalParameters*)&glbParam ) ;
    } // if ( S_ISDIR(pathFileInput_stat.st_mode) )
    else
    {
        cout << endl << "A folder must be passed as first argument." << endl ;
        return -1 ;
    }
    if ( glbParam.nEvents ==0 )
    {
        printf("\nNO files in the time range selected... EXIT\n") ;
        return 0 ;
    }

    glbParam.nEventsAVG = (int)glbParam.nEvents ; // ! FOR COMPATIBILITY 

    //! check_Lidar_Files_Consistency( (strcGlobalParameters*)&glbParam, (char**)inputFilesInTime ) ;
    CLicel_DataFile_Handling oLDH ;

    if ( (strncmp( glbParam.inputDataFileFormat, "LICEL_FILE", 10 ) ==0) )
    {
        cout << "\n\tInput data file: " << glbParam.inputDataFileFormat ;
        glbParam.chSel = 0 ;
        glbParam.evSel = 0 ;
            oLDH.ReadLicel_GlobalParameters( (char*)inputFilesInTime[0], (strcGlobalParameters*)&glbParam ) ;
        glbParam.r     = (double*) new double[glbParam.nBins] ;
        for( int i=0 ; i <glbParam.nBins ; i++ )
            glbParam.r[i]     = (i+1)*glbParam.dr - glbParam.dr /2 ; // glbParam->r[0] = 3.75 (ie) IF DR = 7.5 M --> LPP & PAPALARDO 2004 ET AL
            // glbParam.r[i]     = (i+1)*glbParam.dr ; // glbParam->r[0] = 7.5 (ie) IF DR = 7.5 M --> GFATPY
    }
    // else if( (strcmp( glbParam.inputDataFileFormat, "LALINET_NETCDF" ) ==0) )
    //     cout << "\n\t Input data file: " << glbParam.inputDataFileFormat << " still NOT implemented" ;
    // else if( (strcmp( glbParam.inputDataFileFormat, "SCC_NETCDF" ) ==0) )
    //     cout << "\n\t Input data file: " << glbParam.inputDataFileFormat << " still NOT implemented" ;
    // else if( (strcmp( glbParam.inputDataFileFormat, "CSV" ) ==0) )
    //     cout << "\n\t Input data file: " << glbParam.inputDataFileFormat << " still NOT implemented" ;
    else
        cout << "\n\t Input data file: wrong value. Please, check the variable inputDataFileFormat in $s file"<< glbParam.inputDataFileFormat ;

// * LICEL FILE READOUT ////////////////////////////////////////////////////////////////////////////////////
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

    struct tm   *tmFile_start = (struct tm*) new struct tm [1] ;    tmFile_start->tm_isdst = 0 ;    tmFile_start->tm_gmtoff = round(glbParam.Time_Zone *60*60) ;
    struct tm   *tmFile_stop  = (struct tm*) new struct tm [1] ;    tmFile_stop->tm_isdst  = 0 ;    tmFile_stop->tm_gmtoff  = round(glbParam.Time_Zone *60*60) ;    
    time_t     *Raw_Data_Start_Time = (time_t*) new time_t [glbParam.nEvents] ;
    time_t     *Raw_Data_Stop_Time  = (time_t*) new time_t [glbParam.nEvents] ;
    string      Raw_Data_Start_Time_str[glbParam.nEvents], Raw_Data_Stop_Time_str[glbParam.nEvents] ;
    char	dumpChar = '\0' ;

// MAIN LOOP ACROSS THE CLUSTER FILES ////////////////////////////////////////////////////////////////////////////
    for ( int f=0 ; f <glbParam.nEvents ; f++ )
    {
        glbParam.evSel = f;
        // printf("\n File Nº %d: %s \n", f, inputFilesInTime[f] ) ;
        // avgTime_num =0 ;
            if( (strncmp( glbParam.inputDataFileFormat, "LICEL_FILE", 10 ) ==0) )
                oLDH.ReadLicel_Data ( (char*)inputFilesInTime[f], (strcGlobalParameters*)&glbParam, (strcLidarDataFile*)&dataFile[f] ) ;
            sscanf( glbParam.StartDate, "%2d%2d%4d", &tmFile_start->tm_mday, &tmFile_start->tm_mon, &tmFile_start->tm_year  ) ;
            sscanf( glbParam.StartTime, "%2d%2d%2d", &tmFile_start->tm_hour, &tmFile_start->tm_min, &tmFile_start->tm_sec   ) ;
            sscanf( glbParam.StopDate , "%2d%2d%4d", &tmFile_stop->tm_mday , &tmFile_stop->tm_mon , &tmFile_stop->tm_year   ) ;
            sscanf( glbParam.StopTime , "%2d%2d%2d", &tmFile_stop->tm_hour , &tmFile_stop->tm_min , &tmFile_stop->tm_sec    ) ;
        tmFile_start->tm_mon  = tmFile_start->tm_mon -1 ;       // TRANSLATE TO struct tm CONVENTION.
        tmFile_stop->tm_mon   = tmFile_stop->tm_mon  -1 ;       // TRANSLATE TO struct tm CONVENTION.
        tmFile_start->tm_year = tmFile_start->tm_year -1900 ;   // TRANSLATE TO struct tm CONVENTION.
        tmFile_stop->tm_year  = tmFile_stop->tm_year  -1900 ;   // TRANSLATE TO struct tm CONVENTION.

            sprintf( strTimeMerged, "%s%s", glbParam.StartDate, glbParam.StartTime ) ;
            Raw_Data_Start_Time_str[f].assign(strTimeMerged) ;
                Raw_Data_Start_Time[f] = (time_t)timegm( (tm*)tmFile_start ) ; // - (int)round(glbParam.Time_Zone *60*60) ; // SECONDS IN UTC TIME CONVERSION

            sprintf( strTimeMerged, "%s%s", glbParam.StopDate, glbParam.StopTime ) ;
            Raw_Data_Stop_Time_str[f].assign(strTimeMerged) ;
                Raw_Data_Stop_Time[f] = (time_t)timegm( (tm*)tmFile_stop ) ; // - (int)round(glbParam.Time_Zone *60*60) ; // SECONDS IN UTC TIME CONVERSION

                mkdir  ( Path_Out.c_str(), 0777 ) ;
                sprintf( strTimeMerged, "%c%04d%01x%02d%02d.%02d%02d00.dat", dumpChar, tmFile_start->tm_year +1900, tmFile_start->tm_mon, tmFile_start->tm_mday, tmFile_start->tm_hour, tmFile_start->tm_min, tmFile_start->tm_sec ) ;
                sprintf( glbParam.fileName, "%s", Path_File_Out.c_str() ) ;
                sprintf( strTimeMerged, "%04d%02d%02d%02d%02d%02d", tmFile_start->tm_year +1900, tmFile_start->tm_mon, tmFile_start->tm_mday, tmFile_start->tm_hour, tmFile_start->tm_min, tmFile_start->tm_sec ) ;
                timeVec_str[f].assign(strTimeMerged) ;
                sprintf( strTimeMerged, "%s%s", glbParam.StopDate, glbParam.StopTime ) ;

                // cout << endl << "Raw_Data_Start_Time[f]: " << Raw_Data_Start_Time[f] ;
                // cout << endl << "Raw_Data_Stop_Time[f]: "  << Raw_Data_Stop_Time[f] << endl ;
                // cout << endl << "Raw_Data_Start_Time_str[f]: " << Raw_Data_Start_Time_str[f] ;
                // cout << endl << "Raw_Data_Stop_Time_str[f]: "  << Raw_Data_Stop_Time_str[f] << endl ;
                for ( int c=0 ; c <glbParam.nCh ; c++ )
                {
                    for ( int b=0 ; b <glbParam.nBinsRaw ; b++ )
                    dataToSave[f][c][b] = (double) dataFile[f].db_ADC[c][b] ;
                }
    } // for ( int f=0 ; f <glbParam.nEventsAVG ; f++ )

// NETCDF FILE STUFF
    CNetCDF_Lidar   *oNCL = (CNetCDF_Lidar*) new CNetCDF_Lidar() ;
    if ( strcmp(glbParam.outputDataFileFormat, "SCC_NETCDF") ==0 )
    {
        printf("\n\tOutput datafile: SCC_NETCDF\n") ;
        oNCL->Save_SCC_NCDF_Format( (string)Path_File_Out, (strcGlobalParameters*)&glbParam, (double***)dataToSave, 
                                   (long*)Raw_Data_Start_Time, (string*)Raw_Data_Start_Time_str, (long*)Raw_Data_Stop_Time, (string*)Raw_Data_Stop_Time_str ) ;
    }
    else if ( strcmp(glbParam.outputDataFileFormat, "LALINET_NETCDF") ==0 )
    {
        printf("\n\tOutput datafile: LALINET_NETCDF\n") ;
        oNCL->Save_LALINET_NCDF_PDL0( (string)Path_File_Out, (strcGlobalParameters*)&glbParam, (double***)dataToSave,
                                       (long*)Raw_Data_Start_Time, (long*)Raw_Data_Stop_Time, (char**)inputFilesInTime ) ;
        char *path_dark_files = (char*) new char[100] ;
        ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"PATH_DARK_FILES", (const char*)"string", (char*)path_dark_files ) ;
        if ( strcmp(path_dark_files, "NOT_FOUND") !=0 ) // DARK FILES FOUND
        {
            printf("\n Adding noise files (%s) to NetCDF file \n", path_dark_files ) ;  
            double **data_Bkg = (double**) new double*[glbParam.nCh];
            for ( int c=0 ; c <glbParam.nCh ; c++ )
            {
                data_Bkg[c] = (double*) new double[glbParam.nBinsRaw] ;
                for(int b =0; b <glbParam.nBinsRaw; b++)
                    data_Bkg[c][b] = (double) 0;
            }
            if( oLDH.Read_Bkg_Data_Files( (char*)path_dark_files, (strcGlobalParameters*)&glbParam,(double**)data_Bkg ) >=0 )
            {
                oNCL->Add_Noise_LALINET_NCDF_PDL0( (string*)&Path_File_Out, (strcGlobalParameters*)&glbParam, (double**)data_Bkg ) ;
            }
            else
                printf( "\nNo background files were added to the L0 NetCDF file due to inconsistencies in its parameters\n" ) ;
        }

        char *overlap_file = (char*) new char[100] ;
        ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"OVERLAP_FILE", (const char*)"string", (char*)overlap_file ) ;
        // printf("\n Path to overlap file: %s", overlap_file ) ;
        if ( strcmp(overlap_file, "NOT_FOUND") !=0 ) // OVERLAP FILE FOUND
        {
            printf("\n Adding overlap file (%s) to NetCDF file \n", overlap_file ) ;

            double **ovlp = (double**)new double*[ glbParam.nCh ] ;
            for (int c =0; c <glbParam.nCh; c++)
            {
                ovlp[c] = (double*) new double[ glbParam.nBins ] ;
                memset( (double*)ovlp[c], 0, sizeof(double)*glbParam.nBins ) ;
            }
            
            if ( Read_Overlap_File ( (char*)overlap_file, (strcGlobalParameters*)&glbParam, (double**)ovlp ) >=0 )
            {
                oNCL->Add_Overlap_LALINET_NCDF_PDL0( (string*)&Path_File_Out, (strcGlobalParameters*)&glbParam, (double**)ovlp ) ;
            }
            else
            {
                printf( "\n\n No overlap file was added to the L0 NetCDF file due to inconsistencies in its parameters\n" ) ;                
            }
        }
    } // else if ( strcmp(glbParam.outputDataFileFormat, "LALINET_NETCDF") ==0 )

    printf("\n\n\t\t SUCCESS writing %s file!\n", Path_File_Out.c_str() ) ;
    cout << endl << endl << "\tLidar Analisys PDL0 Done" << endl ;
    printf("\n\n---- lidarAnalisys_PDL0 (END) -----------------------------------------------------------------------------\n\n") ;
 
	return 0 ;
}
