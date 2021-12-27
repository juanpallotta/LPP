
// #include "../libLidar/CDataLevel_1.hpp"  
#include "CDataLevel_1.hpp"  

CDataLevel_1::CDataLevel_1( strcGlobalParameters *glbParam )
{
	ReadAnalisysParameter( glbParam->FILE_PARAMETERS, "AVG_CLOUD_DETECTION" , "int"   , (int*)&AVG_CLOUD_DETECTION 	 ) ;
	ReadAnalisysParameter( glbParam->FILE_PARAMETERS, "errFactor"			, "double", (double*)&errFactor    		 ) ;
	ReadAnalisysParameter( glbParam->FILE_PARAMETERS, "CLOUD_MIN_THICK"	  	, "int"   , (int*)&CLOUD_MIN_THICK 		 ) ;
	ReadAnalisysParameter( glbParam->FILE_PARAMETERS, "stepScanCloud" 	  	, "int"   , (int*)&stepScanCloud 		 ) ;
	ReadAnalisysParameter( glbParam->FILE_PARAMETERS, "errScanCheckFactor"  , "double", (double*)&errScanCheckFactor ) ;
	ReadAnalisysParameter( glbParam->FILE_PARAMETERS, "thresholdFactor"	  	, "double", (double*)&thresholdFactor 	 ) ;
	ReadAnalisysParameter( glbParam->FILE_PARAMETERS, "DELTA_RANGE_LIM_BINS", "int"	  , (int*)&DELTA_RANGE_LIM_BINS  ) ;
	ReadAnalisysParameter( glbParam->FILE_PARAMETERS, "errCloudCheckFactor" , "double", (double*)&errCloudCheckFactor) ;
	ReadAnalisysParameter( glbParam->FILE_PARAMETERS, "ifODcut"			  	, "string", (char*)ifODcut 				 ) ;
	ReadAnalisysParameter( glbParam->FILE_PARAMETERS, "nScanMax"			, "int"   , (int*)&nScanMax 			 ) ;
	scanNumExit = nScanMax ;

	prFit  = (double*) new double [glbParam->nBins] ;
	prS    = (double*) new double [glbParam->nBins] ;
	prprm  = (double*) new double [glbParam->nBins] ;
	prmprm = (double*) new double [glbParam->nBins] ;

	mtxCloud_ON	= (double**) new double*[nScanMax] ;
	for (int i=0 ; i<nScanMax ; i++)
		mtxCloud_ON[i] = (double*) new double [glbParam->nBins] ;

	mtxCloud_ON_counts = (int)0 ; // (int*) new int[nBins] ; // 
	GetMem_cloudProfiles( (strcGlobalParameters*)glbParam ) ;
	GetMem_indxMol      ( (strcGlobalParameters*)glbParam ) ;
}
	
CDataLevel_1::~CDataLevel_1()
{
	delete prS   ;
	delete prprm  ;
	delete prmprm ;
	delete prFit ;

	for (int i =0; i <nScanMax; i++) delete mtxCloud_ON[i] ; 
	delete mtxCloud_ON ;
}

void CDataLevel_1::GetMem_cloudProfiles( strcGlobalParameters *glbParam )
{
	cloudProfiles = (strcCloudProfiles*) new strcCloudProfiles [glbParam->nEventsAVG] ;
	for( int e=0 ; e<glbParam->nEventsAVG ; e++ )
	{
		cloudProfiles[e].clouds_ON 	    = (int*) new int [glbParam->nBins]   	 ; memset( cloudProfiles[e].clouds_ON     , 0, ( sizeof(int) * glbParam->nBins) ) ;
		cloudProfiles[e].indxInitClouds = (int*) new int [NMAXCLOUDS]        	 ; memset( cloudProfiles[e].indxInitClouds, 0, ( sizeof(int) * NMAXCLOUDS	    ) ) ;
		cloudProfiles[e].indxEndClouds  = (int*) new int [NMAXCLOUDS]        	 ; memset( cloudProfiles[e].indxEndClouds , 0, ( sizeof(int) * NMAXCLOUDS	    ) ) ;
		cloudProfiles[e].VOD_cloud      = (double*) new double [NMAXCLOUDS]  	 ; memset( cloudProfiles[e].VOD_cloud     , 0, ( sizeof(double) * NMAXCLOUDS  ) ) ;
		cloudProfiles[e].test_1  		= (double*) new double [glbParam->nBins] ; memset( cloudProfiles[e].test_1		  , 0, ( sizeof(double) * glbParam->nBins ) ) ;
		cloudProfiles[e].test_2  		= (double*) new double [glbParam->nBins] ; memset( cloudProfiles[e].test_2		  , 0, ( sizeof(double) * glbParam->nBins ) ) ;
		cloudProfiles[e].nClouds		= (int)0 ;
	}
}

