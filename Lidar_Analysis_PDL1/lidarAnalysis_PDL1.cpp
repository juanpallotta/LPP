/**
 * @file lidarAnalysisPDL1.cpp
 * @author Juan V. Pallotta (juanpallotta@gmail.com)
 * @brief Main code to process lidar signals.
 * @version 0.1
 * @date 2021-06-08
 * @copyright Copyright (c) 2021
 */

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <cstdlib>
#include <sys/stat.h>
#include <ctime>
#include <assert.h> 
#include <iostream> 
#include <vector>

#include <netcdf>
using namespace std;
using namespace netCDF;
using namespace netCDF::exceptions;

// LIDAR LIBRARY ANALISYS
#include "../libLidar/libLidar.hpp"
#include "../libLidar/CDataLevel_1.hpp"
#include "../libLidar/CMolecularData_USStd.hpp"
#include "../libLidar/CNetCDF_Lidar.hpp"

int main( int argc, char *argv[] )
{
    strcGlobalParameters    glbParam  ;
	sprintf( glbParam.FILE_PARAMETERS , "%s", argv[3] ) ;

	string	Path_File_In  ;
	string	Path_File_Out ;

	Path_File_In.assign  ( argv[1] ) ;
	Path_File_Out.assign ( argv[2] ) ;

    printf("\n Path_File_In:\t %s " , Path_File_In.c_str()  ) ;
    printf("\n Path_File_Out:\t %s\n\n", Path_File_Out.c_str() ) ;

    char cmdCopy[500] ;
    sprintf( cmdCopy, "cp %s %s", Path_File_In.c_str(), Path_File_Out.c_str() ) ;
    system(cmdCopy) ;

// NETCDF FILE STUFF
    int  ncid   ;
    int  retval ;
    
    CNetCDF_Lidar   oNCL = CNetCDF_Lidar() ;

    if ( (retval = nc_open( Path_File_In.c_str(), NC_NOWRITE, &ncid)) )
        ERR(retval);

    // READ VARIABLES FROM DE NETCDF INPUT FILE
    string  *strNameVars = (string*)new string[NVARS_LALINET] ;
    strNameVars[0] = "Raw_Lidar_Data" ;
    int id_var ;
    if ( ( retval = nc_inq_varid( (int)ncid, (const char*)strNameVars[0].c_str(), (int*)&id_var ) ) )
        ERR(retval);
    int num_dim_var ;
    if ( ( retval = nc_inq_varndims( (int)ncid, (int)id_var, (int*)&num_dim_var ) ) )
        ERR(retval);
    // cout << endl << "num_dim_var: " << num_dim_var << "     id_var: " << id_var << endl ;
    assert( num_dim_var ==3 ) ; // SI SE CUMPLE, SIGUE.

    int id_dim[num_dim_var] ;
    if ( ( retval = nc_inq_vardimid( (int)ncid, (int)id_var, (int*)id_dim ) ) )
        ERR(retval);

    int size_dim[num_dim_var] ;
        for ( int d=0 ; d <num_dim_var ; d++ ) size_dim[d] =(int)1 ;
    for( int d=0 ; d <num_dim_var ; d++ )
    {
        if ( ( retval = nc_inq_dimlen( (int)ncid, (int)id_dim[d], (size_t*)&size_dim[d] ) ) )
            ERR(retval);
        // printf( "\nsize_dim[%d]: %d", d, size_dim[d] ) ;
    }
    glbParam.nEvents = size_dim[0] ; glbParam.nEventsAVG = glbParam.nEvents ;
    glbParam.nCh     = size_dim[1] ; glbParam.nLambda    = glbParam.nCh     ; // IT SHOULD BE CALCULATE BASED ON *DIFFERENTS* WAVELENGHS.
    glbParam.nCh     = size_dim[1] ;
    glbParam.nBins   = size_dim[2] ;

    double  ***dataFile     = (double***) new double**[glbParam.nEvents];
    for ( int e=0 ; e <glbParam.nEvents ; e++ )
    {
        dataFile[e] = (double**) new double*[glbParam.nCh] ;
        for ( int c=0 ; c <glbParam.nCh ; c++ )
        {
            dataFile[e][c] = (double*) new double[glbParam.nBins] ;
            for(int b =0 ; b <glbParam.nBins ; b++)
                dataFile[e][c][b] = (double)0;
        }
    }

    size_t startDF[3], countDF[3];
    startDF[0] = 0;   countDF[0] = 1 ; // glbParam.nEvents ; 
    startDF[1] = 0;   countDF[1] = 1 ; // glbParam.nCh; 
    startDF[2] = 0;   countDF[2] = glbParam.nBins;
    for( int e=0 ; e <glbParam.nEvents ; e++ )
    {
        startDF[0] =e ;
        for ( int c=0 ; c <glbParam.nCh ; c++ )
        {
            startDF[1] =c ;
            if ( (retval = nc_get_vara_double((int)ncid, (int)id_var, startDF, countDF, (double*)&dataFile[e][c][0] ) ) )
                ERR(retval);    
        }
    }

    int *Raw_Data_Start_Time = (int*) new int [glbParam.nEvents] ;
    int *Raw_Data_Stop_Time  = (int*) new int [glbParam.nEvents] ;
    oNCL.ReadVar( (int)ncid, (const char*)"Raw_Data_Start_Time", (int*)Raw_Data_Start_Time ) ;
    oNCL.ReadVar( (int)ncid, (const char*)"Raw_Data_Stop_Time" , (int*)Raw_Data_Stop_Time  ) ;

    ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"numEventsToAvg_PDL1", (const char*)"int", (int*)&glbParam.numEventsToAvg ) ;
    glbParam.nEventsAVG = (int)round( glbParam.nEvents /glbParam.numEventsToAvg ) ;

    glbParam.aZenith     = (double*) new double [glbParam.nEvents]    ;
    glbParam.aAzimuth    = (double*) new double [glbParam.nEvents]    ;
    glbParam.aZenithAVG  = (double*) new double [glbParam.nEventsAVG] ;     memset( (double*)glbParam.aZenithAVG , 0, (sizeof(double)*glbParam.nEventsAVG) ) ;
    glbParam.aAzimuthAVG = (double*) new double [glbParam.nEventsAVG] ;     memset( (double*)glbParam.aAzimuthAVG, 0, (sizeof(double)*glbParam.nEventsAVG) ) ;
    oNCL.ReadVar( (int)ncid, (const char*)"Zenith"  , (int*)glbParam.aZenith  ) ;
    oNCL.ReadVar( (int)ncid, (const char*)"Azimuth" , (int*)glbParam.aAzimuth ) ;

    glbParam.temp_Celsius     = (double*) new double [glbParam.nEvents]     ;
    glbParam.pres_hPa         = (double*) new double [glbParam.nEvents]     ;
    glbParam.temp_CelsiusAVG  = (double*) new double [glbParam.nEventsAVG]  ;   memset( (double*)glbParam.temp_CelsiusAVG, 0, (sizeof(double)*glbParam.nEventsAVG) ) ;
    glbParam.pres_hPaAVG      = (double*) new double [glbParam.nEventsAVG]  ;   memset( (double*)glbParam.pres_hPaAVG    , 0, (sizeof(double)*glbParam.nEventsAVG) ) ;
    ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"Temperature_at_Lidar_Station", (const char*)"double", (int*)&glbParam.temp_Celsius[0] ) ;
    ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"Pressure_at_Lidar_Station"   , (const char*)"double", (int*)&glbParam.pres_hPa[0]     ) ;
    for (  int i =1 ; i < glbParam.nEvents ; i++ )
    {   //* TO BE UPDATED WITH /GetRadiosounding/get_Meteodata.py
        glbParam.temp_Celsius[i] = glbParam.temp_Celsius[0]  ;
        glbParam.pres_hPa[i]     = glbParam.pres_hPa[0]      ;
    }

    double  ***dataFile_AVG = (double***) new double**[glbParam.nEventsAVG] ;
    for ( int e=0 ; e <glbParam.nEventsAVG ; e++ )
    {
        dataFile_AVG[e] = (double**) new double*[glbParam.nCh] ;
        for ( int c=0 ; c <glbParam.nCh ; c++ )
        {
            dataFile_AVG[e][c] = (double*) new double[glbParam.nBins] ;
            for(int b =0 ; b <glbParam.nBins ; b++)
                dataFile_AVG[e][c][b] = (double)0.0 ;
        }
    }
    int *Raw_Data_Start_Time_AVG = (int*) new int [glbParam.nEventsAVG] ;   memset( (int*)Raw_Data_Start_Time_AVG, 0, (sizeof(int)*glbParam.nEventsAVG) ) ;
    int *Raw_Data_Stop_Time_AVG  = (int*) new int [glbParam.nEventsAVG] ;   memset( (int*)Raw_Data_Stop_Time_AVG , 0, (sizeof(int)*glbParam.nEventsAVG) ) ;
    Average_In_Time_Lidar_Profiles( (strcGlobalParameters*)&glbParam, (double***)dataFile, (double***)dataFile_AVG, 
                                    (int*)Raw_Data_Start_Time    , (int*)Raw_Data_Stop_Time, 
                                    (int*)Raw_Data_Start_Time_AVG, (int*)Raw_Data_Stop_Time_AVG
                                  ) ;

