
/* GENERAL LIDAR LIBRARY */

#ifndef LIBLIDARROOT_H
#define LIBLIDARROOT_H

#include <TGraph.h>
#include <TH1D.h>

#include <TApplication.h>
#include <TMatrix.h>
#include <TF1.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TSystem.h>
#include <TROOT.h>

#include "TLEvent.h"
#include "LPower.h"

// GPS TIME CONVERSION
#include "TimeStamp.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cmath>
#include <limits.h>
#include <string.h>

#include <stdbool.h>

#include "libLidar.hpp"

#include "CNetCDF_Lidar.hpp"

using namespace std;
using namespace utl;

#define NDIMS 10
#define NVARS 10

/////////////////////////////////////////////////////////////////////////////////////////////////////////

void	splineROOT					 ( int, double*, double*, int, double*, double* 	) ;
int     ReadLidarROOT				 ( char*, strcLidarDataFile*, strcGlobalParameters* ) ;
void 	ReadLidarROOT_3D			 ( char*, double***, strcGlobalParameters*			) ;
void 	ReadGlobalParamROOT			 ( char*, strcGlobalParameters* ) ;
// void 	ReadMolDBAuger				 ( char*, strcRadioSoundingData*, double ) ;
// void 	molDataAuger				 ( char*, strcRadioSoundingData*, strcGlobalParameters*, strcMolecularData* ) ;
// void	ProcessTAMInversion			 ( strcGlobalParameters*, char*, double**, strcFernaldInversion *, strcAerosolData* ) ;
double 	GetAerosolInhomogeneityFactor( double*, double*, strcMolecularData*, strcMolecularData*, strcGlobalParameters*, int, int, strcFernaldInversion* ) ;
void	PreProcesingLidarSignal_Auger( strcGlobalParameters*, strcMolecularData*, strcLidarSignal*, strcIndexMol* ) ;
void 	tam_vaod					 ( double, double, double*, double, double* ) ;
void 	tam_vaod_rho				 ( double, double*, double, double* ) ;
void 	tam_vaod_wFitData			 ( double, double, double*, double, double* ) ;

// void	checkCloudProfiles			( strcGlobalParameters*, strcMolecularData*, strcIndexMol*, strcCloudProfiles*, strcCloudInfoDB* ) ;
void 	fillAugerInfoDB				( strcCloudProfiles*, strcGlobalParameters*, strcMolecularData* ) ;
void  	saveCloudsInfoDB			( strcGlobalParameters*, strcCloudProfiles *, strcDataToSave*, char* ) ;
void 	saveNetCDF_AugerLidarData 	( char*, strcGlobalParameters*, strcDataToSave* ) ;
void 	save_Auger_to_LPP_NetCDF	( char*, strcGlobalParameters*, double *** ) ;

// void	readCLFdata					( char*, char*, strcCLFdata* ) ;
/////////////////////////////////////////////////////////////////////////////////////////////////////////

int ReadLidarROOT( char *strLidarFile, strcLidarDataFile *dataFile, strcGlobalParameters *glbParam )
{
	TFile 	*rootFile ;
	TTree 	*tree = NULL;
	LPower 	lp ;
    TLRunHeader *lrunh ;

 // EVENT GPS SECOND VARIABLES
    int     evty, evtm, evtd, evthh, evtmm, evtss ;
 // Default timezone: GMT --> timeZone = 0
    int     timeZone = -3 ;

	if( !( rootFile = new TFile(strLidarFile) ) )
    {
        cout << "TFile open error on " << strLidarFile << endl;
		return 0 ;
    }
	else if ( !( tree = (TTree*)rootFile->Get("LidarData") ) ) // 
	{
        cout << "No 'LidarData' TTree in TFile" << endl;
		return 0 ;
    }
	else
	{
		lrunh	=	(TLRunHeader*)rootFile->Get("TLRunHeader"); // lrunh	=	(TLRunHeader*)rootFile.Get("TLRunHeader"); // 
		if( lrunh )
		{
		//	cout << "File: " << strLidarFile << " No. Events: " << glbParam->nEvents << endl;
		//  cout << "Log (TLRunHeader): "  << (lrunh->GetLogBook()).Data() << endl;
		}
		else
		{
			cout << "No 'TLRunHeader' found in TFile!" << endl;
			cout << "\nFile: " << strLidarFile << " No. Events: " << glbParam->nEvents << " Log: ?" << endl;
		};
	};
		TBranch *branch  = tree->GetBranch("event");
		if(!branch)
			cout << "Warning:  TTree branch 'event' not found!" << endl;
		else
		{
			TLEvent *levent=0;
			branch->SetAddress(&levent);

			for( int e=0 ; e<glbParam->nEvents ; e++ ) // 
			{
				if(!(tree->GetEntry(e))) // ¿ESTA LINEA ME CAMBIA EL levent DEL drawEvent()?
					cout << "Warning: Error reading event #" << e << endl;
				else
				{
                    lp.SetData( (const TLEvent*)levent, (const int)glbParam->chSel ) ; // lp.SetData(levent, (const int)glbParam->chSel) ;
                    levent->GetTimeStamp( (int*)&evty, (int*)&evtm, (int*)&evtd, (int*)&evthh, (int*)&evtmm, (int*)&evtss ) ; // ME DEVUELVE EN LT

					// ! if ( (evty >=2016) && (evtm >=3) && (evtd >=25) ) PASAR A GPS TIME Y TERMINAR
					// ! timeZone =0 ;	// printf( "\n evty: %d evtm: %d evtd: %d evthh: %d evtmm: %d evtss: %d \n\n", evty, evtm, evtd, evthh, evtmm, evtss ) ;
                    TimeStamp ts((int)evty, (int)evtm, (int)evtd, (int)evthh, (int)evtmm, (int)evtss) ;
                    glbParam->event_gps_sec[e] = (int)(ts.GetGPSSecond() - 3600 * timeZone) ; // SHOULD BE CALLED WITH THE DATE IN UTC (timeZone=0). IF IT'S CALLED IN LT, TIME ZONE CORRECTION SHOULD BE TAKEN INTO ACCOUNT (timeZone=-3).
					// [glbParam->event_gps_sec[e]] = UTC

					for( int b=0 ; b<glbParam->nBinsRaw ; b++ )
					{   // if ( (e==0) && (b<10) )     printf( "\n %u \n",lp.countData[b] ) ;
						dataFile[glbParam->chSel].db_mV[e][b] = (double)lp.rawdata[b] ; // dataFile[glbParam->chSel].db_ADC[e][b] ; // 

						double pho_rateMHz = (double)lp.countData[b] /(glbParam->nShots[glbParam->chSel] * glbParam->tBin_us) ;
						dataFile[glbParam->chSel].db_CountsMHz[e][b] = (double)( pho_rateMHz /( 1.0 - pho_rateMHz / PHO_MAX_COUNT_MHz ) ) ;
					}
					levent->Clear();
					gSystem->ProcessEvents();
				}
			}
		}

// NUMBER OF ZENITHS CLUSTERS
	glbParam->nEventsAVG = 0 ;
	double 	*dZeniths = (double*) new double [glbParam->nEvents -1] ;
	if( (strcmp(glbParam->scanType, "ZD") ==0) || (strcmp(glbParam->scanType, "AD") ==0) )
	{
		for ( int i = 0 ; i<(glbParam->nEvents-1) ; i++ )
			dZeniths[i] = (double)(glbParam->aZenith[i+1] - glbParam->aZenith[i]) ;

		for( int e=0 ; e<glbParam->nEvents-1 ; e++ )
		{
			if ( fabs(dZeniths[e]) >1 )
				glbParam->nEventsAVG++ ;
		}
	}
	else // CONTINUOUS SCAN
		glbParam->nEventsAVG = glbParam->nEvents ;
	// glbParam->indxOffset = (int*) new int [ glbParam->nClusterEvents ] ;
	delete dZeniths ;
	delete tree ;
	delete lrunh ;
	rootFile->Close() ; // rootFile.Close() ; // 
	return 0 ;
} // FIN DEL ReadLidarROOT()

