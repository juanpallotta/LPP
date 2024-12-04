
#include "lidarMathFunc.hpp"

double sum( double *y, int binInic, int binEnd, double *sumOut )
{
	*sumOut = 0 ;

    if ( binInic < binEnd )
    {
        for ( int i=binInic ; i <= binEnd ; i++ )
            *sumOut = *sumOut + y[i] ;

    	return *sumOut ;
    }
    else
    {
        printf("\n\n Wrong call to sum() function (binInic > binEnd)... exit \n\n") ;
        exit(1) ;
    }
}

void cumtrapz ( double dx, double *y, int binInic, int binEnd, double *intFunc )
{
	intFunc[binInic] = 0 ;
    if ( binInic < binEnd )
    {   // binInic > binEnd
        for ( int i=(binInic+1) ; i <= binEnd ; i++ )
            trapz( dx, y, binInic, i, &intFunc[i] ) ;
    }
    else
    {   // binEnd < binInic
        printf("\nWrong call to cumtrapz (binEnd < binInic)\n") ;
    }
}

void trapz( double dx, double *y, int binInic, int binEnd, double *integral )
{
	double sum_ = 0 ;

    if ( binInic < binEnd )
    {
        // for ( int i = binInic ; i <=(binEnd-1) ; i++ )
        for ( int i = binInic ; i <=binEnd ; i++ )
            sum_ = sum_ + (y[i] + y[i+1])/2 ;
    }
    else
    {   // binEnd < binInic
        printf("\nWrong call to trapz (binEnd < binInic)\n") ;
    }
    *integral = sum_ * dx ;
}

void cumtrapz_norm ( double dx, double *y, int binInic, int binRef, int binEnd, double *intFunc_Norm )
{
    for ( int i =binRef; i < (binEnd-1); i++ )
    {   // FORWARD INVERSION
        trapz( dx, y, binRef, i+1, &intFunc_Norm[i] ) ;
    }
    for ( int i =binRef ; i >binInic ; i-- )
    {   // BACKWARD INVERSION
        trapz( -dx, y, i-1, binRef, &intFunc_Norm[i] ) ;
    }
    // printf("\n cumtrapz_norm --> intFunc_Norm[binEnd]= %lf \n", intFunc_Norm[binEnd-1]) ;
}

void diffPr( const double *sig, int nBins, double *diffSig )
{
	for ( int i = 0 ; i<nBins ; i++ )
		diffSig[i] = (double)(sig[i+1] - sig[i]) ;
}

void findIndxMin( double *vec, int indxInic, int indxEnd, int *indxMin, double *min )
{ // ALL INDEXES ARE REFERENCED TO [0-nBins] OF vec
	*min     = DBL_MAX ;
    *indxMin = (int)-10 ;

	for( int i=indxInic ; i<=indxEnd ; i++ )
	{
        if ( fpclassify(vec[i]) != FP_NAN )
        {
            if ( vec[i] < *min )
            {
                *min 	 = vec[i] ;
                *indxMin = (int)i ; // REFERENCED TO [0-nBins] OF vec
            }
        }
        else
            printf("\n findIndxMin --> vec[%d] = %lf (NAN)\n", i, vec[i]) ;
	}
    
    if ( (*indxMin) > indxEnd )
    {
        printf("\tfindIndxMin(error) --> indxInic= %d \t indxMin = %d \t indxEnd= %d\n\tbye...\n", indxInic, *indxMin, indxEnd) ;
        exit(1) ;
    }
    else if ( (*indxMin) <0 )
    {
        printf("\tfindIndxMin(error) --> indxInic= %d \t indxMin = %d \t indxEnd= %d\n\tbye...\n", indxInic, *indxMin, indxEnd) ;
        exit(1) ;
    }
}

