
#include "lidarMathFunc.hpp"

double sum( double *y, int binInic, int binEnd, double *sumOut )
{
	*sumOut = 0 ;

	for ( int i=binInic ; i <= binEnd ; i++ )
        *sumOut = *sumOut + y[i] ;

	return *sumOut ;
}


void trapz( double dx, double *y, int binInic, int binEnd, double *integral )
{
	double sum = 0 ;

	for ( int i = (binInic+1) ; i <= binEnd ; i++ )
        sum = sum + (y[i-1] + y[i])/2 ;

    *integral = sum * dx ;
}

void cumtrapz ( double dx, double *y, int binInic, int binEnd, double *intFunc )
{
	intFunc[binInic] = 0 ;
	for ( int i=(binInic+1) ; i <= binEnd ; i++ )
        trapz( dx, y, binInic, i, &intFunc[i] ) ; // trapz( dx, y, 0, i, &intFunc[i] ) ;
}

void diffPr( const double *sig, int nBins, double *diffSig )
{
	for ( int i = 0 ; i<nBins ; i++ )
		diffSig[i] = (double)(sig[i+1] - sig[i]) ;
}

void findIndxMin( double *vec, int indxInic, int indxEnd, int *indxMin, double *min )
{ // ALL INDEXES ARE REFERENCED TO [0-nBins] OF vec
	*min = DBL_MAX ;

	for( int i=indxInic ; i<=indxEnd ; i++ )
	{
		if ( vec[i] < *min )
		{
			*min 	 = vec[i] ;
			*indxMin = i 	  ; // REFERENCED TO [0-nBins] OF vec
		}
	}
}

void findIndxMax( double *vec, int indxInic, int indxEnd, int *indxMax, double *max )
{ // ALL INDEXES ARE REFERENCED TO [0-nBins] OF vec
	*max = DBL_MIN ;

	for( int i=indxInic ; i<=indxEnd ; i++ )
	{
		if ( vec[i] > *max )
		{
			*max = (double)vec[i] ;
			*indxMax = (int)i  ; // REFERENCED TO [0-nBins] OF vec
		}
	}
}

void smooth( double *sig, int indxInit, int indxEnd, int spam, double *sigMeanAvg )
{
	double  m 	= 0 ;
    int     halfSpam = (int)floor(spam/2) ;
    
	if( (indxEnd - indxInit) >= spam )
	{
        int     N = indxEnd - indxInit +1 ;
        double *avgSig = (double*) new double[ N ] ;
        for( int i=0 ; i <N; i++ )
            avgSig[i] = sig[indxInit +i] ;

		for( int i=indxInit ; i<=(indxEnd-spam-1) ; i++ )
		{
			for( int b=0 ; b<spam ; b++ )	m = m + sig[i+b] ;
			m 	= (double)(m/spam) ;

			avgSig[ i-indxInit +halfSpam ] = (double)m ;
			m =0 ;
		}
		for( int i=indxInit ; i<=indxEnd ; i++ )	sigMeanAvg[i] = (double)avgSig[i -indxInit] ;

        delete avgSig ;
	}
}
// void meanAvg( double *sig, int indxInit, int indxEnd, int spam, double *sigMeanAvg )
// {
// 	double m 	= 0 ;

// 	if( (indxEnd - indxInit) >= spam )
// 	{
// 		for( int i=indxInit ; i<=(indxEnd-spam-1) ; i++ )
// 		{
// 			for( int b=0 ; b<spam ; b++ )	m = m + sig[i+b] ;
// 			m 	= m/spam 	;

// 			for( int b=0 ; b<spam ; b++ )	sigMeanAvg[i+b] = m ;
// 			m =0 ;
// 		}
// 		for( int i=(indxEnd-spam-1) ; i<=indxEnd ; i++ )	sigMeanAvg[i] = (double)sigMeanAvg[indxEnd-spam-1] ;
// 	}
// }

