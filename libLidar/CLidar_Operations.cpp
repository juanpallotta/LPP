
#include "CLidar_Operations.hpp"

CLidar_Operations::CLidar_Operations( strcGlobalParameters *glbParam )
{
	dummy 	 	= (double*) new double[glbParam->nBins] ; 
	dummy1 	 	= (double*) new double[glbParam->nBins] ; 
    pr_NObkg_i	= (double*) new double[glbParam->nBins] ; 
	pr2_i 		= (double*) new double[glbParam->nBins] ;	
	// errRMS_mol	= (double*) new double[glbParam->nBins] ;	
	// errRMS_k 	= (double*) new double[glbParam->nBins] ;	
	// rate 		= (double*) new double[glbParam->nBins] ;	
	// k_ones 		= (double*) new double[glbParam->nBins] ;
	// R2_array	= (double*) new double[glbParam->nBins] ;

	errRMS_Bias = (double*) new double [ nBiasRes_Auto ] ;
	b_i 		= (double*) new double [ nBiasRes_Auto +1 ] ;
	coeff 		= (double*) new double[2] 				;
}

CLidar_Operations::~CLidar_Operations()
{
	delete dummy 	;
}

void CLidar_Operations::BiasCorrection( strcLidarSignal *evSig, strcGlobalParameters *glbParam, strcMolecularData *dataMol )
{
	ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "BkgCorrMethod", "string" , (char*)glbParam->BkgCorrMethod ) ;

	if ( (strcmp( glbParam->BkgCorrMethod, "NO_BKG" ) ==0) || (strcmp( glbParam->BkgCorrMethod, "no_bkg" ) ==0) )
	{
		for (int i =0; i <glbParam->nBins ; i++)
		{
			evSig->pr_noBias[i] 	= evSig->pr[i] ;
			evSig->pr_no_DarkCur[i] = evSig->pr[i] ;
		}
		Find_Max_Range( (double*)evSig->pr_noBias, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam) ;
	}
	else if ( (strcmp( glbParam->BkgCorrMethod, "MEAN" ) ==0) || (strcmp( glbParam->BkgCorrMethod, "mean" ) ==0) )
	{
		if( glbParam->is_Noise_Data_Loaded ==true )
			Bias_Substraction_Mean( (double*)evSig->pr_no_DarkCur, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (double*)evSig->pr_noBias ) ;
		else
			Bias_Substraction_Mean( (double*)evSig->pr		     , (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (double*)evSig->pr_noBias ) ;
	}
	else if ( (strcmp( glbParam->BkgCorrMethod, "FIT" ) ==0) || (strcmp( glbParam->BkgCorrMethod, "fit" ) ==0) )
	{
		if( glbParam->is_Noise_Data_Loaded ==true )
			Bias_Substraction_MolFit( (strcMolecularData*)dataMol, (const double*)evSig->pr_no_DarkCur, (strcGlobalParameters*)glbParam, (double*)evSig->pr_noBias ) ;
		else
			Bias_Substraction_MolFit( (strcMolecularData*)dataMol, (const double*)evSig->pr		 	  , (strcGlobalParameters*)glbParam, (double*)evSig->pr_noBias ) ;
	}
	else if ( (strcmp( glbParam->BkgCorrMethod, "AUTO" ) ==0) || (strcmp( glbParam->BkgCorrMethod, "auto" ) ==0) )
	{
		double 	Bias_Pr ;
		if( glbParam->is_Noise_Data_Loaded ==true )
				Bias_Substraction_Auto( (double*)evSig->pr_no_DarkCur, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (double*)evSig->pr_noBias, (double*)&Bias_Pr ) ;
		else
				Bias_Substraction_Auto( (double*)evSig->pr      	 , (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (double*)evSig->pr_noBias, (double*)&Bias_Pr ) ;
	}
	else if ( (strcmp( glbParam->BkgCorrMethod, "PRE_TRIGGER" ) ==0) || (strcmp( glbParam->BkgCorrMethod, "pre_trigger" ) ==0) )
	{
		if( glbParam->is_Noise_Data_Loaded ==true )
				Bias_Substraction_Pre_Trigger( (double*)evSig->pr_no_DarkCur, (strcGlobalParameters*)glbParam, (double*)evSig->pr_noBias ) ;
		else
				Bias_Substraction_Pre_Trigger( (double*)evSig->pr      	    , (strcGlobalParameters*)glbParam, (double*)evSig->pr_noBias ) ;
	}
	else
	{
		printf( "\n CLidar_Operations::BiasCorrection(): BkgCorrMethod = %s but there is no noise information... extracting bias using the mean of the last %d bins.' \n", glbParam->BkgCorrMethod, glbParam->nBinsBkg ) ;
		Bias_Substraction_Mean( (double*)evSig->pr, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (double*)evSig->pr_noBias ) ;
	}
}

void CLidar_Operations::Bias_Substraction_Auto( double *pr, strcMolecularData *dataMol, strcGlobalParameters *glbParam, double *pr_noBias, double *Bias_Pr )
{
	Find_Max_Range( (double*)pr, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam) ;

printf( "\nBias_Substraction_Auto() ====> \n dataMol->last_Indx_Mol_Low[m]= %lf\n dataMol->last_Indx_Mol_High[m]= %lf\n dataMol->last_Indx_Bkg_Low[m]= %lf\n dataMol->last_Indx_Bkg_High[m]= %lf\n", 
							dataMol->last_Indx_Mol_Low *glbParam->dzr, dataMol->last_Indx_Mol_High *glbParam->dzr, dataMol->last_Indx_Bkg_Low *glbParam->dzr, dataMol->last_Indx_Bkg_High *glbParam->dzr ) ;

	double 	b_ref_max = 0.0 ;
	double 	b_ref_min = 0.0 ;
	double 	b_step    = 0.0 ;

	if 	( 	(dataMol->last_Indx_Bkg_Low >0) && (dataMol->last_Indx_Bkg_High >0) &&
			(dataMol->last_Indx_Mol_Low >0) && (dataMol->last_Indx_Mol_High >0)
		)
	{	// FIRST BIAS GUESS OBTAINED FROM THE MEAN VALUE OF THE LAST BINS
		fitParam.indxInitFit = dataMol->last_Indx_Mol_Low  ;
		fitParam.indxEndFit  = dataMol->last_Indx_Mol_High ;
		fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1 ;

		*Bias_Pr = 0.0 ;
		for( int j=dataMol->last_Indx_Bkg_Low ; j<=dataMol->last_Indx_Bkg_High ; j++ ) 
			*Bias_Pr = *Bias_Pr + pr[j] ;
		*Bias_Pr = *Bias_Pr /( dataMol->last_Indx_Bkg_High - dataMol->last_Indx_Bkg_Low +1) ;

		b_ref_max = 1.5* *Bias_Pr ;
		b_ref_min = 0.5* *Bias_Pr ;
		b_step	  = (b_ref_max - b_ref_min) /nBiasRes_Auto ;

		for ( int s =0; s <nBiasRes_Auto; s++ )
			b_i[s] = (double) b_ref_min + s *b_step ;
		b_i[nBiasRes_Auto] = 0 ; // IN CASE OF UNBIASED LIDAR SIGNAL

		for( int l=0 ; l< nLoopFindBias ; l++ )
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
		} // for( int l=0 ; l< nLoopFindBias ; l++ )
	} // if 	( 	(dataMol->last_Indx_Bkg_Low >0) && (dataMol->last_Indx_Bkg_High >0) &&
	else
	{	// LIDAR SIGNAL CORRUPTED, ALL DATA FILLED WITH ZEROS
		*Bias_Pr = 0.0 	;
		printf("\n CLidar_Operations::Bias_Substraction_Auto() - NO BIAS IS SUBSTRACTED, POSSIBLE DAMAGED SIGNAL.\n") ;
	}

	for ( int i =0; i <glbParam->nBins ; i++)
		pr_noBias[i] = (pr[i] - *Bias_Pr);

}

