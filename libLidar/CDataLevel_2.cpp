
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
	intAlphaMol_r  	= (double*) new double[glbParam->nBins ] ;

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

	oLOp = (CLidar_Operations*) new CLidar_Operations( (strcGlobalParameters*)glbParam ) ;
}
	
CDataLevel_2::~CDataLevel_2()
{

}

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
		// indxRef_Fernald[glbParam->evSel] = Find_Ref_Range( (strcGlobalParameters*)glbParam, (strcMolecularData*)dataMol ) ;
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
			RayleighFit( (double*)&pr[t][0], (double*)dataMol->prMol, glbParam->nBins , "wOutB", "NOTall", (strcFitParam*)&fitParam, (double*)pr2Fit ) ;
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

void CDataLevel_2::FernaldInversion_v0( strcGlobalParameters *glbParam, int t, int c, strcMolecularData *dataMol)
{
	LRM = (double) dataMol->LR_mol ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "R_ref", "double" , (double*)&R_ref ) ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "avg_Points_Fernald", "int", (int*)glbParam->avg_Points_Fernald ) ;
	if ( glbParam->avg_Points_Fernald[c] >1 )
	{
		smooth( (double*)&pr2[t][c][0], (int)0, (int)(glbParam->nBins-1), (int)glbParam->avg_Points_Fernald[c], (double*)pr2_s 			) ;
			for (int i =0; i <glbParam->nBins; i++)
				pr2[t][c][i] = (double)pr2_s[i] 	;

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

	string 	reference_method ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "reference_method", "string", (char*)reference_method.c_str() ) ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "avg_Half_Points_Fernald_Ref", "int", (int*)&avg_Half_Points_Fernald_Ref ) ;

	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "heightRef_Inversion_ASL" , "double" , (double*)&heightRef_Inversion_ASL ) ;

	if ( heightRef_Inversion_ASL >0 )
		indxRef_Fernald[glbParam->evSel] = (int)round( (heightRef_Inversion_ASL - glbParam->siteASL) /dzr ) ;
	else
	{// indxRef_Fernald[glbParam->evSel] = Find_Ref_Range( (strcGlobalParameters*)glbParam, (strcMolecularData*)dataMol ) ;
		indxRef_Fernald[glbParam->evSel] = (int)(glbParam->indxEndSig_ev[glbParam->evSel] - 2*avg_Half_Points_Fernald_Ref) ;
	}
	if ( strcmp( reference_method.c_str(), "MEAN" ) ==0 )
	{
			sum( (double*)&pr[t][0], (int)(indxRef_Fernald[glbParam->evSel] -avg_Half_Points_Fernald_Ref), (int)(indxRef_Fernald[glbParam->evSel] +avg_Half_Points_Fernald_Ref), (double*)&pr2_Ref ) ;
			pr2_Ref = pr2_Ref /(avg_Half_Points_Fernald_Ref +1) ;
			pr2_Ref = pr2_Ref * glbParam->r_avg[indxRef_Fernald[glbParam->evSel]] * glbParam->r_avg[indxRef_Fernald[glbParam->evSel]] ;
	}
	else if ( strcmp( reference_method.c_str(), "FIT" ) ==0 )
	{
		double *pr2Fit = (double*) new double[ glbParam->nBins ] ;
		strcFitParam fitParam ;
		fitParam.indxInicFit = indxRef_Fernald[glbParam->evSel] - avg_Half_Points_Fernald_Ref ;
		fitParam.indxEndFit  = indxRef_Fernald[glbParam->evSel] + avg_Half_Points_Fernald_Ref ;
		fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInicFit +1;
			RayleighFit( (double*)&pr2[t][c][0], (double*)dataMol->pr2Mol_avg, glbParam->nBins , "wOutB", "NOTall", (strcFitParam*)&fitParam, (double*)pr2Fit ) ;

		double *absDiff = (double*) new double[ fitParam.nFit ] ;
		for (int i =0 ; i <fitParam.nFit ; i++)
			absDiff[i] = fabs( pr2[t][c][fitParam.indxInicFit +i] - pr2Fit[fitParam.indxInicFit +i] ) ;
		
		findIndxMin( (double*)absDiff, (int)0, (int)(fitParam.nFit -1), (int*)&indxMin_absDiff, (double*)&minDiff ) ;
		indxRef_Fernald[glbParam->evSel] = fitParam.indxInicFit + indxMin_absDiff ;
		pr2_Ref = pr2Fit[indxRef_Fernald[glbParam->evSel]] ;

		delete pr2Fit  ;
		delete absDiff ;
	}
		pr2[t][c][indxRef_Fernald[glbParam->evSel]] = pr2_Ref ;

