
#pragma once

#ifndef CDATALEVEL_2_H
#define CDATALEVEL_2_H

#include <string>
#include <cmath>
#include <cstdlib>
#include <cfloat>
#include "lidarDataTypes.hpp" // LIDAR DATA TYPE DEFINITIONS
#include "CLidar_Operations.hpp"
class CMolecularData ;

class CDataLevel_2  
{
	public:
		CDataLevel_2( strcGlobalParameters* ) ;
		~CDataLevel_2()  					  ;
		void 	FernaldInversion( strcGlobalParameters*, strcMolecularData* 					) ;
		void 	FernaldInversion( double*, strcMolecularData *, strcGlobalParameters *	 		) ;
		void 	Raman_Inversion ( strcGlobalParameters*, strcMolecularData* 					) ;
		void 	Find_Ref_Range    (strcGlobalParameters*								  		) ;
		void 	Find_Ref_Range_v1 (strcGlobalParameters*, strcMolecularData *					) ;

		void  	GetMemStrcErrorSigSet( strcErrorSignalSet*, int, int ) 						 ;
		void 	MonteCarloRandomError( strcGlobalParameters*, strcMolecularData* ) ;
		// void 	MonteCarloSystematicError	( double*,          strcGlobalParameters*, strcMolecularData*, strcIndexMol*, strcFernaldInversion*, strcErrorSignalSet* ) ;
		void 	GetErrSetParam				( char*, int, int, double, strcErrorSignalSet* ) ;

		int 	Download_AERONET_Data( strcGlobalParameters* ) ;
		void 	Load_AERONET_Data    ( strcGlobalParameters* ) ;
		int 	Check_AERONET_Data   ( char* 				 ) ;

		CLidar_Operations 	*oLOp 					;
	    double  ***alpha_Aer = nullptr			    ;
	    double  ***beta_Aer = nullptr				;
		double  **Ki_Fernald = nullptr				;
	    double  **alpha_Aer_synergy = nullptr		;
	    double  **beta_Aer_synergy = nullptr		;
		double  ***pr2 = nullptr, **pr = nullptr	;
		double  ***data_File_L0 = nullptr, ***data_File_L2 = nullptr      ;
		int		*Start_Time_AVG_L2 = nullptr, *Stop_Time_AVG_L2 = nullptr ;
		int		*Start_Time_L0 = nullptr, *Stop_Time_L0 = nullptr 		  ;
		double  *dummy = nullptr, *pr2_s = nullptr			 			  ;
		double  dzr = 0									;
		double  LRM = 0									;	
		double  *LR = nullptr							;
		int		LR_loop_Max = 0							;	
		double  **AOD_LR = nullptr						;
		char	*aeronet_file = nullptr					;
		char	*aeronet_data_level = nullptr			;
		char	*aeronet_path = nullptr					;
		char	*aeronet_site_name = nullptr			;
		int		nLRs = 0								;
		int 	indxInitSig = 0, indxEndSig = 0, *indxRef_Fernald = nullptr;
		double  R_ref = 0								;

		double 	*AOD_Lidar_Time = nullptr				;
		double 	*Angs_Lidar_Time= nullptr				;
		double 	*AERONET_AOD 	= nullptr				;
		double 	*AERONET_Angs 	= nullptr				;
		int		*AERONET_time	= nullptr				;
		int		i_Num_AERONET_data = 0					;
		double	*LR_inv			= nullptr				;
		double	*AOD_inv		= nullptr				;

		double 	pr2_Ref = 0;
		int 	avg_Half_Points_Fernald_Ref = 0;
		double	heightRef_Inversion_ASL = 0, heightRef_Inversion_Start_ASL = 0, heightRef_Inversion_Stop_ASL = 0 ;
		double  ka = 0  ;
		double  KM_ = 0;

		double 	*pr2n = nullptr;
		double 	*phi = nullptr;
		double 	*p = nullptr;
		double 	*ip = nullptr;
		double 	*ipN = nullptr;
		double 	*betaT = nullptr;
		double  *betaMol_Fit = nullptr;
		double 	*intAlphaMol_Ref_r = nullptr  ;

		int		indxMin_absDiff = 0;
		double  minDiff = 0		;
		std::string 	reference_method = "";

	private:
		void 	FernaldInversion_Core( strcGlobalParameters*, int, strcMolecularData*, int, int ) ;
		int 	*indxRef_Fernald_Start = nullptr, *indxRef_Fernald_Stop = nullptr ;

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