#include <TApplication.h>
#include "TCanvas.h"
#include "TMultiGraph.h"
#include "TGraph.h"
#include "TMatrix.h"
#include "TROOT.h"
#include "TF1.h"
#include "TH1.h"
#include "TF1.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TSystem.h"
#include "TROOT.h"
#include <TImage.h>

#include "../libLidar/TLEvent.h"
#include "../libLidar/LPower.h"

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <cstdlib>
#include <iostream>

#include "../libLidar/libLidar.h"

int main( int argc, char *argv[] )
{
	char lidarFile[100], lidarFileOUT[100], modeRun[10] ;
	sprintf( lidarFile		, "%s", argv[1] ) ;
	sprintf( FILE_PARAMETERS, "%s", argv[2] ) ;

	sprintf( modeRun, "%s" , argv[3] ) ; // IF BULK MODE, DON'T RUN theApp.Run() AT THE END

	TApplication theApp("App", &argc, argv);

	printf("\nNewFileNewFileNewFileNewFileNewFileNewFileNewFileNewFileNewFileNewFileNewFileNewFileNewFileNewFileNewFileNewFileNewFileNewFile\n") ;

// ROOT FILE READOUT ////////////////////////////////////////////////////////////////////////////////////
	strcGlobalParameters	glbParam ;
	strcLidarDataFile		*dataFile ;
	Int_t		 			i    	 ;
	if (	(strstr(lidarFile, "lidar-") !=NULL  )
		&& 	(strstr(lidarFile, ".root" ) !=NULL  )
		&&	(strstr(lidarFile, ".root_") == NULL ) )
	{
		ReadGlobalParamROOT( lidarFile, &glbParam) ;
		dataFile = (strcLidarDataFile*) new strcLidarDataFile[glbParam.nEvents] ;
		ReadLidarROOT( lidarFile, dataFile, &glbParam ) ; // dataFile[nEvents] --> HAS ALL EVENTS OF THE FILE.
	}
	else
	{
		printf("\n\t\t NOT A ROOT LIDAR SIGNAL \n") ;
		gApplication->Terminate(0) ;
	}

	sprintf( lidarFileOUT, "%s%s", lidarFile, "_OUT_TAM.txt" ) ;
	FILE 	*fp = fopen( lidarFileOUT, "w" ) ;
	printf("\n lidarFileOUT: %s \n", lidarFileOUT) ;
	fprintf(fp, "\n%s\n%s\t\t Events: %i \n", lidarFile, glbParam.infoFile, glbParam.nEvents) ; // HEADER
	fprintf(fp, "\n___________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________\n") ;

// GENERAL VARIABLES GENERATOR ////////////////////////////////////////////////////////////////////////////////////

// RANGE VECTOR
	glbParam.r = (double*) new double[ glbParam.nBins ] ;
	for( i=0 ; i<glbParam.nBins ; i++ )
		glbParam.r[i] 	 = (double) (i+1) * glbParam.dr ;

// ANALYZE ONLY DISCRETE SCANS //////////////////////////////////////////////////////////////////////////
if ( (strstr( glbParam.infoFile, "Horizontal Shot" ) != NULL) || (strstr( glbParam.infoFile, "10 sec" ) != NULL) )
	gApplication->Terminate(0);
else if ( (strstr( glbParam.infoFile, "Zenith"  ) != NULL) && (strstr( glbParam.infoFile, "Discrete"   ) != NULL) )
{
	// EVENT INFO
		int 	ev[2] ;
		double 	zenEv[2] ;
		char 	radFile[100] ;
		sprintf( radFile, "./atmospheres/DB-%02d.dat", glbParam.month) ;

	double thetaDiff[3] ;
	readAnalisysParameter( FILE_PARAMETERS, "thetaDiff1", "double" , (double*)&thetaDiff[0] ) ;
	readAnalisysParameter( FILE_PARAMETERS, "thetaDiff2", "double" , (double*)&thetaDiff[1] ) ;
	readAnalisysParameter( FILE_PARAMETERS, "thetaDiff3", "double" , (double*)&thetaDiff[2] ) ;
	strcTheta	theta ; // LOAD THE THETA TO ANALIZE.

// MOLECULAR DATA INFO. INDEX=0/1 -->DATA       INDEX=2 --> ZENITH=0
	strcMolecularData	dataMol[3] ;
	strcAerosolData		dataAer ;
	strcLidarSignal		evSig[2] ;
	//strcCloudData		cloudData[2] ;
// MEMORY ALLOCATION
	for ( int e=0 ; e<3 ; e++ )
	{ // e=0, 1 --> DATA SET       e=2 --> VERTICAL DATA (IT DOESN'T MATTER IF ONE OF THE EVENTS SELECTED ARE IN THE VERTICAL))
		dataMol[e].nBins  	 = glbParam.nBins ;
		dataMol[e].zr		 = (double*) malloc( glbParam.nBins * sizeof(double) )  ;
		dataMol[e].betaMol   = (double*) malloc( glbParam.nBins * sizeof(double) )  ;
		dataMol[e].betaRam   = (double*) malloc( glbParam.nBins * sizeof(double) )  ;
		dataMol[e].alphaMol	 = (double*) malloc( glbParam.nBins * sizeof(double) ) 	;
		dataMol[e].nMol		 = (double*) malloc( glbParam.nBins * sizeof(double) ) 	;
		dataMol[e].prMol	 = (double*) malloc( glbParam.nBins * sizeof(double) ) 	;
		dataMol[e].pr2Mol	 = (double*) malloc( glbParam.nBins * sizeof(double) ) 	;

		if ( e < 2 )
		{ // e=0, 1 --> DATA SET
			evSig  [e].pr 	 	= (double*) malloc( glbParam.nBins * sizeof(double) ) ;
			evSig  [e].pr_noBkg = (double*) malloc( glbParam.nBins * sizeof(double) ) ;
			evSig  [e].pr2		= (double*) malloc( glbParam.nBins * sizeof(double) ) ;
			evSig  [e].prFit	= (double*) malloc( glbParam.nBins * sizeof(double) ) ;
			evSig  [e].pr2Fit	= (double*) malloc( glbParam.nBins * sizeof(double) ) ;
			evSig  [e].prGlued	= (double*) malloc( glbParam.nBins * sizeof(double) ) ;
			evSig  [e].pr2Glued = (double*) malloc( glbParam.nBins * sizeof(double) ) ;

			//cloudData[e].clouds_ON 	 	= (double*) new double  [glbParam.nBins] ;
			//cloudData[e].indxInitClouds = (int*)    new int 	[NMAXCLOUDS] ;
			//cloudData[e].indxEndClouds  = (int*)    new int 	[NMAXCLOUDS] ;	
		}
	}

// LOOP FOR DIFFERENTS DATA SET BASED ON thetaRef AND thetaDiff
// LOOP THOUGH THE DIFFERENTS ANGLES VS ZENITH=0
for( int a=0 ; a<3 ; a++ )
{
	printf("\nNewEventNewEventNewEventNewEventNewEventNewEventNewEventNewEventNewEventNewEventNewEventNewEventNewEventNewEventNewEventNewEvent\n") ;
		theta.thetaRef  = 0 ;				// EVENT NUMBER STORED IN ev[0]
		theta.thetaDiff = thetaDiff[a] ;	// EVENT NUMBER STORED IN ev[1]

// RETRIEVE THE EVENT NUMBER (ev[]) AND THE ZENITHAL ANGLES (zenEv[ev[]) FOR THE ZENITHAL EVENTS LOADED IN theta STRUCTURE.
		chooseEventNumber( &glbParam, &theta, ev, zenEv ) ;
			printf( "\n\t ev[0]: %d \t zen[0]: %f \n\t ev[1]: %d \t zen[1]: %f \n", ev[0], zenEv[0], ev[1], zenEv[1] ) ;

	char	used_channel[20] ;
	readAnalisysParameter( FILE_PARAMETERS, "used_channel", "string" , (char*)used_channel ) ;

	int ch = selectChannel( &glbParam, used_channel ) ;

// LOAD THE SELECTED LIDAR SIGNALS OF THE SELECTED EVENTS, AND ZERO BIN CORRECTION
	for ( int e=0 ; e<2 ; e++ )
	{
		for ( int i=0 ; i < glbParam.nBins ; i++ )
		{
			evSig[e].pr[i] = (double)dataFile[ev[e]].db_mV[ch][i+glbParam.indxOffset] ;
			//CloudDetection( (double*)evSig[e].pr, (strcGlobalParameters*)&glbParam, (strcCloudData*)&cloudData[e] ) ;
		}
	}

// MOLECULAR DATA FOR EACH EVENT SELECTED
			for ( int e=0 ; e<3 ; e++ )
			{
				if ( e==2 ) // INDEX = 2 --> VERTICAL DATA
					dataMol[e].zenith = 0 ;
				else
					dataMol[e].zenith = zenEv[e] ; // [º]

				molData( radFile, &glbParam, &dataMol[e] ) ;
			}

// FOR SinteticLidarSignal() USE bkgSubstractionFit()
//		SinteticLidarSignal( (strcGlobalParameters*)&glbParam, (strcMolecularData*)&dataMol, (strcAerosolData*)&dataAer, (strcLidarSignal*)&evSig ) ;
		ProcessTAMInversion( (strcGlobalParameters*)&glbParam, (strcMolecularData*)&dataMol, (strcAerosolData*)&dataAer, (strcLidarSignal*)&evSig ) ;

fprintf(fp, "\n\t ev[0]: %d \t zen[0]: %f \n\t ev[1]: %d \t zen[1]: %f \n", ev[0], zenEv[0], ev[1], zenEv[1] ) ;
fprintf(fp, "\n------------------------------------------------------------------------------------------------------------------------\n");
fprintf(fp, "\t\t TAM-VAOD: %.2f (@ %lf) \n", dataAer.VAOD, dataAer.ablHeigh) ;
fprintf(fp, "------------------------------------------------------------------------------------------------------------------------\n") ;

// PLOT
	TCanvas 		*cPr			= (TCanvas*)     new TCanvas("cPr","Senal lidar", 200, 10, 700, 500) ;
	TMultiGraph 	*multiGrphPr	= (TMultiGraph*) new TMultiGraph ;
		MultiPlot( (double*)evSig[0].pr2, dataMol[0].zr, glbParam.nBins, (strcMolecularData*)&dataMol[0], (strcGlobalParameters*)&glbParam, multiGrphPr, kBlack, "P(r)", "Heigh ASL [m]", "P(r)" ) ;
		MultiPlot( (double*)evSig[1].pr2, dataMol[1].zr, glbParam.nBins, (strcMolecularData*)&dataMol[1], (strcGlobalParameters*)&glbParam, multiGrphPr, kRed  , "P(r)", "Heigh ASL [m]", "P(r)" ) ;

		MultiPlot( (double*)evSig[0].pr2Glued, dataMol[0].zr, glbParam.nBins, (strcMolecularData*)&dataMol[0], (strcGlobalParameters*)&glbParam, multiGrphPr, kBlue , "Rayleigh-Fit", "Heigh [m]", "Rayleigh-Fit" ) ;
		MultiPlot( (double*)evSig[1].pr2Glued, dataMol[1].zr, glbParam.nBins, (strcMolecularData*)&dataMol[1], (strcGlobalParameters*)&glbParam, multiGrphPr, kGreen, "Rayleigh-Fit", "Heigh [m]", "Rayleigh-Fit" ) ;

	char	imgFileName[200] ;
	TImage *imgcSigOK = TImage::Create() ;
		imgcSigOK->FromPad(cPr) ;
		sprintf( imgFileName, "%s_SigTAM_Ev_%ivs%i.png", lidarFile, ev[0], ev[1] ) ;
		imgcSigOK->WriteImage( imgFileName ) ;
		free(imgcSigOK) ;

} // for( int a=0 ; a<3 ; a++ )
//________________________________________________________________________________________________________________________
} // else if ( (strstr( glbParam.infoFile, "Zenith"  ) != NULL) && (strstr( glbParam.infoFile, "Discrete"   ) != NULL) )
	fclose(fp) ;

	if ( strcmp(modeRun, "bulk") != 0 )
		theApp.Run() ;
	gApplication->Terminate(0);
	return 0 ;
}
