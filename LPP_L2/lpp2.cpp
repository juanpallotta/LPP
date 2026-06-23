/**
 * @author Juan V. Pallotta (juanpallotta@gmail.com)
 * @brief Code to process data level 2 lidar signals.
 * @date 2021-06-08
 */

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <cstdlib>
#include <sys/stat.h>
#include <ctime>
#include <assert.h>
#include <netcdf>

// LIDAR LIBRARY ANALISYS
#include "../libLidar/lidarMathFunc.hpp"
#include "../libLidar/libLidar.hpp"
#include "../libLidar/CDataLevel_1.hpp"
#include "../libLidar/CDataLevel_2.hpp"
#include "../libLidar/CNetCDF_Lidar.hpp"
#include "../libLidar/CMolecularData.hpp"

int main( int argc, char *argv[] )
{
    printf("\n\n---- lidarAnalisys_PDL2 (START) -----------------------------------------------------------------------------\n\n") ;
 
    strcGlobalParameters    glbParam  ;

	sprintf( glbParam.FILE_PARAMETERS , "%s", argv[3] ) ;
    sprintf( glbParam.exeFile         , "%s", argv[0] ) ;

    glbParam.Path_File_In  = (char*) new char[500] ;
    glbParam.Path_File_Out = (char*) new char[500] ;
    sprintf( glbParam.Path_File_In , "%s", argv[1] ) ;
    sprintf( glbParam.Path_File_Out, "%s", argv[2] ) ;

    printf("\n Path_File_In --> %s ", glbParam.Path_File_In  ) ;
    printf("\n Path_File_Out --> %s", glbParam.Path_File_Out ) ;
    printf("\n Settings File --> %s\n", glbParam.FILE_PARAMETERS ) ;

    get_full_path_to_soft_coded_values_file( (strcGlobalParameters*)&glbParam ) ;

    char cmdCopy[500] ;
    sprintf( cmdCopy, "cp %s %s", glbParam.Path_File_In, glbParam.Path_File_Out ) ;
    system(cmdCopy) ;

// NETCDF FILE STUFF
    int  ncid, ncid_L1_Data, ncid_L1_Data_MolData ;
    int  retval ;

    if ( ( retval = nc_open(glbParam.Path_File_In, NC_NOWRITE, &ncid) ) )
        ERR(retval);

    if ( ( retval = nc_inq_grp_ncid( (int)ncid, (const char*)"L1_Data", (int*)&ncid_L1_Data ) ) )
        ERR(retval);
    
    if ( ( retval = nc_inq_grp_ncid( (int)ncid_L1_Data, (const char*)"Molecular_Data", (int*)&ncid_L1_Data_MolData ) ) )
        ERR(retval);

    CNetCDF_Lidar   oNCL = CNetCDF_Lidar() ;

    oNCL.Read_GlbParameters( (int)ncid, (strcGlobalParameters*)&glbParam ) ;

    CDataLevel_2    *oDL2 = (CDataLevel_2*) new CDataLevel_2( (strcGlobalParameters*)&glbParam ) ;

    char reference_method_buf[128] = {0} ;
    ReadAnalysisParameter( (const char*)glbParam.FILE_PARAMETERS, "reference_method", "string", reference_method_buf, sizeof(reference_method_buf) ) ;
    oDL2->reference_method.assign(reference_method_buf) ;
	ReadAnalysisParameter( (const char*)glbParam.FILE_PARAMETERS, "heightRef_Inversion_Start_ASL", "double", (double*)&oDL2->heightRef_Inversion_Start_ASL ) ;
	ReadAnalysisParameter( (const char*)glbParam.FILE_PARAMETERS, "heightRef_Inversion_Stop_ASL" , "double", (double*)&oDL2->heightRef_Inversion_Stop_ASL  ) ;

    CMolecularData  *oMolData = (CMolecularData*) new CMolecularData  ( (strcGlobalParameters*)&glbParam  ) ;

    glbParam.evSel = -10 ; // TO GET THE PROFILES IN THE VERTICAL DIRECTION (NO ZENITHAL CORRECTION)
    oMolData->Get_Mol_Data_L2( (strcGlobalParameters*)&glbParam, (CNetCDF_Lidar*)&oNCL, (int)ncid_L1_Data ) ;

    if ( glbParam.numEventsToAvg_PDL1 != glbParam.numEventsToAvg_PDL2 ) // DATA MUST BE READ FROM L0 AND THE CORRECTIONS MUST BE APPLIED AGAIN
    {
        oNCL.Read_L0_data ( (int)ncid, (strcGlobalParameters*)&glbParam, (CDataLevel_2*)oDL2 ) ; // oDL2->data_File_L0[e][c][i] LOADED

        oDL2->oLOp->Average_in_Time_Lidar_Profiles( (strcGlobalParameters*)&glbParam, (double***)oDL2->data_File_L0, (double***)oDL2->data_File_L2, (int*)oDL2->Start_Time_L0, 
                                                    (int*)oDL2->Stop_Time_L0, (int*)oDL2->Start_Time_AVG_L2, (int*)oDL2->Stop_Time_AVG_L2 ) ;

        double  ***pr_corr = nullptr ;
        double  ***pr2     = nullptr ;
        int     dim_sizes[3] = { glbParam.nEventsAVG, glbParam.nCh, glbParam.nBins } ;
        allocate_memory_matrix( (void*)&pr_corr, 3, dim_sizes, "double" ) ;
        allocate_memory_matrix( (void*)&pr2,     3, dim_sizes, "double" ) ;

        double  **ovlp = nullptr;
        int id_var_ovlp ;
        if ( ( nc_inq_varid ( (int)ncid, "Overlap", (int*)&id_var_ovlp ) ) == NC_NOERR )
        {
            int dim_sizes_ovlp[2] = { glbParam.nCh, glbParam.nBins } ;
            allocate_memory_matrix( (void*)&ovlp, 2, dim_sizes_ovlp, "double" ) ;
            oNCL.Read_Overlap( (int)ncid, (strcGlobalParameters*)&glbParam, (int)id_var_ovlp, (double**)ovlp ) ;
        }
        double  **data_Noise = nullptr;
        int id_var_noise ;
        if ( ( nc_inq_varid ( (int)ncid, "Bkg_Noise", (int*)&id_var_noise ) ) == NC_NOERR )
        {
            int dim_sizes_noise[2] = { glbParam.nCh, glbParam.nBins } ;
            allocate_memory_matrix( (void*)&data_Noise, 2, dim_sizes_noise, "double" ) ;
            oNCL.Read_Bkg_Noise( (int)ncid, (strcGlobalParameters*)&glbParam, (int)id_var_noise, (double**)data_Noise ) ;
        }

        printf("\n** L2: Due to numEventsToAvg_PDL1 != numEventsToAvg_PDL2 --> Corrections MUST be applied again from data L0 data **\n") ;
        oDL2->oLOp->Lidar_Signals_Corrections( (strcGlobalParameters*)&glbParam, (CMolecularData*)oMolData, (double**)ovlp, (double**)data_Noise, 
                                               (double***)oDL2->data_File_L2, (double***)pr_corr, (double***)pr2 ) ;

        // GLUING PROCEDURE (ONLY IF ITS SET IN THE CONFIGURATION FILE) ------------------------------------------------------
        oDL2->oLOp->Gluing_Procedure( (strcGlobalParameters*)&glbParam, (CMolecularData*)oMolData, (double***)pr_corr ) ;
        // -------------------------------------------------------------------------------------------------------------------

        // COPYING THE CORRECTED, AVERAGED AND GLUED LIDAR SIGNALS TO THE oDL2->pr2 AND oDL2->pr (USED IN THE INVERSION)
        for ( int e=0 ; e <glbParam.nEventsAVG ; e++ )
        {
            for ( int c=0 ; c <glbParam.nCh ; c++ )
            {
                for ( int i=0 ; i<glbParam.nBins ; i++ )
                    oDL2->pr2[e][c][i] = (double)pr_corr[e][c][i] * pow(glbParam.r[i], 2) ;
            }
        }
    } // if ( glbParam.numEventsToAvg_PDL1 != glbParam.numEventsToAvg_PDL2 ) // DATA WAS READ FROM L0 --> CORRECTIONS MUST BE APPLIED
    else // DATA WAS READ FROM L1 (ALREADY CORRECTED)
    {   // oDL2->data_File_L2 == Raw_Lidar_Data_L1

        oNCL.Read_L1_into_L2( (int)ncid_L1_Data, (strcGlobalParameters*)&glbParam, (CDataLevel_2*)oDL2 ) ;

        printf("\n\n** L2: Due to numEventsToAvg_PDL1 = numEventsToAvg_PDL2 --> Data is taken from L1 group (already corrected) **.\n") ;
        for ( int e=0 ; e <glbParam.nEventsAVG ; e++ )
        {
            for ( int c=0 ; c <glbParam.nCh ; c++ )
            {
                if ( (glbParam.DAQ_Type[c] == 0) || (glbParam.DAQ_Type[c] == 1) ) // IF THE CHANNEL c IS ANALOG OR PHOTON-COUNTING -->
                {
                    for ( int i=0 ; i<glbParam.nBins ; i++ )
                    oDL2->pr2[e][c][i] = (double)oDL2->data_File_L2[e][c][i] * pow(glbParam.r[i], 2) ;
                }
            }
        }
    }
    //! AT THIS POINT, LIDAR DATA LOADED IN oDL2->data_File_L2 WITH THE AVERAGE SET BY numEventsToAvg_PDL2.

    printf("\nDone\n") ;

                        if ( (retval = nc_close(ncid)) )
                            ERR(retval) ;

//! PUT ALL THIS BLOCK IN A METHOD INSIDE oDL2->AERONET ( (strcGlobalParameters*)&glbParam )
    oDL2->AERONET_Data_Preparation( (strcGlobalParameters*)&glbParam ) ;

// INVERSION THROUGH ALL THE AVERAGED LIDAR PROFILES

// for ( int w =0 ; w < glbParam.nCh_to_process_L2; w++)
// {
    glbParam.chSel = glbParam.indxWL_PDL2[0] ;

    for ( int t=0 ; t <glbParam.nEventsAVG ; t++ )
    {
        glbParam.evSel = t ;

        oMolData->Fill_dataMol_L2( (strcGlobalParameters*)&glbParam ) ;

        printf("\n") ;
        oDL2->dzr = oMolData->dataMol.dzr ;
        ReadAnalysisParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"MonteCarlo_N_SigSet_Err", (const char*)"int", (int*)&glbParam.MonteCarlo_N_SigSet_Err ) ;
        if ( glbParam.MonteCarlo_N_SigSet_Err >=1 )
        {
            printf("\nInverting w/Error Analysis:\t Event: %d (%d) \t Channel: %d \t Wavelenght: %d ", t, oDL2->Start_Time_AVG_L2[t], glbParam.chSel, glbParam.iLambda[glbParam.chSel]) ;
                oDL2->MonteCarloRandomError( (strcGlobalParameters*)&glbParam, (strcMolecularData*)&oMolData->dataMol ) ;
        }
        else
        {
            time_t tt ;
            char formatted_time_start[30], formatted_time_stop[30];
            struct tm *utc_tm ;
            
            tt = (time_t)oDL2->Start_Time_AVG_L2[t] ;
            utc_tm = gmtime( (const time_t*)(&tt) ) ;
            strftime(formatted_time_start, sizeof(formatted_time_start), "%H:%M:%S", utc_tm);
            tt = (time_t)oDL2->Stop_Time_AVG_L2[t] ;
            utc_tm = gmtime( (const time_t*)(&tt) ) ;
            strftime(formatted_time_stop, sizeof(formatted_time_stop), "%H:%M:%S", utc_tm);

            printf("\n-----------\n\nInverting:\t Event indx/max_indx: %d/%d (%s - %s UTC) \t Channel: %d \t Wavelenght: %d nm \t Zenith: %lf", t, (glbParam.nEventsAVG-1) , formatted_time_start, formatted_time_stop, glbParam.chSel, glbParam.iLambda[glbParam.chSel], oMolData->dataMol.zenith ) ;

            oDL2->FernaldInversion( (strcGlobalParameters*)&glbParam, (strcMolecularData*)&oMolData->dataMol ) ;

            // if ( glbParam.indxWL_Raman >=0  ) 
            // {
            //     oDL2->Raman_Inversion ( (strcGlobalParameters*)&glbParam, (strcMolecularData*)&oMolData->dataMol ) ;
            // }
        }
    } // for ( int t=0 ; t <glbParam.nEvents ; t++ )
// } // for ( int w =0 ; w < glbParam.nCh_to_process; w++)
    
    printf("\n\n\t \033[34m Lidar Analisys PDL2 Done\033[0m\n\n") ;  // BLUE
    
    printf( "\n\n\033[32mSaving the NetCDF file\033[0m %s\n", glbParam.Path_File_Out ) ;  // GREEN

    oNCL.Save_LALINET_NCDF_PDL2( (char*)glbParam.Path_File_Out, (strcGlobalParameters*)&glbParam, (CDataLevel_2*)oDL2 ) ;

    printf("\n\n\033[34m---- lidarAnalisys_PDL2 (END) -----------------------------------------------------------------------------\033[0m\n\n") ; // BLUE
	return 0 ;
}