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
#include "../libLidar/CDataLevel_2.hpp"
#include "../libLidar/CNetCDF_Lidar.hpp"
#include "../libLidar/CMolecularData.hpp"

int main( int argc, char *argv[] )
{
    printf("\n\n---- lidarAnalisys_PDL2 (START) -----------------------------------------------------------------------------\n\n") ;
 
    strcGlobalParameters    glbParam  ;

	sprintf( glbParam.FILE_PARAMETERS , "%s", argv[3] ) ;
    sprintf( glbParam.exeFile         , "%s", argv[0] ) ;

	string	Path_File_In  ;
	string	Path_File_Out ;

	Path_File_In.assign  ( argv[1] ) ;
	Path_File_Out.assign ( argv[2] ) ;

    printf("\n Path_File_In --> %s ", Path_File_In.c_str()  ) ;
    printf("\n Path_File_Out --> %s", Path_File_Out.c_str() ) ;
    printf("\n Settings File --> %s", glbParam.FILE_PARAMETERS ) ;

    char cmdCopy[500] ;
    sprintf( cmdCopy, "cp %s %s", Path_File_In.c_str(), Path_File_Out.c_str() ) ;
    system(cmdCopy) ;

// NETCDF FILE STUFF
    int  ncid, ncid_L1_Data ;
    int  retval ;

    if ( ( retval = nc_open(Path_File_In.c_str(), NC_NOWRITE, &ncid) ) )
        ERR(retval);
    if ( ( retval = nc_inq_grp_ncid( (int)ncid, (const char*)"L1_Data", (int*)&ncid_L1_Data ) ) )
        ERR(retval);

    CNetCDF_Lidar   oNCL = CNetCDF_Lidar() ;
    CDataLevel_2    *oDL2 ;

    int numEventsToAvg_PDL1 ;
    ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"numEventsToAvg_PDL1", (const char*)"int", (int*)&numEventsToAvg_PDL1 ) ;
    ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"numEventsToAvg_PDL2", (const char*)"int", (int*)&glbParam.numEventsToAvg ) ;

    oNCL.Read_GlbParameters( (int)ncid, (strcGlobalParameters*)&glbParam ) ;

    oDL2 = (CDataLevel_2*) new CDataLevel_2( (strcGlobalParameters*)&glbParam ) ;

    int indxWL_PDL2[glbParam.nCh], nCh_to_invert ;
    for (int i = 0 ; i <glbParam.nCh; i++)  indxWL_PDL2[i] = -10 ;
    nCh_to_invert = ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"indxWL_PDL2" , (const char*)"int", (int*)&indxWL_PDL2 ) ;
    glbParam.chSel  = indxWL_PDL2[0] ;

    if ( numEventsToAvg_PDL1 != glbParam.numEventsToAvg  )
    {
        oNCL.Read_L0_into_L2 ( (int)ncid, (strcGlobalParameters*)&glbParam, (CDataLevel_2*)oDL2 ) ;

        oDL2->oLOp->Average_in_Time_Lidar_Profiles( (strcGlobalParameters*)&glbParam, (double***)oDL2->data_File_L0, (double***)oDL2->data_File_AVG_L2, 
                                                    (int*)oDL2->Start_Time_L0    , (int*)oDL2->Stop_Time_L0, 
                                                    (int*)oDL2->Start_Time_AVG_L2, (int*)oDL2->Stop_Time_AVG_L2
                                                  ) ;

        if ( glbParam.indxOffset[indxWL_PDL2[0]] >=0 )  // PHOTON-CURRENT SIGNALS --> THE SIGNAL HAVE TO MOVE *BACKWARD* glbParam.indxOffset[c] BINS
        {           
            for ( int e=0 ; e <glbParam.nEventsAVG ; e++ )
            { // BIN OFFSET CORRECTION
                for(int b =0 ; b <(glbParam.nBins -glbParam.indxOffset[indxWL_PDL2[0]]) ; b++)
                    oDL2->data_File_AVG_L2[e][indxWL_PDL2[0]][b] = (double)oDL2->data_File_AVG_L2[e][indxWL_PDL2[0]][ b +glbParam.indxOffset[indxWL_PDL2[0]] ] ; // BIN OFFSET CORRECTION;

                for ( int b=(glbParam.nBins -glbParam.indxOffset[indxWL_PDL2[0]]) ; b <glbParam.nBins ; b++ )
                    oDL2->data_File_AVG_L2[e][indxWL_PDL2[0]][b] = (double)oDL2->data_File_AVG_L2[e][indxWL_PDL2[0]][ glbParam.nBins -glbParam.indxOffset[indxWL_PDL2[0]] ] ; // BIN OFFSET CORRECTION;
            }
        }
        else // glbParam.indxOffset[glbParam.chSel] <0 // PHOTON-COUNTING SIGNALS --> THE SIGNAL HAVE TO MOVE *FORWARD* glbParam.indxOffset[c] BINS
        {
            for ( int e=0 ; e <glbParam.nEventsAVG ; e++ )
            { // BIN OFFSET CORRECTION
                for ( int b= glbParam.indxOffset[indxWL_PDL2[0]] ; b <glbParam.nBins ; b++ )
                    oDL2->data_File_AVG_L2[e][indxWL_PDL2[0]][b] = (double)oDL2->data_File_AVG_L2[e][indxWL_PDL2[0]][ b -glbParam.indxOffset[indxWL_PDL2[0]] ] ; // BIN OFFSET CORRECTION;
                for( int b =0 ; b <glbParam.indxOffset[indxWL_PDL2[0]] ; b++ )
                    oDL2->data_File_AVG_L2[e][indxWL_PDL2[0]][b] = (double)0.0 ;
            }
        }
    }
    else // numEventsToAvg_PDL1 = numEventsToAvg_PDL2
    {   // LIDAR SIGNALS FROM L1 DATASET ARE ALREADY CORRECTED
        oNCL.Read_L1_into_L2( (int)ncid_L1_Data, (strcGlobalParameters*)&glbParam, (CDataLevel_2*)oDL2 ) ;
    }

    oDL2->indxInitSig  = (int)glbParam.indxInitSig ;
    oDL2->indxEndSig   = (int)glbParam.indxEndSig  ;

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

    // SAVE THE LAYER-MASK MATRIX IN oDL2->layer_mask
    // DELETED --> oNCL.Read_LayerMask( (int)ncid_L1_Data, (strcGlobalParameters*)&glbParam, (int**)oDL2->layer_mask ) ;

    // LOAD MOLECULAR PROFILES FROM THE FILE
    int id_var_nmol ;
    if ( ( retval = nc_inq_varid( (int)ncid_L1_Data, (const char*)"Molecular_Density", (int*)&id_var_nmol ) ) )
        ERR(retval);
    oNCL.ReadVar( (int)ncid_L1_Data, (const char*)"Molecular_Density" , (double*)&oDL2->nMol[0] ) ;

    CMolecularData  *oMolData = (CMolecularData*) new CMolecularData  ( (strcGlobalParameters*)&glbParam ) ;

    strcLidarSignal evSig ;
    GetMem_evSig( (strcLidarSignal*)&evSig, (strcGlobalParameters*)&glbParam );

    printf("\n\n") ;
    for ( int e=0 ; e <glbParam.nEventsAVG ; e++ )
    {
        printf("Applying corrections to the lidar signal number %d \r", e) ;
        glbParam.evSel = e ;

// DESATURATION OF THE PHOTON COUNTING CHANNELS
        // double pho_rateMHz = (double)lp.countData[b] /(glbParam->nShots[glbParam->chSel] * glbParam->tBin_us) ;
        // dataFile[glbParam->chSel].db_CountsMHz[e][b] = (double)( pho_rateMHz /( 1.0 - pho_rateMHz / PHO_MAX_COUNT_MHz ) ) ;

        oMolData->Fill_dataMol( (strcGlobalParameters*)&glbParam, (double*)&oDL2->nMol[0] ) ;

        for(int b =0 ; b <glbParam.nBins ; b++)
            evSig.pr[b]  = (double)oDL2->data_File_AVG_L2[e][indxWL_PDL2[0]][b] ;

        if( glbParam.is_Ovlp_Data_Loaded ==true )
        {
            for (int i =0; i <glbParam.nBins ; i++)
                evSig.pr[i] = evSig.pr[i] /ovlp[indxWL_PDL2[0]][i] ;
        }

        if ( glbParam.is_Noise_Data_Loaded == true )
        {
            for(int b =0 ; b <(glbParam.nBins -glbParam.indxOffset[indxWL_PDL2[0]]) ; b++)
                data_Noise[indxWL_PDL2[0]][b] = (double)data_Noise[indxWL_PDL2[0]][ b +glbParam.indxOffset[indxWL_PDL2[0]] ] ; // BIN OFFSET CORRECTION;

            for ( int b=(glbParam.nBins -glbParam.indxOffset[indxWL_PDL2[0]]) ; b <glbParam.nBins ; b++ )
                data_Noise[indxWL_PDL2[0]][b] = (double)data_Noise[indxWL_PDL2[0]][glbParam.nBins -glbParam.indxOffset[indxWL_PDL2[0]]] ; // BIN OFFSET CORRECTION;

            oDL2->oLOp->MakeRangeCorrected( (strcLidarSignal*)&evSig, (strcGlobalParameters*)&glbParam, (double**)data_Noise, (strcMolecularData*)&oMolData->dataMol ) ;
        }
        else // BIAS REMOVAL BASED ON VARIABLE BkgCorrMethod SET IN THE SETTING FILE PASSED AS THIRD ARGUMENT TO lidarAnalysis_PDL2
            oDL2->oLOp->MakeRangeCorrected( (strcLidarSignal*)&evSig, (strcGlobalParameters*)&glbParam, (strcMolecularData*)&oMolData->dataMol ) ;

        for ( int i=0 ; i<glbParam.nBins ; i++ )
        {
            oDL2->pr2[e][indxWL_PDL2[0]][i] = (double)(evSig.pr2[i]) ;
            oDL2->pr[e][i]                  = (double)(evSig.pr_noBias[i]) ;
        }
    }
    printf("\nDone\n") ;

                        if ( (retval = nc_close(ncid)) )
                            ERR(retval) ;

