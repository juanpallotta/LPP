/*
 * CMolecularDataAuger.cpp
 * Copyright 2021 Juan V. Pallotta
 */

#include "CMolecularDataAuger.hpp"


CMolecularDataAuger::CMolecularDataAuger( char *radFile, strcGlobalParameters *glbParam )
{
	GetMem_dataMol( (int)glbParam->nBins ) ; // MEMORY ALLOCATION FOR dataMol STRUCTURE.
	ReadMolDBAuger( (char*)radFile, (double)glbParam->siteASL, (int)glbParam->nBins ) ;	// RADIOSONDE READOUT AND STRUCT RadSondeData FILLING.
}

CMolecularDataAuger::~CMolecularDataAuger()
{

}
// ________________________________________________________________

void CMolecularDataAuger::GetMem_dataMol( int nBins )
{
		dataMol.nBins	= (int)nBins ;
		dataMol.zr		= (double*) new double [nBins] ; memset( dataMol.zr	     , 0, ( sizeof(double) * nBins) ) ;
		dataMol.betaMol = (double*) new double [nBins] ; memset( dataMol.betaMol , 0, ( sizeof(double) * nBins) ) ;
	 // dataMol.betaRam  = (double*) new double [nBins] ; memset( dataMol.betaRam , 0, ( sizeof(double) * nBins) ) ;
		dataMol.alphaMol= (double*) new double [nBins] ; memset( dataMol.alphaMol, 0, ( sizeof(double) * nBins) ) ;
		dataMol.nMol	= (double*) new double [nBins] ; memset( dataMol.nMol    , 0, ( sizeof(double) * nBins) ) ;
		dataMol.prMol	= (double*) new double [nBins] ; memset( dataMol.prMol   , 0, ( sizeof(double) * nBins) ) ;
		dataMol.pr2Mol	= (double*) new double [nBins] ; memset( dataMol.pr2Mol  , 0, ( sizeof(double) * nBins) ) ;
		dataMol.MOD 	= (double*) new double [nBins] ; memset( dataMol.MOD     , 0, ( sizeof(double) * nBins) ) ;
		dataMol.zenith	= (double)0 ;
}

void CMolecularDataAuger::ReadMolDBAuger( char *radFile, double siteASL, int nBins )
{
	char 	ch ;
	int 	lines=0 ;
	char 	lineaRad[100], *pch ;

	FILE *fid = fopen(radFile, "r");
	if ( fid == NULL )
		printf("\n no se puede abrir el archivo %s \n", radFile) ;

// OBTENGO LA CANTIDAD DE FILAS, O CANTIDAD DE BINES DEL RADIOSONDEO (BAJA RESOLUCION).
	while( !feof(fid) )
	{
	  ch = fgetc(fid);
	  if(ch == '\n')	lines++ ;
	}
	lines = lines - 2 ;
	fseek( fid, 0, 0 ) ; // RESET THE POINTER

	fgets(lineaRad, 100, fid) ; // READE THE HEADER
	fgets(lineaRad, 100, fid) ; // READE THE HEADER

// READ RADIOSONDE DATA REFERENCED AT *ASL* AND LOW RESOLUTION (LR)
	int nBinsLR = lines ;
	double *zLR = (double*) new double [lines] ;
	double *pLR = (double*) new double [lines] ;
	double *tLR = (double*) new double [lines] ;
	for ( int l=0 ; l<lines ; l++ ) // BARRO LAS LINEAS.
	{
		fgets(lineaRad, 100, fid) ;
		pch = strtok(lineaRad,"\t") ;
		for( int i=0 ; i<3 ; i++ )
		{
				if (i==0)
					zLR[l] = (double)(atof(pch))*1000 ; // [m]
				else if (i==1)
					tLR[l] = (double)atof(pch) ; // [K]
				else if (i==2)
					pLR[l] = (double)atof(pch) ; // [hPa]
			pch = strtok (NULL, "\t") ;
		}
	}
	fclose(fid) ;
// SITE INDEX ALTITUDE (indxMin)
// FIND THE INDEX OF zLR WHERE zLR=siteASL
	double  *diff = (double*) new double [nBinsLR] ;
	double  min ;
	int 	indxMin = 0 ;
	for ( int i=0 ; i<nBinsLR ; i++ )
	{
		diff[i] = fabs( siteASL - zLR[i] ) ;
		if ( i==0 )
		{	min 	= diff[i] 	;
			indxMin = i 		;
		}
		else if ( diff[i] < min )
		{	min 	= diff[i] 	;
			indxMin = i 		;
		}
	}
	RadSondeData.nBinsLR = (int)(lines - indxMin) ;
	RadSondeData.zLR = (double*) new double [RadSondeData.nBinsLR] ;
	RadSondeData.pLR = (double*) new double [RadSondeData.nBinsLR] ;
	RadSondeData.tLR = (double*) new double [RadSondeData.nBinsLR] ;
	RadSondeData.nLR = (double*) new double [RadSondeData.nBinsLR] ;

	for ( int b=0 ; b<RadSondeData.nBinsLR ; b++ ) // BARRO LAS LINEAS.
	{ // RadSondeData.xLR[0] CORRESPOND TO THE POINT OF THE SITE ALTITUDE.
		RadSondeData.zLR[b] = zLR[b+indxMin] ; // RadSondeData.zLR --> ASL
		RadSondeData.tLR[b] = tLR[b+indxMin] ; // RadSondeData.tLR --> ASL
		RadSondeData.pLR[b] = pLR[b+indxMin] ; // RadSondeData.pLR --> ASL
		RadSondeData.nLR[b] = (100*RadSondeData.pLR[b]/RadSondeData.tLR[b]) /1.3800653e-23 ; // [1/m3]? ¡¡¡¡ASL!!!!
	}
		RadSondeData.nHR = (double*) new double [nBins] ;

	delete zLR  ;
	delete pLR  ;
	delete tLR  ;
	delete diff ;
}

