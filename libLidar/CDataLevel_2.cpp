
#include "CDataLevel_2.hpp"  

CDataLevel_2::CDataLevel_2( strcGlobalParameters *glbParam )
{
	LRM		   = 8*M_PI/3 ;
	pr2        = (double***) new double**[glbParam->nEventsAVG] ;
    alpha_Aer  = (double***) new double**[glbParam->nEventsAVG] ;
    beta_Aer   = (double***) new double**[glbParam->nEventsAVG] ;
    AOD_LR	   = (double**)  new double *[glbParam->nEventsAVG] ;
    pr		   = (double**)  new double *[glbParam->nEventsAVG] ;
    dummy      = (double*)   new double  [glbParam->nBins ]     ;	memset( (double*)dummy, 0, sizeof(double)*glbParam->nBins ) ;
    pr2_s      = (double*)   new double  [glbParam->nBins ]     ;	memset( (double*)pr2_s, 0, sizeof(double)*glbParam->nBins ) ;

// FERNALD INVERSION VECTORS
	pr2n 			= (double*) new double[glbParam->nBins ] ;
	phi  			= (double*) new double[glbParam->nBins ] ;
	p 	  			= (double*) new double[glbParam->nBins ] ;
	ip	   			= (double*) new double[glbParam->nBins ] ;
	ipN   			= (double*) new double[glbParam->nBins ] ;
	betaT  			= (double*) new double[glbParam->nBins ] ;
	betaMol_Fit		= (double*) new double[glbParam->nBins ] ;
	intAlphaMol_r  	= (double*) new double[glbParam->nBins] ;
	// pr2Fit 		= (double*) new double[glbParam->nBins] ;

	for ( int e=0 ; e <glbParam->nEventsAVG ; e++ )
	{
		pr2[e] 		 = (double**) new double*[glbParam->nCh] ;
		for ( int c=0 ; c <glbParam->nCh ; c++ )
		{
			pr2[e][c] 		= (double*) new double[glbParam->nBins] ;
			for(int b =0 ; b <glbParam->nBins ; b++)
				pr2      [e][c][b] = (double)0 ;
		}
	}

	data_File_L2 = (double***) new double**[glbParam->nEventsAVG] ;
	for ( int e=0 ; e <glbParam->nEventsAVG ; e++ )
	{
		data_File_L2[e] = (double**) new double*[glbParam->nCh] ;
		for ( int c=0 ; c <glbParam->nCh ; c++ )
		{
			data_File_L2[e][c] = (double*) new double[glbParam->nBins] ;
				for(int b =0 ; b <glbParam->nBins ; b++)
					data_File_L2[e][c][b] = (double)0.0 ;
		}
	}
	Start_Time_AVG_L2 = (int*) new int [glbParam->nEventsAVG] ;   memset( (int*)Start_Time_AVG_L2, 0, (sizeof(int)*glbParam->nEventsAVG) ) ;
	Stop_Time_AVG_L2  = (int*) new int [glbParam->nEventsAVG] ;   memset( (int*)Stop_Time_AVG_L2 , 0, (sizeof(int)*glbParam->nEventsAVG) ) ;

	nLRs = ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "LR ", "double", (double*)LR ) ;
	for ( int e=0 ; e <glbParam->nEventsAVG ; e++ )
	{
		pr[e]		 = (double*) new double[glbParam->nBins] ;
		alpha_Aer[e] = (double**) new double*[nLRs] ;
		beta_Aer[e]  = (double**) new double*[nLRs] ;
		AOD_LR[e]    = (double*)  new double [nLRs] ;

		for ( int l=0 ; l <nLRs ; l++ )
		{
			alpha_Aer[e][l] = (double*) new double[glbParam->nBins] ;
			beta_Aer[e][l]  = (double*) new double[glbParam->nBins] ;

			for(int b =0 ; b <glbParam->nBins ; b++)
			{
				alpha_Aer[e][l][b] = (double)0 ;
				beta_Aer [e][l][b] = (double)0 ;
			}
		}
	}
	indxRef_Fernald 	  = (int*) new int [ glbParam->nEventsAVG ] ;
	indxRef_Fernald_Start = (int*) new int [ glbParam->nEventsAVG ] ;
	indxRef_Fernald_Stop  = (int*) new int [ glbParam->nEventsAVG ] ;

	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "R_ref", "double" , (double*)&R_ref ) ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "avg_Points_Fernald", "int", (int*)glbParam->avg_Points_Fernald ) ;

	aeronet_file = (char*) new char [ 300 ] ;
    ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"AERONET_FILE", (const char*)"string", (char*)aeronet_file ) ;
    if ( strcmp(aeronet_file, "NOT_FOUND") ==0 )
		printf("\nNo AERONET data set in the configuration file. Using fixed LRs\n") ;
	else
		printf("\nAERONET data file to use: %s\n", aeronet_file) ;

	oLOp = (CLidar_Operations*) new CLidar_Operations( (strcGlobalParameters*)glbParam ) ;
}
	
CDataLevel_2::~CDataLevel_2()
{

}

