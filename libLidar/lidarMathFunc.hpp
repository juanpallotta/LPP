
// #pragma once

#ifndef LIDARMATHFUNC_H
#define LIDARMATHFUNC_H

#ifndef _POLIFITGSL_H
#define _POLIFITGSL_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cmath>
#include <limits.h>
#include <cfloat>
#include <string.h>
#include <vector>

#include <stdbool.h>

#define	 NUMELEM(x) (sizeof(x)/sizeof(*x))

//void linearPieceWiceFit	( int, double*, double*, int, double*, double* 	) ;
void    trapz				( double , double*, int, int, double* 			) ;
double  sum				( double*, int, int, double* 					) ;
void    smooth	    		( double*, int, int, int, double* 				) ;
void    cumtrapz			( double, double*, int, int, double* 			) ;
void    cumtrapz_norm		( double, double*, int, int, int, double* 		) ;
void    diffPr				( const double*, int, double*			 		) ;
void    findIndxMin		( double*, int, int, int*, double* 				) ;
void    findIndxFirstNeg   ( double*, int, int, int*, double* 				) ;
void    findIndxMax		( double*, int, int, int*, double* 				) ;
void    findIndxMax_void	( void*, const char *, int, int, int*, void*    ) ;
void    findIndxMin_void	( void*, const char *, int, int, int*, void*    ) ;
int     polyfitCoeff   	(const double* const, const double* const, unsigned int, unsigned int, double*);

float   correlationCoefficient_int( int*, int*, int ) ;
double  correlationCoefficient_dbl( double*, double*, int ) ;

// bool polynomialfit		( int, int, double*, double*, double *			) ;
// void smoothGSL			( double*, int, int, double*	 				) ;
// void splineGSL			( int, double*, double*, int, double*, double*  ) ;
// void polyfitGSL			( int, double*, double*, int, int, double*, double* ) ;

#endif
#endif
