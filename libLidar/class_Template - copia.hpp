
/* * cfernald.hpp * Copyright 2018 Juan V. Pallotta */
#ifndef CFERNALD_H
#define CFERNALD_H


#include "lidarDataTypes.h"
// #include "lidarMathFunc.h" // ERROR POR REDEFINICION DE LAS FUNCIONES MATEMATICAS: splineROOT(), polynomialFit()...
// #include "libLidar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class CFernald
{
	public:
		CFernald();
		virtual ~CFernald();

		void 	FernaldInversion			  	( double*, strcMolecularData*, strcGlobalParameters*, int, double, strcFernaldInversion*, strcAerosolData* ) ;
		void	GetMemVectorsFernaldInversion 	() ;
		void	FreeMemVectorsFernaldInversion 	() ;

	private:
// struct strcFernaldInversion
		int 	nBins	;
		double 	*pr2n   ;
		double 	*phi    ;
		double 	*p 	    ;
		double 	*ip	    ;
		double 	*ipN    ;
		double 	*betaT  ;
		double 	*pr2Fit ;
		double 	ipNref  ;
		double 	intAlphaMol_Ref ;
		double 	*intAlphaMol_r  ;

};

#endif /* CFERNALD_HPP */
