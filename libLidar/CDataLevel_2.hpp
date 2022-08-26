
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
		// void 	Fernald_1983( strcGlobalParameters*, int, int ) ;
		void 	Fernald_1983( strcGlobalParameters*, int, int, strcMolecularData* ) ;
		int 	Find_Ref_Range(strcGlobalParameters*, strcMolecularData*) ;

		CLidar_Operations 	*oLOp ;
	    double  ***alpha_Aer					    ;
	    double  ***beta_Aer 						;
		double  *nMol 								;
		double  ***pr2, **pr, *pr2n, phi, p, ip, CalTerm, beta_Tot ;
		double  *dummy 								;
		double  dzr									;
		double  LRM, LR[100]						;
		double  **AOD_LR 							;
		int		nLRs 								;
		int 	indxInitSig, indxEndSig, indxRef	;
		double  R_ref    							;
		double  f[7]={1.0, 1.01, 1.03, 1.05, 1.07, 1.1, 1.13} ;

	private:

} ;

#endif