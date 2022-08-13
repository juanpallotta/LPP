
#include "CLidar_Operations.hpp"

CLidar_Operations::CLidar_Operations( strcGlobalParameters *glbParam )
{
	// noiseFit 	= (double*) new double[glbParam->nBins] ;
	dummy 	 	= (double*) new double[glbParam->nBins] ; 
    pr_NObkg_i	= (double*) new double[glbParam->nBins] ; 
	pr2_i 		= (double*) new double[glbParam->nBins] ;	
}

CLidar_Operations::~CLidar_Operations()
{
	// delete noiseFit ;
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

//! IMPLEMENT A FASTER CODE ALREADY DONE IN OCATVE (SEE FindBias_Pr.m)
//! CALL IT FROM C:\Users\jpall\Mega\procesamiento\Octave\Lidar_Analysis_Plot_NetCDF\temp.m
void CLidar_Operations::bkgSubstraction_Auto( double *pr, strcMolecularData *dataMol, strcGlobalParameters *glbParam, int nStepsAuto, double *pr_noBkg, double *Bias_Pr )
{
	int 	indxMaxRange ;
	Find_Max_Range( (double*)pr, (double*)dataMol->prMol, (strcGlobalParameters*)glbParam, (int*)&indxMaxRange ) ;
  	double 	b_ref_max = 0.0 ;
  	double 	b_ref_min = 0.0 ;

// BIAS OBTAINED BY APPLYING A LINEAR FIT FROM rEndSig SET IN THE CONFIGURATION FILE PASSED AS ARGUMENT
	fitParam.indxEndFit  = indxMaxRange ; // glbParam->indxEndSig ; // 
	fitParam.indxInicFit = fitParam.indxEndFit - glbParam->nBinsBkg ;
	fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInicFit +1;
	RayleighFit( (double*)pr, (double*)dataMol->prMol, dataMol->nBins , "wB", "NOTall", (strcFitParam*)&fitParam, (double*)dummy ) ;
	b_ref_max = 1.5* fitParam.b ;
	b_ref_min = 0.5* fitParam.b ;

  	double b_step 		= (b_ref_max - b_ref_min) /nStepsAuto ;
  	double *b_i   		= (double*) new double [ nStepsAuto +2 ] ;
  	double *errRMS_Bias = (double*) new double [ nStepsAuto +2 ] ;
	b_i[0] = (double)0.0 ; // IN CASE OF NO BACKGROUND NEEDED TO BE SUBSTRACTED.
	b_i[1] = (double)0.0 ; // MEAN VALUE OF THE glbParam->nBinsBkg BINS TAKEN FROM THE TAIL OF THE LIDAR SIGNAL.
	for( int j=(glbParam->nBins-glbParam->nBinsBkg) ; j<glbParam->nBins ; j++ ) b_i[1] = (double)(b_i[1] + pr[j]) ;
		b_i[1] = (double)(b_i[1] /glbParam->nBinsBkg) ;
	// PRODUCE THE ARRAY OF BIAS TO TEST b_i[.]
	for ( int s =2; s <(nStepsAuto+2); s++ )
		b_i[s] = (double) b_ref_min + (s-2) *b_step ;

	for (int s =0; s <(nStepsAuto +2); s++)
	{
		for ( int i =0 ; i <glbParam->nBins ; i++ )
		{
			pr_NObkg_i[i]  = pr[i] - b_i[s] ;
			pr2_i[i]       = pr_NObkg_i[i] * pow(glbParam->r[i], 2) ;
		}
		RayleighFit( (double*)pr2_i, (double*)dataMol->pr2Mol, glbParam->nBins , "wOutB", "NOTall", (strcFitParam*)&fitParam, (double*)dummy ) ;
		errRMS_Bias[s] = fitParam.sumsq_m ;
	}

	int 	indxMinErr ;
	double	minErr ;
	findIndxMin( errRMS_Bias, 0, (nStepsAuto +2-1), &indxMinErr, &minErr ) ;
	*Bias_Pr = (double) b_i[indxMinErr] ;

	for ( int i =0; i <glbParam->nBins ; i++)
	   pr_noBkg[i] = (pr[i] - *Bias_Pr);

	delete errRMS_Bias  ;
	delete b_i			;
}

void CLidar_Operations::Find_Max_Range( double *pr, double *prMol, strcGlobalParameters *glbParam, int *indxMaxRange )
{
    strcFitParam fitParam ;
	fitParam.indxEndFit  = glbParam->nBins -1 ;
    fitParam.indxInicFit = fitParam.indxEndFit - glbParam->nBinsBkg ;
	fitParam.nFit		 = fitParam.indxEndFit - fitParam.indxInicFit +1 ;

    int i =0 ;
	double	corrCoeff ;
    while ( true )
	{
		RayleighFit( (double*)pr, (double*)prMol, glbParam->nBins , "wB"    , "NOTall", (strcFitParam*)&fitParam, (double*)dummy ) ;
		corrCoeff = (double) correlationCoefficient_dbl( (double*)&pr[fitParam.indxInicFit], (double*)&dummy[fitParam.indxInicFit], (int)fitParam.nFit ) ;
		if ( corrCoeff > 0.90 )
			break ;

		i = i+50 ;
		if ( i< round(glbParam->nBins*0.9) )
		{
			fitParam.indxEndFit  = glbParam->nBins - i        				;
			fitParam.indxInicFit = fitParam.indxEndFit - glbParam->nBinsBkg ;
		}
		else
			break;
  	} 
	*indxMaxRange = fitParam.indxEndFit ;

    int indxWL_PDLx ;
	if ( strcmp(glbParam->exeFile, "./lidarAnalysis_PDL1" ) ==0 )
	    ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"indxWL_PDL1", (const char*)"int", (int*)&indxWL_PDLx ) ;
	else if ( strcmp(glbParam->exeFile, "./lidarAnalysis_PDL2" ) ==0 )
	    ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"indxWL_PDL2", (const char*)"int", (int*)&indxWL_PDLx ) ;

	if ( glbParam->chSel== indxWL_PDLx )
	{
		glbParam->indxEndSig_ev[glbParam->evSel] = *indxMaxRange ;
		glbParam->rEndSig_ev[glbParam->evSel] 	 = glbParam->indxEndSig_ev[glbParam->evSel] * glbParam->dr ;
	// printf( "\n ev: %d \t indxMaxRange: %d \t Max. Range: %lf \t corrCoeff: %lf \n", glbParam->evSel, *indxMaxRange, glbParam->rEndSig_ev[glbParam->evSel], corrCoeff ) ;
	}
}

