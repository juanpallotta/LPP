
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
	sprintf( glbParam.exeFile         , "%s", argv[0] ) ;

	string	Path_File_In  ;
	string	Path_File_Out ;

	Path_File_In.assign  ( argv[1] ) ;
	Path_File_Out.assign ( argv[2] ) ;

    printf("\n Path_File_In --> %s ", Path_File_In.c_str()  ) ;
    printf("\n Path_File_Out --> %s", Path_File_Out.c_str() ) ;
    printf("\n Settings File --> %s\n", glbParam.FILE_PARAMETERS ) ;

    char cmdCopy[500] ;
    sprintf( cmdCopy, "cp %s %s", Path_File_In.c_str(), Path_File_Out.c_str() ) ;
    system(cmdCopy) ;

// NETCDF FILE STUFF
    int  ncid   ;
    int  retval ;
    
    if ( ( retval = nc_open( Path_File_In.c_str(), NC_NOWRITE, &ncid ) ) )
        ERR(retval);

    CNetCDF_Lidar   oNCL = CNetCDF_Lidar() ;
    ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"numEventsToAvg_PDL1", (const char*)"int", (int*)&glbParam.numEventsToAvg ) ;
    oNCL.Read_GlbParameters( (int)ncid, (strcGlobalParameters*)&glbParam ) ;

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
// cout<<endl<<"(3) glbParam.nEvents: "<< glbParam.nEvents << endl;
// cout<<endl<<"(3) glbParam.nEventsAVG: "<< glbParam.nEventsAVG << endl;
// cout<<endl<<"(3) glbParam.nCh: "<< glbParam.nCh << endl;
// cout<<endl<<"(3) glbParam.nBins: "<< glbParam.nBins << endl;
    CDataLevel_1 *oDL1 = (CDataLevel_1*) new CDataLevel_1 ( (strcGlobalParameters*)&glbParam ) ;
    // ONLY USED IF glbParam.numEventsToAvg !=1
    double  ***data_File_L0      ;
    int *Raw_Data_Start_Time ;
    int *Raw_Data_Stop_Time  ;

    double  ***data_File_L1 = (double***) new double**[glbParam.nEventsAVG] ;
    for ( int e=0 ; e <glbParam.nEventsAVG ; e++ )
    {
        data_File_L1[e] = (double**) new double*[glbParam.nCh] ;
        for ( int c=0 ; c <glbParam.nCh ; c++ )
        {
            data_File_L1[e][c] = (double*) new double[glbParam.nBins] ;
            for(int b =0 ; b <glbParam.nBins ; b++)
                data_File_L1[e][c][b] = (double)0.0 ;
        }
    }
    int *Raw_Data_Start_Time_AVG = (int*) new int [glbParam.nEventsAVG] ;   memset( (int*)Raw_Data_Start_Time_AVG, 0, (sizeof(int)*glbParam.nEventsAVG) ) ;
    int *Raw_Data_Stop_Time_AVG  = (int*) new int [glbParam.nEventsAVG] ;   memset( (int*)Raw_Data_Stop_Time_AVG , 0, (sizeof(int)*glbParam.nEventsAVG) ) ;

    if ( glbParam.numEventsToAvg ==1 ) // PDL0 --> PDL1 WHITHOUT AVERAGING
    {
        oNCL.ReadVar( (int)ncid, (const char*)"Raw_Data_Start_Time", (int*)Raw_Data_Start_Time_AVG ) ;
        oNCL.ReadVar( (int)ncid, (const char*)"Raw_Data_Stop_Time" , (int*)Raw_Data_Stop_Time_AVG  ) ;

        oNCL.ReadVar( (int)ncid, (const char*)"Zenith" , (double*)glbParam.aZenithAVG  ) ;
        oNCL.ReadVar( (int)ncid, (const char*)"Azimuth", (double*)glbParam.aAzimuthAVG ) ;

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
                if ( (retval = nc_get_vara_double((int)ncid, (int)id_var, startDF, countDF, (double*)&data_File_L1[e][c][0] ) ) )
                    ERR(retval);    
            }
        }
    } // if ( glbParam.numEventsToAvg ==1 )
    else // PDL0 --> PDL1 BY AVERAGING glbParam.numEventsToAvg PROFILES
    {
        data_File_L0 = (double***) new double**[glbParam.nEvents] ;
        for ( int e=0 ; e <glbParam.nEvents ; e++ )
        {
            data_File_L0[e] = (double**) new double*[glbParam.nCh] ;
            for ( int c=0 ; c <glbParam.nCh ; c++ )
            {
                data_File_L0[e][c] = (double*) new double[glbParam.nBins] ;
                for(int b =0 ; b <glbParam.nBins ; b++)
                    data_File_L0[e][c][b] = (double)0;
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
                if ( (retval = nc_get_vara_double((int)ncid, (int)id_var, startDF, countDF, (double*)&data_File_L0[e][c][0] ) ) )
                    ERR(retval);    
            }
        }

        Raw_Data_Start_Time = (int*) new int [glbParam.nEvents] ;
        Raw_Data_Stop_Time  = (int*) new int [glbParam.nEvents] ;
        oNCL.ReadVar( (int)ncid, (const char*)"Raw_Data_Start_Time", (int*)Raw_Data_Start_Time ) ;
        oNCL.ReadVar( (int)ncid, (const char*)"Raw_Data_Stop_Time" , (int*)Raw_Data_Stop_Time  ) ;

        oDL1->oLOp->Average_in_Time_Lidar_Profiles( (strcGlobalParameters*)&glbParam, (double***)data_File_L0, (double***)data_File_L1, 
                                        (int*)Raw_Data_Start_Time    , (int*)Raw_Data_Stop_Time, 
                                        (int*)Raw_Data_Start_Time_AVG, (int*)Raw_Data_Stop_Time_AVG
                                    ) ;
    }

    ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"indxWL_PDL1", (const char*)"int", (int*)&glbParam.indxWL_PDL1 ) ;
    assert( glbParam.indxWL_PDL1 <= (glbParam.nCh -1 ) ) ;

    double  **data_Noise ;
    int id_var_noise ;
    if ( ( nc_inq_varid ( (int)ncid, "Bkg_Noise", (int*)&id_var_noise ) ) == NC_NOERR )
    {
        data_Noise = (double**) new double*[glbParam.nCh] ;
        for ( int c=0 ; c <glbParam.nCh ; c++ )
            data_Noise[c] = (double*) new double[glbParam.nBins] ;

        oNCL.Read_Bkg_Noise( (int)ncid, (strcGlobalParameters*)&glbParam, (int)id_var_noise, (double**)data_Noise ) ;
    }

    double  **ovlp ;
    int id_var_ovlp ;
    if ( ( nc_inq_varid ( (int)ncid, "Overlap", (int*)&id_var_ovlp ) ) == NC_NOERR )
    {
        ovlp = (double**) new double*[glbParam.nCh] ;
        for ( int c=0 ; c <glbParam.nCh ; c++ )
            ovlp[c] = (double*) new double[glbParam.nBins] ;

        oNCL.Read_Overlap( (int)ncid, (strcGlobalParameters*)&glbParam, (int)id_var_ovlp, (double**)ovlp ) ;
    }

                        if ( (retval = nc_close(ncid)) )
                            ERR(retval);