void CDataLevel_2::FernaldInversion( strcGlobalParameters *glbParam, strcMolecularData *dataMol)
{
	LRM = (double) dataMol->LR_mol ;

	if ( glbParam->avg_Points_Fernald[glbParam->chSel] >1 )
	{
		smooth( (double*)&dataMol->alphaMol[0], (int)0, (int)(glbParam->nBins-1), (int)glbParam->avg_Points_Fernald[glbParam->chSel], (double*)&dataMol->alphaMol_avg[0] ) ;
		smooth( (double*)&dataMol->betaMol [0], (int)0, (int)(glbParam->nBins-1), (int)glbParam->avg_Points_Fernald[glbParam->chSel], (double*)&dataMol->betaMol_avg [0] ) ;
		smooth( (double*)&dataMol->pr2Mol  [0], (int)0, (int)(glbParam->nBins-1), (int)glbParam->avg_Points_Fernald[glbParam->chSel], (double*)&dataMol->pr2Mol_avg  [0] ) ;

		sum(glbParam->r, 0, glbParam->avg_Points_Fernald[glbParam->chSel]-1, &glbParam->r_avg[0] ) ;
		glbParam->r_avg[0] = glbParam->r_avg[0] /glbParam->avg_Points_Fernald[glbParam->chSel] ;
		for (int i =1; i <glbParam->nBins; i++)
			glbParam->r_avg[i] = glbParam->r_avg[0] + i*glbParam->dr ;
	}
	else
	{
		for (int i =1; i <glbParam->nBins; i++)
		{
			dataMol->alphaMol_avg[i] = dataMol->alphaMol[i] ;
			dataMol->betaMol_avg[i]  = dataMol->betaMol [i] ;
			dataMol->pr2Mol_avg [i]  = dataMol->pr2Mol  [i] ;
			glbParam->r_avg[i] 		 = glbParam->r[i]		;
		}
	}
	
	if ( (heightRef_Inversion_Start_ASL >0) && (heightRef_Inversion_Stop_ASL >0) && (heightRef_Inversion_Stop_ASL > heightRef_Inversion_Start_ASL) )
	{
		heightRef_Inversion_ASL = ( heightRef_Inversion_Stop_ASL + heightRef_Inversion_Start_ASL )/2 ;
		// indxRef_Fernald[glbParam->evSel] 	   = (int)round( ( heightRef_Inversion_ASL - glbParam->siteASL)/dzr 	  ) ;
		indxRef_Fernald_Start[glbParam->evSel] = (int)round( ( heightRef_Inversion_Start_ASL - glbParam->siteASL)/dzr ) ;
		indxRef_Fernald_Stop [glbParam->evSel] = (int)round( ( heightRef_Inversion_Stop_ASL  - glbParam->siteASL)/dzr ) ;
	}
	else
		{
			for (int i =0 ; i <glbParam->nBins_Ch[glbParam->chSel] ; i++) // indxEndSig_ev[glbParam->evSel]
			{
				if ( (fpclassify( dataMol->prMol[i] ) == FP_NAN) || (fpclassify( dataMol->pr2Mol[i] ) == FP_NAN) )
					printf( "Fit(): dataMol.prMol[%d]= %e \t dataMol.pr2Mol[%d]= %e \n", i, dataMol->prMol[i], i, dataMol->pr2Mol[i] ) ;
			}
			Find_Ref_Range( (strcGlobalParameters*)glbParam, (strcMolecularData*)dataMol ) ;
		}

	printf("\tRef. ranges: %lf - %lf  - Ref. inversion: %f", glbParam->dzr*indxRef_Fernald_Start[glbParam->evSel], glbParam->dzr*indxRef_Fernald_Stop[glbParam->evSel], heightRef_Inversion_ASL) ;

	if ( strcmp( reference_method.c_str(), "MEAN" ) ==0 )
	{
			sum( (double*)&pr2[glbParam->evSel][glbParam->chSel][0], (int)indxRef_Fernald_Start[glbParam->evSel], (int)indxRef_Fernald_Stop[glbParam->evSel], (double*)&pr2_Ref ) ;
			pr2_Ref = pr2_Ref /(indxRef_Fernald_Stop[glbParam->evSel] - indxRef_Fernald_Start[glbParam->evSel] +1) ;
	}
	else if ( strcmp( reference_method.c_str(), "FIT" ) ==0 )
	{
		memset( (double*)dummy, 0, sizeof(double)*glbParam->nBins ) ;
		strcFitParam fitParam ;
		fitParam.indxInicFit = indxRef_Fernald_Start[glbParam->evSel] ;
		fitParam.indxEndFit  = indxRef_Fernald_Stop [glbParam->evSel] ;
		fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInicFit +1;
			oLOp->Fit( (double*)&pr2[glbParam->evSel][glbParam->chSel][0], (double*)dataMol->pr2Mol_avg, glbParam->nBins , "wOutB", "NOTall", (strcFitParam*)&fitParam, (double*)dummy ) ;
		double *absDiff = (double*) new double[ fitParam.nFit ] ;
		for (int i =0 ; i <fitParam.nFit ; i++)
			absDiff[i] = fabs( pr2[glbParam->evSel][glbParam->chSel][fitParam.indxInicFit +i] - dummy[fitParam.indxInicFit +i] ) ;
		
		findIndxMin( (double*)absDiff, (int)0, (int)(fitParam.nFit -1), (int*)&indxMin_absDiff, (double*)&minDiff ) ;
		indxRef_Fernald[glbParam->evSel] = fitParam.indxInicFit + indxMin_absDiff ;
		pr2_Ref = dummy[indxRef_Fernald[glbParam->evSel]] ;

		delete absDiff ;
	}
		pr2[glbParam->evSel][glbParam->chSel][indxRef_Fernald[glbParam->evSel]] = pr2_Ref ;

	int indx_integral_max_range_for_AOD ;
	int integral_max_range_for_AOD ;
// ********************** FERNALD INVERSION **********************
	for ( int l=0 ; l <nLRs ; l++ ) // LOOP ACROSS LRs
	{
		FernaldInversion_Core( (strcGlobalParameters*)glbParam, (int)l, (strcMolecularData*)dataMol, (double)LR[l], (int)0, (int)glbParam->nBins ) ;

		ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "integral_max_range_for_AOD", "int", (int*)&integral_max_range_for_AOD ) ;
		indx_integral_max_range_for_AOD = (int)round( integral_max_range_for_AOD /glbParam->dr ) ;

		smooth( (double*)&beta_Aer [glbParam->evSel][l][0], (int)0, (int)(glbParam->nBins-1), (int)glbParam->avg_Points_Fernald[glbParam->chSel], (double*)&beta_Aer [glbParam->evSel][l][0] ) ;
		smooth( (double*)&alpha_Aer[glbParam->evSel][l][0], (int)0, (int)(glbParam->nBins-1), (int)glbParam->avg_Points_Fernald[glbParam->chSel], (double*)&alpha_Aer[glbParam->evSel][l][0] ) ;
		sum(    (double*)&alpha_Aer[glbParam->evSel][l][0], (int)0, (int)indx_integral_max_range_for_AOD, (double*)&AOD_LR[glbParam->evSel][l] ) ;
		AOD_LR[glbParam->evSel][l] = AOD_LR[glbParam->evSel][l] * glbParam->dr ;
		printf("\nAOD@LR = %lf --> %lf", fabs(LR[l]), AOD_LR[glbParam->evSel][l]) ;

	} // for ( int l=0 ; l <nLRs ; l++ ) // LOOP ACROSS LRs
}