void ReadLidarROOT_3D( char *strLidarFile, double ***Raw_Lidar_Data, strcGlobalParameters *glbParam )
{
	TFile 	*rootFile ;
	TTree 	*tree = NULL;
	LPower 	lp ;
    TLRunHeader *lrunh ;

 // EVENT GPS SECOND VARIABLES
    int     evty, evtm, evtd, evthh, evtmm, evtss ;
 // Default timezone: GMT --> timeZone = 0
    int     timeZone = -3 ;

	if( !( rootFile = new TFile(strLidarFile) ) ) // if( !( rootFile(strLidarFile) ) ) // 
    {
        cout << "TFile open error on " << strLidarFile << endl;
    }
	else if ( !( tree = (TTree*)rootFile->Get("LidarData") ) ) // 
	{
        cout << "No 'LidarData' TTree in TFile" << endl;
    }
	else
	{
		lrunh	=	(TLRunHeader*)rootFile->Get("TLRunHeader"); // lrunh	=	(TLRunHeader*)rootFile.Get("TLRunHeader"); // 
		if( lrunh )
		{
		//	cout << "File: " << strLidarFile << " No. Events: " << glbParam->nEvents << endl;
		//  cout << "Log (TLRunHeader): "  << (lrunh->GetLogBook()).Data() << endl;
		}
		else
		{
			cout << "No 'TLRunHeader' found in TFile!" << endl;
			cout << "\nFile: " << strLidarFile << " No. Events: " << glbParam->nEvents << " Log: ?" << endl;
		};
	};
		TBranch *branch  = tree->GetBranch("event");
		if(!branch)
			cout << "Warning:  TTree branch 'event' not found!" << endl;
		else
		{
			TLEvent *levent=0;
			branch->SetAddress(&levent);
			for( int e=0 ; e<glbParam->nEvents ; e++ ) // 
			{
				if(!(tree->GetEntry(e))) // ¿ESTA LINEA ME CAMBIA EL levent DEL drawEvent()?
					cout << "Warning: Error reading event #" << e << endl;
				else
				{
					for( int c=0 ; c <glbParam->nCh ; c++ )
					{
						// lp.SetData( (const TLEvent*)levent, (const int)glbParam->chSel ) ; // lp.SetData(levent, (const int)glbParam->chSel) ;
						lp.SetData( (const TLEvent*)levent, (const int)c ) ; // lp.SetData(levent, (const int)glbParam->chSel) ;
						levent->GetTimeStamp( (int*)&evty, (int*)&evtm, (int*)&evtd, (int*)&evthh, (int*)&evtmm, (int*)&evtss ) ; // ME DEVUELVE EN LT

						// ! if ( (evty >=2016) && (evtm >=3) && (evtd >=25) ) PASAR A GPS TIME Y TERMINAR
						// ! timeZone =0 ;	// printf( "\n evty: %d evtm: %d evtd: %d evthh: %d evtmm: %d evtss: %d \n\n", evty, evtm, evtd, evthh, evtmm, evtss ) ;
						TimeStamp ts((int)evty, (int)evtm, (int)evtd, (int)evthh, (int)evtmm, (int)evtss) ;
						glbParam->event_gps_sec[e] = (int)(ts.GetGPSSecond() - 3600 * timeZone) ; // SHOULD BE CALLED WITH THE DATE IN UTC (timeZone=0). IF IT'S CALLED IN LT, TIME ZONE CORRECTION SHOULD BE TAKEN INTO ACCOUNT (timeZone=-3).
						// [glbParam->event_gps_sec[e]] = UTC

						for( int b=0 ; b<glbParam->nBinsRaw ; b++ )
						{   // if ( (e==0) && (b<10) )     printf( "\n %u \n",lp.countData[b] ) ;
							// dataFile[glbParam->chSel].db_mV[e][b] = (double)lp.rawdata[b] ; // dataFile[glbParam->chSel].db_ADC[e][b] ; // 
							Raw_Lidar_Data[e][c][b] = (double)lp.rawdata[b] ;

							// double pho_rateMHz = (double)lp.countData[b] /(glbParam->nShots[glbParam->chSel] * glbParam->tBin_us) ;
							// dataFile[glbParam->chSel].db_CountsMHz[e][b] = (double)( pho_rateMHz /( 1.0 - pho_rateMHz / PHO_MAX_COUNT_MHz ) ) ;
						}
					}
					levent->Clear();
					gSystem->ProcessEvents();
				}
			}
		}

// NUMBER OF ZENITHS CLUSTERS
	glbParam->nEventsAVG = 0 ;
	double 	*dZeniths = (double*) new double [glbParam->nEvents -1] ;
	if( (strcmp(glbParam->scanType, "ZD") ==0) || (strcmp(glbParam->scanType, "AD") ==0) )
	{
		for ( int i = 0 ; i<(glbParam->nEvents-1) ; i++ )
			dZeniths[i] = (double)(glbParam->aZenith[i+1] - glbParam->aZenith[i]) ;

		for( int e=0 ; e<glbParam->nEvents-1 ; e++ )
		{
			if ( fabs(dZeniths[e]) >1 )
				glbParam->nEventsAVG++ ;
		}
	}
	else // CONTINUOUS SCAN
		glbParam->nEventsAVG = glbParam->nEvents ;
	// glbParam->indxOffset = (int*) new int [ glbParam->nClusterEvents ] ;
	delete dZeniths ;
	delete tree ;
	delete lrunh ;
	rootFile->Close() ; // rootFile.Close() ; // 
}

void ReadGlobalParamROOT( char *strLidarFile, strcGlobalParameters *glbParam )
{
	TFile 	*rootFile ;
	TTree 	*tree = NULL;
	LPower 	lp ;
	TLRunHeader *lrunh ;

	char *pch = strstr( strLidarFile, "/lidar-" ) ;
	sscanf(pch+7, "%2s-%4d%2d%2d-%2d%2d%2d-R%5d.root",
		   glbParam->site, &glbParam->year, &glbParam->month, &glbParam->day, &glbParam->hour, &glbParam->min, &glbParam->sec, &glbParam->run ) ;
		 if ( strncmp( glbParam->site, "ll", 2 ) == 0 )
		 	ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "LL_ASL" , "double", (double*)&glbParam->siteASL ) ; // glbParam->siteASL = 1416.4 ;
	else if ( strncmp( glbParam->site, "ch", 2 ) == 0 )
			ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "CH_ASL" , "double", (double*)&glbParam->siteASL ) ; // glbParam->siteASL = 1712.3 ;
	else if ( strncmp( glbParam->site, "lm", 2 ) == 0 )
			ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "LM_ASL" , "double", (double*)&glbParam->siteASL ) ; // glbParam->siteASL = 1416.4 ;
	else if ( strncmp( glbParam->site, "la", 2 ) == 0 )
			ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "LA_ASL" , "double", (double*)&glbParam->siteASL ) ; // glbParam->siteASL = 1476.7 ;
// printf("\n\nsite: %s \n", glbParam->site) ;
// printf("Site ASL: %lf \n", glbParam->siteASL) ;
   char    used_channel[20] ;
// READ THE CHANNEL (TELESCOPE) NUMBER TO READ IN ch
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "used_channel", "string", (char*)used_channel ) ; // used_channel = long_range_2 - long_range_1 - nothing
    glbParam->nChMax    = MAX_CH ; // MAX NUMBER OF CHANNELS: LL=2, LA=LM=CO=3
	glbParam->chSel     = (int)ReadChannelSelected( glbParam ) ;
	// printf( "\n Channel used: %d \n", glbParam->chSel ) ;
	assert( glbParam->chSel != -1 ) ;  // SI SE CUMPLE, SIGUE.

	if( !( rootFile = new TFile(strLidarFile) ) )
    {
        cout << "TFile open error on " << strLidarFile << endl;
    }
	else if ( !( tree = (TTree*)rootFile->Get("LidarData") ) )
	{
        cout << "No 'LidarData' TTree in TFile" << endl;
    }
	else
	{
		lrunh	=	(TLRunHeader*)rootFile->Get("TLRunHeader"); // TLRunHeader *lrunh	=	(TLRunHeader*)rootFile.Get("TLRunHeader") ; // 
		glbParam->nEvents	=	int(tree->GetEntries()) ; // NUMBERS OF PROFILES.

		if( lrunh )
		{
			cout << "\nFile: " << strLidarFile << " No. Events: " << glbParam->nEvents << endl;
		    // cout << "Log (TLRunHeader): "  << (lrunh->GetLogBook()).Data() << endl;
		    cout << (lrunh->GetLogBook()).Data() << endl;
		    sprintf( glbParam->infoFile, "%s", (lrunh->GetLogBook()).Data() ) ;

				 if (  (strstr(glbParam->infoFile, "Zenith" ) != NULL) && (strstr(glbParam->infoFile, "Discrete") != NULL) )
				strcpy(glbParam->scanType, "ZD") ;
            else if ( (strstr(glbParam->infoFile, "Azimuth") != NULL) && (strstr(glbParam->infoFile, "Discrete") != NULL)  )
				strcpy(glbParam->scanType, "AD") ;
			else if ( (strstr(glbParam->infoFile, "Zenith" ) != NULL) && (strstr(glbParam->infoFile, "Continuous") != NULL) )
				strcpy(glbParam->scanType, "ZC") ;
			else if ( (strstr(glbParam->infoFile, "Azimuth") != NULL) && (strstr(glbParam->infoFile, "Continuous") != NULL) )
				strcpy(glbParam->scanType, "AC") ;
		}
		else
		{
			cout << "No 'TLRunHeader' found in TFile!" << endl;
			cout << "\nFile: " << strLidarFile << " No. Events: " << glbParam->nEvents << " Log: ?" << endl;
		}
	}
		TBranch *branch  = tree->GetBranch("event");
		if(!branch)
			cout << "Warning:  TTree branch 'event' not found!" << endl;
		else
		{
			TLEvent *levent=0;
			branch->SetAddress((TLEvent*)&levent);

			for( int i=0 ; i<glbParam->nEvents ; i++ )
			{
				if(!(tree->GetEntry(i))) // ¿ESTA LINEA ME CAMBIA EL levent DEL drawEvent()?
					cout << "Warning: Error reading event #" << i << endl;
				else
				{
					lp.SetData(levent, 0); // I ONLY READ MODULE 0, TO READ GLOBAL PARAMETERS

					if ( i==0 )
					{
						lp.Get_nCh_nBinsRaw( (const TLEvent*)levent, (int*)&glbParam->nCh, (int*)&glbParam->nBins_in_File ) ;
						// glbParam->nChMax	= MAX_CH ; // MAX NUMBER OF CHANNELS: LL=2, LA=LM=CO=3
						ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "nBinsRaw", "int", (double*)&glbParam->nBinsRaw ) ;
						glbParam->nShots[glbParam->chSel] 	= levent->GetNLaserShots(); // printf("\n levent->GetNLaserShots(): %d \n", glbParam->nShots) ;
						ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "dr", "double", (double*)&glbParam->dr ) ; // [m]
						glbParam->lambda	 = (double)351 ;
						glbParam->aZenith 	 = (double*) new double [glbParam->nEvents] ;
						glbParam->aZenithAVG = (double*) new double [glbParam->nEvents] ;
						glbParam->aAzimuth 	 = (double*) new double [glbParam->nEvents] ;
					}
					lp.zenith  = (double) lp.zenith  * PI/180 ; // MAYOR AXIS
					lp.azimuth = (double) lp.azimuth * PI/180 ; // MINOR AXIS
					glbParam->aZenith[i]  = 90 - asin( cos(lp.azimuth) * cos(lp.zenith) ) * 180/PI  ; // [º]
					glbParam->aAzimuth[i] = abs(atan( - sin(lp.zenith)/tan(lp.azimuth) )  * 180/PI) ; // [º]

					levent->Clear() ;
					gSystem->ProcessEvents() ;
				};
			}
		};
	glbParam->nLambda =1 ;
	// double 	rOffset ; 
	ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "indxOffset" , "int" , (int*)&glbParam->indxOffset[0] ) ;
	// double 	rOffset ; ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "rOffset" , "double" , (double*)&rOffset ) ;
	// glbParam->binOffset	= (int)round(rOffset/(glbParam->dr) ) ;
	glbParam->nBins 	= glbParam->nBinsRaw - glbParam->indxOffset[0] ;
	ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "nBinsBkg", "int"   , &glbParam->nBinsBkg ) ;

  	ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "rInitSig", "double", (double*)&glbParam->rInitSig ) ;
	ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "rEndSig" , "double", (double*)&glbParam->rEndSig  ) ;
	glbParam->indxInitSig = (int) round( glbParam->rInitSig / (glbParam->dr) ) -1 ;
	glbParam->indxEndSig  = (int) round( glbParam->rEndSig  / (glbParam->dr) ) -1 ;
	glbParam->indxEndSigEvnt = (int*) new int[glbParam->nEvents] ;
	for (int i =0; i <glbParam->nEvents; i++)
		glbParam->indxEndSigEvnt[i] = glbParam->indxEndSig ;

    glbParam->indxInitInversion = glbParam->indxInitSig ;

