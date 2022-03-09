
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
		double 				**SE_lay     ;
		double 				**data_Noise ;
		double				errRefBkg    ;
		bool				is_Noise_Data_Loaded =false ;

	private:
		void	GetMem_indxMol( strcGlobalParameters*) ;
	    void 	GetMem_cloudProfiles( strcGlobalParameters* ) ;
		void 	bkgSubstractionMean_L1( double*, int, int, strcGlobalParameters*, double* ) ;
		void 	bkgSubstractionMolFit_L1(strcMolecularData*, const double*, strcFitParam*, double* ) ;
		void 	bkgSubstraction_BkgFile_L1( const double*, strcFitParam*, strcGlobalParameters*, double* ) ;

		double			errFitStage, errFactor, errScanCheckFactor, errCloud, biasRef, sppm, spm, spmpm, m, thresholdFactor, errCloudCheckFactor ;
		double			*prFit, *prS, *prprm, *prmprm, *noiseFit ;
		int 			nScanMax, AVG_CLOUD_DETECTION, CLOUD_MIN_THICK, first_cluster_ON, stepScanCloud, scanNumExit, sum_misc, DELTA_RANGE_LIM_BINS ;
		int				nMaxLoop ;
		char 			ifODcut[5] ;
		strcFitParam	fitParam ;
};
#endif