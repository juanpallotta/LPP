
#include "CDataLevel_1.hpp"  

CDataLevel_1::CDataLevel_1( strcGlobalParameters *glbParam )
{
	ReadAnalisysParameter( glbParam->FILE_PARAMETERS, "avg_Points_Cloud_Mask" , "int"   , (int*)&avg_Points_Cloud_Mask 	 ) ;
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

	SE_lay	= (double**) new double*[nScanMax] ;
	for (int i=0 ; i<nScanMax ; i++)
	{
		SE_lay[i] = (double*) new double [glbParam->nBins] ;
		memset( SE_lay[i], 0, ( sizeof(double) * glbParam->nBins) ) ;
	}

	oLOp = (CLidar_Operations*) new CLidar_Operations( (strcGlobalParameters*)glbParam ) ;

	nMaxLoop = (int)0 ;
	GetMem_cloudProfiles( (strcGlobalParameters*)glbParam ) ;
	GetMem_indxMol      ( (strcGlobalParameters*)glbParam ) ;
}
	
CDataLevel_1::~CDataLevel_1()
{
	delete prS   ;
	delete prprm  ;
	delete prmprm ;
	delete prFit ;

	for (int i =0; i <nScanMax; i++) delete SE_lay[i] ; 
	delete SE_lay ;
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
	if ( avg_Points_Cloud_Mask !=0 )
		smooth( (double*)pr, (int)0, (int)(glbParam->nBins-1), (int)avg_Points_Cloud_Mask, (double*)prS ) ;
	else
	{
		for( int b=0 ; b <glbParam->nBins ; b++ )
			prS[b] 	  = (double)pr[b] ;	
	}
	for( int b=0 ; b <glbParam->nBins ; b++ )
	{
		prprm[b]  = (double)(prS[b] * dataMol->prMol[b]) ;
		prmprm[b] = (double)(dataMol->prMol[b] * dataMol->prMol[b]) ;
	}
	nMaxLoop = (int)0 ;

 // ERROR REFERENCE CALCULATION
	fitParam.indxEndFit  = glbParam->nBins - 1 ; // glbParam->indxEndSig - avg_Points_Cloud_Mask ; //  
	fitParam.indxInicFit = fitParam.indxEndFit - glbParam->nBinsBkg ; //glbParam->nBins - 1 - ; //  glbParam->nBins - 1
	fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInicFit ;
		RayleighFit( (double*)prS, (double*)dataMol->prMol, glbParam->nBins, "wB", "NOTall", (strcFitParam*)&fitParam, (double*)prFit ) ;
	biasRef = fitParam.b ;
	errRefBkg = sqrt ( fitParam.sumsq_m/(fitParam.nFit -1) ) ;
	// printf("\nm_ref: %lf \t biasRef: %lf \t errRefBkg: %lf \t fitParam.sumsq_m: %lf", fitParam.m, biasRef, errRefBkg, fitParam.sumsq_m) ;
	// printf("\nindex fit=%d-%d\n", fitParam.indxInicFit, fitParam.indxEndFit ) ;
 
	for ( int i=0 ; i<nScanMax ; i++ )
	{ 	// RESET SE_lay
		for (int k =0; k <glbParam->nBins ; k++)
			SE_lay[i][k] =(double) 0.0 ;
	}

	fitParam.indxInicFit = (int) glbParam->indxInitSig ;
	fitParam.indxEndFit  = (int) glbParam->indxEndSig  ;
		for ( int i=0 ; i<nScanMax ; i++ ) // ------------------------------------------------------------------------------------------
		{
			for (int b =0; b <glbParam->nBins ; b++)
			{
				cloudProfiles[glbParam->evSel].test_1[b] 	 =(double) 0.0 ;
				cloudProfiles[glbParam->evSel].clouds_ON[b] =(int)    0.0 ;
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
				// printf("\n(%d) ScanCloud_RayleightFit(...): SALIENDO *OK* EN nScan = %d --> fit=%d-%d -- errFit / errRefBkg = %f / %f ",glbParam->evSel, i, fitParam.indxInicFit, fitParam.indxEndFit, errFitStage, errRefBkg ) ;
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
							SE_lay[i][b] = (double)pow( (prS[b] - prFit[b] - errRefBkg *errFactor), 2) ; // SE_lay = D(i, r) --> SE_lay(i, r)  IN THE PAPER
					}
					// REMOVE THE FIRST CLUSTER OF "BIN_CLOUD" IN SE_lay[i][:] VALUES (ABL BINS)
					first_cluster_ON =0 ;
					for( int b=(glbParam->indxInitSig) ; b<=glbParam->indxEndSig ; b++ )
					{
						if ( (SE_lay[i][b] >(double)0) )	{
							SE_lay[i][b] =(double)0.0 ; // ELIMINATE THE BINS OF THE ABL.
							first_cluster_ON  =(double)1   ;	}
						else 			{
							if ( first_cluster_ON ==1 ) // SE_lay[i][b] ==0 AND THE BINS WERE ALREADY RESETED.
								break ; }
					}
					// INCREASE THE COUNTER IF THERE IS A CLOUD DETECTED
					for( int b=glbParam->indxInitSig ; b<=glbParam->indxEndSig ; b++ ) {
						if( SE_lay[i][b] > 0 ) {
							nMaxLoop++ ;
							break ;   				}						    	   }
				}
				else {
					if ( fitParam.indxInicFit >= fitParam.indxEndFit )
						break ; }
			}  // END OF "SET THE CLOUDS CANDIDATES"
		} // for ( int i=0 ; i<nScanMax ; i++ ) ------------------------------------------------------------------------------------------

		if (nMaxLoop == 0)
		{
			printf("\nCompletely pure Rayleigh lidar signal\n") ;
			nMaxLoop = 1 ;
		}

		for( int b=0 ; b <glbParam->nBins ; b++ )
		{
			for( int s=0 ; s <nScanMax ; s++ )
				cloudProfiles[glbParam->evSel].test_1[b] = (double)(cloudProfiles[glbParam->evSel].test_1[b] + (double)SE_lay[s][b]) ;
			// cloudProfiles[glbParam->evSel].test_1[b] = (double)sqrt( cloudProfiles[glbParam->evSel].test_1[b] /SE_lay_counts[b] ) ; 
				cloudProfiles[glbParam->evSel].test_1[b] = (double)sqrt( cloudProfiles[glbParam->evSel].test_1[b] /nMaxLoop )   ; // test1: RMSE_lay
			// cloudProfiles[glbParam->evSel].test_1[b]    = (double)    ( cloudProfiles[glbParam->evSel].test_1[b] /nMaxLoop ) ;
		}

