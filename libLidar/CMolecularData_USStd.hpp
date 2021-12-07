
/*
 * CMolecularDataAuger.hpp
 * Copyright 2021 Juan V. Pallotta
 */

#ifndef CMOLECULARDATA_USSTD_H
#define CMOLECULARDATA_USSTD_H

#include "libLidar.hpp"

class CMolecularData_USStd
{
	public:
		CMolecularData_USStd( char*, strcGlobalParameters* ) ;
		virtual ~CMolecularData_USStd();

    	strcMolecularData *dataMol ;
	    void GetMem_dataMol( int, int ) ;
		void Fill_dataMol( strcGlobalParameters*, int ) ;
    	strcRadioSoundingData RadSondeData ;

	private:
    	// strcRadioSoundingData RadSondeData ;
		void ReadUSSTDfile( const char*, strcGlobalParameters* ) ;

		void RadLowToHighRes() ;
};

#endif