// USED FOR MONTE CARLO ERROR ESTIMATION
void CDataLevel_2::FernaldInversion( double *pr2_, strcMolecularData *dataMol, strcGlobalParameters *glbParam )
{
	LRM = (double) dataMol->LR_mol ;
/*    COPIAR DE VUELTA ESTE METODO (ESTA ARRIBA)
	if ( glbParam->avg_Points_Fernald[c] >1 )
	{
		smooth( (double*)&dataMol->alphaMol[0], (int)0, (int)(glbParam->nBins-1), (int)glbParam->avg_Points_Fernald[c], (double*)&dataMol->alphaMol_avg[0] ) ;
		smooth( (double*)&dataMol->betaMol [0], (int)0, (int)(glbParam->nBins-1), (int)glbParam->avg_Points_Fernald[c], (double*)&dataMol->betaMol_avg [0] ) ;
		smooth( (double*)&dataMol->pr2Mol  [0], (int)0, (int)(glbParam->nBins-1), (int)glbParam->avg_Points_Fernald[c], (double*)&dataMol->pr2Mol_avg  [0] ) ;

		sum(glbParam->r, 0, glbParam->avg_Points_Fernald[c]-1, &glbParam->r_avg[0] ) ;
		glbParam->r_avg[0] = glbParam->r_avg[0] /glbParam->avg_Points_Fernald[c] ;
		for (int i =1; i <glbParam->nBins; i++)
			glbParam->r_avg[i] = glbParam->r_avg[0] + i*glbParam->dr ;
	}
	else
	{
		for (int i =1; i <glbParam->nBins; i++)
		{
			dataMol->alphaMol_avg[i] = dataMol->alphaMol[i] ;
			dataMol->betaMol_avg[i]  = dataMol->betaMol [i] ;
			dataMol->pr2Mol_avg [i]  = dataMol->pr2Mol  [i] ;
			glbParam->r_avg[i] 		 = glbParam->r[i]		;
		}
	}
	
	if ( (heightRef_Inversion_Start_ASL >0) && (heightRef_Inversion_Stop_ASL >0) && (heightRef_Inversion_Stop_ASL > heightRef_Inversion_Start_ASL) )
	{
		heightRef_Inversion_ASL = ( heightRef_Inversion_Stop_ASL + heightRef_Inversion_Start_ASL )/2 ;
		indxRef_Fernald[glbParam->evSel] 	   = (int)round( ( heightRef_Inversion_ASL - glbParam->siteASL)/dzr 	  ) ;
		indxRef_Fernald_Start[glbParam->evSel] = (int)round( ( heightRef_Inversion_Start_ASL - glbParam->siteASL)/dzr ) ;
		indxRef_Fernald_Stop [glbParam->evSel] = (int)round( ( heightRef_Inversion_Stop_ASL  - glbParam->siteASL)/dzr ) ;
	}
	else
		Find_Ref_Range( (strcGlobalParameters*)glbParam, (int)t ) ;

printf("\tRef. ranges: %lf - %lf", glbParam->dzr*indxRef_Fernald_Start[glbParam->evSel], glbParam->dzr*indxRef_Fernald_Stop[glbParam->evSel]) ;

	if ( strcmp( reference_method.c_str(), "MEAN" ) ==0 )
	{
			sum( (double*)&pr2[t][c][0], (int)indxRef_Fernald_Start[glbParam->evSel], (int)indxRef_Fernald_Stop[glbParam->evSel], (double*)&pr2_Ref ) ;
			pr2_Ref = pr2_Ref /(indxRef_Fernald_Stop[glbParam->evSel] - indxRef_Fernald_Start[glbParam->evSel] +1) ;
	}
	else if ( strcmp( reference_method.c_str(), "FIT" ) ==0 )
	{
		memset( (double*)dummy, 0, sizeof(double)*glbParam->nBins ) ;
		strcFitParam fitParam ;
		fitParam.indxInicFit = indxRef_Fernald_Start[glbParam->evSel] ;
		fitParam.indxEndFit  = indxRef_Fernald_Stop [glbParam->evSel] ;
		fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInicFit +1;
			oLOp->Fit( (double*)&pr2[t][c][0], (double*)dataMol->pr2Mol_avg, glbParam->nBins , "wOutB", "NOTall", (strcFitParam*)&fitParam, (double*)dummy ) ;
		double *absDiff = (double*) new double[ fitParam.nFit ] ;
		for (int i =0 ; i <fitParam.nFit ; i++)
			absDiff[i] = fabs( pr2[t][c][fitParam.indxInicFit +i] - dummy[fitParam.indxInicFit +i] ) ;
		
		findIndxMin( (double*)absDiff, (int)0, (int)(fitParam.nFit -1), (int*)&indxMin_absDiff, (double*)&minDiff ) ;
		indxRef_Fernald[glbParam->evSel] = fitParam.indxInicFit + indxMin_absDiff ;
		pr2_Ref = dummy[indxRef_Fernald[glbParam->evSel]] ;

		delete absDiff ;
	}
		pr2[t][c][indxRef_Fernald[glbParam->evSel]] = pr2_Ref ;

	int indx_integral_max_range_for_AOD ;
	int integral_max_range_for_AOD ;
// ********************** FERNALD INVERSION **********************
	for ( int l=0 ; l <nLRs ; l++ ) // LOOP ACROSS LRs
	{
		// ********************** FERNALD INVERSION: TEST REFERENCE VALUE **********************
		// FernaldInversion_Test_Ref_Value( (strcGlobalParameters*)glbParam, (int)t, (int)c, (int)l, (strcMolecularData*)dataMol, (double)fabs(LR[l]),
		// 	(int)indxRef_Fernald_Start[glbParam->evSel], (int)indxRef_Fernald_Stop[glbParam->evSel] ) ;

		FernaldInversion_Core( (strcGlobalParameters*)glbParam, (int)t, (int)c, (int)l, (strcMolecularData*)dataMol, (double)LR[l], (int)0, (int)glbParam->nBins ) ;

		ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "integral_max_range_for_AOD", "int", (int*)&integral_max_range_for_AOD ) ;
		indx_integral_max_range_for_AOD = (int)round( integral_max_range_for_AOD /glbParam->dr ) ;

		smooth( (double*)&beta_Aer[t][l][0] , (int)0, (int)(glbParam->nBins-1), (int)glbParam->avg_Points_Fernald[c], (double*)&beta_Aer[t][l][0]  ) ;
		smooth( (double*)&alpha_Aer[t][l][0], (int)0, (int)(glbParam->nBins-1), (int)glbParam->avg_Points_Fernald[c], (double*)&alpha_Aer[t][l][0] ) ;
		sum(    (double*)&alpha_Aer[t][l][0], (int)0, (int)indx_integral_max_range_for_AOD, (double*)&AOD_LR[t][l] ) ;
		AOD_LR[t][l] = AOD_LR[t][l] * glbParam->dr ;
		printf("\nAOD@LR = %lf --> %lf", fabs(LR[l]), AOD_LR[t][l]) ;

	} // for ( int l=0 ; l <nLRs ; l++ ) // LOOP ACROSS LRs
*/
}

void CDataLevel_2::FernaldInversion_Core( strcGlobalParameters *glbParam, int l, strcMolecularData *dataMol, double LR, int indxStart, int indxStop )
{
	memset( (double*)phi		  , 0, sizeof(double)*glbParam->nBins ) ;
	memset( (double*)p  		  , 0, sizeof(double)*glbParam->nBins ) ;
	memset( (double*)ip 		  , 0, sizeof(double)*glbParam->nBins ) ;
	memset( (double*)ipN		  , 0, sizeof(double)*glbParam->nBins ) ;
	memset( (double*)betaT		  , 0, sizeof(double)*glbParam->nBins ) ;
	memset( (double*)intAlphaMol_r, 0, sizeof(double)*glbParam->nBins ) ;
	memset( (double*)alpha_Aer[glbParam->evSel][l], 0, sizeof(double)*glbParam->nBins ) ;
	memset( (double*)beta_Aer[glbParam->evSel][l] , 0, sizeof(double)*glbParam->nBins ) ;

	ka  = 1/fabs(LR) ;
	KM_ = 1/dataMol->LR_mol ;

	cumtrapz( glbParam->dr, dataMol->alphaMol_avg, 0, (dataMol->nBins-1)			   , intAlphaMol_r    ) ; // INTEGRALS ARE THRU SLANT PATH -> dr
	trapz	( glbParam->dr, dataMol->alphaMol_avg, 0, indxRef_Fernald[glbParam->evSel] , &intAlphaMol_Ref ) ; // INTEGRALS ARE THRU SLANT PATH -> dr

	for( int i=indxStart ; i <indxStop ; i++  )
	{
		pr2n[i] = pr2[glbParam->evSel][glbParam->chSel][i] /pr2_Ref ;
		phi[i]	= 2*((KM_-ka)/ka) * (intAlphaMol_r[i] - intAlphaMol_Ref) ;
		p[i]   	= pr2n[i] * exp(-phi[i]) ;
	}

	cumtrapz( glbParam->dr, p, 0, (glbParam->nBins-1)		      , ip 		) ;
	trapz   ( glbParam->dr, p, 0, indxRef_Fernald[glbParam->evSel], &ipNref ) ;

	for ( int i=indxStart ; i <indxStop ; i++ )
	{
		ipN[i] 	 = ip[i] - ipNref ;
		betaT[i] = p[i] / ( (1/dataMol->betaMol_avg[indxRef_Fernald[glbParam->evSel]]) - (2/ka) * ipN[i] ) ;
	}

		// if ( LR <0 )
		// {
			strcFitParam fitParam ;
			fitParam.indxInicFit = indxRef_Fernald_Start[glbParam->evSel] ;
			fitParam.indxEndFit  = indxRef_Fernald_Stop [glbParam->evSel] ;
			fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInicFit +1;
 			 oLOp->Fit( (double*)&betaT[0], (double*)dataMol->betaMol_avg    , glbParam->nBins , "wB", "all", (strcFitParam*)&fitParam, (double*)betaMol_Fit ) ;
			LR = (double)fabs(LR) ;
		// }
		// else
		// 	for (int i =0; i <glbParam->nBins_Ch[glbParam->chSel]; i++)
		// 		betaMol_Fit[i] = dataMol->betaMol_avg[i] ;

	for ( int i=indxStart ; i <indxStop ; i++ )
	{
		beta_Aer[glbParam->evSel][l][i]  = betaT[i] - betaMol_Fit[i] ;
		alpha_Aer[glbParam->evSel][l][i] = beta_Aer[glbParam->evSel][l][i] *LR ; // r
	}
	for (int i =0 ; i <glbParam->indxInitSig; i++)
	{
		alpha_Aer[glbParam->evSel][l][i] = alpha_Aer[glbParam->evSel][l][glbParam->indxInitSig] ;
		beta_Aer[glbParam->evSel][l][i]  = beta_Aer [glbParam->evSel][l][glbParam->indxInitSig] ;
	}
}

