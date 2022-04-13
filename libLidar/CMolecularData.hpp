
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
		void Alpha_Beta_Mol_from_N_Mol( strcMolecularData*, strcGlobalParameters* ) ;
		static void Elastic_Rayleigh_Lidar_Signal ( strcMolecularData*, double* ) ;

    	strcRadioSoundingData RadSondeData ;

	private:
		// void ReadUSSTDfile( const char*, strcGlobalParameters* ) ;
		void Read_range_Temp_Pres_From_File( strcGlobalParameters* ) ;

		void RadLowToHighRes() ;
};

#endif
