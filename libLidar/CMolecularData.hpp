
/*
 * CMolecularData.hpp
 * Copyright 2021 Juan V. Pallotta
 */
#pragma once

#ifndef CMOLECULARDATA_H
#define CMOLECULARDATA_H

#include "libLidar.hpp"
#include "CNetCDF_Lidar.hpp"

class CMolecularData
{
	public:
		CMolecularData( strcGlobalParameters* ) ;
		virtual ~CMolecularData();

    	strcMolecularData 	dataMol ;
		void Get_Mol_Data_L1( strcGlobalParameters* ) ;
		void Get_Mol_Data_L2( strcGlobalParameters* ) ;
		void Get_Mol_Data_L2_v1( strcGlobalParameters*, CNetCDF_Lidar*, int ) ;
	    void GetMem_dataMol( strcGlobalParameters* ) ;
		void Fill_dataMol_L1( strcGlobalParameters* ) ;
		void Fill_dataMol_L1_from_RadSondeData( strcGlobalParameters* ) ;
		void Fill_dataMol_L2_from_RadSondeData( strcGlobalParameters* ) ;
		void Fill_dataMol_L2( strcGlobalParameters* ) ;
		void Elastic_Rayleigh_Lidar_Signal ( double* ) ;
		void TemK_PresPa_to_N_Alpha_Beta_MOL ( double*, double*, double, double, int, double*, double*, double*, double* ) ;
		void TemK_PresPa_to_N_Alpha_Beta_MOL_simple ( double*, double*, double, int, double*, double*, double*, double* ) ;
		void Nmol_Ref_ASL_Site( strcGlobalParameters* ) ;

    	strcRadioSoundingData RadSondeData ;

	private:
		void Mol_Low_To_High_Res( ) 	;
		void Tem_Pres_to_HR_pw() 	;
		void Molecular_Profile_Resampled_Zenithal( strcGlobalParameters* ) ;
};

#endif