void CDataLevel_2::Find_Ref_Range( strcGlobalParameters *glbParam, strcMolecularData *dataMol )
{
	int indx_Top_Cloud = 0 ;

	// CHECK IF THERE ARE CLOUDS IN THE PROFILE ANALIZED
	for ( int i= glbParam->indxEndSig_ev[glbParam->evSel] ; i >=0 ; i-- )
	{
		if ( layer_mask[glbParam->evSel][i] ==1 )
		{
			indx_Top_Cloud = (int)i ;
			break ;
		}
	}

	int 	nWinSize ;
	int 	nWin = (glbParam->indxEndSig_ev[glbParam->evSel] - indx_Top_Cloud) /500 ;
	if ( nWin <=2 )
		nWinSize = 500 ;
	else if( nWin <=4 )
		nWinSize= 1000 ;
	else if( nWin <=8 )
		nWinSize= 2000 ;
	else
		nWinSize= 2000 ;

	int 			nSteps			= glbParam->indxEndSig_ev[glbParam->evSel] - indx_Top_Cloud - nWinSize ;
	printf("\nFind_Ref_Range(): ||| indx_Top_Cloud: %d ||| glbParam->indxEndSig_ev[glbParam->evSel]: %d  ||| nWinSize: %d ||| nSteps: %d\n", indx_Top_Cloud, glbParam->indxEndSig_ev[glbParam->evSel], nWinSize, nSteps ) ;
 
	strcFitParam	fitParam 											;
	double		 	*stdPr 			= (double*) new double	[nSteps] 	;
	int		 		*indxRef_Start 	= (int*) 	new int		[nSteps] 	;
	int		 		*indxRef_Stop  	= (int*) 	new int		[nSteps] 	;
	double 			*prFit = (double*) new double[glbParam->nBins] 		;

	fitParam.indxInicFit = indx_Top_Cloud 									;
	fitParam.indxEndFit  = fitParam.indxInicFit + nWinSize					;
	fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInicFit +1	;
	int i 			  	 = 0 ;
	do
	{
		for (int i =fitParam.indxInicFit ; i <fitParam.indxEndFit ; i++)
		{
			if ( (fpclassify( pr[glbParam->chSel][i] ) == FP_NAN) || (fpclassify( dataMol->prMol[i] ) == FP_NAN) )
				printf( "\t\tFind_Ref_Range(): pr[%d]= %e \t dataMol->prMol[%d]= %e\n", i, pr[glbParam->chSel][i], i, dataMol->prMol[i] ) ;
		}

		oLOp->Fit( (double*)&dataMol->prMol[0], (double*)&pr[glbParam->chSel][0], fitParam.nFit, "wB", "NOTall", (strcFitParam*)&fitParam, (double*)prFit ) ;

		stdPr[i] 		 = sqrt( fitParam.squared_sum_fit/(fitParam.nFit -1) ) ;
		indxRef_Start[i] = fitParam.indxInicFit ;
		indxRef_Stop [i] = fitParam.indxEndFit  ;

		fitParam.indxInicFit = fitParam.indxInicFit +1							;
		fitParam.indxEndFit  = fitParam.indxInicFit + nWinSize					;
		fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInicFit +1	;
		i++ ;
	} while( fitParam.indxEndFit < glbParam->indxEndSig_ev[glbParam->evSel] ) ;
	delete  prFit ;	
	
	int indxMin_Std, minStd ;
	findIndxMin( (double*)stdPr, (int)0, (int)(nSteps -1), (int*)(&indxMin_Std), (double*)(&minStd) ) ;

	indxRef_Fernald_Start[glbParam->evSel] = indxRef_Start[indxMin_Std] ;
	indxRef_Fernald_Stop [glbParam->evSel] = indxRef_Stop [indxMin_Std] ;
	heightRef_Inversion_ASL = (double) (glbParam->siteASL + glbParam->dzr*( indxRef_Fernald_Start[glbParam->evSel] + indxRef_Fernald_Stop[glbParam->evSel] )/2) ;
}

// BACKUP DE LA PRIMER VERSION
// void CDataLevel_2::Find_Ref_Range( strcGlobalParameters *glbParam, int t )
// {
// 	int indx_Top_Cloud = 0 ;

// 	// CHECK IF THERE ARE CLOUDS IN THE PROFILE ANALIZED
// 	for ( int i= glbParam->indxEndSig_ev[glbParam->evSel] ; i >=0 ; i-- )
// 	{
// 		if ( layer_mask[t][i] ==1 )
// 		{
// 			indx_Top_Cloud = (int)i ;
// 			break ;
// 		}
// 	}

// 	if ( indx_Top_Cloud ==0 ) // NO CLOUDS --> REFERENCE RANGE DEFINED BETWEEN 5 AND 10 KM
// 	{
// 		indxRef_Fernald_Start[glbParam->evSel] = (int) round( 5000 /glbParam->dzr ) ;
// 		indxRef_Fernald_Stop [glbParam->evSel] = (int) round( 10000/glbParam->dzr ) ;
// 	}
// 	else
// 	{ // THERE ARE CLOUDS
// 		indxRef_Fernald_Stop [glbParam->evSel] = (int) glbParam->indxEndSig_ev[glbParam->evSel] -10 ;
// 		if ( (indxRef_Fernald_Stop [glbParam->evSel] - indx_Top_Cloud) >=1000 )
// 			indxRef_Fernald_Start[glbParam->evSel] = (int) indxRef_Fernald_Stop[glbParam->evSel] -1000 ;
// 		else
// 			indxRef_Fernald_Start[glbParam->evSel] = (int) glbParam->indxEndSig_ev[glbParam->evSel] - (indxRef_Fernald_Stop [glbParam->evSel] - indx_Top_Cloud -20) ;
// 		// indxRef_Fernald_Start[glbParam->evSel] = (int) indx_Top_Cloud +100 	;
// 		// indxRef_Fernald_Stop [glbParam->evSel] = (int) indxRef_Fernald_Start[glbParam->evSel] + (int) round( 5000 /glbParam->dzr ) ;
// 	}

// 	if ( indxRef_Fernald_Stop[glbParam->evSel] >= glbParam->indxEndSig_ev[glbParam->evSel] )
// 		indxRef_Fernald_Stop[glbParam->evSel] = (int) glbParam->indxEndSig_ev[glbParam->evSel] ;

// 	heightRef_Inversion_ASL 			= (double) (glbParam->siteASL + glbParam->dzr*( indxRef_Fernald_Start[glbParam->evSel] + indxRef_Fernald_Stop[glbParam->evSel] )/2) ;
// 	// indxRef_Fernald[glbParam->evSel]	= (int)    round( ( heightRef_Inversion_ASL - glbParam->siteASL)/dzr ) ;
// }



