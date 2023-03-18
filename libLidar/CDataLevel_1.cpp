
#include "CDataLevel_1.hpp"  

CDataLevel_1::CDataLevel_1( strcGlobalParameters *glbParam )
{
	ReadAnalisysParameter( glbParam->FILE_PARAMETERS, "avg_Points_Cloud_Mask", "int"   , (int*)&avg_Points_Cloud_Mask 	) ;
	ReadAnalisysParameter( glbParam->FILE_PARAMETERS, "errFactor"			 , "double", (double*)&errFactor    		) ;
	ReadAnalisysParameter( glbParam->FILE_PARAMETERS, "CLOUD_MIN_THICK"	  	 , "int"   , (int*)&CLOUD_MIN_THICK 		) ;
	ReadAnalisysParameter( glbParam->FILE_PARAMETERS, "stepScanCloud" 	  	 , "int"   , (int*)&stepScanCloud 		    ) ;
	ReadAnalisysParameter( glbParam->FILE_PARAMETERS, "errScanCheckFactor"   , "double", (double*)&errScanCheckFactor   ) ;
	ReadAnalisysParameter( glbParam->FILE_PARAMETERS, "thresholdFactor"	  	 , "double", (double*)&thresholdFactor 	    ) ;
	ReadAnalisysParameter( glbParam->FILE_PARAMETERS, "DELTA_RANGE_LIM_BINS" , "int"   , (int*)&DELTA_RANGE_LIM_BINS    ) ;
	ReadAnalisysParameter( glbParam->FILE_PARAMETERS, "errCloudCheckFactor"  , "double", (double*)&errCloudCheckFactor  ) ;
	ReadAnalisysParameter( glbParam->FILE_PARAMETERS, "ifODcut"			  	 , "string", (char*)ifODcut 				) ;
	ReadAnalisysParameter( glbParam->FILE_PARAMETERS, "nScanMax"			 , "int"   , (int*)&nScanMax 			    ) ;

	scanNumExit = nScanMax ;

	prFit  = (double*) new double [glbParam->nBins] ;
	prS    = (double*) new double [glbParam->nBins] ;
	prprm  = (double*) new double [glbParam->nBins] ;
	prmprm = (double*) new double [glbParam->nBins] ;
	dco    = (int*   ) new int    [glbParam->nBins] ;

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
	{	
		smooth( (double*)pr			, (int)0, (int)(glbParam->nBins-1), (int)avg_Points_Cloud_Mask, (double*)prS ) ;
		smooth( (double*)glbParam->r, (int)0, (int)(glbParam->nBins-1), (int)avg_Points_Cloud_Mask, (double*)glbParam->r_avg ) ;
	}
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

	glbParam->indxEndSig = glbParam->indxEndSig_ev[glbParam->evSel] ;
	fitParam.indxInicFit = (int) glbParam->indxInitSig ;
	fitParam.indxEndFit  = (int) glbParam->indxEndSig  ;
		for ( int i=0 ; i<nScanMax ; i++ ) // ------------------------------------------------------------------------------------------
		{
			for (int b =0; b <glbParam->nBins ; b++)
			{
				cloudProfiles[glbParam->evSel].test_1[b] 	=(double) 0.0 ;
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
					for( int b=glbParam->indxInitSig ; b<=glbParam->indxEndSig ; b++ ) 
					{
						if( SE_lay[i][b] > 0 )
						{
							nMaxLoop++ ;
							break ;
						}
					}
				}
				else {
					if ( fitParam.indxInicFit >= fitParam.indxEndFit )
						break ; }
			}  // END OF "SET THE CLOUDS CANDIDATES"
		} // for ( int i=0 ; i<nScanMax ; i++ ) ------------------------------------------------------------------------------------------

		if (nMaxLoop == 0)
		{
			printf("\t Completely pure Rayleigh lidar signal\t") ;
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

		for( int b=0 ; b <glbParam->nBins ; b++ )
		{
			if ( cloudProfiles[glbParam->evSel].test_1[b] > (double)(errRefBkg *thresholdFactor) )
				cloudProfiles[glbParam->evSel].clouds_ON[b] = (int)BIN_CLOUD ;
			else
				cloudProfiles[glbParam->evSel].clouds_ON[b] = (int)0 ;
		}
		// CHECK CLOUDS TESTS:
		// REMOVE CLOUDS THINNER THAN CLOUD_MIN_THICK BINS.
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
	GetCloudLimits( (strcGlobalParameters*)glbParam ) ;

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
		
		GetCloudLimits( (strcGlobalParameters*)glbParam ) ;
}