void CDataLevel_1::GetMem_indxMol( strcGlobalParameters *glbParam )
{
	indxMol  = (strcIndexMol*) new strcIndexMol [ glbParam->nEventsAVG ] ;
	for( int e=0 ; e<glbParam->nEventsAVG ; e++ )
	{
		indxMol[e].indxMolON   = (int*)    new int   [glbParam->nBins] ;	memset( indxMol[e].indxMolON  , 0, ( sizeof(int)    * glbParam->nBins ) ) ;
		indxMol[e].errRMS      = (double*) new double[glbParam->nBins] ;	memset( indxMol[e].errRMS     , 0, ( sizeof(double) * glbParam->nBins ) ) ;
		indxMol[e].indxInicMol = (int*)    new int   [MAX_MOL_RANGES]  ;	memset( indxMol[e].indxInicMol, 0, ( sizeof(int)    * MAX_MOL_RANGES  ) ) ;
		indxMol[e].indxEndMol  = (int*)    new int   [MAX_MOL_RANGES]  ;	memset( indxMol[e].indxEndMol , 0, ( sizeof(int)    * MAX_MOL_RANGES  ) ) ;
		indxMol[e].nMolRanges  = (int)0	; // CONTAIN THE NUMBER OF MOLECULAR RANGES DETECTED.
    	indxMol[e].indxRef	   = (int)0 ;
		indxMol[e].ablHeight   = (int)0 ;
		indxMol[e].endRayFit   = (int)0 ;
	}
}