void CDataLevel_2::MonteCarloRandomError( strcGlobalParameters *glbParam, strcMolecularData *dataMol)
{
	strcAerosolData dataAerErr ;
	dataAerErr.alphaAer	= (double*) malloc( glbParam->nBins * sizeof(double) ) ;  memset( dataAerErr.alphaAer, 0, ( glbParam->nEventsAVG * sizeof(double) ) ) ;
	dataAerErr.betaAer	= (double*) malloc( glbParam->nBins * sizeof(double) ) ;  memset( dataAerErr.betaAer , 0, ( glbParam->nEventsAVG * sizeof(double) ) ) ;
	dataAerErr.VAODr	= (double*) malloc( glbParam->nBins * sizeof(double) ) ;  memset( dataAerErr.VAODr   , 0, ( glbParam->nEventsAVG * sizeof(double) ) ) ;

	strcErrorSignalSet errSigSet ;
		GetMemStrcErrorSigSet( (strcErrorSignalSet*)&errSigSet, (int)glbParam->MonteCarlo_N_SigSet_Err, (int)glbParam->nBins ) ;

	strcFitParam	fitParam ;
	fitParam.indxInicFit = dataMol->nBins - glbParam->nBinsBkg  	  ;
	fitParam.indxEndFit  = dataMol->nBins -1 						  ;
	fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInicFit +1 ;
	double *prFit = (double*) new double[glbParam->nBins] ;
		oLOp->Fit( (double*)&dataMol->prMol[0], (double*)&pr[0], fitParam.nFit   , "wB", "NOTall", (strcFitParam*)&fitParam, (double*)prFit ) ;
	delete  prFit ;	
	double 	stdPr = sqrt( fitParam.squared_sum_fit/(fitParam.nFit -1) ) ;

	int 	spamAvgWin ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "spamAvgWin", "int", (int*)&spamAvgWin ) ;

	// PRODUCE THE LIDAR SIGNAL SET FROM pr[glbParam->evSel][:]


		// SEGUIR ACA!!!!!!!!!!!!!!!!!!!!

	for( int m=0 ; m < glbParam->MonteCarlo_N_SigSet_Err ; m++ )
	{
		smooth( (double*)&pr[glbParam->evSel][0], (int)0, (int)(glbParam->nBins-1), (int)spamAvgWin, (double*)&errSigSet.pr_smooth[m][0] ) ;

		for ( int i=0 ; i < glbParam->nBins ; i++ )
		{
			// errSigSet.prNoisy [m][i] = RandN2( pr[glbParam->evSel][i], stdPr ) ; // ADD ERROR TO pr
			errSigSet.prNoisy [m][i] = RandN2( errSigSet.pr_smooth[m][i], stdPr ) ; // ADD ERROR TO pr
			errSigSet.pr2Noisy[m][i] = errSigSet.prNoisy[m][i] * (glbParam->r[i]*glbParam->r[i]) ; // MAKE pr2 NOISY
		}
		// smooth( (double*)&errSigSet.pr2Noisy[m][0], (int)0, (int)(glbParam->nBins-1), (int)spamAvgWin, (double*)errSigSet.pr2Noisy[m] ) ;
		// FERNALD INVERSION
			FernaldInversion( (double*)&errSigSet.pr2Noisy[m][0], (strcMolecularData*)dataMol, (strcGlobalParameters*)&glbParam ) ;

		for( int b=0 ; b<glbParam->nBins ; b++ )
		{
			errSigSet.alphaAer_ErrSet[m][b] = alpha_Aer[glbParam->evSel][0][b] ; // !! SOLO UTILIZO EL PRIMER LR !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			// errSigSet.VAODr_ErrSet[m][b]    = VAODr   [b] ;
		}
	} // for( int m=0 ; m < MonteCarlo_N_SigSet_Err ; m++ )
	// OBTAIN MEAN AND STANDAR DEVIATION
		GetErrSetParam( (char*)glbParam->FILE_PARAMETERS, (int)glbParam->MonteCarlo_N_SigSet_Err, (int)glbParam->nBins, (double)dataMol->dzr, (strcErrorSignalSet*)&errSigSet ) ;
} // void CDataLevel_2::MonteCarloRandomError( strcGlobalParameters *glbParam, int t, int c, strcMolecularData *dataMol)

/*
void CDataLevel_2::MonteCarloSystematicError( double *pr2Glued, strcGlobalParameters *glbParam, strcMolecularData *dataMol, strcIndexMol *indxMol, strcFernaldInversion *fernaldVectors, strcErrorSignalSet *sysErrSigSet )
{
	strcAerosolData dataAerErr ;
	dataAerErr.alphaAer	= (double*) malloc( glbParam->nBins * sizeof(double) ) ;
	dataAerErr.betaAer	= (double*) malloc( glbParam->nBins * sizeof(double) ) ;
	dataAerErr.VAODr	= (double*) malloc( glbParam->nBins * sizeof(double) ) ;

	double 	LR_init, LR_end, LR_step ;
		ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "LR_init", "double" , (double*)&LR_init ) ;
		ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "LR_end" , "double" , (double*)&LR_end  ) ;
	int 	MonteCarlo_N_SigSet_Err ;
		ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "MonteCarlo_N_SigSet_Err", "int" , (int*)&MonteCarlo_N_SigSet_Err ) ;

	double 	*ka_ite = (double*) malloc( MonteCarlo_N_SigSet_Err * sizeof(double) ) ;
	LR_step = (double) 1 ; // ((LR_end - LR_init) /MonteCarlo_N_SigSet_Err) ;
	for ( int j=0 ; j<MonteCarlo_N_SigSet_Err ; j++ )		ka_ite[j] = (double) 1/(LR_init + j*LR_step) ;
//	double ka_ite[10] = { 0.015, 0.018, 0.020, 0.025, 0.03, 0.04, 0.05, 0.06, 0.07, 0.08 } ;

	for( int j=0 ; j<MonteCarlo_N_SigSet_Err ; j++ )
	{
// FERNALD INVERSION
	FernaldInversion( (double*)pr2Glued, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (int)indxMol->indxInicMol[0]+110, (double)ka_ite[j], (strcFernaldInversion*)fernaldVectors , (strcAerosolData*)&dataAerErr ) ;
	//FernaldInversion( (double*)pr2Glued, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (int)indxMol->indxInicMol[0]+110, (double)ka_ite[j], (strcAerosolData*)&dataAerErr ) ;
		for( int b=0 ; b<glbParam->nBins ; b++ )
		{
			sysErrSigSet->alphaAer_ErrSet[j][b] = dataAerErr.alphaAer[b] ;
			sysErrSigSet->   VAODr_ErrSet[j][b] = dataAerErr.VAODr   [b] ;
		}
	}
// OBTAIN MEAN AND STANDAR DEVIATION
	GetErrSetParam( (char*)glbParam->FILE_PARAMETERS, (int)MonteCarlo_N_SigSet_Err, (int)glbParam->nBins, (double)dataMol->dzr, (strcErrorSignalSet*)sysErrSigSet ) ;

// FREE MEM FROM fernaldVectors
	//FreeMemVectorsFernaldInversion( (strcFernaldInversion*)&fernaldVectors ) ;
}
*/
/*
void CDataLevel_2::FernaldInversion_Test_Ref_Value( strcGlobalParameters *glbParam, int t, int c, int l, strcMolecularData *dataMol, double LR, int indxStart, int indxStop )
{
	int nSamples ;
	ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "nBiasRes_Auto", "int" , (int*)&nSamples ) ;

	double *coeff     = (double*) new double[3 +1] ;
	double *alpha_sum = (double*) new double [nSamples] ;

	float 	maxFactor = 1.1 ;
	float 	minFactor = 0.9 ;

	double pr2_Ref_Max 		= maxFactor *pr2_Ref ;
	double pr2_Ref_Min	 	= minFactor *pr2_Ref ;
	double pr2_Ref_step		= (pr2_Ref_Max - pr2_Ref_Min) /nSamples ;
	double *pr2_Ref_Samples = (double*) new double[ nSamples ] ;
	for ( int s =0; s <nSamples; s++ )
		pr2_Ref_Samples[s] = (double) pr2_Ref_Min + s *pr2_Ref_step ;

	for( int iLoop=0 ; iLoop<3 ; iLoop++ )
	{
		for( int iSample=0 ; iSample<(nSamples) ; iSample++ )
		{
			pr2_Ref = pr2_Ref_Samples[iSample] ;
			FernaldInversion_Core( (strcGlobalParameters*)glbParam, (int)t, (int)c, (int)l, (strcMolecularData*)dataMol, (double)LR, (int)indxStart, (int)indxStop ) ;
			sum( (double*)&alpha_Aer[t][l][0], (int)indxStart, (int)indxStop, (double*)&alpha_sum[iSample] ) ;
			alpha_sum[iSample] = alpha_sum[iSample] * alpha_sum[iSample] ;
		}
		polyfitCoeff( 	(const double* const  ) pr2_Ref_Samples , // X DATA
						(const double* const  ) alpha_sum	 	, // Y DATA
						(unsigned int         ) nSamples 		,
						(unsigned int		  ) 2			 	,
						(double*			  ) coeff	 ) 	 	;

		pr2_Ref = (double) -coeff[1] /(2*coeff[2]) ;
		pr2_Ref_Max 	= maxFactor *pr2_Ref ;
		pr2_Ref_Min	 	= minFactor *pr2_Ref ;
		pr2_Ref_step	= (pr2_Ref_Max - pr2_Ref_Min) /nSamples ;
		for ( int s =0; s <nSamples; s++ )
			pr2_Ref_Samples[s] = (double) pr2_Ref_Min + s *pr2_Ref_step ;
	}
}
*/