//! /////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // MOLECULAR DATA READOUT FOR EACH CHANNEL (MUST BE FOR EACH LAMBDA)
    CMolecularData  *oMolData = (CMolecularData*) new CMolecularData  ( (strcGlobalParameters*)&glbParam ) ;
    oMolData->Get_Mol_Data_L1( (strcGlobalParameters*)&glbParam ) ;

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
            }
    }
    // LIDAR SIGNAL CORRECTIONS:
    // - OFFSET
    // - PHOTON COUNTING DESATURATION
    // - BACKGROUND NOISE
    // - BIAS 
    // - OVERLAP
    printf("\n") ;
    oDL1->oLOp->Lidar_Signals_Corrections( (strcGlobalParameters*)&glbParam, (CMolecularData*)oMolData, (double**)ovlp, (double**)data_Noise, (double***)data_File_L1, (double***)pr_corr, (double***)pr2 ) ;

printf("\n\n") ;

// START GLUING PROCEDURE (ONLY IF ITS SET IN THE CONFIGURATION FILE) ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	glbParam.indx_gluing_Low_AN     = (int*) new int[ glbParam.nCh ] ;
	glbParam.indx_gluing_High_PHO   = (int*) new int[ glbParam.nCh ] ;

	ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"MAX_TOGGLE_RATE_MHZ", (const char*)"double", (double*)&glbParam.MAX_TOGGLE_RATE_MHZ ) ;
	ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"MIN_TOGGLE_RATE_MHZ", (const char*)"double", (double*)&glbParam.MIN_TOGGLE_RATE_MHZ ) ;

    int nIndxsToGlue_Low_AN   = ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"indx_Gluing_Low_AN"  , (const char*)"int", (int*)glbParam.indx_gluing_Low_AN   ) ;
    int nIndxsToGlue_High_PHO = ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"indx_Gluing_High_PHO", (const char*)"int", (int*)glbParam.indx_gluing_High_PHO ) ;

    // CHECK IF THE GLUING INFORMATION IS CORRECTLY SET
    if  ( ( (nIndxsToGlue_Low_AN == nIndxsToGlue_High_PHO) && (nIndxsToGlue_Low_AN >0) && (nIndxsToGlue_High_PHO >0) )
            &&
          ( (glbParam.MAX_TOGGLE_RATE_MHZ >0) && (glbParam.MIN_TOGGLE_RATE_MHZ >0) && (glbParam.MIN_TOGGLE_RATE_MHZ < glbParam.MAX_TOGGLE_RATE_MHZ) )
        )
    {
        printf("Gluing procedure:") ;
        for (int c =0; c <nIndxsToGlue_High_PHO ; c++)
        {
            if ( ( glbParam.iLambda[glbParam.indx_gluing_Low_AN[c]]     == glbParam.iLambda[glbParam.indx_gluing_High_PHO[c]] ) &&
                 ( glbParam.DAQ_Type[glbParam.indx_gluing_Low_AN[c]]    == 0 )                                                  &&
                 ( glbParam.DAQ_Type[glbParam.indx_gluing_High_PHO[c]]  == 1 )     )
            {
                glbParam.nPair_Ch_to_Glue = nIndxsToGlue_Low_AN ;
                for ( int e =0; e <glbParam.nEventsAVG; e++)
                {
                    glbParam.evSel = e ;
                    oDL1->oLOp->GluingLidarSignals( (strcGlobalParameters*)&glbParam, (double***)pr_corr ) ;
                }
            }
            else
            {
                printf( "\nGLUING: indexes %d of the gluing data do not correspond to the same wavelength and/or acquitition type.\n", c) ;
                printf( "\t indx_Gluing_Low_AN[%d]   --> %d nm\n", c, glbParam.iLambda[ glbParam.indx_gluing_Low_AN  [c] ] ) ;
                printf( "\t indx_Gluing_High_PHO[%d] --> %d nm\n", c, glbParam.iLambda[ glbParam.indx_gluing_High_PHO[c] ] ) ;
            }
        }
    }
    else
    {
        printf("\nGluing was *NOT* performed due to its configuration variables in: %s", glbParam.FILE_PARAMETERS ) ;
        if ( nIndxsToGlue_Low_AN <0 )
            printf("\n\t Variable indx_Gluing_Low_AN is commented or not set in setting file." ) ;
        if ( nIndxsToGlue_High_PHO <0 )
            printf("\n\t Variable indx_Gluing_High_PHO is commented or not set in setting file." ) ;
        if ( (nIndxsToGlue_Low_AN != nIndxsToGlue_High_PHO) && (nIndxsToGlue_Low_AN >0) && (nIndxsToGlue_High_PHO >0) )
            printf("\n\t Different numbers of elements in the arrarys indx_Gluing_Low_AN and indx_Gluing_High_PHO" ) ;
        printf("\n\t NO gluing is applied for this analysis.\n\n" ) ;
    }
