
#include "CDataLevel_1.hpp"  

CDataLevel_1::CDataLevel_1( strcGlobalParameters *glbParam )
{
	ReadAnalysisParameter( glbParam->FILE_SOFT_CODED_VALUES, "avg_Points_Cloud_Mask", "int", (int*)&avg_Points_Cloud_Mask 	) ;
	ReadAnalysisParameter( glbParam->FILE_SOFT_CODED_VALUES, "CLOUD_MIN_THICK"	  	, "int", (int*)&CLOUD_MIN_THICK 		) ;
	ReadAnalysisParameter( glbParam->FILE_SOFT_CODED_VALUES, "stepScanCloud" 	  	, "int", (int*)&stepScanCloud 		    ) ;

	// ReadAnalysisParameter( glbParam->FILE_PARAMETERS, "errFactor"			 , "double", (double*)&errFactor    		) ;
	// ReadAnalysisParameter( glbParam->FILE_PARAMETERS, "errScanCheckFactor"   , "double", (double*)&errScanCheckFactor   ) ;
	// ReadAnalysisParameter( glbParam->FILE_PARAMETERS, "thresholdFactor"	  	 , "double", (double*)&thresholdFactor 	    ) ;
	// ReadAnalysisParameter( glbParam->FILE_PARAMETERS, "DELTA_RANGE_LIM_BINS" , "int"   , (int*)&DELTA_RANGE_LIM_BINS    ) ;
	// ReadAnalysisParameter( glbParam->FILE_PARAMETERS, "errCloudCheckFactor"  , "double", (double*)&errCloudCheckFactor  ) ;

	ReadAnalysisParameter( glbParam->FILE_PARAMETERS, "ifODcut", "string", (char*)ifODcut, sizeof(ifODcut) ) ;
	char compute_layer_mask_buf[128] = {0} ;
	ReadAnalysisParameter( glbParam->FILE_SOFT_CODED_VALUES, "COMPUTE_LAYER_MASK", "string", compute_layer_mask_buf, sizeof(compute_layer_mask_buf) ) ;
	strCompCM.assign(compute_layer_mask_buf) ;
	char compute_pbl_mask_buf[128] = {0} ;
	ReadAnalysisParameter( glbParam->FILE_SOFT_CODED_VALUES, "COMPUTE_PBL_MASK", "string", compute_pbl_mask_buf, sizeof(compute_pbl_mask_buf) ) ;
	strCompPBL.assign(compute_pbl_mask_buf) ;
	// ReadAnalysisParameter( glbParam->FILE_PARAMETERS, "COMPUTE_LAYER_MASK"   , "string", (char*)strCompCM.c_str() 		) ;
	// ReadAnalysisParameter( glbParam->FILE_PARAMETERS, "COMPUTE_PBL_MASK"     , "string", (char*)strCompPBL.c_str() 		) ;

	prFit  = (double*) new double [glbParam->nBins] ;
	prS    = (double*) new double [glbParam->nBins] ;
	pr2    = (double*) new double [glbParam->nBins] ;
	prprm  = (double*) new double [glbParam->nBins] ;
	prmprm = (double*) new double [glbParam->nBins] ;
	dco    = (int*   ) new int    [glbParam->nBins] ;

	oLOp = (CLidar_Operations*) new CLidar_Operations( (strcGlobalParameters*)glbParam ) ;
}
	
CDataLevel_1::~CDataLevel_1()
{
	delete[] prS   ;
	delete[] prprm  ;
	delete[] prmprm ;
	delete[] prFit ;
	delete[] dco ;
	delete oLOp ;

	// for (int i =0; i <nScanMax; i++) delete[] SE_lay[i] ; 
	// delete[] SE_lay ;
}

/*
void CDataLevel_1::GetMem_cloudProfiles( strcGlobalParameters *glbParam )
{
	cloudProfiles = (strcCloudProfiles*) new strcCloudProfiles [glbParam->nEventsAVG] ;
	for( int e=0 ; e<glbParam->nEventsAVG ; e++ )
	{
		cloudProfiles[e].clouds_ON 	    = (int*) new int [glbParam->nBins]   	 ; memset( cloudProfiles[e].clouds_ON     , 0, ( sizeof(int) * glbParam->nBins    ) ) ;
		cloudProfiles[e].indxInitClouds = (int*) new int [NMAXCLOUDS]        	 ; memset( cloudProfiles[e].indxInitClouds, 0, ( sizeof(int) * NMAXCLOUDS	      ) ) ;
		cloudProfiles[e].indxEndClouds  = (int*) new int [NMAXCLOUDS]        	 ; memset( cloudProfiles[e].indxEndClouds , 0, ( sizeof(int) * NMAXCLOUDS	      ) ) ;
		cloudProfiles[e].VOD_cloud      = (double*) new double [NMAXCLOUDS]  	 ; memset( cloudProfiles[e].VOD_cloud     , 0, ( sizeof(double) * NMAXCLOUDS      ) ) ;
		cloudProfiles[e].test_1  		= (double*) new double [glbParam->nBins] ; memset( cloudProfiles[e].test_1		  , 0, ( sizeof(double) * glbParam->nBins ) ) ;
		cloudProfiles[e].test_2  		= (double*) new double [glbParam->nBins] ; memset( cloudProfiles[e].test_2		  , 0, ( sizeof(double) * glbParam->nBins ) ) ;
		cloudProfiles[e].nClouds		= (int)0 ;

		cloudProfiles[e].indxEndPBL		= (int)0 	  ;
		cloudProfiles[e].rangeEndPBL	= (double)0.0 ;
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
*/