// printf("\n lidarAnalysis_PDL1.Raw_Data_Start_Time_AVG: %d ", Raw_Data_Start_Time_AVG[0]) ;
// printf("\n lidarAnalysis_PDL1.Raw_Data_Start_Time_AVG: %d ", Raw_Data_Start_Time_AVG[1]) ;
// printf("\n lidarAnalysis_PDL1.Raw_Data_Start_Time_AVG: %d ", Raw_Data_Start_Time_AVG[2]) ;
// printf("\n lidarAnalysis_PDL1.Raw_Data_Start_Time_AVG: %d ", Raw_Data_Start_Time_AVG[3]) ;

// READ GLOBAL PARAMETERS FROM NETCDF FILE

    if ( ( retval = nc_get_att_double( (int)ncid, (int)NC_GLOBAL, (const char*)"Range_Resolution", (double*)&glbParam.dr) ) )
        ERR(retval);

    glbParam.r = (double*) new double[glbParam.nBins] ;
    for( int i=0 ; i <glbParam.nBins ; i++ )
        glbParam.r[i] = i*glbParam.dr ;

    if ( ( retval = nc_get_att_double( (int)ncid, (int)NC_GLOBAL, (const char*)"Altitude_meter_asl", (double*)&glbParam.siteASL ) ) )
        ERR(retval);

    int indxWL_PDL1 ;
    ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"indxWL_PDL1", (const char*)"int", (int*)&indxWL_PDL1 ) ;

    assert( indxWL_PDL1 <= (glbParam.nCh -1 ) ) ;

    glbParam.iLambda = (int*) new int [glbParam.nCh] ;
    oNCL.ReadVar( (int)ncid, (const char*)"Wavelenghts", (int*)glbParam.iLambda ) ;

                        if ( (retval = nc_close(ncid)) )
                            ERR(retval);

    glbParam.indxOffset = (int*) new int [ glbParam.nCh ] ;
    ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"nBinsBkg"   , (const char*)"int"   , (int*)&glbParam.nBinsBkg      ) ;
    ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"indxOffset" , (const char*)"int"   , (int*)glbParam.indxOffset    ) ;
    ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"rInitSig"   , (const char*)"double", (double*)&glbParam.rInitSig   ) ;
    ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"rEndSig"    , (const char*)"double", (double*)&glbParam.rEndSig    ) ;
    glbParam.indxInitSig = (int)round( glbParam.rInitSig /glbParam.dr ) ;
    glbParam.indxEndSig  = (int)round( glbParam.rEndSig  /glbParam.dr );

