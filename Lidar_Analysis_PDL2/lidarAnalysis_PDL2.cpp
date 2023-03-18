
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
    printf("\n Settings File --> %s\n", glbParam.FILE_PARAMETERS ) ;

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

    int numEventsToAvg_PDL1 ;
    ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"numEventsToAvg_PDL1", (const char*)"int", (int*)&numEventsToAvg_PDL1     ) ;
    ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"numEventsToAvg_PDL2", (const char*)"int", (int*)&glbParam.numEventsToAvg ) ;

    oNCL.Read_GlbParameters( (int)ncid, (strcGlobalParameters*)&glbParam ) ;

    CDataLevel_1    *oDL1 ;
    CDataLevel_2    *oDL2 = (CDataLevel_2*) new CDataLevel_2( (strcGlobalParameters*)&glbParam ) ;

    int nCh_to_invert ;
    nCh_to_invert = ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"indxWL_PDL2", (const char*)"int", (int*)&glbParam.indxWL_PDL2 ) ;
    assert( glbParam.indxWL_PDL2 <= (glbParam.nCh -1 ) ) ;
    glbParam.chSel  = glbParam.indxWL_PDL2 ;

// LOADING LIDAR DATA  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if ( numEventsToAvg_PDL1 != glbParam.numEventsToAvg  ) // READ DATA FROM L0 DATA LEVEL AND AVERAGE TO L2 DATA LEVEL
    {
        oNCL.Read_L0_into_L2 ( (int)ncid, (strcGlobalParameters*)&glbParam, (CDataLevel_2*)oDL2 ) ;

        oDL2->oLOp->Average_in_Time_Lidar_Profiles( (strcGlobalParameters*)&glbParam, (double***)oDL2->data_File_L0, (double***)oDL2->data_File_L2, 
                                                    (int*)oDL2->Start_Time_L0    , (int*)oDL2->Stop_Time_L0, 
                                                    (int*)oDL2->Start_Time_AVG_L2, (int*)oDL2->Stop_Time_AVG_L2
                                                  ) ;

       oDL1 = (CDataLevel_1*) new CDataLevel_1 ( (strcGlobalParameters*)&glbParam ) ;
    }
    else // numEventsToAvg_PDL1 = numEventsToAvg_PDL2
    {   // LIDAR SIGNALS FROM L1 DATASET ARE ALREADY CORRECTED --> COPY TO L2 OBJECT
        oNCL.Read_L1_into_L2( (int)ncid_L1_Data, (strcGlobalParameters*)&glbParam, (CDataLevel_2*)oDL2 ) ;
    }