/*
void CDataLevel_1::Layer_Mask( const double *pr, strcMolecularData *dataMol, strcGlobalParameters *glbParam )
{
	ReadAnalysisParameter( glbParam->FILE_SOFT_CODED_VALUES, "avg_Points_Cloud_Mask", "int", (int*)&avg_Points_Cloud_Mask ) ;
	// pr IS ALREADY BACKGROUND CORRECTED
	if ( avg_Points_Cloud_Mask >1 )
	{
		smooth( (double*)pr			    , (int)0, (int)(glbParam->nBins-1), (int)avg_Points_Cloud_Mask, (double*)prS 					) ;
		smooth( (double*)glbParam->r    , (int)0, (int)(glbParam->nBins-1), (int)avg_Points_Cloud_Mask, (double*)glbParam->r_avg 		) ;
		smooth( (double*)dataMol->prMol , (int)0, (int)(glbParam->nBins-1), (int)avg_Points_Cloud_Mask, (double*)dataMol->prMol_avg 	) ;
		smooth( (double*)dataMol->pr2Mol, (int)0, (int)(glbParam->nBins-1), (int)avg_Points_Cloud_Mask, (double*)dataMol->pr2Mol_avg	) ;
	}
	else
	{
		for( int b=0 ; b <glbParam->nBins ; b++ )
		{
			prS[b] 	  				= (double)pr     [b] ;	
			dataMol->prMol_avg[b] 	= dataMol->prMol [b] ;
			dataMol->pr2Mol_avg[b] 	= dataMol->pr2Mol[b] ;
			glbParam->r_avg[b] 		= glbParam->r    [b] ;
		}
	}

	for (int  j =0; j <glbParam->nBins_Ch[glbParam->chSel] ; j++)	
		pr2[j] = (double) prS[j] * glbParam->r[j]*glbParam->r[j] ; // RANGE CORRECTED LIDAR SIGNAL

	if ( glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] <=0 )
	{
		printf("\nFinding last molecular ranges\n") ;
		oLOp->Find_Max_Mol_Range( (double*)prS, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (int)0 ) ;
	}
	// GET THE REFERENCE-STANDARD DEVIATION FOR THE SIGNAL
	fitParam.indxInitFit = (int)round( (glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] + glbParam->nBins_Ch[glbParam->chSel])/2 ) ;
	fitParam.indxEndFit  = glbParam->nBins_Ch[glbParam->chSel] -1 						;
	fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1;
	double std_ref = 0;
	for ( int j = fitParam.indxInitFit; j <= fitParam.indxEndFit; j++ )
		fitParam.std += pow( prS[j], 2 ) ; // prS IS ALREADY BACGROUND CORRECTED
	std_ref = sqrt( fitParam.std / (fitParam.nFit - 1) );

	// SET THE INITIAL FITTING RANGES FOR THE LAYER DETECTION ALGORITHM
	int 	deltaNorm ; // NUMBER OF BINS FOR THE RAYLEIGH FIT NORMALIZATION
	ReadAnalysisParameter( glbParam->FILE_SOFT_CODED_VALUES, "deltaNorm", "int", (int*)&deltaNorm )  ;
	fitParam.indxEndFit  = glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] - deltaNorm  ;
	fitParam.indxInitFit = fitParam.indxEndFit - deltaNorm ;
	fitParam.indxMidFit  = (int)round( (fitParam.indxInitFit + fitParam.indxEndFit)/2 ) ;
	fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1;
	ReadAnalysisParameter( glbParam->FILE_SOFT_CODED_VALUES, "std_factor_layer_mask", "float", (float*)&glbParam->std_factor_layer_mask ) ;

	memset( oLOp->cloudProfiles[glbParam->evSel].clouds_ON, 0, ( sizeof(int) * glbParam->nBins ) ) ;

	// FIRST STAGE OF CLOUD DETECTION
	while ( fitParam.indxInitFit >= glbParam->indxInitSig )
	{
		memset( (double*)prFit, 0, sizeof(double) * glbParam->nBins ) ;
		oLOp->RayleighFit( (double*)prS, (double*)dataMol->prMol_avg, glbParam->nBins, (strcFitParam*)&fitParam, (double*)prFit ) ;
		// for (int j =fitParam.indxInitFit ; j < glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] ; j++)
		for (int j =fitParam.indxInitFit ; j < glbParam->nBins_Ch[glbParam->chSel] ; j++)
		{
			if ( prS[j] >= (prFit[j] + std_ref * glbParam->std_factor_layer_mask ) )
				oLOp->cloudProfiles[glbParam->evSel].clouds_ON[j] = (int)BIN_CLOUD ;
		}
		fitParam.indxEndFit  = fitParam.indxEndFit - stepScanCloud ; // glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] - deltaNorm -i*stepScanCloud ; // STEP DOWN IN THE SCAN
		fitParam.indxInitFit = fitParam.indxEndFit - deltaNorm     ; // DELTA NOMALIZATION STABLISHED
		fitParam.indxMidFit  = (int)round( (fitParam.indxInitFit + fitParam.indxEndFit)/2 ) ;
		fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1;
	}

	FilterThinClouds( (strcGlobalParameters*)glbParam, (int*)oLOp->cloudProfiles[glbParam->evSel].clouds_ON ) ;
	oLOp->GetCloudLimits( (strcGlobalParameters*)glbParam ) ; // ONLY LIMITS OF THE CLOUDS, *NOT* PBL!!!!

	//! PBL HEIGH - START
	fitParam.indxInitFit = (int)-10 ;
	fitParam.indxEndFit  = (int)-10 ;
	fitParam.indxMidFit  = (int)-10 ;
	fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1;

	if ( strcmp( glbParam->exeFile, "./lpp1" ) ==0 )
	// 	IF DATA LEVEL 1 IS RUNNING, THE PBL MASK IS IS COMPUTED DEPENDING ON THE CONFIGURATION FILE
	{
		char compute_pbl_mask_buf[128] = {0} ;
		ReadAnalysisParameter( glbParam->FILE_SOFT_CODED_VALUES, "COMPUTE_PBL_MASK", "string", compute_pbl_mask_buf, sizeof(compute_pbl_mask_buf) ) ;
		strCompPBL.assign(compute_pbl_mask_buf) ;
	}
	else if ( strcmp( glbParam->exeFile, "./lpp2" ) ==0 )
	{  	// 	IF DATA LEVEL 2 IS RUNNING, THE PBL MASK IS ALWAYS COMPUTED, INDEPENDENTLY OF THE CONFIGURATION FILE
		strCompPBL.assign("YES") ;
		printf("\n --> Running LPP2, so the PBL mask will be computed independently of the configuration file. \n") ;
	}

	if ( strcmp( strCompPBL.c_str(), "YES" ) ==0 )
	{
		int a, b ;
		printf(" --> Getting PBL layer: \n") ;
		if ( oLOp->cloudProfiles[glbParam->evSel].nClouds ==0  )
		{
			a = (int)round( (glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] - glbParam->indxInitSig) *1/4 ) ;
			b = (int)round( (glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] - glbParam->indxInitSig) *3/8 ) ;
			printf(" (1) Fit between the start range and the maximun range (%d, %d).", a, b) ;
		}
		else if ( (oLOp->cloudProfiles[glbParam->evSel].nClouds >=1) )
		{
			for (int c =0 ; c <oLOp->cloudProfiles[glbParam->evSel].nClouds ; c++) // SEARCH FOR A CLOUD BASE HIGHER THAN 3 KM
			{
				if ( oLOp->cloudProfiles[glbParam->evSel].indxInitClouds[c] > (int)round(3000/glbParam->dr) )
				{
					a = (int)round( (oLOp->cloudProfiles[glbParam->evSel].indxInitClouds[c] - round(2000/glbParam->dr)) *1/4 ) ;
					b = (int)round( (oLOp->cloudProfiles[glbParam->evSel].indxInitClouds[c] - round(2000/glbParam->dr)) *3/8 ) ;
					printf(" (2) Fit before the first cloud higher than 3 km (%d, %d).", a, b) ;
				}
			}
			if (fitParam.indxInitFit ==-10 ) // IF THE CLOUDS ARE ALL BELOW 3 KM, THE PBL FIT IS DONE AT 3 KM
			{
				a = (int)-10 ;
				b = (int)-10 ;
				// a = (int)round( (glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] - round(2000/glbParam->dr)) *1/4 ) ;
				// b = (int)round( (glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] - round(2000/glbParam->dr)) *3/8 ) ;
				// printf(" (3) Fit between the start range and the maximun range (%d, %d).", a, b) ;
			}
		} // else if ( (oLOp->cloudProfiles[glbParam->evSel].nClouds >=1) )

			if ( a == -10 )
			{
				fitParam.indxInitFit = (int)round( 3000/glbParam->dr ) 		 ;
				fitParam.indxEndFit  = fitParam.indxInitFit + (int)deltaNorm ;
			}
			else
			{
				fitParam.indxInitFit = (int)round( 2000/glbParam->dr + b ) 	;
				fitParam.indxEndFit  = fitParam.indxInitFit + a 			;
					if (fitParam.indxInitFit == fitParam.indxEndFit )
						fitParam.indxEndFit = fitParam.indxInitFit + 1 		;
			}
			fitParam.indxMidFit  = (int)round( (fitParam.indxInitFit + fitParam.indxEndFit)/2 ) ;
			fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1;
			printf(" --> %f - %f m\n", fitParam.indxInitFit *glbParam->dr, fitParam.indxEndFit *glbParam->dr ) ;

			memset( (double*)prFit, 0, sizeof(double) * glbParam->nBins ) ;
			oLOp->RayleighFit( (double*)prS, (double*)dataMol->prMol_avg, glbParam->nBins, (strcFitParam*)&fitParam, (double*)prFit ) ;

			for (int i =glbParam->indxInitSig ; i < fitParam.indxEndFit ; i++)
			{
				if ( prS[i] > (prFit[i] + std_ref * glbParam->std_factor_layer_mask ) )
					oLOp->cloudProfiles[glbParam->evSel].clouds_ON[i] = (int)1 ;
				else
					oLOp->cloudProfiles[glbParam->evSel].clouds_ON[i] = (int)0 ;
			}
			for (int i =0 ; i <=glbParam->indxInitSig ; i++)
					oLOp->cloudProfiles[glbParam->evSel].clouds_ON[i] = (int)1 ;
	} // if ( strcmp( strCompPBL.c_str(), "YES" ) ==0 )
	//! PBL HEIGH - END

	// IF THERE ARE CLOUDS DETECTED --> ADJUST THE BOUNDARIES
	int sumLay =0 ;
	sum_int( (int*)&oLOp->cloudProfiles[glbParam->evSel].clouds_ON[0], (int)0, (int)(glbParam->nBins_Ch[glbParam->chSel]-1), (int*)&sumLay ) ;
	if ( sumLay >0 )
	{
		oLOp->GetCloudLimits( (strcGlobalParameters*)glbParam ) ; // ONLY LIMITS OF THE CLOUDS, *NOT* PBL!!!!

		for (int c =0; c < oLOp->cloudProfiles[glbParam->evSel].nClouds; c++)
		{	//! CLOUD *BASE* HEIGH - START //////////////////////////////////////////////////////
			int 	indxMin =0 ;
			double	min 	=0 ;
			findIndxMin( (double*)&pr2[0], (int)glbParam->indxInitSig, (int)oLOp->cloudProfiles[glbParam->evSel].indxInitClouds[c], 
						   											   (int*)&indxMin, (double*)&min ) ;

			printf("\n BEFORE ADJUSTING CLOUD --> Cloud %d base at %f m\n", c+1, oLOp->cloudProfiles[glbParam->evSel].indxInitClouds[c] * glbParam->dr ) ;
			for (int j =indxMin; j < oLOp->cloudProfiles[glbParam->evSel].indxInitClouds[c] ; j++)
				oLOp->cloudProfiles[glbParam->evSel].clouds_ON[j] = (int)1 ;
			oLOp->cloudProfiles[glbParam->evSel].indxInitClouds[c] = indxMin ;
			printf(" AFTER ADJUSTING CLOUD--> Cloud %d base at %f m\n", c+1, oLOp->cloudProfiles[glbParam->evSel].indxInitClouds[c] * glbParam->dr ) ;
			//! CLOUD *BASE* HEIGH - END //////////////////////////////////////////////////////
	
			//! CLOUD *TOP*  HEIGH - START //////////////////////////////////////////////////////
			// FIT BEFORE THE CLOUD
			fitParam.indxInitFit = oLOp->cloudProfiles[glbParam->evSel].indxInitClouds[c] - round(deltaNorm*1.5) ;
			fitParam.indxEndFit  = fitParam.indxInitFit + deltaNorm ;
			fitParam.indxMidFit  = (int)round( (fitParam.indxInitFit + fitParam.indxEndFit)/2 ) ;
			fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1;
			memset( (double*)prFit, 0, sizeof(double) * glbParam->nBins ) ;
			oLOp->RayleighFit( (double*)prS, (double*)dataMol->prMol_avg, glbParam->nBins, (strcFitParam*)&fitParam, (double*)prFit ) ;
	
			// for (int j =fitParam.indxMidFit ; j < glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel]; j++ )
			for (int j =fitParam.indxMidFit ; j < oLOp->cloudProfiles[glbParam->evSel].indxEndClouds[c] + deltaNorm ; j++ )
			{
				if ( prS[j] < prFit[j] )
				{
					oLOp->cloudProfiles[glbParam->evSel].indxEndClouds[c] = j ;
					break ;
				}
			}
			//! CLOUD *TOP*  HEIGH - END //////////////////////////////////////////////////////
		} // for (int c =0; c < oLOp->cloudProfiles[glbParam->evSel].nClouds; c++)
		oLOp->GetCloudLimits( (strcGlobalParameters*)glbParam ) ;
	} // if ( sumLay >0 ) 
	else // NO CLOUDS DETECTED
		memset( (double*)&oLOp->cloudProfiles[glbParam->evSel].clouds_ON[0], 0, sizeof(int)*glbParam->nBins ) ;
*/
	// FilterThinClouds( (strcGlobalParameters*)glbParam, (int*)oLOp->cloudProfiles[glbParam->evSel].clouds_ON ) ;
	// oLOp->GetCloudLimits( (strcGlobalParameters*)glbParam ) ;
