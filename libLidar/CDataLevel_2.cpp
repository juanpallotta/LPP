
#include "CDataLevel_2.hpp"  

CDataLevel_2::CDataLevel_2( strcGlobalParameters *glbParam )
{
	LRM		  = 8*M_PI/3 ;

	pr2n      = (double*)   new double  [glbParam->nBins  ] ;	memset( (double*)pr2n , 0, sizeof(double)*glbParam->nBins ) ;
	pr2_s     = (double*)   new double  [glbParam->nBins  ] ;	memset( (double*)pr2_s, 0, sizeof(double)*glbParam->nBins ) ;
	pr2       = (double***) new double**[glbParam->nEventsAVG] ;
    alpha_Aer = (double***) new double**[glbParam->nEventsAVG] ;
    beta_Aer  = (double***) new double**[glbParam->nEventsAVG] ;

    nMol	  = (double*)  new double[glbParam->nBins] ; 	memset( (double*)nMol, 0, sizeof(double)*glbParam->nBins ) ;
    alpha_Mol = (double**) new double*[glbParam->nCh]  ;
    beta_Mol  = (double**) new double*[glbParam->nCh]  ;
	for ( int c=0 ; c <glbParam->nCh ; c++ )
	{
		alpha_Mol[c] = (double*) new double[glbParam->nBins] ;	memset( (double*)alpha_Mol[c], 0, sizeof(double)*glbParam->nBins ) ;
		beta_Mol[c]  = (double*) new double[glbParam->nBins] ;	memset( (double*)beta_Mol[c] , 0, sizeof(double)*glbParam->nBins ) ;
	}

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

	nLRs = ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "LR ", "double", (double*)LR ) ;
	AOD_LR = (double*) new double[nLRs] ;
	for ( int e=0 ; e <glbParam->nEventsAVG ; e++ )
	{
		alpha_Aer[e] = (double**) new double*[nLRs] ;
		beta_Aer[e]  = (double**) new double*[nLRs] ;

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
}
	
CDataLevel_2::~CDataLevel_2()
{

}

void CDataLevel_2::Fernald_1983( strcGlobalParameters *glbParam, int t, int c )
{
	double heightRef_Inversion_ASL ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "heightRef_Inversion_ASL" , "double" , (double*)&heightRef_Inversion_ASL ) ;
	indxRef = (int)round( (heightRef_Inversion_ASL - glbParam->siteASL) /dzr ) ;

	int avg_Points_Fernald ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "avg_Points_Fernald", "int", (int*)&avg_Points_Fernald ) ;
	smooth( (double*)&pr2[t][c][0], (int)0, (int)(glbParam->nBins-1), (int)avg_Points_Fernald, (double*)pr2_s ) ;

	double 	pr2_Ref ;
	int 	avg_Half_Points_Fernald_Ref ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "avg_Half_Points_Fernald_Ref", "int", (int*)&avg_Half_Points_Fernald_Ref ) ;
	sum( (double*)pr2_s, (int)(indxRef -avg_Half_Points_Fernald_Ref), (int)(indxRef +avg_Half_Points_Fernald_Ref), (double*)&pr2_Ref ) ;
	pr2_Ref = pr2_Ref /(avg_Half_Points_Fernald_Ref +1) ;

	for( int i=0 ; i <glbParam->nBins ; i++  )
		pr2n[i] = pr2_s[i] /pr2_Ref ;

	for ( int l=0 ; l <nLRs ; l++ ) // LOOP ACROSS LRs
	{	// BACKWARD INVERSION - FERNALD 1983
		beta_Aer[t][l][indxRef+1] = beta_Mol[c][indxRef] ;
		int invDirection = -1 ; // 1 = BACKWARD
		for( int i=indxRef ; i >indxInitSig ; i-- )
		{
			phi = (LR[l]-LRM) * ( beta_Mol[c][i+1] + beta_Mol[c][i] ) * invDirection * glbParam->dr ;
			p   = pr2n[i] * exp(-phi) ;

			CalTerm = pr2n[i+1] / ( beta_Aer[t][l][i+1] + beta_Mol[c][i+1] ) ;
			ip = ( pr2n[i+1] + p ) * invDirection * glbParam->dr /2 ;

			beta_Tot = p / ( CalTerm - 2*LR[l] * ip ) ;
			beta_Aer[t][l][i]  = beta_Tot - beta_Mol[c][i] ;
			alpha_Aer[t][l][i] = beta_Aer[t][l][i] * LR[l] ;
		}
		beta_Aer[t][l][indxRef-1] = beta_Mol[c][indxRef] ;
		invDirection = 1 ; // 1 = FORWARD
		for ( int i=indxRef ; i <indxEndSig ; i++ )
		{
			phi = (LR[l]-LRM) * ( beta_Mol[c][i-1] + beta_Mol[c][i] ) * invDirection * glbParam->dr ;
			p   = pr2n[i] * exp(-phi) ;
			
			CalTerm = pr2n[i-1] / ( beta_Aer[t][l][i-1] + beta_Mol[c][i-1] ) ;
			ip = ( pr2n[i-1] + p ) * invDirection * glbParam->dr /2 ;

			beta_Tot = p / ( CalTerm - 2*LR[l] * ip ) ;
			beta_Aer[t][l][i]  = beta_Tot - beta_Mol[c][i] ;
			alpha_Aer[t][l][i] = beta_Aer[t][l][i] * LR[l] ;
		}
		sum( (double*)&alpha_Aer[t][l][0] , (int)indxInitSig, (int)indxEndSig, (double*)&AOD_LR[l] ) ;
		AOD_LR[l] = AOD_LR[l] * glbParam->dr ;
		printf("\nAOD@LR = %lf --> %lf", LR[l], AOD_LR[l]) ;
	} // for ( int l=0 ; l <nLRs ; l++ ) // LOOP ACROSS LRs
}

