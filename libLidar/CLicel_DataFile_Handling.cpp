
#include "CLicel_DataFile_Handling.h"

CLicel_DataFile_Handling::CLicel_DataFile_Handling()
{

}

CLicel_DataFile_Handling::~CLicel_DataFile_Handling()
{

}


void CLicel_DataFile_Handling::ReadLicel_Time_and_Coord( FILE *fid, strcGlobalParameters *glbParam )
{
	assert( fid !=NULL ) ;

	// char strMisc[50] ;
// LINE 1: File name
	// fscanf( fid, "%s ", strMisc )  ;
	read_Licel_Header_Line( (FILE*)fid, (int)1, (strcGlobalParameters*)glbParam) ;
		// printf( "\nLINE 1: %s\n", glbParam->fileName ) ;

// LINE 2:
	read_Licel_Header_Line( (FILE*)fid, (int)2, (strcGlobalParameters*)glbParam) ;

	// 	//! IMPLEMENT THIS: IF THE FILE IS LICEL TYPE, SET THE TEMP AND PRESSURE FROM THE CONFIGURATION FILE.
	// 	//! SAVE THIS ARRAYS IN DATA LEVEL 0 
		// ReadAnalisysParameter( (const char*) glbParam->FILE_PARAMETERS, "Temperature_at_Lidar_Station_K", "double", (double*)&glbParam->temp_K_agl[glbParam->evSel] ) ;
		// ReadAnalisysParameter( (const char*) glbParam->FILE_PARAMETERS, "Pressure_at_Lidar_Station_Pa"   , "double", (double*)&glbParam->pres_Pa_agl[glbParam->evSel]     ) ;
		glbParam->temp_K_agl[glbParam->evSel]  = (double)-999.0 ;
		glbParam->pres_Pa_agl[glbParam->evSel] = (double)-999.0 ;

	// StartDate = 05/08/2020
	strncpy( &glbParam->StartDate[2], &glbParam->StartDate[3], 7 ) ; // 05/08/2020 --> 0508/2020
	strncpy( &glbParam->StartDate[4], &glbParam->StartDate[5], 4 ) ; // 0508/2020  --> 05082020  
	// StoptDate = 05/08/2020
    strncpy( &glbParam->StopDate[2], &glbParam->StopDate[3], 7 ) ; // 05/08/2020 --> 0508/2020 
	strncpy( &glbParam->StopDate[4], &glbParam->StopDate[5], 4 ) ; // 0508/2020  --> 05082020 

	// StartTime = 00:00:30
	strncpy( (char*)&glbParam->StartTime[2], (char*)&glbParam->StartTime[3], 5 ) ; // 00:00:30 --> 0000:30
	strncpy( (char*)&glbParam->StartTime[4], (char*)&glbParam->StartTime[5], 2 ) ; // 0000:30  --> 000030
	// StoptDate = 00:00:40
	strncpy( (char*)&glbParam->StopTime[2], (char*)&glbParam->StopTime[3], 5 ) ; // 00:00:30 --> 0000:30
	strncpy( (char*)&glbParam->StopTime[4], (char*)&glbParam->StopTime[5], 2 ) ; // 0000:30  --> 000030 

		// printf("\nReadLicel_Time_and_Coord()") ;
		// printf("\n glbParam->site: %s	lidarHeaderData.StartD: %s	lidarHeaderData.StartT: %s	\nlidarHeaderData.EndD: %s	lidarHeaderData.EndT: %s\n",
		//  		glbParam->site, glbParam->StartDate, glbParam->StartTime, glbParam->StopDate, glbParam->StopTime ) ;
}