// } // void CDataLevel_1::Layer_Mask( const double *pr, strcMolecularData *dataMol, strcGlobalParameters *glbParam )


// void CDataLevel_1::Layer_Mask( const double *pr, strcMolecularData *dataMol, strcGlobalParameters *glbParam )
// {
// 	// pr IS ALREADY BACKGROUND CORRECTED
// 	if ( avg_Points_Cloud_Mask >1 )
// 	{
// 		smooth( (double*)pr			    , (int)0, (int)(glbParam->nBins-1), (int)avg_Points_Cloud_Mask, (double*)prS 					) ;
// 		smooth( (double*)glbParam->r    , (int)0, (int)(glbParam->nBins-1), (int)avg_Points_Cloud_Mask, (double*)glbParam->r_avg 		) ;
// 		smooth( (double*)dataMol->prMol , (int)0, (int)(glbParam->nBins-1), (int)avg_Points_Cloud_Mask, (double*)dataMol->prMol_avg 	) ;
// 		smooth( (double*)dataMol->pr2Mol, (int)0, (int)(glbParam->nBins-1), (int)avg_Points_Cloud_Mask, (double*)dataMol->pr2Mol_avg	) ;
// 	}
// 	else
// 	{
// 		for( int b=0 ; b <glbParam->nBins ; b++ )
// 		{
// 			prS[b] 	  				= (double)pr     [b] ;	
// 			dataMol->prMol_avg[b] 	= dataMol->prMol [b] ;
// 			dataMol->pr2Mol_avg[b] 	= dataMol->pr2Mol[b] ;
// 			glbParam->r_avg[b] 		= glbParam->r    [b] ;
// 		}
// 	}

// 	if ( glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] <=0 )
// 	{
// 		printf("\nFinding last molecular ranges\n") ;
// 		oLOp->Find_Max_Mol_Range( (double*)prS, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (int)0 ) ;
// 	}

// 	// GET THE REFERENCE-STANDARD DEVIATION FOR THE SIGNAL
// 	fitParam.indxInitFit = glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] ;
// 	fitParam.indxEndFit  = glbParam->nBins_Ch[glbParam->chSel] -1 						;
// 	fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1;
// 	double std_ref = 0;
// 	for ( int j = fitParam.indxInitFit; j <= fitParam.indxEndFit; j++ )
// 		fitParam.std += pow( (prS[j] - prFit[j]), 2 );
// 	std_ref = sqrt( fitParam.std / (fitParam.nFit - 1) );

