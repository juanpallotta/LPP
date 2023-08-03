
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <cstdlib>
#include <iostream>
#include <errno.h>
#include <limits.h>
#include <fstream>
// #include <netcdf.h>

// GENERIC LIDAR LIBRARIES
#include "../libLidar/libLidar.hpp"
#include "../libLidar/CNetCDF_Lidar.hpp"
#include "../libLidar/CDataLevel_1.hpp"
#include "../libLidar/CMolecularData_USStd.hpp"

int main( int argc, char *argv[] )
{
    strcGlobalParameters	glbParam  ;
    string	Path_File_In, Path_File_Out ;

	Path_File_In.assign  ( argv[1] ) ;
	Path_File_Out.assign ( argv[2] ) ;
	sprintf( (char*)glbParam.FILE_PARAMETERS , "%s"	, argv[3] ) ;

    printf("\n\n---- CLOUD DB (START) -----------------------------------------------------------------------------\n\n") ;
    printf("\n\n Path_File_In: %s \n Path_File_Out: %s\n\n", Path_File_In.c_str(), Path_File_Out.c_str() ) ;

    // printf( "\n filename: %s \n", Path_File_In.substr( Path_File_In.find_last_of("/\\") + 1 ).c_str() ) ;

    strcpy( glbParam.fileName, Path_File_In.substr( Path_File_In.find_last_of("/\\") + 1 ).c_str() ) ;

    int  ncid   ;
    int  retval ;
    CNetCDF_Lidar   oNCL = CNetCDF_Lidar() ;

    if ( (retval = nc_open( Path_File_In.c_str(), NC_NOWRITE, &ncid)) )
        ERR(retval);

    int numgrps, nc_grp_id ;
    if ( ( retval = nc_inq_grps( ncid, &numgrps, &nc_grp_id ) ) )
           ERR(retval) ;

    //* START - READ "Cloud_Mask" VARIABLE FROM DE NETCDF FILE //////////////////////////////////////////////////////////////////////////////////
    int id_var_cloud_mask ;
    if ( ( retval = nc_inq_varid( (int)nc_grp_id, (const char*)"Cloud_Mask", (int*)&id_var_cloud_mask ) ) )
        ERR(retval);
    int num_dim_var ;
    if ( ( retval = nc_inq_varndims( (int)nc_grp_id, (int)id_var_cloud_mask, (int*)&num_dim_var ) ) )
        ERR(retval);
    // cout << endl << "num_dim_var: " << num_dim_var << "     id_var_cloud_mask: " << id_var_cloud_mask << endl ;
        assert( num_dim_var ==2 ) ; // SI SE CUMPLE, SIGUE.
    int id_dim_cloud_mask[num_dim_var] ;
    if ( ( retval = nc_inq_vardimid( (int)nc_grp_id, (int)id_var_cloud_mask, (int*)id_dim_cloud_mask ) ) )
        ERR(retval);

    int size_dim[num_dim_var] ;
        for ( int d=0 ; d <num_dim_var ; d++ ) size_dim[d] =(int)1 ;
    for( int d=0 ; d <num_dim_var ; d++ )
    {
        if ( ( retval = nc_inq_dimlen( (int)nc_grp_id, (int)id_dim_cloud_mask[d], (size_t*)&size_dim[d] ) ) )
            ERR(retval);
        // printf( "\n Cloud_Mask -> size_dim[%d]: %d\n", d, size_dim[d] ) ;
    }

    // glbParam.nEventsAVG MUST BE USED ACROSS ALL THE CODE. glbParam.nEvents = glbParam.nEventsAVG JUST IN CASE
    glbParam.nEvents = size_dim[0] ; glbParam.nEventsAVG = glbParam.nEvents ;
    glbParam.nBins   = size_dim[1] ;

    if ( ( retval = nc_get_att_double(	(int)ncid, (int)NC_GLOBAL, (const char*)"Range_Resolution", (double*)&glbParam.dr) ) )
        ERR(retval);
    glbParam.r = (double*) new double[glbParam.nBins] ;
    for( int i=1 ; i <=glbParam.nBins ; i++ )
        glbParam.r[i-1] = i*glbParam.dr ;

    int  **cloud_mask = (int**) new int*[glbParam.nEventsAVG];
    for ( int e=0 ; e <glbParam.nEventsAVG ; e++ )
        cloud_mask[e] = (int*) new int[glbParam.nBins] ;

    size_t startDF[2], countDF[2];
    startDF[0] = 0;   countDF[0] = 1 ; // glbParam.nEvents ; 
    startDF[1] = 0;   countDF[1] = glbParam.nBins ;
    for( int e=0 ; e <glbParam.nEventsAVG ; e++ )
    {
        startDF[0] =e ;
            if ( ( retval = nc_get_vara_int( (int)nc_grp_id, (int)id_var_cloud_mask, startDF, countDF, (int*)&cloud_mask[e][0] ) ) )
                ERR(retval);    
    }
    //* END - READ "Cloud_Mask" VARIABLE FROM DE NETCDF FILE //////////////////////////////////////////////////////////////////////////////////

    glbParam.aZenithAVG  = (double*) new double [glbParam.nEventsAVG] ;     memset( (double*)glbParam.aZenithAVG , 0, (sizeof(double)*glbParam.nEventsAVG) ) ;
    oNCL.ReadVar( (int)nc_grp_id, (const char*)"Zenith_AVG_L1", (double*)glbParam.aZenithAVG  ) ;

    int *Raw_Data_Start_Time_AVG = (int*) new int [glbParam.nEventsAVG ] ;
    int *Raw_Data_Stop_Time_AVG  = (int*) new int [glbParam.nEventsAVG ] ;
    oNCL.ReadVar( (int)nc_grp_id, (const char*)"Start_Time_AVG_L1", (int*)Raw_Data_Start_Time_AVG ) ;
    oNCL.ReadVar( (int)nc_grp_id, (const char*)"Stop_Time_AVG_L1" , (int*)Raw_Data_Stop_Time_AVG  ) ;

    glbParam.event_gps_sec = (long*) new long[glbParam.nEventsAVG] ;
    for ( int i =0 ; i < glbParam.nEventsAVG ; i++ )
        glbParam.event_gps_sec[i] = Raw_Data_Start_Time_AVG[i] ;

    if ( ( retval = nc_get_att_double( (int)ncid, NC_GLOBAL, "Range_Resolution", (double*)&glbParam.dr ) ) )
        ERR(retval);

    if ( ( retval = nc_get_att_double( (int)ncid, (int)NC_GLOBAL, (const char*)"Altitude_meter_asl", (double*)&glbParam.siteASL ) ) )
        ERR(retval);

    if ( ( retval = nc_get_att_text( (int)ncid, NC_GLOBAL, "Site_Name", (char*)glbParam.site ) ) )
        ERR(retval);
    
    //* START READ "Raw_Lidar_Data_L1" VARIABLE FROM DE NETCDF FILE //////////////////////////////////////////////////////////////////////////////////
    int id_var_Raw_Lidar_Data_L1 ;
    if ( ( retval = nc_inq_varid( (int)nc_grp_id, (const char*)"Raw_Lidar_Data_L1", (int*)&id_var_Raw_Lidar_Data_L1 ) ) )
        ERR(retval);
    if ( ( retval = nc_inq_varndims( (int)nc_grp_id, (int)id_var_Raw_Lidar_Data_L1, (int*)&num_dim_var ) ) )
        ERR(retval);
    // cout << endl << "Raw_Lidar_Data_L1 ---> num_dim_var: " << num_dim_var << "     id_var_Raw_Lidar_Data_L1: " << id_var_Raw_Lidar_Data_L1 << endl ;
        assert( num_dim_var ==3 ) ; // SI SE CUMPLE, SIGUE.
    int id_dim_Raw_Lidar_Data_L1[num_dim_var] ;
    if ( ( retval = nc_inq_vardimid( (int)nc_grp_id, (int)id_var_Raw_Lidar_Data_L1, (int*)id_dim_Raw_Lidar_Data_L1 ) ) )
        ERR(retval);

    int size_dim_Raw_Lidar_Data_L1[num_dim_var] ;
        for ( int d=0 ; d <num_dim_var ; d++ ) size_dim_Raw_Lidar_Data_L1[d] =(int)1 ;
    for( int d=0 ; d <num_dim_var ; d++ )
    {
        if ( ( retval = nc_inq_dimlen( (int)nc_grp_id, (int)id_dim_Raw_Lidar_Data_L1[d], (size_t*)&size_dim_Raw_Lidar_Data_L1[d] ) ) )
            ERR(retval);
        // printf( "\nsize_dim_Raw_Lidar_Data_L1[%d]: %d\n", d, size_dim_Raw_Lidar_Data_L1[d] ) ;
    }
    glbParam.nCh     = size_dim_Raw_Lidar_Data_L1[1] ;

    ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"numEventsToAvg_PDL1", (const char*)"int", (int*)&glbParam.numEventsToAvg ) ;
        CDataLevel_1 oDL1 = CDataLevel_1( (strcGlobalParameters*)&glbParam ) ;

    double  *pr_VOD = (double*) new double [ glbParam.nBins ] ;     memset( (double*)pr_VOD, 0, (sizeof(double)*glbParam.nBins) ) ;
    int     indxWL_PDL1 ;
    ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"indxWL_PDL1", (const char*)"int", (int*)&indxWL_PDL1 ) ;

    glbParam.iLambda = (int*) new int [glbParam.nCh] ;
    oNCL.ReadVar( (int)ncid, (const char*)"Wavelengths", (int*)glbParam.iLambda ) ;

    CMolecularData       *oMolData = (CMolecularData*) new CMolecularData ( (strcGlobalParameters*)&glbParam ) ;
    oMolData->Get_Mol_Data_L1( (strcGlobalParameters*)&glbParam ) ;

    strcCloudInfoDB_LPP cloudInfoDB ;
	cloudInfoDB.lowestCloudHeight_ASL = (double*) new double [ glbParam.nEventsAVG ] ;
	cloudInfoDB.lowestCloudThickness  = (double*) new double [ glbParam.nEventsAVG ] ;
	cloudInfoDB.lowestCloud_VOD       = (double*) new double [ glbParam.nEventsAVG ] ;
	cloudInfoDB.cloudTime             = (int*   ) new int    [ glbParam.nEventsAVG ] ;
	cloudInfoDB.nClouds               = (int*   ) new int    [ glbParam.nEventsAVG ] ;

    glbParam.rEndSig_ev    = (double*) new double [ glbParam.nEventsAVG ] ;
    glbParam.indxEndSig_ev = (int*)    new int    [ glbParam.nEventsAVG ] ;
    oNCL.ReadVar( (int)nc_grp_id, (const char*)"MaxRangeAnalysis", (double*)&glbParam.rEndSig_ev[0] ) ;
    for (int i =0; i <glbParam.nEventsAVG; i++)
        glbParam.indxEndSig_ev[i] = (int)round(glbParam.rEndSig_ev[i] /glbParam.dr) ;

    size_t start_pr[3], count_pr[3] ; 
    start_pr[0] = 0             ;   count_pr[0] = 1 ; // glbParam.nEvents ; 
    start_pr[1] = indxWL_PDL1   ;   count_pr[1] = 1 ;
    start_pr[2] = 0             ;   count_pr[2] = glbParam.nBins ;
    glbParam.chSel = indxWL_PDL1;

    for ( int t =0; t <glbParam.nEventsAVG ; t++ )
    {
        glbParam.evSel = t ;
        oMolData->Fill_dataMol_L1( (strcGlobalParameters*)&glbParam ) ;

        for ( int b =0; b <glbParam.nBins ; b++)
            oDL1.cloudProfiles[t].clouds_ON[b] = cloud_mask[t][b] ;

        oDL1.GetCloudLimits( (strcGlobalParameters*)&glbParam ) ;

        if ( oDL1.cloudProfiles[t].nClouds >=1 )
        {   // FIRST CLOUD OD CALCULATION. JUST IF ITS HIGHER THAN 2000 MTS ASL
            start_pr[0] =t ;
            if ( ( retval = nc_get_vara_double( (int)nc_grp_id, (int)id_var_Raw_Lidar_Data_L1, start_pr, count_pr, (double*)&pr_VOD[0] ) ) )
                ERR(retval) ;

            // smooth( (double*)pr_VOD, (int)0, (int)(glbParam.nBins-1), (int)oDL1.avg_Points_Cloud_Mask, (double*)pr_VOD ) ;
                // TransmissionMethod_pr( (double*)pr_VOD, (strcGlobalParameters*)&glbParam, (strcMolecularData*)&oMolData->dataMol, 
                //                     (int)oDL1.cloudProfiles[t].indxInitClouds[0], (int)oDL1.cloudProfiles[t].indxEndClouds[0], (double*)&oDL1.cloudProfiles[t].VOD_cloud[0] ) ;

    printf("\n(%d) CLOUDS DETECTED \t Base heigh: %lf \t Top heigh: %lf \n", t, oDL1.cloudProfiles[t].indxInitClouds[0] *glbParam.dzr + glbParam.siteASL
                                                                           ,    oDL1.cloudProfiles[t].indxEndClouds[0]  *glbParam.dzr + glbParam.siteASL ) ;
        }
        else
            printf( "\n(%d) NO CLOUDS DETECTED", t ) ;

        cloudInfoDB.lowestCloudHeight_ASL[glbParam.evSel] =  oDL1.cloudProfiles[t].indxInitClouds[0] *glbParam.dr ;
        cloudInfoDB.lowestCloudThickness [glbParam.evSel] = (oDL1.cloudProfiles[t].indxEndClouds[0] - oDL1.cloudProfiles[t].indxInitClouds[0]) *glbParam.dr ;
        cloudInfoDB.lowestCloud_VOD      [glbParam.evSel] =  oDL1.cloudProfiles[t].VOD_cloud[0] ;
        cloudInfoDB.nClouds              [glbParam.evSel] =  oDL1.cloudProfiles[t].nClouds      ;
        cloudInfoDB.cloudTime            [glbParam.evSel] =  Raw_Data_Start_Time_AVG[t]         ;

        // printf("\n\n\t main() " ) ;
        // printf("\n\t cloudInfoDB.lowestCloudHeight_ASL[%d]: %lf " , glbParam.evSel, cloudInfoDB.lowestCloudHeight_ASL[t] ) ;
        // printf("\n\t cloudInfoDB.lowestCloudThickness[%d] : %lf " , glbParam.evSel, cloudInfoDB.lowestCloudThickness[t]  ) ;
        // printf("\n\t cloudInfoDB.lowestCloud_VOD[%d]      : %lf " , glbParam.evSel, cloudInfoDB.lowestCloud_VOD[t]       ) ;
        // printf("\n\t cloudInfoDB.nClouds[%d]              : %d "  , glbParam.evSel, cloudInfoDB.nClouds[t]	            ) ;
        // printf("\n\t cloudInfoDB.cloudTime[%d]            : %d "  , glbParam.evSel, cloudInfoDB.cloudTime[t]	            ) ;
    } // for ( int t =0; t <glbParam.nEventsAVG ; t++ )

    // oDL1.saveCloudsInfoDB( (char*)Path_File_Out.c_str(), (strcGlobalParameters*)&glbParam, (strcCloudInfoDB_LPP*)&cloudInfoDB ) ;
    oDL1.saveCloudsInfoDB( (char*)Path_File_Out.c_str(), (strcGlobalParameters*)&glbParam, (int*)Raw_Data_Start_Time_AVG ) ;
  
    printf("\n\n---- CLOUD DB (END) -----------------------------------------------------------------------------\n\n\n\n") ;

    return 0;
}