void CDataLevel_1::GetCloudLimits( strcGlobalParameters *glbParam )
{
	int maxCloud, indxMax ;
	findIndxMax_void( (int*)&cloudProfiles[glbParam->evSel].clouds_ON[0], (const char*)"int", 0, (glbParam->nBins-1), (int*)&indxMax, (int*)&maxCloud ) ;
	// ( maxCloud == 1 ) ? printf("\nThere *ARE* clouds at event number %d", glbParam->evSel) : printf("There are *NOT* clouds at event number %d", glbParam->evSel) ;

	// BASIC CLOUDLESS SETTINGS
	memset( cloudProfiles[glbParam->evSel].indxInitClouds, 0, ( sizeof(int) * NMAXCLOUDS ) ) ;
	memset( cloudProfiles[glbParam->evSel].indxEndClouds , 0, ( sizeof(int) * NMAXCLOUDS ) ) ;
	memset( indxMol[glbParam->evSel].indxInicMol, 0, ( sizeof(int) * MAX_MOL_RANGES  ) ) ;
	memset( indxMol[glbParam->evSel].indxEndMol , 0, ( sizeof(int) * MAX_MOL_RANGES  ) ) ;
	cloudProfiles[glbParam->evSel].nClouds  = 0 ; // USED AS INDEX AND THEN, AS TOTAL NUMBER.
	indxMol[glbParam->evSel].nMolRanges 	= 0 ; // USED AS INDEX AND THEN, AS TOTAL NUMBER.

	if ( maxCloud ==1 ) // IF THERE ARE CLOUDS --> GETS THEIRS LIMITS
	{
		memset( (int*)dco, 0, (sizeof(int) * glbParam->nBins) ) ;

		for ( int i =0 ; i<=glbParam->indxEndSig_ev[glbParam->evSel] ; i++ )
			dco[i] = (int)( cloudProfiles[glbParam->evSel].clouds_ON[i+1] - cloudProfiles[glbParam->evSel].clouds_ON[i] ) ;

		// CHECK CONSISTENCY --> nInicCloud = nEndCloud
		int nInicCloud=0, nEndCloud=0 ;
		for (int i =0 ; i <=glbParam->indxEndSig_ev[glbParam->evSel] ; i++)
		{
			if ( dco[i] ==(int)BIN_CLOUD )
				nInicCloud++ ;
			else if ( dco[i]==(int)(-BIN_CLOUD) )
				nEndCloud++ ;
		}
		if ( nInicCloud == nEndCloud ) // IN CASE OF CLOUDY PROFILE --> GET THEIR LIMITS
		{
			// CLOUDS DETECTION
			for( int i=0 ; i <=(glbParam->indxEndSig_ev[glbParam->evSel]) ; i++ )
			{
				if( dco[i] == (int)BIN_CLOUD ) // INIT CLOUD
				{
					cloudProfiles[glbParam->evSel].indxInitClouds[cloudProfiles[glbParam->evSel].nClouds] = i ;
					for (int j =i ; j <=glbParam->indxEndSig_ev[glbParam->evSel] ; j++)
					{	// SEARCH THE END OF THE CLOUD
						if( dco[j] == (int)(-BIN_CLOUD) ) // END CLOUD
						{
							cloudProfiles[glbParam->evSel].indxEndClouds[cloudProfiles[glbParam->evSel].nClouds] = j -1;
							cloudProfiles[glbParam->evSel].nClouds++ ; // COUNT CLOUDS
							break ;
						}
					}
				}
			}
			indxMol[glbParam->evSel].nMolRanges = cloudProfiles[glbParam->evSel].nClouds +1 ;
			// CLOUDS DETECTED --> MOLECULAR RANGES SETTINGS
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
						indxMol[glbParam->evSel].indxEndMol[i]  = glbParam->indxEndSig_ev[glbParam->evSel] ;
					else
						indxMol[glbParam->evSel].indxEndMol[i]  = cloudProfiles[glbParam->evSel].indxInitClouds[i] -1 ;
				}
			}
		} // if ( nInicCloud == nEndCloud )
		else
			printf("\nGetCloudLim(...) --> error\n") ;
	} // if ( maxCloud ==1 ) // IF THERE ARE CLOUDS --> GETS THEIRS LIMITS
	else // NO CLOUDS DETECTED
	{
		cloudProfiles[glbParam->evSel].nClouds = 0 ;
		indxMol[glbParam->evSel].nMolRanges    = cloudProfiles[glbParam->evSel].nClouds +1 ;

		// MOLECULAR RANGES SETTINGS
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
				indxMol[glbParam->evSel].indxEndMol[i]  = glbParam->indxEndSig_ev[glbParam->evSel] ;
			}
		}
	}

	// printf("\n\t CDataLevel_1::GetCloudLimits() " ) ;
	// printf("\n\t cloudProfiles[%d].nClouds: %d ", glbParam->evSel, cloudProfiles[glbParam->evSel].nClouds ) 					;
	// printf("\n\t cloudProfiles[%d].indxInitClouds[0]: %d ", glbParam->evSel, cloudProfiles[glbParam->evSel].indxInitClouds[0] ) ;
	// printf("\n\t cloudProfiles[%d].indxEndClouds[0] : %d ", glbParam->evSel, cloudProfiles[glbParam->evSel].indxEndClouds[0]  ) ;
	// printf("\n\t cloudProfiles[%d].VOD_cloud[0]     : %lf ", glbParam->evSel, cloudProfiles[glbParam->evSel].VOD_cloud[0]	  ) ;
}

