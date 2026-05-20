
#pragma once

#ifndef CDATALEVEL_1_H
#define CDATALEVEL_1_H

#include "libLidar.hpp" // LIDAR DATA TYPE DEFINITIONS
#include "CLidar_Operations.hpp"
// #include "CMolecularData.hpp"
class CMolecularData ;

class CDataLevel_1  
{
	public:
		CDataLevel_1( strcGlobalParameters* );
		~CDataLevel_1();

		// void ScanCloud_RayleighFit( const double*, strcGlobalParameters*, strcMolecularData*    ) ;
		// void Layer_Mask			  ( const double*, strcMolecularData*   , strcGlobalParameters* ) ;
		// void FilterThinClouds( strcGlobalParameters*, double* ) ;
		// void FilterThinClouds( strcGlobalParameters*, int*    ) ;
		void saveCloudsInfoDB( char*, strcGlobalParameters*, strcCloudInfoDB_LPP* ) ;
		void saveCloudsInfoDB( char*, strcGlobalParameters*, int* ) ;

		CLidar_Operations 	*oLOp 					;
		double 				**pr_for_cloud_mask		;
		double 				**SE_lay     			;
		double				*prS, errRefBkg			;
		int					avg_Points_Cloud_Mask 	;
		int 				*dco					; // DIFFERENTIAL CLOUD ON 
		string		  		strCompCM, strCompPBL  	;

	private:
		// double 			errFactor, thresholdFactor, errScanCheckFactor, errCloudCheckFactor, DELTA_RANGE_LIM_BINS ;
		double			errFitStage, errCloud, biasRef, sppm, spm, spmpm, m ;
		double			*prFit, *pr2, *prprm, *prmprm ;
		int 			CLOUD_MIN_THICK, stepScanCloud, first_cluster_ON, scanNumExit, sum_misc ;
		char 			ifODcut[5] ;
		strcFitParam	fitParam ;
};
#endif