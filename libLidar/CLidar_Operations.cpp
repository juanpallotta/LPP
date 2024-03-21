
#include "CLidar_Operations.hpp"

CLidar_Operations::CLidar_Operations( strcGlobalParameters *glbParam )
{
	dummy 	 	= (double*) new double[glbParam->nBins] ; 
    pr_NObkg_i	= (double*) new double[glbParam->nBins] ; 
	pr2_i 		= (double*) new double[glbParam->nBins] ;	
	// errRMS_mol	= (double*) new double[glbParam->nBins] ;	
	// errRMS_k 	= (double*) new double[glbParam->nBins] ;	
	// rate 		= (double*) new double[glbParam->nBins] ;	
	// k_ones 		= (double*) new double[glbParam->nBins] ;
	// R2_array	= (double*) new double[glbParam->nBins] ;

	ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "nBiasRes_Auto", "int" , (int*)&nBiasRes_Auto) ;

	errRMS_Bias = (double*) new double [ nBiasRes_Auto ] ;
	b_i 		= (double*) new double [ nBiasRes_Auto +1 ] ;
	coeff 		= (double*) new double[2] 				;
}

CLidar_Operations::~CLidar_Operations()
{
	delete dummy 	;
}

// MakeRangeCorrected() WITHOUT BACKGROUND NOISE FILE, USING 'MEAN', 'FIT', 'NO_BKG' OR 'AUTO' METHODS TO REMOVE BIAS.
void CLidar_Operations::MakeRangeCorrected( strcLidarSignal *evSig, strcGlobalParameters *glbParam, strcMolecularData *dataMol )
{
	if ( (glbParam->DAQ_Type[glbParam->chSel] ==0) || (glbParam->DAQ_Type[glbParam->chSel] ==1) )
		BiasCorrection( (strcLidarSignal*)evSig, (strcGlobalParameters*)glbParam, (strcMolecularData*)dataMol ) ;
	else
	{
		// for ( int i=0 ; i<glbParam->nBins ; i++ ) 	
		for ( int i=0 ; i<glbParam->nBins_Ch[glbParam->chSel] ; i++ ) 	
		{
			evSig->pr_noBias[i] = (double)evSig->pr[i] ;
			evSig->pr_noBkg [i] = (double)evSig->pr[i] ;
		}
	}
		for ( int i=0 ; i<glbParam->nBins ; i++ ) 	evSig->pr2[i] = evSig->pr_noBias[i] * pow(glbParam->r[i], 2) ;
}

// IF data_noise IS PASSED AS ARGUMENT, IT IS USED
void CLidar_Operations::MakeRangeCorrected( strcLidarSignal *evSig, strcGlobalParameters *glbParam, double **data_Noise, strcMolecularData *dataMol )
{
	if ( (glbParam->DAQ_Type[glbParam->chSel] ==0) || (glbParam->DAQ_Type[glbParam->chSel] ==1) )
	{
		for (int i =0; i <glbParam->nBins; i++)
			evSig->pr_noBkg[i] = (double)( evSig->pr[i] - data_Noise[glbParam->chSel][i] ) ;

		BiasCorrection( (strcLidarSignal*)evSig, (strcGlobalParameters*)glbParam, (strcMolecularData*)dataMol ) ;
	}
	else
	{
		for ( int i=0 ; i<glbParam->nBins ; i++ ) 	
		{
			evSig->pr_noBias[i] = (double)evSig->pr[i] ;
			evSig->pr_noBkg [i] = (double)evSig->pr[i] ;
		}
	}
		for ( int i=0 ; i<glbParam->nBins ; i++ ) 	evSig->pr2[i] = evSig->pr_noBias[i] * pow(glbParam->r[i], 2) ;
}

void CLidar_Operations::BiasCorrection( strcLidarSignal *evSig, strcGlobalParameters *glbParam, strcMolecularData *dataMol )
{
	ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "BkgCorrMethod", "string" , (char*)glbParam->BkgCorrMethod ) ;

	if ( (strcmp( glbParam->BkgCorrMethod, "NO_BKG" ) ==0) || (strcmp( glbParam->BkgCorrMethod, "no_bkg" ) ==0) )
	{
		for (int i =0; i <glbParam->nBins ; i++)
		{
			evSig->pr_noBias[i] = evSig->pr[i] ;
			evSig->pr_noBkg[i]  = evSig->pr[i] ;
		}

		// int	indxMaxRange = Get_Max_Range( (double*)evSig->pr_noBias, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam) ;
		Get_Max_Range( (double*)evSig->pr_noBias, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam) ;
	}
	else if ( (strcmp( glbParam->BkgCorrMethod, "MEAN" ) ==0) || (strcmp( glbParam->BkgCorrMethod, "mean" ) ==0) )
	{
		Bias_Substraction_Mean( (double*)evSig->pr, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (double*)evSig->pr_noBias ) ;
	}
	else if ( (strcmp( glbParam->BkgCorrMethod, "FIT" ) ==0) || (strcmp( glbParam->BkgCorrMethod, "fit" ) ==0) )
	{
		Bias_Substraction_MolFit( (strcMolecularData*)dataMol, (const double*)evSig->pr, (strcGlobalParameters*)glbParam, (double*)evSig->pr_noBias ) ;
	}
	else if ( (strcmp( glbParam->BkgCorrMethod, "AUTO" ) ==0) || (strcmp( glbParam->BkgCorrMethod, "auto" ) ==0) )
	{
		double 	Bias_Pr ;
			Bias_Substraction_Auto( (double*)evSig->pr, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (double*)evSig->pr_noBias, (double*)&Bias_Pr ) ;
	}
	else
	{
		printf( "\n CLidar_Operations::BiasCorrection(): BkgCorrMethod = %s but there is no noise information... extracting bias using the mean of the last %d bins.' \n", glbParam->BkgCorrMethod, glbParam->nBinsBkg ) ;
		Bias_Substraction_Mean( (double*)evSig->pr, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (double*)evSig->pr_noBias ) ;
	}
}

