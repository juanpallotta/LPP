/*
 * CMolecularData_USStd.cpp
 * Copyright 2021 Juan V. Pallotta
 */

#include "CMolecularData_USStd.hpp"


CMolecularData_USStd::CMolecularData_USStd( char *radFile, strcGlobalParameters *glbParam )
{
	GetMem_dataMol( (int)glbParam->nBins, (int)glbParam->nLambda ) ; // MEMORY ALLOCATION FOR dataMol STRUCTURE.
	ReadUSSTDfile( (char*)radFile, (strcGlobalParameters*)glbParam ) ;	// RADIOSONDE READOUT AND STRUCT RadSondeData FILLING.
}

CMolecularData_USStd::~CMolecularData_USStd()
{

}
// ________________________________________________________________

void CMolecularData_USStd::GetMem_dataMol( int nBins, int nLambda )
{
	dataMol = (strcMolecularData*) new strcMolecularData[nLambda] ;
	for (int l=0; l <nLambda; l++)
	{
		dataMol[l].nBins	= (int)nBins ;
		dataMol[l].zr		= (double*) new double [nBins] ; memset( dataMol[l].zr	    , 0, ( sizeof(double) * nBins) ) ;
		dataMol[l].betaMol  = (double*) new double [nBins] ; memset( dataMol[l].betaMol , 0, ( sizeof(double) * nBins) ) ;
	 // dataMol[l].betaRam  = (double*) new double [nBins] ; memset( dataMol[l].betaRam , 0, ( sizeof(double) * nBins) ) ;
		dataMol[l].alphaMol = (double*) new double [nBins] ; memset( dataMol[l].alphaMol, 0, ( sizeof(double) * nBins) ) ;
		dataMol[l].nMol	 	= (double*) new double [nBins] ; memset( dataMol[l].nMol    , 0, ( sizeof(double) * nBins) ) ;
		dataMol[l].prMol	= (double*) new double [nBins] ; memset( dataMol[l].prMol   , 0, ( sizeof(double) * nBins) ) ;
		dataMol[l].pr2Mol	= (double*) new double [nBins] ; memset( dataMol[l].pr2Mol  , 0, ( sizeof(double) * nBins) ) ;
		dataMol[l].zenith	= 0 ;
	}
}
/**
 * @brief Fill RadSondeData structure based on the radiosonde file passed as argument.
 *
 * @param radFile 
 * @param siteASL 
 */
void CMolecularData_USStd::ReadUSSTDfile( const char *radFile, strcGlobalParameters *glbParam )
{
	char 	ch ;
	int 	lines=0 ;
	char 	lineaRad[100], strDump[100] ;

	FILE *fid = fopen(radFile, "r");
	if ( fid == NULL )
		printf("\n ReadUsSTDfile(...): No se puede abrir el archivo %s \n", radFile) ;

// OBTENGO LA CANTIDAD DE FILAS, O CANTIDAD DE BINES DEL RADIOSONDEO (BAJA RESOLUCION).
	while( !feof(fid) )
	{
	  ch = fgetc(fid);
	  if(ch == '\n')	lines++ ;
	}
	fseek( fid, 0, 0 ) ; // RESET THE POINTER

// READ RADIOSONDE DATA REFERENCED AT *ASL* AND LOW RESOLUTION (LR)
	double *zLR = (double*) new double [lines] ;
	double *pLR = (double*) new double [lines] ;
	double *tLR = (double*) new double [lines] ;
	double *nLR = (double*) new double [lines] ;
	for ( int l=0 ; l<lines ; l++ ) // BARRO LAS LINEAS.
	{
		fgets(lineaRad, 100, fid) ;
		sscanf(lineaRad, "%lf,%lf,%lf,%s", &zLR[l], &tLR[l], &pLR[l], strDump ) ; // a-t-p
	}
// SITE INDEX ALTITUDE (indxMin)
// FIND THE INDEX OF zLR WHERE zLR=siteASL AND WHEN zLR=glbParam->rEndSig
	double  diff ;
	double  diffMin    ; // , diffMax ;
	int 	indxMin =0 ; // , indxMax =0 ;
	for ( int i=0 ; i<lines ; i++ )
	{
		diff = fabs( glbParam->siteASL - zLR[i] ) ; // diff = fabs( dataMol.zr[0] - zLR[i] ) ;
		if ( i==0 )
		{	
			diffMin	= diff 	;
			indxMin = i		;
		}
		else if ( diff < diffMin )
		{	
			diffMin = diff 	;
			indxMin = i 	;
		}
		// diff = fabs( float(dataMol.zr[dataMol.nBins-1]) - zLR[i] ) ;
		// if ( i==0 )
		// {	
		// 	diffMax	= diff 	;
		// 	indxMax = i		;
		// }
		// else if ( diff < diffMax )
		// {	
		// 	diffMax = diff 	;
		// 	indxMax = i 	;
		// }
	}
	RadSondeData.nBinsLR = lines - indxMin ; // indxMax - indxMin +1 ; // 
	RadSondeData.zLR = (double*) new double [RadSondeData.nBinsLR] ;
	RadSondeData.pLR = (double*) new double [RadSondeData.nBinsLR] ;
	RadSondeData.tLR = (double*) new double [RadSondeData.nBinsLR] ;
	RadSondeData.nLR = (double*) new double [RadSondeData.nBinsLR] ;

	for ( int b=0 ; b<RadSondeData.nBinsLR ; b++ ) // BARRO LAS LINEAS.
	{ // RadSondeData.xLR[0] CORRESPOND TO THE POINT OF THE SITE ALTITUDE ASL.
		RadSondeData.zLR[b] = (double) zLR[b+indxMin] ; // RadSondeData.zLR --> ASL
		RadSondeData.tLR[b] = (double) tLR[b+indxMin] ; // RadSondeData.tLR --> ASL
		RadSondeData.pLR[b] = (double) pLR[b+indxMin] ; // RadSondeData.pLR --> ASL
		RadSondeData.nLR[b] = (double) (100*RadSondeData.pLR[b]/RadSondeData.tLR[b]) /1.3800653e-23 ; // [1/m3]? ¡¡¡¡ASL!!!!
	}
	RadSondeData.nHR = (double*) new double [glbParam->nBins] ;

	fclose(fid) ;
	delete zLR ;
	delete pLR ;
	delete tLR ;
	delete nLR ;
}