void CMolecularDataAuger::Fill_dataMol( strcGlobalParameters *glbParam )
{
		dataMol.zenith = glbParam->aZenithAVG[glbParam->event_analyzed] ;
		// zr ASL
		for ( int i=0 ; i < glbParam->nBins ; i++ )
			dataMol.zr[i]   	= (double) glbParam->siteASL + glbParam->r[i] * cos(dataMol.zenith *PI/180) ; // zr = ASL
		dataMol.dzr = (double)(dataMol.zr[1] - dataMol.zr[0]) ;
		// double N2_shift = 2331e2 ;
		// double N2_XS_BS = 3.5e-34 * pow( ( (1/glbParam->lambda)-N2_shift ), 4) / pow( (1e9/337.1-N2_shift), 4 ) ;
			RadLowToHighRes() ;
		for ( int i =0 ; i <dataMol.nBins ; i++ )
			dataMol.nMol[i] = RadSondeData.nHR[i] ;

		for( int i=0 ; i < dataMol.nBins ; i++ )
		{ // betaMol, alphaMol AND betaRam ACROSS r AND ¡¡¡¡¡¡¡ASL, STARTING AT THE SITE ALTITUDE!!!!!!!
			dataMol.betaMol [i] = (double)(dataMol.nMol[i] * ( 5.45 * pow(10, -32) * pow((550.0/glbParam->lambda), 4) ) ) ; // r [1/m*sr]
			dataMol.alphaMol[i] = (double)(dataMol.betaMol[i] * 8.0 * 3.1415/3.0) ; // r [1/m]
			// dataMol.betaRam [i] = (double)(N2_XS_BS * dataMol.nMol[i]) ; // r
		}
			cumtrapz( (glbParam->r[1] - glbParam->r[0]), dataMol.alphaMol, 0, dataMol.nBins-1, (double*)dataMol.MOD ) ;
			for ( int b=0 ; b < dataMol.nBins  ; b++ )
			{
				dataMol.pr2Mol[b] 	= (double)   dataMol.betaMol[b] * exp( -2*dataMol.MOD[b] ) ;
				dataMol.prMol [b] 	= (double) ( dataMol.pr2Mol[b] / (glbParam->r[b] * glbParam->r[b]) ) ;
			}
}

void CMolecularDataAuger::RadLowToHighRes()
{
	double *coeff = (double*) new double[3 +1] ;
	
	polyfitCoeff( (const double* const) RadSondeData.zLR, // X DATA
			    (const double* const  ) RadSondeData.nLR, // Y DATA
			    (unsigned int         ) RadSondeData.nBinsLR,
			    (unsigned int		  ) 3,
			    (double*			  ) coeff	 ) ;

	for (int i =0 ; i <dataMol.nBins ; i++ )
	{
		RadSondeData.nHR[i] = pow(dataMol.zr[i], 3) *coeff[3] + pow(dataMol.zr[i], 2) *coeff[2] + dataMol.zr[i]*coeff[1] + coeff[0] ;
	}
	delete coeff ;	
}

