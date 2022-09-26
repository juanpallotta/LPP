
#include "CLidar_Operations.hpp"

CLidar_Operations::CLidar_Operations( strcGlobalParameters *glbParam )
{
	dummy 	 	= (double*) new double[glbParam->nBins] ; 
    pr_NObkg_i	= (double*) new double[glbParam->nBins] ; 
	pr2_i 		= (double*) new double[glbParam->nBins] ;	
	errRMS_mol	= (double*) new double[glbParam->nBins] ;	
	errRMS_k 	= (double*) new double[glbParam->nBins] ;	
	rate 		= (double*) new double[glbParam->nBins] ;	
	k_ones 		= (double*) new double[glbParam->nBins] ;

	ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "nBiasRes_Auto", "int" , (int*)&nBiasRes_Auto) ;

	errRMS_Bias = (double*) new double [ nBiasRes_Auto ] ;
	b_i 		= (double*) new double [ nBiasRes_Auto ] ;
	coeff 		= (double*) new double[2] 				;
}

CLidar_Operations::~CLidar_Operations()
{
	delete dummy 	;
}

// MakeRangeCorrected() WITHOUT BACKGROUND NOISE FILE, USING 'MEAN', 'FIT', 'NO_BKG' OR 'AUTO' METHODS TO REMOVE BIAS.
void CLidar_Operations::MakeRangeCorrected( strcLidarSignal *evSig, strcGlobalParameters *glbParam, strcMolecularData *dataMol )
{
	BiasCorrection( (strcLidarSignal*)evSig, (strcGlobalParameters*)glbParam, (strcMolecularData*)dataMol ) ;
	for ( int i=0 ; i<glbParam->nBins ; i++ ) 	evSig->pr2[i] = evSig->pr_noBias[i] * pow(glbParam->r[i], 2) ;
}

// IF data_noise IS PASSED AS ARGUMENT, IT IS USED
void CLidar_Operations::MakeRangeCorrected( strcLidarSignal *evSig, strcGlobalParameters *glbParam, double **data_Noise, strcMolecularData *dataMol )
{
	BiasCorrection( (strcLidarSignal*)evSig, (strcGlobalParameters*)glbParam, (double**)data_Noise, (strcMolecularData*)dataMol ) ;
	for ( int i=0 ; i<glbParam->nBins ; i++ ) 	evSig->pr2[i] = evSig->pr_noBias[i] * pow(glbParam->r[i], 2) ;
}

void CLidar_Operations::Bias_Substraction_Auto( double *pr, strcMolecularData *dataMol, strcGlobalParameters *glbParam, double *pr_noBias, double *Bias_Pr )
{
	int 	indxMaxRange ;
	Find_Max_Range( (double*)pr, (double*)dataMol->prMol, (strcGlobalParameters*)glbParam, (int*)&indxMaxRange ) ;
	// printf("\n CLidar_Operations::Bias_Substraction_Auto() ---> After Find_Max_Range() --> indxMaxRange= %d \n", indxMaxRange) ;

	if ( indxMaxRange <= (glbParam->nBins-1) )
	{
		double 	b_ref_max = 0.0 ;
		double 	b_ref_min = 0.0 ;

	// FIRST BIAS OBTAINED BY APPLYING A MOLECULAR FIT USING nBinsBkg POINTS FROM THE INDEX indxMaxRange
		fitParam.indxEndFit  = indxMaxRange ; 
		fitParam.indxInicFit = fitParam.indxEndFit - glbParam->nBinsBkg ;
		fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInicFit +1;
		RayleighFit( (double*)pr, (double*)dataMol->prMol, dataMol->nBins , "wB", "NOTall", (strcFitParam*)&fitParam, (double*)dummy ) ;

		b_ref_max = 1.5* fitParam.b ;
		b_ref_min = 0.5* fitParam.b ;
		double b_step 	= (b_ref_max - b_ref_min) /nBiasRes_Auto ;

		for ( int s =0; s <nBiasRes_Auto; s++ )
			b_i[s] = (double) b_ref_min + s *b_step ;

		for( int l=0 ; l< nLoopFindBias ; l++ )
		{
			for (int b =0; b <nBiasRes_Auto; b++)
			{
				for ( int i =0 ; i <glbParam->nBins ; i++ )
				{
					pr_NObkg_i[i]  = pr[i] - b_i[b] ;
					pr2_i[i]       = pr_NObkg_i[i] * pow(glbParam->r[i], 2) ;
				}
				RayleighFit( (double*)pr2_i, (double*)dataMol->pr2Mol, glbParam->nBins , "wOutB", "NOTall", (strcFitParam*)&fitParam, (double*)dummy ) ;
				errRMS_Bias[b] = fitParam.sumsq_m ;
			}
				polyfitCoeff( (const double* const) b_i			, // X DATA
							(const double* const  ) errRMS_Bias	, // Y DATA
							(unsigned int         ) nBiasRes_Auto 	,
							(unsigned int		  ) 2			,
							(double*			  ) coeff	 ) 	;
				*Bias_Pr = (double) -coeff[1] /(2*coeff[2]) ;

				b_ref_max = 1.5* (*Bias_Pr) ;
				b_ref_min = 0.5* (*Bias_Pr) ;
				b_step 	= (b_ref_max - b_ref_min) /nBiasRes_Auto ;
				for ( int b =0; b <nBiasRes_Auto; b++ )
					b_i[b] = (double) b_ref_min + b *b_step ;
		}
			for ( int i =0; i <glbParam->nBins ; i++)
				pr_noBias[i] = (pr[i] - *Bias_Pr);
	}
	else
	{
		printf("\n(ev= %d)Background substraction metod set is %s, but MEAN method is applied (nBins= %d \t indxMaxRange= %d).\n", glbParam->evSel, glbParam->BkgCorrMethod, glbParam->nBins, indxMaxRange ) ;
		Bias_Substraction_Mean( (double*)pr, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (double*)pr_noBias) ;
	}
}