// printf("\n\n ScanCloud \n") ;
// printf("\n nMaxLoop: %d \n\n", nMaxLoop) ;

		for( int b=0 ; b <glbParam->nBins ; b++ )
		{
			if ( cloudProfiles[glbParam->evSel].test_1[b] > (double)(errRefBkg *thresholdFactor) )
				cloudProfiles[glbParam->evSel].clouds_ON[b] = (int)BIN_CLOUD ;
			else
				cloudProfiles[glbParam->evSel].clouds_ON[b] = (int)0 ;
		}
		//// CHECK CLOUDS TESTS:
		//// REMOVE CLOUDS THINNER THAN CLOUD_MIN_THICK BINS.
		for (int b =0 ; b <=(glbParam->indxEndSig -CLOUD_MIN_THICK -1) ; b++)
		{
			if ( (cloudProfiles[glbParam->evSel].clouds_ON[b] == (int)0) && (cloudProfiles[glbParam->evSel].clouds_ON[b+1] == (int)BIN_CLOUD) ) // IF A CLOUD START...
			{
				int sum_misc =0 ;
				for (int i =(b+1) ; i <=(b+1+CLOUD_MIN_THICK); i++)
					sum_misc = sum_misc + cloudProfiles[glbParam->evSel].clouds_ON[i];
				if ( sum_misc < (CLOUD_MIN_THICK*BIN_CLOUD) )
				{
					for (int k =(b+1) ; k <=(b+1+CLOUD_MIN_THICK); k++)
						cloudProfiles[glbParam->evSel].clouds_ON[k] = (int)0 ;
				}
				b = b +CLOUD_MIN_THICK ;
			}
		}
		// REMOVE CLOUDS'S GAPS THINNER THAN CLOUD_MIN_THICK BINS.
		for (int b =0 ; b <=(glbParam->indxEndSig -CLOUD_MIN_THICK -1) ; b++)
		{
			if ( (cloudProfiles[glbParam->evSel].clouds_ON[b] == (int)BIN_CLOUD) && (cloudProfiles[glbParam->evSel].clouds_ON[b+1] == (int)0) ) // IF A CLOUD ENDS...
			{
				sum_misc =0 ;
				for (int i =(b+1) ; i <=(b+1+CLOUD_MIN_THICK); i++)
					sum_misc = sum_misc + cloudProfiles[glbParam->evSel].clouds_ON[i];
				if ( sum_misc >0 )
				{
					for (int k =(b+1) ; k <=(b+1+CLOUD_MIN_THICK); k++)
						cloudProfiles[glbParam->evSel].clouds_ON[k] = (int)BIN_CLOUD ;
				}
				b = b +CLOUD_MIN_THICK ;
			}
		}
		cloudProfiles[glbParam->evSel].clouds_ON[0] 				    = (int)0 ; // NO -> THE ABL IS CONSIDERED AS CLOUD. FIRST BIN IS SET TO ZERO BE COHERENT FOR THE INDEXES COMPUTATION.
		cloudProfiles[glbParam->evSel].clouds_ON[glbParam->indxEndSig] = (int)0 ; // LAST BIN IS SET TO ONE BE COHERENT FOR THE INDEXES COMPUTATION.