// void CLidar_Operations::Bias_Substraction_Auto( double *pr, strcMolecularData *dataMol, strcGlobalParameters *glbParam, double *pr_noBias, double *Bias_Pr )
// {
// 	Find_Max_Range( (double*)pr, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam) ;

// 	// printf( "\nBias_Substraction_Auto() ====> \n dataMol->last_Indx_Mol_Low[m]= %lf\n dataMol->last_Indx_Mol_High[m]= %lf\n dataMol->last_Indx_Bkg_Low[m]= %lf\n dataMol->last_Indx_Bkg_High[m]= %lf\n", 
// 	// 							dataMol->last_Indx_Mol_Low *glbParam->dzr, dataMol->last_Indx_Mol_High *glbParam->dzr, dataMol->last_Indx_Bkg_Low *glbParam->dzr, dataMol->last_Indx_Bkg_High *glbParam->dzr ) ;

// 	double 	b_ref_max = 0.0 ;
// 	double 	b_ref_min = 0.0 ;
// 	double 	b_step    = 0.0 ;

// 	if ( (dataMol->last_Indx_Bkg_Low >0) && (dataMol->last_Indx_Bkg_High >0) ) // && (dataMol->last_Indx_Bkg_Low >0) && (dataMol->last_Indx_Bkg_High >0) 
// 	{	// FIRST BIAS GUESS OBTAINED FROM THE MEAN VALUE OF THE LAST BINS
// 		fitParam.indxInitFit = dataMol->last_Indx_Bkg_Low  ;
// 		fitParam.indxEndFit  = dataMol->last_Indx_Bkg_High ;
// 		fitParam.nFit	  	 =  fitParam.indxEndFit - fitParam.indxInitFit +1 ;
// 	}
// 	else if( (dataMol->last_Indx_Mol_Low >0) && (dataMol->last_Indx_Mol_High >0) )
// 	{	// FIRST BIAS GUESS OBTAINED FROM THE FIT IN THE MOLECULAR RANGES
// 		fitParam.indxInitFit = dataMol->last_Indx_Mol_Low  ;
// 		fitParam.indxEndFit  = dataMol->last_Indx_Mol_High ;
// 		fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1 ;
// 	}
// 	else
// 	{	// LIDAR SIGNAL CORRUPTED, ALL DATA FILLED WITH ZEROS
// 		*Bias_Pr = 0.0 	;
// 		printf("\n CLidar_Operations::Bias_Substraction_Auto() - NO BIAS IS SUBSTRACTED, POSSIBLE DAMAGED SIGNAL.\n") ;
// 		goto 	NO_BIAS	;
// 	}

// 		Fit( (double*)pr, (double*)dataMol->prMol, dataMol->nBins , "wB", "NOTall", (strcFitParam*)&fitParam, (double*)dummy ) ;

// 		b_ref_max = 1.5* fitParam.b ;
// 		b_ref_min = 0.5* fitParam.b ;
// 		b_step	  = (b_ref_max - b_ref_min) /nBiasRes_Auto ;

// 		for ( int s =0; s <nBiasRes_Auto; s++ )
// 			b_i[s] = (double) b_ref_min + s *b_step ;
// 		b_i[nBiasRes_Auto] = 0 ; // IN CASE OF UNBIASED LIDAR SIGNAL

// 		for( int l=0 ; l< nLoopFindBias ; l++ ) // nLoopFindBias = 5 3
// 		{
// 			for (int b =0; b <nBiasRes_Auto; b++)
// 			{
// 				for ( int i =0 ; i <glbParam->nBins ; i++ )
// 				{
// 					pr_NObkg_i[i]  = pr[i] - b_i[b] ;
// 					pr2_i[i]       = pr_NObkg_i[i] * pow(glbParam->r[i], 2) ;
// 				}
// 				Fit( (double*)pr2_i, (double*)dataMol->pr2Mol, glbParam->nBins , "wOutB", "NOTall", (strcFitParam*)&fitParam, (double*)dummy ) ;
// 				errRMS_Bias[b] = fitParam.squared_sum_fit ;
// 			}
// 				polyfitCoeff( (const double* const) b_i				, // X DATA
// 							(const double* const  ) errRMS_Bias		, // Y DATA
// 							(unsigned int         ) nBiasRes_Auto	,
// 							(unsigned int		  ) 2				,
// 							(double*			  ) coeff	 ) 		;
// 				*Bias_Pr = (double) -coeff[1] /(2*coeff[2]) ;

// 				b_ref_max = 1.5* (*Bias_Pr) ;
// 				b_ref_min = 0.5* (*Bias_Pr) ;
// 				b_step 	= (b_ref_max - b_ref_min) /nBiasRes_Auto ;
// 				for ( int b =0; b <nBiasRes_Auto; b++ )
// 					b_i[b] = (double) b_ref_min + b *b_step ;
// 		}

// NO_BIAS:
// 	for ( int i =0; i <glbParam->nBins ; i++)
// 		pr_noBias[i] = (pr[i] - *Bias_Pr);

// 	// } // if ( indxMaxRange <= (glbParam->nBins-1) )
// 	// else
// 	// {
// 	// 	printf("\n CLidar_Operations::Bias_Substraction_Auto() (glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] > (glbParam->nBins-1)) - Background substraction metod set is %s, but MEAN method is applied because nBins= %d and indxMaxRange= %d).\n", glbParam->BkgCorrMethod, glbParam->nBins, glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] ) ;
// 	// 	Bias_Substraction_Mean( (double*)pr, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (double*)pr_noBias) ;
// 	// }
// }

