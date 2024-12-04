
/*
 * CMolecularDataAuger.hpp
 * Copyright 2021 Juan V. Pallotta
 */

#ifndef CMOLECULARDATAAUGER_H
#define CMOLECULARDATAAUGER_H

#include "libLidar.hpp"

class CMolecularDataAuger
{
	public:
		CMolecularDataAuger( char*, strcGlobalParameters* ) ;
		virtual ~CMolecularDataAuger();

    	strcMolecularData dataMol ;
	    void GetMem_dataMol( int ) ;
		void Fill_dataMol( strcGlobalParameters* ) ;

	private:
    	strcRadioSoundingData RadSondeData ;
		void ReadMolDBAuger( char*, double, int ) ;

		void RadLowToHighRes() ;
		// void RadLowToHighRes( int nLR, double *xLR, double *yLR, int nHR, double *xHR, double *yHR ) ;
};

#endif