// 	// SET THE INITIAL FITTING RANGES FOR THE LAYER DETECTION ALGORITHM
// 	int 	deltaNorm ; // NUMBER OF BINS FOR THE RAYLEIGH FIT NORMALIZATION
// 	ReadAnalysisParameter( glbParam->FILE_SOFT_CODED_VALUES, "deltaNorm", "int", (int*)&deltaNorm )  ;
// 	fitParam.indxEndFit  = glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] - deltaNorm  ;
// 	fitParam.indxInitFit = fitParam.indxEndFit - deltaNorm ;
// 	fitParam.indxMidFit  = (int)round( (fitParam.indxInitFit + fitParam.indxEndFit)/2 ) ;
// 	fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1;
// 	ReadAnalysisParameter( glbParam->FILE_SOFT_CODED_VALUES, "std_factor_layer_mask", "float", (float*)&glbParam->std_factor_layer_mask ) ;

// 	memset( oLOp->cloudProfiles[glbParam->evSel].clouds_ON, 0, ( sizeof(int) * glbParam->nBins ) ) ;
// 	// FIRST STAGE OF CLOUD DETECTION
// 	while ( fitParam.indxInitFit >= glbParam->indxInitSig )
// 	{
// 		memset( (double*)prFit, 0, sizeof(double) * glbParam->nBins ) ;
// 		oLOp->RayleighFit( (double*)prS, (double*)dataMol->prMol_avg, glbParam->nBins, (strcFitParam*)&fitParam, (double*)prFit ) ;
// 		for (int j =fitParam.indxInitFit ; j < glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] ; j++)
// 		{
// 			if ( prS[j] >= (prFit[j] + std_ref * glbParam->std_factor_layer_mask ) )
// 				oLOp->cloudProfiles[glbParam->evSel].clouds_ON[j] = (int)BIN_CLOUD ;
// 		}
// 		fitParam.indxEndFit  = fitParam.indxEndFit - stepScanCloud ; // glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] - deltaNorm -i*stepScanCloud ; // STEP DOWN IN THE SCAN
// 		fitParam.indxInitFit = fitParam.indxEndFit - deltaNorm     ; // DELTA NOMALIZATION STABLISHED
// 		fitParam.indxMidFit  = (int)round( (fitParam.indxInitFit + fitParam.indxEndFit)/2 ) ;
// 		fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1;
// 	}

// 	FilterThinClouds( (strcGlobalParameters*)glbParam, (int*)oLOp->cloudProfiles[glbParam->evSel].clouds_ON ) ;

// 	// CHECK IF THERE ARE LAYERS DETECTED
// 	int sumLay =0 ;
// 	sum_int( (int*)&oLOp->cloudProfiles[glbParam->evSel].clouds_ON[0], (int)0, (int)(glbParam->nBins_Ch[glbParam->chSel]-1), (int*)&sumLay ) ;
// 	if ( sumLay >0 ) // IF THERE ARE CLOUDS DETECTED --> ADJUST THE BOUNDARIES
// 	{
// 		oLOp->GetCloudLimits( (strcGlobalParameters*)glbParam ) ; // ONLY LIMITS OF THE CLOUDS, *NOT* PBL!!!!
 
// 		//! CLOUD *BASE* HEIGH - START //////////////////////////////////////////////////////
// 		for (int  j =0; j <glbParam->nBins_Ch[glbParam->chSel] ; j++)		pr2[j] = (double) prS[j] * glbParam->r[j]*glbParam->r[j] ; // RANGE CORRECTED LIDAR SIGNAL

// 		// LOOP ACROSS THE CLOUDS DETECTED
// 			int 	indxMin =0 ;
// 			double	min 	=0 ;
// 			if ( oLOp->cloudProfiles[glbParam->evSel].indxInitClouds[0] <= glbParam->indxInitSig )
//   				 oLOp->cloudProfiles[glbParam->evSel].indxInitClouds[0] = glbParam->indxInitSig +1 ;
// 			findIndxMin( (double*)&pr2[0], (int)glbParam->indxInitSig, (int)oLOp->cloudProfiles[glbParam->evSel].indxInitClouds[0], (int*)&indxMin, (double*)&min ) ;

// 			for (int j =indxMin; j < oLOp->cloudProfiles[glbParam->evSel].indxInitClouds[0] ; j++)
// 				oLOp->cloudProfiles[glbParam->evSel].clouds_ON[j] = (int)1 ;

// 			oLOp->cloudProfiles[glbParam->evSel].indxInitClouds[0] = indxMin ;
// 			//! CLOUD *BASE* HEIGH - END //////////////////////////////////////////////////////

// 			//! CLOUD *TOP*  HEIGH - START //////////////////////////////////////////////////////
// 			fitParam.indxInitFit = oLOp->cloudProfiles[glbParam->evSel].indxInitClouds[0] - round(deltaNorm*1.5) ;
// 			fitParam.indxEndFit  = fitParam.indxInitFit + deltaNorm ;
// 			fitParam.indxMidFit  = (int)round( (fitParam.indxInitFit + fitParam.indxEndFit)/2 ) ;
// 			fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1;
// 			memset( (double*)prFit, 0, sizeof(double) * glbParam->nBins ) ;
// 			oLOp->RayleighFit( (double*)prS, (double*)dataMol->prMol_avg, glbParam->nBins, (strcFitParam*)&fitParam, (double*)prFit ) ;

// 			for (int j =fitParam.indxMidFit ; j < glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel]; j++ )
// 			{
// 				if ( prS[j] < prFit[j] )
// 				{
// 					oLOp->cloudProfiles[glbParam->evSel].indxEndClouds[0] = j ;
// 					break ;
// 				}
// 			}
// 			//! CLOUD *TOP*  HEIGH - STOP //////////////////////////////////////////////////////

// 		oLOp->GetCloudLimits( (strcGlobalParameters*)glbParam ) ;
// 	} // if ( sumLay >0 ) 
// 	else // NO CLOUDS DETECTED
// 		memset( (double*)&oLOp->cloudProfiles[glbParam->evSel].clouds_ON[0], 0, sizeof(int)*glbParam->nBins ) ;