void CLidar_Operations::Find_Max_Range( double *pr, strcMolecularData *dataMol, strcGlobalParameters *glbParam )
{
	dataMol->last_Indx_Mol_Low  =-1 ;
	dataMol->last_Indx_Mol_High =-1 ;
	dataMol->last_Indx_Bkg_Low  =-1 ;
	dataMol->last_Indx_Bkg_High =-1 ;

	double R2_Max_Range = 0.20  	; // winzize = 2000 m --> este es el valor que se usa en el programa de matlab para SNR=1
	int winSize = (int)round(2000/dataMol->dzr) ;

	int indxInit_Search ;
	if ( dataMol->z_Mol_Max > glbParam->r[glbParam->nBins_Ch[glbParam->chSel]] )
		indxInit_Search = glbParam->nBins_Ch[glbParam->chSel] -1 ;
	else
		indxInit_Search = round(dataMol->z_Mol_Max/glbParam->dr) ;

	printf("\n") ;
	for ( int i = indxInit_Search ; i >=(glbParam->indxInitSig +winSize) ; i-- )
	{
		fitParam.indxEndFit  = i       ;
		fitParam.indxInitFit = fitParam.indxEndFit - winSize ;
		fitParam.nFit		 = fitParam.indxEndFit - fitParam.indxInitFit +1 ;
			Fit( (double*)pr, (double*)dataMol->prMol, glbParam->nBins_Ch[glbParam->chSel] , "wB", "NOTall", (strcFitParam*)&fitParam, (double*)dummy ) ;
			// if ( i== (glbParam->nBins_Ch[glbParam->chSel] -1) )
			// 	printf("\n Find_Max_Range(1) --> fitParam.indxInitFit= %d  fitParam.indxEndFit= %d  fitParam.R2= %lf", fitParam.indxInitFit, fitParam.indxEndFit, fitParam.R2 ) ;

		// if ( (fitParam.R2 >R2_NO_Signal) && (dataMol->last_Indx_Bkg_Low <0) && (fitParam.m >0) )
		// {
		// 	count_R2_NO_Signal++ ;
		// 	if ( count_R2_NO_Signal ==1 )
		// 	{
		// 		dataMol->last_Indx_Bkg_Low  = fitParam.indxInitFit 						;
		// 		dataMol->last_Indx_Bkg_High = glbParam->nBins_Ch[glbParam->chSel] -1 	;
		// 		// printf("\n Find_Max_Range(2) setting dataMol->last_Indx_Bkg_Low -> fitParam.indxInitFit= %d  fitParam.indxEndFit= %d  fitParam.R2= %lf", fitParam.indxInitFit, fitParam.indxEndFit, fitParam.R2 ) ;
		// 	}
		// }
		
		if ( (fitParam.R2_adj >R2_Max_Range) && (dataMol->last_Indx_Mol_Low <0) && (fitParam.m >0) )
		{
				dataMol->last_Indx_Mol_Low  = fitParam.indxInitFit	;
				dataMol->last_Indx_Mol_High = fitParam.indxEndFit   ;
				dataMol->last_Indx_Bkg_Low  = round( (dataMol->last_Indx_Mol_High + glbParam->nBins_Ch[glbParam->chSel]) /2 ) ;
				dataMol->last_Indx_Bkg_High = glbParam->nBins_Ch[glbParam->chSel] -1 ;
// printf("\n Find_Max_Range(3) setting dataMol->last_Indx_Mol_Low -> fitParam.indxInitFit= %d  fitParam.indxEndFit= %d  fitParam.R2= %lf", fitParam.indxInitFit, fitParam.indxEndFit, fitParam.R2 ) ;

				// IF THE BACKGROUND RANGE IS TOO SMALL, THEN IT IS NOT CONSIDERED
				if ( (dataMol->last_Indx_Bkg_High - dataMol->last_Indx_Bkg_Low) < winSize )
				{
					dataMol->last_Indx_Bkg_Low  = -1 ;
					dataMol->last_Indx_Bkg_High = -1 ;
				}
		}

		// if ( (dataMol->last_Indx_Mol_Low == dataMol->last_Indx_Bkg_Low) && (dataMol->last_Indx_Mol_Low >0) )
		// {   // IF THE FIRST FIT PRODUCE A R2 >R2_Max_Range (ALSO R2 >R2_NO_Signal) ==> IT ALREADY WENT INTO THE LAST TWO if()...
		// 	dataMol->last_Indx_Mol_Low  = fitParam.indxInitFit 	;
		// 	dataMol->last_Indx_Mol_High = fitParam.indxEndFit  	;
		// 	dataMol->last_Indx_Bkg_Low  = -1 					;
		// 	dataMol->last_Indx_Bkg_High = -1 					;
		// 	// printf("\n Find_Max_Range(4) -> dataMol->last_Indx_Mol_Low == dataMol->last_Indx_Bkg_Low \n") ;
		// }

		if( dataMol->last_Indx_Mol_Low >0 )
		{
			// printf("\n Find_Max_Range(5) ==> exiting with fitparam.R2_adj= %lf -- Max Range: %lf \n", fitParam.R2_adj, dataMol->last_Indx_Mol_High *glbParam->dzr ) ;
			break;
		}
	} // for ( int i = indxInit_Search ; i >=(glbParam->indxInitSig +winSize) ; i-- )

	// if (dataMol->last_Indx_Mol_Low <round(glbParam->indxInitSig /glbParam->dzr) )
	// 	dataMol->last_Indx_Mol_Low = *indxMaxRange ;

	// if( (dataMol->last_Indx_Mol_Low >0) && (dataMol->last_Indx_Mol_High>0) )
	// {
	// 	Remove_Cloud_Mol_Range( (double*)pr, (strcGlobalParameters*)glbParam, (strcMolecularData*)dataMol ) ;

	// 	dataMol->last_Indx_Mol_High = dataMol->last_Indx_Mol_Low + winSize 	;
	// 	if ( dataMol->last_Indx_Mol_High > (glbParam->nBins_Ch[glbParam->chSel] -1) )
	// 		dataMol->last_Indx_Mol_High = glbParam->nBins_Ch[glbParam->chSel] -1 ;
	// }

	glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] = dataMol->last_Indx_Mol_High ;
	glbParam->rEndSig_ev_ch   [glbParam->evSel][glbParam->chSel] = glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] * glbParam->dr ;

// printf( "CLidar_Operations::Find_Max_Range() --> ev: %d \n\t\t\t dataMol->last_Indx_Mol_Low[m]= %lf \t dataMol->last_Indx_Mol_High[m]= %lf \n\t\t\t dataMol->last_Indx_Bkg_Low[m]= %lf \t dataMol->last_Indx_Bkg_High[m]= %lf \n", 
// 										glbParam->evSel, dataMol->last_Indx_Mol_Low *glbParam->dzr, dataMol->last_Indx_Mol_High *glbParam->dzr,
// 										dataMol->last_Indx_Bkg_Low *glbParam->dzr, dataMol->last_Indx_Bkg_High *glbParam->dzr ) ;
} // void CLidar_Operations::Find_Max_Range( double *pr, double *prMol, strcGlobalParameters *glbParam )

// void CLidar_Operations::Find_Max_Range( double *pr, strcMolecularData *dataMol, strcGlobalParameters *glbParam )
// {
// 	dataMol->last_Indx_Mol_Low  =-1 ;
// 	dataMol->last_Indx_Mol_High =-1 ;
// 	dataMol->last_Indx_Bkg_Low  =-1 ;
// 	dataMol->last_Indx_Bkg_High =-1 ;

// 	double R2_Max_Range = 0.06  	;
// 	double R2_NO_Signal = 0.01 		;
// 	int	   count_R2_Max_Range = 0 	;
// 	int	   count_R2_NO_Signal = 0 	;
// 	int winSize = (int)round(4000/dataMol->dzr) ;

// 	int indxInit_Search ;
// 	if ( dataMol->z_Mol_Max > glbParam->r[glbParam->nBins_Ch[glbParam->chSel]] )
// 		indxInit_Search = glbParam->nBins_Ch[glbParam->chSel] -1 ;
// 	else
// 		indxInit_Search = round(dataMol->z_Mol_Max/glbParam->dr) ;