void CLicel_DataFile_Handling::read_Licel_Header_Line( FILE *fid, int header_line_number, strcGlobalParameters *glbParam )
{
	nLineElements =0 ;

	memset( strLine    , 0, sizeof(strLine    ) ) ;
	memset( strLine_bkp, 0, sizeof(strLine_bkp) ) ;

	fgets(strLine, sizeof(strLine), fid) ;
	indx_EL = strcspn(strLine, "\r\n") ; // indx_EL: index END LINE 
	if (indx_EL >= 0) 
		sprintf(&strLine[indx_EL], " \r\n") ; // FORMAT THE STRING WITH CARRIAGE RETURN AND LINE FEED
	sprintf( strLine_bkp, "%s", strLine ) ;

	token = (char*) strtok(strLine, " ") ; // token POINTING TO THE FIRST ELEMENT OF THE LINE 

	switch ( header_line_number )
	{
		case 1:
				sprintf( glbParam->fileName, "%s", token ) ;
				break;

		case 2 : // LINE NUMBER 2
				if ( strcmp( token, "Sao" ) ==0 )
				{
					strcpy( glbParam->siteName, "Sao Paul" ) ;
					token = (char*)strtok(NULL, " ") ; // GET THE POINTER TO THE NEXT WORD
				}
				else
					strcpy( glbParam->siteName, token ) ;

				nLineElements++ ;
				token = (char*)strtok(NULL, " ") ; // GET THE POINTER TO THE NEXT WORD
 
				// COUNT THE NUMBER OF ELEMENTS IN THE LINE
				while ( strcmp(token, "\r\n") !=0 )
				{
					nLineElements++ ;
					token = strtok(NULL, " ") ; // GET THE POINTER TO THE NEXT WORD
				}

				char strDump_1[20], strDump_2[20] ;
				if ( nLineElements ==9 ) // OLD LICEL FORMAT (IE. SAO PAULO, ARGENTINA)
				{	// LAST VALUE= ZENITH
					sprintf( glbParam->inputDataFileFormat, "LICEL_FILE_OLD" ) ;
					if ( strcmp( glbParam->siteName, "Sao Paul" ) ==0 )
					{   // ONLY FOR SAO PAULO
						sscanf( strLine_bkp, "%s %s %s %s %s %s %04d %lf %lf %lf ", strDump_1, strDump_2, glbParam->StartDate, 
											glbParam->StartTime, glbParam->StopDate, glbParam->StopTime, &glbParam->siteASL, &glbParam->siteLat, 
											&glbParam->siteLong, &glbParam->aZenith[glbParam->evSel] ) ;
					}
					else
						sscanf( strLine_bkp, "%s %s %s %s %s %04d %lf %lf %lf ", strDump_1, glbParam->StartDate, glbParam->StartTime, glbParam->StopDate, 
											glbParam->StopTime, &glbParam->siteASL, &glbParam->siteLat, &glbParam->siteLong, &glbParam->aZenith[glbParam->evSel] ) ;
				}
				else if ( nLineElements ==10 ) // LICEL NEW (IE. GRANADA/MEDELLIN)
				{	// LAST VALUES= ZENITH AZIMUTH
					sprintf( glbParam->inputDataFileFormat, "LICEL_FILE_NEW" ) ;
					sscanf( strLine_bkp, "%s %s %s %s %s %04d %lf %lf %lf %lf ", strDump_1, glbParam->StartDate, glbParam->StartTime, 
											glbParam->StopDate, glbParam->StopTime, &glbParam->siteASL, &glbParam->siteLat, &glbParam->siteLong, 
											&glbParam->aZenith[glbParam->evSel], &glbParam->aAzimuth[0] ) ;

					// if ( strcmp( glbParam->siteName, "Medellin" ) ==0 )
					// 	glbParam->aZenith[glbParam->evSel] = glbParam->aZenith[glbParam->evSel] -90 ;
				}
				else if ( nLineElements ==11 ) // LICEL NEW (IE. GRANADA/MEDELLIN) WITH INFO STRING AT THE END OF LINE 2 BETWEEN INVERTED COMMAS
				{	// LAST VALUES= ZENITH AZIMUTH
					sprintf( glbParam->inputDataFileFormat, "LICEL_FILE_NEW" ) ;
					sscanf( strLine_bkp, "%s %s %s %s %s %04d %lf %lf %lf %lf %s", strDump_1, glbParam->StartDate, glbParam->StartTime, 
											glbParam->StopDate, glbParam->StopTime, &glbParam->siteASL, &glbParam->siteLat, &glbParam->siteLong, 
											&glbParam->aZenith[glbParam->evSel], &glbParam->aAzimuth[0], strDump_2 ) ;

					if ( strcmp( glbParam->siteName, "Medellin" ) ==0 )
						glbParam->aZenith[glbParam->evSel] = glbParam->aZenith[glbParam->evSel] -90 ;
				}
				else if( nLineElements ==12 ) // RAYMETRIC
				{	// LAST VALUES: ZENITH AZIMUTH TEMPERATURE PRESSURE
					sprintf( glbParam->inputDataFileFormat, "LICEL_FILE_RAYMETRICS" ) ;
						sscanf( strLine_bkp, "%s %s %s %s %s %04d %lf %lf %lf %lf %lf %lf", glbParam->siteName, glbParam->StartDate, 
												glbParam->StartTime, glbParam->StopDate, glbParam->StopTime, &glbParam->siteASL, &glbParam->siteLat,
												&glbParam->siteLong, &glbParam->aZenith[glbParam->evSel], &glbParam->aAzimuth[0], &glbParam->temp_K_agl[0], 
												&glbParam->pres_Pa_agl[0] ) ;
				}
				else
				{	// WRONG NUMBER OF ELEMENTS IN LINE 2
					printf("\n void read_Licel_Header_Line()--> file %s is not compatible \nNumber of elements (%d) in line number 2... exit\n\n", glbParam->fileName, nLineElements ) ;
					exit(1) ;
				}
				glbParam->aZenithAVG [glbParam->evSel] = glbParam->aZenith [glbParam->evSel] ;
				glbParam->aAzimuthAVG[glbParam->evSel] = glbParam->aAzimuth[glbParam->evSel] ;
				break ;

		case 3 : // LINE NUMBER 3
				if ( ( strcmp( glbParam->inputDataFileFormat, "LICEL_FILE_OLD" ) ==0 ) || ( strcmp( glbParam->inputDataFileFormat, "LICEL_FILE_RAYMETRICS" ) ==0 ) ) 
				{	// TWO LASER PARAMETERS AND NUMBER OF CHANNELS
					sscanf( strLine_bkp, " %07d %d %07d %d %02d", &glbParam->Accum_Pulses[0], &glbParam->Laser_Frec[0], &glbParam->Accum_Pulses[1]
																, &glbParam->Laser_Frec[1]  , &glbParam->nCh ) ;
				}
				else if ( strcmp( glbParam->inputDataFileFormat, "LICEL_FILE_NEW" ) ==0 ) // LICEL NEW (IE. GRANADA/MEDELLIN)
				{	// TREE LASER PARAMETERS, NUMBER OF CHANNELS, THIRD LASER PARAMETER AND 0000000 0000
					sscanf( strLine_bkp, " %07d %d %07d %d %02d %07d %d 0000000 0000", &glbParam->Accum_Pulses[0], &glbParam->Laser_Frec[0], 
														 				  			   &glbParam->Accum_Pulses[1], &glbParam->Laser_Frec[1], &glbParam->nCh, 
																					   &glbParam->Accum_Pulses[2], &glbParam->Laser_Frec[2] ) ;
				}
		break ;

		case 4:	// DATASET
				char cAct ;
				for ( int c=0 ; c<glbParam->nCh ; c++ )
				{
					while ( strcmp(token, "\r\n") !=0 ) // SEARCH FOR THE END OF LINE
					{
						nLineElements++ ;
						token = strtok(NULL, " ") ;
					}
					if ( ( strcmp( glbParam->inputDataFileFormat, "LICEL_FILE_OLD" ) ==0 ) ) // OLD LICEL FORMAT (IE. SAO PAULO, ARGENTINA)  || ( strcmp( glbParam->inputDataFileFormat, "LICEL_FILE_RAYMETRICS" ) ==0 )
					{ // OLD LICEL DATAFILE 
						if ( (strncmp(&strLine_bkp[3], "0", 1) ==0) || (strncmp(&strLine_bkp[3], "1", 1) ==0) )
						{ // THE OLD DATAFILE ONLY CONTAIN DATASET TYPE EQUAL TO 0 (ANALOG) OR 1 (PHOTONCOUNTING)
							sscanf( strLine_bkp, " %c %d %d %05d %1d %04d %lf %05d.%s 0 0 %02d 000 %2d %06d %lf %s", 
									&cAct, &glbParam->DAQ_Type[c], &glbParam->Laser_Src[c], &glbParam->nBins_Ch[c], &glbParam->iPol[c], &glbParam->PMT_Voltage[c], &glbParam->dr, &glbParam->iLambda[c],
									&glbParam->sPol[c],/*0 0*/ &glbParam->bin_shift_whole[c], /*000*/ &glbParam->iADCbits[c], &glbParam->nShots[c], &glbParam->iMax_mVLic[c], &glbParam->sChDescription[c][0] ) ;

	// printf( "\n\nread_licel_line() --> %c %d %d %d 1 %d %1.2lf %05d.%c 0 0 %02d 000 %2d %06d %0.3lf \n", cAct, /*2*/glbParam->DAQ_Type[c], glbParam->Laser_Src[c], 
	// 		glbParam->nBins_Ch[c], /*1*/ /*6*/glbParam->PMT_Voltage[c], glbParam->dr, glbParam->iLambda[c], glbParam->sPol[c], /*0 0*/ glbParam->bin_shift_whole[c], /*000*/
	// 		glbParam->iADCbits[c], glbParam->nShots[c], glbParam->iMax_mVLic[c] ) ;
						}
					}
					else
					{ // NEW LICEL DATASET
						if ( (strncmp(&strLine_bkp[3], "0", 1) ==0) || (strncmp(&strLine_bkp[3], "1", 1) ==0) || (strncmp(&strLine_bkp[3], "2", 1) ==0) )
						{ 	// AN, PHO or STD
							if ( nLineElements == 16 )
							{ 	// DATASET *DOES NOT* CONTAIN EXTRA STRING INFORMATION
								sscanf( strLine_bkp, " %c %d %d %d %d %04d %lf %05d.%s 0 0 %02d %03d %2d %06d %lf %s ", /*%s*/
									&cAct, &glbParam->DAQ_Type[c], &glbParam->Laser_Src[c], &glbParam->nBins_Ch[c], &glbParam->iPol[c], &glbParam->PMT_Voltage[c],
									&glbParam->dr, &glbParam->iLambda[c], &glbParam->sPol[c], /*0 0*/ &glbParam->bin_shift_whole[c], &glbParam->bin_shift_decimal[c],
									&glbParam->iADCbits[c], &glbParam->nShots[c], &glbParam->iMax_mVLic[c], &glbParam->sChDescription[c][0]  ) ;
							}
							else
							{ 	// DATASET *DOES* CONTAIN EXTRA STRING INFORMATION
								sscanf( strLine_bkp, " %c %d %d %d %d %04d %lf %05d.%s 0 0 %02d %03d %2d %06d %lf %s %99[^\n] ",
									&cAct, &glbParam->DAQ_Type[c], &glbParam->Laser_Src[c], &glbParam->nBins_Ch[c], &glbParam->iPol[c], &glbParam->PMT_Voltage[c],
									&glbParam->dr, &glbParam->iLambda[c], &glbParam->sPol[c], /*0 0*/ &glbParam->bin_shift_whole[c], &glbParam->bin_shift_decimal[c],
									&glbParam->iADCbits[c], &glbParam->nShots[c], &glbParam->iMax_mVLic[c], &glbParam->sChDescription[c][0], &glbParam->sChInformation[c][0] ) ;
							}
// printf("\n%c %d %d %05d %d %04d %1.2lf %05d.%c 0 0 %02d %03d %2d %06d %lf %s %s", cAct, glbParam->DAQ_Type[c], glbParam->Laser_Src[c], glbParam->nBins_Ch[c], 
// 						glbParam->iPol[c], glbParam->PMT_Voltage[c], glbParam->dr, glbParam->iLambda[c], glbParam->sPol[c], /*0 0*/ glbParam->bin_shift_whole[c],
// 						glbParam->bin_shift_decimal[c], glbParam->iADCbits[c], glbParam->nShots[c], glbParam->iMax_mVLic[c], glbParam->sChDescription[c], glbParam->sChInformation[c] ) ;
						}
						else if ( (strncmp(&strLine_bkp[3], "3", 1) ==0) || (strncmp(&strLine_bkp[3], "4", 1) ==0) )
						{ // "Power Meter" OR "Overflow dataset" THEN DO NOT LOAD glbParam->dz
							if ( nLineElements == 16 )
							{ 	// DATASET *DOES NOT* CONTAIN EXTRA STRING INFORMATION
								sscanf( strLine_bkp, " %c %d %d %d %d %04d %lf %05d.%s 0 0 %02d %03d %2d %06d %lf %s",
									&cAct, &glbParam->DAQ_Type[c], &glbParam->Laser_Src[c], &glbParam->nBins_Ch[c], &glbParam->iPol[c], 
									&glbParam->PMT_Voltage[c], &lidarHeaderData.Dz, &glbParam->iLambda[c], &glbParam->sPol[c], 
									/*0 0*/ &glbParam->bin_shift_whole[c], &glbParam->bin_shift_decimal[c], &glbParam->iADCbits[c], &glbParam->nShots[c],
									&glbParam->iMax_mVLic[c], &glbParam->sChDescription[c][0] ) ;
							}
							else
							{ 	// DATASET *DOES* CONTAIN EXTRA STRING INFORMATION
								sscanf( strLine_bkp, " %c %d %d %d %d %04d %lf %05d.%s 0 0 %02d %03d %2d %06d %lf %s %99[^\n]", 
									&cAct, &glbParam->DAQ_Type[c], &glbParam->Laser_Src[c], &glbParam->nBins_Ch[c], &glbParam->iPol[c], 
									&glbParam->PMT_Voltage[c], &lidarHeaderData.Dz, &glbParam->iLambda[c], &glbParam->sPol[c], 
									/*0 0*/ &glbParam->bin_shift_whole[c], &glbParam->bin_shift_decimal[c], &glbParam->iADCbits[c], &glbParam->nShots[c],
									&glbParam->iMax_mVLic[c], &glbParam->sChDescription[c][0], glbParam->sChInformation[c] ) ;
							}
// printf("\n%c %d %d %05d %d %04d %1.2lf %05d.%c 0 0 %02d %03d %2d %06d %lf %s %s", cAct, glbParam->DAQ_Type[c], glbParam->Laser_Src[c], glbParam->nBins_Ch[c], 
// 						glbParam->iPol[c], glbParam->PMT_Voltage[c], lidarHeaderData.Dz, glbParam->iLambda[c], glbParam->sPol[c], /*0 0*/ glbParam->bin_shift_whole[c],
// 						glbParam->bin_shift_decimal[c], glbParam->iADCbits[c], glbParam->nShots[c], glbParam->iMax_mVLic[c], glbParam->sChDescription[c], glbParam->sChInformation[c] ) ;
						// glbParam->indxOffset[c] = lidarHeaderData.bin_shift_whole ;
						}
					}

					if ( (glbParam->DAQ_Type[c] ==0) || (glbParam->DAQ_Type[c] ==2) ) // glbParam->DAQ_Type[c] --> 0: ANALOG	2:ERROR
					{
						glbParam->nAnCh++   ; // CONTADOR DE CANALES ANALOGICOS
						glbParam->iMax_mVLic[c] = glbParam->iMax_mVLic[c] *1000 ;
					}
					else if ( glbParam->DAQ_Type[c] ==1 )
					 	glbParam->nPhotCh++ ; // CONTADOR DE CANALES DE FOTOCONTEO

					fgets(strLine, sizeof(strLine), fid) ;
					indx_EL = strcspn(strLine, "\r\n") ;
					if (indx_EL >= 0) 
						sprintf(&strLine[indx_EL], " \r\n") ;
					sprintf( strLine_bkp, "%s", strLine ) ;
					
					token = (char*) strtok(strLine, " ") ;
				} // for ( int c=0 ; c<glbParam->nCh ; c++ )

				glbParam->nLambda   = glbParam->nCh;
				glbParam->nBinsRaw  = *max_element( glbParam->nBins_Ch, glbParam->nBins_Ch +glbParam->nCh ) ;
				glbParam->nBins		= glbParam->nBinsRaw ;
				glbParam->nBins_in_File = glbParam->nBinsRaw ;
		break ; // case 4:	// DATASET
	} // switch ( header_line_number )
} // void CLicel_DataFile_Handling::read_Licel_Header_Line( FILE *fid, int header_line_number, strcGlobalParameters *glbParam )

