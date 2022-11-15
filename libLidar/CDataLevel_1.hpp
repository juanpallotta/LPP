#pragma once

#ifndef CDATALEVEL_1_H
#define CDATALEVEL_1_H

#include "libLidar.hpp" // LIDAR DATA TYPE DEFINITIONS
#include "CLidar_Operations.hpp" // LIDAR DATA TYPE DEFINITIONS

class CDataLevel_1  
{
	public:
		CDataLevel_1( strcGlobalParameters* );
		~CDataLevel_1();

		void ScanCloud_RayleightFit( const double*, strcGlobalParameters*, strcMolecularData* ) ;
		void GetCloudLimits( strcGlobalParameters*) ;
		void saveCloudsInfoDB( char*, strcGlobalParameters*, strcCloudInfoDB_LPP* ) ;

		CLidar_Operations 	*oLOp ;		

	    strcCloudProfiles 	*cloudProfiles 			;
		strcIndexMol      	*indxMol  	 			;
		double 				**SE_lay     			;
		double				*prS, errRefBkg			;
		int					avg_Points_Cloud_Mask 	;
		int 				*dco					;

	private:
		void	GetMem_indxMol( strcGlobalParameters*) ;
	    void 	GetMem_cloudProfiles( strcGlobalParameters* ) ;

		double			errFitStage, errFactor, errScanCheckFactor, errCloud, biasRef, sppm, spm, spmpm, m, thresholdFactor, errCloudCheckFactor ;
		double			*prFit, *prprm, *prmprm ;
		int 			nScanMax, CLOUD_MIN_THICK, first_cluster_ON, stepScanCloud, scanNumExit, sum_misc, DELTA_RANGE_LIM_BINS ;
		int				nMaxLoop ;
		char 			ifODcut[5] ;
		strcFitParam	fitParam ;
};
#endif