void CLidar_Operations::Bias_Substraction_Auto( double *pr, strcMolecularData *dataMol, strcGlobalParameters *glbParam, double *pr_noBias, double *Bias_Pr )
{
	int 	indxMaxRange = Get_Max_Range( (double*)pr, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam) ;

	if ( indxMaxRange <= (glbParam->nBins-1) )
	{
		double 	b_ref_max = 0.0 ;
		double 	b_ref_min = 0.0 ;

	 // FIRST BIAS OBTAINED BY APPLYING A MOLECULAR FIT USING nBinsBkg POINTS FROM THE INDEX glbParam->indxEndSig_ev[glbParam->evSel]
		fitParam.indxEndFit  = indxMaxRange ; 
		fitParam.indxInicFit = fitParam.indxEndFit - glbParam->nBinsBkg ;
		fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInicFit +1;
		Fit( (double*)pr, (double*)dataMol->prMol, dataMol->nBins , "wB", "NOTall", (strcFitParam*)&fitParam, (double*)dummy ) ;

		b_ref_max = 1.5* fitParam.b ;
		b_ref_min = 0.5* fitParam.b ;
		double b_step 	= (b_ref_max - b_ref_min) /nBiasRes_Auto ;

		for ( int s =0; s <nBiasRes_Auto; s++ )
			b_i[s] = (double) b_ref_min + s *b_step ;
		b_i[nBiasRes_Auto] = 0 ; // IN CASE OF UNBIASED LIDAR SIGNAL

		for( int l=0 ; l< nLoopFindBias ; l++ ) // nLoopFindBias = 3
		{
			for (int b =0; b <nBiasRes_Auto; b++)
			{
				for ( int i =0 ; i <glbParam->nBins ; i++ )
				{
					pr_NObkg_i[i]  = pr[i] - b_i[b] ;
					pr2_i[i]       = pr_NObkg_i[i] * pow(glbParam->r[i], 2) ;
				}
				Fit( (double*)pr2_i, (double*)dataMol->pr2Mol, glbParam->nBins , "wOutB", "NOTall", (strcFitParam*)&fitParam, (double*)dummy ) ;
				errRMS_Bias[b] = fitParam.squared_sum_fit ;
			}
				polyfitCoeff( (const double* const) b_i				, // X DATA
							(const double* const  ) errRMS_Bias		, // Y DATA
							(unsigned int         ) nBiasRes_Auto	,
							(unsigned int		  ) 2				,
							(double*			  ) coeff	 ) 		;
				*Bias_Pr = (double) -coeff[1] /(2*coeff[2]) ;

				b_ref_max = 1.5* (*Bias_Pr) ;
				b_ref_min = 0.5* (*Bias_Pr) ;
				b_step 	= (b_ref_max - b_ref_min) /nBiasRes_Auto ;
				for ( int b =0; b <nBiasRes_Auto; b++ )
					b_i[b] = (double) b_ref_min + b *b_step ;
		}
			for ( int i =0; i <glbParam->nBins ; i++)
				pr_noBias[i] = (pr[i] - *Bias_Pr);
	} // if ( indxMaxRange <= (glbParam->nBins-1) )
	else
	{
		printf("\n CLidar_Operations::Bias_Substraction_Auto() (glbParam->indxEndSig_ev[glbParam->evSel] > (glbParam->nBins-1)) - Background substraction metod set is %s, but MEAN method is applied because nBins= %d and indxMaxRange= %d).\n", glbParam->BkgCorrMethod, glbParam->nBins, glbParam->indxEndSig_ev[glbParam->evSel] ) ;
		Bias_Substraction_Mean( (double*)pr, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (double*)pr_noBias) ;
	}
}

void CLidar_Operations::Find_Max_Range( double *pr, double *prMol, strcGlobalParameters *glbParam, int *indxMaxRange )
{
	// int indxMax_ ;
	smooth( (double*)pr, (int)0, (int)(glbParam->nBins_Ch[glbParam->chSel]-1), (int)3, (double*)dummy ) ;

	fitParam.indxEndFit  = glbParam->nBins_Ch[glbParam->chSel] - 1  	 ;
    fitParam.indxInicFit = fitParam.indxEndFit - glbParam->nBinsBkg 	 ;
	fitParam.nFit		 = fitParam.indxEndFit - fitParam.indxInicFit +1 ;

	int i=0 ;
	printf("\n") ;
	do
	{
		Fit( (double*)pr, (double*)prMol, glbParam->nBins_Ch[glbParam->chSel] , "wB", "NOTall", (strcFitParam*)&fitParam, (double*)dummy ) ;

		i = i+1 ;
		fitParam.indxEndFit  = glbParam->nBins_Ch[glbParam->chSel] - i       ;
		fitParam.indxInicFit = fitParam.indxEndFit - glbParam->nBinsBkg 	 ;
	} while( (fitParam.indxInicFit >glbParam->indxInitSig) && (fitParam.R2 <0.1) ) ; //! CHANGE TO THE PEAK OF THE LIDAR SIGNAL

	fitParam.indxEndFit  = fitParam.indxEndFit + 1       ;
	fitParam.indxInicFit = fitParam.indxEndFit + glbParam->nBinsBkg 	 ;

	if ( fitParam.indxEndFit > (glbParam->nBins_Ch[glbParam->chSel]-1) )
	{
		printf( "*ERROR* CLidar_Operations::Find_Max_Range() -> fitParam.indxEndFit (%d) > (glbParam->nBins-1) (%d) --> fitParam.indxEndFit = (glbParam->nBins-1)\n", 
				fitParam.indxEndFit, glbParam->nBins-1 ) ;
		fitParam.indxEndFit = glbParam->nBins_Ch[glbParam->chSel] -1 ;
	}

	*indxMaxRange = fitParam.indxEndFit ;

    int indxWL_PDLx ;
	if ( strcmp( glbParam->exeFile, "./lidarAnalysis_PDL1" ) ==0 )
		ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"indxWL_PDL1", (const char*)"int", (int*)&indxWL_PDLx ) ;
	else if ( strcmp( glbParam->exeFile, "./lidarAnalysis_PDL2" ) ==0 )
	    ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"indxWL_PDL2", (const char*)"int", (int*)&indxWL_PDLx ) ;

	if ( glbParam->chSel== indxWL_PDLx )
	{
		glbParam->indxEndSig_ev[glbParam->evSel] = (int)  *indxMaxRange ;
		glbParam->rEndSig_ev   [glbParam->evSel] = (double)glbParam->indxEndSig_ev[glbParam->evSel] * glbParam->dr ;
	// printf( "CLidar_Operations::Find_Max_Range() (2) --> glbParam->rEndSig_ev[glbParam->evSel]= %lf \t glbParam->indxEndSig_ev[glbParam->evSel]= %d", 
														//    glbParam->rEndSig_ev[glbParam->evSel], glbParam->indxEndSig_ev[glbParam->evSel] ) ;
	}
		printf( "CLidar_Operations::Find_Max_Range() --> ev: %d \t Max. Range= %lf\t R2= %lf\n", 
															glbParam->evSel, glbParam->rEndSig_ev[glbParam->evSel], fitParam.R2 ) ;
} // void CLidar_Operations::Find_Max_Range( double *pr, double *prMol, strcGlobalParameters *glbParam, int *indxMaxRange )