// ********************** FERNALD INVERSION **********************
	for ( int l=0 ; l <nLRs ; l++ ) // LOOP ACROSS LRs
	{
		// ********************** FERNALD INVERSION: TEST REFERENCE VALUE **********************
		// FernaldInversion_Test_Ref_Value( (strcGlobalParameters*)glbParam, (int)t, (int)c, (int)l, (strcMolecularData*)dataMol, (double)LR[l],
		// 	(int)(indxRef_Fernald[glbParam->evSel] - avg_Half_Points_Fernald_Ref), (int)(indxRef_Fernald[glbParam->evSel] + avg_Half_Points_Fernald_Ref) ) ;

		FernaldInversion_Core( (strcGlobalParameters*)glbParam, (int)t, (int)c, (int)l, (strcMolecularData*)dataMol, (double)LR[l],
								(int)0, (int)glbParam->nBins ) ;

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

void CDataLevel_2::FernaldInversion( strcGlobalParameters *glbParam, int t, int c, strcMolecularData *dataMol)
{
	LRM = (double) dataMol->LR_mol ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "R_ref", "double" , (double*)&R_ref ) ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "avg_Points_Fernald", "int", (int*)glbParam->avg_Points_Fernald ) ;
	if ( glbParam->avg_Points_Fernald[c] >1 )
	{
		smooth( (double*)&pr2[t][c][0], (int)0, (int)(glbParam->nBins-1), (int)glbParam->avg_Points_Fernald[c], (double*)pr2_s 			) ;
			for (int i =0; i <glbParam->nBins; i++)
				pr2[t][c][i] = (double)pr2_s[i] 	;

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
	string 	reference_method ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "reference_method", "string", (char*)reference_method.c_str() ) ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "heightRef_Inversion_Start_ASL", "double", (double*)&heightRef_Inversion_Start_ASL ) ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "heightRef_Inversion_Stop_ASL" , "double", (double*)&heightRef_Inversion_Stop_ASL  ) ;

	if ( (heightRef_Inversion_Start_ASL >0) && (heightRef_Inversion_Stop_ASL >0) && (heightRef_Inversion_Stop_ASL > heightRef_Inversion_Start_ASL) )
	{
		heightRef_Inversion_ASL = ( heightRef_Inversion_Stop_ASL + heightRef_Inversion_Start_ASL )/2 ;
		indxRef_Fernald[glbParam->evSel] = (int)round( ( heightRef_Inversion_ASL - glbParam->siteASL)/dzr ) ;
		indxRef_Fernald_Start[glbParam->evSel] = (int)round( ( heightRef_Inversion_Start_ASL - glbParam->siteASL)/dzr ) ;
		indxRef_Fernald_Stop [glbParam->evSel] = (int)round( ( heightRef_Inversion_Stop_ASL  - glbParam->siteASL)/dzr ) ;
	}
	else
	{
		// indxRef_Fernald[glbParam->evSel] = Find_Ref_Range( (strcGlobalParameters*)glbParam, (strcMolecularData*)dataMol ) ;
		indxRef_Fernald[glbParam->evSel] = (int)(glbParam->indxEndSig_ev[glbParam->evSel] - 2*glbParam->nBinsBkg) ;
	}
	if ( strcmp( reference_method.c_str(), "MEAN" ) ==0 )
	{
			// sum( (double*)&pr[t][0], (int)(indxRef_Fernald[glbParam->evSel] -avg_Half_Points_Fernald_Ref), (int)(indxRef_Fernald[glbParam->evSel] +avg_Half_Points_Fernald_Ref), (double*)&pr2_Ref ) ;
			sum( (double*)&pr[t][0], (int)indxRef_Fernald_Start[glbParam->evSel], (int)indxRef_Fernald_Stop[glbParam->evSel], (double*)&pr2_Ref ) ;
			pr2_Ref = pr2_Ref /(indxRef_Fernald_Stop[glbParam->evSel] - indxRef_Fernald_Start[glbParam->evSel] +1) ;
			pr2_Ref = pr2_Ref * glbParam->r_avg[indxRef_Fernald[glbParam->evSel]] * glbParam->r_avg[indxRef_Fernald[glbParam->evSel]] ;
	}
	else if ( strcmp( reference_method.c_str(), "FIT" ) ==0 )
	{
		double *pr2Fit = (double*) new double[ glbParam->nBins ] ;
		strcFitParam fitParam ;
		fitParam.indxInicFit = indxRef_Fernald_Start[glbParam->evSel] ;
		fitParam.indxEndFit  = indxRef_Fernald_Stop [glbParam->evSel] ;
		fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInicFit +1;
			RayleighFit( (double*)&pr2[t][c][0], (double*)dataMol->pr2Mol_avg, glbParam->nBins , "wOutB", "NOTall", (strcFitParam*)&fitParam, (double*)pr2Fit ) ;
		double *absDiff = (double*) new double[ fitParam.nFit ] ;
		for (int i =0 ; i <fitParam.nFit ; i++)
			absDiff[i] = fabs( pr2[t][c][fitParam.indxInicFit +i] - pr2Fit[fitParam.indxInicFit +i] ) ;
		
		findIndxMin( (double*)absDiff, (int)0, (int)(fitParam.nFit -1), (int*)&indxMin_absDiff, (double*)&minDiff ) ;
		indxRef_Fernald[glbParam->evSel] = fitParam.indxInicFit + indxMin_absDiff ;
		pr2_Ref = pr2Fit[indxRef_Fernald[glbParam->evSel]] ;

		delete pr2Fit  ;
		delete absDiff ;
	}
		pr2[t][c][indxRef_Fernald[glbParam->evSel]] = pr2_Ref ;

