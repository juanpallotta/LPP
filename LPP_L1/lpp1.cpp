/**
 * @author Juan V. Pallotta (juanpallotta@gmail.com)
 * @brief Code to process data level 1 lidar signals.
 * @date 2021-06-08
 */

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <cstdlib>
#include <sys/stat.h>
#include <ctime>
#include <assert.h> 
#include <vector>

#include <netcdf>

// LIDAR LIBRARY ANALISYS
#include "../libLidar/lidarMathFunc.hpp"
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

    glbParam.Path_File_In  = (char*) new char[500] ;
    glbParam.Path_File_Out = (char*) new char[500] ;
    sprintf( glbParam.Path_File_In,  "%s", argv[1] ) ;
    sprintf( glbParam.Path_File_Out, "%s", argv[2] ) ;

    printf("\n Path_File_In  --> %s " , glbParam.Path_File_In    ) ;
    printf("\n Path_File_Out --> %s"  , glbParam.Path_File_Out   ) ;
    printf("\n Settings File --> %s\n", glbParam.FILE_PARAMETERS ) ;

    get_full_path_to_soft_coded_values_file( (strcGlobalParameters*)&glbParam ) ;

    char cmdCopy[500] ;
    sprintf( cmdCopy, "cp %s %s", glbParam.Path_File_In, glbParam.Path_File_Out ) ;
    system(cmdCopy) ;

