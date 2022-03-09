
#ifndef CDATALEVEL_2_H
#define CDATALEVEL_2_H

#include "libLidar.hpp" // LIDAR DATA TYPE DEFINITIONS

class CDataLevel_2  
{
	public:
		CDataLevel_2( strcGlobalParameters* ) ;
		~CDataLevel_2()  					  ;
		// void 	Fernald_1983_v0( strcGlobalParameters*, int, int ) ;
		void 	Fernald_1983( strcGlobalParameters*, int, int ) ;

	    double  ***alpha_Aer, **alpha_Mol, *nMol	;
	    double  ***beta_Aer , **beta_Mol  			;
		double  ***pr2, *pr2n, phi, p, ip, CalTerm, beta_Tot ;
		double  *pr2_s 								;
		double  **data_Noise						;
		double  dzr									;
		double  LRM, LR[100]						;
		double  **AOD_LR 							;
		int		nLRs 								;
		int 	indxInitSig, indxEndSig, indxRef	;
		double  R_ref    							;
		bool	is_Noise_Data_Loaded =false 		;

	private:

} ;

#endif