//printf("\n glbParam->chSel: %d \n", glbParam->chSel) ;
	glbParam->tBin_us = pow(10, 6) * 2*glbParam->dr /(3*pow(10, 8)) ;
// printf("\n glbParam->tBin_us: %f \n", glbParam->tBin_us ) ;
    // PRODUCT DATA LEVEL
    ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "PDL", "string", (char*)glbParam->PDL ) ; 
    ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "OUTPUT_FILE", "string", (char*)glbParam->OUTPUT_FILE ) ; 

	delete tree ;
	rootFile->Close() ;
}

void splineROOT( int nLR, double *xLR, double *yLR, int nHR, double *xHR, double *yHR )
{ // yHR: OUTPUT (REST OF THE PARAMETERS ARE INPUTS)

	TGraph 	*g = new TGraph( (Int_t)nLR, (double*)xLR, (double*)yLR ) ; // TGraph(n, x, y) ;
	TF1		*f = new TF1("f", "pol3", xLR[0], xLR[nLR-1] ); //    xLR[nLR-1] = 30 km

	g->Fit(f, "QR") ;

	Double_t p0 = f->GetParameter(0) ;
	Double_t p1 = f->GetParameter(1) ;
	Double_t p2 = f->GetParameter(2) ;
	Double_t p3 = f->GetParameter(3) ;

	int i ;
	for ( i=0 ; i<nHR ; i++ )
		yHR[i] = p3 * pow(xHR[i], 3) + p2 * pow(xHR[i], 2) + p1 * xHR[i] + p0 ;

	delete g ;
	delete f ;
}

// void ReadMolDBAuger( char *radFile, strcRadioSoundingData *RadData, double siteASL )
// {
// 	char 	ch ;
// 	int 	lines=0 ;
// 	char 	lineaRad[100], *pch ;

// 	FILE *fid = fopen(radFile, "r");
// 	if ( fid == NULL )
// 		printf("\n no se puede abrir el archivo %s \n", radFile) ;

// // OBTENGO LA CANTIDAD DE FILAS, O CANTIDAD DE BINES DEL RADIOSONDEO (BAJA RESOLUCION).
// 	while( !feof(fid) )
// 	{
// 	  ch = fgetc(fid);
// 	  if(ch == '\n')	lines++ ;
// 	}
// 	lines = lines - 2 ;
// 	fseek( fid, 0, 0 ) ; // RESET THE POINTER

// 	fgets(lineaRad, 100, fid) ; // READE THE HEADER
// 	fgets(lineaRad, 100, fid) ; // READE THE HEADER

// // READ RADIOSONDE DATA REFERENCED AT *ASL* AND LOW RESOLUTION (LR)
// 	int nBinsLR = lines ;
// 	double *zLR = (double*) new double [lines] ;
// 	double *pLR = (double*) new double [lines] ;
// 	double *tLR = (double*) new double [lines] ;
// 	for ( int l=0 ; l<lines ; l++ ) // BARRO LAS LINEAS.
// 	{
// 		fgets(lineaRad, 100, fid) ;
// 		pch = strtok(lineaRad,"\t") ;
// 			for( int i=0 ; i<3 ; i++ )
// 			{
// 					if (i==0)
// 						zLR[l] = (double)(atof(pch))*1000 ; // [m]
// 					else if (i==1)
// 						tLR[l] = (double)atof(pch) ; // [K]
// 					else if (i==2)
// 						pLR[l] = (double)atof(pch) ; // [hPa]
// 				pch = strtok (NULL, "\t") ;
// 			}
// 	}
// 	fclose(fid) ;
// // SITE INDEX ALTITUDE (indxMin)
// // FIND THE INDEX OF zLR WHERE zLR=siteASL
// 	double  *diff = (double*) new double [nBinsLR] ;
// 	double  min ;
// 	int 	indxMin = 0 ;
// 	for ( int i=0 ; i<nBinsLR ; i++ )
// 	{
// 		diff[i] = fabs( siteASL - zLR[i] ) ;
// 		if ( i==0 )
// 		{	min 	= diff[i] 	;
// 			indxMin = i 		;
// 		}
// 		else if ( diff[i] < min )
// 		{	min 	= diff[i] 	;
// 			indxMin = i 		;
// 		}
// 	}
// 	RadData->nBinsLR = lines - indxMin ;
// 	RadData->zLR = (double*) new double [RadData->nBinsLR] ;
// 	RadData->pLR = (double*) new double [RadData->nBinsLR] ;
// 	RadData->tLR = (double*) new double [RadData->nBinsLR] ;
// 	RadData->nLR = (double*) new double [RadData->nBinsLR] ;

// 	for ( int b=0 ; b<RadData->nBinsLR ; b++ ) // BARRO LAS LINEAS.
// 	{ // RadData->xLR[0] CORRESPOND TO THE POINT OF THE SITE ALTITUDE.
// 		RadData->zLR[b] = zLR[b+indxMin] ; // RadData->zLR --> ASL
// 		RadData->tLR[b] = tLR[b+indxMin] ; // RadData->tLR --> ASL
// 		RadData->pLR[b] = pLR[b+indxMin] ; // RadData->pLR --> ASL
// 	}

// 	delete zLR  ;
// 	delete pLR  ;
// 	delete tLR  ;
// 	delete diff ;
// }

// void molDataAuger( char *radFile, strcRadioSoundingData *RadSondeData, strcGlobalParameters *glbParam, strcMolecularData *dataMol )
// {
// 	if ( RadSondeData->radSoundingDataLOADED == false )
// 	{
// 		RadSondeData->radSoundingDataLOADED = true ;
// 		ReadMolDBAuger( (char*)radFile, (strcRadioSoundingData*)&RadSondeData, (double)glbParam->siteASL ) ;

// 		// RadSondeData->nLR IN LOW RESOLUTION AND ZENITHAL DIRECTION. VALUES REFERENCED TO ASL
// 		for ( int i=0 ; i<RadSondeData->nBinsLR ; i++ )
// 			RadSondeData->nLR[i] = (100*RadSondeData->pLR[i]/RadSondeData->tLR[i]) /1.3800653e-23 ; // [1/m3]? ¡¡¡¡ASL!!!!
// 		// splineROOT( RadSondeData->nBinsLR, RadSondeData->zLR, RadSondeData->nLR, glbParam->nBins, dataMol->zr, dataMol->nMol ) ; // dataMol->nMol ACROSS r
// 		// printf("\n\n RadSondeData->zLR[0]= %f \t dataMol->zr[0]= %f \n", RadSondeData->zLR[0], dataMol->zr[0]) ;
// 		// printf("\n RadSondeData->zLR[end]= %f \t dataMol->zr[end]= %f \n\n", RadSondeData->zLR[RadSondeData->nBinsLR -1], dataMol->zr[glbParam->nBins -1]) ;
// 		// splineGSL ( (int)RadSondeData->nBinsLR, (double*)RadSondeData->zLR, (double*)RadSondeData->nLR, (int)glbParam->nBins, (double*)dataMol->zr, (double*)dataMol->nMol ) ;
// 		// polyfitGSL( (int)RadSondeData->nBinsLR, (double*)RadSondeData->zLR, (double*)RadSondeData->nLR, (int)4,    (int)glbParam->nBins, (double*)dataMol->zr, (double*)dataMol->nMol ) ;
// 		// smoothGSL( (double*)dataMol->nMol, (int)dataMol->nBins, (int)11, (double*)dataMol->nMol ) ;		
// 		// printf("\n\n RadSondeData->pLR[0]= %f \n", RadSondeData->pLR[0] ) ;
// 		// printf("\n RadSondeData->pLR[end]= %f \n\n", RadSondeData->pLR[RadSondeData->nBinsLR -1] ) ;
// 		// printf("\n dataMol->zenith: %lf \n", dataMol->zenith) ;
// 	}
// 	dataMol->nBins = glbParam->nBins ;
// 	// zr ASL
// 	for ( int i=0 ; i < glbParam->nBins ; i++ )
// 		dataMol->zr[i]   	= (double) glbParam->siteASL + glbParam->r[i] * cos(dataMol->zenith *PI/180) ; // zr = ASL
// 	dataMol->dzr = (double)(dataMol->zr[1] - dataMol->zr[0]) ;
// 	// double N2_shift = 2331e2 ;
// 	// double N2_XS_BS = 3.5e-34 * pow( ( (1/glbParam->lambda)-N2_shift ), 4) / pow( (1e9/337.1-N2_shift), 4 ) ;
// 	// polyfitGSL( (int)RadSondeData->nBinsLR, (double*)RadSondeData->zLR, (double*)RadSondeData->nLR, (int)3, (int)glbParam->nBins, (double*)dataMol->zr, (double*)dataMol->nMol ) ;
// 	// splineGSL( (int)RadSondeData->nBinsLR, (double*)RadSondeData->zLR, (double*)RadSondeData->nLR, (int)glbParam->nBins, (double*)dataMol->zr, (double*)dataMol->nMol ) ;
// 	RadLowToHighRes( (int)RadSondeData->nBinsLR, (double*)RadSondeData->zLR, (double*)RadSondeData->nLR, (int)glbParam->nBins, (double*)dataMol->zr, (double*)dataMol->nMol ) ;
// 	for( int i=0 ; i < glbParam->nBins ; i++ )
// 	{ // betaMol, alphaMol AND betaRam ACROSS r AND ¡¡¡¡¡¡¡ASL, STARTING AT THE SITE ALTITUDE!!!!!!!
// 		dataMol->betaMol [i] = (double)(dataMol->nMol[i] * ( 5.45 * pow(10, -32) * pow((550.0/glbParam->lambda), 4) ) ) ; // r [1/m*sr]
// 		dataMol->alphaMol[i] = (double)(dataMol->betaMol[i] * 8.0 * 3.1415/3.0) ; // r [1/m]
// 		// dataMol->betaRam [i] = (double)(N2_XS_BS * dataMol->nMol[i]) ; // r
// 	}
// 		Elastic_Rayleigh_Lidar_Signal ( (strcMolecularData*)dataMol, (double*)glbParam->r ) ;
// }