void CLidar_Operations::Find_Max_Range( double *pr, double *prMol, strcGlobalParameters *glbParam, int *indxMaxRange )
{
	int i=0, indxMax_, indxMin_Pr ;
	double min ;
	smooth( (double*)pr, (int)0, (int)(glbParam->nBins-1), (int)15, (double*)dummy ) ;
	findIndxMin( (double*)pr, (int)glbParam->indxInitSig, (int)(glbParam->nBins-1), (int*)&indxMin_Pr, (double*)&min ) ;

	fitParam.indxEndFit  = indxMin_Pr ; // glbParam->nBins -1 ; // 
    fitParam.indxInicFit = fitParam.indxEndFit - glbParam->nBinsBkg ;
	fitParam.nFit		 = fitParam.indxEndFit - fitParam.indxInicFit +1 ;

	for (int i =0; i <glbParam->nBins; i++)
	{
		k_ones[i] 		= (double)1.0 		;
		errRMS_mol[i] 	= (double)DBL_MAX 	; 
		errRMS_k[i] 	= (double)0.0 		;
		rate[i] 		= (double)0.0 		;
	}

	do
	{
		RayleighFit( (double*)pr, (double*)prMol, glbParam->nBins , "wB", "NOTall", (strcFitParam*)&fitParam, (double*)dummy ) ;
		errRMS_mol[fitParam.indxEndFit] = (double)fitParam.sumsq_m ;

		RayleighFit( (double*)pr, (double*)k_ones, glbParam->nBins , "wOutB", "NOTall", (strcFitParam*)&fitParam, (double*)dummy ) ;
		errRMS_k[fitParam.indxEndFit] = (double)fitParam.sumsq_m ;

		i = i+1 ;
		fitParam.indxEndFit  = glbParam->nBins - i            				 ;
		fitParam.indxInicFit = fitParam.indxEndFit - glbParam->nBinsBkg 	 ;
		fitParam.nFit		 = fitParam.indxEndFit - fitParam.indxInicFit +1 ;
	} while( fitParam.indxInicFit >glbParam->indxInitSig ) ; //! CHANGE TO THE PEAK OF THE LIDAR SIGNAL

	for ( i =glbParam->indxInitSig; i <indxMin_Pr; i++ )
	{
		rate[i] = (double)(errRMS_k[i] / errRMS_mol[i]) ;
	}

	for (i =indxMin_Pr; i>=0; i--)
	{
		if ( rate[i] >1.02 )
		{
			indxMax_ = i ;
			break;
		}
	}
	*indxMaxRange = indxMax_ ;
	findIndxMin( (double*)errRMS_mol, (int)0, (int)indxMax_, (int*)indxMaxRange, (double*)&min ) ;

    int indxWL_PDLx ;
	if ( strcmp(glbParam->exeFile, "./lidarAnalysis_PDL1" ) ==0 )
	    ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"indxWL_PDL1", (const char*)"int", (int*)&indxWL_PDLx ) ;
	else if ( strcmp(glbParam->exeFile, "./lidarAnalysis_PDL2" ) ==0 )
	    ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"indxWL_PDL2", (const char*)"int", (int*)&indxWL_PDLx ) ;

	if ( glbParam->chSel== indxWL_PDLx )
	{
		glbParam->indxEndSig_ev[glbParam->evSel] = *indxMaxRange ;
		glbParam->rEndSig_ev[glbParam->evSel] 	 = glbParam->indxEndSig_ev[glbParam->evSel] * glbParam->dr ;
	}
	// printf( "\n ev: %d \t glbParam->indxInitSig= %d \n indxMin_Pr= %d \n Max. Range: %lf \n indxMaxRange= %d", glbParam->evSel, glbParam->indxInitSig, indxMin_Pr, glbParam->rEndSig_ev[glbParam->evSel], *indxMaxRange ) ;
	// printf( "\n CLidar_Operations::Find_Max_Range() --> ev: %d \t indxMaxRange= %d", glbParam->evSel, *indxMaxRange ) ;
}

