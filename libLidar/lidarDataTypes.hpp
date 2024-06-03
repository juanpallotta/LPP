#ifndef LIDARDATATYPES_H
#define LIDARDATATYPES_H

#include <cstddef>
#include <string>

#define MAX_CH_LICEL	100 // == MAX CHANNELS
#define MAX_CH 			3   // == MAX_TEL
#define MAX_EV 			150
#define MOL_BIN_GAP		200
#define MOL_ON			1
#define PI 				3.14159265
#define KM 				3/(8*PI)
#define EVNT_OK				100
#define EVNT_WRONG			200
#define EVNT_WRONG_USHOOT	300
#define BIN_CLOUD			1
#define VAOD_ERR			0
#define VAOD_OK				0X0F
// #define PHO_MAX_COUNT_MHz	250    // [MHz] PHOTONCOUNTING MAXIMUN COUNT RATE 
#define MAX_MOL_RANGES		51
#define NMAXCLOUDS 			10 		// MAX NUMBER OF CLOUDS TO BE DETECTED.

/*
tyspedef struct{
    int active; // active (1) non-active (0)
    int photons; // photons (1) analog (0)
    int elastic; // elastic (1) raman (2)
    int ndata; // number of data points
    int pmtv; // pmt high voltage (V)
    float binw; // bin width (m)    
    int wlen; // wavelength (nm)
    single pol; // polarization (o, p, s)
    int bits; // adc bits (for analog channels)
    int nshoots; 
    float discr;// voltage range or discriminator level
    char tr[3];
    bin *raw; // raw data in original data format
    float *phy; // physical data converted to 4-byte float
} channel;

typedef struct{
    char file[80]; // file name as written in header
    char site[80]; // site name 
    RM_Date start; // date
    RM_Date end; // date
    int alt; // altitude (m)
    float lon; // longitute (deg)
    float lat; // latitude (deg)
    int zen; // zenith (deg)
    int idum;
    float T0; // reference surface temperature
    float P0; // reference surface pressure
    int nshoots; // number of shoots
    int nhz; // repetition rate (Hz)
    int nshoots2; // number of shoots
    int nhz2; // repetition rate (Hz)
    int nch; // number of channels
    channel *ch;
} RMDataFile;
*/

