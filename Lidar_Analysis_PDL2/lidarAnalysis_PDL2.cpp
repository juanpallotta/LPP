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
#include "../libLidar/CDataLevel_2.hpp"
#include "../libLidar/CNetCDF_Lidar.hpp"

int main( int argc, char *argv[] )
{
    printf("\n\n---- lidarAnalisys_PDL2 (START) -----------------------------------------------------------------------------\n\n") ;
 
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
    int id_var_pr ;
    if ( ( retval = nc_inq_varid( (int)ncid, (const char*)"Raw_Lidar_Data", (int*)&id_var_pr ) ) )
        ERR(retval) ;
    int num_dim_var ;
    if ( ( retval = nc_inq_varndims( (int)ncid, (int)id_var_pr, (int*)&num_dim_var ) ) )
        ERR(retval);
    // cout << endl << "num_dim_var: " << num_dim_var << "     id_var_pr: " << id_var_pr << endl ;
    assert( num_dim_var ==3 ) ; // SI SE CUMPLE, SIGUE.

    int id_dim[num_dim_var] ;
    if ( ( retval = nc_inq_vardimid( (int)ncid, (int)id_var_pr, (int*)id_dim ) ) )
        ERR(retval);

    int size_dim[num_dim_var] ;
        for ( int d=0 ; d <num_dim_var ; d++ ) size_dim[d] =(int)1 ;
    for( int d=0 ; d <num_dim_var ; d++ )
    {
        if ( ( retval = nc_inq_dimlen( (int)ncid, (int)id_dim[d], (size_t*)&size_dim[d] ) ) )
            ERR(retval);
        // printf( "\nsize_dim[%d]: %d", d, size_dim[d] ) ;
    }
    glbParam.nEvents = size_dim[0] ; glbParam.nEventsAVG = glbParam.nEvents ; // 'time' DIMENSION
    glbParam.nCh     = size_dim[1] ; glbParam.nLambda    = glbParam.nCh     ; // IT SHOULD BE CALCULATE BASED ON *DIFFERENTS* WAVELENGHS.
    glbParam.nCh     = size_dim[1] ; // 'channels' DIMENSION
    glbParam.nBins   = size_dim[2] ; // 'points' DIMENSION

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
            if ( (retval = nc_get_vara_double((int)ncid, (int)id_var_pr, startDF, countDF, (double*)&dataFile[e][c][0] ) ) )
                ERR(retval);    
        }
    }

    int *Raw_Data_Start_Time = (int*) new int [glbParam.nEvents] ;
    int *Raw_Data_Stop_Time  = (int*) new int [glbParam.nEvents] ;
    oNCL.ReadVar( (int)ncid, (const char*)"Raw_Data_Start_Time", (int*)Raw_Data_Start_Time ) ;
    oNCL.ReadVar( (int)ncid, (const char*)"Raw_Data_Stop_Time" , (int*)Raw_Data_Stop_Time  ) ;

    ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"numEventsToAvg_PDL2", (const char*)"int", (int*)&glbParam.numEventsToAvg ) ;
    glbParam.numEventsToAvg = (glbParam.numEventsToAvg <0) ? glbParam.nEvents : glbParam.numEventsToAvg ;

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

    // READ VARIABLES FROM DE NETCDF FILE INSIDE THE GROUP "L1_Data": MOLECULAR INFORMATION
    int ncid_L1_Data ;
    if ( ( retval = nc_inq_grp_ncid( (int)ncid, (const char*)"L1_Data", (int*)&ncid_L1_Data ) ) )
        ERR(retval);

    if ( ( retval = nc_get_att_double(	(int)ncid, (int)NC_GLOBAL, (const char*)"Range_Resolution", (double*)&glbParam.dr) ) )
        ERR(retval);

    if ( ( retval = nc_get_att_double(	(int)ncid, (int)NC_GLOBAL, (const char*)"Altitude_meter_asl", (double*)&glbParam.siteASL) ) )
        ERR(retval);

    glbParam.indxOffset = (int*) new int [ glbParam.nCh ] ;
    ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"nBinsBkg"   , (const char*)"int"   , (int*)&glbParam.nBinsBkg      ) ;
    ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"indxOffset" , (const char*)"int"   , (int*)glbParam.indxOffset     ) ;
    ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"rInitSig"   , (const char*)"double", (double*)&glbParam.rInitSig   ) ;
    ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"rEndSig"    , (const char*)"double", (double*)&glbParam.rEndSig    ) ;
    glbParam.indxInitSig = (int)round( glbParam.rInitSig /glbParam.dr ) ;
    glbParam.indxEndSig  = (int)round( glbParam.rEndSig  /glbParam.dr ) ;
    glbParam.iLambda = (int*) new int [glbParam.nCh] ;
    oNCL.ReadVar( (int)ncid, (const char*)"Wavelengths", (double*)glbParam.iLambda ) ;

    int indxWL_PDL2[glbParam.nCh], nCh_to_invert ;
    for (int i = 0 ; i <glbParam.nCh; i++)  indxWL_PDL2[i] = -10 ;
    nCh_to_invert = ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"indxWL_PDL2" , (const char*)"int", (int*)&indxWL_PDL2 ) ;

