
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
		void 	Find_Ref_Range  (strcGlobalParameters* , int    					  ) ;

		CLidar_Operations 	*oLOp ;
	    double  ***alpha_Aer					    ;
	    double  ***beta_Aer 						;
		double  ***pr2, **pr ;
		double  ***data_File_L0, ***data_File_L2    ;
		int		*Start_Time_AVG_L2, *Stop_Time_AVG_L2 ;
		int		*Start_Time_L0, *Stop_Time_L0 		;
		int 	**layer_mask 						;
		double  *dummy, *pr2_s						;
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
		// double  *betaTot_Fit ;
		double 	*intAlphaMol_r  ;
		double 	ipNref ;
		double 	intAlphaMol_Ref ;

		int		indxMin_absDiff ;
		double  minDiff 		;

	private:
		void FernaldInversion_Test_Ref_Value( strcGlobalParameters*, int, int, int, strcMolecularData*, double, int, int ) ;
		void FernaldInversion_Core( strcGlobalParameters*, int, int, int, strcMolecularData*, double, int, int ) ;

} ;

#endif