// 	printf("\n") ;
// 	for ( int i = indxInit_Search ; i >=(glbParam->indxInitSig +winSize) ; i-- )
// 	{
// 		fitParam.indxEndFit  = i       ;
// 		fitParam.indxInitFit = fitParam.indxEndFit - winSize ; // glbParam->nBinsBkg 	 ;
// 		fitParam.nFit		 = fitParam.indxEndFit - fitParam.indxInitFit +1 ;
// 			Fit( (double*)pr, (double*)dataMol->prMol, glbParam->nBins_Ch[glbParam->chSel] , "wB", "NOTall", (strcFitParam*)&fitParam, (double*)dummy ) ;
// 			// if ( i== (glbParam->nBins_Ch[glbParam->chSel] -1) )
// 			// 	printf("\n Find_Max_Range(1) --> fitParam.indxInitFit= %d  fitParam.indxEndFit= %d  fitParam.R2= %lf", fitParam.indxInitFit, fitParam.indxEndFit, fitParam.R2 ) ;

// 		if ( (fitParam.R2 >R2_NO_Signal) && (dataMol->last_Indx_Bkg_Low <0) && (fitParam.m >0) )
// 		{
// 			count_R2_NO_Signal++ ;
// 			if ( count_R2_NO_Signal ==1 )
// 			{
// 				dataMol->last_Indx_Bkg_Low  = fitParam.indxInitFit 						;
// 				dataMol->last_Indx_Bkg_High = glbParam->nBins_Ch[glbParam->chSel] -1 	;
// 				// printf("\n Find_Max_Range(2) setting dataMol->last_Indx_Bkg_Low -> fitParam.indxInitFit= %d  fitParam.indxEndFit= %d  fitParam.R2= %lf", fitParam.indxInitFit, fitParam.indxEndFit, fitParam.R2 ) ;
// 			}
// 		}
		
// 		if ( (fitParam.R2 >R2_Max_Range) && (dataMol->last_Indx_Mol_Low <0) && (fitParam.m >0) )
// 		{
// 			count_R2_Max_Range++ ;
// 			if ( count_R2_Max_Range ==1 )
// 			{
// 				dataMol->last_Indx_Mol_Low  = fitParam.indxInitFit	;
// 				dataMol->last_Indx_Mol_High = fitParam.indxInitFit +winSize ;
// 				// printf("\n Find_Max_Range(3) setting dataMol->last_Indx_Mol_Low -> fitParam.indxInitFit= %d  fitParam.indxEndFit= %d  fitParam.R2= %lf", fitParam.indxInitFit, fitParam.indxEndFit, fitParam.R2 ) ;
// 			}
// 		}

// 		if ( (dataMol->last_Indx_Mol_Low == dataMol->last_Indx_Bkg_Low) && (dataMol->last_Indx_Mol_Low >0) )
// 		{   // IF THE FIRST FIT PRODUCE A R2 >R2_Max_Range (ALSO R2 >R2_NO_Signal) ==> IT ALREADY WENT INTO THE LAST TWO if()...
// 			dataMol->last_Indx_Mol_Low  = fitParam.indxInitFit 	;
// 			dataMol->last_Indx_Mol_High = fitParam.indxEndFit  	;
// 			dataMol->last_Indx_Bkg_Low  = -1 					;
// 			dataMol->last_Indx_Bkg_High = -1 					;
// 			// printf("\n Find_Max_Range(4) -> dataMol->last_Indx_Mol_Low == dataMol->last_Indx_Bkg_Low \n") ;
// 		}

// 		if( dataMol->last_Indx_Mol_Low >0 )
// 		{
// 			// printf("\n Find_Max_Range(5) ==> exiting with fitparam.R2= %lf \n", fitParam.R2 ) ;
// 			break;
// 		}
// 	}
// 	// dataMol->last_Indx_Mol_Low  = dataMol->last_Indx_Mol_Low - winSize ;

// 	// if (dataMol->last_Indx_Mol_Low <round(glbParam->indxInitSig /glbParam->dzr) )
// 	// 	dataMol->last_Indx_Mol_Low = *indxMaxRange ;

// 	// dataMol->last_Indx_Mol_High = dataMol->last_Indx_Mol_Low + winSize 	;
// 	// if ( dataMol->last_Indx_Mol_High > (glbParam->nBins_Ch[glbParam->chSel] -1) )
// 	// 	dataMol->last_Indx_Mol_High = glbParam->nBins_Ch[glbParam->chSel] -1 ;

// 	if( (dataMol->last_Indx_Mol_Low >0) && (dataMol->last_Indx_Mol_High>0) )
// 	{
// 		Remove_Cloud_Mol_Range( (double*)pr, (strcGlobalParameters*)glbParam, (strcMolecularData*)dataMol ) ;

// 		dataMol->last_Indx_Mol_High = dataMol->last_Indx_Mol_Low + winSize 	;
// 		if ( dataMol->last_Indx_Mol_High > (glbParam->nBins_Ch[glbParam->chSel] -1) )
// 			dataMol->last_Indx_Mol_High = glbParam->nBins_Ch[glbParam->chSel] -1 ;
// 	}

// 	glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] = dataMol->last_Indx_Mol_High ;
// 	glbParam->rEndSig_ev_ch   [glbParam->evSel][glbParam->chSel] = glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] * glbParam->dr ;

// // printf( "CLidar_Operations::Find_Max_Range() --> ev: %d \n\t\t\t dataMol->last_Indx_Mol_Low[m]= %lf \t dataMol->last_Indx_Mol_High[m]= %lf \n\t\t\t dataMol->last_Indx_Bkg_Low[m]= %lf \t dataMol->last_Indx_Bkg_High[m]= %lf \n", 
// // 										glbParam->evSel, dataMol->last_Indx_Mol_Low *glbParam->dzr, dataMol->last_Indx_Mol_High *glbParam->dzr,
// // 										dataMol->last_Indx_Bkg_Low *glbParam->dzr, dataMol->last_Indx_Bkg_High *glbParam->dzr ) ;
// } // void CLidar_Operations::Find_Max_Range( double *pr, double *prMol, strcGlobalParameters *glbParam )

void CLidar_Operations::Remove_Cloud_Mol_Range( double *pr, strcGlobalParameters *glbParam, strcMolecularData *dataMol )
{
		int 	nWinSize = round( (dataMol->last_Indx_Mol_High - dataMol->last_Indx_Mol_Low +1)/2 ) ;
		int     nSteps   = dataMol->last_Indx_Mol_High - dataMol->last_Indx_Mol_Low +1 - nWinSize   ;
	
		strcFitParam	fitParam 											;
		double		 	*stdPr 			= (double*) new double	[nSteps] 	;
		int		 		*indxRef_Start 	= (int*) 	new int		[nSteps] 	;
		int		 		*indxRef_Stop  	= (int*) 	new int		[nSteps] 	;
		double 			*prFit = (double*) new double[glbParam->nBins] 		;
		fitParam.indxInitFit = dataMol->last_Indx_Mol_Low					;
		fitParam.indxEndFit  = fitParam.indxInitFit + nWinSize					;
		fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1	;
		int i 			  	 = 0 ;
		do
		{
			Fit( (double*)&pr[0], (double*)&(dataMol->prMol[0]), fitParam.nFit, "wB", "NOTall", (strcFitParam*)&fitParam, (double*)prFit ) ;
			stdPr[i] 		 = sqrt( fitParam.squared_sum_fit/(fitParam.nFit -1) ) ;
			indxRef_Start[i] = fitParam.indxInitFit ;
			indxRef_Stop [i] = fitParam.indxEndFit  ;

			fitParam.indxInitFit = fitParam.indxInitFit +1							;
			fitParam.indxEndFit  = fitParam.indxInitFit + nWinSize					;
			fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1	;
			i++ ;
		} while( fitParam.indxEndFit <= dataMol->last_Indx_Mol_High ) ;
		delete  prFit ;	

		int indxMin_Std, minStd ;
		findIndxMin( (double*)stdPr, (int)0, (int)(nSteps -1), (int*)(&indxMin_Std), (double*)(&minStd) ) ;

		dataMol->last_Indx_Mol_Low  = indxRef_Start[indxMin_Std] ;
		dataMol->heightRef_Inversion_ASL = (double) (glbParam->siteASL + glbParam->dzr*( dataMol->last_Indx_Mol_Low + dataMol->last_Indx_Mol_High )/2) ;
}