/*  BACKUP
void CLidar_Operations::Find_Max_Range( double *pr, double *prMol, strcGlobalParameters *glbParam, int *indxMaxRange )
{
	int indxMax_ ;
	smooth( (double*)pr, (int)0, (int)(glbParam->nBins_Ch[glbParam->chSel]-1), (int)15, (double*)dummy ) ;

	fitParam.indxEndFit  = glbParam->nBins_Ch[glbParam->chSel] - 1  	 ;
    fitParam.indxInicFit = fitParam.indxEndFit - glbParam->nBinsBkg 	 ;
	fitParam.nFit		 = fitParam.indxEndFit - fitParam.indxInicFit +1 ;

	for (int j =0; j <glbParam->nBins_Ch[glbParam->chSel]; j++)
	{
		k_ones[j] 		= (double)1.0 		;
		errRMS_mol[j] 	= (double)DBL_MAX 	; 
		errRMS_k[j] 	= (double)0.0 		;
		rate[j] 		= (double)0.0 		;
	}

	int i=0 ;
	printf("\n") ;
	do
	{
		// Fit( (double*)pr, (double*)prMol, glbParam->nBins , "wB", "NOTall", (strcFitParam*)&fitParam, (double*)dummy ) ;
		Fit( (double*)pr, (double*)prMol, glbParam->nBins_Ch[glbParam->chSel] , "wB", "NOTall", (strcFitParam*)&fitParam, (double*)dummy ) ;
		errRMS_mol[fitParam.indxEndFit] = (double)fitParam.squared_sum_fit ;
			
			if ( fitParam.indxInicFit == (glbParam->nBins_Ch[glbParam->chSel] - 1 - glbParam->nBinsBkg) )
				printf("\n CLidar_Operations::Find_Max_Range() START R2 = %e", fitParam.R2 ) ;
			else if ( fitParam.indxInicFit == 2000 )
				printf("\n CLidar_Operations::Find_Max_Range() END   R2 = %e\n", fitParam.R2 ) ;

		// Fit( (double*)pr, (double*)k_ones, glbParam->nBins , "wOutB", "NOTall", (strcFitParam*)&fitParam, (double*)dummy ) ;
		Fit( (double*)pr, (double*)k_ones, glbParam->nBins_Ch[glbParam->chSel] , "wOutB", "NOTall", (strcFitParam*)&fitParam, (double*)dummy ) ;
		errRMS_k[fitParam.indxEndFit] = (double)fitParam.squared_sum_fit ;

		i = i+1 ;
		// fitParam.indxEndFit  = glbParam->nBins - i            				 ;
		fitParam.indxEndFit  = glbParam->nBins_Ch[glbParam->chSel] - i       ;
		fitParam.indxInicFit = fitParam.indxEndFit - glbParam->nBinsBkg 	 ;
		fitParam.nFit		 = fitParam.indxEndFit - fitParam.indxInicFit +1 ;
	} while( fitParam.indxInicFit >glbParam->indxInitSig ) ; //! CHANGE TO THE PEAK OF THE LIDAR SIGNAL

	// for ( i =glbParam->indxInitSig; i <(glbParam->nBins - 1); i++ )
	for ( i =glbParam->indxInitSig; i <(glbParam->nBins_Ch[glbParam->chSel] - 1); i++ )
		rate[i] = (double)(errRMS_k[i] / errRMS_mol[i]) ;

	// for (i =(glbParam->nBins - 1); i>=0; i--)
	for (i =(glbParam->nBins_Ch[glbParam->chSel] - 1); i>=0; i--)
	{
		// if ( rate[i] >1.1 )
		if ( rate[i] >1.02 )
		{
			// printf("\n CLidar_Operations::Find_Max_Range() exiting with R2 = %e \n\t n= %d -- squared_sum_fit_vs_Mean= %lf -- squared_sum_fit= %lf \n", 
			// 		fitParam.R2, fitParam.nFit, fitParam.squared_sum_fit_vs_Mean, fitParam.squared_sum_fit ) ;
			indxMax_ = i ;
			break;
		}
	}

	// if ( indxMax_ > (glbParam->nBins-1) )
	if ( indxMax_ > (glbParam->nBins_Ch[glbParam->chSel]-1) )
	{
		printf( "\nCLidar_Operations::Find_Max_Range() (1) -> indxMax_ (%d) > (glbParam->nBins-1) (%d) \n indxMax_ = (glbParam->nBins-1)", indxMax_, glbParam->nBins-1 ) ;
		// indxMax_ = glbParam->nBins -1 ;
		indxMax_ = glbParam->nBins_Ch[glbParam->chSel] -1 ;
	}

	*indxMaxRange = indxMax_ ;

    int indxWL_PDLx ;
	if ( strcmp( glbParam->exeFile, "./lidarAnalysis_PDL1" ) ==0 )
		ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"indxWL_PDL1", (const char*)"int", (int*)&indxWL_PDLx ) ;
	else if ( strcmp( glbParam->exeFile, "./lidarAnalysis_PDL2" ) ==0 )
	    ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"indxWL_PDL2", (const char*)"int", (int*)&indxWL_PDLx ) ;

	if ( glbParam->chSel== indxWL_PDLx )
	{
		glbParam->indxEndSig_ev[glbParam->evSel] = (int)  *indxMaxRange ; // glbParam->nBins -1 ; // 
		glbParam->rEndSig_ev   [glbParam->evSel] = (double)glbParam->indxEndSig_ev[glbParam->evSel] * glbParam->dr ;
	// printf( "CLidar_Operations::Find_Max_Range() (2) --> glbParam->rEndSig_ev[glbParam->evSel]= %lf \t glbParam->indxEndSig_ev[glbParam->evSel]= %d", 
														//    glbParam->rEndSig_ev[glbParam->evSel], glbParam->indxEndSig_ev[glbParam->evSel] ) ;
	}
	// printf( "\n ev: %d \t glbParam->indxInitSig= %d \n indxMin_Pr= %d \n Max. Range: %lf \n indxMaxRange= %d", glbParam->evSel, glbParam->indxInitSig, indxMin_Pr, glbParam->rEndSig_ev[glbParam->evSel], *indxMaxRange ) ;
	// printf( "\nCLidar_Operations::Find_Max_Range() --> ev: %d \t Max. Range= %lf\n glbParam->indxEndSig_ev[glbParam->evSel]= %d", 
														// glbParam->evSel, glbParam->rEndSig_ev[glbParam->evSel], glbParam->indxEndSig_ev[glbParam->evSel] ) ;
} // void CLidar_Operations::Find_Max_Range( double *pr, double *prMol, strcGlobalParameters *glbParam, int *indxMaxRange )
*/