// NETCDF FILE STUFF
    int  ncid   ;
    int  retval ;
    
    if ( ( retval = nc_open( glbParam.Path_File_In, NC_NOWRITE, &ncid ) ) )
        ERR(retval);

    CNetCDF_Lidar   oNCL = CNetCDF_Lidar() ;
    ReadAnalysisParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"numEventsToAvg_PDL1", (const char*)"int", (int*)&glbParam.numEventsToAvg ) ;
    oNCL.Read_GlbParameters( (int)ncid, (strcGlobalParameters*)&glbParam ) ;

    // READ VARIABLES FROM DE NETCDF INPUT FILE
    int id_var_raw_lidar ;
    if ( ( retval = nc_inq_varid( (int)ncid, (const char*)"Raw_Lidar_Data", (int*)&id_var_raw_lidar ) ) )
        ERR(retval) ;

    int num_dim_var ;
    if ( ( retval = nc_inq_varndims( (int)ncid, (int)id_var_raw_lidar, (int*)&num_dim_var ) ) )
        ERR(retval);
    // cout << endl << "num_dim_var: " << num_dim_var << "     id_var_raw_lidar: " << id_var_raw_lidar << endl ;
    assert( num_dim_var ==3 ) ; // SI SE CUMPLE, SIGUE.

    std::vector<int> id_dim(num_dim_var);
    if ( ( retval = nc_inq_vardimid( (int)ncid, (int)id_var_raw_lidar, (int*)id_dim.data() ) ) )
        ERR(retval);

    std::vector<int> size_dim(num_dim_var, 1);
    for( int d=0 ; d <num_dim_var ; d++ )
    {
        size_t len;
        if ( ( retval = nc_inq_dimlen( (int)ncid, (int)id_dim[d], &len ) ) )
            ERR(retval);
        size_dim[d] = (int)len;
        // printf( "\nsize_dim[%d]: %d", d, size_dim[d] ) ;
    }

    CDataLevel_1 *oDL1 = (CDataLevel_1*) new CDataLevel_1 ( (strcGlobalParameters*)&glbParam ) ;

    // ONLY USED IF glbParam.numEventsToAvg !=1
    int *Raw_Data_Start_Time ;
    int *Raw_Data_Stop_Time  ;

    double  ***data_File_L1 ;
    int     dim_sizes[3] = {glbParam.nEventsAVG, glbParam.nCh, glbParam.nBins} ;
    allocate_memory_matrix( (void*)&data_File_L1, 3, dim_sizes, "double" ) ;

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
                if ( (retval = nc_get_vara_double((int)ncid, (int)id_var_raw_lidar, startDF, countDF, (double*)&data_File_L1[e][c][0] ) ) )
                    ERR(retval);    
            }
            glbParam.temp_K_agl_AVG [e] = glbParam.temp_K_agl [e] ;
            glbParam.pres_Pa_agl_AVG[e] = glbParam.pres_Pa_agl[e] ;
        }
    } // if ( glbParam.numEventsToAvg ==1 )
    else // PDL0 --> PDL1 BY AVERAGING glbParam.numEventsToAvg PROFILES
    {
        double  ***data_File_L0 = nullptr ;
        {
            int dim_sizes[3] = {glbParam.nEvents, glbParam.nCh, glbParam.nBins} ;
            allocate_memory_matrix( (void*)&data_File_L0, 3, dim_sizes, "double" ) ;
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
                if ( (retval = nc_get_vara_double((int)ncid, (int)id_var_raw_lidar, startDF, countDF, (double*)&data_File_L0[e][c][0] ) ) )
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
    } // else // PDL0 --> PDL1 BY AVERAGING glbParam.numEventsToAvg PROFILES

    // ReadAnalysisParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"indxWL_PDL1", (const char*)"int", (int*)&glbParam.indxWL_PDL1[0] ) ;
    // assert( glbParam.indxWL_PDL1 <= (glbParam.nCh -1 ) ) ;

    double  **data_Noise = nullptr ;
    int id_var_noise ;
    if ( ( nc_inq_varid ( (int)ncid, "Bkg_Noise", (int*)&id_var_noise ) ) == NC_NOERR )
    {
        int dim_sizes[2] = { glbParam.nCh, glbParam.nBins } ;
        allocate_memory_matrix( (void*)&data_Noise, 2, dim_sizes, "double" ) ;
        oNCL.Read_Bkg_Noise( (int)ncid, (strcGlobalParameters*)&glbParam, (int)id_var_noise, (double**)data_Noise ) ;
    }

    double  **ovlp = nullptr ;
    int id_var_ovlp ;
    if ( ( nc_inq_varid ( (int)ncid, "Overlap", (int*)&id_var_ovlp ) ) == NC_NOERR )
    {
        int dim_sizes[2] = { glbParam.nCh, glbParam.nBins } ;
        allocate_memory_matrix( (void*)&ovlp, 2, dim_sizes, "double" ) ;
        oNCL.Read_Overlap( (int)ncid, (strcGlobalParameters*)&glbParam, (int)id_var_ovlp, (double**)ovlp ) ;
    }

                        if ( (retval = nc_close(ncid)) )
                            ERR(retval);

//! /////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // MOLECULAR DATA READOUT FOR EACH CHANNEL (MUST BE FOR EACH LAMBDA)
    CMolecularData  *oMolData = (CMolecularData*) new CMolecularData  ( (strcGlobalParameters*)&glbParam ) ;
    glbParam.evSel = -10 ;
    oMolData->Get_Mol_Data_L1( (strcGlobalParameters*)&glbParam ) ;

    double ***pr_corr = nullptr ;
    double ***pr2     = nullptr ;
    {
        int dim_sizes[3] = { glbParam.nEventsAVG, glbParam.nCh, glbParam.nBins } ;
        allocate_memory_matrix( (void*)&pr_corr, 3, dim_sizes, "double" ) ;
        allocate_memory_matrix( (void*)&pr2,     3, dim_sizes, "double" ) ;
    }
    for ( int e=0 ; e <glbParam.nEventsAVG ; e++ )
        for ( int c=0 ; c <glbParam.nCh ; c++ )
            memcpy(pr_corr[e][c], data_File_L1[e][c], sizeof(double) * glbParam.nBins) ;

    // LIDAR SIGNAL CORRECTIONS:
    // - OFFSET
    // - PHOTON COUNTING DESATURATION
    // - BACKGROUND NOISE
    // - BIAS 
    // - OVERLAP
    oDL1->oLOp->Lidar_Signals_Corrections( (strcGlobalParameters*)&glbParam, (CMolecularData*)oMolData, (double**)ovlp, (double**)data_Noise, (double***)data_File_L1, (double***)pr_corr, (double***)pr2 ) ;
    printf("\n\n") ;

    if ( glbParam.DAQ_Type[glbParam.indxWL_PDL1[0]] !=0 ) 
        printf( "\nData Level 1 - Cloud-Mask: the channel selected (%d) is not analog\n", glbParam.indxWL_PDL1[0] ) ;

// GLUING PROCEDURE (ONLY IF ITS SET IN THE CONFIGURATION FILE) ----------------------------------------------------------------------------------------------
    oDL1->oLOp->Gluing_Procedure( (strcGlobalParameters*)&glbParam, (CMolecularData*)oMolData, (double***)pr_corr ) ;
// -----------------------------------------------------------------------------------------------------------------------------------------------------------

// START DEPOLARIZATION PROCEDURE (ONLY IF ITS SET IN THE CONFIGURATION FILE) ------------------------------------------------------------------------------------
        glbParam.indx_Ch_Pol_S    = (int*)    new int   [ glbParam.nCh ] ;
        glbParam.indx_Ch_Pol_P    = (int*)    new int   [ glbParam.nCh ] ;
        glbParam.Pol_Cal_Constant = (double*) new double[ glbParam.nCh ] ;
        int nS   = ReadAnalysisParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"indx_Ch_Pol_S"   , (const char*)"int"   , (int*)   glbParam.indx_Ch_Pol_S    ) ;
        int nP   = ReadAnalysisParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"indx_Ch_Pol_P"   , (const char*)"int"   , (int*)   glbParam.indx_Ch_Pol_P    ) ;
        int nCal = ReadAnalysisParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"Pol_Cal_Constant", (const char*)"double", (double*)glbParam.Pol_Cal_Constant ) ;
    
        if  ( (nS == nP) && (nCal >0) && (nCal == nS) ) // CHECK IF THE DEPOLARIZATION INFORMATION IS CORRECTLY SET
        {
            for (int e =0; e <glbParam.nEventsAVG ; e++)
            {
                for (int i =0; i <glbParam.nBins; i++)
                    pr_corr[e][glbParam.indx_Ch_Pol_P[0]][i] = pr_corr[e][glbParam.indx_Ch_Pol_P[0]][i] + glbParam.Pol_Cal_Constant[0] * pr_corr[e][glbParam.indx_Ch_Pol_S[0]][i] ;
            }
        }
        else
        {
            printf("\n\nDepolarization setting: No depolarization information provided.\n\n") ;
        }
