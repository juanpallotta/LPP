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
#include "../libLidar/CMolecularData.hpp"
#include "../libLidar/CNetCDF_Lidar.hpp"

int main( int argc, char *argv[] )
{
    printf("\n\n---- lidarAnalisys_PDL1 (START) -----------------------------------------------------------------------------\n\n") ;

    strcGlobalParameters    glbParam  ;
	sprintf( glbParam.FILE_PARAMETERS , "%s", argv[3] ) ;

	string	Path_File_In  ;
	string	Path_File_Out ;

	Path_File_In.assign  ( argv[1] ) ;
	Path_File_Out.assign ( argv[2] ) ;

    printf("\n Path_File_In: %s " , Path_File_In.c_str()  ) ;
    printf("\n Path_File_Out: %s", Path_File_Out.c_str() ) ;
    printf("\n Settings File: %s\n\n", glbParam.FILE_PARAMETERS ) ;

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
    int id_var ;
    if ( ( retval = nc_inq_varid( (int)ncid, (const char*)"Raw_Lidar_Data", (int*)&id_var ) ) )
        ERR(retval) ;
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
    glbParam.nEvents = size_dim[0] ; glbParam.nEventsAVG = glbParam.nEvents ; // 'time' DIMENSION
    glbParam.nCh     = size_dim[1] ; glbParam.nLambda    = glbParam.nCh     ; // IT SHOULD BE CALCULATE BASED ON *DIFFERENTS* WAVELENGHS.
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
            if ( (retval = nc_get_vara_double((int)ncid, (int)id_var, startDF, countDF, (double*)&dataFile[e][c][0] ) ) )
                ERR(retval);    
        }
    }

    int *Raw_Data_Start_Time = (int*) new int [glbParam.nEvents] ; // long *Raw_Data_Start_Time = (long*) new long [glbParam.nEvents] ; // 
    int *Raw_Data_Stop_Time  = (int*) new int [glbParam.nEvents] ; // long *Raw_Data_Stop_Time  = (long*) new long [glbParam.nEvents] ; // 
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
    CDataLevel_1 *oDL1 = (CDataLevel_1*) new CDataLevel_1 ( (strcGlobalParameters*)&glbParam ) ;

    int *Raw_Data_Start_Time_AVG = (int*) new int [glbParam.nEventsAVG] ;   memset( (int*)Raw_Data_Start_Time_AVG, 0, (sizeof(int)*glbParam.nEventsAVG) ) ;
    int *Raw_Data_Stop_Time_AVG  = (int*) new int [glbParam.nEventsAVG] ;   memset( (int*)Raw_Data_Stop_Time_AVG , 0, (sizeof(int)*glbParam.nEventsAVG) ) ;

    oDL1->oLOp->Average_in_Time_Lidar_Profiles( (strcGlobalParameters*)&glbParam, (double***)dataFile, (double***)dataFile_AVG, 
                                    (int*)Raw_Data_Start_Time    , (int*)Raw_Data_Stop_Time, 
                                    (int*)Raw_Data_Start_Time_AVG, (int*)Raw_Data_Stop_Time_AVG
                                  ) ;

    if ( ( retval = nc_get_att_double( (int)ncid, (int)NC_GLOBAL, (const char*)"Range_Resolution", (double*)&glbParam.dr) ) )
        ERR(retval);

    glbParam.r = (double*) new double[glbParam.nBins] ;
    for( int i=1 ; i <=glbParam.nBins ; i++ )
        glbParam.r[i-1] = i*glbParam.dr ;

    if ( ( retval = nc_get_att_double( (int)ncid, (int)NC_GLOBAL, (const char*)"Altitude_meter_asl", (double*)&glbParam.siteASL ) ) )
        ERR(retval);

    int indxWL_PDL1 ;
    ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"indxWL_PDL1", (const char*)"int", (int*)&indxWL_PDL1 ) ;
    assert( indxWL_PDL1 <= (glbParam.nCh -1 ) ) ;

    glbParam.iLambda = (int*) new int [glbParam.nCh] ;
    oNCL.ReadVar( (int)ncid, (const char*)"Wavelengths", (int*)glbParam.iLambda ) ;

    glbParam.indxOffset = (int*) new int [ glbParam.nCh ] ;
    ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"nBinsBkg"   , (const char*)"int"   , (int*)&glbParam.nBinsBkg      ) ;
    ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"indxOffset" , (const char*)"int"   , (int*)glbParam.indxOffset    ) ;
    ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"rInitSig"   , (const char*)"double", (double*)&glbParam.rInitSig   ) ;
    ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"rEndSig"    , (const char*)"double", (double*)&glbParam.rEndSig    ) ;
    glbParam.indxInitSig = (int)round( glbParam.rInitSig /glbParam.dr ) ;
    glbParam.indxEndSig  = (int)round( glbParam.rEndSig  /glbParam.dr );

    glbParam.indxEndSig_ev = (int*)    new int    [ glbParam.nEventsAVG ] ;
    glbParam.rEndSig_ev    = (double*) new double [ glbParam.nEventsAVG ] ;

    double  **data_Noise = (double**) new double*[glbParam.nCh] ;
    int id_var_noise ;
    if ( ( nc_inq_varid ( (int)ncid, "Bkg_Noise", (int*)&id_var_noise ) ) == NC_NOERR )
    {
        for ( int c=0 ; c <glbParam.nCh ; c++ )
        {
            data_Noise[c] = (double*) new double[glbParam.nBins] ;
            for(int b =0 ; b <glbParam.nBins ; b++)
                data_Noise[c][b] = (double)0.0 ;
        }
        size_t start_noise[2], count_noise[2];
        start_noise[0] = 0;   count_noise[0] = 1 ; // glbParam.nCh; 
        start_noise[1] = 0;   count_noise[1] = glbParam.nBins;
        for ( int c=0 ; c <glbParam.nCh ; c++ )
        {
            start_noise[0] =c ;
            if ( (retval = nc_get_vara_double((int)ncid, (int)id_var_noise, start_noise, count_noise, (double*)&data_Noise[c][0] ) ) )
                ERR(retval);    
        }
        glbParam.is_Noise_Data_Loaded = true ;
    }
                        if ( (retval = nc_close(ncid)) )
                            ERR(retval);

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
                pr_corr[e][c][b] = (double)dataFile_AVG[e][c][b +glbParam.indxOffset[c]] ; // -1 BIN OFFSET CORRECTION;
            for ( int b=(glbParam.nBins -glbParam.indxOffset[c]) ; b <glbParam.nBins ; b++ )
                pr_corr[e][c][b] = (double)dataFile_AVG[e][c][glbParam.nBins -glbParam.indxOffset[c]] ; // -1 BIN OFFSET CORRECTION;
        }
    }

    int  **Cloud_Profiles = (int**) new int*[glbParam.nEventsAVG];
    for ( int e=0 ; e <glbParam.nEventsAVG ; e++ )
        Cloud_Profiles[e] = (int*) new int[glbParam.nBins] ;
    double  **RMSE_lay = (double**) new double*[glbParam.nEventsAVG];
    for ( int e=0 ; e <glbParam.nEventsAVG ; e++ )
        RMSE_lay[e] = (double*) new double[glbParam.nBins] ;

    double  *RMSerr_Ref = (double*) new double[glbParam.nEventsAVG ];

    // MOLECULAR DATA READOUT FOR EACH CHANNEL (MUST BE FOR EACH LAMBDA)
    CMolecularData  *oMolData = (CMolecularData*) new CMolecularData  ( (strcGlobalParameters*)&glbParam ) ;

    string  strCompCM ;
    ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"COMPUTE_CLOUD_MASK", (const char*)"string", (char*)strCompCM.c_str() ) ;

    for ( int t=0 ; t <glbParam.nEventsAVG ; t++ )
    {
        cout << endl << endl ;
        glbParam.evSel = t ;
        for ( int c=0 ; c <glbParam.nCh ; c++ )
        {
            glbParam.chSel = c ;
            cout << endl << "Event: " << t << "\t Wavelenght: " << glbParam.iLambda[c] ;
            for ( int i=0 ; i <glbParam.nBins ; i++ )
                evSig.pr[i] = (double)pr_corr[t][c][i] ;

            oMolData->Fill_dataMol( (strcGlobalParameters*)&glbParam ) ;

            if ( glbParam.is_Noise_Data_Loaded == true )
                oDL1->oLOp->MakeRangeCorrected( (strcLidarSignal*)&evSig, (strcGlobalParameters*)&glbParam, (double**)data_Noise, (strcMolecularData*)&oMolData->dataMol ) ;
            else // BIAS REMOVAL BASED ON VARIABLE BkgCorrMethod SET IN FILE THE SETTING FILE PASSED AS ARGUMENT TO lidarAnalysis_PDL2
                oDL1->oLOp->MakeRangeCorrected( (strcLidarSignal*)&evSig, (strcGlobalParameters*)&glbParam, (strcMolecularData*)&oMolData->dataMol ) ;

            if ( oDL1->avg_Points_Cloud_Mask >0 )
                smooth( (double*)&evSig.pr2[0], (int)0, (int)(glbParam.nBins-1), (int)oDL1->avg_Points_Cloud_Mask, (double*)&pr2[t][c][0]     ) ;
            else
            {
                for ( int i=0 ; i <glbParam.nBins ; i++ )
                    pr2[t][c][i] = (double)evSig.pr2[i] ;
            }

            if ( c == indxWL_PDL1 )
            {
                if ( strcmp(strCompCM.c_str(), "YES" ) ==0 )
                {
                    printf("\t --> Getting cloud profile...");
                    oDL1->ScanCloud_RayleightFit( (const double*)evSig.pr_noBkg , (strcGlobalParameters*)&glbParam, (strcMolecularData*)&oMolData->dataMol ) ;
                }
                else
                    printf("\n Cloud profiles are not computed. \n") ;

                if ( (oDL1->cloudProfiles[t].nClouds) >0 )
                    printf(" %d clouds detected at %lf m asl @ %lf deg zenithal angle", oDL1->cloudProfiles[t].nClouds, oMolData->dataMol.zr[ oDL1->cloudProfiles[t].indxInitClouds[0] ], glbParam.aZenithAVG[t] ) ;
                else
                    printf(" NO clouds detected at %lf zenithal angle", glbParam.aZenithAVG[t]  ) ;

                for( int b=0 ; b <glbParam.nBins ; b++ )
                {
                    Cloud_Profiles[t][b] = (int)    oDL1->cloudProfiles[t].clouds_ON[b] ;
                    RMSE_lay      [t][b] = (double) oDL1->cloudProfiles[t].test_1[b]    ; // RMSE_lay
                }
                RMSerr_Ref[t] = (double)oDL1->errRefBkg ;
            }
        } // for ( int c=0 ; c <glbParam.nCh ; c++ )
    } // for ( int t=0 ; t <glbParam.nEventsAVG ; t++ )

    glbParam.evSel = (int) -10; // TO RETRIEVE THE MOLECULAR PROFILE IN A ZENITHAL=0
    glbParam.chSel = indxWL_PDL1 ;
    oMolData->Fill_dataMol( (strcGlobalParameters*)&glbParam ) ; // oMolData->Fill_dataMol( (strcGlobalParameters*)&glbParam, (int)indxWL_PDL1 ) ;
    // oNCL.Save_LALINET_NCDF_PDL1( (string*)&Path_File_In, (string*)&Path_File_Out, (strcGlobalParameters*)&glbParam, (double**)RMSE_lay, (double*)RMSerr_Ref, (int**)Cloud_Profiles,
    //                              (double***)pr_corr, (double***)pr2, (int*)Raw_Data_Start_Time_AVG, (int*)Raw_Data_Stop_Time_AVG, (CMolecularData*)oMolData ) ;
    oNCL.Save_LALINET_NCDF_PDL1( (string*)&Path_File_Out, (strcGlobalParameters*)&glbParam, (double**)RMSE_lay, (double*)RMSerr_Ref, (int**)Cloud_Profiles,
                                 (double***)pr_corr, (int*)Raw_Data_Start_Time_AVG, (int*)Raw_Data_Stop_Time_AVG, (CMolecularData*)oMolData ) ;

    for ( int e=0; e <glbParam.nEventsAVG ; e++  )
        delete [] Cloud_Profiles[e] ;
    delete [] Cloud_Profiles ;

    delete Raw_Data_Start_Time      ;
    delete Raw_Data_Stop_Time       ;
    delete Raw_Data_Start_Time_AVG  ;
    delete Raw_Data_Stop_Time_AVG   ;

    cout << endl << endl << "\tLidar Analisys PDL1 Done" << endl << endl ;
    printf("\n\n---- lidarAnalisys_PDL1 (END) -----------------------------------------------------------------------------\n\n") ;
    
	return 0 ;
}