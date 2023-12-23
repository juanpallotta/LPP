
// #pragma once

#ifndef CNETCDF_LIDAR_H
#define CNETCDF_LIDAR_H

#include <netcdf>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <cstdlib>
#include <assert.h> 

#include "../libLidar/libLidar.hpp"
#include "../libLidar/CDataLevel_1.hpp"
#include "../libLidar/CDataLevel_2.hpp"
// #include "../libLidar/CMolecularData_USStd.hpp"
#include "../libLidar/CMolecularData.hpp"

using namespace std;
using namespace netCDF;
using namespace netCDF::exceptions;

#define NDIMS_SCC 5
#define NVARS_SCC 15

#define NDIMS_LALINET_L0 3
#define NVARS_LALINET_L0 20
#define NDIMS_LALINET_L1 3
#define NVARS_LALINET_L1 20
#define NVARS_LALINET_L2 20

#define ERRCODE 20
#define ERR(e) {printf("NetCDF Error: %s\n", nc_strerror(e)); exit(ERRCODE);}

class CNetCDF_Lidar  
{
	private:
		int retval ;

	public:
		CNetCDF_Lidar()	;
		~CNetCDF_Lidar();

		void DefineDims( int, char*, int, int* ) ;
		void DefineVariable( int, char*, const char*, int, int*, int* ) ;
        void DefineVarDims( int, int, string*, int*, int*, char*, const char*, int* ) ;
		void PutVar( int, int, const char*, void* ) ;
		void PutVar_String( int, int, char** ) ;
		void Putt_Bulk_Att_Text( int, int, int, string*, string* ) ;
		void Putt_Bulk_Att_Double( int, int, int, string*, double* ) ;
		void Putt_Bulk_Att_Int( int, int, int, string*, int* ) ;
		void Set_LALINET_Units_L0( int, int* ) ;
		void Set_LALINET_Units_L1( int, int* ) ;
		void Set_LALINET_Units_L2( int, int* ) ;
		void ReadVar( int, const char*, void* ) ;
		void Read_Bkg_Noise( int, strcGlobalParameters*, int, double** ) ;
		void Read_Overlap( int, strcGlobalParameters*, int, double** ) ;

		void Read_GlbParameters( int, strcGlobalParameters* ) ;
		void Read_L0_into_L2   ( int, strcGlobalParameters*, CDataLevel_2* ) ;
		void Read_L1_into_L2   ( int, strcGlobalParameters*, CDataLevel_2* ) ;

		void Save_SCC_NCDF_Format	 		( string, strcGlobalParameters*, double***, long*, string*, long*, string* ) ;
		void Save_LALINET_NCDF_PDL0			( string, strcGlobalParameters*, double***, long*, long*, char** ) ;
		// void Save_LALINET_NCDF_PDL0			( string, strcGlobalParameters*, double***, long*, long*		 ) ;
		void Add_Noise_LALINET_NCDF_PDL0	( string*, strcGlobalParameters*, double** ) ;
		void Add_Overlap_LALINET_NCDF_PDL0	( string *Path_File_Out, strcGlobalParameters *glbParam, double **ovlp ) ;
		void Save_LALINET_NCDF_PDL1  		( string*, strcGlobalParameters*, double**, double*, int**, double***, 	  	   int*, int*, CMolecularData* ) ;
		void Save_LALINET_NCDF_PDL2		 	( string*, strcGlobalParameters*, CDataLevel_2* ) ;
		void CloseFile( int ) ;
};
#endif