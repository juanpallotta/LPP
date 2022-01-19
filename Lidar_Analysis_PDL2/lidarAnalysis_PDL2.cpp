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

    if ( (retval = nc_open( Path_File_In.c_str(), NC_NOWRITE, (int*)&ncid)) )
        ERR(retval);

    // READ VARIABLES FROM DE NETCDF FILE INSIDE THE GROUP "L1_Data"
    int ncid_L1_Data ;
    if ( ( retval = nc_inq_grp_ncid( (int)ncid, (const char*)"L1_Data", (int*)&ncid_L1_Data ) ) )
        ERR(retval);

    string  strNameVars[NVARS_LALINET] ;
    strNameVars[0] = "Raw_Lidar_Data_L1" ; //! BORRAR
    strNameVars[1] = "Range_Corrected_Lidar_Signal_L1" ;
    strNameVars[2] = "Molecular_Extinction" ;
    strNameVars[3] = "Molecular_Backscattering" ;
    int id_var_RCLS, id_var_beta_mol, id_var_alpha_mol  ;
    if ( ( retval = nc_inq_varid( (int)ncid_L1_Data, (const char*)strNameVars[1].c_str(), (int*)&id_var_RCLS ) ) )
        ERR(retval);
    if ( ( retval = nc_inq_varid( (int)ncid_L1_Data, (const char*)strNameVars[2].c_str(), (int*)&id_var_alpha_mol ) ) )
        ERR(retval);
    if ( ( retval = nc_inq_varid( (int)ncid_L1_Data, (const char*)strNameVars[3].c_str(), (int*)&id_var_beta_mol ) ) )
        ERR(retval);

    int num_dim_var ;
    if ( ( retval = nc_inq_varndims( (int)ncid_L1_Data, (int)id_var_RCLS, (int*)&num_dim_var ) ) )
        ERR(retval);
    assert( num_dim_var ==3 ); // SI SE CUMPLE, SIGUE.

    int id_dim_RCLS[num_dim_var] ;
    if ( ( retval = nc_inq_vardimid( (int)ncid_L1_Data, (int)id_var_RCLS, (int*)id_dim_RCLS ) ) )
        ERR(retval) ;

    int size_dim[num_dim_var] ;
    for ( int d=0 ; d <num_dim_var ; d++ )
        size_dim[d] =(int)1 ;

    for( int d=0 ; d <num_dim_var ; d++ )
    {
        if ( ( retval = nc_inq_dimlen( (int)ncid_L1_Data, (int)id_dim_RCLS[d], (size_t*)&size_dim[d] ) ) )
            ERR(retval);
    }
    glbParam.nEvents = size_dim[0] ; glbParam.nEventsAVG = glbParam.nEvents ;
    glbParam.nLambda = size_dim[1] ; // IT SHOULD BE CALCULATE BASED ON DIFFERENTS WAVELENGHS.
    glbParam.nCh     = size_dim[1] ;
    glbParam.nBins   = size_dim[2] ;

    oNCL.ReadVar( (int)ncid, (const char*)"Wavelengths", (double*)glbParam.iLambda ) ;

    CDataLevel_2 oDL2 = CDataLevel_2( (strcGlobalParameters*)&glbParam ) ;

    // LOAD pr2 FROM THE FILE
    size_t start[3], count[3];
    start[0] = 0;   count[0] = 1 ; // TIME DIM. glbParam.nEventsAVG; 
    start[1] = 0;   count[1] = 1 ; // CHANNEL DIM. glbParam.nCh; 
    start[2] = 0;   count[2] = glbParam.nBins; // POINTS DIM. 
    for( int e=0 ; e <glbParam.nEvents ; e++ )
    {
        start[0] =e ;
        for ( int c=0 ; c <glbParam.nCh ; c++ )
        {
            start[1] =c ;
            if ( (retval = nc_get_vara_double( (int)ncid_L1_Data, (int)id_var_RCLS, start, count, (double*)&oDL2.pr2[e][c][0] ) ) )
                ERR(retval);
        }
    }
    // LOAD MOLECULAR PROFILES FROM THE FILE
    start[0] = 0;   count[0] = 1 ; // TIME DIM. glbParam.nEventsAVG; 
    start[1] = 0;   count[1] = 1 ; // CHANNEL DIM. glbParam.nCh; 
    start[2] = 0;   count[2] = glbParam.nBins; // POINTS DIM. 
    for ( int c=0 ; c <glbParam.nCh ; c++ )
    {
        start[1] =c ;
        if ( (retval = nc_get_vara_double( (int)ncid_L1_Data, (int)id_var_alpha_mol, &start[1], &count[1], (double*)&oDL2.alpha_Mol[c][0] ) ) )
            ERR(retval);

        if ( (retval = nc_get_vara_double( (int)ncid_L1_Data, (int)id_var_beta_mol, &start[1], &count[1], (double*)&oDL2.beta_Mol[c][0] ) ) )
            ERR(retval);
    }

    if ( ( retval = nc_get_att_double(	(int)ncid, (int)NC_GLOBAL, (const char*)"Range_Resolution", (double*)&glbParam.dr) ) )
        ERR(retval);

    if ( ( retval = nc_get_att_double(	(int)ncid, (int)NC_GLOBAL, (const char*)"Altitude_meter_asl", (double*)&glbParam.siteASL) ) )
        ERR(retval);

            if ( (retval = nc_close(ncid)) )
                ERR(retval) ;