void CDataLevel_1::ScanCloud_RayleightFit ( const double *pr, strcGlobalParameters *glbParam, strcMolecularData *dataMol )
{
	if ( AVG_CLOUD_DETECTION !=0 )
		smooth( (double*)pr, (int)0, (int)(glbParam->nBins-1), (int)AVG_CLOUD_DETECTION, (double*)prS ) ;
	else
	{
		for( int b=0 ; b <glbParam->nBins ; b++ )
			prS[b] 	  = (double)pr[b] ;	
	}
	for( int b=0 ; b <glbParam->nBins ; b++ )
	{
		prprm[b]  = (double)(prS[b] * dataMol->prMol[b]) ;
		prmprm[b] = (double)(dataMol->prMol[b] * dataMol->prMol[b]) ;
		// mtxCloud_ON_counts[b] = (int)0 ;
	}
	mtxCloud_ON_counts = (int)0 ;

 // ERROR REFERENCE CALCULATION
	fitParam.indxEndFit  = glbParam->nBins - 1 ; // glbParam->indxEndSig - AVG_CLOUD_DETECTION ; //  
	fitParam.indxInicFit = fitParam.indxEndFit - glbParam->nBinsBkg ; //glbParam->nBins - 1 - ; //  glbParam->nBins - 1
	fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInicFit ;
		RayleighFit( (double*)prS, (double*)dataMol->prMol, glbParam->nBins, "wB", "NOTall", (strcFitParam*)&fitParam, (double*)prFit ) ;
	biasRef = fitParam.b ;
	// printf("\nm_ref: %lf \t biasRef: %lf ", fitParam.m, biasRef) ;
	errRefBkg = sqrt ( fitParam.sumsq_m/(fitParam.nFit -1) ) ;
	// printf("\nerrRefBkg: %f \t biasRef: %lf (N= %d, fitParam.sumsq_m= %lf) (index fit=%d-%d)\n", errRefBkg, biasRef, fitParam.nFit, fitParam.sumsq_m, fitParam.indxInicFit, fitParam.indxEndFit ) ;
 
	for ( int i=0 ; i<nScanMax ; i++ )
	{ 	// RESET mtxCloud_ON
		for (int k =0; k <glbParam->nBins ; k++)
			mtxCloud_ON[i][k] =(double) 0.0 ;
	}

	fitParam.indxInicFit = (int) glbParam->indxInitSig ;
	fitParam.indxEndFit  = (int) glbParam->indxEndSig  ;
		for ( int i=0 ; i<nScanMax ; i++ ) // ------------------------------------------------------------------------------------------
		{
			for (int b =0; b <glbParam->nBins ; b++)
			{
				cloudProfiles[glbParam->event_analyzed].test_1[b] 	 =(double) 0.0 ;
				cloudProfiles[glbParam->event_analyzed].clouds_ON[b] =(int)    0.0 ;
				prFit[b] 									  	 	 =(double) 0.0 ;
			}
			sppm = (double)0.0 ;	spm = (double)0.0 ;		spmpm = (double)0.0 ;	 m = (double)0.0 ;
			fitParam.nFit = fitParam.indxEndFit - fitParam.indxInicFit ;
			sum( (double*)prprm    		, (int)fitParam.indxInicFit, (int)fitParam.indxEndFit, (double*)&sppm ) ;
			sum( (double*)dataMol->prMol, (int)fitParam.indxInicFit, (int)fitParam.indxEndFit, (double*)&spm  ) ;
			sum( (double*)prmprm		, (int)fitParam.indxInicFit, (int)fitParam.indxEndFit, (double*)&spmpm) ;
				m = (double)((sppm - biasRef * spm) /spmpm) ;

			for( int b=0 ; b <=glbParam->indxEndSig ; b++ ) // for( int b=glbParam->indxInitSig ; b <=glbParam->indxEndSig ; b++ ) // for( int b=fitParam.indxInicFit ; b <=fitParam.indxEndFit ; b++ )
				prFit[b] = m * dataMol->prMol[b] + biasRef ;

			errFitStage = 0 ;
			for( int b=fitParam.indxInicFit ; b <=fitParam.indxEndFit ; b++ )
				errFitStage = (double)( errFitStage + pow( (prFit[b]-prS[b]), 2 ) ) ;
			errFitStage = (double)sqrt( errFitStage /(fitParam.nFit -1) ) ;
			if ( (errFitStage <= (errRefBkg *errScanCheckFactor )) && ( (m*fitParam.m) >0 ) )
			{ // ME FIJO SI ESE FUE EL ULTIMO FIT...
				scanNumExit =i;
				// printf("\n(%d) ScanCloud_RayleightFit(...): SALIENDO *OK* EN nScan = %d --> fit=%d-%d -- errFit / errRefBkg = %f / %f ",glbParam->event_analyzed, i, fitParam.indxInicFit, fitParam.indxEndFit, errFitStage, errRefBkg ) ;
				break ;
			}
			else // SET THE CLOUDS CANDIDATES
			{
				fitParam.indxInicFit = (int)fitParam.indxInicFit + (int)stepScanCloud ;
				if ( m>0 ) // IF THE FIT IS OK
				{
					// SET THE CLOUDS/PLUMES CANDIDATES
					for( int b=glbParam->indxInitSig ; b<=glbParam->indxEndSig ; b++ )
					{
						if( prS[b] > ( prFit[b] + errRefBkg *errFactor ) )
							mtxCloud_ON[i][b] = (double)pow( (prS[b] - prFit[b] - errRefBkg *errFactor), 2) ;
					}
					// REMOVE THE FIRST CLUSTER OF "BIN_CLOUD" IN mtxCloud_ON[i][:] VALUES (ABL BINS)
					first_cluster_ON =0 ;
					// if ( mtxCloud_ON[i][glbParam->indxInitSig] >0 )
					// {
						for( int b=(glbParam->indxInitSig) ; b<=glbParam->indxEndSig ; b++ )
						{
							if ( (mtxCloud_ON[i][b] >(double)0) )
							{
								mtxCloud_ON[i][b] =(double)0.0 ; // ELIMINATE THE BINS OF THE ABL.
								first_cluster_ON  =(double)1   ;
							}
							else
							{
								if ( first_cluster_ON ==1 )
									break ; // mtxCloud_ON[i][b] ==0 AND THE BINS WERE ALREADY RESETED.
							}
						}
					// }
					for( int b=glbParam->indxInitSig ; b<=glbParam->indxEndSig ; b++ )
					{ // INCREASE THE COUNTER IF THERE IS A CLOUD DETECTED
						if( mtxCloud_ON[i][b] > 0 )
						{
							mtxCloud_ON_counts++ ;
							break ;
						}
					}
				}
				else
				{
					if ( fitParam.indxInicFit >= fitParam.indxEndFit )
						break ;
				}
			}  // END OF "SET THE CLOUDS CANDIDATES"
		} // for ( int i=0 ; i<nScanMax ; i++ ) ------------------------------------------------------------------------------------------

		if (mtxCloud_ON_counts == 0)
			mtxCloud_ON_counts = 1 ;

		for( int b=0 ; b <glbParam->nBins ; b++ )
		{
			for( int s=0 ; s <nScanMax ; s++ )
				cloudProfiles[glbParam->event_analyzed].test_1[b] = (double)(cloudProfiles[glbParam->event_analyzed].test_1[b] + (double)mtxCloud_ON[s][b]) ; // test1: SUM OF mtxCloud_ON
			// cloudProfiles[glbParam->event_analyzed].test_1[b] = (double)sqrt( cloudProfiles[glbParam->event_analyzed].test_1[b] /mtxCloud_ON_counts[b] ) ; 
			      cloudProfiles[glbParam->event_analyzed].test_1[b] = (double)sqrt( cloudProfiles[glbParam->event_analyzed].test_1[b] /mtxCloud_ON_counts ) ;
			// cloudProfiles[glbParam->event_analyzed].test_1[b]    = (double)    ( cloudProfiles[glbParam->event_analyzed].test_1[b] /mtxCloud_ON_counts ) ;
		}

		for( int b=0 ; b <glbParam->nBins ; b++ )
		{
			if ( cloudProfiles[glbParam->event_analyzed].test_1[b] > (double)(errRefBkg *thresholdFactor) )
				cloudProfiles[glbParam->event_analyzed].clouds_ON[b] = (int)BIN_CLOUD ;
			else
				cloudProfiles[glbParam->event_analyzed].clouds_ON[b] = (int)0 ;
		}
		//// CHECK CLOUDS TESTS:
		//// REMOVE CLOUDS THINNER THAN CLOUD_MIN_THICK BINS.
		for (int b =0 ; b <=(glbParam->indxEndSig -CLOUD_MIN_THICK -1) ; b++)
		{
			if ( (cloudProfiles[glbParam->event_analyzed].clouds_ON[b] == (int)0) && (cloudProfiles[glbParam->event_analyzed].clouds_ON[b+1] == (int)BIN_CLOUD) ) // IF A CLOUD START...
			{
				int sum_misc =0 ;
				for (int i =(b+1) ; i <=(b+1+CLOUD_MIN_THICK); i++)
					sum_misc = sum_misc + cloudProfiles[glbParam->event_analyzed].clouds_ON[i];
				if ( sum_misc < (CLOUD_MIN_THICK*BIN_CLOUD) )
				{
					for (int k =(b+1) ; k <=(b+1+CLOUD_MIN_THICK); k++)
						cloudProfiles[glbParam->event_analyzed].clouds_ON[k] = (int)0 ;
				}
				b = b +CLOUD_MIN_THICK ;
			}
		}
		// REMOVE CLOUDS'S GAPS THINNER THAN CLOUD_MIN_THICK BINS.
		for (int b =0 ; b <=(glbParam->indxEndSig -CLOUD_MIN_THICK -1) ; b++)
		{
			if ( (cloudProfiles[glbParam->event_analyzed].clouds_ON[b] == (int)BIN_CLOUD) && (cloudProfiles[glbParam->event_analyzed].clouds_ON[b+1] == (int)0) ) // IF A CLOUD ENDS...
			{
				sum_misc =0 ;
				for (int i =(b+1) ; i <=(b+1+CLOUD_MIN_THICK); i++)
					sum_misc = sum_misc + cloudProfiles[glbParam->event_analyzed].clouds_ON[i];
				if ( sum_misc >0 )
				{
					for (int k =(b+1) ; k <=(b+1+CLOUD_MIN_THICK); k++)
						cloudProfiles[glbParam->event_analyzed].clouds_ON[k] = (int)BIN_CLOUD ;
				}
				b = b +CLOUD_MIN_THICK ;
			}
		}
		cloudProfiles[glbParam->event_analyzed].clouds_ON[0] 				    = (int)0 ; // NO -> THE ABL IS CONSIDERED AS CLOUD. FIRST BIN IS SET TO ZERO BE COHERENT FOR THE INDEXES COMPUTATION.
		cloudProfiles[glbParam->event_analyzed].clouds_ON[glbParam->indxEndSig] = (int)0 ; // LAST BIN IS SET TO ONE BE COHERENT FOR THE INDEXES COMPUTATION.

// DONE WITH cloudProfile, NOW GET THE CLOUD LIMITS
	GetCloudLimits( (strcGlobalParameters*)glbParam, (strcMolecularData*)dataMol ) ;

		for( int i=0 ; i <cloudProfiles[glbParam->event_analyzed].nClouds ; i++ )
		{
			fitParam.indxInicFit = cloudProfiles[glbParam->event_analyzed].indxInitClouds[i] -DELTA_RANGE_LIM_BINS ; // cloudProfiles[glbParam->event_analyzed].indxInitClouds[0] 					   -DELTA_RANGE_LIM_BINS ; // 
			fitParam.indxEndFit  = cloudProfiles[glbParam->event_analyzed].indxEndClouds[i]  +DELTA_RANGE_LIM_BINS ; // cloudProfiles[glbParam->event_analyzed].indxEndClouds[cloudProfiles[glbParam->event_analyzed].nClouds-1]  +DELTA_RANGE_LIM_BINS ; // 
			fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInicFit ;

			if( fitParam.indxInicFit > glbParam->indxInitSig )
			{
					RayleighFit( (double*)prS, (double*)dataMol->prMol, glbParam->nBins, "wB", "NOTall", (strcFitParam*)&fitParam, (double*)prFit ) ;	
				errCloud =0 ;
				// errCloud = (double)sqrt(fitParam.sumsq_m/(fitParam.nFit -1)) ; // (errCloud/fitParam.nFit) ;
				errCloud = (double)(fitParam.sumsq_m/(fitParam.nFit -1)) ;

				if ( errCloud <= (errRefBkg *errCloudCheckFactor) )
				{
					for( int b =(fitParam.indxInicFit) ; b <=(fitParam.indxEndFit) ; b++ ) // for( int b =(fitParam.indxInicFit-2) ; b <=(fitParam.indxEndFit+2) ; b++ ) // 
						cloudProfiles[glbParam->event_analyzed].clouds_ON[b] =0;
				}
				// else
				// 	printf("SC_RF() - (%d) Clouds: errRefBkg: %2.3lf \t errCloud: %2.3lf --> Cloud: %d-%d --> Fit: %d-%d\n", glbParam->event_analyzed, errRefBkg, errCloud, cloudProfiles[glbParam->event_analyzed].indxInitClouds[i], cloudProfiles[glbParam->event_analyzed].indxEndClouds[i], cloudProfiles[glbParam->event_analyzed].indxInitClouds[i] -DELTA_RANGE_LIM_BINS, cloudProfiles[glbParam->event_analyzed].indxEndClouds[i]  +DELTA_RANGE_LIM_BINS) ;
			
				if ( strcmp(ifODcut, "YES") ==0 )
					ODcut( (double*)prS, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (strcFitParam*)&fitParam, (int*)cloudProfiles[glbParam->event_analyzed].clouds_ON ) ;
			}
		}
		// if (cloudProfiles[glbParam->event_analyzed].nClouds ==0)
		// 	printf("\nSC_RF() - (%d) NO Clouds detected\n", glbParam->event_analyzed) ;
		
		GetCloudLimits( (strcGlobalParameters*)glbParam, (strcMolecularData*)dataMol ) ;
}