// BIAS SUBSTRACTION METHODS
void CLidar_Operations::bkgSubstraction_Mean( double *sig, strcMolecularData *dataMol, strcGlobalParameters *glbParam, double *pr_noBkg)
{
	int 	indxMaxRange ;
	Find_Max_Range( (double*)sig, (double*)dataMol->prMol, (strcGlobalParameters*)glbParam, (int*)&indxMaxRange ) ;

	if ( indxMaxRange<glbParam->nBins )
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

void CLidar_Operations::bkgSubstraction_MolFit(strcMolecularData *dataMol, const double *prEl, strcGlobalParameters *glbParam, double *pr_noBkg)
{
	int 	indxMaxRange ;
	Find_Max_Range( (double*)prEl, (double*)dataMol->prMol, (strcGlobalParameters*)glbParam, (int*)&indxMaxRange ) ;

	fitParam.indxEndFit  = indxMaxRange ;
	fitParam.indxInicFit = fitParam.indxEndFit - glbParam->nBinsBkg ;
	fitParam.nFit	   	 = fitParam.indxEndFit - fitParam.indxInicFit +1;

	RayleighFit( (double*)prEl, (double*)dataMol->prMol, dataMol->nBins , "wB", "NOTall", (strcFitParam*)&fitParam, (double*)dummy ) ;
		for ( int i=0 ; i<dataMol->nBins ; i++ ) 	pr_noBkg[i] = (double)(prEl[i] - fitParam.b) ; 
}

// void CLidar_Operations::bkgSubstraction_BkgFile( const double *prEl, strcFitParam *fitParam, double **data_Noise, strcGlobalParameters *glbParam, double *pr_noBkg)
// {
// 	// smooth( (double*)&data_Noise[glbParam->chSel][0], (int)0, (int)(glbParam->nBins-1), (int)3, (double*)dummy ) ;
// 	smooth( (double*)&data_Noise[glbParam->chSel][0], (int)0, (int)(glbParam->nBins-1), (int)3, (double*)noiseFit ) ;

// 	// RayleighFit( (double*)prEl, (double*)dummy, glbParam->nBins, "wOutB", "all", (strcFitParam*)fitParam, (double*)noiseFit ) ;

// 	for ( int i=0 ; i<glbParam->nBins ; i++ ) 	pr_noBkg[i] = (double)( prEl[i] - noiseFit[i] ) ; 
// }

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

// BiasCorrection() WITHOUT BACKGROUND NOISE FILE, USING 'MEAN', 'FIT', 'NO_BKG' OR 'AUTO' METHODS TO REMOVE BIAS.
void CLidar_Operations::BiasCorrection( strcLidarSignal *evSig, strcGlobalParameters *glbParam, strcMolecularData *dataMol )
{
	char BkgCorrMethod[10] ;

	ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "BkgCorrMethod", "string" , (char*)BkgCorrMethod ) ;

	if ( (strcmp( BkgCorrMethod, "NO_BKG" ) ==0) || (strcmp( BkgCorrMethod, "no_bkg" ) ==0) )
	{
		for (int i =0; i <glbParam->nBins ; i++)
			evSig->pr_noBias[i] = evSig->pr[i] ;
	}
	else if ( (strcmp( BkgCorrMethod, "MEAN" ) ==0) || (strcmp( BkgCorrMethod, "mean" ) ==0) )
	{
		bkgSubstraction_Mean( (double*)evSig->pr, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (double*)evSig->pr_noBias ) ;
	}
	else if ( (strcmp( BkgCorrMethod, "FIT" ) ==0) || (strcmp( BkgCorrMethod, "fit" ) ==0) )
	{
		bkgSubstraction_MolFit( (strcMolecularData*)dataMol, (const double*)evSig->pr, (strcGlobalParameters*)glbParam, (double*)evSig->pr_noBias ) ;
	}
	else if ( (strcmp( BkgCorrMethod, "AUTO" ) ==0) || (strcmp( BkgCorrMethod, "auto" ) ==0) )
	{
		int 	nStepsAuto ; 
		double 	Bias_Pr ;
		ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "nStepsAuto", "int" , (int*)&nStepsAuto) ;
		bkgSubstraction_Auto( (double*)evSig->pr, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (int)nStepsAuto, (double*)evSig->pr_noBias, (double*)&Bias_Pr ) ;
	}
	else
	{
		printf( "\n BkgCorrMethod = %s but there is no noise information... extracting bias using the mean of the last %d bins.' \n", BkgCorrMethod, glbParam->nBinsBkg ) ;
		bkgSubstraction_Mean( (double*)evSig->pr, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (double*)evSig->pr_noBias ) ;
	}
}