void CDataLevel_2::GetMemStrcErrorSigSet( strcErrorSignalSet *errSigSet, int MonteCarlo_N_SigSet_Err, int nBins )
{
		errSigSet->pr_smooth  	   = (double**) new double*[MonteCarlo_N_SigSet_Err] ;
		errSigSet->prNoisy  	   = (double**) new double*[MonteCarlo_N_SigSet_Err] ;
		errSigSet->pr2Noisy 	   = (double**) new double*[MonteCarlo_N_SigSet_Err] ;
		errSigSet->alphaAer_ErrSet = (double**) new double*[MonteCarlo_N_SigSet_Err] ;
		errSigSet->VAODr_ErrSet    = (double**) new double*[MonteCarlo_N_SigSet_Err] ;

		for( int j=0 ; j < MonteCarlo_N_SigSet_Err ; j++ )
		{
			errSigSet->pr_smooth		  [j] = (double*) new double[nBins] ;
			errSigSet->prNoisy		  [j] = (double*) new double[nBins] ;
			errSigSet->pr2Noisy		  [j] = (double*) new double[nBins] ;
			errSigSet->alphaAer_ErrSet[j] = (double*) new double[nBins] ;
			errSigSet->   VAODr_ErrSet[j] = (double*) new double[nBins] ;
		}

		errSigSet->VAODr_meanErrSet	   = (double*) new double[nBins] ; 	memset( errSigSet->VAODr_meanErrSet   , 0, sizeof(double) * nBins ) ;
		errSigSet-> VAODr_stdErrSet	   = (double*) new double[nBins] ; 	memset( errSigSet->VAODr_stdErrSet    , 0, sizeof(double) * nBins ) ;
		errSigSet->alphaAer_meanErrSet = (double*) new double[nBins] ; 	memset( errSigSet->alphaAer_meanErrSet, 0, sizeof(double) * nBins ) ;
		errSigSet->alphaAer_stdErrSet  = (double*) new double[nBins] ;	memset( errSigSet->alphaAer_stdErrSet , 0, sizeof(double) * nBins ) ;

		memset( errSigSet->hVAOD_meanErrSet, 0, sizeof(double) * 4 ) ;
		memset( errSigSet-> hVAOD_stdErrSet, 0, sizeof(double) * 4 ) ;
}

void CDataLevel_2::GetErrSetParam( char *FILE_PARAMETERS, int MonteCarlo_N_SigSet_Err, int nBins, double dzr, strcErrorSignalSet *errSigSet )
{
	float 	VAODheigh ;
// r_meanErrSet
	memset( errSigSet->alphaAer_meanErrSet, 0, sizeof(double) * nBins ) ;
	memset( errSigSet->VAODr_meanErrSet   , 0, sizeof(double) * nBins ) ;
		for ( int b=0 ; b<nBins ; b++ )
		{
			for ( int j=0 ; j < MonteCarlo_N_SigSet_Err ; j++ )
			{
				errSigSet->alphaAer_meanErrSet[b] = errSigSet->alphaAer_meanErrSet[b] + errSigSet->alphaAer_ErrSet[j][b] ;
				errSigSet->   VAODr_meanErrSet[b] = errSigSet->   VAODr_meanErrSet[b] + errSigSet->   VAODr_ErrSet[j][b] ;
			}
			errSigSet->alphaAer_meanErrSet[b] = errSigSet->alphaAer_meanErrSet[b] / MonteCarlo_N_SigSet_Err ;
			errSigSet->   VAODr_meanErrSet[b] = errSigSet->   VAODr_meanErrSet[b] / MonteCarlo_N_SigSet_Err ;
		}
// VAODmean OF THE DATASET AT DIFFERENT HEIGHTS
	ReadAnalisysParameter( (const char*)FILE_PARAMETERS, "VAOD_HEIGH0", "float", (float*)&VAODheigh ) ;		errSigSet->hVAOD_meanErrSet[0] = errSigSet->VAODr_meanErrSet[(int)round(VAODheigh/dzr)] ;
	ReadAnalisysParameter( (const char*)FILE_PARAMETERS, "VAOD_HEIGH1", "float", (float*)&VAODheigh ) ;		errSigSet->hVAOD_meanErrSet[1] = errSigSet->VAODr_meanErrSet[(int)round(VAODheigh/dzr)] ;
	ReadAnalisysParameter( (const char*)FILE_PARAMETERS, "VAOD_HEIGH2", "float", (float*)&VAODheigh ) ;		errSigSet->hVAOD_meanErrSet[2] = errSigSet->VAODr_meanErrSet[(int)round(VAODheigh/dzr)] ;
	ReadAnalisysParameter( (const char*)FILE_PARAMETERS, "VAOD_HEIGH3", "float", (float*)&VAODheigh ) ;		errSigSet->hVAOD_meanErrSet[3] = errSigSet->VAODr_meanErrSet[(int)round(VAODheigh/dzr)] ;

// r_stdErrSet STANDAR ERROR MEAN OF THE DATASET AT DIFFERENT HEIGHTS
		memset( errSigSet->alphaAer_stdErrSet, 0, sizeof(double) * nBins ) ;
		memset( errSigSet->VAODr_stdErrSet   , 0, sizeof(double) * nBins ) ;
		for ( int b=0 ; b<nBins ; b++ )
		{
			for ( int j=0 ; j < MonteCarlo_N_SigSet_Err ; j++ )
			{
				errSigSet->alphaAer_stdErrSet[b] = errSigSet->alphaAer_stdErrSet[b] + pow( (errSigSet->alphaAer_ErrSet[j][b] - errSigSet->alphaAer_meanErrSet[b]), 2) ;
				errSigSet->   VAODr_stdErrSet[b] = errSigSet->   VAODr_stdErrSet[b] + pow( (errSigSet->   VAODr_ErrSet[j][b] - errSigSet->   VAODr_meanErrSet[b]), 2) ;
			}
			errSigSet->alphaAer_stdErrSet[b] = sqrt(errSigSet->alphaAer_stdErrSet[b] / (MonteCarlo_N_SigSet_Err-1) ) ;
			errSigSet->   VAODr_stdErrSet[b] = sqrt(errSigSet->   VAODr_stdErrSet[b] / (MonteCarlo_N_SigSet_Err-1) ) ;
		}
// VAODstd OF THE DATASET
	ReadAnalisysParameter( (const char*)FILE_PARAMETERS, "VAOD_HEIGH0", "float", (float*)&VAODheigh ) ;		errSigSet->hVAOD_stdErrSet[0] = errSigSet->VAODr_stdErrSet[(int)round(VAODheigh/dzr )] ;
	ReadAnalisysParameter( (const char*)FILE_PARAMETERS, "VAOD_HEIGH1", "float", (float*)&VAODheigh ) ;		errSigSet->hVAOD_stdErrSet[1] = errSigSet->VAODr_stdErrSet[(int)round(VAODheigh/dzr )] ;
	ReadAnalisysParameter( (const char*)FILE_PARAMETERS, "VAOD_HEIGH2", "float", (float*)&VAODheigh ) ;		errSigSet->hVAOD_stdErrSet[2] = errSigSet->VAODr_stdErrSet[(int)round(VAODheigh/dzr )] ;
	ReadAnalisysParameter( (const char*)FILE_PARAMETERS, "VAOD_HEIGH3", "float", (float*)&VAODheigh ) ;		errSigSet->hVAOD_stdErrSet[3] = errSigSet->VAODr_stdErrSet[(int)round(VAODheigh/dzr )] ;
}