void CLicel_DataFile_Handling::ReadLicel_GlobalParameters( char *lidarFile, strcGlobalParameters *glbParam )
{
	fid = fopen(lidarFile, "r");
	if ( fid == NULL )
		printf("\n Failed at opening the file %s \n", lidarFile) ;
	assert( fid !=NULL ) ;

	glbParam->nAnCh   = 0 ;
	glbParam->nPhotCh = 0 ;

	glbParam->aAzimuth 	   		= (double*) new double[glbParam->nEvents] 	 ; memset( (double*)glbParam->aAzimuth    	 , 0, (sizeof(double)*glbParam->nEvents) 	) ;
	glbParam->aAzimuthAVG  		= (double*) new double[glbParam->nEventsAVG] ; memset( (double*)glbParam->aAzimuthAVG	 , 0, (sizeof(double)*glbParam->nEventsAVG) ) ;	
	glbParam->aZenith      		= (double*) new double[glbParam->nEvents] 	 ; memset( (double*)glbParam->aZenith     	 , 0, (sizeof(double)*glbParam->nEvents) 	) ;
	glbParam->aZenithAVG   		= (double*) new double[glbParam->nEventsAVG] ; memset( (double*)glbParam->aZenithAVG     , 0, (sizeof(double)*glbParam->nEventsAVG) ) ;
	glbParam->temp_K_agl 		= (double*) new double[glbParam->nEvents] 	 ; memset( (double*)glbParam->temp_K_agl	 , 0, (sizeof(double)*glbParam->nEvents) 	) ;
	glbParam->pres_Pa_agl   	= (double*) new double[glbParam->nEvents] 	 ; memset( (double*)glbParam->pres_Pa_agl    , 0, (sizeof(double)*glbParam->nEvents) 	) ;
	glbParam->temp_K_agl_AVG 	= (double*) new double[glbParam->nEventsAVG] ; memset( (double*)glbParam->temp_K_agl_AVG , 0, (sizeof(double)*glbParam->nEventsAVG) ) ;
	glbParam->pres_Pa_agl_AVG   = (double*) new double[glbParam->nEventsAVG] ; memset( (double*)glbParam->pres_Pa_agl_AVG, 0, (sizeof(double)*glbParam->nEventsAVG) ) ;
	// glbParam->indxEndSig_ev   	= (int*   ) new int   [glbParam->nEventsAVG] ;

	if ( strncmp( glbParam->inputDataFileFormat, "LICEL_FILE", 10 ) ==0 )
	{
		// HEADER LINE 1: File name
		// fscanf( fid, "%s ", glbParam->fileName ) ;
		read_Licel_Header_Line( (FILE*)fid, (int)1, (strcGlobalParameters*)glbParam) ;
			// printf( "\nLINE 1: %s\n", glbParam->fileName ) ;

		// HEADER LINE 2: INITALIZED AT 1 SO WE HAVE THE SAME CONVENTION OF THE TEXT EDITORS
		read_Licel_Header_Line( (FILE*)fid, (int)2, (strcGlobalParameters*)glbParam) ;
			// printf( "\nLINE 2: %s %10s %s %s %s %04d %lf %lf %lf \n", glbParam->siteName, glbParam->StartDate, glbParam->StartTime, glbParam->StopDate, glbParam->StopTime,
			// 					  	   		 				          glbParam->siteASL, glbParam->siteLat, glbParam->siteLong, glbParam->aZenith[0] ) ;

		// HEADER LINE 3: INITALIZED AT 1 SO WE HAVE THE SAME CONVENTION OF THE EDITORS
		read_Licel_Header_Line( (FILE*)fid, (int)3, (strcGlobalParameters*)glbParam) ;
			// printf( "\nLINE 3: %07d %d %07d %d %02d %07d %d 0000000 0000",  glbParam->Accum_Pulses[0], glbParam->Laser_Frec[0], 
			// 																glbParam->Accum_Pulses[1], glbParam->Laser_Frec[1], glbParam->nCh, 
			// 																glbParam->Accum_Pulses[2], glbParam->Laser_Frec[2] ) ;

		// HEADER LINE >=4 --> DATASETS
		glbParam->DAQ_Type    		= (int*)    new int 	 [ glbParam->nCh ] ;
		glbParam->Laser_Src   		= (int*)    new int 	 [ glbParam->nCh ] ;
		glbParam->nBins_Ch    		= (int*)    new int 	 [ glbParam->nCh ] ;
		glbParam->PMT_Voltage 		= (int*)    new int 	 [ glbParam->nCh ] ;
		glbParam->iLambda     		= (int*)    new int 	 [ glbParam->nCh ] ;
		glbParam->iADCbits    		= (int*)    new int 	 [ glbParam->nCh ] ;
		glbParam->nShots	  		= (int*)	new int 	 [ glbParam->nCh ] ;
		glbParam->indxOffset  		= (int*) 	new int 	 [ glbParam->nCh ] ; memset( (int*)glbParam->indxOffset       , 0, (sizeof(int)*glbParam->nCh) ) ;
		glbParam->bin_shift_whole 	= (int*) 	new int 	 [ glbParam->nCh ] ; memset( (int*)glbParam->bin_shift_whole  , 0, (sizeof(int)*glbParam->nCh) ) ;
		glbParam->bin_shift_decimal = (int*) 	new int 	 [ glbParam->nCh ] ; memset( (int*)glbParam->bin_shift_decimal, 0, (sizeof(int)*glbParam->nCh) ) ;
		glbParam->sPol        		= (char*)   new char	 [ glbParam->nCh ] ;
		glbParam->iPol        		= (int*)    new int 	 [ glbParam->nCh ] ;
		glbParam->iMax_mVLic  		= (double*) new double   [ glbParam->nCh ] ;

		glbParam->sChDescription = (char**) new char*[glbParam->nCh] ;
		glbParam->sChInformation = (char**) new char*[glbParam->nCh] ;
		for ( int c=0 ; c <glbParam->nCh ; c++ )
		{
				glbParam->sChDescription[c] = (char*) new char[10] ;
				glbParam->sChInformation[c] = (char*) new char[100];
		}

		read_Licel_Header_Line( (FILE*)fid, (int)4, (strcGlobalParameters*)glbParam) ;
		// for ( int c=0 ; c<glbParam->nCh ; c++ )
		// {
		// 	printf( "\n 1 %d %d %05d %d %04d %1.2lf %05d.%c 0 0 %02d 000 %02d %06d %1.3lf", glbParam->DAQ_Type[c], 
		// 			glbParam->Laser_Src[c], glbParam->nBins_Ch[c], glbParam->iPol[c], glbParam->PMT_Voltage[c], glbParam->dr, glbParam->iLambda[c], glbParam->sPol[c],
		// 			 /*0 0*/ glbParam->bin_shift_whole[c], /* 000 */glbParam->iADCbits[c], glbParam->nShots[c], glbParam->iMax_mVLic[c] ) ;
		// }
	} // if ( strncmp( glbParam->inputDataFileFormat, "LICEL_FILE", 10 ) ==0 )

	if ( fclose(fid) != 0 )
		printf("\n Failed to close the lidar file.\n\n") ;
}