void CLidar_Operations::Remove_Bkg_Mol_Range( double *pr, strcGlobalParameters *glbParam, strcMolecularData *dataMol )
{
	int nSteps = dataMol->last_Indx_Mol_High - dataMol->last_Indx_Mol_Low +1 ;
 
	strcFitParam	fitParam 											;
	double		 	*R2 			= (double*) new double	[nSteps] 	;  memset( R2, 0, (sizeof(double) * nSteps) ) ;
	int		 		*indxRef_Stop  	= (int*) 	new int		[nSteps] 	;
	double 			*prFit = (double*) new double[glbParam->nBins] 		;
	fitParam.indxEndFit  = dataMol->last_Indx_Mol_High ; // fitParam.indxInitFit + nWinSize					;
	fitParam.indxInitFit = dataMol->last_Indx_Mol_Low  ;
	fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1	;
	int i 			  	 = 0 ;
	do
	{
		Fit( (double*)&pr[0], (double*)&(dataMol->prMol[0]), fitParam.nFit, "wB", "NOTall", (strcFitParam*)&fitParam, (double*)prFit ) ;
		R2[i] 		 = fitParam.R2 ;
		indxRef_Stop [i] = fitParam.indxEndFit  ;
		// printf( "\nRange Stop[i]= %lf \t R2[i]= %lf", indxRef_Stop[i]*glbParam->dr, R2[i] ) ;

		fitParam.indxEndFit  = fitParam.indxEndFit -1 ;
		fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1	;
		i++ ;
	} while( i <(nSteps-5) ) ;
	delete  prFit ;	

	int indxMin_Std, minStd ;
	findIndxMax( (double*)R2, (int)0, (int)(nSteps -1), (int*)(&indxMin_Std), (double*)(&minStd) ) ;
	
	// printf( "\nindxRef_Stop[%d]= %d", indxMin_Std, indxRef_Stop[indxMin_Std] ) ;
	dataMol->last_Indx_Mol_High = indxRef_Stop [indxMin_Std] ;
}

// BIAS SUBSTRACTION METHODS
void CLidar_Operations::Bias_Substraction_Mean( double *sig, strcMolecularData *dataMol, strcGlobalParameters *glbParam, double *pr_noBias)
{
	if ( (glbParam->range_Bkg_Start <0) || (glbParam->range_Bkg_Stop <0) || ( glbParam->range_Bkg_Start > glbParam->range_Bkg_Stop ) )
	{	
		Find_Max_Range( (double*)sig, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam) ;
		printf( "\nBias_Substraction_Mean() ====> \n dataMol->last_Indx_Mol_Low[m]= %lf\n dataMol->last_Indx_Mol_High[m]= %lf\n dataMol->last_Indx_Bkg_Low[m]= %lf\n dataMol->last_Indx_Bkg_High[m]= %lf\n", 
							dataMol->last_Indx_Mol_Low *glbParam->dzr, dataMol->last_Indx_Mol_High *glbParam->dzr, dataMol->last_Indx_Bkg_Low *glbParam->dzr, dataMol->last_Indx_Bkg_High *glbParam->dzr ) ;
	}
	else
	{  	// BACKGROUND RANGE IS SET BY THE USER
		dataMol->last_Indx_Bkg_Low  = (int) round(glbParam->range_Bkg_Start /glbParam->dr) ;
		dataMol->last_Indx_Bkg_High = (int) round(glbParam->range_Bkg_Stop  /glbParam->dr) ;
	}

	double 	bkgMean = 0.0 ;

	// IF Find_Max_Range() DOES NOT FIND A GOOD RANGE, THEN THE BIAS IS OBTAINED FROM THE AUTOMATIC METHOD
	if ( (dataMol->last_Indx_Bkg_Low <0) && (dataMol->last_Indx_Bkg_High <0) && (dataMol->last_Indx_Mol_Low <0) && (dataMol->last_Indx_Mol_High <0) )
		printf("\nBias_Substraction_Mean() ==> No background substracted to channel %d \n", glbParam->chSel ) ;
	else
	{	// CALCULATE THE MEAN VALUE OF THE BACKGROUND AND SUBSTRACT IT FROM THE SIGNAL.
		for( int j=dataMol->last_Indx_Bkg_Low ; j<=dataMol->last_Indx_Bkg_High ; j++ ) bkgMean = bkgMean + sig[j] ;

		bkgMean = bkgMean /( dataMol->last_Indx_Bkg_High - dataMol->last_Indx_Bkg_Low +1) ;
		for ( int i=0 ; i<glbParam->nBins_Ch[glbParam->chSel] ; i++ ) 	pr_noBias[i] = (double)(sig[i] - bkgMean) ;
	}
	// Find_Max_Range( (double*)sig, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam) ;	
}

void CLidar_Operations::Bias_Substraction_MolFit(strcMolecularData *dataMol, const double *prEl, strcGlobalParameters *glbParam, double *pr_no_DarkCur)
{
	if ( (glbParam->range_Bkg_Start <0) || (glbParam->range_Bkg_Stop <0) || ( glbParam->range_Bkg_Start > glbParam->range_Bkg_Stop ) )
		Find_Max_Range( (double*)prEl, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam) ;
	else
	{	// BACKGROUND RANGE IS SET BY THE USER
		dataMol->last_Indx_Mol_Low  = (int) round(glbParam->range_Bkg_Start /glbParam->dr) ;
		dataMol->last_Indx_Mol_High = (int) round(glbParam->range_Bkg_Stop  /glbParam->dr) ;
	}

	double Bias_Pr = 0.0 ;

	if ( (dataMol->last_Indx_Mol_Low <0) && (dataMol->last_Indx_Mol_High <0) )
		Bias_Substraction_Auto( (double*)prEl, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (double*)pr_no_DarkCur, (double*)&Bias_Pr ) ;
	else
	{
		fitParam.indxInitFit = dataMol->last_Indx_Mol_Low  ;
		fitParam.indxEndFit  = dataMol->last_Indx_Mol_High ;
		fitParam.nFit	   	 = fitParam.indxEndFit - fitParam.indxInitFit +1;
		// printf("\nBias_Substraction_MolFit() ==> fitParam.indxInitFit[m]= %lf \t fitParam.indxEndFit[m]= %lf \n", fitParam.indxInitFit *glbParam->dzr, fitParam.indxEndFit*glbParam->dzr ) ;

			Fit( (double*)prEl, (double*)dataMol->prMol, dataMol->nBins , "wB", "NOTall", (strcFitParam*)&fitParam, (double*)dummy ) ;
				for ( int i=0 ; i<dataMol->nBins ; i++ ) 	pr_no_DarkCur[i] = (double)(prEl[i] - fitParam.b) ; 
	}
}