// BIAS SUBSTRACTION METHODS
void CLidar_Operations::Bias_Substraction_Mean( double *sig, strcMolecularData *dataMol, strcGlobalParameters *glbParam, double *pr_noBkg)
{
	int 	indxMaxRange = Get_Max_Range( (double*)sig, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam) ;

	// if ( indxMaxRange<(glbParam->nBins-1) ) // OK
	if ( indxMaxRange<(glbParam->nBins_Ch[glbParam->chSel]-1) ) // OK
	{
		// fitParam.indxInicFit = (int)round( (glbParam->nBins-1 + glbParam->indxEndSig_ev[glbParam->evSel])/2 ) ;
	 	// fitParam.indxInicFit = (int)round( (glbParam->nBins-1 + indxMaxRange)/2 ) ;
	 	fitParam.indxInicFit = (int)round( (glbParam->nBins_Ch[glbParam->chSel]-1 + indxMaxRange)/2 ) ;
		// fitParam.indxEndFit  = glbParam->nBins -1 ;
		fitParam.indxEndFit  = glbParam->nBins_Ch[glbParam->chSel] -1 ;
		fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInicFit +1;
	}
	else
	{
		// fitParam.indxInicFit = glbParam->nBins -1 - glbParam->nBinsBkg ;
		fitParam.indxInicFit = glbParam->nBins_Ch[glbParam->chSel] -1 - glbParam->nBinsBkg ;
		// fitParam.indxEndFit  = glbParam->nBins -1 ;
		fitParam.indxEndFit  = glbParam->nBins_Ch[glbParam->chSel] -1 ;
		fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInicFit +1;
	}
	double 	bkgMean = 0 ;
	for( int j=fitParam.indxInicFit ; j<fitParam.indxEndFit ; j++ ) bkgMean = bkgMean + sig[j] ;

	bkgMean = bkgMean /fitParam.nFit ;
	// for ( int i=0 ; i<glbParam->nBins ; i++ ) 	pr_noBkg[i] = (double)(sig[i] - bkgMean) ;
	for ( int i=0 ; i<glbParam->nBins_Ch[glbParam->chSel] ; i++ ) 	pr_noBkg[i] = (double)(sig[i] - bkgMean) ;
}

void CLidar_Operations::Bias_Substraction_MolFit(strcMolecularData *dataMol, const double *prEl, strcGlobalParameters *glbParam, double *pr_noBkg)
{
	int 	indxMaxRange = Get_Max_Range( (double*)prEl, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam) ;

	// if ( indxMaxRange < (glbParam->nBins-1) )
	if ( indxMaxRange < (glbParam->nBins_Ch[glbParam->chSel]-1) )
	{
		fitParam.indxEndFit  = indxMaxRange ;
		fitParam.indxInicFit = fitParam.indxEndFit - glbParam->nBinsBkg ;
		fitParam.nFit	   	 = fitParam.indxEndFit - fitParam.indxInicFit +1;

		Fit( (double*)prEl, (double*)dataMol->prMol, dataMol->nBins , "wB", "NOTall", (strcFitParam*)&fitParam, (double*)dummy ) ;
			for ( int i=0 ; i<dataMol->nBins ; i++ ) 	pr_noBkg[i] = (double)(prEl[i] - fitParam.b) ; 
	}
	else
	{
		// printf("\n(ev= %d)Background substraction metod set is %s, but MEAN method is applied (nBins= %d \t indxMaxRange= %d).\n", glbParam->evSel, glbParam->BkgCorrMethod, glbParam->nBins, glbParam->indxEndSig_ev[glbParam->evSel] ) ;
		printf("\n(ev= %d)Background substraction metod set is %s, but MEAN method is applied (nBins= %d \t indxMaxRange= %d).\n", glbParam->evSel, glbParam->BkgCorrMethod, glbParam->nBins_Ch[glbParam->chSel], glbParam->indxEndSig_ev[glbParam->evSel] ) ;
		Bias_Substraction_Mean( (double*)prEl, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (double*)pr_noBkg) ;
	}
}

/*
CLidar_Operations::Get_Max_Range
GET THE MAXIMUN RANGE OF THE SIGNAL sig AND SAVE IT IN THE VARIABLE indxMaxRange
IF glbParam->indxWL_PDLx == glbParam->chSel ---> glbParam->indxEndSig_ev[glbParam->evSel] = indxMaxRange
*/
int CLidar_Operations::Get_Max_Range( double *sig, strcMolecularData *dataMol, strcGlobalParameters *glbParam )
{
	int indxMaxRange ;

	if ( glbParam->rEndSig <0 )
	{
		Find_Max_Range( (double*)sig, (double*)dataMol->prMol, (strcGlobalParameters*)glbParam, (int*)&indxMaxRange ) ;
		return indxMaxRange ;
		// glbParam->indxEndSig_ev[glbParam->evSel] 
		// glbParam->rEndSig_ev[glbParam->evSel]
		// LOADED IN Find_Max_Range(...) IF glbParam->chSel == indxWL_PDLx
	}
	else 
	{ // MAXIMUN RANGE IS *NOT* OBTAINED AUTOMATICALLY

		indxMaxRange = glbParam->indxEndSig ;

		if ( // SAVE glbParam->indxEndSig_ev[glbParam->evSel] AND glbParam->rEndSig_ev[glbParam->evSel] IN CASE IT'S NECESSARY
		( ( strcmp( glbParam->exeFile, "./lidarAnalysis_PDL1" ) ==0 ) && ( glbParam->indxWL_PDL1 == glbParam->chSel ) ) 
		||
		( ( strcmp( glbParam->exeFile, "./lidarAnalysis_PDL2" ) ==0 ) && ( glbParam->indxWL_PDL2 == glbParam->chSel ) )
		)
		{ // SAVING THE MAXIMUN RANGES OF THE CHANNEL INDEX	indxWL_PDLx, BASED ON THE 
			glbParam->indxEndSig_ev[glbParam->evSel] = (int) glbParam->indxEndSig ;
			glbParam->rEndSig_ev   [glbParam->evSel] = glbParam->indxEndSig_ev[glbParam->evSel] * glbParam->dr ;
		}
		return indxMaxRange ;
	}
} // int CLidar_Operations::Get_Max_Range()

