
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
		void 	Fernald_1983    ( strcGlobalParameters*, int, int, strcMolecularData* ) ;
		void 	FernaldInversion( strcGlobalParameters*, int, int, strcMolecularData* ) ;
		// int 	Find_Ref_Range(strcGlobalParameters*, strcMolecularData*) ;

		CLidar_Operations 	*oLOp ;
	    double  ***alpha_Aer					    ;
	    double  ***beta_Aer 						;
		// double  *nMol 								;
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
		int 	indxInitSig, indxEndSig, *indxRef_Fernald ;
		double  R_ref    							;

		double 	*pr2n  ;
		double 	*phi   ;
		double 	*p 	   ;
		double 	*ip	   ;
		double 	*ipN   ;
		double 	*betaT ;
		double 	*intAlphaMol_r  ;
		double 	ipNref ;
		double 	intAlphaMol_Ref ;

	private:

} ;

#endif