// BIAS SUBSTRACTION METHODS
void CLidar_Operations::Bias_Substraction_Mean( double *sig, strcMolecularData *dataMol, strcGlobalParameters *glbParam, double *pr_noBkg)
{
	int 	indxMaxRange ;
	Find_Max_Range( (double*)sig, (double*)dataMol->prMol, (strcGlobalParameters*)glbParam, (int*)&indxMaxRange ) ;

	if ( indxMaxRange<(glbParam->nBins-1) )
	{
		fitParam.indxInicFit = (int)round( (glbParam->nBins-1 + indxMaxRange)/2 ) ;
		fitParam.indxEndFit  = glbParam->nBins -1 ;
		fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInicFit +1;
	}
	else
	{
		fitParam.indxInicFit = glbParam->nBins -1 - glbParam->nBinsBkg ;
		fitParam.indxEndFit  = glbParam->nBins -1 ;
		fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInicFit +1;
	}
	double 	bkgMean = 0 ;
	for( int j=fitParam.indxInicFit ; j<fitParam.indxEndFit ; j++ ) bkgMean = bkgMean + sig[j] ;

	bkgMean = bkgMean /fitParam.nFit ;
	for ( int i=0 ; i<glbParam->nBins ; i++ ) 	pr_noBkg[i] = (double)(sig[i] - bkgMean) ;
}