void CDataLevel_1::GetCloudLimits( strcGlobalParameters *glbParam, strcMolecularData *dataMol )
{
	double *dco   = (double*) new double[glbParam->nBins] ;
	memset( (double*)dco  , 0, (sizeof(double) * glbParam->nBins) ) ;
	for ( int i = 0 ; i<=glbParam->indxEndSig ; i++ )
		dco[i] = (double)( cloudProfiles[glbParam->event_analyzed].clouds_ON[i+1] - cloudProfiles[glbParam->event_analyzed].clouds_ON[i] ) ;

	int nInicCloud=0, nEndCloud=0 ;
	for (int i =0; i <=glbParam->indxEndSig; i++)
	{
		if ( dco[i] ==(double)BIN_CLOUD )
			nInicCloud++ ;
		else if ( dco[i]==(double)(-BIN_CLOUD) )
			nEndCloud++ ;
	}
	if ( nInicCloud == nEndCloud )
	{
		memset( cloudProfiles[glbParam->event_analyzed].indxInitClouds, 0, ( sizeof(int) * NMAXCLOUDS ) ) ;
		memset( cloudProfiles[glbParam->event_analyzed].indxEndClouds , 0, ( sizeof(int) * NMAXCLOUDS ) ) ;
		memset( indxMol[glbParam->event_analyzed].indxInicMol, 0, ( sizeof(int) * MAX_MOL_RANGES  ) ) ;
		memset( indxMol[glbParam->event_analyzed].indxEndMol , 0, ( sizeof(int) * MAX_MOL_RANGES  ) ) ;
		cloudProfiles[glbParam->event_analyzed].nClouds  = 0 ; // USED AS INDEX AND THEN, AS TOTAL NUMBER.
		indxMol[glbParam->event_analyzed].nMolRanges 	= 0 ; // USED AS INDEX AND THEN, AS TOTAL NUMBER.

		//CLOUDS DETECTION
		for( int i=0 ; i <=(glbParam->indxEndSig-1) ; i++ )
		{
			if( dco[i] == (double)BIN_CLOUD ) // INIT CLOUD
			{
				cloudProfiles[glbParam->event_analyzed].indxInitClouds[cloudProfiles[glbParam->event_analyzed].nClouds] = i ;
				for (int j =i ; j <=(glbParam->indxEndSig-1) ; j++)
				{	// SEARCH THE END OF THE CLOUD
					if( dco[j] == (double)(-BIN_CLOUD) ) // END CLOUD
					{
						cloudProfiles[glbParam->event_analyzed].indxEndClouds[cloudProfiles[glbParam->event_analyzed].nClouds] = j -1;
						cloudProfiles[glbParam->event_analyzed].nClouds++ ; // COUNT CLOUDS
						break ;
					}
				}
			}
		}
		// MOLECULAR RANGES DETECTION
		indxMol[glbParam->event_analyzed].nMolRanges = cloudProfiles[glbParam->event_analyzed].nClouds +1 ;
		if ( indxMol[glbParam->event_analyzed].nMolRanges ==1 )
		{
			int 	heightRef_Inversion_ASL ;
			ReadAnalisysParameter( glbParam->FILE_PARAMETERS, "heightRef_Inversion_ASL" , "int" , (int*)&heightRef_Inversion_ASL ) ;
			indxMol[glbParam->event_analyzed].indxRef = (int)round(heightRef_Inversion_ASL /dataMol->dzr) ;
				if ( indxMol[glbParam->event_analyzed].indxRef > (glbParam->nBins-1) )
					indxMol[glbParam->event_analyzed].indxRef = glbParam->nBins -10 ;

			for (int i =0; i < NMAXCLOUDS ; i++)
			{
				cloudProfiles[glbParam->event_analyzed].indxInitClouds[i] = 0 ;
				cloudProfiles[glbParam->event_analyzed].indxEndClouds [i] = 0 ;
				indxMol[glbParam->event_analyzed].indxInicMol[i] = 0 ;
				indxMol[glbParam->event_analyzed].indxEndMol[i]  = glbParam->indxEndSig  ;
			}
		}
		else
		{
			for( int i=0 ; i <indxMol[glbParam->event_analyzed].nMolRanges ; i++ )
			{
				if (i==0)
				{
					indxMol[glbParam->event_analyzed].indxInicMol[i] = 0 ;
					indxMol[glbParam->event_analyzed].indxEndMol[i]  = cloudProfiles[glbParam->event_analyzed].indxInitClouds[i] -1  ;
				}
				else
				{
					indxMol[glbParam->event_analyzed].indxInicMol[i] = cloudProfiles[glbParam->event_analyzed].indxEndClouds[i-1] +1 ;
					if ( i == (indxMol[glbParam->event_analyzed].nMolRanges-1) )
						indxMol[glbParam->event_analyzed].indxEndMol[i]  = glbParam->indxEndSig ;
					else
						indxMol[glbParam->event_analyzed].indxEndMol[i]  = cloudProfiles[glbParam->event_analyzed].indxInitClouds[i] -1 ;
				}
			}
		}
	}
	else
		printf("\nGetCloudLim(...) --> error\n") ;

	delete dco   ;
}