void CDataLevel_2::Load_AERONET_Data( strcGlobalParameters *glbParam )
{
	char 	line[100] 		;
	i_Num_AERONET_data = 0 	;

 	FILE *fp = fopen( aeronet_file, "r");
    if (!fp)
        printf( "Can't open %s file... exit\nAERONET data is no used and saved in the output NetCDF file.", aeronet_file );
	else
	{
		while( fgets(line, 100, fp)!= NULL )
        	i_Num_AERONET_data++ ;
		fseek(fp, 0, SEEK_SET) ;

		AERONET_AOD  = (double*) new double [ i_Num_AERONET_data ] ;
		AERONET_time = (int*   ) new int    [ i_Num_AERONET_data ] ;

		// if ( i_Num_AERONET_data == glbParam->nEventsAVG )
		// {
			// for( int r=0 ; r <glbParam->nEventsAVG ; r++ )
			for( int r=0 ; r <i_Num_AERONET_data ; r++ )
			{
				fscanf(fp, "%d,%lf", &AERONET_time[r], &AERONET_AOD[r] ) ;
				// printf(  "\n%d \t %lf",  AERONET_time[r],  AERONET_AOD[r] ) ;
			}
		// }
		// else
		// 	printf("\nAERONET data not used due to error in the numbers of data in the file %s\n", aeronet_file) ;
	}
	fclose(fp);
}

/* 		BACKUP 
void CDataLevel_2::MonteCarloRandomError( double *pr2Glued, double *pr, strcGlobalParameters *glbParam, strcMolecularData *dataMol, strcIndexMol *indxMol, strcFernaldInversion *fernaldVectors, strcErrorSignalSet *rndErrSigSet )
{
	strcAerosolData dataAerErr ;
	dataAerErr.alphaAer	= (double*) malloc( glbParam->nBins * sizeof(double) ) ;  memset( dataAerErr.alphaAer, 0, ( glbParam->nEventsAVG * sizeof(double) ) ) ;
	dataAerErr.betaAer	= (double*) malloc( glbParam->nBins * sizeof(double) ) ;  memset( dataAerErr.betaAer , 0, ( glbParam->nEventsAVG * sizeof(double) ) ) ;
	dataAerErr.VAODr	= (double*) malloc( glbParam->nBins * sizeof(double) ) ;  memset( dataAerErr.VAODr   , 0, ( glbParam->nEventsAVG * sizeof(double) ) ) ;

	int 	MonteCarlo_N_SigSet_Err ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "MonteCarlo_N_SigSet_Err", "int" , (int*)&MonteCarlo_N_SigSet_Err ) ;
// GET THE STANDAR DEVIATION TO GENERATE THE RANDOM ERROR IN THE LIDAR SIGNALS SET
	strcFitParam	fitParam ;
	fitParam.indxInicFit = dataMol->nBins - glbParam->nBinsBkg  	  ;
	fitParam.indxEndFit  = dataMol->nBins -1 						  ;
	fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInicFit +1 ;
	double *prFit = (double*) new double[glbParam->nBins] ;
		oLOp->Fit( (double*)&dataMol->prMol[fitParam.indxInicFit], (double*)&pr[fitParam.indxInicFit], fitParam.nFit   , "wB", "NOTall", (strcFitParam*)&fitParam, (double*)prFit ) ;
	delete prFit ;	
	double 	stdPr = sqrt( fitParam.squared_sum_fit/(fitParam.nFit -1) ) ;
	int 	spamAvgWin ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "spamAvgWin", "int", (int*)&spamAvgWin ) ;

	for( int j=0 ; j < MonteCarlo_N_SigSet_Err ; j++ )
	{
		for ( int i=0 ; i < glbParam->nBins ; i++ )
		{
			rndErrSigSet->prGlued [j][i] = pr2Glued[i] / (glbParam->r[i]*glbParam->r[i]) ;
			rndErrSigSet->prNoisy [j][i] = RandN2( rndErrSigSet->prGlued[j][i], stdPr ) ;
			rndErrSigSet->pr2Noisy[j][i] = rndErrSigSet->prNoisy[j][i] * (glbParam->r[i]*glbParam->r[i]) ;
		}
		// smoothGSL( (double*)rndErrSigSet->pr2Noisy[j], (int)(glbParam->nBins), (int)spamAvgWin, (double*)rndErrSigSet->pr2Noisy[j] ) ;
		smooth( (double*)rndErrSigSet->pr2Noisy[j], (int)0, (int)(glbParam->nBins-1), (int)spamAvgWin, (double*)rndErrSigSet->pr2Noisy[j] ) ;
// FERNALD INVERSION
	// FernaldInversion( (double*)rndErrSigSet->pr2Noisy[j], (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (int)indxMol->indxInicMol[0]+110, (double)glbParam->ka, (strcFernaldInversion*)fernaldVectors, (strcAerosolData*)&dataAerErr ) ;
	FernaldInversion( (strcGlobalParameters*)&glbParam, (int)glbParam->evSel, (int)glbParam->indxWL_PDL2, (strcMolecularData*)dataMol ) ;
		for( int b=0 ; b<glbParam->nBins ; b++ )
		{
			rndErrSigSet->alphaAer_ErrSet[j][b] = dataAerErr.alphaAer[b] ;
			rndErrSigSet->   VAODr_ErrSet[j][b] = dataAerErr.VAODr   [b] ;
		}
	} // for( int j=0 ; j < MonteCarlo_N_SigSet_Err ; j++ )
// OBTAIN MEAN AND STANDAR DEVIATION
		// GetErrSetParam( (char*)glbParam->FILE_PARAMETERS, (int)MonteCarlo_N_SigSet_Err, (int)glbParam->nBins, (double)dataMol->dzr, (strcErrorSignalSet*)rndErrSigSet ) ;
}
*/