struct strcGlobalParameters
{
	char 	FILE_PARAMETERS[500] 		; //! MAKE IT string CLASS!!!!!
	// char 	Measurement_Att_File[500] 	;
	char    inputDataFileFormat[50]	;
	char    exeFile[50]				;
	char    outputDataFileFormat[50];
	char 	BkgCorrMethod[10]    	;
	double 	*r	 					;
	double 	*r_avg 					;
	int		*avg_Points_Fernald		;
	double 	dr 						;
	double 	dzr						;
	int 	nChMax 					;
	int 	nCh 					;
	int 	*indx_gluing_Low_AN		;
	int 	*indx_gluing_High_PHO	;
	int 	nPair_Ch_to_Glue		; // = lenght(indx_gluing_Low_AN) = = lenght(indx_gluing_High_PHO)
	double  MIN_TOGGLE_RATE_MHZ		;
	double  MAX_TOGGLE_RATE_MHZ 	;
	double  PHO_MAX_COUNT_MHz    	; // [MHz] PHOTONCOUNTING MAXIMUN COUNT RATE 
    int     chSel               	;
	int 	*nBins_Ch				; // int 	nBins_Ch[MAX_CH_LICEL]	;
	int 	nBinsRaw				;
	int 	nBins_in_File			;
	int 	nBinsBkg 				;
	int 	nBins					;
	double 	rInitSig				;
	double 	rEndSig					;
	int 	indxInitSig				;
	int 	indxEndSig				;
	double 	*rEndSig_ev				;
	int 	*indxEndSig_ev			;
	int 	indxInitErr				;
	int 	indxInitInversion   	;
	double 	tBin_us					;
	int 	*nShots				 	;
	int		lambda					;
	int		*iLambda			  	; //	int		iLambda[MAX_CH_LICEL]  	;
	int		indxWL_PDL1				;
	int		indxWL_PDL2				;
	int 	nLambda					;
	char	*sPol			    	; // char	sPol[MAX_CH_LICEL]    	;
	int		*iPol			    	; // char	sPol[MAX_CH_LICEL]    	;
	int 	*DAQ_Type				; // int 	DAQ_Type[MAX_CH_LICEL]   ;
	double 	*iMax_mVLic				; // double 	iMax_mVLic[MAX_CH_LICEL];
	int 	*iADCbits 				; // int 	iADCbits[MAX_CH_LICEL] 	;
	int 	*PMT_Voltage   			;  // int 	PMT_Voltage[MAX_CH_LICEL];
	int 	nAnCh 					;
	int 	nPhotCh 				;
	double 	ScaleFactor_Analog 		;
	double 	ScaleFactor_Dig 		;
    int     evSel  		 			;
	double 	*aZenith 				;
	double 	*aZenithAVG 			;
	double 	*aAzimuth 				;
	double 	*aAzimuthAVG			;
	int 	nEvents 				;
	int 	nEventsAVG 				;
	int		numEventsToAvg			;
	int		numEventsToAvg_PDL1		;
	int		numEventsToAvg_PDL2		;
	int 	*indxOffset				;
	int 	*bin_shift_whole		;
	int		*bin_shift_decimal		;
	char	fileName[200] 			;
	char	infoFile[100] 			;
	char 	site[20] 				;
	int 	Laser_Frec[3] 			;
	int		Accum_Pulses[3]			;
	int		*Laser_Src				; // int		Laser_Src[MAX_CH_LICEL]	;
	char	scanType[5]				;
	double 	siteASL 				;
	double 	siteLat 				;
	double 	siteLong 				;
	char 	siteName[10]			;
	double  *temp_K_agl				;
	double  *pres_Pa_agl			;
	double  *temp_K_agl_AVG			;
	double  *pres_Pa_agl_AVG		;
	char	StartDate[20]			;
	char	StartTime[20]			;
	char	StopDate[20]			;
	char	StopTime[20]			;
	char	**sChDescription		;
	char	**sChInformation		;
	float	Time_Zone				;
    long    *event_gps_sec  		; // GPS TIME
	time_t	start_time_secs			; // UNIX TIME
	time_t	stop_time_secs			; // UNIX TIME
	int 	year 					;
	int 	month 					;
	int 	day 					;
	int 	hour 					;
	int 	min 					;
	int 	sec 					;
	int 	run 					;
	bool	is_Noise_Data_Loaded =false ;
	bool	is_Ovlp_Data_Loaded  =false ;

	int 	MonteCarlo_N_SigSet_Err	;

	double 	drFit					;
	int 	dNFit					;
    double  ka                  	;
    double  LR                  	;
} ;

struct strcMolecularData
{ // IN HIGH RESOLUTION
	double 	*betaMol   		=NULL 	;
	double 	*betaMol_avg  	=NULL 	;
	double 	*betaRam   		=NULL 	;
	double 	*alphaMol  		=NULL	;
	double 	*alphaMol_avg 	=NULL	;
	double  *nMol 	   		=NULL	;
	double  *prMol	   		=NULL 	;
	double  *prMol_avg 		=NULL 	;
	double  *pr2Mol	   		=NULL 	;
	double  *pr2Mol_avg		=NULL 	;
	double 	*zr		   		=NULL	;
	double 	*z_asl     		=NULL	;
	double 	*MOD	   		=NULL	;
	int 	nBins   	 			;
	double  *pPa	   		=NULL	; // PRESSURE [Pa] FROM THE MODEL/RADIOSOUNDING TO BE SAVED IN THE NETCDF FILE
	double  *tK 	   		=NULL	; // TEMPERATURE [K] THE MODEL/RADIOSOUNDING TO BE SAVED IN THE NETCDF FILE
	double 	zenith					;
	double 	dzr		  				;
	double  MOD_REF					;
	double  LR_mol					;
} ;

struct strcLidarDataFile
{
	int 	**db_ADC 	 	; // [channel_number][bin_Number]
	double 	**db_mV  	 	; // [channel_number][bin_Number]
	double 	**db_mV_TEST 	; // [channel_number][bin_Number]
	double 	**db_CountsMHz  ; // [channel_number][bin_Number]
} ;