void CLidar_Operations::Average_in_Time_Lidar_Profiles( strcGlobalParameters *glbParam, double ***dataFile, double ***dataFile_AVG, 
														int *Raw_Data_Start_Time    , int *Raw_Data_Stop_Time, 
														int *Raw_Data_Start_Time_AVG, int *Raw_Data_Stop_Time_AVG	)
{
	printf( "\n\n" ) ;
    for ( int fC=0 ; fC <glbParam->nEventsAVG ; fC++ )
    {
        printf("Averaging in time Cluster Nº %d \n", fC ) ;
		for ( int c=0 ; c <glbParam->nCh ; c++ )
		{
			for ( int b=0 ; b <glbParam->nBins_Ch[c] ; b++ )
			{
				for ( int t=0 ; t <glbParam->numEventsToAvg ; t++ )
				{
					dataFile_AVG[fC][c][b] = (double) dataFile_AVG[fC][c][b] + dataFile[ fC *glbParam->numEventsToAvg +t ][c][b] ;
					if( (b==0) && (c==0) )
					{	// TIME IS *NOT* AVERAGED!!!!
						if ( t ==0 )
							Raw_Data_Start_Time_AVG[fC]	  = (long)Raw_Data_Start_Time[ fC *glbParam->numEventsToAvg ] ;
						if ( t ==(glbParam->numEventsToAvg -1) )
							Raw_Data_Stop_Time_AVG[fC] 	  = (long)Raw_Data_Stop_Time[ fC *glbParam->numEventsToAvg +t ] ;
					}
				}
					if ( glbParam->DAQ_Type[c] == 0 ) // IF IS A PHOTON-CURRENT CHANNEL, THE SIGNALS HAVE TO BE AVERAGED
						dataFile_AVG[fC][c][b]      = (double)(dataFile_AVG[fC][c][b] /glbParam->numEventsToAvg) ;
			}
		} // for ( int c=0 ; c <glbParam->nCh ; c++ )

		for ( int t=0 ; t <glbParam->numEventsToAvg ; t++ )
		{
			glbParam->aAzimuthAVG[fC] 	  = glbParam->aAzimuthAVG[fC]     + glbParam->aAzimuth[fC*glbParam->numEventsToAvg +t]    ;
			glbParam->aZenithAVG[fC]  	  = glbParam->aZenithAVG[fC]      + glbParam->aZenith [fC*glbParam->numEventsToAvg +t]    ;
			glbParam->temp_K_agl_AVG[fC]  = glbParam->temp_K_agl_AVG[fC]  + glbParam->temp_K_agl[fC*glbParam->numEventsToAvg +t]  ;
			glbParam->pres_Pa_agl_AVG[fC] = glbParam->pres_Pa_agl_AVG[fC] + glbParam->pres_Pa_agl[fC*glbParam->numEventsToAvg +t] ;
		}
			glbParam->aAzimuthAVG[fC] 	  = glbParam->aAzimuthAVG[fC]     /glbParam->numEventsToAvg ;
			glbParam->aZenithAVG[fC]  	  = glbParam->aZenithAVG[fC]      /glbParam->numEventsToAvg ;
			glbParam->temp_K_agl_AVG[fC]  = glbParam->temp_K_agl_AVG[fC]  /glbParam->numEventsToAvg ;
			glbParam->pres_Pa_agl_AVG[fC] = glbParam->pres_Pa_agl_AVG[fC] /glbParam->numEventsToAvg ;
// printf("\n CLidar_Operations::Average_in_Time_Lidar_Profiles() --> glbParam->aZenithAVG[%d]= %lf\n", fC, glbParam->aZenithAVG[fC] ) ;
	} // for ( int fC=0 ; fC <glbParam->nEventsAVG ; fC++ )
}

void CLidar_Operations::Lidar_Signals_Corrections( strcGlobalParameters *glbParam, CMolecularData *oMolData, double **ovlp, double **data_Noise, double ***data_File_Lx, double ***pr_corr, double ***pr2 )
{
    strcLidarSignal 	evSig ;
    GetMem_evSig( (strcLidarSignal*) &evSig, (strcGlobalParameters*)glbParam );

	ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"PHO_MAX_COUNT_MHz", (const char*)"double", (double*)&glbParam->PHO_MAX_COUNT_MHz ) ;

    for ( int e=0 ; e <glbParam->nEventsAVG ; e++ )
    {
        glbParam->evSel = e ;
printf("\n\n\nApplying corrections to the lidar event number: %d/%d -------------------------------------------------------------------------------------------------", e, (glbParam->nEventsAVG -1) ) ;
        for ( int c=0 ; c <glbParam->nCh ; c++ )
        {
            glbParam->chSel = (int)c ;
			switch ( glbParam->DAQ_Type[c] )
			{
				case 0:
						printf( "\n Channel: %02d - %04d nm - (Analog) - Correction Applied -------------------> ", c, glbParam->iLambda[c] ) ;
						break;
				case 1:
						printf( "\n Channel: %02d - %04d nm - (Photon Counting) - Correction Applied ----------> ", c, glbParam->iLambda[c] ) ;
						break;
				case 2:
						printf( "\n Channel: %02d - %04d nm - (Analog Squared) - Correction Applied -----------> ", c, glbParam->iLambda[c] ) ;
						break;
				case 3:
						printf( "\n Channel: %02d - %04d nm - (Photon Counting Squared) - Correction Applied --> ", c, glbParam->iLambda[c] ) ;
						break;
				case 4:
						printf( "\n Channel: %02d - %04d nm - (Power Meter) - Correction Applied --------------> ", c, glbParam->iLambda[c] ) ;
						break;
				case 5:
						printf( "\n Channel: %02d - %04d nm - (Overflow) - Correction Applied -----------------> ", c, glbParam->iLambda[c] ) ;
						break;
			}
            // OFFSET CORRECTIONS /*------------------------------------------------------------------------*/
            // PHOTON-CURRENT SIGNALS --> THE SIGNAL HAVE TO MOVE *BACKWARD* glbParam->indxOffset[c] BINS
			printf("| Offset | ") ;
            if ( glbParam->indxOffset[c] >=0 ) 
            {
                for(int b =0; b <(glbParam->nBins_Ch[glbParam->chSel] -glbParam->indxOffset[c]); b++)
					pr_corr[e][c][b] = (double)data_File_Lx[e][c][b +glbParam->indxOffset[c]] ;
                for ( int b=(glbParam->nBins_Ch[glbParam->chSel] -glbParam->indxOffset[c]) ; b <glbParam->nBins_Ch[glbParam->chSel] ; b++ )
					pr_corr[e][c][b] = (double)data_File_Lx[e][c][glbParam->nBins_Ch[glbParam->chSel] -glbParam->indxOffset[c]] ;
            }
            else // glbParam->indxOffset[c] <0 // PHOTON-COUNTING SIGNALS --> THE SIGNAL HAVE TO MOVE FORWARD glbParam->indxOffset[c]
            {
                for ( int b=(glbParam->nBins_Ch[glbParam->chSel]-1) ; b >=(-1*glbParam->indxOffset[c]) ; b-- )
					pr_corr[e][c][b] = (double)data_File_Lx[e][c][b +glbParam->indxOffset[c]] ;
                for(int b =0 ; b <(-1*glbParam->indxOffset[c]) ; b++)
					pr_corr[e][c][b] = (double)0.0 ;
            } /*--------------------------------------------------------------------------------------------*/

            // DESATURATION OF THE PHOTON COUNTING CHANNELS ------------------------------------------------
            if ( glbParam->DAQ_Type[c] == 1 )
            {
				for (int b =0; b <glbParam->nBins_Ch[glbParam->chSel] ; b++) // COUNTS TO MHZ
					pr_corr[e][c][b] = (double)( pr_corr[e][c][b] /( glbParam->numEventsToAvg * glbParam->nShots[c] * glbParam->tBin_us) ) ; // [MHz]

				if ( glbParam->PHO_MAX_COUNT_MHz <0 )
					printf("| Dead-Time Correction *NOT* applied (converted to MHz only due to PHO_MAX_COUNT_MHz<0) |\t" ) ;
				else
				{
					printf("| Dead-Time |  ") ;
					for (int b =0; b <glbParam->nBins_Ch[glbParam->chSel] ; b++)
						pr_corr[e][c][b] = (double)( pr_corr[e][c][b] /( 1.0 - pr_corr[e][c][b] / glbParam->PHO_MAX_COUNT_MHz ) ) ; // Non-paralyzable correction 
				}
			} /*--------------------------------------------------------------------------------------------*/

			if ( (glbParam->DAQ_Type[c] == 0) || (glbParam->DAQ_Type[c] == 1) ) // ANALOG OR PHOTON-COUNTING LIDAR SIGNALS DATA TYPES
			{
				if( glbParam->is_Ovlp_Data_Loaded ==true )
				{
					// OVERLAP CORRECTION //--------------------------------------------------------------
					printf("| Overlap |  ") ;
					for ( int i=0 ; i <glbParam->nBins_Ch[glbParam->chSel] ; i++ )
						evSig.pr[i] = (double)pr_corr[e][c][i] /ovlp[c][i] ;
				}
				else
				{
					for ( int i=0 ; i <glbParam->nBins_Ch[glbParam->chSel] ; i++ )
						evSig.pr[i] = (double)pr_corr[e][c][i] ;
				}

				// BACKGROUND & BIAS CORRECTION //--------------------------------------------------------------
				if ( strcmp(glbParam->exeFile, "./lidarAnalysis_PDL1" ) ==0 )
					oMolData->Fill_dataMol_L1( (strcGlobalParameters*)glbParam ) ;
				else if ( strcmp(glbParam->exeFile, "./lidarAnalysis_PDL2" ) ==0 )
					oMolData->Fill_dataMol_L2( (strcGlobalParameters*)glbParam ) ;

				// DARK-CURRENT AND RANGE CORRECTION
				if ( glbParam->is_Noise_Data_Loaded == true )
				{
					printf("| Dark-Current and bias |  ") ;
					MakeRangeCorrected( (strcLidarSignal*)&evSig, (strcGlobalParameters*)glbParam, (double**)data_Noise, (strcMolecularData*)&oMolData->dataMol ) ;
				}
				else // BIAS REMOVAL BASED ON VARIABLE BkgCorrMethod SET IN FILE THE SETTING FILE PASSED AS ARGUMENT TO lidarAnalysis_PDL2
				{
					printf("| Bias |  ") ;
					MakeRangeCorrected( (strcLidarSignal*)&evSig, (strcGlobalParameters*)glbParam, (strcMolecularData*)&oMolData->dataMol ) ;
				}
			} // if ( (glbParam->DAQ_Type[c] == 0) || (glbParam->DAQ_Type[c] == 1) )
			else
			{
				for ( int i =0 ; i <glbParam->nBins_Ch[glbParam->chSel] ; i++ )
				{
					evSig.pr_noBias[i] = (double) pr_corr[e][c][i]	;
					evSig.pr2[i]       = (double) pr_corr[e][c][i]	;
				}
			}

			// LOAD IN THE RETURNED VARIABLES pr_corr AND pr2 THE CORRECTED SIGNALS
			for ( int i =0 ; i <glbParam->nBins_Ch[glbParam->chSel] ; i++ )
			{
				pr_corr[e][c][i] = (double)evSig.pr_noBias[i] ;
				pr2[e][c][i] 	 = (double)evSig.pr2[i]		  ;
			}
        } // for ( int c=0 ; c <glbParam->nCh ; c++ )
    } // for ( int e=0 ; e <glbParam.nEventsAVG ; e++ )
}

