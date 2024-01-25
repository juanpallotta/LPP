
#pragma once

#ifndef CDATALEVEL_2_H
#define CDATALEVEL_2_H

#include "libLidar.hpp" // LIDAR DATA TYPE DEFINITIONS
#include "CLidar_Operations.hpp" // LIDAR DATA TYPE DEFINITIONS

class CDataLevel_2  
{
	public:
		CDataLevel_2( strcGlobalParameters* ) ;
		~CDataLevel_2()  					  ;
		// void 	Fernald_1983    ( strcGlobalParameters*, int, int, strcMolecularData* ) ;
		void 	FernaldInversion( strcGlobalParameters*, int, int, strcMolecularData* ) ;
		void 	FernaldInversion( double*, strcMolecularData*, strcGlobalParameters*  ) ;
		void 	Find_Ref_Range  (strcGlobalParameters* , int    					  ) ;

		void  	GetMemStrcErrorSigSet( strcErrorSignalSet*, int, int ) 						 ;
		void 	MonteCarloRandomError( strcGlobalParameters*, strcMolecularData* ) ;
		// void 	MonteCarloSystematicError	( double*,          strcGlobalParameters*, strcMolecularData*, strcIndexMol*, strcFernaldInversion*, strcErrorSignalSet* ) ;
		void 	GetErrSetParam				( char*, int, int, double, strcErrorSignalSet* ) ;

		CLidar_Operations 	*oLOp ;
	    double  ***alpha_Aer					    ;
	    double  ***beta_Aer 						;
		double  ***pr2, **pr ;
		double  ***data_File_L0, ***data_File_L2    ;
		int		*Start_Time_AVG_L2, *Stop_Time_AVG_L2 ;
		int		*Start_Time_L0, *Stop_Time_L0 		;
		int 	**layer_mask 						;
		double  *dummy, *pr2_s			 			;
		// double  *pr2Fit						; // *prFit, 
		double  dzr									;
		double  LRM, LR[100]						;
		double  **AOD_LR 							;
		int		nLRs 								;
		int 	indxInitSig, indxEndSig, *indxRef_Fernald, *indxRef_Fernald_Start, *indxRef_Fernald_Stop ;
		double  R_ref    							;

		double 	pr2_Ref ;
		int 	avg_Half_Points_Fernald_Ref ;
		double	heightRef_Inversion_ASL, heightRef_Inversion_Start_ASL, heightRef_Inversion_Stop_ASL ;
		double  ka  ;
		double  KM_ ;

		double 	*pr2n  ;
		double 	*phi   ;
		double 	*p 	   ;
		double 	*ip	   ;
		double 	*ipN   ;
		double 	*betaT ;
		double  *betaMol_Fit ;
		double 	*intAlphaMol_r  ;
		double 	ipNref ;
		double 	intAlphaMol_Ref ;

		int		indxMin_absDiff ;
		double  minDiff 		;
		string 	reference_method ;

	private:
		// void FernaldInversion_Test_Ref_Value( strcGlobalParameters*, int, int, int, strcMolecularData*, double, int, int ) ;
		void FernaldInversion_Core( strcGlobalParameters*, int, int, int, strcMolecularData*, double, int, int ) ;

		const double DBL_EPS_COMP = 1 - DBL_EPSILON; // DBL_EPSILON is defined in <limits.h>.
		inline double RandU()
		{
			return DBL_EPSILON + ((double) rand()/RAND_MAX) ;
		}

		inline double RandN2(double mu, double sigma)
		{
			return mu + (rand()%2 ? -1.0 : 1.0)*sigma*pow(-log(DBL_EPS_COMP*RandU()), 0.5) ;
		}

} ;

#endif