void CLicel_DataFile_Handling::ReadLicel_Data( char *lidarFile, strcGlobalParameters *glbParam, strcLidarDataFile *dataFile )
{
	// FILE *fid = fopen(lidarFile, "r");
	fid = fopen(lidarFile, "r");
	if ( fid == NULL )
		printf("\n Failed at opening the file %s \n", lidarFile) ;

	ReadLicel_Time_and_Coord( (FILE*)fid, (strcGlobalParameters*)glbParam ) ;
	fseek( (FILE*)fid, 0, SEEK_SET ) ;

	char strLine[500] ;
	for ( int l = 0; l < (3+glbParam->nCh+1) ; l++)	// READ TILL THE LIDAR DATA. +1 IS FOR THE EMPTY LINE BETWEEN THE HEADER AND THE LIDAR VECTORS
			fgets( strLine, sizeof(strLine), fid ) ;

	// long int prev_offset = ftell(fid) ;
	for ( int c=0 ; c<glbParam->nCh ; c++ )
	{
		fread( (int*)&dataFile->db_ADC[c][0], sizeof(int), glbParam->nBins_Ch[c], fid ) ;
			// if ( glbParam->evSel ==0 )
			// {
			// 	printf("\nch %d --> after read %d bins --> offset: %ld --> offset diff: %ld --> bins diff: %ld", 
			// 				c, glbParam->nBins_Ch[c], ftell(fid), ftell(fid)-prev_offset, (ftell(fid)-prev_offset)/4 ) ;
			// 	prev_offset = ftell(fid) ;
			// }
		fseek( fid, 2, SEEK_CUR ) ;

			// SIN USO
			// int cAn, cPh ;
			// cAn =0 ; cPh =0 ;
			// if ( glbParam->DAQ_Type[c] ==0 ) // ANALOG
			// {
			// 	glbParam->ScaleFactor_Analog = ( glbParam->iMax_mVLic[c] / pow(2, glbParam->iADCbits[c]) ) ;
			// 	for( int b=0 ; b<glbParam->nBinsRaw ; b++ )
			// 		dataFile->db_mV[cAn][b] = (double) ( dataFile->db_ADC[c][b] * glbParam->ScaleFactor_Analog ) ;
			// 	cAn++ ;
			// }
			// else if ( glbParam->DAQ_Type[c] ==1 ) // PHOTONCOUNTING
			// {
			// 	glbParam->ScaleFactor_Dig = (double) 1 ;
			// 	for( int b=0 ; b<glbParam->nBinsRaw ; b++ )
			// 		dataFile->db_CountsMHz[cPh][b] = (double) ( dataFile->db_ADC[c][b] * glbParam->ScaleFactor_Dig ) ;
			// 	cPh++ ;
			// }
	} // for ( int c=0 ; c<glbParam->nCh ; c++ )

	if ( fclose(fid) != 0 )
		printf("\n Failed to close the lidar file.\n\n") ;
} // FIN DEL ReadLidarData()