void CLidar_Operations::Bias_Substraction_MolFit(strcMolecularData *dataMol, const double *prEl, strcGlobalParameters *glbParam, double *pr_noBkg)
{
	int 	indxMaxRange ;
	Find_Max_Range( (double*)prEl, (double*)dataMol->prMol, (strcGlobalParameters*)glbParam, (int*)&indxMaxRange ) ;

	if ( indxMaxRange < (glbParam->nBins-1) )
	{
		fitParam.indxEndFit  = indxMaxRange ;
		fitParam.indxInicFit = fitParam.indxEndFit - glbParam->nBinsBkg ;
		fitParam.nFit	   	 = fitParam.indxEndFit - fitParam.indxInicFit +1;

		RayleighFit( (double*)prEl, (double*)dataMol->prMol, dataMol->nBins , "wB", "NOTall", (strcFitParam*)&fitParam, (double*)dummy ) ;
			for ( int i=0 ; i<dataMol->nBins ; i++ ) 	pr_noBkg[i] = (double)(prEl[i] - fitParam.b) ; 
	}
	else
	{
		printf("\n(ev= %d)Background substraction metod set is %s, but MEAN method is applied (nBins= %d \t indxMaxRange= %d).\n", glbParam->evSel, glbParam->BkgCorrMethod, glbParam->nBins, indxMaxRange ) ;
		Bias_Substraction_Mean( (double*)prEl, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (double*)pr_noBkg) ;
	}
}

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
			for ( int b=0 ; b <glbParam->nBins ; b++ )
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

						glbParam->aAzimuthAVG[fC] 	  = glbParam->aAzimuthAVG[fC] + glbParam->aAzimuth[fC*glbParam->numEventsToAvg +t] ;
						glbParam->aZenithAVG[fC]  	  = glbParam->aZenithAVG[fC]  + glbParam->aZenith [fC*glbParam->numEventsToAvg +t]  ;
						glbParam->temp_CelsiusAVG[fC] = glbParam->temp_CelsiusAVG[fC] + glbParam->temp_Celsius[fC*glbParam->numEventsToAvg +t] ;
						glbParam->pres_hPaAVG[fC]     = glbParam->pres_hPaAVG[fC]     + glbParam->pres_hPa[fC*glbParam->numEventsToAvg +t] 	  ;
					}
				}
					dataFile_AVG[fC][c][b]      = (double)(dataFile_AVG[fC][c][b] /glbParam->numEventsToAvg) ;
					if( (b==0) && (c==0) )
					{
						glbParam->aAzimuthAVG[fC] 	  = glbParam->aAzimuthAVG[fC] /glbParam->numEventsToAvg ;
						glbParam->aZenithAVG[fC]  	  = glbParam->aZenithAVG[fC]  /glbParam->numEventsToAvg ;
						glbParam->temp_CelsiusAVG[fC] = glbParam->temp_CelsiusAVG[fC] /glbParam->numEventsToAvg ;
						glbParam->pres_hPaAVG[fC]     = glbParam->pres_hPaAVG[fC]     /glbParam->numEventsToAvg ;
					}
			}
		}
	}
}

void CLidar_Operations::BiasCorrection( strcLidarSignal *evSig, strcGlobalParameters *glbParam, strcMolecularData *dataMol )
{
	ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "BkgCorrMethod", "string" , (char*)glbParam->BkgCorrMethod ) ;

	if ( (strcmp( glbParam->BkgCorrMethod, "NO_BKG" ) ==0) || (strcmp( glbParam->BkgCorrMethod, "no_bkg" ) ==0) )
	{
		for (int i =0; i <glbParam->nBins ; i++)
			evSig->pr_noBias[i] = evSig->pr[i] ;
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
		printf( "\n BkgCorrMethod = %s but there is no noise information... extracting bias using the mean of the last %d bins.' \n", glbParam->BkgCorrMethod, glbParam->nBinsBkg ) ;
		Bias_Substraction_Mean( (double*)evSig->pr, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (double*)evSig->pr_noBias ) ;
	}
}

// IF data_noise IS PASSED AS ARGUMENT, IT IS USED
void CLidar_Operations::BiasCorrection( strcLidarSignal *evSig, strcGlobalParameters *glbParam, double **data_Noise, strcMolecularData *dataMol )
{
	for (int i =0; i <glbParam->nBins; i++)
		evSig->pr_noBkg[i] = (double)( evSig->pr[i] - data_Noise[glbParam->chSel][i] ) ;

	ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "BkgCorrMethod", "string" , (char*)glbParam->BkgCorrMethod ) ;

	if ( (strcmp( glbParam->BkgCorrMethod, "FIT" ) ==0) || (strcmp( glbParam->BkgCorrMethod, "fit" ) ==0) )
	{
		Bias_Substraction_MolFit( (strcMolecularData*)dataMol, (const double*)evSig->pr_noBkg, (strcGlobalParameters*)glbParam, (double*)evSig->pr_noBias ) ;
	}
	else if ( (strcmp( glbParam->BkgCorrMethod, "MEAN" ) ==0) || (strcmp( glbParam->BkgCorrMethod, "mean" ) ==0) )
	{
		Bias_Substraction_Mean( (double*)evSig->pr_noBkg, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (double*)evSig->pr_noBias ) ;
	}
	else if  ( (strcmp( glbParam->BkgCorrMethod, "AUTO" ) ==0) || (strcmp( glbParam->BkgCorrMethod, "mean" ) ==0) )
	{
		double 	Bias_Pr ;
		Bias_Substraction_Auto( (double*)evSig->pr_noBkg, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (double*)evSig->pr_noBias, (double*)&Bias_Pr ) ;
	}
}