// void CDataLevel_2::Fernald_1983_v0( strcGlobalParameters *glbParam, int t, int c )
// {
// 	double  LR[10] 	    ;
// 	int		indxRef, nLRs ;

// 	double heightRef_Inversion_ASL ;
// 	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "heightRef_Inversion_ASL" , "double" , (double*)&heightRef_Inversion_ASL ) ;
// 	indxRef = (int)round( (heightRef_Inversion_ASL - glbParam->siteASL) /dzr ) ;

// 	int avg_Points_Fernald ;
// 	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "avg_Points_Fernald", "int", (int*)&avg_Points_Fernald ) ;
// 	smooth( (double*)&pr2[t][c][0], (int)0, (int)(glbParam->nBins-1), (int)avg_Points_Fernald, (double*)pr2_s ) ;

// 	double 	pr2_Ref ;
// 	int 	avg_Half_Points_Fernald_Ref ;
// 	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "avg_Half_Points_Fernald_Ref", "int", (int*)&avg_Half_Points_Fernald_Ref ) ;
// 	sum( (double*)pr2_s, (int)(indxRef -avg_Half_Points_Fernald_Ref), (int)(indxRef +avg_Half_Points_Fernald_Ref), (double*)&pr2_Ref ) ;
// 	pr2_Ref = pr2_Ref /(avg_Half_Points_Fernald_Ref +1) ;

// 	// cout << endl ;
// 	// cout << endl << "CDataLevel_2::Fernald_1983(...) indxRef: " << indxRef ;
// 	// cout << endl << "CDataLevel_2::Fernald_1983(...) indxInitSig: " << indxInitSig ;
// 	// cout << endl << "CDataLevel_2::Fernald_1983(...) indxEndSig: "  << indxEndSig ;
// 	// cout << endl << "CDataLevel_2::Fernald_1983(...) pr2_Ref: " << pr2_Ref ;
// 	// cout << endl << "CDataLevel_2::Fernald_1983(...) avg_Half_Points_Fernald_Ref: " << avg_Half_Points_Fernald_Ref ;
// 	nLRs = ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "LR ", "double", (double*)LR ) ;
// 	// for ( int i=0 ; i <nLRs ; i++  )
// 	// 	cout << endl << "CDataLevel_2::Fernald_1983(...) LR[i]: " << LR[i] ;

// 	for( int i=0 ; i <glbParam->nBins ; i++  )
// 		pr2n[i] = pr2_s[i] /pr2_Ref ;
// 		// pr2n[i] = pr2_s[i] /pr2_s[indxRef] ;

// // BACKWARD INVERSION - FERNALD 1983
// // if ( (strcmp(invDirection, 'backward') ==1) || (strcmp(invDirection, 'both') ==1) )
// 	beta_Aer[t][c][indxRef+1] = beta_Mol[c][indxRef] ;
// 	int invDirection = -1 ; // 1 = BACKWARD
// 	for( int i=indxRef ; i >indxInitSig ; i-- )
// 	{
// 		phi = (LR[0]-LRM) * ( beta_Mol[c][i+1] + beta_Mol[c][i] ) * invDirection * glbParam->dr ;
// 		p   = pr2n[i] * exp(-phi) ;

// 		CalTerm = pr2n[i+1] / ( beta_Aer[t][c][i+1] + beta_Mol[c][i+1] ) ;
// 		ip = ( pr2n[i+1] + p ) * invDirection * glbParam->dr /2 ;

// 		beta_Tot = p / ( CalTerm - 2*LR[0] * ip ) ;
// 		beta_Aer[t][c][i]  = beta_Tot - beta_Mol[c][i] ;
// 		alpha_Aer[t][c][i] = beta_Aer[t][c][i] * LR[0] ;
// 	}
//  	beta_Aer[t][c][indxRef-1] = beta_Mol[c][indxRef] ;
//     invDirection = 1 ; // 1 = FORWARD
//     for ( int i=indxRef ; i <indxEndSig ; i++ )
// 	{
// 		// phi = (LR-LRM) * ( betaM(i-1) + betaM(i) ) *dz ; % dz >0 --> phi >0
// 		phi = (LR[0]-LRM) * ( beta_Mol[c][i-1] + beta_Mol[c][i] ) * invDirection * glbParam->dr ;
//       	// p   = attBn(i) * exp(-phi);
// 		p   = pr2n[i] * exp(-phi) ;

//       	// CalTerm = attBn(i-1) / (betaA(i-1) + betaM(i-1) ) ;
// 		CalTerm = pr2n[i-1] / ( beta_Aer[t][c][i-1] + beta_Mol[c][i-1] ) ;
//       	// ip =  ( attBn(i-1) + p ) *dz /2 ;
// 		ip = ( pr2n[i-1] + p ) * invDirection * glbParam->dr /2 ;

// 		// betaT = p ./ ( CalTerm - 2*LR * ip ) ;
// 		beta_Tot = p / ( CalTerm - 2*LR[0] * ip ) ;
// 		// betaA(i) = betaT - betaM(i) ;
// 		beta_Aer[t][c][i]  = beta_Tot - beta_Mol[c][i] ;
// 		// alphaA(i) = betaA(i) * LR ;
// 		alpha_Aer[t][c][i] = beta_Aer[t][c][i] * LR[0] ; // pr2_s[i] ; // 
// 	}
// }