//----------------------------------------------------
//
// METHOD:  polyfit
//
// INPUTS:  dependentValues[0..(countOfElements-1)]
//          independentValues[0...(countOfElements-1)]
//          countOfElements
//          order - Order of the polynomial fitting
//
// OUTPUTS: coefficients[0..order] - indexed by term
//               (the (coef*x^3) is coefficients[3])
//
//----------------------------------------------------
int polyfitCoeff( const double* const dependentValues, // X DATA
                  const double* const independentValues, // Y DATA
                  unsigned int        countOfElements,
                  unsigned int        order,				
			      double			  *coefficients )
{
    // Declarations...
    // ----------------------------------
    enum {maxOrder = 5};
    
    double B[maxOrder+1] = {0.0f};
    double P[((maxOrder+1) * 2)+1] = {0.0f};
    double A[(maxOrder + 1)*2*(maxOrder + 1)] = {0.0f};

    double x, y, powx;

    unsigned int ii, jj, kk;

    // Verify initial conditions....
    // ----------------------------------

    // This method requires that the countOfElements > 
    // (order+1) 
    if (countOfElements <= order)
        return -1;

    // This method has imposed an arbitrary bound of
    // order <= maxOrder.  Increase maxOrder if necessary.
    if (order > maxOrder)
        return -1;

    // Begin Code...
    // ----------------------------------

    // Identify the column vector
    for (ii = 0; ii < countOfElements; ii++)
    {
        x    = dependentValues[ii];
        y    = independentValues[ii];
        powx = 1;

        for (jj = 0; jj < (order + 1); jj++)
        {
            B[jj] = B[jj] + (y * powx);
            powx  = powx * x;
        }
    }

    // Initialize the PowX array
    P[0] = countOfElements;

    // Compute the sum of the Powers of X
    for (ii = 0; ii < countOfElements; ii++)
    {
        x    = dependentValues[ii];
        powx = dependentValues[ii];

        for (jj = 1; jj < ((2 * (order + 1)) + 1); jj++)
        {
            P[jj] = P[jj] + powx;
            powx  = powx * x;
        }
    }

    // Initialize the reduction matrix
    //
    for (ii = 0; ii < (order + 1); ii++)
    {
        for (jj = 0; jj < (order + 1); jj++)
        {
            A[(ii * (2 * (order + 1))) + jj] = P[ii+jj];
        }

        A[(ii*(2 * (order + 1))) + (ii + (order + 1))] = 1;
    }

    // Move the Identity matrix portion of the redux matrix
    // to the left side (find the inverse of the left side
    // of the redux matrix
    for (ii = 0; ii < (order + 1); ii++)
    {
        x = A[(ii * (2 * (order + 1))) + ii];
        if (x != 0)
        {
            for (kk = 0; kk < (2 * (order + 1)); kk++)
            {
                A[(ii * (2 * (order + 1))) + kk] = 
                    A[(ii * (2 * (order + 1))) + kk] / x;
            }

            for (jj = 0; jj < (order + 1); jj++)
            {
                if ((jj - ii) != 0)
                {
                    y = A[(jj * (2 * (order + 1))) + ii];
                    for (kk = 0; kk < (2 * (order + 1)); kk++)
                    {
                        A[(jj * (2 * (order + 1))) + kk] = 
                            A[(jj * (2 * (order + 1))) + kk] -
                            y * A[(ii * (2 * (order + 1))) + kk];
                    }
                }
            }
        }
        else
        {
            // Cannot work with singular matrices
            return -1;
        }
    }

    // Calculate and Identify the coefficients
    for (ii = 0; ii < (order + 1); ii++)
    {
        for (jj = 0; jj < (order + 1); jj++)
        {
            x = 0;
            for (kk = 0; kk < (order + 1); kk++)
            {
                x = x + (A[(ii * (2 * (order + 1))) + (kk + (order + 1))] *
                    B[kk]);
            }
            coefficients[ii] = x;
        }
    }

    return 0;
}
