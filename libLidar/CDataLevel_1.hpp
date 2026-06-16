
#pragma once

#ifndef CDATALEVEL_1_H
#define CDATALEVEL_1_H

#include "lidarDataTypes.hpp" // LIDAR DATA TYPE DEFINITIONS
#include "CLidar_Operations.hpp"

class CMolecularData ;

class CDataLevel_1  
{
	public:
		CDataLevel_1( strcGlobalParameters* );
		~CDataLevel_1();

		void saveCloudsInfoDB( char*, strcGlobalParameters*, strcCloudInfoDB_LPP* ) ;
		void saveCloudsInfoDB( char*, strcGlobalParameters*, int* ) ;

		CLidar_Operations 	*oLOp 					;
		double 				**pr_for_cloud_mask		;
		std::string		  	strCompCM, strCompPBL  	;

	private:
		char 			ifODcut[5] ;
		strcFitParam	fitParam ;
};
#endif