// 	//! PBL HEIGH - START
// 	oLOp->GetCloudLimits( (strcGlobalParameters*)glbParam ) ; // ONLY LIMITS OF THE CLOUDS, *NOT* PBL!!!!
// 	if ( strcmp( strCompPBL.c_str(), "YES" ) ==0 )
// 	{
// 		printf(" --> Getting PBL layer") ;
// //! SEGUIR ACA!!!!!!!!!!!!1 
// 		if ( oLOp->cloudProfiles[glbParam->evSel].nClouds ==0  )
// 		{
// 			printf(" fit between the start range and the maximun range ") ;
// 			int a = (int)round(glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] - glbParam->indxInitSig) /2 ;
// 			int b = (int)round(glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] - glbParam->indxInitSig) /4 ;
// 			fitParam.indxInitFit = (int)round ( glbParam->indxInitSig + b ) ;
// 			fitParam.indxEndFit  = fitParam.indxInitFit + a ;
// 			fitParam.indxMidFit  = (int)round( (fitParam.indxInitFit + fitParam.indxEndFit)/2 ) ;
// 			fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1;
// 		}
// 		else if ( oLOp->cloudProfiles[glbParam->evSel].nClouds ==1 )
// 		{
// 			printf(" fit around the first cloud ") ;
// 			fitParam.indxInitFit = oLOp->cloudProfiles[glbParam->evSel].indxInitClouds[0] - round(deltaNorm*1.5) ;
// 			fitParam.indxEndFit  = fitParam.indxInitFit + deltaNorm ;
// 			fitParam.indxMidFit  = (int)round( (fitParam.indxInitFit + fitParam.indxEndFit)/2 ) ;
// 			fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1;
// 		}
// /*
// 		if ( sumLay >0 )
// 		{
// 			printf(" fit before the first cloud ") ;
// 			fitParam.indxInitFit = oLOp->cloudProfiles[glbParam->evSel].indxInitClouds[0] - round(deltaNorm*1.5) ;
// 			fitParam.indxEndFit  = fitParam.indxInitFit + deltaNorm ;
// 			fitParam.indxMidFit  = (int)round( (fitParam.indxInitFit + fitParam.indxEndFit)/2 ) ;
// 			fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1;
// 		}
// 		else
// 		{
// 			printf(" fit between the start range and the maximun range ") ;
// 			fitParam.indxEndFit  = (int)round ( (glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] - glbParam->indxInitSig)*3/4 + glbParam->indxInitSig ) ;
// 			fitParam.indxInitFit = fitParam.indxEndFit - deltaNorm ;
// 			fitParam.indxMidFit  = (int)round( (fitParam.indxInitFit + fitParam.indxEndFit)/2 ) ;
// 			fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1;
// 		}
// */
// 			printf("-\n fit at %f-%f m\n", fitParam.indxInitFit *glbParam->dr, fitParam.indxEndFit *glbParam->dr ) ;

// 			memset( (double*)prFit, 0, sizeof(double) * glbParam->nBins ) ;
// 			oLOp->RayleighFit( (double*)prS, (double*)dataMol->prMol_avg, glbParam->nBins, (strcFitParam*)&fitParam, (double*)prFit ) ;

// 			for (int i =glbParam->indxInitSig ; i < fitParam.indxEndFit ; i++)
// 			{
// 				if ( prS[i] > (prFit[i] + std_ref * glbParam->std_factor_layer_mask ) )
// 					oLOp->cloudProfiles[glbParam->evSel].clouds_ON[i] = (int)1 ;
// 				else
// 					oLOp->cloudProfiles[glbParam->evSel].clouds_ON[i] = (int)0 ;
// 			}
// 			for (int i =0 ; i <=glbParam->indxInitSig ; i++)
// 					oLOp->cloudProfiles[glbParam->evSel].clouds_ON[i] = (int)1 ;
// 	} // if ( strcmp( strCompPBL.c_str(), "YES" ) ==0 )
// 	//! PBL HEIGH - STOP

// 	FilterThinClouds( (strcGlobalParameters*)glbParam, (int*)oLOp->cloudProfiles[glbParam->evSel].clouds_ON ) ;
// 	oLOp->GetCloudLimits( (strcGlobalParameters*)glbParam ) ;
// }

/*
void CDataLevel_1::FilterThinClouds( strcGlobalParameters *glbParam, double *layer_mask )
{
	for (int b =0 ; b <=(glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] -CLOUD_MIN_THICK -1) ; b++)
	{
		if ( (layer_mask[b]   == (double)0		   ) && 
			 (layer_mask[b+1] == (double)BIN_CLOUD ) ) 
		{
			double sum_lay =0 ;
			sum( (double*)layer_mask, (int)(b+1), (int)(b+CLOUD_MIN_THICK), (double*)&sum_lay ) ;

			if ( sum_lay < CLOUD_MIN_THICK )
			{
				for (int k =(b+1) ; k <=(b+CLOUD_MIN_THICK); k++)
						layer_mask[k] = (double)0.0 ;
			}
			else if ( layer_mask[b+CLOUD_MIN_THICK +1] == (double)0.0 )
			{
				for (int k =(b+1) ; k <=(b+CLOUD_MIN_THICK); k++)
					layer_mask[k] = (double)0.0 ;
			}
		}
	}
}

void CDataLevel_1::FilterThinClouds( strcGlobalParameters *glbParam, int *layer_mask )
{
	for (int b =0 ; b <=(glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] -CLOUD_MIN_THICK -1) ; b++)
	{
		if ( (layer_mask[b]   == (int)0		    ) && 
			 (layer_mask[b+1] == (int)BIN_CLOUD ) ) 
		{
			int sum_lay =0 ;
			sum_int( (int*)layer_mask, (int)(b+1), (int)(b+CLOUD_MIN_THICK), (int*)&sum_lay ) ;

			if ( sum_lay < CLOUD_MIN_THICK )
			{
				for (int k =(b+1) ; k <=(b+CLOUD_MIN_THICK); k++)
						layer_mask[k] = (int)0 ;
			}
			else if ( layer_mask[b+CLOUD_MIN_THICK +1] == (int)0 )
			{
				for (int k =(b+1) ; k <=(b+CLOUD_MIN_THICK); k++)
					layer_mask[k] = (int)0 ;
			}
		}
	}
}
*/