/*
 ANULADO MOMENTANEAMENTE HASTA NO TENER UNA CLASE QUE MANEJE LA LECTURA DEL RADIOSONDEO (1 SOLA VEZ) Y CONTENGA A:
 - molDataAuger
 - ReadMolDBAuger

void ProcessTAMInversion( strcGlobalParameters *glbParam, char* radFile, double **mtxSig, strcFernaldInversion *fernaldVectors, strcAerosolData *dataAer )
{
// SELECT THE EVENTS TO PROCESS
	int 		aEv[2]     	;
	double  	aZenEv[2]  	;
	strcTheta	aTheta 		;
	double		H 		 	;

	ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "thetaRef"  , "double" , (double*)&aTheta.thetaRef  ) ;
	ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "thetaDiff" , "double" , (double*)&aTheta.thetaDiff ) ;

	chooseEventNumber( (strcGlobalParameters*)glbParam, (strcTheta*)&aTheta, (int*)aEv, (double*)aZenEv ) ;
	// printf("\n ProcessTAMInversion(...)--> Angles choosen/event number: %lf/%d - %lf/%d", aTheta.thetaRef, aEv[0], aTheta.thetaDiff, aEv[1] ) ;
	// printf("\n ProcessTAMInversion(...)--> Angles selected/event selected: %lf/%d - %lf/%d", aZenEv[0], aEv[0], aZenEv[1], aEv[1] ) ;

    double **prMtrx = (double**) new double*[2] ;
    for ( int i =0 ; i <2 ; i++ )
        prMtrx[i] = (double*) new double[glbParam->nBins] ;
    for ( int i =0 ; i <glbParam->nBins ; i++ )
    {
        prMtrx[0][i] = (double) mtxSig[aEv[0]][i] ;
        prMtrx[1][i] = (double) mtxSig[aEv[1]][i] ;
    }
	strcFitParam	fitParam ;
	// fitParam.indxInicFit = glbParam->nBins - glbParam->nBinsBkg ; // glbParam->indxEndSig - glbParam->nBinsBkg ; // 
	// fitParam.indxEndFit  = glbParam->nBins ; // glbParam->indxEndSig ; // 
	// fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInicFit ;
	// 	bkgSubstractionMean	  ( (double*)prMtrx[0], fitParam.indxInicFit, fitParam.indxEndFit, glbParam->nBins, (double*)prMtrx[0] ) ;
	// 	bkgSubstractionMean	  ( (double*)prMtrx[1], fitParam.indxInicFit, fitParam.indxEndFit, glbParam->nBins, (double*)prMtrx[1] ) ;

		int 	spamAvgWin ;	ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "spamAvgWin", "int", &spamAvgWin ) ;
		smooth( (double*)prMtrx[0], (int)0, (int)(glbParam->nBins-1), (int)spamAvgWin, (double*)prMtrx[0] ) ;
		smooth( (double*)prMtrx[1], (int)0, (int)(glbParam->nBins-1), (int)spamAvgWin, (double*)prMtrx[1] ) ;
		// smoothGSL( (double*)prMtrx[0], (int)(glbParam->nBins), (int)spamAvgWin, (double*)prMtrx[0] ) ;
		// smoothGSL( (double*)prMtrx[1], (int)(glbParam->nBins), (int)spamAvgWin, (double*)prMtrx[1] ) ;

	if ( (aZenEv[0] >=0) && (aZenEv[1] >=0) )
	{	// MOLECULAR DATA READOUT FOR EACH EVENT
		// printf("\n ProcessTAMInversion(...)--> (%d-%d) zenithal angles: %lf - %lf ", aTheta.thetaRef, aTheta.thetaDiff, aZenEv[0], aZenEv[1] ) ;
		strcMolecularData	dataMolV, dataMolS ;
		if ( dataMolV.alphaMol == NULL )     GetMem_dataMol( (strcMolecularData*)&dataMolV, (strcGlobalParameters*)glbParam );
		if ( dataMolS.alphaMol == NULL )     GetMem_dataMol( (strcMolecularData*)&dataMolS, (strcGlobalParameters*)glbParam );
		dataMolV.zenith = aZenEv[0] ; // [º]
			molDataAuger( (char*)radFile, (strcGlobalParameters*)glbParam, (strcMolecularData*)&dataMolV ) ; // dataMol: REFERENCED TO zr (vertical)
		dataMolS.zenith = aZenEv[1] ; // [º]
			molDataAuger( (char*)radFile, (strcGlobalParameters*)glbParam, (strcMolecularData*)&dataMolS ) ; // dataMol: REFERENCED TO zr (vertical)

		int 	TAMheightStart ;
		ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "TAMheightStart" , "int" , &TAMheightStart ) ;
		int 	TAMheightStep ;
		ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "TAMheightStep" , "int" , &TAMheightStep ) ;
		int 	TAMStepNumber ;
		ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "TAMStepNumber" , "int" , &TAMStepNumber ) ;

		int 	dNFitTAM ;
		ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "dNFitTAM" , "int" , &dNFitTAM ) ;

		int 	binZref		= round( (TAMheightStart -glbParam->siteASL)/glbParam->dr ) ; // GROUND LEVEL REFERENCED
		// if ( binZref )
		int		binVAODev0 	= (int) round( binZref / cos(aZenEv[0] *PI/180) ) ;  // GROUND LEVEL REFERENCED
		int 	binVAODev1 	= (int) round( binZref / cos(aZenEv[1] *PI/180) ) ;  // GROUND LEVEL REFERENCED
		H =	(double)GetAerosolInhomogeneityFactor( (double*)prMtrx[0], (double*)prMtrx[1], (strcMolecularData*)&dataMolV, (strcMolecularData*)&dataMolS, (strcGlobalParameters*)glbParam, (int)binVAODev0, (int)binVAODev1, (strcFernaldInversion*)fernaldVectors ) ;

		double m0 =0, m1=0 ;
		for( int i=0 ; i<TAMStepNumber ; i++ )
		{
			binZref		= round( ( (TAMheightStart -glbParam->siteASL) +i*TAMheightStep)/glbParam->dr ) ; // GROUND LEVEL REFERENCED
			binVAODev0 	= (int) round( binZref / cos(aZenEv[0] *PI/180) ) ;  // GROUND LEVEL REFERENCED
			binVAODev1 	= (int) round( binZref / cos(aZenEv[1] *PI/180) ) ;  // GROUND LEVEL REFERENCED

			trapz( glbParam->dr, dataMolV.alphaMol, 0, binZref, &dataMolV.MOD_REF ) ;
			// printf("\n \t\t TAMheight: %d \t binRef_0: %d \t binRef_1: %d \t dNFitTAM: %d", TAMheightStart +i*TAMheightStep, binVAODev0, binVAODev1, dNFitTAM ) ;
			fitParam.indxInicFit = binVAODev0 -dNFitTAM ;
			fitParam.indxEndFit  = binVAODev0 +dNFitTAM ;
			fitParam.nFit		 = fitParam.indxEndFit - fitParam.indxInicFit ;
				RayleighFit( (double*)prMtrx[0], (double*)dataMolV.prMol, dataMolV.nBins, "wOutB", "NOTall", (strcFitParam*)&fitParam, (double*)prMtrx[0] ) ;
			m0 = m0 + fitParam.m ;
			fitParam.indxInicFit = binVAODev1 -dNFitTAM ;
			fitParam.indxEndFit  = binVAODev1 +dNFitTAM ;
			fitParam.nFit		 = fitParam.indxEndFit - fitParam.indxInicFit ;
				RayleighFit( (double*)prMtrx[1], (double*)dataMolS.prMol, dataMolS.nBins, "wOutB", "NOTall", (strcFitParam*)&fitParam, (double*)prMtrx[1] ) ;
			m1 = m1 + fitParam.m ;
			// printf("\tAcumm --> m0: %E \t m1:%E", m0, m1) ;
		}
			m0 = m0 / TAMStepNumber ;
			m1 = m1 / TAMStepNumber ;
			// printf("\nrho Averaged (%d): %lf", TAMStepNumber, rho) ;

			// tam_vaod( sigEv0, sigEv1, aZenEv, dataMolV.MOD_REF, &dataAer->VAOD_REF ) ;
			// tam_vaod_rho( rho, aZenEv, dataMolV.MOD_REF, (double*)&dataAer->VAOD_REF ) ;
			tam_vaod_wFitData ( (double)m0, (double)m1, (double*)aZenEv, (double)H, (double*)&dataAer->VAOD_REF ) ;

			// printf("\n------------------------------------------------------------------------------------------------------------------------\n");
			printf("\n\t\t TAM-VAOD: %.2f (@ %i m with %fº and %fº) \n", dataAer->VAOD_REF, TAMheightStart, aZenEv[0], aZenEv[1]) ;
			printf("------------------------------------------------------------------------------------------------------------------------\n") ;
 	}
	else
		dataAer->VAOD_REF = -200 ; // -2: THERE'S NO ZENITHALS ANGLES TO CHOOSE BASED ON THE CONFIGURATION FILE.
}
*/
void tam_vaod_wFitData ( double m0, double m1, double *aZenEv, double H, double *VAODzm )
{ // TAM METHOD OBTAINED WITH FITTED PARAMETERS IN P(r)
	// double 	C0 = 1/cos(aZenEv[0] *PI/180) ;
	// double  C1 = 1/cos(aZenEv[1] *PI/180) ;
	// double 	S  = -2*( C0 - C1 ) ;
	// *VAODzm = (double)log(m0/m1) / S ;

	double 	S  = -0.5*( 1 - 1/H ) ;
	*VAODzm = S * log(m0/m1) ;
}