struct strcLidarSignalSet
{ // FOR TAM METHOD
	double 	*pr		 [2] ;
	double 	*pr_noBkg[2] ;
	double 	*pr2	 [2] ;
	double 	*prFit	 [2] ;
	double 	*pr2Fit	 [2] ;
	double 	*prGlued [2] ;
	double 	*pr2Glued[2] ;
} ;

struct strcLidarSignal
{
	double 	*pr		  =NULL	;
	double 	*pr_noBkg =NULL	;
	double 	*pr_noBias=NULL	;
	double 	*pr2	  =NULL	;
	double 	*prFit	  =NULL	;
	double 	*pr2Fit	  =NULL	;
	double 	*prGlued  =NULL	;
	double 	*pr2Glued =NULL	;
	// double 	*phCounts =NULL	;
} ;

struct strcDataToSave
{
	int		nEventsSaved			;
	double 	**zr	   		=NULL	;
	double 	**pr	   		=NULL	;
	// double 	**prFit 		=NULL	;
	double 	**pr2	 		=NULL	;
	double 	**pr2Mol 		=NULL	;
	double 	**phMHz 		=NULL	;
	double 	**alphaAerCloud =NULL	;
	double 	*zeniths 		=NULL	;
	double 	*azimuth 		=NULL	;
	int 	*GPSTime 		=NULL	;
	double 	VAOD_TAM				;
	double 	*VAOD_Fernald	=NULL	;
	double 	*LR				=NULL	;
} ;

struct strcFernaldInversion
{
	bool	created =false 	;
	double 	*pr2n     		;
	double 	*prn     		;
	double 	*phi    		;
	double 	*p 	    		;
	double 	*ip	    		;
	double 	*ipN    		;
	double 	*betaT  		;
	double 	*pr2Fit 		;
	double 	*prFit	 		;
	double 	ipNref  		;
	double 	intAlphaMol_Ref ;
	double 	*intAlphaMol_r  ;
} ;

struct strcAerosolData
{
	double 	*betaAer  =NULL	;
	double 	*alphaAer =NULL	;
	double 	*VAODr 	  =NULL	;
	double 	SAOD 			;

	double 	aVAOD[4]		;
	double 	VAOD 			;
	double 	VAOD_REF		;
} ;

struct strcCloudProfiles
{
	int 	*clouds_ON 		;
	int 	*indxInitClouds	;
	int 	*indxEndClouds 	;
	int 	nClouds			;
	int 	nRangesMol		;
	double  *test_1			;
	double  *test_2			;
	double 	*VOD_cloud		;
} ;

struct strcCloudInfoDB
{
	double  cloudCoverage			;
	double	lowestCloudHeight_ASL 	;
	double 	lowestCloudThickness  	;
	double 	highestCloud_VOD       	;
	long 	lowestCloud_GPSstart  	;
	long 	lowestCloud_GPSend    	;
} ;

struct strcCloudInfoDB_LPP
{
	int 	*nClouds			  	;
	double	*lowestCloudHeight_ASL 	;
	double 	*lowestCloudThickness  	;
	double 	*lowestCloud_VOD       	;
	int 	*cloudTime			  	;
} ;

struct strcCLFdata
{
	double 	*VAODht    	  [4] ;
	double 	*VAODhtMin	  [4] ;
	double 	*VAODhtMax	  [4] ;
	double 	*VAODmeasHeigh[4] ;
	int 	*year			  ;
	int 	*month			  ;
	int 	*day			  ;
	int 	*hour			  ;
	int 	*min 			  ;
	int 	*sec 			  ;
	int 	clfPoints		  ;
} ;

struct strcLidarDataOut
{
	double 	*VAODht    	  [4] ;
	double 	*VAODhtErr	  [4] ;
//	double 	*VAODhtMax	  [4] ;
	double 	*VAODmeasHeigh[4] ;
	int 	*year			  ;
	int 	*month			  ;
	int 	*day			  ;
	int 	*hour			  ;
	int 	*min 			  ;
	int 	*sec 			  ;
	int 	lidarPoints		  ;
} ;

struct strcErrorSignalSet
{
	double 	**pr_smooth		  	 ;
	double 	**prNoisy		   	 ;
	double 	**pr2Noisy		  	 ;

	double 	**alphaAer_ErrSet ;
	double 	*alphaAer_meanErrSet ;
	double 	*alphaAer_stdErrSet ;