// ********************** FERNALD INVERSION **********************
	for ( int l=0 ; l <nLRs ; l++ ) // LOOP ACROSS LRs
	{
		// ********************** FERNALD INVERSION: TEST REFERENCE VALUE **********************
		// FernaldInversion_Test_Ref_Value( (strcGlobalParameters*)glbParam, (int)t, (int)c, (int)l, (strcMolecularData*)dataMol, (double)LR[l],
		// 	(int)(indxRef_Fernald[glbParam->evSel] - avg_Half_Points_Fernald_Ref), (int)(indxRef_Fernald[glbParam->evSel] + avg_Half_Points_Fernald_Ref) ) ;

		FernaldInversion_Core( (strcGlobalParameters*)glbParam, (int)t, (int)c, (int)l, (strcMolecularData*)dataMol, (double)LR[l],
								(int)0, (int)glbParam->nBins ) ;

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

void CDataLevel_2::FernaldInversion_Core( strcGlobalParameters *glbParam, int t, int c, int l, strcMolecularData *dataMol, double LR, int indxStart, int indxStop )
{
	memset( (double*)phi		  , 0, sizeof(double)*glbParam->nBins ) ;
	memset( (double*)p  		  , 0, sizeof(double)*glbParam->nBins ) ;
	memset( (double*)ip 		  , 0, sizeof(double)*glbParam->nBins ) ;
	memset( (double*)ipN		  , 0, sizeof(double)*glbParam->nBins ) ;
	memset( (double*)betaT		  , 0, sizeof(double)*glbParam->nBins ) ;
	memset( (double*)intAlphaMol_r, 0, sizeof(double)*glbParam->nBins ) ;
	memset( (double*)alpha_Aer[t][l], 0, sizeof(double)*glbParam->nBins ) ;
	memset( (double*)beta_Aer[t][l] , 0, sizeof(double)*glbParam->nBins ) ;

	ka  = 1/LR ;
	KM_ = 1/dataMol->LR_mol ;

	cumtrapz( glbParam->dr, dataMol->alphaMol_avg, 0, (dataMol->nBins-1)			   , intAlphaMol_r    ) ; // INTEGRALS ARE THRU SLANT PATH -> dr
	trapz	( glbParam->dr, dataMol->alphaMol_avg, 0, indxRef_Fernald[glbParam->evSel] , &intAlphaMol_Ref ) ; // INTEGRALS ARE THRU SLANT PATH -> dr
	// cumtrapz( glbParam->dr, dataMol->alphaMol, 0, (dataMol->nBins-1)			   , intAlphaMol_r    ) ; // INTEGRALS ARE THRU SLANT PATH -> dr
	// trapz	( glbParam->dr, dataMol->alphaMol, 0, indxRef_Fernald[glbParam->evSel] , &intAlphaMol_Ref ) ; // INTEGRALS ARE THRU SLANT PATH -> dr

	for( int i=indxStart ; i <indxStop ; i++  )
	{
		pr2n[i] = pr2[t][c][i] /pr2_Ref ;
		phi[i]	= 2*((KM_-ka)/ka) * (intAlphaMol_r[i] - intAlphaMol_Ref) ;
		p[i]   	= pr2n[i] * exp(-phi[i]) ;
	}

	cumtrapz( glbParam->dr, p, 0, (glbParam->nBins-1)		      , ip 		) ;
	trapz   ( glbParam->dr, p, 0, indxRef_Fernald[glbParam->evSel], &ipNref ) ;

	for ( int i=indxStart ; i <indxStop ; i++ )
	{
		ipN[i] 	 = ip[i] - ipNref ;
		betaT[i] = p[i] / ( (1/dataMol->betaMol_avg[indxRef_Fernald[glbParam->evSel]]) - (2/ka) * ipN[i] ) ;
		// betaT[i] = p[i] / ( (1/dataMol->betaMol[indxRef_Fernald[glbParam->evSel]]) - (2/ka) * ipN[i] ) ;

		beta_Aer[t][l][i]  = betaT[i] - dataMol->betaMol_avg[i]  ; // r
		// beta_Aer[t][l][i]  = betaT[i] - dataMol->betaMol[i]  ; // r
		alpha_Aer[t][l][i] = beta_Aer[t][l][i] *LR ; // r
	}
	for (int i =0 ; i <glbParam->indxInitSig; i++)
	{
		alpha_Aer[t][l][i] = alpha_Aer[t][l][glbParam->indxInitSig] ;
		beta_Aer[t][l][i]  = beta_Aer [t][l][glbParam->indxInitSig] ;
	}
}

// int CDataLevel_2::Find_Ref_Range( strcGlobalParameters *glbParam, strcMolecularData *dataMol )
// {

// }

// {
// 	printf("\n pr[%d][100]= %e \n"  , glbParam->evSel, pr[glbParam->evSel][100] ) ;
// 	printf(" pr_Mol[%d][100]= %e \n", glbParam->evSel, dataMol->prMol[100] ) ;

// 	int avg_Half_Points_Fernald_Ref ;
// 	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "avg_Half_Points_Fernald_Ref", "int", (int*)&avg_Half_Points_Fernald_Ref ) ;
// 	strcFitParam fitParam ;
//     fitParam.indxInicFit = indxInitSig ; // round( 3000/7.5 ); // 
// 	fitParam.indxEndFit  = fitParam.indxInicFit + 2*avg_Half_Points_Fernald_Ref -1;
// 	fitParam.nFit		 = fitParam.indxEndFit - fitParam.indxInicFit +1;

// 	double	corrCoeff ;
//     while ( fitParam.indxEndFit <(glbParam->nBins-1) )
// 	{
// 		RayleighFit( (double*)&pr[glbParam->evSel][0], (double*)&dataMol->prMol[0], glbParam->nBins , "wOutB", "NOTall", (strcFitParam*)&fitParam, (double*)dummy ) ;
// 		corrCoeff = (double) correlationCoefficient_dbl( (double*)&pr[glbParam->evSel][fitParam.indxInicFit], (double*)&dummy[fitParam.indxInicFit], (int)fitParam.nFit ) ;
// 		if ( corrCoeff > 0.999 )
// 		{	
// 			printf("\n Break: %lf \t fitParam.m= %e \t glbParam->evSel: %d \t fitParam.indxInitFit: %d", corrCoeff, fitParam.m, glbParam->evSel, fitParam.indxInicFit+avg_Half_Points_Fernald_Ref) ;
// 			break ;
// 		}

// 		fitParam.indxInicFit = fitParam.indxInicFit +100 ;
// 		fitParam.indxEndFit  = fitParam.indxInicFit + 2*avg_Half_Points_Fernald_Ref -1;
// 		fitParam.nFit		 = fitParam.indxEndFit - fitParam.indxInicFit +1;
//   	} 
// 	return (fitParam.indxInicFit+avg_Half_Points_Fernald_Ref) ;
// }