void CLidar_Operations::GluingLidarSignals( strcGlobalParameters *glbParam, double ***pr_corr )
{
	double 	MHz_Max, MHz_Min, a ;
	int 	indx_MHz_Max, indx_MHz_Min ;
	int		indxs_fit[3] ;

	for ( int c =0 ; c <glbParam->nPair_Ch_to_Glue ; c++ )
	{
		// findIndxMin( (double*)&pr_corr[glbParam->evSel][glbParam->indx_gluing_High_PHO[c]][0], (glbParam->nBins-1-glbParam->nBinsBkg), (glbParam->nBins-1), (int*)&indx_MHz_Min, (double*)&MHz_Min ) ;
		findIndxMin( (double*)&pr_corr[glbParam->evSel][glbParam->indx_gluing_High_PHO[c]][0], (glbParam->nBins_Ch[glbParam->chSel]-1-glbParam->nBinsBkg), (glbParam->nBins_Ch[glbParam->chSel]-1), (int*)&indx_MHz_Min, (double*)&MHz_Min ) ;
		findIndxMax( (double*)&pr_corr[glbParam->evSel][glbParam->indx_gluing_High_PHO[c]][0], 0									 , (glbParam->nBins_Ch[glbParam->chSel]-1), (int*)&indx_MHz_Max, (double*)&MHz_Max ) ;

		if ( (MHz_Min <= glbParam->MIN_TOGGLE_RATE_MHZ) && (MHz_Max >= glbParam->MAX_TOGGLE_RATE_MHZ) )
		{
			printf("\nEvent= %d --> Gluing channels %d and %d (%d nm)", glbParam->evSel, glbParam->indx_gluing_Low_AN[c],
																		glbParam->indx_gluing_High_PHO[c], glbParam->iLambda[glbParam->indx_gluing_High_PHO[c]] ) ;
			// CORRECTED PHOTON COUNTING VALUES HIGHER THAN MAX_TOGGLE_RATE_MHZ
			// memset( dummy, 0, ( sizeof(double) * glbParam->nBins ) ) ;
			memset( dummy, 0, ( sizeof(double) * glbParam->nBins_Ch[glbParam->chSel] ) ) ;
			// for ( int b =0 ; b <glbParam->nBins ; b++ )
			for ( int b =0 ; b <glbParam->nBins_Ch[glbParam->chSel] ; b++ )
				dummy[b] = fabs(pr_corr[glbParam->evSel][glbParam->indx_gluing_High_PHO[c]][b] - glbParam->MAX_TOGGLE_RATE_MHZ) ;
			findIndxMin( (double*)&dummy[0], 0			 , indx_MHz_Max		  , (int*)&indxs_fit[0], (double*)&a ) ; // indxs_fit[0] DISCARDED
			// findIndxMin( (double*)&dummy[0], indx_MHz_Max, (glbParam->nBins-1), (int*)&indxs_fit[1], (double*)&a ) ;
			findIndxMin( (double*)&dummy[0], indx_MHz_Max, (glbParam->nBins_Ch[glbParam->chSel]-1), (int*)&indxs_fit[1], (double*)&a ) ;

			// CORRECTED PHOTON COUNTING VALUES LOWER THAN MIN_TOGGLE_RATE_MHZ
			// memset( dummy, 0, ( sizeof(double) * glbParam->nBins ) ) ;
			memset( dummy, 0, ( sizeof(double) * glbParam->nBins_Ch[glbParam->chSel] ) ) ;
			// for ( int b =0 ; b <glbParam->nBins ; b++ )
			for ( int b =0 ; b <glbParam->nBins_Ch[glbParam->chSel] ; b++ )
				dummy[b] = fabs(pr_corr[glbParam->evSel][glbParam->indx_gluing_High_PHO[c]][b] - glbParam->MIN_TOGGLE_RATE_MHZ) ;
			// findIndxMin( (double*)&dummy[0], indxs_fit[1], (glbParam->nBins-1), (int*)&indxs_fit[2], (double*)&a ) ;
			findIndxMin( (double*)&dummy[0], indxs_fit[1], (glbParam->nBins_Ch[glbParam->chSel]-1), (int*)&indxs_fit[2], (double*)&a ) ;

			memset( dummy, 0, ( sizeof(double) * glbParam->nBins ) ) ;
			fitParam.indxInicFit = indxs_fit[1] ;
			fitParam.indxEndFit  = indxs_fit[2] ;
			fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInicFit +1 ;
// Fit( (double*)&pr_corr[glbParam->evSel][glbParam->indx_gluing_High_PHO[c]][0], 
// 			 (double*)&pr_corr[glbParam->evSel][glbParam->indx_gluing_Low_AN  [c]][0], glbParam->nBins, "wB", "all", (strcFitParam*)&fitParam, (double*)dummy ) ;
Fit( (double*)&pr_corr[glbParam->evSel][glbParam->indx_gluing_High_PHO[c]][0], 
			 (double*)&pr_corr[glbParam->evSel][glbParam->indx_gluing_Low_AN  [c]][0], glbParam->nBins_Ch[glbParam->chSel], "wB", "all", (strcFitParam*)&fitParam, (double*)dummy ) ;

			for ( int b =0 ; b <indxs_fit[1] ; b++ )
				pr_corr[glbParam->evSel][glbParam->indx_gluing_Low_AN[c]][b] = (double)dummy[b] ;

			// for ( int b =indxs_fit[1] ; b <glbParam->nBins ; b++ )
			for ( int b =indxs_fit[1] ; b <glbParam->nBins_Ch[glbParam->chSel] ; b++ )
				pr_corr[glbParam->evSel][glbParam->indx_gluing_Low_AN[c]][b] = (double)pr_corr[glbParam->evSel][glbParam->indx_gluing_High_PHO[c]][b] ;
		} // if ( (MHz_Min <= glbParam->MIN_TOGGLE_RATE_MHZ) && (MHz_Max >= glbParam->MAX_TOGGLE_RATE_MHZ) )
		else
			printf("\nEvent= %d --> There is no need to glue the channels %d and %d\n", glbParam->evSel, glbParam->indx_gluing_Low_AN[c], glbParam->indx_gluing_High_PHO[c]) ;
	} // for ( int c =0 ; c <glbParam->nPair_Ch_to_Glue ; c++ )
	printf("\n") ;
}