	double 	**VAODr_ErrSet	   	 ;
	double 	*VAODr_meanErrSet 	 ;
	double 	*VAODr_stdErrSet  	 ;

	double 	hVAOD_meanErrSet[4]	 ;
	double 	hVAOD_stdErrSet [4]	 ;
} ;

struct strcRadioSoundingData
{
    // bool    radSoundingDataLOADED = false ;
	double  *zLR 	;
	double	*pLR 	;
	double 	*tLR 	;
	double 	*nLR 	;
	double 	*nHR 	;
	double 	*alpha_mol ;
	double 	*beta_mol  ;
	int 	nBinsLR ;
} ;

struct strcFitParam
{ // FITTING DATA USED IN FUNCTIONS WHERE RAYLEIGH-FIT IS APPLIED.
	int 	indxInicFit  ;
	int 	indxEndFit   ;
	int 	nFit		 ;
// PARAMETERS OF THE FIT TO REDUCE THE RMS ERROR
	double 	m ;
	double  b ;
	double  f ;
	double  squared_sum_fit ;
	double  var     ;
	double  std     ;
	double  R2	    ;

	//INTERMEDIATE VARIABLES FOR R2
	double s  				   ;
	double mean_sig 		   ;
	double squared_sum_sig_vs_Mean ;

	// double  cov00    ;
	// double 	cov01   ;
	// double 	cov11   ;
} ;

struct strcIndexMol
{
	int		*indxMolON 	 ; // size(indxMolON) = nBins   1->MOL   0->AER
	int		nMolRanges 	 ; // CONTAIN THE NUMBER OF MOLECULAR RANGES DETECTED.
	int 	*indxInicMol ;
	int 	*indxEndMol  ;
	double 	*errRMS		 ;
    int     indxRef      ;
	double 	ablHeight    ;
	double 	endRayFit	 ;
} ;

struct strcIndxLimToAvg
{
	double 	zenithClusterEvnt	;
	int		indxEvntInit 	    ;
	int		indxEvntEnd  	    ;
} ;

struct strcTheta
{
	double 	thetaRef  ;
	double 	thetaDiff ;
} ;


struct strcLicelDataFile
{
	char Name[17] 	 ; 	// File name
	char Station[15] ;  // Station name
	char StartD[15]  ; 	// Start date
	char StartT[15]	 ;	// Start time
	char EndD[15] 	 ;	// End date
	char EndT[15] 	 ;	// End time
	int  Alt 		 ;	// Altitude
	double  Lat 	 ;	// Latitude
	double  Long	 ;	// Longitude
	float  Zen 		 ;	// Zenith angle
	int  Shots1 	 ;	// Number of shots
	int  LaserRate1  ;  // Read 3 fields
	int  Shots2		 ;  // Number of shots
	int  LaserRate2	 ;  // Read 3 fields
	int  nCh		 ;	// Number of channels

	char 	sAct[MAX_CH_LICEL][2]	 ;
	int 	DAQ_Type[MAX_CH_LICEL]   ;
	char 	sLasSrc[MAX_CH_LICEL][2] ;  //! TRANSFORMARLO EN INT
	int 	nBins ;
	int 	iPMTv[MAX_CH_LICEL];
	char 	sDump1[20] ;
	char 	sDump2[20] ;
	double 	Dz ;
	int		iLambda[MAX_CH_LICEL]  ;
	char 	sPol[MAX_CH_LICEL][2]  ;
	int 	iPol[MAX_CH_LICEL]	   ;
	int		bin_shift_whole 	   ;
	int		bin_shift_decimal 	   ;
 	int 	iADCbits[MAX_CH_LICEL] ;
	int	 	nShots ;
	float 	iMaxVLic[MAX_CH_LICEL] ;
	char 	sDescrp[MAX_CH_LICEL][5] ;
	char 	sInfoChannel[MAX_CH_LICEL][100] ;


	// int			*dbADC[MAX_CH] ; // long int	*dbADC[MAX_CH] ; // 
	// int			*dbPHO[MAX_CH] ; // long int	*dbPHO[MAX_CH] ; // 
	// double 		*db_mV[MAX_CH] ;
	double 		ScaleFactor_Cont ;
	double 		ScaleFactor_Dig ;
} ;

#endif