// MOLECULAR DATA READOUT FOR EACH CHANNEL (MUST BE FOR EACH LAMBDA)
//! THE PATH SHOULD BE READ FROM glbParam.FILE_PARAMETERS
    char radFile[100] ;
    sprintf( radFile, "./US-StdA_DB_CEILAP.csv") ;
    CMolecularData_USStd *oMolData = (CMolecularData_USStd*) new CMolecularData_USStd( (char*)radFile, (strcGlobalParameters*)&glbParam ) ;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    strcLidarSignal 	evSig ;
    GetMem_evSig( (strcLidarSignal*) &evSig, (strcGlobalParameters*) &glbParam );

    double  ***pr_corr = (double***) new double**[glbParam.nEventsAVG];
    double  ***pr2     = (double***) new double**[glbParam.nEventsAVG];
    for ( int e=0 ; e <glbParam.nEventsAVG ; e++ )
    {
        pr_corr[e] = (double**) new double*[glbParam.nCh] ;
        pr2[e]     = (double**) new double*[glbParam.nCh] ;
        for ( int c=0 ; c <glbParam.nCh ; c++ )
        {
            pr2[e][c]     = (double*) new double[glbParam.nBins] ;
            pr_corr[e][c] = (double*) new double[glbParam.nBins] ;

            for(int b =0; b <(glbParam.nBins -glbParam.indxOffset[c]); b++)
                pr_corr[e][c][b] = (double)dataFile_AVG[e][c][b +glbParam.indxOffset[c]-1] ; // BIN OFFSET CORRECTION;
            for ( int b=(glbParam.nBins -glbParam.indxOffset[c]) ; b <glbParam.nBins ; b++ )
                pr_corr[e][c][b] = (double)dataFile_AVG[e][c][glbParam.nBins -glbParam.indxOffset[c]-1] ; // BIN OFFSET CORRECTION;
        }
    }

    int  **clouds_ON_mtx = (int**) new int*[glbParam.nEventsAVG];
    for ( int e=0 ; e <glbParam.nEventsAVG ; e++ )
        clouds_ON_mtx[e] = (int*) new int[glbParam.nBins] ;

    double  **alpha_mol = (double**) new double*[glbParam.nCh];
    double  **beta_mol  = (double**) new double*[glbParam.nCh];
    for ( int c=0 ; c <glbParam.nCh ; c++ )
    {
        alpha_mol[c] = (double*) new double[glbParam.nBins] ;
        beta_mol[c]  = (double*) new double[glbParam.nBins] ;
    }

    CDataLevel_1 oDL1 = CDataLevel_1( (strcGlobalParameters*)&glbParam ) ;
    for ( int t=0 ; t <glbParam.nEventsAVG ; t++ )
    {
        cout << endl << endl ;
        glbParam.event_analyzed = t ;
        for ( int c=0 ; c <glbParam.nCh ; c++ )
        {
            cout << endl << "Event: " << t << "\t Wavelengh: " << glbParam.iLambda[c] ;
            for ( int i=0 ; i <glbParam.nBins ; i++ )
                evSig.pr[i] = (double)pr_corr[t][c][i] ;
            // for ( int i=0 ; i <(glbParam.nBins -glbParam.indxOffset[c]) ; i++ )
            //     evSig.pr[i] = (double)pr_corr[t][c][i] ;
            // for ( int i=(glbParam.nBins -glbParam.indxOffset[c]) ; i <glbParam.nBins ; i++ )
            //     evSig.pr[i] = (double)pr_corr[t][c][i] ;
            /*
            // if ( glbParam.rEndSig >0 )
            //     glbParam.indxEndSig  = (int)round( glbParam.rEndSig /glbParam.dr ) ;
            // else
            // {
                // double  min_pr      = *min_element( evSig.pr +100, evSig.pr +glbParam.nBins ) ;
                // double  indx_min_pr =  min_element( evSig.pr +100, evSig.pr +glbParam.nBins ) -evSig.pr +100 ;
                // glbParam.indxEndSig = min_element( evSig.pr +100, evSig.pr +glbParam.nBins-101 ) -evSig.pr +100 ;
                // if ( glbParam.indxEndSig <2000 )
                //     glbParam.indxEndSig  = (int)2000 ; // round( 20000 / glbParam.dr ) ;

                // glbParam.rEndSig = glbParam.indxEndSig *glbParam.dr ;
                // cout << endl << "glbParam.indxEndSig: " << glbParam.indxEndSig << "\t glbParam.nBins: " << glbParam.nBins << "\t glbParam.rEndSig: " << glbParam.rEndSig ;
            // }
            */
            oMolData->Fill_dataMol( (strcGlobalParameters*)&glbParam, (int)c ) ;
            oDL1.MakeRangeCorrected ( (strcLidarSignal*)&evSig, (strcGlobalParameters*)&glbParam, (strcMolecularData*)&oMolData->dataMol[c] ) ;
            for( int i=0 ; i <glbParam.nBins ; i++ )
            {
                pr2[t][c][i]    = (double)evSig.pr2[i] ;
                alpha_mol[c][i] = (double)oMolData->dataMol[c].alphaMol[i] ;
                beta_mol[c][i]  = (double)oMolData->dataMol[c].betaMol[i]  ;
            }

            if ( c == indxWL_PDL1 )
            {
                cout << "\t --> Getting the cloud profile..." ;
                oDL1.ScanCloud_RayleightFit( (const double*)evSig.pr , (strcGlobalParameters*)&glbParam, (strcMolecularData*)&oMolData->dataMol[indxWL_PDL1] ) ;
                cout << " done." ;

                for( int b=0 ; b <glbParam.nBins ; b++ )
                    clouds_ON_mtx[t][b] = (int) oDL1.cloudProfiles[t].clouds_ON[b] ;
            }
        } // for ( int c=0 ; c <glbParam.nCh ; c++ )
    } // for ( int t=0 ; t <glbParam.nEventsAVG ; t++ )


    // oNCL.Save_LALINET_NCDF_PDL1( (string*)&Path_File_In, (string*)&Path_File_Out, (strcGlobalParameters*)&glbParam, (int**)clouds_ON_mtx,
    //                              (double***)pr_corr, (double***)pr2, (CMolecularData_USStd*)oMolData ) ;
    oNCL.Save_LALINET_NCDF_PDL1( (string*)&Path_File_In, (string*)&Path_File_Out, (strcGlobalParameters*)&glbParam, (int**)clouds_ON_mtx,
                                 (double***)pr_corr, (double***)pr2, (int*)Raw_Data_Start_Time_AVG, (int*)Raw_Data_Stop_Time_AVG, (CMolecularData_USStd*)oMolData ) ;

    for ( int e=0; e <glbParam.nEventsAVG ; e++  )
        delete [] clouds_ON_mtx[e] ;
    delete [] clouds_ON_mtx ;

    delete Raw_Data_Start_Time      ;
    delete Raw_Data_Stop_Time       ;
    delete Raw_Data_Start_Time_AVG  ;
    delete Raw_Data_Stop_Time_AVG   ;

    cout << endl << endl << "\tLidar Analisys PDL1 Done" << endl << endl ;
	return 0 ;
}