// DONE WITH cloudProfile, NOW GET THE CLOUD LIMITS
	// GetCloudLimits( (strcGlobalParameters*)glbParam, (strcMolecularData*)dataMol ) ;
	GetCloudLimits_v1( (strcGlobalParameters*)glbParam ) ;

		for( int i=0 ; i <cloudProfiles[glbParam->evSel].nClouds ; i++ )
		{
			fitParam.indxInicFit = cloudProfiles[glbParam->evSel].indxInitClouds[i] -DELTA_RANGE_LIM_BINS ; // cloudProfiles[glbParam->evSel].indxInitClouds[0] 					   -DELTA_RANGE_LIM_BINS ; // 
			fitParam.indxEndFit  = cloudProfiles[glbParam->evSel].indxEndClouds[i]  +DELTA_RANGE_LIM_BINS ; // cloudProfiles[glbParam->evSel].indxEndClouds[cloudProfiles[glbParam->evSel].nClouds-1]  +DELTA_RANGE_LIM_BINS ; // 
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
						cloudProfiles[glbParam->evSel].clouds_ON[b] =0;
				}
				// else
				// 	printf("SC_RF() - (%d) Clouds: errRefBkg: %2.3lf \t errCloud: %2.3lf --> Cloud: %d-%d --> Fit: %d-%d\n", glbParam->evSel, errRefBkg, errCloud, cloudProfiles[glbParam->evSel].indxInitClouds[i], cloudProfiles[glbParam->evSel].indxEndClouds[i], cloudProfiles[glbParam->evSel].indxInitClouds[i] -DELTA_RANGE_LIM_BINS, cloudProfiles[glbParam->evSel].indxEndClouds[i]  +DELTA_RANGE_LIM_BINS) ;
			
				if ( strcmp(ifODcut, "YES") ==0 )
					ODcut( (double*)prS, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (strcFitParam*)&fitParam, (int*)cloudProfiles[glbParam->evSel].clouds_ON ) ;
			}
		}
		// if (cloudProfiles[glbParam->evSel].nClouds ==0)
		// 	printf("\nSC_RF() - (%d) NO Clouds detected\n", glbParam->evSel) ;
		
		// GetCloudLimits( (strcGlobalParameters*)glbParam, (strcMolecularData*)dataMol ) ;
		GetCloudLimits_v1( (strcGlobalParameters*)glbParam ) ;
}

