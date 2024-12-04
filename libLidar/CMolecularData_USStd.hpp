
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
		void ReadUSSTDfile( const char*, strcGlobalParameters* ) ;
		void Get_Mol_Data_L1( strcGlobalParameters* ) ;

		void Mol_Low_To_High_Res() ;
};

#endif