void tam_vaod( double pr_ev0_zm, double pr_ev1_zm, double *aZenEv, double MOD_REF, double *VAOD_REF )
{ // TAM METHOD OBTAINED WITH P(r)

	double 	C  = ( 1/cos(aZenEv[0] *PI/180) ) - ( 1/cos(aZenEv[1] *PI/180) ) ;
	double 	CosZenRate = pow(cos(aZenEv[0] *PI/180), 2) / pow(cos(aZenEv[1] *PI/180), 2) ; // IF pr_ev0_zm (NOT RANGE-CORRECTED)

	double 	Tm2C_V_zm  = exp( -2* C * MOD_REF ) ;

	double 	rho = pr_ev0_zm / pr_ev1_zm ; // 0.82* 

	double	rho_1 = rho / ( CosZenRate * Tm2C_V_zm ) ;

	*VAOD_REF = log(rho_1) /(-2*C) ;

	// printf("\n tam_vaod: ") ;
	// printf( "\n Tm2C_V_zm (>1): %f ", Tm2C_V_zm ) ;
	// printf("\n C (<0): %f \n rho(>1): %f \n CosZenRate(>1): %f \n rho_1: %f \n", C, rho, CosZenRate, rho_1 ) ;
	printf("\n rho(>1): %f", rho) ;
	// printf("\n (1/(-2*C)) %f \n log(rho_1): %lf", 1/(-2*C), log(rho_1) ) ;
	// printf("\n VAOD: %f \n", *VAOD_REF ) ;
}

void tam_vaod_rho( double rho, double *aZenEv, double MOD_REF, double *VAOD_REF )
{ // TAM METHOD OBTAINED WITH P(r)

	double 	C  = ( 1/cos(aZenEv[0] *PI/180) ) - ( 1/cos(aZenEv[1] *PI/180) ) ;
	double 	CosZenRate = pow(cos(aZenEv[0] *PI/180), 2) / pow(cos(aZenEv[1] *PI/180), 2) ; // IF pr_ev0_zm (NOT RANGE-CORRECTED)

	double 	Tm2C_V_zm  = exp( -2* C * MOD_REF ) ;

	double	rho_1 = rho / ( CosZenRate * Tm2C_V_zm ) ;

	*VAOD_REF = log(rho_1) /(-2*C) ;
}

double GetAerosolInhomogeneityFactor( double *prMtrx_0, double *prMtrx_1, strcMolecularData *dataMolV, strcMolecularData *dataMolS, strcGlobalParameters *glbParam, int binVAODev0, int binVAODev1, strcFernaldInversion *fernaldVectors )
{
	strcAerosolData dataAerV, dataAerS ;
	GetMem_dataAer( (strcAerosolData*)&dataAerV, (strcGlobalParameters*)glbParam ) ;
	GetMem_dataAer( (strcAerosolData*)&dataAerS, (strcGlobalParameters*)glbParam ) ;
	
	FernaldInversion_pr( (double*)prMtrx_0, (strcMolecularData*)dataMolV, (strcGlobalParameters*)glbParam, (int)binVAODev0, (double)(1/50.0), (strcFernaldInversion*)fernaldVectors, (strcAerosolData*)&dataAerV ) ;
	FernaldInversion_pr( (double*)prMtrx_1, (strcMolecularData*)dataMolS, (strcGlobalParameters*)glbParam, (int)binVAODev1, (double)(1/50.0), (strcFernaldInversion*)fernaldVectors, (strcAerosolData*)&dataAerS ) ;

	double H = dataAerV.SAOD / dataAerS.SAOD ; // dataAerV.VAOD / dataAerS.VAOD ; // (dataAerS.SAOD / dataAerV.VAOD) * cos( (dataMolS->zenith - dataMolV->zenith)*PI/180 ) ;
	printf("\n\n H: %lf (dataAerV.SAOD / dataAerS.SAOD): %lf / %lf \n", H, dataAerV.VAOD, dataAerS.VAOD) ;

	return H ;
}

void PreProcesingLidarSignal_Auger( strcGlobalParameters *glbParam, strcMolecularData *dataMol, strcLidarSignal *evSig, strcIndexMol *indxMol )
{
// BACKGROUND SUPRESSION FIT LIMITS
	strcFitParam	fitParam ;
		// MakeRangeCorrected( (strcGlobalParameters*)glbParam, (strcMolecularData*)dataMol, (strcLidarSignal*)evSig )  ;
       	// LowRangeCorrection( (strcGlobalParameters*)glbParam, (double*)evSig->pr2 ) ;

	int 	spamAvgWin ;	ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "spamAvgWin", "int", &spamAvgWin ) ;
	// smoothGSL( (double*)evSig->pr2, (int)(glbParam->nBins), (int)spamAvgWin, (double*)evSig->pr2 ) ;
	smooth( (double*)evSig->pr2, (int)0, (int)(glbParam->nBins-1), (int)spamAvgWin, (double*)evSig->pr2 ) ;

	glbParam->indxInitErr = glbParam->indxInitInversion ; // glbParam->indxInitSig ;

	char Rayleight_Fit_TYPE[10] ;
	ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "Rayleight_Fit_TYPE", "string", (char*)Rayleight_Fit_TYPE ) ;

//RAYLEIGH_FIT_ANALISYS:
for( int i=0 ; i<glbParam->nBins ; i++ )	evSig->pr2Glued[i] = evSig->pr2[i] ;

	indxMol->endRayFit = glbParam->siteASL + cos(dataMol->zenith*PI/180) * glbParam->r[ indxMol->indxEndMol [0] ] ; // ASL!!!!!!!!!!!

for ( int i=0 ; i<MAX_MOL_RANGES ; i++ ) // for ( int i=0 ; i<1 ; i++ ) // 
{ // FINAL FIT ACROSS MOLECULAR RANGES DETECTED
	if ( indxMol->indxInicMol[i] != 0 ) // IF THERE ARE MOLECULAR RANGES DETECTED...
	{
		fitParam.indxInicFit = indxMol->indxInicMol[i] ; // r 
		fitParam.indxEndFit  = indxMol->indxEndMol [i] ; // fitParam.indxInicFit +1000 ; // glbParam->indxEndSig ;
		fitParam.nFit		 = fitParam.indxEndFit - fitParam.indxInicFit ;

		if ( strcmp(Rayleight_Fit_TYPE, "FACTOR") == 0 )
			RayleighFit_Factor ( (double*)evSig->pr2, (double*)dataMol->pr2Mol, 					  (strcFitParam*)&fitParam, (double*)evSig->pr2Glued ) ;
		else // strcmp(Rayleight_Fit_TYPE, "RMS") == 0 ==> DEFAULT VALUE
			RayleighFit( (double*)evSig->pr2, (double*)dataMol->pr2Mol, dataMol->nBins, "wOutB", "NOTall", (strcFitParam*)&fitParam, (double*)evSig->pr2Glued ) ;

	  //// RAYLEIGH FIT CHECK
			// for( int j=glbParam->indxInitSig ; j<=fitParam.indxInicFit ; j++ )
			// {
			// 	sTest[j] = (evSig->pr2[j] - fitParam.m * dataMol->pr2Mol[j]) ;
		// }
			// 	sum( (double*)sTest, (int)glbParam->indxInitSig, (int)fitParam.indxInicFit, (double*)&sTestSum );
			// if ( sTestSum<0 ) // ( evSig->pr2Glued[fitParam.indxInicFit] > evSig->pr2Glued[fitParam.indxInicFit -1] )
			// {
			// // AGAIN: evSig->pr2Glued[i] = evSig->pr2[i] ACROSS FITTED VALUES
		// 	for( int g=fitParam.indxInicFit ; g<fitParam.indxEndFit ; g++ )		evSig->pr2Glued[g] = evSig->pr2[g] ;
		// 	if ( (indxMol->indxInicMol[i] +10) < (indxMol->indxEndMol[i]-500) )
			// 		{
			// 			indxMol->indxInicMol[i] = indxMol->indxInicMol[i] +10 ;
			// 			goto FIT_AGAIN ;
			// 		}
			// 	else
			// 		goto END_FIT ;
			// }
		//if (indxMol->nMolRanges >= 2)
		//{
			//if (m[1] > m[0])
			//{
				//glbParam->dNFit = glbParam->dNFit +10 ;
				//printf("\n Wrong fit performed before (%lf) and after (%lf) the cloud. Running the analisys again...", m[0], m[1]);
				//goto RAYLEIGH_FIT_ANALISYS ;
			//}
		//}
	}
	//glbParam->indxInitErr = glbParam->indxInitErr +10 ;
//} while ( evSig->pr2Glued[fitParam.indxInicFit] >= evSig->pr2[fitParam.indxInicFit] ) ;
}
// END_FIT:
	for( int i=0 ; i<glbParam->nBins ; i++ )
	{
		evSig->prGlued[i]  = evSig->pr2Glued[i] / pow(glbParam->r[i], 2) ;
		evSig->pr_noBkg[i] = evSig->pr2[i]      / pow(glbParam->r[i], 2) ;
	}
}

// void checkCloudProfiles( strcGlobalParameters *glbParam, strcMolecularData *dataMol, strcIndexMol *indxMol, strcCloudProfiles *cloudProfiles, strcCloudInfoDB *cloudInfoDB )
// {
// 	// int s =0 ;
// 	for( int e =1 ; e <=(glbParam->nEvents -2) ; e++ )
// 	{
// 		for ( int b =0 ; b <glbParam->nBins ; b++)
// 		{
// 			if ( (cloudProfiles[e].clouds_ON[b] == BIN_CLOUD) && ( cloudProfiles[e-1].clouds_ON[b] ==0 ) && ( cloudProfiles[e+1].clouds_ON[b] ==0 ) )
// 				cloudProfiles[e].clouds_ON[b] == (int)0 ;
// 		}
// 		GetCloudLimits( (strcGlobalParameters*)glbParam, (strcMolecularData*)dataMol, (strcCloudProfiles*)&cloudProfiles[e], (strcIndexMol*)indxMol ) ;
// 	}
// 	for ( int b =0 ; b <glbParam->nBins ; b++)
// 	{
// 		cloudProfiles[0].clouds_ON[b] 					= cloudProfiles[1].clouds_ON[b] ;
// 		cloudProfiles[glbParam->nEvents-1].clouds_ON[b] = cloudProfiles[glbParam->nEvents-2].clouds_ON[b] ;
// 	}

// 	GetCloudLimits( (strcGlobalParameters*)glbParam, (strcMolecularData*)dataMol, (strcCloudProfiles*)&cloudProfiles[0]					 , (strcIndexMol*)indxMol ) ;
// 	GetCloudLimits( (strcGlobalParameters*)glbParam, (strcMolecularData*)dataMol, (strcCloudProfiles*)&cloudProfiles[glbParam->nEvents-1], (strcIndexMol*)indxMol ) ;
// }

