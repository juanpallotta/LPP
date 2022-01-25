
/*
 * CMolecularData.hpp
 * Copyright 2021 Juan V. Pallotta
 */

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
	    // void GetMem_dataMol( int, int ) ;
		void Fill_dataMol( strcGlobalParameters*, int ) ;
    	strcRadioSoundingData RadSondeData ;

	private:
		// void ReadUSSTDfile( const char*, strcGlobalParameters* ) ;
		void Read_range_Temp_Pres_From_File( strcGlobalParameters* ) ;

		void RadLowToHighRes() ;
};

#endif