void CLidar_Operations::Bias_Substraction_Pre_Trigger( double *sig, strcGlobalParameters *glbParam, double *pr_noBias)
{
	fitParam.indxInitFit = round(glbParam->range_Bkg_Start /glbParam->dr)  ;
	fitParam.indxEndFit  = round(glbParam->range_Bkg_Stop  /glbParam->dr) ;
	fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1;
	// printf("\nBias_Substraction_Mean() ==> fitParam.indxInitFit[m]= %lf \t fitParam.indxEndFit[m]= %lf \n", fitParam.indxInitFit *glbParam->dzr, fitParam.indxEndFit*glbParam->dzr ) ;

	double 	bkgMean = 0 ;
	for( int j=fitParam.indxInitFit ; j<fitParam.indxEndFit ; j++ ) bkgMean = bkgMean + sig[j] ;

	bkgMean = bkgMean /fitParam.nFit ;
	for ( int i=0 ; i<glbParam->nBins_Ch[glbParam->chSel] ; i++ ) 	pr_noBias[i] = (double)(sig[i] - bkgMean) ;
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
			for ( int b=0 ; b <glbParam->nBins_Ch[c] ; b++ )
			{
				for ( int t=0 ; t <glbParam->numEventsToAvg ; t++ )
				{
					dataFile_AVG[fC][c][b] = (double) dataFile_AVG[fC][c][b] + dataFile[ fC *glbParam->numEventsToAvg +t ][c][b] ;
					if( (b==0) && (c==0) )
					{	// TIME IS *NOT* AVERAGED, ITS SAVED THE START AND END TIME OF EACH CLUSTER
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
		printf("\n\n\n====================================> Applying corrections to the lidar event number: %d/%d <======================================================================", glbParam->evSel, (glbParam->nEventsAVG -1) ) ;
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
                for ( int b=(glbParam->nBins_Ch[glbParam->chSel] -2*glbParam->indxOffset[c]) ; b <(glbParam->nBins_Ch[glbParam->chSel]-glbParam->indxOffset[c]) ; b++ )
					pr_corr[e][c][b+glbParam->indxOffset[c]] = (double)data_File_Lx[e][c][b] ;
                // for ( int b=(glbParam->nBins_Ch[glbParam->chSel] -glbParam->indxOffset[c]) ; b <glbParam->nBins_Ch[glbParam->chSel] ; b++ )
				// 	pr_corr[e][c][b] = (double)data_File_Lx[e][c][glbParam->nBins_Ch[glbParam->chSel] -glbParam->indxOffset[c]] ;
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

			if ( (glbParam->DAQ_Type[c] == 0) || (glbParam->DAQ_Type[c] == 1) ) //      ANALOG OR PHOTON-COUNTING LIDAR SIGNALS DATA TYPES
			{
				if( glbParam->is_Ovlp_Data_Loaded ==true )
				{
					// OVERLAP CORRECTION //--------------------------------------------------------------
					printf("| Overlap |  ") ;
					for ( int i=0 ; i <glbParam->nBins_Ch[glbParam->chSel] ; i++ )
						evSig.pr[i] = (double)( (double)pr_corr[e][c][i] / (double)(ovlp[c][i]) ) ;
				}
				else
				{
					for ( int i=0 ; i <glbParam->nBins_Ch[glbParam->chSel] ; i++ )
						evSig.pr[i] = (double)pr_corr[e][c][i] ;
				}

				// BACKGROUND & BIAS CORRECTION //--------------------------------------------------------------
				// DARK-CURRENT AND RANGE CORRECTION
				if ( glbParam->is_Noise_Data_Loaded == true )
				{
						if ( strcmp( glbParam->exeFile, "./lpp1" ) ==0 )
							oMolData->Fill_dataMol_L1( (strcGlobalParameters*)glbParam ) ;
						else if ( strcmp( glbParam->exeFile, "./lpp2" ) ==0 )
							oMolData->Fill_dataMol_L2( (strcGlobalParameters*)glbParam ) ;
						else
						{
							printf("Error: Unknown executable file name: %s\n", glbParam->exeFile) ;
							exit(1) ;
						}

					printf("| Dark-Current and bias |  ") ;
					if ( (glbParam->DAQ_Type[glbParam->chSel] ==0) || (glbParam->DAQ_Type[glbParam->chSel] ==1) )
					{
						memset( dummy, 0, ( sizeof(double) * glbParam->nBins_Ch[glbParam->chSel] ) ) ;
						smooth( (double*)data_Noise[c], (int)0, (int)(glbParam->nBins_Ch[glbParam->chSel]-1), (int)5, (double*)dummy ) ;	

						// for (int i =0; i <glbParam->nBins_Ch[e]; i++)
						// 	dummy[i] = glbParam->r[i] ;
						// memset( dummy1, 0, ( sizeof(double) * glbParam->nBins_Ch[glbParam->chSel] ) ) ;
						// fitParam.indxInitFit = 10 ;
						// fitParam.indxEndFit  = glbParam->nBins_Ch[c] ;
						// fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1 ;
						// Fit( (double*)dummy, (double*)data_Noise[c], glbParam->nBins, "wB", "all", (strcFitParam*)&fitParam, (double*)dummy1 ) ;

						for (int i =0; i <glbParam->nBins; i++)
							evSig.pr_no_DarkCur[i] = (double)( evSig.pr[i] - dummy[i] ) ;
							// evSig.pr_no_DarkCur[i] = (double)( evSig.pr[i] - dummy1[i] ) ;
							// evSig.pr_no_DarkCur[i] = (double)( evSig.pr[i] - data_Noise[glbParam->chSel][i] ) ;

						BiasCorrection( (strcLidarSignal*)&evSig, (strcGlobalParameters*)glbParam, (strcMolecularData*)&oMolData->dataMol ) ;
					}
					else
					{
						for ( int i=0 ; i<glbParam->nBins ; i++ ) 	
						{
							evSig.pr_noBias[i] 		= (double)evSig.pr[i] ;
							evSig.pr_no_DarkCur [i] = (double)evSig.pr[i] ;
						}
					}
				}
				else // BIAS REMOVAL BASED ON VARIABLE BkgCorrMethod SET IN FILE THE SETTING FILE PASSED AS ARGUMENT TO lpp2
				{
					printf("| Bias |  ") ;

						if ( strcmp( glbParam->exeFile, "./lpp1" ) ==0 )
							oMolData->Fill_dataMol_L1( (strcGlobalParameters*)glbParam ) ;
						else if ( strcmp( glbParam->exeFile, "./lpp2" ) ==0 )
							oMolData->Fill_dataMol_L2( (strcGlobalParameters*)glbParam ) ;
						else
						{
							printf("Error: Unknown executable file name: %s\n", glbParam->exeFile) ;
							exit(1) ;
						}

					if ( (glbParam->DAQ_Type[glbParam->chSel] ==0) || (glbParam->DAQ_Type[glbParam->chSel] ==1) ) // BIAS REMOVAL ONLY FOR ANALOG SIGNALS 
						BiasCorrection( (strcLidarSignal*)&evSig, (strcGlobalParameters*)glbParam, (strcMolecularData*)&oMolData->dataMol ) ;
					else
					{
						for ( int i=0 ; i<glbParam->nBins_Ch[glbParam->chSel] ; i++ ) 	
						{
							evSig.pr_noBias[i] 		= (double)evSig.pr[i] ;
							evSig.pr_no_DarkCur [i] = (double)evSig.pr[i] ;
						}
					}
				}
					for ( int i=0 ; i<glbParam->nBins ; i++ ) 	evSig.pr2[i] = evSig.pr_noBias[i] * pow(glbParam->r[i], 2) ;
				// } // if ( indxWL_PDLx == glbParam->chSel )
			} // if ( (glbParam->DAQ_Type[c] == 0) || (glbParam->DAQ_Type[c] == 1) )
			else
			{	// CHANNEL IS NOT ANALOG NOR PHOTONCOUNTING (MAYBE ERROR CHANNEL, POWER METER OR OVERFLOW FOR THE NEW LICEL FILENAMES)
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
	int		indxs_fit[3] ; // INDEXES TO PERFORM THE FIT FOR THE GLUING

	for ( int c =0 ; c <glbParam->nPair_Ch_to_Glue ; c++ )
	{
		findIndxMin( (double*)&pr_corr[glbParam->evSel][glbParam->indx_gluing_High_PHO[c]][0], (glbParam->nBins_Ch[glbParam->chSel]-1-glbParam->nBinsBkg), (glbParam->nBins_Ch[glbParam->chSel]-1), (int*)&indx_MHz_Min, (double*)&MHz_Min ) ;
		findIndxMax( (double*)&pr_corr[glbParam->evSel][glbParam->indx_gluing_High_PHO[c]][0], 0														 , (glbParam->nBins_Ch[glbParam->chSel]-1), (int*)&indx_MHz_Max, (double*)&MHz_Max ) ;

		if ( (MHz_Min <= glbParam->MIN_TOGGLE_RATE_MHZ) && (MHz_Max >= glbParam->MAX_TOGGLE_RATE_MHZ) )
		{
			printf("\nEvent= %d --> Gluing channels %d and %d (%d nm)", glbParam->evSel, glbParam->indx_gluing_Low_AN[c],
																		glbParam->indx_gluing_High_PHO[c], glbParam->iLambda[glbParam->indx_gluing_High_PHO[c]] ) ;
			// CORRECTED PHOTON COUNTING VALUES HIGHER THAN MAX_TOGGLE_RATE_MHZ
			memset( dummy, 0, ( sizeof(double) * glbParam->nBins_Ch[glbParam->chSel] ) ) ;
			for ( int b =0 ; b <glbParam->nBins_Ch[glbParam->chSel] ; b++ )
				dummy[b] = fabs(pr_corr[glbParam->evSel][glbParam->indx_gluing_High_PHO[c]][b] - glbParam->MAX_TOGGLE_RATE_MHZ) ;
			findIndxMin( (double*)&dummy[0], 0			 , indx_MHz_Max		  					  , (int*)&indxs_fit[0], (double*)&a ) ; // indxs_fit[0] DISCARDED
			findIndxMin( (double*)&dummy[0], indx_MHz_Max, (glbParam->nBins_Ch[glbParam->chSel]-1), (int*)&indxs_fit[1], (double*)&a ) ;

			// CORRECTED PHOTON COUNTING VALUES LOWER THAN MIN_TOGGLE_RATE_MHZ
			memset( dummy, 0, ( sizeof(double) * glbParam->nBins_Ch[glbParam->chSel] ) ) ;
			for ( int b =0 ; b <glbParam->nBins_Ch[glbParam->chSel] ; b++ )
				dummy[b] = fabs(pr_corr[glbParam->evSel][glbParam->indx_gluing_High_PHO[c]][b] - glbParam->MIN_TOGGLE_RATE_MHZ) ;
			findIndxMin( (double*)&dummy[0], indxs_fit[1], (glbParam->nBins_Ch[glbParam->chSel]-1), (int*)&indxs_fit[2], (double*)&a ) ;

			memset( dummy, 0, ( sizeof(double) * glbParam->nBins ) ) ;
			fitParam.indxInitFit = indxs_fit[1] ;
			fitParam.indxEndFit  = indxs_fit[2] ;
			fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1 ;
			printf(" across %lf - %lf m\n", indxs_fit[1]*glbParam->dzr, indxs_fit[2]*glbParam->dzr) ;

			Fit( (double*)&pr_corr[glbParam->evSel][glbParam->indx_gluing_High_PHO[c]][0], 
			(double*)&pr_corr[glbParam->evSel][glbParam->indx_gluing_Low_AN  [c]][0], glbParam->nBins_Ch[glbParam->chSel], "wB", "all", (strcFitParam*)&fitParam, (double*)dummy ) ;

			// dummy: ANALOG SIGNAL FITTED TO THE PHOTON-COUNTING SIGNAL IN THE RANGES indxs_fit[1]-indxs_fit[2]
			for ( int b =0 ; b <indxs_fit[1] ; b++ )
				pr_corr[glbParam->evSel][glbParam->indx_gluing_Low_AN[c]][b] = (double)dummy[b] ;
			for ( int b =indxs_fit[1] ; b <glbParam->nBins_Ch[glbParam->chSel] ; b++ )
				pr_corr[glbParam->evSel][glbParam->indx_gluing_Low_AN[c]][b] = (double)pr_corr[glbParam->evSel][glbParam->indx_gluing_High_PHO[c]][b] ;

		} // if ( (MHz_Min <= glbParam->MIN_TOGGLE_RATE_MHZ) && (MHz_Max >= glbParam->MAX_TOGGLE_RATE_MHZ) )
		else
			printf("\nEvent= %d --> There is no need to glue the channels %d and %d\n", glbParam->evSel, glbParam->indx_gluing_Low_AN[c], glbParam->indx_gluing_High_PHO[c]) ;
	} // for ( int c =0 ; c <glbParam->nPair_Ch_to_Glue ; c++ )
	printf("\n") ;
}

void CLidar_Operations::Fit( double *sig, double *sigMol, int nBins, const char *modeBkg, const char *modeRangesFit, strcFitParam *fitParam, double *sigFit )
{
	fitParam->squared_sum_fit = (double)0.0  ; // RSS

	if ( strcmp( modeBkg, "wB" ) == 0 )
	{
		double *coeff = (double*) new double[2] ;
		polyfitCoeff( (const double* const) &sigMol[fitParam->indxInitFit], // X DATA
		  			  (const double* const) &sig   [fitParam->indxInitFit], // Y DATA
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
				sigFit[i] = (double) ( sigMol[i] * fitParam->m + fitParam->b ) ;
				fitParam->squared_sum_fit = fitParam->squared_sum_fit + pow( (sigFit[i] - sig[i]), 2 ) ; // RSS
			}
		}
		else
		{ // "NOTall"
			for ( int i=fitParam->indxInitFit ; i <=fitParam->indxEndFit ; i++ )
			{
				sigFit[i] = (double) ( sigMol[i] * fitParam->m + fitParam->b ) ;
				fitParam->squared_sum_fit = fitParam->squared_sum_fit + pow( (sigFit[i] - sig[i]), 2 ) ; // RSS
			}
		}
	}
	else if ( strcmp( modeBkg, "wOutB" ) == 0 )
	{
		double	mNum=0, mDen=0 ;
		for ( int i=fitParam->indxInitFit ; i <= fitParam->indxEndFit ; i++ )
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
				sigFit[i] = (double) ( sigMol[i] * fitParam->m ) ;
				fitParam->squared_sum_fit = fitParam->squared_sum_fit + pow( (sigFit[i] - sig[i]), 2 ) ; // RSS
			}
		}
		else
		{
			for ( int i=fitParam->indxInitFit ; i <=fitParam->indxEndFit ; i++ )
			{
				sigFit[i] = (double) ( sigMol[i] * fitParam->m ) ;
				fitParam->squared_sum_fit = fitParam->squared_sum_fit + pow( (sigFit[i] - sig[i]), 2 ) ; // RSS
			}
		}
	}

	if ( fpclassify(fitParam->squared_sum_fit) == FP_NAN )
	{
		fitParam->var = (double)DBL_MAX ;
		fitParam->std = (double)DBL_MAX ;
			printf("\n\tFit(): a NAN value was obtained during the fit... (fitParam->indxInitFit= %d - fitParam->indxEndFit= %d)\n ", fitParam->indxInitFit, fitParam->indxEndFit ) ;

			for (int i =0; i <nBins ; i++)
			{
				if ( (fpclassify(sigMol[i]) == FP_NAN) || (fpclassify(sig[i]) == FP_NAN) )
				{
					printf("\n\t\t sig[%d] = %lf ", i, sig[i] ) ;
					break ;
				}
			}
	}
	else
	{
		fitParam->var = (double)fitParam->squared_sum_fit / (fitParam->nFit -1) ;
		fitParam->std = (double)sqrt(fitParam->var)  							;
	}

	fitParam->mean_sig  = (double) 0.0 ;
	fitParam->s 		= (double) 0.0 ;
// printf("\n\tFit(): fitParam->indxInitFit = %d \t fitParam->indxEndFit= %d \n", fitParam->indxInitFit , fitParam->indxEndFit ) ;
	sum( (double*)sig, (int)fitParam->indxInitFit, (int)fitParam->indxEndFit, (double*)&fitParam->s ) ;

	if ( fitParam->s !=0 )
	{
		fitParam->mean_sig	= (double) fitParam->s /fitParam->nFit ;
		fitParam->squared_sum_sig_vs_Mean = (double)0.0 ;
		for ( int b=fitParam->indxInitFit ;  b<=fitParam->indxEndFit ; b++ )
			fitParam->squared_sum_sig_vs_Mean  = fitParam->squared_sum_sig_vs_Mean + pow( (sig[b] - fitParam->mean_sig), 2) ; // TSS

		// R2: COEFFICIENT OF DETERMINATION = 1 - RSS/TSS
		// RSS = Residuals Squared Sum  = ( sig - sigFit   )^2 --> SQUARED SUM OF THE ERROR OF THE MODEL
		// TSS = Total Squares Sum      = ( sig - mean_sig )^2 --> SQUARED SUM OF THE ERROR OF THE MEAN
		// dfTSS = nFit -1
		// dfRSS = nFit -2 -1
		// R2_adj = 1 - ( RSS / (nFit -2) ) / ( TSS / (nFit -1) )
		double dfTSS = (double)fitParam->nFit -1 	;
		double dfRSS = (double)fitParam->nFit -2 -1 ;
		fitParam->R2  	 = (double) ( fitParam->squared_sum_sig_vs_Mean - fitParam->squared_sum_fit) / fitParam->squared_sum_sig_vs_Mean ;
		fitParam->R2_adj = (double) 1- ( (fitParam->squared_sum_fit /dfRSS) / (fitParam->squared_sum_sig_vs_Mean /dfTSS) ) ;
		fitParam->F  	 = (double) ( fitParam->squared_sum_sig_vs_Mean - fitParam->squared_sum_fit) / fitParam->squared_sum_fit		  ;
	}
	else
	{
		fitParam->R2 = (double) 0.0 ;
		fitParam->F  = (double) 0.0 ;
	}	
// printf( "\n Fit() ==> fitParam->squared_sum_sig_vs_Mean= %e \t fitParam->squared_sum_fit= %e \t fitParam->R2= %e \t fitParam->mean_sig= %e \n", 
// 					  fitParam->squared_sum_sig_vs_Mean, fitParam->squared_sum_fit, fitParam->R2, fitParam->mean_sig ) ;
} // void CLidar_Operations::Fit( double *sig, double *sigMol, int nBins, const char *modeBkg, const char *modeRangesFit, strcFitParam *fitParam, double *sigFit )

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
		fitParam_before_cloud.indxInitFit = indxBefCloud - (int)round(DELTA_RANGE_LIM_BINS) ;
		fitParam_before_cloud.indxEndFit  = indxBefCloud - (int)round(DELTA_RANGE_LIM_BINS/5) ;
		fitParam_before_cloud.nFit = fitParam_before_cloud.indxEndFit - fitParam_before_cloud.indxInitFit +1;
			Fit( (double*)pr, (double*)dataMol->prMol, glbParam->nBins, "wB", "NOTall", (strcFitParam*)&fitParam_before_cloud, (double*)prFit ) ;
			indxB = indxBefCloud -(int)round(DELTA_RANGE_LIM_BINS/2) ;
			prBeforeCloud = prFit[indxB] ;
			// printf("\n prFit[indxB]= %lf \t fitParam_before_cloud.m= %lf \n", prFit[indxB], fitParam_before_cloud.m ) ;
			// printf(" pr[%d]= %lf \n", indxB, pr[indxB] ) ;
		fitParam_after_cloud.indxInitFit = indxAftCloud + (int)round(DELTA_RANGE_LIM_BINS/5) ;
		fitParam_after_cloud.indxEndFit  = indxAftCloud + (int)round(DELTA_RANGE_LIM_BINS) ;
		fitParam_after_cloud.nFit = fitParam_after_cloud.indxEndFit - fitParam_after_cloud.indxInitFit +1;
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
	indxBefCloud = fitParam->indxInitFit ;
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
	// 	printf("\nOptical Depth of the cloud: %lf \t Altitude (asl): %lf - %lf\n", OD, glbParam->siteASL + fitParam->indxInitFit * dataMol->dzr, glbParam->siteASL + fitParam->indxEndFit * dataMol->dzr ) ;

	// delete pr2 ;
}

// void CLidar_Operations::Bias_Residual_Correction( const double *pr, strcGlobalParameters *glbParam, strcMolecularData *dataMol, double *pr_res_corr )
// {
// 	fitParam.indxInitFit = 300 ;
// 	fitParam.indxEndFit  = 900 ;
// 	fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1 ;
// 	Fit( (double*)&pr[0], (double*)&dataMol->prMol[0], glbParam->nBins, "wB", "all", (strcFitParam*)&fitParam, (double*)dummy ) ;

// 	for (int i =0; i <glbParam->nBins; i++)
// 		pr_res_corr[i] = pr[i] - dummy[i] ;
	
// 	double *coeff = (double*) new double[2] ;
// 	polyfitCoeff( (const double* const) &sigMol[fitParam->indxInitFit], // X DATA
// 				  (const double* const) &sig[fitParam->indxInitFit], // Y DATA
// 				  (unsigned int       ) fitParam->nFit,
// 				  (unsigned int		  ) 1,
// 				  (double*			  ) coeff	 ) ;
// 	fitParam.m = (double)coeff[1] ;
// 	fitParam.b = (double)coeff[0] ;
// 	delete coeff ;
// }