void CLidar_Operations::Fit( double *sig, double *sigMol, int nBins, const char *modeBkg, const char *modeRangesFit, strcFitParam *fitParam, double *sigFil )
{
	fitParam->squared_sum_fit = (double)0.0  ;

	if ( strcmp( modeBkg, "wB" ) == 0 )
	{
		double *coeff = (double*) new double[2] ;
		polyfitCoeff( (const double* const) &sigMol[fitParam->indxInicFit], // X DATA
		  			  (const double* const) &sig[fitParam->indxInicFit], // Y DATA
					  (unsigned int       ) fitParam->nFit,
					  (unsigned int		  ) 1,
					  (double*			  ) coeff	 ) ;
		fitParam->m = (double)coeff[1] ;
		fitParam->b = (double)coeff[0] ;
		delete coeff ;

		if ( strcmp( modeRangesFit, "all" ) == 0 )
		{
			for ( int i=0 ; i < nBins ; i++ )
			{
				sigFil[i] = (double) ( sigMol[i] * fitParam->m + fitParam->b ) ;
				fitParam->squared_sum_fit = fitParam->squared_sum_fit + pow( (sigFil[i] - sig[i]), 2 ) ;
			}
		}
		else
		{ // "NOTall"
			for ( int i=fitParam->indxInicFit ; i <=fitParam->indxEndFit ; i++ )
			{
				sigFil[i] = (double) ( sigMol[i] * fitParam->m + fitParam->b ) ;
				fitParam->squared_sum_fit = fitParam->squared_sum_fit + pow( (sigFil[i] - sig[i]), 2 ) ;
			}
		}
	}
	else if ( strcmp( modeBkg, "wOutB" ) == 0 )
	{
		double	mNum=0, mDen=0 ;
		for ( int i=fitParam->indxInicFit ; i <= fitParam->indxEndFit ; i++ )
		{
			mNum = mNum + sig   [i]*sigMol[i] ;
			mDen = mDen + sigMol[i]*sigMol[i] ;
		}
		fitParam->m = mNum/mDen ;
		fitParam->b = 0     	;

		if ( strcmp( modeRangesFit, "all" ) == 0 )
		{
			for ( int i=0 ; i < nBins ; i++ )
			{
				sigFil[i] = (double) ( sigMol[i] * fitParam->m ) ;
				fitParam->squared_sum_fit = fitParam->squared_sum_fit + pow( (sigFil[i] - sig[i]), 2 ) ;

			}
		}
		else
		{
			for ( int i=fitParam->indxInicFit ; i <=fitParam->indxEndFit ; i++ )
			{
				sigFil[i] = (double) ( sigMol[i] * fitParam->m ) ;
				fitParam->squared_sum_fit = fitParam->squared_sum_fit + pow( (sigFil[i] - sig[i]), 2 ) ;
			}
		}
	}

	fitParam->var = (double)fitParam->squared_sum_fit / (fitParam->nFit -1) ;
	fitParam->std = (double)sqrt(fitParam->var)  						;

	fitParam->mean_sig = (double) 0.0 ;
	
	fitParam->s 		= (double) 0.0 ;
	fitParam->s			= (double) sum( (double*)sig, (int)fitParam->indxInicFit, (int)fitParam->indxEndFit, (double*)&fitParam->s ) ;
	fitParam->mean_sig	= (double) fitParam->s /fitParam->nFit ;
	fitParam->squared_sum_fit_vs_Mean = (double)0.0 ;
	for ( int b=fitParam->indxInicFit ;  b<=fitParam->indxEndFit ; b++ )
		fitParam->squared_sum_fit_vs_Mean  = fitParam->squared_sum_fit_vs_Mean + pow( (sig[b] - fitParam->mean_sig), 2) ;
	
	fitParam->R2  = (double) ( fitParam->squared_sum_fit_vs_Mean - fitParam->squared_sum_fit) / fitParam->squared_sum_fit_vs_Mean ;

} // void CLidar_Operations::Fit( double *sig, double *sigMol, int nBins, const char *modeBkg, const char *modeRangesFit, strcFitParam *fitParam, double *sigFil )