int CLicel_DataFile_Handling::Read_Bkg_Data_Files( char *path_to_bkg_files, strcGlobalParameters *glbParam , double **data_Bkg )
{
    struct stat path_to_bkg_files_stat ;
    stat( path_to_bkg_files, &path_to_bkg_files_stat ) ;

  	if ( S_ISDIR(path_to_bkg_files_stat.st_mode) )
	{ // A *FOLDER* WAS PASSED AS AN ARGUMENT
		int     nFilesInInputFolder =0 ;
		DIR *d;
		struct dirent *dir;
		// GET THE NUMBER OF FILES IN THE INPUT FOLDER
		d = opendir( path_to_bkg_files ) ;
		while ( (dir = readdir(d)) != NULL )
		{ // GET THE NUMBER OF FILES INSIDE THE FOLDER
			if ( ( dir->d_type == DT_REG ) && ( strcmp(dir->d_name, "..") !=0 ) && ( strcmp(dir->d_name, ".") !=0 ) && ( strcmp(dir->d_name, "log.txt") !=0 ) && ( strcmp(dir->d_name, "temp.dat") !=0 ) )
				nFilesInInputFolder++;
		}

		if ( nFilesInInputFolder ==0 )
		{
			printf("\n There are not Licel files in the folder (see %s) \nBye...", glbParam->FILE_PARAMETERS ) ;
			return -1 ;
		}
		rewinddir(d) ;
		
		string *bkg_files = (string*) new string[nFilesInInputFolder] ;

		int f=0 ;
		printf("\n\tNumber of background files to average: %d\n", nFilesInInputFolder) ;
		while ( (dir = readdir(d)) != NULL )
		{ // GET THE BACKGROUND FILES NAMES
			if ( ( dir->d_type == DT_REG ) && ( strcmp(dir->d_name, "..") !=0 ) && ( strcmp(dir->d_name, ".") !=0 ) &&
				( strcmp(dir->d_name, "log.txt") !=0 ) && ( strcmp(dir->d_name, "temp.dat") !=0 ) )
			{
				bkg_files[f].assign(path_to_bkg_files) ;
				bkg_files[f].append(dir->d_name) ;
				// printf( "\n\tbkg_files[%d]= %s", f, bkg_files[f].c_str() ) ;
				f++ ;
			}
		}

		strcGlobalParameters glbParam_bkg ;
		sprintf( glbParam_bkg.inputDataFileFormat, "%s", glbParam->inputDataFileFormat ) ;
		sprintf( glbParam_bkg.siteName			 , "%s", glbParam->siteName ) ;
		glbParam_bkg.nEvents	= (int)nFilesInInputFolder ;
		glbParam_bkg.nEventsAVG = (int)1 ; // glbParam->nEventsAVG ;
			ReadLicel_GlobalParameters( (char*)bkg_files[0].c_str(), (strcGlobalParameters*)&glbParam_bkg ) ;

		f=0 ;
		if ( ( glbParam_bkg.nCh == glbParam->nCh) && ( glbParam_bkg.nBins >= glbParam->nBins )  )
		{
			strcLidarDataFile	*dataFile    = (strcLidarDataFile*) new strcLidarDataFile[ glbParam_bkg.nEvents ] ;
			GetMem_DataFile( (strcLidarDataFile*)dataFile, (strcGlobalParameters*)&glbParam_bkg ) ;
			
			for ( f=0 ; f <nFilesInInputFolder ; f++ )
			{
				// printf("\nbkg_files[%d].c_str() = %s", f, bkg_files[f].c_str() ) ;
				glbParam->evSel = f;
				ReadLicel_Data ( (char*)bkg_files[f].c_str(), (strcGlobalParameters*)&glbParam_bkg, (strcLidarDataFile*)&dataFile[f] ) ;

				for (int c=0 ; c <glbParam_bkg.nCh ; c++)
				{
					for (int i=0 ; i <glbParam->nBins; i++) // IT MUST BE glbParam, NOT glbParam_bkg
						data_Bkg[c][i] = data_Bkg[c][i] + dataFile[f].db_ADC[c][i] ;
				}
			}

			for ( int c =0; c <glbParam_bkg.nCh ; c++)
			{
				for (int i =0; i <glbParam->nBins; i++) // IT MUST BE glbParam, NOT glbParam_bkg
					data_Bkg[c][i] = data_Bkg[c][i] /nFilesInInputFolder ;
			}
			if ( glbParam_bkg.nBins > glbParam->nBins )
			{
				printf("\n Background files contain %d bins \n Lidar files contain %d \n Background signals are truncated to %d bins.\n\n", glbParam_bkg.nBins, glbParam->nBins, glbParam->nBins ) ;
			}
		}
		else
		{
			printf("\n *** Background files with different number of channels and/or bins. Not saved in the NetCDF file ***\n") ;
			printf(" *** Number of channels in the background files: %d ***\n", glbParam_bkg.nCh 	) ;
			printf(" *** Number of channels in the lidar files: %d ***\n", glbParam->nCh 			) ;
			printf(" *** Number of bins in the background files: %d ***\n", glbParam_bkg.nBins		) ;
			printf(" *** Number of bins in the lidar files: %d ***\n", glbParam->nBins				) ;
			return -10 ;
		}
    }
	else
	{
		printf("\nDark-Current files: A folder must be set containing the dark-current files in the configuration file.\n") ;
		printf("%s\n", path_to_bkg_files) ;
		return -1 ;
	}
	return 0 ;
}

void CLicel_DataFile_Handling::check_Lidar_Files_Consistency( strcGlobalParameters *glbParam, char **inputFilesInTime )
{

}