// LIDAR DATA LOADED IN oDL2->data_File_L2 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    oDL2->indxInitSig  = (int)glbParam.indxInitSig ;
    oDL2->indxEndSig   = (int)glbParam.indxEndSig  ;

    CMolecularData  *oMolData = (CMolecularData*) new CMolecularData  ( (strcGlobalParameters*)&glbParam ) ;
    oNCL.ReadVar( (int)ncid_L1_Data, (const char*)"Temperature_K", (double*)&oMolData->dataMol.tK[0]  ) ;
    oNCL.ReadVar( (int)ncid_L1_Data, (const char*)"Pressure_Pa"  , (double*)&oMolData->dataMol.pPa[0] ) ;

    oMolData->Get_Mol_Data_L2( (strcGlobalParameters*)&glbParam ) ;

    // for (int i =0; i <glbParam.nBins; i++)
    //     oDL2->nMol[i] = oMolData->dataMol.nMol[i] ;

    printf("\n\n") ;
    if ( numEventsToAvg_PDL1 != glbParam.numEventsToAvg  ) // DATA WAS READ FROM L0
    {
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

        double  **ovlp ;
        int id_var_ovlp ;
        if ( ( nc_inq_varid ( (int)ncid, "Overlap", (int*)&id_var_ovlp ) ) == NC_NOERR )
        {
            // IF THE OVERLAP IS SAVED IN L1 DATA GROUP...
            ovlp = (double**) new double*[glbParam.nCh] ;
            for ( int c=0 ; c <glbParam.nCh ; c++ )
                ovlp[c] = (double*) new double[glbParam.nBins] ;

            oNCL.Read_Overlap( (int)ncid, (strcGlobalParameters*)&glbParam, (int)id_var_ovlp, (double**)ovlp ) ;
        }
        double  **data_Noise ;
        int id_var_noise ;
        if ( ( nc_inq_varid ( (int)ncid, "Bkg_Noise", (int*)&id_var_noise ) ) == NC_NOERR )
        {
            data_Noise = (double**) new double*[glbParam.nCh] ;
            for ( int c=0 ; c <glbParam.nCh ; c++ )
                data_Noise[c] = (double*) new double[glbParam.nBins] ;

            oNCL.Read_Bkg_Noise( (int)ncid, (strcGlobalParameters*)&glbParam, (int)id_var_noise, (double**)data_Noise ) ;
        }

        printf("\n** L2: Due to numEventsToAvg_PDL1 != numEventsToAvg_PDL2 --> Corrections MUST be applied again to data from L0 group **\n") ;
oDL1->oLOp->Lidar_Signals_Corrections( (strcGlobalParameters*)&glbParam, (CMolecularData*)oMolData, (double**)ovlp, (double**)data_Noise, (double***)oDL2->data_File_L2, (double***)pr_corr, (double***)pr2 ) ;

        for ( int e=0 ; e <glbParam.nEventsAVG ; e++ )
        {
            for ( int c=0 ; c <glbParam.nCh ; c++ )
            {
                for ( int i=0 ; i<glbParam.nBins ; i++ )
                    oDL2->pr2[e][c][i] = (double)pr2[e][c][i] ;
            }
            for ( int i=0 ; i<glbParam.nBins ; i++ )
                oDL2->pr[e][i] = (double)pr_corr[e][glbParam.indxWL_PDL2][i]  ;
        }
    } // if ( numEventsToAvg_PDL1 != glbParam.numEventsToAvg  )
    else // DATA WAS READ FROM L1 (ALREADY CORRECTED)
    {
        printf("\n** L2: Due to numEventsToAvg_PDL1 = numEventsToAvg_PDL2 --> Data is taken from L1 group (already corrected) **.\n") ;
        for ( int e=0 ; e <glbParam.nEventsAVG ; e++ )
        {
            for ( int c=0 ; c <glbParam.nCh ; c++ )
            {
                for ( int i=0 ; i<glbParam.nBins ; i++ )
                    oDL2->pr2[e][c][i] = (double)oDL2->data_File_L2[e][c][i] * pow(glbParam.r[i], 2) ;
            }
            for ( int i=0 ; i<glbParam.nBins ; i++ )
                oDL2->pr[e][i] = (double)oDL2->data_File_L2[e][glbParam.indxWL_PDL2][i]  ;
        }
    }

    printf("\nDone\n") ;

                        if ( (retval = nc_close(ncid)) )
                            ERR(retval) ;

//! TO-DO RE-ANALYSIS: SWIPE ACROSS THE SELECTED EVENTS BETWEEN minTime and maxTime
    glbParam.chSel = glbParam.indxWL_PDL2 ;
    for ( int t=0 ; t <glbParam.nEventsAVG ; t++ )
    {
        printf("\n") ;
        glbParam.evSel = t ;

        oMolData->Fill_dataMol_L2( (strcGlobalParameters*)&glbParam ) ;

        // if ( numEventsToAvg_PDL1 != glbParam.numEventsToAvg  )
        // {
            //  printf("   --> Getting cloud profile...");
            // oDL1->ScanCloud_RayleightFit( (const double*)&oDL2->pr[t][0], (strcGlobalParameters*)&glbParam, (strcMolecularData*)&oMolData->dataMol ) ;
        //     for (int b = 0; b <glbParam.nBins ; b++)
        //         oDL2->layer_mask[t][b] = oDL1->cloudProfiles[t].clouds_ON[b] ;
        // }

        oDL2->dzr = oMolData->dataMol.dzr ;
        for ( int c=0 ; c <nCh_to_invert ; c++ ) // nCh_to_invert =1 
        {
            cout << endl << "Inverting: " << "\t Event: " << t << "\t Channel: " << glbParam.indxWL_PDL2 << "\t Wavelenght: " << glbParam.iLambda[glbParam.indxWL_PDL2] ;
            oDL2->FernaldInversion( (strcGlobalParameters*)&glbParam, (int)t, (int)glbParam.indxWL_PDL2, (strcMolecularData*)&oMolData->dataMol ) ;
            // oDL2->Fernald_1983( (strcGlobalParameters*)&glbParam, (int)t, (int)glbParam.indxWL_PDL2, (strcMolecularData*)&oMolData->dataMol ) ;
        } // for ( int t=0 ; t <glbParam.nEvents ; t++ )
    } // for ( int t=0 ; t <glbParam.nEvents ; t++ )
    printf( "\n\nDone inverting.\n Saving the NetCDF file %s\n", Path_File_Out.c_str() ) ;

    oNCL.Save_LALINET_NCDF_PDL2( (string*)&Path_File_Out, (strcGlobalParameters*)&glbParam, (CDataLevel_2*)oDL2 ) ;

    printf("\n\n---- lidarAnalisys_PDL2 (END) -----------------------------------------------------------------------------\n\n") ;
	return 0 ;
}
