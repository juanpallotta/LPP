
#include "CDataLevel_2.hpp"  

CDataLevel_2::CDataLevel_2( strcGlobalParameters *glbParam )
{
	LRM		   = 8*M_PI/3 ;
	pr2n       = (double*)   new double  [glbParam->nBins  ]    ;	memset( (double*)pr2n , 0, sizeof(double)*glbParam->nBins ) ;
	pr2        = (double***) new double**[glbParam->nEventsAVG] ;
    alpha_Aer  = (double***) new double**[glbParam->nEventsAVG] ;
    beta_Aer   = (double***) new double**[glbParam->nEventsAVG] ;
    AOD_LR	   = (double**)  new double *[glbParam->nEventsAVG] ;
    pr		   = (double**)  new double *[glbParam->nEventsAVG] ;
    nMol	   = (double*)   new double  [glbParam->nBins]      ;	memset( (double*)nMol , 0, sizeof(double)*glbParam->nBins ) ;
    dummy      = (double*)   new double  [glbParam->nBins ]     ;	memset( (double*)dummy, 0, sizeof(double)*glbParam->nBins ) ;

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
	indxRef_Fernald = (int*) new int [ glbParam->nEventsAVG ] ;

	oLOp = (CLidar_Operations*) new CLidar_Operations( (strcGlobalParameters*)glbParam ) ;
}
	
CDataLevel_2::~CDataLevel_2()
{

}

void CDataLevel_2::Fernald_1983( strcGlobalParameters *glbParam, int t, int c, strcMolecularData *dataMol )
{
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "R_ref", "double" , (double*)&R_ref ) ;

	int avg_Points_Fernald ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "avg_Points_Fernald", "int", (int*)&avg_Points_Fernald ) ;
	// smooth( (double*)&pr2[t][c][0], (int)0, (int)(glbParam->nBins-1), (int)avg_Points_Fernald, (double*)pr2_s ) ;
	// for (int i =0; i <glbParam->nBins; i++)
		// pr2[t][c][i] = (double)pr2_s[i] ;

	double 	pr2_Ref ;
	int 	avg_Half_Points_Fernald_Ref ;
	string 	reference_method ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "reference_method", "string", (char*)reference_method.c_str() ) ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "avg_Half_Points_Fernald_Ref", "int", (int*)&avg_Half_Points_Fernald_Ref ) ;

	double heightRef_Inversion_ASL ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "heightRef_Inversion_ASL" , "double" , (double*)&heightRef_Inversion_ASL ) ;

	if ( heightRef_Inversion_ASL >0 )
	{	indxRef_Fernald[glbParam->evSel] = (int)round( (heightRef_Inversion_ASL - glbParam->siteASL) /dzr ) ;	}
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

	for ( int l=0 ; l <nLRs ; l++ ) // LOOP ACROSS LRs
	{
		// do{
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

		smooth( (double*)&beta_Aer[t][l][0] , (int)0, (int)(glbParam->nBins-1), (int)avg_Points_Fernald, (double*)&beta_Aer[t][l][0]  ) ;
		smooth( (double*)&alpha_Aer[t][l][0], (int)0, (int)(glbParam->nBins-1), (int)avg_Points_Fernald, (double*)&alpha_Aer[t][l][0] ) ;
		sum(    (double*)&alpha_Aer[t][l][0], (int)0, (int)indx_integral_max_range_for_AOD, (double*)&AOD_LR[t][l] ) ;
		AOD_LR[t][l] = AOD_LR[t][l] * glbParam->dr ;
		printf("\nAOD@LR = %lf --> %lf", LR[l], AOD_LR[t][l]) ;
		// } while( (AOD_LR[t][l] >2) || (AOD_LR[t][l] <0) ) ;
	} // for ( int l=0 ; l <nLRs ; l++ ) // LOOP ACROSS LRs
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