void fillAugerInfoDB( strcCloudProfiles *cloudProfiles, strcGlobalParameters *glbParam, strcMolecularData *dataMol, strcCloudInfoDB *cloudInfoDB )
{
	if ( cloudProfiles->nClouds >0 )
	{
		cloudInfoDB->cloudCoverage =  cloudInfoDB->cloudCoverage +1;
		// GET THE LOWEST CLOUDS PARAMETER: HEIGHT ABOVE SEA LEVEL
		if ( ( glbParam->siteASL + cloudProfiles->indxInitClouds[0] * dataMol->dzr ) < cloudInfoDB->lowestCloudHeight_ASL )
		{
			cloudInfoDB->lowestCloudHeight_ASL = glbParam->siteASL + cloudProfiles->indxInitClouds[0] * dataMol->dzr ;
			cloudInfoDB->lowestCloudThickness  = (cloudProfiles->indxEndClouds[0] - cloudProfiles->indxInitClouds[0]) *dataMol->dzr ;
			cloudInfoDB->lowestCloud_GPSstart  = glbParam->event_gps_sec[glbParam->event_analyzed] ;
			cloudInfoDB->lowestCloud_GPSend    = cloudInfoDB->lowestCloud_GPSstart ;
		}
		if ( cloudInfoDB->highestCloud_VOD < cloudProfiles->VOD_cloud[0] )
			cloudInfoDB->highestCloud_VOD = cloudProfiles->VOD_cloud[0] ;
	}
	// else if ( cloudInfoDB->lowestCloud_GPSstart ==0 )
	// { // IF THE PROFILE ANALIZED DOESN'T HAVE CLOUDS, SETS THE GPS START/ENT TIME OF THE FILE.
	// 	cloudInfoDB->lowestCloud_GPSstart  = glbParam->event_gps_sec[0] ;
	// 	cloudInfoDB->lowestCloud_GPSend    = glbParam->event_gps_sec[glbParam->nEvents -1] ;
	// }
}

void saveNetCDF_AugerLidarData( char *lidarFileOUT_NetCDF, strcGlobalParameters *glbParam, strcDataToSave *dataToSave )
{
	CNetCDF_Lidar   oNCL = CNetCDF_Lidar() ;

	int  nc_id, retval ;

	if ( ( retval = nc_create( lidarFileOUT_NetCDF, NC_CLOBBER, &nc_id ) ) )
	    ERR(retval);
// DIMENSION DEFINITIONS
    string      dimsName [NDIMS] ;
    int         dimsSize[NDIMS]  ;
    int         dim_ids[NDIMS]   ;
    dimsName[0] = "Events" ;    dimsSize[0] = glbParam->nEventsAVG   ;
    dimsName[1] = "points" ;	dimsSize[1] = glbParam->nBins	     ;
	int  var_ids[NVARS] ;
    string  strNameVars[NVARS] ;
    strNameVars[0].assign("Raw_Lidar_Data");
    strNameVars[1].assign("Height") ;
    strNameVars[2].assign("GPS_Time") ;
    strNameVars[3].assign("Zeniths") ;
    strNameVars[4].assign("Azimuths") ;
    strNameVars[5].assign("Range") ;
    strNameVars[6].assign("RCLS") ;
    strNameVars[7].assign("Alpha_Aer_Cloud_Mask") ;
    strNameVars[8].assign("Photon_Counting_MHz") ;
    strNameVars[9].assign("Mol_RCLS") ;

	// DEFINE VARIABLE Raw_Lidar_Data AND ITS DIMENSIONS (USED FOR THE REST OF VARIABLES)
	oNCL.DefineVarDims(  (int)nc_id, (int)2, (string*)dimsName, (int*)dimsSize, (int*)dim_ids, (char*)strNameVars[0].c_str(), (const char*)"double", (int*)&var_ids[0] ) ;
    oNCL.DefineVariable( (int)nc_id, (char*)strNameVars[1].c_str(), (const char*)"double", (int)2, (int*)&dim_ids[0], (int*)&var_ids[1]  ) ; // Height
    oNCL.DefineVariable( (int)nc_id, (char*)strNameVars[2].c_str(), (const char*)"int"   , (int)1, (int*)&dim_ids[0], (int*)&var_ids[2]  ) ; // GPS_Time
    oNCL.DefineVariable( (int)nc_id, (char*)strNameVars[3].c_str(), (const char*)"double", (int)1, (int*)&dim_ids[0], (int*)&var_ids[3]  ) ; // Zeniths
    oNCL.DefineVariable( (int)nc_id, (char*)strNameVars[4].c_str(), (const char*)"double", (int)1, (int*)&dim_ids[0], (int*)&var_ids[4]  ) ; // Azimuths
    oNCL.DefineVariable( (int)nc_id, (char*)strNameVars[5].c_str(), (const char*)"double", (int)1, (int*)&dim_ids[1], (int*)&var_ids[5]  ) ; // Range
    oNCL.DefineVariable( (int)nc_id, (char*)strNameVars[6].c_str(), (const char*)"double", (int)2, (int*)&dim_ids[0], (int*)&var_ids[6]  ) ; // RCLS
    oNCL.DefineVariable( (int)nc_id, (char*)strNameVars[7].c_str(), (const char*)"double", (int)2, (int*)&dim_ids[0], (int*)&var_ids[7]  ) ; // Alpha_Aer_Cloud_Mask
    oNCL.DefineVariable( (int)nc_id, (char*)strNameVars[8].c_str(), (const char*)"double", (int)2, (int*)&dim_ids[0], (int*)&var_ids[8]  ) ; // Photon_Counting_MHz
    oNCL.DefineVariable( (int)nc_id, (char*)strNameVars[9].c_str(), (const char*)"double", (int)2, (int*)&dim_ids[0], (int*)&var_ids[9]  ) ; // Mol_RCLS

    string strAttListName[10], strAttList[2] ;
    strAttListName[0] = "Site_Name"             ;   strAttList[0].assign(glbParam->site)     ;
    strAttListName[1] = "Log. File"             ;   strAttList[1].assign(glbParam->infoFile) ;
        oNCL.Putt_Bulk_Att_Text( (int)nc_id, (int)NC_GLOBAL, (int)1, (string*)strAttListName, (string*)strAttList ) ;

    double dblAttList[5] ;
    strAttListName[0] = "Altitude_meter_asl"     ;   dblAttList[0] = (double)glbParam->siteASL    ;
    strAttListName[1] = "Numbers_of_Events"      ;   dblAttList[1] = (double)glbParam->nEventsAVG ;
    strAttListName[2] = "Range_Resolution" 		 ;   dblAttList[2] = (double)glbParam->dr		  ;
    // strAttListName[3] = "Range_Resolution"       ;   dblAttList[3] = (double)glbParam->dr         ;
        oNCL.Putt_Bulk_Att_Double( (int)nc_id, (int)NC_GLOBAL, (int)3, (string*)strAttListName, (double*)dblAttList ) ;

				if ( (retval = nc_enddef(nc_id)) )
					ERR(retval);

   // PUTTING VARIABLES
    size_t start[2], count[2];
    start[0] = 0;   count[0] = 1 ; // glbParam.nEventsAVG; 
    start[1] = 0;   count[1] = glbParam->nBins ;
    for( int e=0 ; e <glbParam->nEventsAVG ; e++ )
    {
		start[0] =e ;
		if ( ( retval = nc_put_vara_double((int)nc_id, (int)var_ids[1], start, count, (double*)&dataToSave->zr[e][0] ) ) )
			ERR(retval);
		if ( ( retval = nc_put_vara_double((int)nc_id, (int)var_ids[0], start, count, (double*)&dataToSave->pr[e][0] ) ) )
			ERR(retval);
		if ( ( retval = nc_put_vara_double((int)nc_id, (int)var_ids[6], start, count, (double*)&dataToSave->pr2[e][0] ) ) )
			ERR(retval);
		if ( ( retval = nc_put_vara_double((int)nc_id, (int)var_ids[7], start, count, (double*)&dataToSave->alphaAerCloud[e][0] ) ) )
			ERR(retval);
		if ( ( retval = nc_put_vara_double((int)nc_id, (int)var_ids[8], start, count, (double*)&dataToSave->phMHz[e][0] ) ) )
			ERR(retval);
		if ( ( retval = nc_put_vara_double((int)nc_id, (int)var_ids[9], start, count, (double*)&dataToSave->pr2Mol[e][0] ) ) )
			ERR(retval);
    }

	oNCL.PutVar( (int)nc_id, (int)var_ids[2], (const char*)"int"   , (int*)dataToSave->GPSTime      ) ; 
    oNCL.PutVar( (int)nc_id, (int)var_ids[3], (const char*)"double", (double*)dataToSave->zeniths   ) ;
    oNCL.PutVar( (int)nc_id, (int)var_ids[4], (const char*)"double", (double*)dataToSave->azimuth   ) ;
    oNCL.PutVar( (int)nc_id, (int)var_ids[5], (const char*)"double", (double*)glbParam->r  			) ;

	if ( (retval = nc_close(nc_id)) )
	    ERR(retval);

	printf("\n\n*** SUCCESS writing NetCDF file %s!\n", lidarFileOUT_NetCDF ) ;
}

