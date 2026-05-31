
#include "CDataLevel_1.hpp"  

CDataLevel_1::CDataLevel_1( strcGlobalParameters *glbParam )
{
	ReadAnalysisParameter( glbParam->FILE_PARAMETERS, "ifODcut", "string", (char*)ifODcut, sizeof(ifODcut) ) ;
	char compute_layer_mask_buf[128] = {0} ;
	ReadAnalysisParameter( glbParam->FILE_SOFT_CODED_VALUES, "COMPUTE_LAYER_MASK", "string", compute_layer_mask_buf, sizeof(compute_layer_mask_buf) ) ;
	strCompCM.assign(compute_layer_mask_buf) ;
	char compute_pbl_mask_buf[128] = {0} ;
	ReadAnalysisParameter( glbParam->FILE_SOFT_CODED_VALUES, "COMPUTE_PBL_MASK", "string", compute_pbl_mask_buf, sizeof(compute_pbl_mask_buf) ) ;
	strCompPBL.assign(compute_pbl_mask_buf) ;

	oLOp = (CLidar_Operations*) new CLidar_Operations( (strcGlobalParameters*)glbParam ) ;
}
	
CDataLevel_1::~CDataLevel_1()
{
	delete oLOp ;
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
