
#ifndef CDATALEVEL_1_H
#define CDATALEVEL_1_H

#include "libLidar.hpp" // LIDAR DATA TYPE DEFINITIONS

class CDataLevel_1  
{
	public:
		CDataLevel_1( strcGlobalParameters* );
		~CDataLevel_1();

		void MakeRangeCorrected( strcLidarSignal*, strcGlobalParameters*, strcMolecularData*) ;
		void ScanCloud_RayleightFit( const double*, strcGlobalParameters*, strcMolecularData* ) ;
		// void GetCloudLimits( strcGlobalParameters*, strcMolecularData*) ;
		void GetCloudLimits_v1( strcGlobalParameters*) ;

	    strcCloudProfiles 	*cloudProfiles ;
		strcIndexMol      	*indxMol  	 ;
		// double 				**mtxCloud_ON ;
		double 				**SE_lay ;
		double				errRefBkg ;

	private:
		void	GetMem_indxMol( strcGlobalParameters*) ;
	    void 	GetMem_cloudProfiles( strcGlobalParameters* ) ;

		double			errFitStage, errFactor, errScanCheckFactor, errCloud, biasRef, sppm, spm, spmpm, m, thresholdFactor, errCloudCheckFactor ;
		double			*prFit, *prS, *prprm, *prmprm ;
		int 			nScanMax, AVG_CLOUD_DETECTION, CLOUD_MIN_THICK, first_cluster_ON, stepScanCloud, scanNumExit, sum_misc, DELTA_RANGE_LIM_BINS ;
		int				nMaxLoop ;
		char 			ifODcut[5] ;
		strcFitParam	fitParam ;
};
#endif