//! ------------------------ CORRECTIONS OF THE LIDAR SIGNAL: LASER OFFSET AND BIAS
	int indxInicFit = glbParam.indxEndSig - glbParam.nBinsBkg ; // glbParam.nBins - glbParam.nBinsBkg ; // 
	int indxEndFit  = glbParam.indxEndSig -1 ; // glbParam.nBins -1 ;

    CDataLevel_2 oDL2 = CDataLevel_2( (strcGlobalParameters*)&glbParam ) ;
    oDL2.indxInitSig  = (int)glbParam.indxInitSig ;
    oDL2.indxEndSig   = (int)glbParam.indxEndSig  ;

    glbParam.r = (double*) new double[glbParam.nBins] ;
    for( int i=0 ; i <glbParam.nBins ; i++ )
        glbParam.r[i] = i*glbParam.dr ;

    double  **pr        = (double**) new double*[glbParam.nEventsAVG] ;
    double  **pr_noBkg  = (double**) new double*[glbParam.nEventsAVG] ;
    for ( int e=0 ; e <glbParam.nEventsAVG ; e++ )
    {
        pr[e]       = (double*) new double[glbParam.nBins] ;
        pr_noBkg[e] = (double*) new double[glbParam.nBins] ;
        for(int b =0 ; b <(glbParam.nBins -glbParam.indxOffset[indxWL_PDL2[0]]) ; b++)
        {
            pr[e][b]        = (double)dataFile_AVG[e][indxWL_PDL2[0]][ b +glbParam.indxOffset[indxWL_PDL2[0]] ] ; // BIN OFFSET CORRECTION; ;
            pr_noBkg[e][b]  = (double)0.0 ;
        }
        for ( int b=(glbParam.nBins -glbParam.indxOffset[indxWL_PDL2[0]]) ; b <glbParam.nBins ; b++ )
        {
            pr[e][b]        = (double)dataFile_AVG[e][indxWL_PDL2[0]][ glbParam.nBins -glbParam.indxOffset[indxWL_PDL2[0]] ] ; // BIN OFFSET CORRECTION;
            pr_noBkg[e][b]  = (double)0.0 ;
        }
        bkgSubstractionMean( (double*)pr[e], indxInicFit, indxEndFit, glbParam.nBins, (double*)&pr_noBkg[e][0] ) ;
        // RANGE CORRECTED: ONLY FOR THE CHANNEL SELECTED FOR THE INVERSION
        for ( int i=0 ; i<glbParam.nBins ; i++ )    oDL2.pr2[e][indxWL_PDL2[0]][i] = (double)(pr_noBkg[e][i] * pow(glbParam.r[i], 2)) ;
    }

    // LOAD MOLECULAR PROFILES FROM THE FILE
    int id_var_nmol ;
    if ( ( retval = nc_inq_varid( (int)ncid_L1_Data, (const char*)"Molecular_Density", (int*)&id_var_nmol ) ) )
        ERR(retval);

    size_t start_mol[1], count_mol[1];
    start_mol[0] = 0;   count_mol[0] = glbParam.nBins ; // BINS
    if ( (retval = nc_get_vara_double( (int)ncid_L1_Data, (int)id_var_nmol, start_mol, count_mol, (double*)&oDL2.nMol[0] ) ) )
        ERR(retval) ;

    for( int c=0 ; c <glbParam.nCh ; c++ )
    { 
        for( int i=0 ; i < glbParam.nBins ; i++ )
        {
            oDL2.beta_Mol [c][i] = (double)(oDL2.nMol[i] * ( 5.45 * pow(10, -32) * pow((550.0/glbParam.iLambda[c] ), 4) ) ) ; // r [1/m*sr]
            oDL2.alpha_Mol[c][i] = (double)(oDL2.beta_Mol[c][i] * 8.0 * 3.1415/3.0) ; // r [1/m]
        }
    }

            if ( (retval = nc_close(ncid)) )
                ERR(retval) ;

    for ( int t=0 ; t <glbParam.nEventsAVG ; t++ )
    {
        cout << endl ;
        glbParam.event_analyzed = t ;
        oDL2.dzr = (glbParam.r[2] - glbParam.r[1]) * 1 ;
        for ( int c=0 ; c <nCh_to_invert ; c++ ) // nCh_to_invert =1 
        {
            cout << endl << "Inverting: " << "\t Event: " << t << "\t Channel: " << indxWL_PDL2[c] << "\t Wavelenght: " << glbParam.iLambda[indxWL_PDL2[c]] ;
            oDL2.Fernald_1983( (strcGlobalParameters*)&glbParam, (int)t , (int)indxWL_PDL2[c] ) ;
        } // for ( int t=0 ; t <glbParam.nEvents ; t++ )
    } // for ( int t=0 ; t <glbParam.nEvents ; t++ )

    oNCL.Save_LALINET_NCDF_PDL2( (string*)&Path_File_Out, (strcGlobalParameters*)&glbParam, (int*)Raw_Data_Start_Time_AVG, (int*)Raw_Data_Stop_Time_AVG, (CDataLevel_2*)&oDL2 ) ;

    printf("\n\n---- lidarAnalisys_PDL2 (END) -----------------------------------------------------------------------------\n\n") ;
	return 0 ;
}
 