void CDataLevel_1::GetCloudLimits_v1( strcGlobalParameters *glbParam )
{
	double *dco   = (double*) new double[glbParam->nBins] ;
	memset( (double*)dco  , 0, (sizeof(double) * glbParam->nBins) ) ;
	for ( int i = 0 ; i<=glbParam->indxEndSig ; i++ )
		dco[i] = (double)( cloudProfiles[glbParam->event_analyzed].clouds_ON[i+1] - cloudProfiles[glbParam->event_analyzed].clouds_ON[i] ) ;

	int nInicCloud=0, nEndCloud=0 ;
	for (int i =0; i <=glbParam->indxEndSig; i++)
	{
		if ( dco[i] ==(double)BIN_CLOUD )
			nInicCloud++ ;
		else if ( dco[i]==(double)(-BIN_CLOUD) )
			nEndCloud++ ;
	}
	if ( nInicCloud == nEndCloud )
	{
		memset( cloudProfiles[glbParam->event_analyzed].indxInitClouds, 0, ( sizeof(int) * NMAXCLOUDS ) ) ;
		memset( cloudProfiles[glbParam->event_analyzed].indxEndClouds , 0, ( sizeof(int) * NMAXCLOUDS ) ) ;
		memset( indxMol[glbParam->event_analyzed].indxInicMol, 0, ( sizeof(int) * MAX_MOL_RANGES  ) ) ;
		memset( indxMol[glbParam->event_analyzed].indxEndMol , 0, ( sizeof(int) * MAX_MOL_RANGES  ) ) ;
		cloudProfiles[glbParam->event_analyzed].nClouds  = 0 ; // USED AS INDEX AND THEN, AS TOTAL NUMBER.
		indxMol[glbParam->event_analyzed].nMolRanges 	 = 0 ; // USED AS INDEX AND THEN, AS TOTAL NUMBER.

		//CLOUDS DETECTION
		for( int i=0 ; i <=(glbParam->indxEndSig-1) ; i++ )
		{
			if( dco[i] == (double)BIN_CLOUD ) // INIT CLOUD
			{
				cloudProfiles[glbParam->event_analyzed].indxInitClouds[cloudProfiles[glbParam->event_analyzed].nClouds] = i ;
				for (int j =i ; j <=(glbParam->indxEndSig-1) ; j++)
				{	// SEARCH THE END OF THE CLOUD
					if( dco[j] == (double)(-BIN_CLOUD) ) // END CLOUD
					{
						cloudProfiles[glbParam->event_analyzed].indxEndClouds[cloudProfiles[glbParam->event_analyzed].nClouds] = j -1;
						cloudProfiles[glbParam->event_analyzed].nClouds++ ; // COUNT CLOUDS
						break ;
					}
				}
			}
		}
		// MOLECULAR RANGES DETECTION
		indxMol[glbParam->event_analyzed].nMolRanges = cloudProfiles[glbParam->event_analyzed].nClouds +1 ;
		if ( indxMol[glbParam->event_analyzed].nMolRanges ==1 )
		{
			int 	heightRef_Inversion_ASL ;
			ReadAnalisysParameter( glbParam->FILE_PARAMETERS, "heightRef_Inversion_ASL" , "int" , (int*)&heightRef_Inversion_ASL ) ;
			indxMol[glbParam->event_analyzed].indxRef = (int)round(heightRef_Inversion_ASL /glbParam->dzr) ;
				if ( indxMol[glbParam->event_analyzed].indxRef > (glbParam->nBins-1) )
					indxMol[glbParam->event_analyzed].indxRef = glbParam->nBins -10 ;

			for (int i =0; i < NMAXCLOUDS ; i++)
			{
				cloudProfiles[glbParam->event_analyzed].indxInitClouds[i] = 0 ;
				cloudProfiles[glbParam->event_analyzed].indxEndClouds [i] = 0 ;
				indxMol[glbParam->event_analyzed].indxInicMol[i] = 0 ;
				indxMol[glbParam->event_analyzed].indxEndMol[i]  = glbParam->indxEndSig  ;
			}
		}
		else
		{
			for( int i=0 ; i <indxMol[glbParam->event_analyzed].nMolRanges ; i++ )
			{
				if (i==0)
				{
					indxMol[glbParam->event_analyzed].indxInicMol[i] = 0 ;
					indxMol[glbParam->event_analyzed].indxEndMol[i]  = cloudProfiles[glbParam->event_analyzed].indxInitClouds[i] -1  ;
				}
				else
				{
					indxMol[glbParam->event_analyzed].indxInicMol[i] = cloudProfiles[glbParam->event_analyzed].indxEndClouds[i-1] +1 ;
					if ( i == (indxMol[glbParam->event_analyzed].nMolRanges-1) )
						indxMol[glbParam->event_analyzed].indxEndMol[i]  = glbParam->indxEndSig ;
					else
						indxMol[glbParam->event_analyzed].indxEndMol[i]  = cloudProfiles[glbParam->event_analyzed].indxInitClouds[i] -1 ;
				}
			}
		}
	}
	else
		printf("\nGetCloudLim(...) --> error\n") ;

	delete dco   ;
}

