
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
	    void GetMem_dataMol( int ) ;
		void Fill_dataMol( strcGlobalParameters* ) ;
		void Fill_dataMol( strcGlobalParameters*, double* ) ;
		void Alpha_Beta_Mol_from_N_Mol( strcGlobalParameters* ) ;
		void Elastic_Rayleigh_Lidar_Signal ( double* ) ;
		void Read_range_Temp_Pres_From_File( strcGlobalParameters* ) ;
		void Nmol_Ref_ASL_Site( strcGlobalParameters* ) ;

    	strcRadioSoundingData RadSondeData ;

	private:
		void RadLowToHighRes() ;
		void Molecular_Profile_Resampled_Zenithal( strcGlobalParameters* ) ;
};

#endif
