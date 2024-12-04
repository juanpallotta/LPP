
/*
 * CMolecularData.hpp
 * Copyright 2021 Juan V. Pallotta
 */
#pragma once

#ifndef CMOLECULARDATA_H
#define CMOLECULARDATA_H

#include "libLidar.hpp"

class CMolecularData
{
	public:
		CMolecularData( strcGlobalParameters* ) ;
		virtual ~CMolecularData();

    	strcMolecularData dataMol ;
		void Get_Mol_Data_L1( strcGlobalParameters* ) ;
		void Get_Mol_Data_L2( strcGlobalParameters* ) ;
	    void GetMem_dataMol( int ) ;
		void Fill_dataMol_L1( strcGlobalParameters* ) ;
		// void Fill_dataMol_L1_from_RadSondeData( strcGlobalParameters* ) ;
		void Fill_dataMol_L2( strcGlobalParameters* ) ;
		// void Alpha_Beta_Mol_from_N_Mol( strcGlobalParameters* ) ;
		void Elastic_Rayleigh_Lidar_Signal ( double* ) ;
		void TemK_PresPa_to_N_Alpha_Beta_MOL ( double*, double*, double, double, int, double*, double*, double*, double* ) ;
		void Nmol_Ref_ASL_Site( strcGlobalParameters* ) ;

    	strcRadioSoundingData RadSondeData ;

	private:
		void Mol_Low_To_High_Res( strcGlobalParameters* ) 	;
		void Tem_Pres_to_HR() 		;
		void Molecular_Profile_Resampled_Zenithal( strcGlobalParameters* ) ;
};

#endif