// -----------------------------------------------------------------------------------------------------------------------------------------------------------

// PREPARE THE PROFILES FOR LAYER MASK DETECTION
    {
        int dim_sizes[2] = { glbParam.nEventsAVG, glbParam.nBins } ;
        allocate_memory_matrix( (void*)&oDL1->pr_for_cloud_mask, 2, dim_sizes, "double" ) ;
    }

    for (int e=0 ; e <glbParam.nEventsAVG ; e++)
        memcpy(oDL1->pr_for_cloud_mask[e], pr_corr[e][glbParam.indxWL_PDL1[0]], sizeof(double) * glbParam.nBins) ;

    printf("\n========================================> Layer detection algorithm <================================================================================") ;
    for ( int t=0 ; t <glbParam.nEventsAVG ; t++ )
    {
        glbParam.evSel = t ;
        printf("\n") ;
        for ( int c=0 ; c <glbParam.nCh ; c++ )
        {
            glbParam.chSel = c ;
            printf("\nEvent: %d/%d \t Ch= %02d - Wavelenght: %04d nm", t, glbParam.nEventsAVG-1, c, glbParam.iLambda[c] ) ;

            if ( c == glbParam.indxWL_PDL1[0] )
            {
                oMolData->Fill_dataMol_L1( (strcGlobalParameters*)&glbParam ) ;
                if ( strcmp( oDL1->oLOp->compute_layer_mask.c_str(), "YES" ) ==0 )
                {
                    printf("   --> Getting layers limits") ;
                    if ( strcmp(  oDL1->oLOp->compute_pbl_mask.c_str(), "YES" ) ==0 )    printf(" and PBL layer. ") ;

                    oDL1->oLOp->Layer_Mask( (double*)&oDL1->pr_for_cloud_mask[t][0], (strcMolecularData*)&oMolData->dataMol, (strcGlobalParameters*)&glbParam ) ;
                }
                else
                    printf("\t Cloud profiles are not computed. \t") ;

                if ( (oDL1->oLOp->cloudProfiles[t].nClouds) >0 )
                    // printf( "\t\t\t\t\t %d layer detected ", oDL1->oLOp->cloudProfiles[t].nClouds ) ;
                    printf(". Lower layer detected at height %lf m asl ", oMolData->dataMol.zr[ oDL1->oLOp->cloudProfiles[t].indxInitClouds[0] ] ) ;
                else
                    printf(" NO layer detected at %lf zenithal angle ", glbParam.aZenithAVG[t]  ) ;
            }
        } // for ( int c=0 ; c <glbParam.nCh ; c++ )
    } // for ( int t=0 ; t <glbParam.nEventsAVG ; t++ )

    printf("\n\n\t\033[34m Lidar Analisys PDL1 Done\033[0m\n\n") ; // BLUE
    printf( "\n\n\033[32mSaving the NetCDF file\033[0m %s\n", glbParam.Path_File_Out ) ;  // GREEN

    glbParam.evSel = -10 ; // SET THE EVENT SELECTED NEGATIVE TO FILLING OF THE MOLECUALR DATA AT ZENITH=0
    oMolData->Fill_dataMol_L1( (strcGlobalParameters*)&glbParam ) ;

    oNCL.Save_LALINET_NCDF_PDL1( (char*)glbParam.Path_File_Out, (strcGlobalParameters*)&glbParam, (CDataLevel_1*)oDL1, (double***)pr_corr, 
                                 (int*)Raw_Data_Start_Time_AVG, (int*)Raw_Data_Stop_Time_AVG, (CMolecularData*)oMolData ) ;

    printf("\n\n\033[34m---- lidarAnalisys_PDL1 (END) -----------------------------------------------------------------------------\033[0m\n\n") ;  // BLUE
    
	return 0 ;
}
/*
    printf("\033[31mRed text\033[0m\n");
    printf("\033[32mGreen text\033[0m\n");
    printf("\033[33mYellow text\033[0m\n");
    printf("\033[34mBlue text\033[0m\n");
    printf("\033[0mNormal text\n");
*/