#pragma once

#ifndef CDATALEVEL_1_H
#define CDATALEVEL_1_H

#include "libLidar.hpp" // LIDAR DATA TYPE DEFINITIONS
#include "CLidar_Operations.hpp"
#include "CMolecularData.hpp"

class CDataLevel_1  
{
	public:
		CDataLevel_1( strcGlobalParameters* );
		~CDataLevel_1();

		void ScanCloud_RayleighFit( const double*, strcGlobalParameters*, strcMolecularData* ) ;
		// void ScanCloud_RayleighFit_wBias( const double*, strcGlobalParameters*, strcMolecularData* ) ;
		void GetCloudLimits( strcGlobalParameters* ) ;
		void saveCloudsInfoDB( char*, strcGlobalParameters*, strcCloudInfoDB_LPP* ) ;
		void saveCloudsInfoDB( char*, strcGlobalParameters*, int* ) ;

		CLidar_Operations 	*oLOp ;

	    strcCloudProfiles 	*cloudProfiles 			;
		strcIndexMol      	*indxMol  	 			;
		double 				**pr_for_cloud_mask		;
		double 				**SE_lay     			;
		double				*prS, errRefBkg			;
		int					avg_Points_Cloud_Mask 	;
		int 				*dco					; // DIFFERENTIAL CLOUD ON 
		string		  		strCompCM, strCompPBL  	;

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