void CDataLevel_1::saveCloudsInfoDB( char *Path_File_Out, strcGlobalParameters *glbParam, strcCloudInfoDB_LPP *cloudInfoDB )
{
	FILE 	*fpDB = fopen( Path_File_Out, "w+t" ) ;

	for (int t =0 ; t <glbParam->nEventsAVG ; t++)
	{
		fprintf( fpDB, "%d %d %lf %lf \n", cloudInfoDB->cloudTime[t], cloudInfoDB->nClouds[t], cloudInfoDB->lowestCloudHeight_ASL[t], cloudInfoDB->lowestCloudThickness[t] ) ;

		// fprintf(fpDB, "%d %lf %lf %lf \n", cloudInfoDB->cloudTime[t], cloudInfoDB->lowestCloudHeight_ASL[t], cloudInfoDB->lowestCloudThickness[t], 
		// 									cloudInfoDB->lowestCloud_VOD[t] ) ;
	}
	fclose(fpDB) ;
	printf( "\n\nDone with the cloud data base file: %s \n", Path_File_Out ) ;
}

void CDataLevel_1::saveCloudsInfoDB( char *Path_File_Out, strcGlobalParameters *glbParam, int *Raw_Data_Start_Time_AVG )
{
	FILE 	*fpDB = fopen( Path_File_Out, "w+t" ) ;

	for (int t =0 ; t <glbParam->nEventsAVG ; t++)
	{
		fprintf( fpDB, "%d %d ", Raw_Data_Start_Time_AVG[t], cloudProfiles[t].nClouds ) ;

		for ( int n =0; n <cloudProfiles[t].nClouds ; n++)
			fprintf( fpDB, "%5.3lf %5.3lf ", cloudProfiles[t].indxInitClouds[0] *glbParam->dr,
				   (cloudProfiles[t].indxEndClouds[n] - cloudProfiles[t].indxInitClouds[n]) *glbParam->dr ) ;

		fprintf( fpDB, "\n" ) ;
	}
	fclose(fpDB) ;
	printf( "\n\nDone with the cloud data base file: %s \n", Path_File_Out ) ;
}