/*
void CDataLevel_2::Fernald_1983( strcGlobalParameters *glbParam, int t, int c, strcMolecularData *dataMol )
{
	LRM = (double) dataMol->LR_mol ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "R_ref", "double" , (double*)&R_ref ) ;

	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "avg_Points_Fernald", "int", (int*)&glbParam->avg_Points_Fernald ) ;
	if ( glbParam->avg_Points_Fernald[c] >1 )
	{
		smooth( (double*)&pr2[t][c][0], (int)0, (int)(glbParam->nBins-1), (int)glbParam->avg_Points_Fernald[c], (double*)pr2_s 			) ;
		smooth( (double*)glbParam->r  , (int)0, (int)(glbParam->nBins-1), (int)glbParam->avg_Points_Fernald[c], (double*)glbParam->r_avg ) ;
// printf("\nglbParam->r[0]: %lf\nglbParam->r[1]: %lf\nglbParam->r[2]: %lf\nglbParam->r[3]: %lf\n", glbParam->r[0], glbParam->r[1], glbParam->r[2], glbParam->r[3] ) ;
// printf("\nglbParam->r_avg[0]: %lf\nglbParam->r_avg[1]: %lf\nglbParam->r_avg[2]: %lf\nglbParam->r_avg[3]: %lf\n", glbParam->r_avg[0], glbParam->r_avg[1], glbParam->r_avg[2], glbParam->r_avg[3] ) ;

		for (int i =0; i <glbParam->nBins; i++)
		{
			pr2[t][c][i] 	= (double)pr2_s[i] 			 ;
			glbParam->r[i] 	= (double)glbParam->r_avg[i] ; 
		}
	}

	double 	pr2_Ref ;
	int 	avg_Half_Points_Fernald_Ref ;
	string 	reference_method ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "reference_method", "string", (char*)reference_method.c_str() ) ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "avg_Half_Points_Fernald_Ref", "int", (int*)&avg_Half_Points_Fernald_Ref ) ;

	double heightRef_Inversion_ASL ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "heightRef_Inversion_ASL" , "double" , (double*)&heightRef_Inversion_ASL ) ;

	if ( heightRef_Inversion_ASL >0 )
		indxRef_Fernald[glbParam->evSel] = (int)round( (heightRef_Inversion_ASL - glbParam->siteASL) /dzr ) ;
	else
	{
		indxRef_Fernald[glbParam->evSel] = (int)(glbParam->indxEndSig_ev[glbParam->evSel] - 2*avg_Half_Points_Fernald_Ref) ;
	}
	if ( strcmp( reference_method.c_str(), "MEAN" ) ==0 )
	{
			sum( (double*)&pr[t][0], (int)(indxRef_Fernald[glbParam->evSel] -avg_Half_Points_Fernald_Ref), (int)(indxRef_Fernald[glbParam->evSel] +avg_Half_Points_Fernald_Ref), (double*)&pr2_Ref ) ;
			pr2_Ref = pr2_Ref /(avg_Half_Points_Fernald_Ref +1) ;
			pr2_Ref = pr2_Ref * glbParam->r[indxRef_Fernald[glbParam->evSel]] * glbParam->r[indxRef_Fernald[glbParam->evSel]] ;
	}
	else if ( strcmp( reference_method.c_str(), "FIT" ) ==0 )
	{
		double *pr2Fit = (double*) new double[ glbParam->nBins ] ;
		strcFitParam fitParam ;
		fitParam.indxInicFit = indxRef_Fernald[glbParam->evSel] - avg_Half_Points_Fernald_Ref ;
		fitParam.indxEndFit  = indxRef_Fernald[glbParam->evSel] + avg_Half_Points_Fernald_Ref ;
		fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInicFit +1;
		// 	RayleighFit( (double*)&pr2[t][c][0], (double*)dataMol->pr2Mol, glbParam->nBins , "wOutB", "NOTall", (strcFitParam*)&fitParam, (double*)pr2Fit ) ;
		// pr2_Ref = pr2Fit[indxRef_Fernald[glbParam->evSel]] ;
			oLOp->Fit( (double*)&pr[t][0], (double*)dataMol->prMol, glbParam->nBins , "wOutB", "NOTall", (strcFitParam*)&fitParam, (double*)pr2Fit ) ;
		pr2_Ref = pr2Fit[indxRef_Fernald[glbParam->evSel]] * glbParam->r[indxRef_Fernald[glbParam->evSel]] * glbParam->r[indxRef_Fernald[glbParam->evSel]] ;
		delete pr2Fit ;
	}
	pr2_Ref = pr2_Ref *1.0 ;

	for( int i=0 ; i <glbParam->nBins ; i++  )
		pr2n[i] = pr2[t][c][i] /pr2_Ref ;

	double phi, p, ip, CalTerm, beta_Tot ;

	for ( int l=0 ; l <nLRs ; l++ ) // LOOP ACROSS LRs
	{
		beta_Aer[t][l][indxRef_Fernald[glbParam->evSel]+1] = R_ref * dataMol->betaMol[indxRef_Fernald[glbParam->evSel]] - dataMol->betaMol[indxRef_Fernald[glbParam->evSel]] ;
		int invDirection = -1 ; // 1 = BACKWARD
		for( int i=indxRef_Fernald[glbParam->evSel] ; i >=indxInitSig ; i-- )
		{	// BACKWARD INVERSION - FERNALD 1983
			phi = (LR[l]-LRM) * ( dataMol->betaMol[i+1] + dataMol->betaMol[i] ) * invDirection * glbParam->dr ;
			p   = pr2n[i] * exp(-phi) ;

			CalTerm = pr2n[i+1] / ( beta_Aer[t][l][i+1] + dataMol->betaMol[i+1] ) ;
			ip = ( pr2n[i+1] + p ) * invDirection * glbParam->dr /2 ;

			beta_Tot = p / ( CalTerm - 2*LR[l] * ip ) ;
			beta_Aer[t][l][i]  = beta_Tot - dataMol->betaMol[i] ;
			alpha_Aer[t][l][i] = beta_Aer[t][l][i] * LR[l] ;
		}

		beta_Aer[t][l][indxRef_Fernald[glbParam->evSel]-1] = R_ref * dataMol->betaMol[indxRef_Fernald[glbParam->evSel]] - dataMol->betaMol[indxRef_Fernald[glbParam->evSel]] ;
		invDirection = 1 ; // 1 = FORWARD
		for ( int i=(indxRef_Fernald[glbParam->evSel]-1) ; i <=indxEndSig ; i++ )
		{	// FORWARD INVERSION - FERNALD 1983
			phi = (LR[l]-LRM) * ( dataMol->betaMol[i-1] + dataMol->betaMol[i] ) * invDirection * glbParam->dr ;
			p   = pr2n[i] * exp(-phi) ;
			
			CalTerm = pr2n[i-1] / ( beta_Aer[t][l][i-1] + dataMol->betaMol[i-1] ) ;
			ip = ( pr2n[i-1] + p ) * invDirection * glbParam->dr /2 ;

			beta_Tot = p / ( CalTerm - 2*LR[l] * ip ) ;
			beta_Aer[t][l][i]  = beta_Tot - dataMol->betaMol[i] ;
			alpha_Aer[t][l][i] = beta_Aer[t][l][i] * LR[l] ;
		}
		for (int i =0 ; i <indxInitSig; i++)
		{
			alpha_Aer[t][l][i] = alpha_Aer[t][l][indxInitSig] ;
			beta_Aer[t][l][i]  = beta_Aer[t][l][indxInitSig] ;
		}

		int indx_integral_max_range_for_AOD ;
		int integral_max_range_for_AOD ;
		ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "integral_max_range_for_AOD", "int", (int*)&integral_max_range_for_AOD ) ;
		indx_integral_max_range_for_AOD = (int)round(integral_max_range_for_AOD /glbParam->dr) ;

		smooth( (double*)&beta_Aer[t][l][0] , (int)0, (int)(glbParam->nBins-1), (int)glbParam->avg_Points_Fernald[c], (double*)&beta_Aer[t][l][0]  ) ;
		smooth( (double*)&alpha_Aer[t][l][0], (int)0, (int)(glbParam->nBins-1), (int)glbParam->avg_Points_Fernald[c], (double*)&alpha_Aer[t][l][0] ) ;
		sum(    (double*)&alpha_Aer[t][l][0], (int)0, (int)indx_integral_max_range_for_AOD, (double*)&AOD_LR[t][l] ) ;
		AOD_LR[t][l] = AOD_LR[t][l] * glbParam->dr ;
		printf("\nAOD@LR = %lf --> %lf", LR[l], AOD_LR[t][l]) ;

	} // for ( int l=0 ; l <nLRs ; l++ ) // LOOP ACROSS LRs
}
*/