// void CLidar_Operations::Bias_Substraction_Auto( double *pr, strcMolecularData *dataMol, strcGlobalParameters *glbParam, int nBiasRes_Auto, double *pr_noBkg, double *Bias_Pr )
// {
// 	int 	indxMaxRange ;
// 	Find_Max_Range( (double*)pr, (double*)dataMol->prMol, (strcGlobalParameters*)glbParam, (int*)&indxMaxRange ) ;
// 	// printf("\n CLidar_Operations::nBiasRes_Auto() ---> After Find_Max_Range() --> indxMaxRange= %d \n", indxMaxRange) ;

// 	if ( indxMaxRange <= (glbParam->nBins-1) )
// 	{
// 		double 	b_ref_max = 0.0 ;
// 		double 	b_ref_min = 0.0 ;

// 	// BIAS OBTAINED BY APPLYING A LINEAR FIT FROM rEndSig SET IN THE CONFIGURATION FILE PASSED AS ARGUMENT
// 		fitParam.indxEndFit  = indxMaxRange ; // glbParam->indxEndSig ; // 
// 		fitParam.indxInicFit = fitParam.indxEndFit - glbParam->nBinsBkg ;
// 		fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInicFit +1;
// 		RayleighFit( (double*)pr, (double*)dataMol->prMol, dataMol->nBins , "wB", "NOTall", (strcFitParam*)&fitParam, (double*)dummy ) ;
// 		b_ref_max = 1.5* fitParam.b ;
// 		b_ref_min = 0.5* fitParam.b ;

// 		double b_step 		= (b_ref_max - b_ref_min) /nBiasRes_Auto ;
// 		double *b_i   		= (double*) new double [ nBiasRes_Auto +2 ] ;
// 		errRMS_Bias = (double*) new double [ nBiasRes_Auto +2 ] ;
// 		b_i[0] = (double)0.0 ; // IN CASE OF NO BACKGROUND NEEDED TO BE SUBSTRACTED.
// 		b_i[1] = (double)0.0 ; // MEAN VALUE OF THE glbParam->nBinsBkg BINS TAKEN FROM THE TAIL OF THE LIDAR SIGNAL.
// 		for( int j=(glbParam->nBins-glbParam->nBinsBkg) ; j<glbParam->nBins ; j++ ) b_i[1] = (double)(b_i[1] + pr[j]) ;
// 			b_i[1] = (double)(b_i[1] /glbParam->nBinsBkg) ;
// 		// PRODUCE THE ARRAY OF BIAS TO TEST b_i[.]
// 		for ( int s =2; s <(nBiasRes_Auto+2); s++ )
// 			b_i[s] = (double) b_ref_min + (s-2) *b_step ;

// 		for (int s =0; s <(nBiasRes_Auto +2); s++)
// 		{
// 			for ( int i =0 ; i <glbParam->nBins ; i++ )
// 			{
// 				pr_NObkg_i[i]  = pr[i] - b_i[s] ;
// 				pr2_i[i]       = pr_NObkg_i[i] * pow(glbParam->r[i], 2) ;
// 			}
// 			RayleighFit( (double*)pr2_i, (double*)dataMol->pr2Mol, glbParam->nBins , "wOutB", "NOTall", (strcFitParam*)&fitParam, (double*)dummy ) ;
// 			errRMS_Bias[s] = fitParam.sumsq_m ;
// 		}

// 		int 	indxMinErr ;
// 		double	minErr ;
// 		findIndxMin( errRMS_Bias, 0, (nBiasRes_Auto +2-1), &indxMinErr, &minErr ) ;
// 		*Bias_Pr = (double) b_i[indxMinErr] ;

// 		for ( int i =0; i <glbParam->nBins ; i++)
// 		pr_noBkg[i] = (pr[i] - *Bias_Pr);

// 		delete b_i			;
// 	}
// 	else
// 	{
// 		printf("\n(ev= %d)Background substraction metod set is %s, but MEAN method is applied (nBins= %d \t indxMaxRange= %d).\n", glbParam->evSel, glbParam->BkgCorrMethod, glbParam->nBins, indxMaxRange ) ;
// 		Bias_Substraction_Mean( (double*)pr, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (double*)pr_noBkg) ;
// 	}
// }