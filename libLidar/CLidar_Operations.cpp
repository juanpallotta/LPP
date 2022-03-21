
#include "CLidar_Operations.hpp"

CLidar_Operations::CLidar_Operations( strcGlobalParameters *glbParam )
{
	noiseFit = (double*) new double[glbParam->nBins] ;
	dummy 	 = (double*) new double[glbParam->nBins] ; 
}

CLidar_Operations::~CLidar_Operations()
{
	delete noiseFit ;
	delete dummy 	;
}

// MakeRangeCorrected() USING 'MEAN' OR 'FIT' METHODS TO REMOVE BIAS.
void CLidar_Operations::MakeRangeCorrected( strcLidarSignal *evSig, strcGlobalParameters *glbParam, strcMolecularData *dataMol )
{
	char BkgCorrMethod[10] ;

	fitParam.indxInicFit = glbParam->nBins - glbParam->nBinsBkg ;
	fitParam.indxEndFit  = glbParam->nBins -1 ;
	fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInicFit ;

	ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "BkgCorrMethod", "string" , (char*)BkgCorrMethod ) ;

	if ( (strcmp( BkgCorrMethod, "MEAN" ) ==0) || (strcmp( BkgCorrMethod, "mean" ) ==0) )
		bkgSubstraction_Mean( (double*)evSig->pr, (strcFitParam*)&fitParam, (strcGlobalParameters*)glbParam, (double*)evSig->pr_noBkg ) ;
	else if ( (strcmp( BkgCorrMethod, "FIT" ) ==0) || (strcmp( BkgCorrMethod, "fit" ) ==0) )
		bkgSubstraction_MolFit( (strcMolecularData*)dataMol, (const double*)evSig->pr, (strcFitParam*)&fitParam, (double*)evSig->pr_noBkg ) ;
	else
	{
		printf( "\n BkgCorrMethod = %s but there is no noise information... extracting bias using the mean of the last %d bins.' \n", BkgCorrMethod, glbParam->nBinsBkg ) ;
		bkgSubstraction_Mean( (double*)evSig->pr, (strcFitParam*)&fitParam, (strcGlobalParameters*)glbParam, (double*)evSig->pr_noBkg ) ;
	}
	// RANGE CORRECTED
	for ( int i=0 ; i<glbParam->nBins ; i++ ) 	evSig->pr2[i] = evSig->pr_noBkg[i] * pow(glbParam->r[i], 2) ;
}

// MakeRangeCorrected() USING 'MEAN' METHODS TO REMOVE BIAS.
void CLidar_Operations::MakeRangeCorrected( strcLidarSignal *evSig, strcGlobalParameters *glbParam )
{
	char BkgCorrMethod[10] ;

	fitParam.indxInicFit = glbParam->nBins - glbParam->nBinsBkg ;
	fitParam.indxEndFit  = glbParam->nBins -1 ;
	fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInicFit ;

	ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "BkgCorrMethod", "string" , (char*)BkgCorrMethod ) ;

	if ( (strcmp( BkgCorrMethod, "MEAN" ) !=0) && (strcmp( BkgCorrMethod, "mean" ) !=0) )
		printf( "\nBkgCorrMethod = %s set in %s\n But only MEAN method can be applied. Extracting bias using the mean of the last %d bins.' \n", BkgCorrMethod, glbParam->FILE_PARAMETERS, glbParam->nBinsBkg ) ;

	bkgSubstraction_Mean( (double*)evSig->pr, (strcFitParam*)&fitParam, (strcGlobalParameters*)glbParam, (double*)evSig->pr_noBkg ) ;

	// RANGE CORRECTED
	for ( int i=0 ; i<glbParam->nBins ; i++ ) 	evSig->pr2[i] = evSig->pr_noBkg[i] * pow(glbParam->r[i], 2) ;
}