void save_Auger_to_LPP_NetCDF( char *lidarFileOUT_NetCDF, strcGlobalParameters *glbParam, double ***Raw_Lidar_Data )
{
	CNetCDF_Lidar   oNCL = CNetCDF_Lidar() ;

	int  nc_id, retval ;

    if ( (retval = nc_create( lidarFileOUT_NetCDF, NC_NETCDF4, &nc_id)) )
        ERR(retval);

// DIMENSION DEFINITIONS
    string      dimsName [NDIMS] ;
    int         dimsSize[NDIMS]  ;
    int         dim_ids[NDIMS]   ;
    dimsName[0] = "time"     ;  dimsSize[0] = glbParam->nEvents 		;
    dimsName[1] = "channels" ;  dimsSize[1] = glbParam->nCh 			;
    dimsName[2] = "points"   ;	dimsSize[2] = glbParam->nBins_in_File	;
	int  var_ids[NVARS] ;
    string  strNameVars[NVARS] ;
    strNameVars[0].assign("Raw_Lidar_Data");
    strNameVars[1].assign("Height") ;
    strNameVars[2].assign("Raw_Data_Start_Time") ;
    strNameVars[3].assign("Zeniths") ;
    strNameVars[4].assign("Azimuths") ;
    strNameVars[5].assign("Range") ;
    strNameVars[6].assign("RCLS") ;
    strNameVars[7].assign("Alpha_Aer_Cloud_Mask") ;
    strNameVars[8].assign("Photon_Counting_MHz") ;
    strNameVars[9].assign("Mol_RCLS") ;

	// DEFINE VARIABLE Raw_Lidar_Data AND ITS DIMENSIONS (USED FOR THE REST OF VARIABLES)
	oNCL.DefineVarDims(  (int)nc_id, (int)3, (string*)dimsName, (int*)dimsSize, (int*)dim_ids, (char*)strNameVars[0].c_str(), (const char*)"double", (int*)&var_ids[0] ) ;
    oNCL.DefineVariable( (int)nc_id, (char*)strNameVars[1].c_str(), (const char*)"double", (int)2, (int*)&dim_ids[0], (int*)&var_ids[1]  ) ; // Height
    oNCL.DefineVariable( (int)nc_id, (char*)strNameVars[2].c_str(), (const char*)"int"   , (int)1, (int*)&dim_ids[0], (int*)&var_ids[2]  ) ; // GPS_Time
    oNCL.DefineVariable( (int)nc_id, (char*)strNameVars[3].c_str(), (const char*)"double", (int)1, (int*)&dim_ids[0], (int*)&var_ids[3]  ) ; // Zeniths
    oNCL.DefineVariable( (int)nc_id, (char*)strNameVars[4].c_str(), (const char*)"double", (int)1, (int*)&dim_ids[0], (int*)&var_ids[4]  ) ; // Azimuths
    oNCL.DefineVariable( (int)nc_id, (char*)strNameVars[5].c_str(), (const char*)"double", (int)1, (int*)&dim_ids[1], (int*)&var_ids[5]  ) ; // Range
    oNCL.DefineVariable( (int)nc_id, (char*)strNameVars[6].c_str(), (const char*)"double", (int)2, (int*)&dim_ids[0], (int*)&var_ids[6]  ) ; // RCLS
    oNCL.DefineVariable( (int)nc_id, (char*)strNameVars[7].c_str(), (const char*)"double", (int)2, (int*)&dim_ids[0], (int*)&var_ids[7]  ) ; // Alpha_Aer_Cloud_Mask
    oNCL.DefineVariable( (int)nc_id, (char*)strNameVars[8].c_str(), (const char*)"double", (int)2, (int*)&dim_ids[0], (int*)&var_ids[8]  ) ; // Photon_Counting_MHz
    oNCL.DefineVariable( (int)nc_id, (char*)strNameVars[9].c_str(), (const char*)"double", (int)2, (int*)&dim_ids[0], (int*)&var_ids[9]  ) ; // Mol_RCLS

    string strAttListName[10], strAttList[2] ;
    strAttListName[0] = "Site_Name"             ;   strAttList[0].assign(glbParam->site)     ;
    strAttListName[1] = "Log. File"             ;   strAttList[1].assign(glbParam->infoFile) ;
        oNCL.Putt_Bulk_Att_Text( (int)nc_id, (int)NC_GLOBAL, (int)1, (string*)strAttListName, (string*)strAttList ) ;

    double dblAttList[5] ;
    strAttListName[0] = "Altitude_meter_asl"     ;   dblAttList[0] = (double)glbParam->siteASL    ;
    strAttListName[1] = "Numbers_of_Events"      ;   dblAttList[1] = (double)glbParam->nEventsAVG ;
    strAttListName[2] = "Range_Resolution" 		 ;   dblAttList[2] = (double)glbParam->dr		  ;
    // strAttListName[3] = "Range_Resolution"       ;   dblAttList[3] = (double)glbParam->dr         ;
        oNCL.Putt_Bulk_Att_Double( (int)nc_id, (int)NC_GLOBAL, (int)3, (string*)strAttListName, (double*)dblAttList ) ;

				if ( (retval = nc_enddef(nc_id)) )
					ERR(retval);

   // PUTTING VARIABLES
    size_t start[3], count[3];
    start[0] = 0;   count[0] = 1 ; // glbParam.nEventsAVG; 
    start[1] = 0;   count[1] = 1 ; // glbParam.nCh; 
    start[2] = 0;   count[2] = dimsSize[2] ; // glbParam->nBins_in_File ;
    for( int e=0 ; e <dimsSize[0] ; e++ )
    {
        start[0] =e ;
        for ( int c=0 ; c <dimsSize[1] ; c++ )
        {
            start[1] =c ;
            if ( (retval = nc_put_vara_double((int)nc_id, (int)var_ids[0], start, count, (double*)&Raw_Lidar_Data[e][c][0] ) ) )
                ERR(retval);
        }
    }
    // size_t start[2], count[2];
    // start[0] = 0;   count[0] = 1 ; // glbParam.nEventsAVG; 
    // start[1] = 0;   count[1] = glbParam->nBins ;
    // for( int e=0 ; e <glbParam->nEventsAVG ; e++ )
    // {
	// 	start[0] =e ;
	// 	if ( ( retval = nc_put_vara_double((int)nc_id, (int)var_ids[1], start, count, (double*)&dataToSave->zr[e][0] ) ) )
	// 		ERR(retval);
	// 	if ( ( retval = nc_put_vara_double((int)nc_id, (int)var_ids[0], start, count, (double*)&dataToSave->pr[e][0] ) ) )
	// 		ERR(retval);
	// 	if ( ( retval = nc_put_vara_double((int)nc_id, (int)var_ids[6], start, count, (double*)&dataToSave->pr2[e][0] ) ) )
	// 		ERR(retval);
	// 	if ( ( retval = nc_put_vara_double((int)nc_id, (int)var_ids[7], start, count, (double*)&dataToSave->alphaAerCloud[e][0] ) ) )
	// 		ERR(retval);
	// 	if ( ( retval = nc_put_vara_double((int)nc_id, (int)var_ids[8], start, count, (double*)&dataToSave->phMHz[e][0] ) ) )
	// 		ERR(retval);
	// 	if ( ( retval = nc_put_vara_double((int)nc_id, (int)var_ids[9], start, count, (double*)&dataToSave->pr2Mol[e][0] ) ) )
	// 		ERR(retval);
    // }

	oNCL.PutVar( (int)nc_id, (int)var_ids[2], (const char*)"int"   , (int*)glbParam->event_gps_sec   ) ; 
    // oNCL.PutVar( (int)nc_id, (int)var_ids[3], (const char*)"double", (double*)glbParam->zeniths   ) ;
    // oNCL.PutVar( (int)nc_id, (int)var_ids[4], (const char*)"double", (double*)glbParam->azimuth   ) ;
    // oNCL.PutVar( (int)nc_id, (int)var_ids[5], (const char*)"double", (double*)glbParam->r  			) ;

	if ( (retval = nc_close(nc_id)) )
	    ERR(retval);

	printf("\n\n*** SUCCESS writing NetCDF file %s!\n", lidarFileOUT_NetCDF ) ;
}

void saveCloudsInfoDB( strcGlobalParameters *glbParam, strcCloudInfoDB *cloudInfoDB, strcDataToSave *dataToSave, char *PathFile_OUT_FULL )
{
	char fileDB_out[200] ;
	strcpy( fileDB_out, PathFile_OUT_FULL) ;
	strcat( fileDB_out, ".cloudDB.dat") ;

    FILE 	*fpDB = fopen( fileDB_out, "w+t" ) ;

	int zone_id ;
	if ( strcmp(glbParam->site, "ch") ==0 )
	{
		ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "CH_ZONE" , "int" , (int*)&zone_id ) ;
		// printf("\n CH_ZONE: %d \n", zone_id) ;
	}
	else if ( strcmp(glbParam->site, "ll") ==0 )
	{		
		ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "LL_ZONE" , "int" , (int*)&zone_id ) ;
		// printf("\n LL_ZONE: %d \n", zone_id) ;
	}
	else if ( strcmp(glbParam->site, "lm") ==0 )
	{
		ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "LM_ZONE" , "int" , (int*)&zone_id ) ;
		// printf("\n LM_ZONE: %d \n", zone_id) ;
	}
	else if ( strcmp(glbParam->site, "la") ==0 )
	{
		ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "LA_ZONE" , "int" , (int*)&zone_id ) ;
		// printf("\n LA_ZONE: %d \n", zone_id) ;
	}

	double  cloudCoverage = cloudInfoDB->cloudCoverage / dataToSave->nEventsSaved ;
	double	lowestCloudHeight_ASL 	=999.999 ;
	double 	lowestCloudThickness 	=999.999 ;
	double  lidarMaxHeight = round( glbParam->rEndSig /1000 ) ;

	if ( cloudCoverage >0 )
	{
		lowestCloudHeight_ASL	= cloudInfoDB->lowestCloudHeight_ASL /1000 ;
		lowestCloudThickness 	= cloudInfoDB->lowestCloudThickness  /1000 ;
	}

		 if ( strstr(glbParam->infoFile, "Zenith" ) != NULL )
		 	zone_id = (int)0 ;
	else if ( strstr(glbParam->infoFile, "Azimuth") != NULL )
			zone_id = (int)1 ;

	fprintf(fpDB, "%d %d %d %2.2lf %1.3lf %3.3lf %3.3lf %3.3lf %s\n", zone_id, cloudInfoDB->lowestCloud_GPSstart, cloudInfoDB->lowestCloud_GPSend, 
					lidarMaxHeight, cloudCoverage, lowestCloudHeight_ASL, lowestCloudThickness, cloudInfoDB->highestCloud_VOD, glbParam->fileName ) ;

	printf("\n\n%d %d %d %2.2lf %1.3lf %3.3lf %3.3lf %3.3lf %s\n\n", zone_id, cloudInfoDB->lowestCloud_GPSstart, cloudInfoDB->lowestCloud_GPSend, 
					lidarMaxHeight, cloudCoverage, lowestCloudHeight_ASL, lowestCloudThickness, cloudInfoDB->highestCloud_VOD, glbParam->fileName ) ;

	fclose(fpDB) ;
	printf( "\n\nDone with the cloud data base file: %s \n", fileDB_out ) ;
}