//! RE-ANALYSIS: SWIPE ACROSS THE EVENTS SELECTED BETWEEN minTime and maxTime
    for ( int t=0 ; t <glbParam.nEventsAVG ; t++ )
    {
        cout << endl ;
        glbParam.evSel = t ;

        oMolData->Fill_dataMol( (strcGlobalParameters*)&glbParam, (double*)&oDL2->nMol[0] ) ;

        // if ( numEventsToAvg_PDL1 != glbParam.numEventsToAvg  )
        // {
        // scancloudmask
        // }

        oDL2->dzr = (glbParam.r[2] - glbParam.r[1]) ;
        for ( int c=0 ; c <nCh_to_invert ; c++ ) // nCh_to_invert =1 
        {
            cout << endl << "Inverting: " << "\t Event: " << t << "\t Channel: " << indxWL_PDL2[c] << "\t Wavelenght: " << glbParam.iLambda[indxWL_PDL2[c]] ;
            oDL2->Fernald_1983( (strcGlobalParameters*)&glbParam, (int)t, (int)indxWL_PDL2[c], (strcMolecularData*)&oMolData->dataMol ) ;
        } // for ( int t=0 ; t <glbParam.nEvents ; t++ )
    } // for ( int t=0 ; t <glbParam.nEvents ; t++ )
    printf( "\n\nDone inverting.\n Saving the NetCDF file %s\n", Path_File_Out.c_str() ) ;

    oNCL.Save_LALINET_NCDF_PDL2( (string*)&Path_File_Out, (strcGlobalParameters*)&glbParam, (CDataLevel_2*)oDL2 ) ;

    printf("\n\n---- lidarAnalisys_PDL2 (END) -----------------------------------------------------------------------------\n\n") ;
	return 0 ;
}
 