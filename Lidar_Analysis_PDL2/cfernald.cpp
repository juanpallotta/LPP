/* * cfernald.cpp * Copyright 2018 Juan V. Pallotta <Juan V. Pallotta@JUAN> */
#include "cfernald.h"

CFernald::CFernald()
{
	GetMemVectorsFernaldInversion() ;
}

CFernald::~CFernald()
{
	FreeMemVectorsFernaldInversion() ;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CFernald::GetMemVectorsFernaldInversion()
{
	int nBins = 4000 ;

	pr2n 	   	  = (double*) malloc( nBins * sizeof(double) ) ;	memset( pr2n  		 , 0, sizeof(double) * nBins ) ;
	phi  	 	  = (double*) malloc( nBins * sizeof(double) ) ;	memset( phi   		 , 0, sizeof(double) * nBins ) ;
	p 		  	  = (double*) malloc( nBins * sizeof(double) ) ;	memset( p   		 , 0, sizeof(double) * nBins ) ;
	ip		  	  = (double*) malloc( nBins * sizeof(double) ) ;	memset( ip    		 , 0, sizeof(double) * nBins ) ;
	ipN 		  = (double*) malloc( nBins * sizeof(double) ) ;	memset( ipN   		 , 0, sizeof(double) * nBins ) ;
	betaT 		  = (double*) malloc( nBins * sizeof(double) ) ;	memset( betaT 		 , 0, sizeof(double) * nBins ) ;
	pr2Fit		  = (double*) malloc( nBins * sizeof(double) ) ;	memset( pr2Fit		 , 0, sizeof(double) * nBins ) ;
	intAlphaMol_r = (double*) malloc( nBins * sizeof(double) ) ;	memset( intAlphaMol_r, 0, sizeof(double) * nBins ) ;
}

void CFernald::FreeMemVectorsFernaldInversion()
{

	free(pr2n)  		;
	free(phi)  			;
	free(p) 	   		;
	free(ip)	   		;
	free(ipN)   		;
	free(betaT) 		;
	free(pr2Fit)		;
	free(intAlphaMol_r) ;
}

void CFernald::FernaldInversion( double *pr2, strcMolecularData *dataMol, strcGlobalParameters *glbParam, int indxRef, double ka, strcFernaldInversion *fernaldVectors, strcAerosolData *dataAer )
{ /*
	cumtrapz( glbParam->dr, dataMol->alphaMol, 0, (dataMol->nBins-1),  fernaldVectors->intAlphaMol_r   ) ; // INTEGRALS ARE THRU SLANT PATH -> dr
	trapz	( glbParam->dr, dataMol->alphaMol, 0, indxRef		      , &fernaldVectors->intAlphaMol_Ref ) ; // INTEGRALS ARE THRU SLANT PATH -> dr

// REFERENCE POINT CALCULATION
	strcFitParam	fitParam ;
	fitParam.indxInicFit = indxRef - 100 ;
	fitParam.indxEndFit  = indxRef + 100 ;
	fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInicFit ;
		RayleighFit_bin   ( (double*)pr2, (double*)dataMol->pr2Mol, dataMol->nBins, "wB", (strcFitParam*)&fitParam, (double*)fernaldVectors->pr2Fit ) ;

	for ( int i=0 ; i < dataMol->nBins ; i++ )
	{
		fernaldVectors->pr2n[i] = pr2[i] / fernaldVectors->pr2Fit[indxRef] ;
		fernaldVectors->phi[i]	= 2*((KM-ka)/ka) * (fernaldVectors->intAlphaMol_r[i] - fernaldVectors->intAlphaMol_Ref) ;
		fernaldVectors->p[i]   	= fernaldVectors->pr2n[i]*exp(-fernaldVectors->phi[i]) ;
	}

	cumtrapz( glbParam->dr, fernaldVectors->p, 0, (dataMol->nBins-1),  fernaldVectors->ip     ) ;
	trapz   ( glbParam->dr, fernaldVectors->p, 0, indxRef 		    , &fernaldVectors->ipNref ) ;

	for ( int i=0 ; i < dataMol->nBins ; i++ )
	{
		fernaldVectors->ipN[i] 	 = fernaldVectors->ip[i] - fernaldVectors->ipNref ;
		fernaldVectors->betaT[i] = fernaldVectors->p[i] / ( (1/dataMol->betaMol[indxRef]) - (2/ka)* fernaldVectors->ipN[i] ) ;

		dataAer->betaAer [i] = fernaldVectors->betaT[i] - dataMol->betaMol[i] ; // r
		dataAer->alphaAer[i] = dataAer->betaAer[i]/ka         ; // r
	}

		int indxEndSig_r = (int) round( (dataAer->ablHeigh/dataMol->dzr) /cos(dataMol->zenith*PI/180) ) +100 ; // ABL HEIGH INDEX
		int 	indxMax ;
		double 	max 	;
			findIndxMax( (double*)dataAer->alphaAer, 0, indxEndSig_r, (int*)&indxMax, (double*)&max ) ;
		for( int i=0 ; i<indxMax ; i++ )	dataAer->alphaAer[i] = dataAer->alphaAer[indxMax] ; //

// VAODr: VAOD PROFILE
		cumtrapz( dataMol->dzr, dataAer->alphaAer, 0, (dataMol->nBins-1), dataAer->VAODr ) ; // VAODr=VAODh

	int 	VAODheigh ;
	readAnalisysParameter( FILE_PARAMETERS, "VAOD_HEIGH0" , "int" , &VAODheigh ) ;		dataAer->aVAOD[0] = dataAer->VAODr[ (int)round(VAODheigh/dataMol->dzr) ] ;
	readAnalisysParameter( FILE_PARAMETERS, "VAOD_HEIGH1" , "int" , &VAODheigh ) ;		dataAer->aVAOD[1] = dataAer->VAODr[ (int)round(VAODheigh/dataMol->dzr) ] ;
	readAnalisysParameter( FILE_PARAMETERS, "VAOD_HEIGH2" , "int" , &VAODheigh ) ;		dataAer->aVAOD[2] = dataAer->VAODr[ (int)round(VAODheigh/dataMol->dzr) ] ;
	readAnalisysParameter( FILE_PARAMETERS, "VAOD_HEIGH3" , "int" , &VAODheigh ) ;		dataAer->aVAOD[3] = dataAer->VAODr[ (int)round(VAODheigh/dataMol->dzr) ] ;
// VAOD @ VAOD_HEIGH3
		dataAer->VAOD = dataAer->aVAOD[3] ;
		dataAer->AOD  = dataAer->VAOD / cos( dataMol->zenith * PI/180 ) ;
*/
}