void CDataLevel_1::MakeRangeCorrected( strcLidarSignal *evSig, strcGlobalParameters *glbParam, strcMolecularData *dataMol )
{
	char BkgCorrMethod[10] ;

	fitParam.indxInicFit = glbParam->nBins - glbParam->nBinsBkg ;
	fitParam.indxEndFit  = glbParam->nBins -1 ;
	fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInicFit ;

	// cout << endl ; // <<"------ ¡¡¡¡PUEST A MANO!!!!!-------------- MakeRangeCorrected -> e: " << glbParam->event_analyzed ;
	// printf("\nCDataLevel_1::MakeRangeCorrected() --> fitParam.indxInicFit: %d\nfitParam.indxEndFit: %d\nfitParam.nFit: %d", fitParam.indxInicFit, fitParam.indxEndFit, fitParam.nFit) ;

	ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "BkgCorrMethod", "string" , (char*)BkgCorrMethod ) ;

	if ( (strcmp( BkgCorrMethod, "MEAN" ) ==0) || (strcmp( BkgCorrMethod, "mean" ) ==0) )
		bkgSubstractionMean( (double*)evSig->pr, fitParam.indxInicFit, fitParam.indxEndFit, glbParam->nBins, (double*)evSig->pr_noBkg ) ;
	else if ( (strcmp( BkgCorrMethod, "FIT" ) ==0) || (strcmp( BkgCorrMethod, "fit" ) ==0) )
		bkgSubstractionMolFit( (strcMolecularData*)dataMol, (const double*)evSig->pr, (strcFitParam*)&fitParam, (double*)evSig->pr_noBkg ) ;
	else
	{
		printf("\n Wrong setting of 'Background Correction Method (BkgCorrMethod) in %s. Used FIT method' \n", (char*)glbParam->FILE_PARAMETERS) ;
		bkgSubstractionMolFit ( (strcMolecularData*)dataMol, (double*)evSig->pr, (strcFitParam*)&fitParam, (double*)evSig->pr_noBkg ) ;
	}
	// RANGE CORRECTED
	for ( int i=0 ; i<glbParam->nBins ; i++ ) 	evSig->pr2[i] = evSig->pr_noBkg[i] * pow(glbParam->r[i], 2) ;
}