/*
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// READ GLOBAL PARAMETERS FROM NETCDF FILE
    glbParam.r = (double*) new double[glbParam.nBins] ;
    for( int i=0 ; i <glbParam.nBins ; i++ )
        glbParam.r[i] = i*glbParam.dr ;

    ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"rInitSig", (const char*)"double", (double*)&glbParam.rInitSig ) ;
    ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"rEndSig" , (const char*)"double", (double*)&glbParam.rEndSig  ) ;
    glbParam.indxInitSig = (int)round( glbParam.rInitSig /glbParam.dr ) ;
    glbParam.indxEndSig  = (int)round( glbParam.rEndSig  /glbParam.dr ) ;
    oDL2.indxInitSig     = (int)glbParam.indxInitSig ;
    oDL2.indxEndSig      = (int)glbParam.indxEndSig  ;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int Ch_to_Invert[MAX_CH_LICEL] ;
    ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"Ch_to_Invert" , (const char*)"int", (int*)&Ch_to_Invert ) ;
    // for (int i = 0 ; i <MAX_CH_LICEL; i++)
    // {
    //     cout << endl << "Ch_to_Invert[i]: " << Ch_to_Invert[i] ;
    // }
    
    for ( int t=0 ; t <glbParam.nEvents ; t++ )
    {
        cout << endl ;
        glbParam.event_analyzed = t ;
        oDL2.dzr = (glbParam.r[2] - glbParam.r[1]) * 1 ;
        for ( int c=0 ; c <glbParam.nCh ; c++ )
        {
            cout << endl << "Inverting: " << "\t Event: " << t << "\t Channel: " << c << "\t Wavelenght: " << glbParam.iLambda[c] ;
            // oDL2.Fernald_1983( (strcGlobalParameters*)&glbParam, (int)t , (int)c ) ;
        } // for ( int t=0 ; t <glbParam.nEvents ; t++ )
    } // for ( int t=0 ; t <glbParam.nEvents ; t++ )
    
    oNCL.Save_LALINET_NCDF_PDL2( (string*)&Path_File_Out, (strcGlobalParameters*)&glbParam, (CDataLevel_2*)&oDL2 ) ;
*/
    printf("\n\n---- lidarAnalisys_PDL2 (END) -----------------------------------------------------------------------------\n\n") ;
	return 0 ;
}
 