// void CDataLevel_1::GetCloudLimits( strcGlobalParameters *glbParam, strcMolecularData *dataMol )
// {
// 	double *dco   = (double*) new double[glbParam->nBins] ;
// 	memset( (double*)dco  , 0, (sizeof(double) * glbParam->nBins) ) ;
// 	for ( int i = 0 ; i<=glbParam->indxEndSig ; i++ )
// 		dco[i] = (double)( cloudProfiles[glbParam->evSel].clouds_ON[i+1] - cloudProfiles[glbParam->evSel].clouds_ON[i] ) ;

// 	int nInicCloud=0, nEndCloud=0 ;
// 	for (int i =0; i <=glbParam->indxEndSig; i++)
// 	{
// 		if ( dco[i] ==(double)BIN_CLOUD )
// 			nInicCloud++ ;
// 		else if ( dco[i]==(double)(-BIN_CLOUD) )
// 			nEndCloud++ ;
// 	}
// 	if ( nInicCloud == nEndCloud )
// 	{
// 		memset( cloudProfiles[glbParam->evSel].indxInitClouds, 0, ( sizeof(int) * NMAXCLOUDS ) ) ;
// 		memset( cloudProfiles[glbParam->evSel].indxEndClouds , 0, ( sizeof(int) * NMAXCLOUDS ) ) ;
// 		memset( indxMol[glbParam->evSel].indxInicMol, 0, ( sizeof(int) * MAX_MOL_RANGES  ) ) ;
// 		memset( indxMol[glbParam->evSel].indxEndMol , 0, ( sizeof(int) * MAX_MOL_RANGES  ) ) ;
// 		cloudProfiles[glbParam->evSel].nClouds  = 0 ; // USED AS INDEX AND THEN, AS TOTAL NUMBER.
// 		indxMol[glbParam->evSel].nMolRanges 	= 0 ; // USED AS INDEX AND THEN, AS TOTAL NUMBER.

// 		//CLOUDS DETECTION
// 		for( int i=0 ; i <=(glbParam->indxEndSig-1) ; i++ )
// 		{
// 			if( dco[i] == (double)BIN_CLOUD ) // INIT CLOUD
// 			{
// 				cloudProfiles[glbParam->evSel].indxInitClouds[cloudProfiles[glbParam->evSel].nClouds] = i ;
// 				for (int j =i ; j <=(glbParam->indxEndSig-1) ; j++)
// 				{	// SEARCH THE END OF THE CLOUD
// 					if( dco[j] == (double)(-BIN_CLOUD) ) // END CLOUD
// 					{
// 						cloudProfiles[glbParam->evSel].indxEndClouds[cloudProfiles[glbParam->evSel].nClouds] = j -1;
// 						cloudProfiles[glbParam->evSel].nClouds++ ; // COUNT CLOUDS
// 						break ;
// 					}
// 				}
// 			}
// 		}
// 		// MOLECULAR RANGES DETECTION
// 		indxMol[glbParam->evSel].nMolRanges = cloudProfiles[glbParam->evSel].nClouds +1 ;
// 		if ( indxMol[glbParam->evSel].nMolRanges ==1 )
// 		{
// 			int 	heightRef_Inversion_ASL ;
// 			ReadAnalisysParameter( glbParam->FILE_PARAMETERS, "heightRef_Inversion_ASL" , "int" , (int*)&heightRef_Inversion_ASL ) ;
// 			indxMol[glbParam->evSel].indxRef = (int)round(heightRef_Inversion_ASL /dataMol->dzr) ;
// 				if ( indxMol[glbParam->evSel].indxRef > (glbParam->nBins-1) )
// 					indxMol[glbParam->evSel].indxRef = glbParam->nBins -10 ;