void findIndxFirstNeg( double *vec, int indxInic, int indxEnd, int *indxMin, double *min )
{ // ALL INDEXES ARE REFERENCED TO [0-nBins] OF vec
	*min = 0 ;
    int found =0 ;

	for( int i=indxInic ; i<=indxEnd ; i++ )
	{
		if ( vec[i] < *min )
		{
			*min 	 = vec[i] ;
			*indxMin = i 	  ; // REFERENCED TO [0-nBins] OF vec
            found = INT_MAX ;
            break ;
		}
	}
    if ( found == 0 )
        *indxMin = indxInic ;
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

void findIndxMin_void( void *vec, const char *varType, int indxInic, int indxEnd, int *indxMin, void *min )
{ // ALL INDEXES ARE REFERENCED TO [0-nBins] OF vec

    if ( strcmp( varType, "int" ) == 0 )
    {
    	(*(int*)min) = INT_MAX ;
        for( int i=indxInic ; i<=indxEnd ; i++ )
        {
            if ( ((int*)vec)[i] < (*(int*)min) )
            {
                (*(int*)min) = ((int*)vec)[i] ;
                *indxMin = (int)i  ; // REFERENCED TO [0-nBins] OF vec
            }
        }
    }
    if ( strcmp( varType, "double" ) == 0 )
    {
    	(*(double*)min) = DBL_MAX ;
        for( int i=indxInic ; i<=indxEnd ; i++ )
        {
            if ( ((double*)vec)[i] < (*(double*)min) )
            {
                (*(double*)min) = ((double*)vec)[i] ;
                *indxMin = (int)i  ; // REFERENCED TO [0-nBins] OF vec
            }
        }
    }
}

void findIndxMax_void( void *vec, const char *varType, int indxInic, int indxEnd, int *indxMax, void *max )
{ // ALL INDEXES ARE REFERENCED TO [0-nBins] OF vec

    if ( strcmp( varType, "int" ) == 0 )
    {
    	(*(int*)max) = INT_MIN ;
        for( int i=indxInic ; i<=indxEnd ; i++ )
        {
            if ( ((int*)vec)[i] > (*(int*)max) )
            {
                (*(int*)max) = ((int*)vec)[i] ;
                *indxMax = (int)i  ; // REFERENCED TO [0-nBins] OF vec
            }
        }
    }
    if ( strcmp( varType, "double" ) == 0 )
    {
    	(*(double*)max) = DBL_MIN ;
        for( int i=indxInic ; i<=indxEnd ; i++ )
        {
            if ( ((double*)vec)[i] > (*(double*)max) )
            {
                (*(double*)max) = ((double*)vec)[i] ;
                *indxMax = (int)i  ; // REFERENCED TO [0-nBins] OF vec
            }
        }
    }
}

void smooth( double *sig, int indxInit, int indxEnd, int spam, double *sigMeanAvg )
{
	double  m 	= 0 ;

    // if (spam%2 != 0) // IF SPAM IS ODD, MAKE IT EVEN
    //     spam++ ;
    // int halfSpam = (int)floor(spam/2) ;
    
	if( (indxEnd - indxInit) >= spam )
	{
        int     N = indxEnd - indxInit +1 ;
        double *avgSig = (double*) new double[ N ] ; // guardo en este vector los puntos a promediar del vector sig
        for( int i=0 ; i <N; i++ )
            avgSig[i] = sig[indxInit +i] ;

		for( int i=indxInit ; i<=(indxEnd-spam-1) ; i++ )
		{
			for( int b=0 ; b<spam ; b++ )	m = m + sig[i+b] ;
			m 	= (double)(m/spam) ;

			// avgSig[ i-indxInit +halfSpam ] = (double)m ;
			avgSig[ i-indxInit ] = (double)m ;
			m =0 ;
		}
        for (int i =(indxEnd-spam); i <=indxEnd; i++) // PAD THE LAST spam BINS
            avgSig[i] = avgSig[indxEnd-spam-1] ;

		for( int i=indxInit ; i<=indxEnd ; i++ )	sigMeanAvg[i] = (double)avgSig[i -indxInit] ;

        delete avgSig ;
	}
}

// vector<double> smooth(vector<double> arr, int window_size) 
// {
//     vector<double> smoothed_arr;
//     double sum = 0;

//     for(int i = 0; i < window_size; i++) {
//         sum += arr[i];
//     }

//     smoothed_arr.push_back(sum / window_size);

//     for(int i = window_size; i < arr.size(); i++) {
//         sum += arr[i] - arr[i - window_size];
//         smoothed_arr.push_back(sum / window_size);
//     }

//     return smoothed_arr;
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

// function that returns correlation coefficient.
float correlationCoefficient_int(int X[], int Y[], int n)
{
    int sum_X = 0, sum_Y = 0, sum_XY = 0;
    int squareSum_X = 0, squareSum_Y = 0;
  
    for (int i = 0; i < n; i++)
    {
        // sum of elements of array X.
        sum_X = sum_X + X[i];
  
        // sum of elements of array Y.
        sum_Y = sum_Y + Y[i];
  
        // sum of X[i] * Y[i].
        sum_XY = sum_XY + X[i] * Y[i];
  
        // sum of square of array elements.
        squareSum_X = squareSum_X + X[i] * X[i];
        squareSum_Y = squareSum_Y + Y[i] * Y[i];
    }
  
    // use formula for calculating correlation coefficient.
    float corr = (float)(n * sum_XY - sum_X * sum_Y) 
                  / sqrt((n * squareSum_X - sum_X * sum_X) 
                      * (n * squareSum_Y - sum_Y * sum_Y));
  
    return corr;
}

// function that returns correlation coefficient.
double correlationCoefficient_dbl( double X[], double Y[], int n)
{
    double sum_X = 0, sum_Y = 0, sum_XY = 0;
    double squareSum_X = 0, squareSum_Y = 0;
  
    for (int i = 0; i < n; i++)
    {
        // sum of elements of array X.
        sum_X = sum_X + X[i];
  
        // sum of elements of array Y.
        sum_Y = sum_Y + Y[i];
  
        // sum of X[i] * Y[i].
        sum_XY = sum_XY + X[i] * Y[i];
  
        // sum of square of array elements.
        squareSum_X = squareSum_X + X[i] * X[i];
        squareSum_Y = squareSum_Y + Y[i] * Y[i];
    }
  
    // use formula for calculating correlation coefficient.
    double corr = (double)(n * sum_XY - sum_X * sum_Y) 
                  / sqrt((n * squareSum_X - sum_X * sum_X) 
                      * (n * squareSum_Y - sum_Y * sum_Y));
  
    return corr;
}