/*
void CDataLevel_1::GetCloudLimits( strcGlobalParameters *glbParam, int *clouds_ON )
{
	int maxCloud, indxMax ;
	findIndxMax_void( (int*)&clouds_ON[0], (const char*)"int", 0, (glbParam->nBins-1), (int*)&indxMax, (int*)&maxCloud ) ;
	// ( maxCloud == 1 ) ? printf("\nThere *ARE* clouds at event number %d", glbParam->evSel) : printf("There are *NOT* clouds at event number %d", glbParam->evSel) ;

	// BASIC CLOUDLESS SETTINGS
	memset( cloudProfiles[glbParam->evSel].indxInitClouds, 0, ( sizeof(int) * NMAXCLOUDS ) ) ;
	memset( cloudProfiles[glbParam->evSel].indxEndClouds , 0, ( sizeof(int) * NMAXCLOUDS ) ) ;
	memset( indxMol[glbParam->evSel].indxInicMol, 0, ( sizeof(int) * MAX_MOL_RANGES  ) ) ;
	memset( indxMol[glbParam->evSel].indxEndMol , 0, ( sizeof(int) * MAX_MOL_RANGES  ) ) ;
	cloudProfiles[glbParam->evSel].nClouds  = 0 ; // USED AS INDEX AND THEN, AS TOTAL NUMBER.
	indxMol[glbParam->evSel].nMolRanges 	= 0 ; // USED AS INDEX AND THEN, AS TOTAL NUMBER.

	if ( maxCloud ==1 ) // IF THERE ARE CLOUDS IN THE EVENT UNDER ANALYSIS --> GETS THEIRS LIMITS
	{
		memset( (int*)dco, 0, (sizeof(int) * glbParam->nBins) ) ;

		for ( int i =0 ; i<=glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] ; i++ )
			dco[i] = (int)( clouds_ON[i+1] - clouds_ON[i] ) ;

		// CHECK CONSISTENCY --> nInicCloud = nEndCloud
		int nInicCloud=0, nEndCloud=0 ;
		for (int i =0 ; i <=glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] ; i++)
		{
			if ( dco[i] ==(int)BIN_CLOUD )
				nInicCloud++ ;
			else if ( dco[i]==(int)(-BIN_CLOUD) )
				nEndCloud++ ;
		}
		if ( (nInicCloud + 1) == nEndCloud ) // IN CASE OF CLOUDY PROFILE --> GET THEIR LIMITS
		{
			// CLOUDS DETECTION
			for( int i=0 ; i <=(glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel]) ; i++ )
			{
				if ( cloudProfiles[glbParam->evSel].nClouds < NMAXCLOUDS )
				{
					if( dco[i] == (int)BIN_CLOUD ) // INIT CLOUD
					{
						cloudProfiles[glbParam->evSel].indxInitClouds[cloudProfiles[glbParam->evSel].nClouds] = i ;  // USED AS INDEX (INITIALIZED WITH 0) AND THEN, AS TOTAL NUMBER.
						for (int j =i ; j <=glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] ; j++)
						{	// SEARCH THE END OF THE CLOUD
							if( dco[j] == (int)(-BIN_CLOUD) ) // END CLOUD
							{
								cloudProfiles[glbParam->evSel].indxEndClouds[cloudProfiles[glbParam->evSel].nClouds] = j -1;
								cloudProfiles[glbParam->evSel].nClouds++ ; // COUNT CLOUDS
								i = j+1 ;
								break ;
							}
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
						indxMol[glbParam->evSel].indxEndMol[i]  = glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] ;
					else
						indxMol[glbParam->evSel].indxEndMol[i]  = cloudProfiles[glbParam->evSel].indxInitClouds[i] -1 ;
				}
			}
		} // if ( nInicCloud == nEndCloud )
		else
			printf("\033[31m\nGetCloudLim(...) --> Inconsistent cloud profile detected (nInicCloud: %d, nEndCloud: %d) \n\033[0m", nInicCloud, nEndCloud ) ;
	} // if ( maxCloud ==1 ) // IF THERE ARE CLOUDS --> GETS THEIRS LIMITS
	else // NO CLOUDS DETECTED
	{
		cloudProfiles[glbParam->evSel].nClouds = 0 ;
		indxMol[glbParam->evSel].nMolRanges    = 1 ;

		// MOLECULAR RANGES SETTINGS
		if ( indxMol[glbParam->evSel].nMolRanges ==1 )
		{
			double 	heightRef_Inversion_ASL, heightRef_Inversion_Start_ASL, heightRef_Inversion_Stop_ASL ;
			ReadAnalysisParameter( glbParam->FILE_PARAMETERS, "heightRef_Inversion_Start_ASL" , "double", (int*)&heightRef_Inversion_Start_ASL ) ;
			ReadAnalysisParameter( glbParam->FILE_PARAMETERS, "heightRef_Inversion_Stop_ASL"  , "double", (int*)&heightRef_Inversion_Stop_ASL  ) ;
			heightRef_Inversion_ASL = ( fabs(heightRef_Inversion_Start_ASL) + fabs(heightRef_Inversion_Stop_ASL) )/2 ;

			indxMol[glbParam->evSel].indxRef = (int)round(heightRef_Inversion_ASL /glbParam->dzr) ;
				if ( indxMol[glbParam->evSel].indxRef > (glbParam->nBins-1) )
					indxMol[glbParam->evSel].indxRef = glbParam->nBins -10 ;

			for (int i =0; i < NMAXCLOUDS ; i++)
			{
				cloudProfiles[glbParam->evSel].indxInitClouds[i] = 0 ;
				cloudProfiles[glbParam->evSel].indxEndClouds [i] = 0 ;
				indxMol[glbParam->evSel].indxInicMol[i] = 0 ;
				indxMol[glbParam->evSel].indxEndMol[i]  = glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] ;
			}
		} // if ( indxMol[glbParam->evSel].nMolRanges ==1 )
	}

//! NO PREGUNTO SI SE CALCULO LA PBL PORQUE LO ESTOY HACIENDO SIEMPRE
	for ( int i =0 ; i<=glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] ; i++ )
	{
		if ( clouds_ON[i] == (int)0 )
		{
			cloudProfiles[glbParam->evSel].indxEndPBL  = (int)i ;
			cloudProfiles[glbParam->evSel].rangeEndPBL = (double) i*glbParam->dr ;
			break;
		}
	}

	printf("\n\t CDataLevel_1::GetCloudLimits() " ) ;
	printf("\n\t cloudProfiles[%d].nClouds: %d "			, glbParam->evSel, cloudProfiles[glbParam->evSel].nClouds 			) ;
	printf("\n\t cloudProfiles[%d].indxInitClouds[0]: %d "	, glbParam->evSel, cloudProfiles[glbParam->evSel].indxInitClouds[0] ) ;
	printf("\n\t cloudProfiles[%d].indxEndClouds[0] : %d "	, glbParam->evSel, cloudProfiles[glbParam->evSel].indxEndClouds[0]  ) ;
	printf("\n\t cloudProfiles[%d].indxEndPBL: %d "    		, glbParam->evSel, cloudProfiles[glbParam->evSel].indxEndPBL    	) ;
	printf("\n\t cloudProfiles[%d].rangeEndPBL: %lf \n"		, glbParam->evSel, cloudProfiles[glbParam->evSel].rangeEndPBL 		) ;
	// printf("\n\t cloudProfiles[%d].VOD_cloud[0]     : %lf ", glbParam->evSel, cloudProfiles[glbParam->evSel].VOD_cloud[0]	  ) ;
} // void CDataLevel_1::GetCloudLimits( strcGlobalParameters *glbParam )
*/

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
		fprintf( fpDB, "%d %d ", Raw_Data_Start_Time_AVG[t], oLOp->cloudProfiles[t].nClouds ) ;

		for ( int n =0; n < oLOp->cloudProfiles[t].nClouds ; n++)
			fprintf( fpDB, "%5.3lf %5.3lf ", oLOp->cloudProfiles[t].indxInitClouds[n] *glbParam->dr,
				   (oLOp->cloudProfiles[t].indxEndClouds[n] - oLOp->cloudProfiles[t].indxInitClouds[n]) *glbParam->dr ) ;

		fprintf( fpDB, "\n" ) ;
	}
	fclose(fpDB) ;
	printf( "\n\nDone with the cloud data base file: %s \n", Path_File_Out ) ;
}