// END GLUING ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int  **Cloud_Profiles = (int**) new int*[glbParam.nEventsAVG];
    for ( int e=0 ; e <glbParam.nEventsAVG ; e++ )
        Cloud_Profiles[e] = (int*) new int[glbParam.nBins] ;
    double  **RMSE_lay = (double**) new double*[glbParam.nEventsAVG];
    for ( int e=0 ; e <glbParam.nEventsAVG ; e++ )
        RMSE_lay[e] = (double*) new double[glbParam.nBins] ;

    double  *RMSerr_Ref = (double*) new double[glbParam.nEventsAVG ];

    string  strCompCM ;
    ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"COMPUTE_CLOUD_MASK", (const char*)"string", (char*)strCompCM.c_str() ) ;

    for ( int t=0 ; t <glbParam.nEventsAVG ; t++ )
    {
        glbParam.evSel = t ;

        for ( int c=0 ; c <glbParam.nCh ; c++ )
        {
            glbParam.chSel = c ;
            printf("\nEvent: %d/%d \t Wavelenght: %d", t, glbParam.nEventsAVG, glbParam.iLambda[c] ) ;

            oMolData->Fill_dataMol_L1( (strcGlobalParameters*)&glbParam ) ;

            if ( c == glbParam.indxWL_PDL1 )
            {
                if ( strcmp(strCompCM.c_str(), "YES" ) ==0 )
                {
                    printf("   --> Getting cloud profile...");
                    oDL1->ScanCloud_RayleightFit( (const double*)&pr_corr[t][c][0], (strcGlobalParameters*)&glbParam, (strcMolecularData*)&oMolData->dataMol ) ;
                }
                else
                    printf("\t Cloud profiles are not computed. \t") ;

                if ( (oDL1->cloudProfiles[t].nClouds) >0 )
                    printf(" %d clouds detected with a cloud base height at %lf m asl @ %lf deg zenithal angle ", oDL1->cloudProfiles[t].nClouds, oMolData->dataMol.zr[ oDL1->cloudProfiles[t].indxInitClouds[0] ], glbParam.aZenithAVG[t] ) ;
                else
                    printf(" NO clouds detected at %lf zenithal angle ", glbParam.aZenithAVG[t]  ) ;

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
    glbParam.chSel = glbParam.indxWL_PDL1 ;
    oMolData->Fill_dataMol_L1( (strcGlobalParameters*)&glbParam ) ;

    oNCL.Save_LALINET_NCDF_PDL1( (string*)&Path_File_Out, (strcGlobalParameters*)&glbParam, (double**)RMSE_lay, (double*)RMSerr_Ref, (int**)Cloud_Profiles,
                                 (double***)pr_corr, (int*)Raw_Data_Start_Time_AVG, (int*)Raw_Data_Stop_Time_AVG, (CMolecularData*)oMolData ) ;

    // for ( int e=0; e <glbParam.nEventsAVG ; e++  )
    //     delete [] Cloud_Profiles[e] ;
    // delete [] Cloud_Profiles ;

    // delete Raw_Data_Start_Time      ;
    // delete Raw_Data_Stop_Time       ;
    // delete Raw_Data_Start_Time_AVG  ;
    // delete Raw_Data_Stop_Time_AVG   ;

    cout << endl << endl << "\tLidar Analisys PDL1 Done" << endl << endl ;
    printf("\n\n---- lidarAnalisys_PDL1 (END) -----------------------------------------------------------------------------\n\n") ;
    
	return 0 ;
}