void CLidar_Operations::TransmissionMethod_pr( double *pr, strcGlobalParameters *glbParam, strcMolecularData *dataMol, int indxBefCloud, int indxAftCloud, double *VOD )
{
	double *prFit = (double*) new double [ glbParam->nBins ] ;

	int DELTA_RANGE_LIM_BINS ;
	ReadAnalisysParameter( (const char*) glbParam->FILE_PARAMETERS, "DELTA_RANGE_LIM_BINS", "int", (int*)&DELTA_RANGE_LIM_BINS ) ;

	strcFitParam fitParam_before_cloud ;
	strcFitParam fitParam_after_cloud  ;
	double prBeforeCloud, prAfterCloud ;
	int indxA, indxB ;
	do
	{
		fitParam_before_cloud.indxInicFit = indxBefCloud - (int)round(DELTA_RANGE_LIM_BINS) ;
		fitParam_before_cloud.indxEndFit  = indxBefCloud - (int)round(DELTA_RANGE_LIM_BINS/5) ;
		fitParam_before_cloud.nFit = fitParam_before_cloud.indxEndFit - fitParam_before_cloud.indxInicFit +1;
			Fit( (double*)pr, (double*)dataMol->prMol, glbParam->nBins, "wB", "NOTall", (strcFitParam*)&fitParam_before_cloud, (double*)prFit ) ;
			indxB = indxBefCloud -(int)round(DELTA_RANGE_LIM_BINS/2) ;
			prBeforeCloud = prFit[indxB] ;
			// printf("\n prFit[indxB]= %lf \t fitParam_before_cloud.m= %lf \n", prFit[indxB], fitParam_before_cloud.m ) ;
			// printf(" pr[%d]= %lf \n", indxB, pr[indxB] ) ;
		fitParam_after_cloud.indxInicFit = indxAftCloud + (int)round(DELTA_RANGE_LIM_BINS/5) ;
		fitParam_after_cloud.indxEndFit  = indxAftCloud + (int)round(DELTA_RANGE_LIM_BINS) ;
		fitParam_after_cloud.nFit = fitParam_after_cloud.indxEndFit - fitParam_after_cloud.indxInicFit +1;
			Fit( (double*)pr, (double*)dataMol->prMol, glbParam->nBins, "wB", "NOTall", (strcFitParam*)&fitParam_after_cloud, (double*)prFit ) ;
			indxA = indxAftCloud +(int)round(DELTA_RANGE_LIM_BINS/2) ;
			prAfterCloud = prFit[indxA] ;
			// printf(" prFit[indxA]= %lf \t fitParam_after_cloud.m= %lf \n", prFit[indxA], fitParam_after_cloud.m ) ;
			// printf(" pr[%d]= %lf \n", indxA, pr[indxA] ) ;
		// DELTA_RANGE_LIM_BINS = DELTA_RANGE_LIM_BINS -3 ;
		indxBefCloud = indxBefCloud -1 ;
		indxAftCloud = indxAftCloud +1 ;
	} while( (prBeforeCloud < prAfterCloud) ) ;

	double rPr 		= prAfterCloud / prBeforeCloud ;
	double rPrMol   = dataMol->prMol[indxA] / dataMol->prMol[indxB] ;
	double rR2 		= pow(glbParam->r[indxA], 2) / pow(glbParam->r[indxB], 2) ;
	double Tp2Cloud	= rPr / (rPrMol * rR2) ;
	// printf("\t\t rPr: %lf - prAfterCloud[%d]: %lf - prBeforeCloud[%d]: %lf \n", rPr, indxA, prAfterCloud, indxB, prBeforeCloud ) ;
	// printf("\t\t %d-%d --- rPr: %lf - rPrMol: %lf - Tp2Cloud: %lf \n", indxBefCloud, indxAftCloud, rPr, rPrMol, Tp2Cloud) ;
	*VOD = -0.5*log(Tp2Cloud) *cos(dataMol->zenith*PI/180) ;
	delete prFit ;
}

void CLidar_Operations::ODcut( double *prS, strcMolecularData *dataMol, strcGlobalParameters *glbParam, strcFitParam *fitParam, int *clouds_ON )
{
	double OD =0 ; // CLOUD TRANSMISSION
	// double *pr2	   = (double*) new double [ glbParam->nBins ] ;

	// 	MakeRangeCorrected_array( (double*)prS, (strcGlobalParameters*)glbParam, (strcMolecularData*)dataMol, (double*)pr2 ) ;

	int indxBefCloud, indxAftCloud ;
	indxBefCloud = fitParam->indxInicFit ;
	indxAftCloud = fitParam->indxEndFit ;
		TransmissionMethod_pr( (double*)prS, (strcGlobalParameters*)glbParam, (strcMolecularData*)dataMol, (int)indxBefCloud, (int)indxAftCloud, (double*)&OD ) ;

	double OD_cut ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "OD_cut", "double", (double*)&OD_cut ) ;

	if( (OD >=0) && (OD <= OD_cut) )
	{
		printf("\n<%d> filtering cloud due to OD (%lf)...\n", glbParam->evSel, OD) ;
		for (int i =fitParam->indxEndFit; i <=fitParam->indxEndFit ; i++)
			clouds_ON[i] =0;
	}
	// else
	// 	printf("\nOptical Depth of the cloud: %lf \t Altitude (asl): %lf - %lf\n", OD, glbParam->siteASL + fitParam->indxInicFit * dataMol->dzr, glbParam->siteASL + fitParam->indxEndFit * dataMol->dzr ) ;

	// delete pr2 ;
}

// void CLidar_Operations::Bias_Residual_Correction( const double *pr, strcGlobalParameters *glbParam, strcMolecularData *dataMol, double *pr_res_corr )
// {
// 	fitParam.indxInicFit = 300 ;
// 	fitParam.indxEndFit  = 900 ;
// 	fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInicFit +1 ;
// 	Fit( (double*)&pr[0], (double*)&dataMol->prMol[0], glbParam->nBins, "wB", "all", (strcFitParam*)&fitParam, (double*)dummy ) ;

// 	for (int i =0; i <glbParam->nBins; i++)
// 		pr_res_corr[i] = pr[i] - dummy[i] ;
	
// 	double *coeff = (double*) new double[2] ;
// 	polyfitCoeff( (const double* const) &sigMol[fitParam->indxInicFit], // X DATA
// 				  (const double* const) &sig[fitParam->indxInicFit], // Y DATA
// 				  (unsigned int       ) fitParam->nFit,
// 				  (unsigned int		  ) 1,
// 				  (double*			  ) coeff	 ) ;
// 	fitParam.m = (double)coeff[1] ;
// 	fitParam.b = (double)coeff[0] ;
// 	delete coeff ;
// }
