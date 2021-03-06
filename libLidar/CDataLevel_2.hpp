
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
		void 	Fernald_1983( strcGlobalParameters*, int, int ) ;

		CLidar_Operations 	*oLOp ;
	    double  ***alpha_Aer, **alpha_Mol, *nMol    ;
	    double  ***beta_Aer , **beta_Mol			;
		double  *pr2_Mol							;
		double  ***pr2, *pr2n, phi, p, ip, CalTerm, beta_Tot ;
		// double  *pr2_s 								;
		double  dzr									;
		double  LRM, LR[100]						;
		double  **AOD_LR 							;
		int		nLRs 								;
		int 	indxInitSig, indxEndSig, indxRef	;
		double  R_ref    							;

	private:

} ;

#endif