void CMolecularData_USStd::Fill_dataMol( strcGlobalParameters *glbParam, int indxWL )
{
	int 	i ;

	dataMol->zenith = glbParam->aZenithAVG[glbParam->event_analyzed] ;

	// VALUES FROM ASL
	dataMol[indxWL].nBins = glbParam->nBins ;
	for ( i=0 ; i < glbParam->nBins ; i++ )
		dataMol->zr[i]   	= (double) glbParam->siteASL + glbParam->r[i] * cos(dataMol->zenith *PI/180) ; // zr = ASL
	dataMol[indxWL].dzr = (double)(dataMol[indxWL].zr[1] - dataMol[indxWL].zr[0]) ; // [m]
	glbParam->dzr = dataMol[indxWL].dzr ;

	RadLowToHighRes() ;
	for ( i =0 ; i <glbParam->nBins ; i++ )
		dataMol[indxWL].nMol[i] = RadSondeData.nHR[i] ;
		// double N2_shift = 2331e2 ;
		// double N2_XS_BS = 3.5e-34 * pow( ( (1/glbParam->iLambda[indxWL])-N2_shift ), 4) / pow( (1e9/337.1-N2_shift), 4 ) ;

 	for( i=0 ; i < glbParam->nBins ; i++ )
	{ // betaMol, alphaMol AND betaRam ACROSS r AND ¡¡¡¡¡¡¡ASL, STARTING AT THE SITE ALTITUDE!!!!!!!
		dataMol[indxWL].betaMol [i] = (double)(dataMol[indxWL].nMol[i] * ( 5.45 * pow(10, -32) * pow((550.0/glbParam->iLambda[indxWL]), 4) ) ) ; // r [1/m*sr]
		dataMol[indxWL].alphaMol[i] = (double)(dataMol[indxWL].betaMol[i] * 8.0 * 3.1415/3.0) ; // r [1/m]
		// dataMol[indxWL]->betaRam [i] = (double)(N2_XS_BS * dataMol[indxWL]->nMol[i]) ; // r
	}
		Elastic_Rayleigh_Lidar_Signal ( (strcMolecularData*)&dataMol[indxWL], (double*)glbParam->r ) ;
}

void CMolecularData_USStd::RadLowToHighRes( )
{
	double *coeff = (double*) new double[3 +1] ;

	polyfitCoeff( (const double* const) RadSondeData.zLR, // X DATA
			      (const double* const) RadSondeData.nLR, // Y DATA
			      (unsigned int       ) RadSondeData.nBinsLR,
			      (unsigned int		  ) 3,
			      (double*			  ) coeff	 ) ;

	for (int i =0 ; i <dataMol[0].nBins ; i++ )
	{
		RadSondeData.nHR[i] = (double)( pow(dataMol[0].zr[i], 3) *coeff[3] + pow(dataMol[0].zr[i], 2) *coeff[2] + dataMol[0].zr[i]*coeff[1] + coeff[0] ) ;
	}
	delete coeff ;
}
