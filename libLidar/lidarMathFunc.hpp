
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
// void    trapz				( double , double*, int, int, double* 			) ;
double  sum				( double*, int, int, double* 					    ) ;
void    smooth	    		( double*, int, int, int, double* 				) ;
void    cumtrapz			( double, double*, int, int, double* 			) ;
void    cumtrapz_norm		( double, double*, int, int, int, double* 		) ;
// void    trapz               ( double, double*, int, int, double             ) ;
void    diffPr				( const double*, int, double*			 		) ;
void    findIndxMin		( double*, int, int, int*, double* 				    ) ;
void    findIndxFirstNeg   ( double*, int, int, int*, double* 				) ;
void    findIndxMax		( double*, int, int, int*, double* 				    ) ;
void    findIndxMax_void	( void*, const char *, int, int, int*, void*    ) ;
void    findIndxMin_void	( void*, const char *, int, int, int*, void*    ) ;
int     polyfitCoeff   	(const double* const, const double* const, unsigned int, unsigned int, double*);

float   correlationCoefficient_int( int*, int*, int ) ;
double  correlationCoefficient_dbl( double*, double*, int ) ;

// Structure to hold spline coefficients for each interval
// S_i(x) = a + b*(x-x_i) + c*(x-x_i)^2 + d*(x-x_i)^3
typedef struct
    {
        double a, b, c, d, x_i;
    } Spline;
// Forward declarations
void    create_natural_cubic_spline(double x[], double y[], int n, Spline spl[]);
double  evaluate_spline(Spline spl[], int n, double x);

#endif
#endif