// MakeRangeCorrected() USING 'FILE_BKG' OR 'MEAN' METHODS TO REMOVE BIAS.
void CLidar_Operations::MakeRangeCorrected( strcLidarSignal *evSig, strcGlobalParameters *glbParam, double **data_Noise )
{
	char BkgCorrMethod[10] ;

	fitParam.indxInicFit = glbParam->nBins - glbParam->nBinsBkg ;
	fitParam.indxEndFit  = glbParam->nBins -1 ;
	fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInicFit ;

	ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "BkgCorrMethod", "string" , (char*)BkgCorrMethod ) ;

	if ( (strcmp( BkgCorrMethod, "FILE_BKG" ) ==0) || (strcmp( BkgCorrMethod, "file_bkg" ) ==0) )
		bkgSubstraction_BkgFile( (const double*)evSig->pr, (strcFitParam*)&fitParam, (double**)data_Noise, (strcGlobalParameters*)glbParam, (double*)evSig->pr_noBkg ) ;
	else
	{
		printf( "\nBkgCorrMethod = %s \nBUT there is noise information available.\nIt is highly recommended to use BkgCorrMethod = FILE_BKG in configuration file %s.' \n", BkgCorrMethod, glbParam->FILE_PARAMETERS ) ;
		bkgSubstraction_Mean( (double*)evSig->pr, (strcFitParam*)&fitParam, (strcGlobalParameters*)glbParam, (double*)evSig->pr_noBkg ) ;
	}

	// RANGE CORRECTED
	for ( int i=0 ; i<glbParam->nBins ; i++ ) 	evSig->pr2[i] = evSig->pr_noBkg[i] * pow(glbParam->r[i], 2) ;
}

// BIAS SUBSTRACTION METHODS
void CLidar_Operations::bkgSubstraction_Mean( double *sig, strcFitParam *fitParam, strcGlobalParameters *glbParam, double *pr_noBkg)
{
	double 	bkgMean = 0 ;

	for( int j=fitParam->indxInicFit ; j<fitParam->indxInicFit ; j++ ) bkgMean = bkgMean + sig[j] ;

	bkgMean = bkgMean /fitParam->nFit ;

	for ( int i=0 ; i<glbParam->nBins ; i++ ) 	pr_noBkg[i] = (double)(sig[i] - bkgMean) ;
}

void CLidar_Operations::bkgSubstraction_MolFit(strcMolecularData *dataMol, const double *prEl, strcFitParam *fitParam, double *pr_noBkg)
{
	RayleighFit( (double*)prEl, (double*)dataMol->prMol, dataMol->nBins , "wB", "NOTall", (strcFitParam*)fitParam, (double*)dummy ) ;
		for ( int i=0 ; i<dataMol->nBins ; i++ ) 	pr_noBkg[i] = (double)(prEl[i] - fitParam->b) ; 
}

void CLidar_Operations::bkgSubstraction_BkgFile( const double *prEl, strcFitParam *fitParam, double **data_Noise, strcGlobalParameters *glbParam, double *pr_noBkg)
{
	smooth( (double*)&data_Noise[glbParam->chSel][0], (int)0, (int)(glbParam->nBins-1), (int)3, (double*)dummy ) ;

	RayleighFit( (double*)prEl, (double*)dummy, glbParam->nBins, "wOutB", "all", (strcFitParam*)fitParam, (double*)noiseFit ) ;

	for ( int i=0 ; i<glbParam->nBins ; i++ ) 	pr_noBkg[i] = (double)( prEl[i] - noiseFit[i] ) ; 
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
							Raw_Data_Start_Time_AVG[fC]	  = (int)Raw_Data_Start_Time[ fC *glbParam->numEventsToAvg ] ;
						if ( t ==(glbParam->numEventsToAvg -1) )
							Raw_Data_Stop_Time_AVG[fC] 	  = (int)Raw_Data_Stop_Time[ fC *glbParam->numEventsToAvg +t ] ;

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