// 			for (int i =0; i < NMAXCLOUDS ; i++)
// 			{
// 				cloudProfiles[glbParam->evSel].indxInitClouds[i] = 0 ;
// 				cloudProfiles[glbParam->evSel].indxEndClouds [i] = 0 ;
// 				indxMol[glbParam->evSel].indxInicMol[i] = 0 ;
// 				indxMol[glbParam->evSel].indxEndMol[i]  = glbParam->indxEndSig  ;
// 			}
// 		}
// 		else
// 		{
// 			for( int i=0 ; i <indxMol[glbParam->evSel].nMolRanges ; i++ )
// 			{
// 				if (i==0)
// 				{
// 					indxMol[glbParam->evSel].indxInicMol[i] = 0 ;
// 					indxMol[glbParam->evSel].indxEndMol[i]  = cloudProfiles[glbParam->evSel].indxInitClouds[i] -1  ;
// 				}
// 				else
// 				{
// 					indxMol[glbParam->evSel].indxInicMol[i] = cloudProfiles[glbParam->evSel].indxEndClouds[i-1] +1 ;
// 					if ( i == (indxMol[glbParam->evSel].nMolRanges-1) )
// 						indxMol[glbParam->evSel].indxEndMol[i]  = glbParam->indxEndSig ;
// 					else
// 						indxMol[glbParam->evSel].indxEndMol[i]  = cloudProfiles[glbParam->evSel].indxInitClouds[i] -1 ;
// 				}
// 			}
// 		}
// 	}
// 	else
// 		printf("\nGetCloudLim(...) --> error\n") ;

// 	delete dco   ;
// }