/*
void CDataLevel_1::ScanCloud_RayleighFit ( const double *pr, strcGlobalParameters *glbParam, strcMolecularData *dataMol )
{ // pr IS ALREADY BACKGROUND CORRECTED
	if ( avg_Points_Cloud_Mask >1 )
	{
		smooth( (double*)pr			, (int)0, (int)(glbParam->nBins-1), (int)avg_Points_Cloud_Mask, (double*)prS ) ;
		smooth( (double*)glbParam->r, (int)0, (int)(glbParam->nBins-1), (int)avg_Points_Cloud_Mask, (double*)glbParam->r_avg ) ;

		smooth( (double*)dataMol->prMol, (int)0, (int)(glbParam->nBins-1), (int)avg_Points_Cloud_Mask, (double*)dataMol->prMol_avg ) ;
	}
	else
	{
		for( int b=0 ; b <glbParam->nBins ; b++ )
		{
			prS[b] 	  				= (double)pr[b] 	;	
			dataMol->prMol_avg[b] 	= dataMol->prMol[b] ;
			glbParam->r_avg[b] 		= glbParam->r[b] 	;
		}
	}
	for( int b=0 ; b <glbParam->nBins ; b++ )
	{
		prprm [b] = (double)(prS[b] * dataMol->prMol_avg[b]) ;
		prmprm[b] = (double)(dataMol->prMol_avg[b] * dataMol->prMol_avg[b]) ;
	}
	nMaxLoop = (int)0 ;

 // ERROR REFERENCE CALCULATION
	oLOp->Find_Max_Range( (double*)prS, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam) ;
	fitParam.indxInitFit = dataMol->last_Indx_Mol_Low  ;
	fitParam.indxEndFit  = dataMol->last_Indx_Mol_High ;
	fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1;
do
{ // TODO
	//  oLOp->Fit( (double*)prS, (double*)dataMol->prMol_avg, glbParam->nBins, "wB", "NOTall", (strcFitParam*)&fitParam, (double*)prFit ) ;
		oLOp->Fit( (double*)prS, (double*)dataMol->prMol_avg, glbParam->nBins_Ch[glbParam->indxWL_PDL1], "wB", "NOTall", (strcFitParam*)&fitParam, (double*)prFit ) ;
	biasRef = fitParam.b ;
	errRefBkg = sqrt ( fitParam.squared_sum_fit/(fitParam.nFit -1) ) ;

	fitParam.indxEndFit  = fitParam.indxEndFit -1 ;
	fitParam.indxInitFit = fitParam.indxEndFit - fitParam.nFit ; // ALREADY DEFINED IN THE Find_Max_Range()
} while( (fitParam.m <0) ) ;
// } while( ((fitParam.m <0) || (biasRef <0)) ) ; // && (fitParam.indxInitFit>=glbParam->indxInitSig)

	if ( fitParam.indxInitFit < glbParam->indxInitSig )
	{
		printf("\n ScanCloud_RayleighFit(...):  fitParam.indxInitFit < glbParam->indxInitSig while finidng the reference range... bye \n") ;
		printf(" ScanCloud_RayleighFit(...):  fitParam.m= %lf \t fitParam.b= %lf \n", fitParam.m, fitParam.b ) ;
		exit(1) ;
	}

	for ( int i=0 ; i<nScanMax ; i++ )
	{ 	// RESET SE_lay
		for (int k =0; k <glbParam->nBins ; k++)
			SE_lay[i][k] =(double) 0.0 ;
	}

	fitParam.indxInitFit = (int) glbParam->indxInitSig ;
	fitParam.indxEndFit  = (int) glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel]  ;
		for ( int i=0 ; i<nScanMax ; i++ ) // ------------------------------------------------------------------------------------------
		{
			for (int b =0; b <glbParam->nBins ; b++)
			{
				cloudProfiles[glbParam->evSel].test_1[b] 	=(double) 0.0 ;
				cloudProfiles[glbParam->evSel].clouds_ON[b] =(int)    0.0 ;
				prFit[b] 							 	 	=(double) 0.0 ;
			}
CLOUD_FIT:	// PERFORM THE FIT
			fitParam.nFit = fitParam.indxEndFit - fitParam.indxInitFit +1;
			fitParam.indxMidFit = fitParam.indxInitFit + (int)round(fitParam.nFit/2) ;
			sppm = (double)0.0 ;	spm = (double)0.0 ;		spmpm = (double)0.0 ;	 m = (double)0.0 ;
// printf("\n\n(2)ScanCloud_RayleighFit() - fitParam.indxInitFit= %d \t fitParam.indxEndFit= %d\n\n", fitParam.indxInitFit, fitParam.indxEndFit) ;
			sum( (double*)prprm    			, (int)fitParam.indxInitFit, (int)fitParam.indxEndFit, (double*)&sppm ) ;
			sum( (double*)dataMol->prMol_avg, (int)fitParam.indxInitFit, (int)fitParam.indxEndFit, (double*)&spm  ) ;
			sum( (double*)prmprm			, (int)fitParam.indxInitFit, (int)fitParam.indxEndFit, (double*)&spmpm) ;
				m = (double)((sppm - biasRef * spm) /spmpm) ;
				for( int b=0 ; b <=glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] ; b++ ) // for( int b=glbParam->indxInitSig ; b <=glbParam->indxEndSig ; b++ ) // for( int b=fitParam.indxInitFit ; b <=fitParam.indxEndFit ; b++ )
					prFit[b] = m * dataMol->prMol_avg[b] + biasRef ;
// printf("(3)ScanCloud_RayleighFit() - fitParam.indxInitFit= %d \t fitParam.indxEndFit= %d\n", fitParam.indxInitFit, fitParam.indxEndFit) ;
			// OBTAIN THE RMS ERROR OF THE FITTED SIGNAL
			errFitStage = 0 ;
			for( int b=fitParam.indxInitFit ; b <=fitParam.indxEndFit ; b++ )
				errFitStage = (double)( errFitStage + pow( (prFit[b]-prS[b]), 2 ) ) ;
			errFitStage = (double)sqrt( errFitStage /(fitParam.nFit -1) ) ;
			// CHECK IF THIS WAS THE LAST FIT...
			if ( (errFitStage <= (errRefBkg *errScanCheckFactor )) ) // && ( (m*fitParam.m) >0 )
			{
				scanNumExit =i;
				// printf("\n(%d) ScanCloud_RayleighFit(...): SALIENDO *OK* EN nScan = %d --> fit=%d-%d -- errFit / errRefBkg = %f / %f ",glbParam->evSel, i, fitParam.indxInitFit, fitParam.indxEndFit, errFitStage, errRefBkg ) ;
				break ; // EXTIS THE SCAN --> for ( int i=0 ; i<nScanMax ; i++ )
			}
			else // SET THE CLOUDS CANDIDATES
			{
				fitParam.indxInitFit = (int)fitParam.indxInitFit + (int)stepScanCloud ; // PREPARED FOR THE NEXT SCAN
				if ( m>0 ) // IF THE FIT IS OK
				{
					// SET THE CLOUDS/PLUMES CANDIDATES
					for( int b=glbParam->indxInitSig ; b<=glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] ; b++ )
					{
						if( prS[b] > ( prFit[b] + errRefBkg *errFactor ) ) // errFactor = 2.0
							SE_lay[i][b] = (double)pow( (prS[b] - prFit[b] - errRefBkg *errFactor), 2) ; // SE_lay = D(i, r) --> SE_lay(i, r)  IN THE PAPER
					}

					// REMOVE THE BINS RELATED TO THE PBL IF "COMPUTE_PBL_MASK = NO" IS SET IN THE SETTING FILE
					// REMOVE THE FIRST CLUSTER OF "BIN_CLOUD" IN SE_lay[i][:] VALUES (PBL BINS)
					if ( strcmp( strCompPBL.c_str(), "NO" ) ==0 )
                	{
						first_cluster_ON =0 ;
						for( int b=(glbParam->indxInitSig) ; b<=glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] ; b++ )
						{
							if ( (SE_lay[i][b] >(double)0) )	{
								SE_lay[i][b] =(double)0.0 ; // ELIMINATE THE BINS OF THE ABL.
								first_cluster_ON  =(double)1   ;	}
							else 			{
								if ( first_cluster_ON ==1 ) // SE_lay[i][b] ==0 AND THE FIRST CLUSTER OF BINS WERE ALREADY RESETED.
									break ; }
						}
					}

					// INCREASE THE COUNTER IF A CLOUD DETECTED WAS DETECTED IN THE PROFILE
					for( int b=glbParam->indxInitSig ; b<=glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] ; b++ ) 
					{
						if( SE_lay[i][b] > 0 )
						{
							nMaxLoop++ ;
							break ;
						}
					} // for( int b=glbParam->indxInitSig ; b<=glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] ; b++ ) 
				} // if ( m>0 )
				else // m<0
				{
					// printf("\nScanCloud_RayleighFit(...): m<0 --> FIT NOT TAKEN INTO ACCOUNT; \n") ;
					// TODO: ALGO SALIO MAL EN EL FITEO 
					fitParam.indxInitFit = (int)fitParam.indxInitFit + (int)stepScanCloud ; // PREPARED FOR THE NEXT SCAN
					goto CLOUD_FIT ;
					// break;
				}
				if ( fitParam.indxInitFit >= fitParam.indxEndFit )
				{
					printf("\nScanCloud_RayleighFit(...): fitParam.indxInitFit >= fitParam.indxEndFit --> break; \n") ;
					break;
					// TODO: ALGO SALIO MAL EN EL ESCANEO ---> HACER ALGO
				}
			}  // END OF "SET THE CLOUDS CANDIDATES"
		} // for ( int i=0 ; i<nScanMax ; i++ ) ------------------------------------------------------------------------------------------

		if (nMaxLoop == 0)
		{
			// printf("\t Completely pure Rayleigh lidar signal\t") ;
			nMaxLoop = 1 ;
		}
		// for ( int l =0; l < nScanMax ; l++)
		// {
		// 	// smooth( (double*)&(SE_lay[l][0]), (int)0, (int)(glbParam->nBins-1), (int)avg_Points_Cloud_Mask, (double*)&(cloudProfiles[glbParam->evSel].test_2[0]) ) ;
		// 	smooth( (double*)&(SE_lay[l][0]), (int)0, (int)(glbParam->nBins-1), (int)5, (double*)&(cloudProfiles[glbParam->evSel].test_2[0]) ) ;
		// 	for( int b=0 ; b <glbParam->nBins ; b++ )
		// 		SE_lay[l][b] = cloudProfiles[glbParam->evSel].test_2[b] ;
		// }

		for( int b=0 ; b <glbParam->nBins ; b++ )
		{
			for( int s=0 ; s <nScanMax ; s++ )
				cloudProfiles[glbParam->evSel].test_1[b] = (double)(cloudProfiles[glbParam->evSel].test_1[b] + (double)SE_lay[s][b]) ; // cloudProfiles[glbParam->evSel].test_1[b] = (double)sqrt( cloudProfiles[glbParam->evSel].test_1[b] /SE_lay_counts[b] ) ; 

			cloudProfiles[glbParam->evSel].test_1[b] = (double)sqrt( cloudProfiles[glbParam->evSel].test_1[b] /nMaxLoop ) ; // test1: RMSE_lay // cloudProfiles[glbParam->evSel].test_1[b]    = (double)    ( cloudProfiles[glbParam->evSel].test_1[b] /nMaxLoop ) ;
		}

		// SET THE CLOUD-MASK PROFILE FOR THE PROFILE UNDER ANALYSIS
		for( int b=0 ; b <glbParam->nBins ; b++ )
		{
			if ( cloudProfiles[glbParam->evSel].test_1[b] > (double)(errRefBkg *thresholdFactor) )
				cloudProfiles[glbParam->evSel].clouds_ON[b] = (int)BIN_CLOUD ;
			else
				cloudProfiles[glbParam->evSel].clouds_ON[b] = (int)0 ;
		}

		// CHECK CLOUDS TESTS:
		// REMOVE CLOUDS THINNER THAN CLOUD_MIN_THICK BINS.
		for (int b =0 ; b <=(glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] -CLOUD_MIN_THICK -1) ; b++)
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
		for (int b =0 ; b <=(glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] -CLOUD_MIN_THICK -1) ; b++)
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
		cloudProfiles[glbParam->evSel].clouds_ON[0] 				   = (int)0 ; // NO -> THE ABL IS CONSIDERED AS CLOUD. FIRST BIN IS SET TO ZERO BE COHERENT FOR THE INDEXES COMPUTATION.
		cloudProfiles[glbParam->evSel].clouds_ON[glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel]] = (int)0 ; // LAST BIN IS SET TO ONE BE COHERENT FOR THE INDEXES COMPUTATION.
		// DONE WITH cloudProfile, NOW GET THE CLOUD LIMITS
		GetCloudLimits( (strcGlobalParameters*)glbParam ) ;

		// REMOVE THE FAKE CLOUDS BASED ON ITS RMS ERROR
		for( int i=0 ; i <cloudProfiles[glbParam->evSel].nClouds ; i++ )
		{
			fitParam.indxInitFit = cloudProfiles[glbParam->evSel].indxInitClouds[i] -DELTA_RANGE_LIM_BINS ; // cloudProfiles[glbParam->evSel].indxInitClouds[0] 					   -DELTA_RANGE_LIM_BINS ; // 
			fitParam.indxEndFit  = cloudProfiles[glbParam->evSel].indxEndClouds[i]  +DELTA_RANGE_LIM_BINS ; // cloudProfiles[glbParam->evSel].indxEndClouds[cloudProfiles[glbParam->evSel].nClouds-1]  +DELTA_RANGE_LIM_BINS ; // 
			fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1;
			if( (fitParam.indxInitFit > glbParam->indxInitSig) && ( fitParam.indxInitFit < fitParam.indxEndFit ) )
			{
					oLOp->Fit( (double*)prS, (double*)dataMol->prMol_avg, glbParam->nBins, "wB", "NOTall", (strcFitParam*)&fitParam, (double*)prFit ) ;	
				errCloud =0 ;
				errCloud = (double)sqrt(fitParam.squared_sum_fit/(fitParam.nFit -1)) ; // errCloud = (double)(fitParam.squared_sum_fit/(fitParam.nFit -1)) ;

				if ( errCloud <= (errRefBkg *errCloudCheckFactor) ) // errCloudCheckFactor =0
				{
					for( int b =(fitParam.indxInitFit) ; b <=(fitParam.indxEndFit) ; b++ ) // for( int b =(fitParam.indxInitFit-2) ; b <=(fitParam.indxEndFit+2) ; b++ ) // 
						cloudProfiles[glbParam->evSel].clouds_ON[b] =0;
				}
				// else
				// 	printf("SC_RF() - (%d) Clouds: errRefBkg: %2.3lf \t errCloud: %2.3lf --> Cloud: %d-%d --> Fit: %d-%d\n", glbParam->evSel, errRefBkg, errCloud, cloudProfiles[glbParam->evSel].indxInitClouds[i], cloudProfiles[glbParam->evSel].indxEndClouds[i], cloudProfiles[glbParam->evSel].indxInitClouds[i] -DELTA_RANGE_LIM_BINS, cloudProfiles[glbParam->evSel].indxEndClouds[i]  +DELTA_RANGE_LIM_BINS) ;
			
				if ( strcmp(ifODcut, "YES") ==0 )
					oLOp->ODcut( (double*)prS, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (strcFitParam*)&fitParam, (int*)cloudProfiles[glbParam->evSel].clouds_ON ) ;
			} // if( fitParam.indxInitFit > glbParam->indxInitSig )
		} // for( int i=0 ; i <cloudProfiles[glbParam->evSel].nClouds ; i++ )
		// if (cloudProfiles[glbParam->evSel].nClouds ==0)
		// 	printf("\nSC_RF() - (%d) NO Clouds detected\n", glbParam->evSel) ;

		if ( strcmp( strCompPBL.c_str(), "YES" ) ==0 )
		{
			for (int i =1; i < glbParam->indxInitSig; i++)
				cloudProfiles[glbParam->evSel].clouds_ON[i] = BIN_CLOUD ;
		}
		GetCloudLimits( (strcGlobalParameters*)glbParam ) ;
} // void CDataLevel_1::ScanCloud_RayleighFit ( const double *pr, strcGlobalParameters *glbParam, strcMolecularData *dataMol )
*/