// IF data_noise IS PASSED AS ARGUMENT, IT IS USED
void CLidar_Operations::BiasCorrection( strcLidarSignal *evSig, strcGlobalParameters *glbParam, double **data_Noise, strcMolecularData *dataMol )
{
	char BkgCorrMethod[10] ;

	for (int i =0; i <glbParam->nBins; i++)
		evSig->pr_noBkg[i] = (double)( evSig->pr[i] - data_Noise[glbParam->chSel][i] ) ;

	ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "BkgCorrMethod", "string" , (char*)BkgCorrMethod ) ;

	if ( (strcmp( BkgCorrMethod, "FIT" ) ==0) || (strcmp( BkgCorrMethod, "fit" ) ==0) )
	{
		bkgSubstraction_MolFit( (strcMolecularData*)dataMol, (const double*)evSig->pr_noBkg, (strcGlobalParameters*)glbParam, (double*)evSig->pr_noBias ) ;
	}
	else if ( (strcmp( BkgCorrMethod, "MEAN" ) ==0) || (strcmp( BkgCorrMethod, "mean" ) ==0) )
	{
		bkgSubstraction_Mean( (double*)evSig->pr_noBkg, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (double*)evSig->pr_noBias ) ;
	}
	else if  ( (strcmp( BkgCorrMethod, "AUTO" ) ==0) || (strcmp( BkgCorrMethod, "mean" ) ==0) )
	{
		int 	nStepsAuto ;
		double 	Bias_Pr ;
		ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "nStepsAuto", "int" , (int*)&nStepsAuto) ;
		bkgSubstraction_Auto( (double*)evSig->pr_noBkg, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (int)nStepsAuto, (double*)evSig->pr_noBias, (double*)&Bias_Pr ) ;
	}
}