void CDataLevel_1::GetCloudLimits_v1( strcGlobalParameters *glbParam )
{
	double *dco   = (double*) new double[glbParam->nBins] ;
	memset( (double*)dco  , 0, (sizeof(double) * glbParam->nBins) ) ;
	for ( int i = 0 ; i<=glbParam->indxEndSig ; i++ )
		dco[i] = (double)( cloudProfiles[glbParam->evSel].clouds_ON[i+1] - cloudProfiles[glbParam->evSel].clouds_ON[i] ) ;

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
		memset( cloudProfiles[glbParam->evSel].indxInitClouds, 0, ( sizeof(int) * NMAXCLOUDS ) ) ;
		memset( cloudProfiles[glbParam->evSel].indxEndClouds , 0, ( sizeof(int) * NMAXCLOUDS ) ) ;
		memset( indxMol[glbParam->evSel].indxInicMol, 0, ( sizeof(int) * MAX_MOL_RANGES  ) ) ;
		memset( indxMol[glbParam->evSel].indxEndMol , 0, ( sizeof(int) * MAX_MOL_RANGES  ) ) ;
		cloudProfiles[glbParam->evSel].nClouds  = 0 ; // USED AS INDEX AND THEN, AS TOTAL NUMBER.
		indxMol[glbParam->evSel].nMolRanges 	 = 0 ; // USED AS INDEX AND THEN, AS TOTAL NUMBER.

		//CLOUDS DETECTION
		for( int i=0 ; i <=(glbParam->indxEndSig-1) ; i++ )
		{
			if( dco[i] == (double)BIN_CLOUD ) // INIT CLOUD
			{
				cloudProfiles[glbParam->evSel].indxInitClouds[cloudProfiles[glbParam->evSel].nClouds] = i ;
				for (int j =i ; j <=(glbParam->indxEndSig-1) ; j++)
				{	// SEARCH THE END OF THE CLOUD
					if( dco[j] == (double)(-BIN_CLOUD) ) // END CLOUD
					{
						cloudProfiles[glbParam->evSel].indxEndClouds[cloudProfiles[glbParam->evSel].nClouds] = j -1;
						cloudProfiles[glbParam->evSel].nClouds++ ; // COUNT CLOUDS
						break ;
					}
				}
			}
		}
		// MOLECULAR RANGES DETECTION
		indxMol[glbParam->evSel].nMolRanges = cloudProfiles[glbParam->evSel].nClouds +1 ;
		if ( indxMol[glbParam->evSel].nMolRanges ==1 )
		{
			int 	heightRef_Inversion_ASL ;
			ReadAnalisysParameter( glbParam->FILE_PARAMETERS, "heightRef_Inversion_ASL" , "int" , (int*)&heightRef_Inversion_ASL ) ;
			indxMol[glbParam->evSel].indxRef = (int)round(heightRef_Inversion_ASL /glbParam->dzr) ;
				if ( indxMol[glbParam->evSel].indxRef > (glbParam->nBins-1) )
					indxMol[glbParam->evSel].indxRef = glbParam->nBins -10 ;

			for (int i =0; i < NMAXCLOUDS ; i++)
			{
				cloudProfiles[glbParam->evSel].indxInitClouds[i] = 0 ;
				cloudProfiles[glbParam->evSel].indxEndClouds [i] = 0 ;
				indxMol[glbParam->evSel].indxInicMol[i] = 0 ;
				indxMol[glbParam->evSel].indxEndMol[i]  = glbParam->indxEndSig  ;
			}
		}
		else
		{
			for( int i=0 ; i <indxMol[glbParam->evSel].nMolRanges ; i++ )
			{
				if (i==0)
				{
					indxMol[glbParam->evSel].indxInicMol[i] = 0 ;
					indxMol[glbParam->evSel].indxEndMol[i]  = cloudProfiles[glbParam->evSel].indxInitClouds[i] -1  ;
				}
				else
				{
					indxMol[glbParam->evSel].indxInicMol[i] = cloudProfiles[glbParam->evSel].indxEndClouds[i-1] +1 ;
					if ( i == (indxMol[glbParam->evSel].nMolRanges-1) )
						indxMol[glbParam->evSel].indxEndMol[i]  = glbParam->indxEndSig ;
					else
						indxMol[glbParam->evSel].indxEndMol[i]  = cloudProfiles[glbParam->evSel].indxInitClouds[i] -1 ;
				}
			}
		}
	}
	else
		printf("\nGetCloudLim(...) --> error\n") ;

	delete dco   ;

// printf("\n\t CDataLevel_1::GetCloudLimits_v1() " ) ;
// printf("\n\t cloudProfiles[c].nClouds: %d ", cloudProfiles[glbParam->evSel].nClouds ) ;
// printf("\n\t cloudProfiles[.].indxInitClouds[0]: %d ", cloudProfiles[glbParam->evSel].indxInitClouds[0]  ) ;
// printf("\n\t cloudProfiles[.].indxInitClouds[0]: %d ", cloudProfiles[glbParam->evSel].indxEndClouds[0]  ) ;
}

// void CDataLevel_1::MakeRangeCorrected( strcLidarSignal *evSig, strcGlobalParameters *glbParam, strcMolecularData *dataMol )
// {
// 	char BkgCorrMethod[10] ;

// 	fitParam.indxInicFit = glbParam->nBins - glbParam->nBinsBkg ;
// 	fitParam.indxEndFit  = glbParam->nBins -1 ;
// 	fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInicFit ;

// 	ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "BkgCorrMethod", "string" , (char*)BkgCorrMethod ) ;