// void readCLFdata( char *clfFile, char *siteIN, strcCLFdata *clfData )
// {
// 	int 	yearFile, monthFile, dayFile ;
// 	char 	strDummy[100] ;

// 	printf("\nclfFile: %s \t siteSearch: %s \n", clfFile, siteIN) ;

// 	char *ptr = strstr(clfFile, "/clf_") ;
// 	sscanf( ptr, "/clf_%4i%02i%02i.dat", &yearFile, &monthFile, &dayFile ) ;

// 	char siteSearch[20] ;
// 	if( strcmp( siteIN, "ll" ) == 0 )
// 		strcpy(siteSearch, "CLFLosLeones") ;
// 	if( strcmp( siteIN, "lm" ) == 0 )
// 		strcpy(siteSearch, "CLFLosMorados") ;
// 	if( strcmp( siteIN, "la" ) == 0 )
// 		strcpy(siteSearch, "CLFLomaAmarilla") ;
// 	if( strcmp( siteIN, "ch" ) == 0 )
// 		strcpy(siteSearch, "CLFCoihueco") ;

// 	FILE	*pFile = fopen( clfFile, "r" ) ;
// 	if ( pFile != NULL )
// 	{
// 		rewind(pFile) ;
// 		//unsigned long pos = 0, initPos = 0 ;
// 		long pos = 0, initPos = 0 ;
// 		clfData->clfPoints = 0 ;

// // COUNT THE NUMBER OF OCCURENCES OF siteSearch IN clfFile.
// 		for ( int i=0 ; i<500 ; i++ )
// 		{
// 			pos = FileSearch( pFile, siteSearch, initPos, 1 ) ;
// 			if ( pos != -5)
// 			{
// 				clfData->clfPoints++ ;
// 				initPos = pos + strlen(siteSearch)+1 ;
// 			}
// 			else
// 				i = 700 ;
// 		}
// 		rewind(pFile) ;
// 	if ( clfData->clfPoints !=0 )
// 	{
// 	//	int 	year, month, day ;
// 		clfData->year 	 = (int*)  malloc( clfData->clfPoints * sizeof(int) )  ; 	memset(clfData->year , 0, (clfData->clfPoints * sizeof(int) ) ) ;
// 		clfData->month 	 = (int*)  malloc( clfData->clfPoints * sizeof(int) )  ; 	memset(clfData->month, 0, (clfData->clfPoints * sizeof(int) ) ) ;
// 		clfData->day 	 = (int*)  malloc( clfData->clfPoints * sizeof(int) )  ; 	memset(clfData->day  , 0, (clfData->clfPoints * sizeof(int) ) ) ;

// 		clfData->hour 	 = (int*)  malloc( clfData->clfPoints * sizeof(int) )  ; 	memset(clfData->hour, 0, (clfData->clfPoints * sizeof(int) ) ) ;
// 		clfData->min	 = (int*)  malloc( clfData->clfPoints * sizeof(int) )  ; 	memset(clfData->min , 0, (clfData->clfPoints * sizeof(int) ) ) ;
// 		clfData->sec	 = (int*)  malloc( clfData->clfPoints * sizeof(int) )  ; 	memset(clfData->sec , 0, (clfData->clfPoints * sizeof(int) ) ) ;
// 		int 	*gpsTime = (int*)  malloc( clfData->clfPoints * sizeof(int) )  ;	memset(gpsTime	    , 0, (clfData->clfPoints * sizeof(int) ) ) ;
// 		char 	*strSite = (char*) malloc( clfData->clfPoints * sizeof(char) ) ;	memset(strSite	    , 0, (clfData->clfPoints * sizeof(char)) ) ;
// 		char	strLine[500] ;	memset( strLine, 0, (500*sizeof(char)) );

// 		double 	rMin, rMax, VAODclf, VAODclfMin, VAODclfMax ;

// 		clfData->VAODht[0] = (double*) malloc( clfData->clfPoints * sizeof(double) ) ; memset( clfData->VAODht[0], 0, sizeof(double) * clfData->clfPoints ) ; // 0 = 1.5 km
// 		clfData->VAODht[1] = (double*) malloc( clfData->clfPoints * sizeof(double) ) ; memset( clfData->VAODht[1], 0, sizeof(double) * clfData->clfPoints ) ; // 1 = 2   km
// 		clfData->VAODht[2] = (double*) malloc( clfData->clfPoints * sizeof(double) ) ; memset( clfData->VAODht[2], 0, sizeof(double) * clfData->clfPoints ) ; // 2 = 3   km
// 		clfData->VAODht[3] = (double*) malloc( clfData->clfPoints * sizeof(double) ) ; memset( clfData->VAODht[3], 0, sizeof(double) * clfData->clfPoints ) ; // 3 = 4   km

// 		clfData->VAODhtMin[0] = (double*) malloc( clfData->clfPoints * sizeof(double) ) ; memset( clfData->VAODhtMin[0], 0, sizeof(double) * clfData->clfPoints ) ; // 0 = 1.5 km
// 		clfData->VAODhtMin[1] = (double*) malloc( clfData->clfPoints * sizeof(double) ) ; memset( clfData->VAODhtMin[1], 0, sizeof(double) * clfData->clfPoints ) ; // 1 = 2   km
// 		clfData->VAODhtMin[2] = (double*) malloc( clfData->clfPoints * sizeof(double) ) ; memset( clfData->VAODhtMin[2], 0, sizeof(double) * clfData->clfPoints ) ; // 2 = 3   km
// 		clfData->VAODhtMin[3] = (double*) malloc( clfData->clfPoints * sizeof(double) ) ; memset( clfData->VAODhtMin[3], 0, sizeof(double) * clfData->clfPoints ) ; // 3 = 4   km

// 		clfData->VAODhtMax[0] = (double*) malloc( clfData->clfPoints * sizeof(double) ) ; memset( clfData->VAODhtMax[0], 0, sizeof(double) * clfData->clfPoints ) ; // 0 = 1.5 km
// 		clfData->VAODhtMax[1] = (double*) malloc( clfData->clfPoints * sizeof(double) ) ; memset( clfData->VAODhtMax[1], 0, sizeof(double) * clfData->clfPoints ) ; // 1 = 2   km
// 		clfData->VAODhtMax[2] = (double*) malloc( clfData->clfPoints * sizeof(double) ) ; memset( clfData->VAODhtMax[2], 0, sizeof(double) * clfData->clfPoints ) ; // 2 = 3   km
// 		clfData->VAODhtMax[3] = (double*) malloc( clfData->clfPoints * sizeof(double) ) ; memset( clfData->VAODhtMax[3], 0, sizeof(double) * clfData->clfPoints ) ; // 3 = 4   km

// 		double 	VAODmeasHeigh[4] ;
// 		ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "VAOD_HEIGH0" , "double" , (double*)&VAODmeasHeigh[0] ) ; // 0 = 1.5 km
// 		ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "VAOD_HEIGH1" , "double" , (double*)&VAODmeasHeigh[1] ) ; // 1 = 2   km
// 		ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "VAOD_HEIGH2" , "double" , (double*)&VAODmeasHeigh[2] ) ; // 2 = 3   km
// 		ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "VAOD_HEIGH3" , "double" , (double*)&VAODmeasHeigh[3] ) ; // 3 = 4   km

// 		pos = 0 ; initPos = 0 ;
// 		for( int t=0 ; t<clfData->clfPoints ; t++ )
// 		{
// 			memset( strDummy, 0, (100*sizeof(char)) ) ;
// // FIND THE SITE
// 			pos = FileSearch( pFile, siteSearch, initPos, 1 ) ;
// 			fseek(pFile, pos, SEEK_SET) ;
// 				fscanf(pFile, "%s %s", strSite, strDummy ) ; //  "  CLFCoihueco  6.10496e+06  457360"

// // FIND THE MEASUREMENT TIME LOCATED *BEFORE* THE SITE NAME
// 			sprintf(strDummy, "### %04i/%02i/%02i/", yearFile, monthFile, dayFile) ;
// 			initPos = pos ;
// 			pos = FileSearch( pFile, strDummy, initPos, -1 ) ; // SEARCH BACKWARD THE MEASUREMENT TIME.
// 			fseek(pFile, pos, SEEK_SET) ; // POINTER LOCATED AT "### 20..."
// 				fscanf( pFile, "### %4d/%02d/%02d/%02d/%02d/%02d %d ### ", &(clfData->year[t]), &(clfData->month[t]), &(clfData->day[t]),
// 																		   &(clfData->hour[t]), &(clfData->min[t]), &(clfData->sec[t]), &gpsTime[t] ) ;
// // FIND THE RANGES TO GET THE VAOD
// 			fgets(strDummy, sizeof(strDummy), pFile) ; // READ THE REST OF THE LINE "  CLFCoihueco  6.10496e+06  457360"
// 			fgets(strLine , sizeof(strLine) , pFile) ; // READ THE LINE OF rMin rMax ... (1712.3 1912.3 ...)
// 		do 	{
// 				sscanf(strLine, "  %lf  %lf  %lf  %lf  %lf  %s\n", &rMin, &rMax, &VAODclf, &VAODclfMin, &VAODclfMax, strDummy ) ;

// 				for ( int h=0 ; h<4 ; h++ )
// 				{
// 					if ( (VAODmeasHeigh[h]>rMin) && (VAODmeasHeigh[h]<rMax) )
// 					{
// 						clfData->VAODht   [h][t] = VAODclf 	 ;
// 						clfData->VAODhtMin[h][t] = VAODclfMin ;
// 						clfData->VAODhtMax[h][t] = VAODclfMax ;
// 					}
// 				}
// 				fgets(strLine, sizeof(strLine), pFile) ;  // READ THE LINE OF rMin  rMax ... (1712.3 1912.3 ...)
// 			} while ( strcmp(strLine, "#\n") != 0 ) ;

// 			pos 	= FileSearch( pFile, siteSearch, initPos, 1 ) ;
// 			initPos = pos + sizeof(siteSearch) ;
// // POINTER pFile READY TO SEARCH THE NEXT ENTRY OF THE SITE.
// 		}
// 	}
// 	else
// 		printf("\n There's no data of %s' in the file %s \n", siteSearch, clfFile) ;

// 	fclose(pFile) ;
// 	}
// 	else
// 	{
// 		printf("\n Error opening the file %s \n", clfFile) ;
// 	}
// }

#endif