// 	if ( (strcmp( BkgCorrMethod, "MEAN" ) ==0) || (strcmp( BkgCorrMethod, "mean" ) ==0) )
// 		bkgSubstractionMean_L1( (double*)evSig->pr, fitParam.indxInicFit, fitParam.indxEndFit, (strcGlobalParameters*)glbParam, (double*)evSig->pr_noBkg ) ;
// 	else if ( (strcmp( BkgCorrMethod, "FIT" ) ==0) || (strcmp( BkgCorrMethod, "fit" ) ==0) )
// 		bkgSubstractionMolFit_L1( (strcMolecularData*)dataMol, (const double*)evSig->pr, (strcFitParam*)&fitParam, (double*)evSig->pr_noBkg ) ;
// 	else if ( (strcmp( BkgCorrMethod, "FILE_BKG" ) ==0) || (strcmp( BkgCorrMethod, "file_bkg" ) ==0) )
// 		if ( glbParam->is_Noise_Data_Loaded == true )
// 			bkgSubstraction_BkgFile_L1( (const double*)evSig->pr, (strcFitParam*)&fitParam, (strcGlobalParameters*)glbParam, (double*)evSig->pr_noBkg ) ;
// 		else
// 		{
// 			printf("\n Noise data is not loaded \n") ;
// 			bkgSubstractionMean_L1( (double*)evSig->pr, fitParam.indxInicFit, fitParam.indxEndFit, (strcGlobalParameters*)glbParam, (double*)evSig->pr_noBkg ) ;
// 		}
// 	else
// 	{
// 		printf("\n Wrong setting of 'Background Correction Method (BkgCorrMethod) in %s. Used FIT method' \n", (char*)glbParam->FILE_PARAMETERS) ;
// 		bkgSubstractionMolFit ( (strcMolecularData*)dataMol, (double*)evSig->pr, (strcFitParam*)&fitParam, (double*)evSig->pr_noBkg ) ;
// 	}
// 	// RANGE CORRECTED
// 	for ( int i=0 ; i<glbParam->nBins ; i++ ) 	evSig->pr2[i] = evSig->pr_noBkg[i] * pow(glbParam->r[i], 2) ;
// }

// void CDataLevel_1::bkgSubstractionMean_L1( double *sig, int binInitMean, int binEndMean, strcGlobalParameters *glbParam, double *pr_noBkg)
// {
// 	int nFit = binEndMean - binInitMean ;

// 	double 	bkgMean = 0 ;

// 	for( int j=binInitMean ; j<binEndMean ; j++ ) bkgMean = bkgMean + sig[j] ;

// 	bkgMean = bkgMean /nFit ;

// 	for ( int i=0 ; i<glbParam->nBins ; i++ ) 	pr_noBkg[i] = (double)(sig[i] - bkgMean) ;
// }

// void CDataLevel_1::bkgSubstractionMolFit_L1 (strcMolecularData *dataMol, const double *prEl, strcFitParam *fitParam, double *pr_noBkg)
// {
// 	double *dummy = (double*) new double[dataMol->nBins] ; 
// 	RayleighFit( (double*)prEl, (double*)dataMol->prMol, dataMol->nBins , "wB", "NOTall", (strcFitParam*)fitParam, (double*)dummy ) ;
// 		for ( int i=0 ; i<dataMol->nBins ; i++ ) 	pr_noBkg[i] = (double)(prEl[i] - fitParam->b) ; 

// 	delete dummy ;
// }

// void CDataLevel_1::bkgSubstraction_BkgFile_L1( const double *prEl, strcFitParam *fitParam, strcGlobalParameters *glbParam, double *pr_noBkg)
// {
// 	double *dummy = (double*) new double[glbParam->nBins] ; 
// 	smooth( (double*)&data_Noise[glbParam->chSel][0], (int)0, (int)(glbParam->nBins-1), (int)11, (double*)dummy ) ;

// 	RayleighFit( (double*)prEl, (double*)dummy, glbParam->nBins , "wOutB", "all", (strcFitParam*)fitParam, (double*)noiseFit ) ;
// 	// RayleighFit( (double*)prEl, (double*)&data_Noise[glbParam->chSel][0], glbParam->nBins , "wOutB", "all", (strcFitParam*)fitParam, (double*)noiseFit ) ;
// 		for ( int i=0 ; i<glbParam->nBins ; i++ ) 	pr_noBkg[i] = (double)(prEl[i] - noiseFit[i]) ; 

// 	delete dummy ;
// }
