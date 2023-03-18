
#include "CLicel_DataFile_Handling.h"

CLicel_DataFile_Handling::CLicel_DataFile_Handling()
{

}

CLicel_DataFile_Handling::~CLicel_DataFile_Handling()
{

}

void CLicel_DataFile_Handling::ReadLicelTime_and_Coord( FILE *fid, strcGlobalParameters *glbParam )
{
	assert( fid !=NULL ) ;

	char strMisc[50] ;

// LINE 1: File name
	fscanf( fid, "%s ", strMisc )  ;
// LINE 2:
	if ( strcmp( glbParam->inputDataFileFormat, "LICEL_FILE" ) ==0 )
	{
		fscanf( fid, "%s ", strMisc ) ;
		if ( strcmp( strMisc, "Sao" ) ==0 )
		{
			sprintf( glbParam->site, "Sao Paul" ) ;
			fscanf( fid, "%s %10s %08s %10s %08s %lf %lf %lf %lf ", 
			strMisc, glbParam->StartDate, glbParam->StartTime, glbParam->StopDate, glbParam->StopTime, 
			&glbParam->siteASL, &glbParam->siteLat, &glbParam->siteLong, &glbParam->aZenith[glbParam->evSel] ) ;
		}
		else
		{
			sprintf( glbParam->site, "%s", strMisc ) ;
			fscanf( fid, "%10s %08s %10s %08s %lf %lf %lf %lf ",
					glbParam->StartDate, glbParam->StartTime, glbParam->StopDate, glbParam->StopTime, 
					&glbParam->siteASL, &glbParam->siteLat, &glbParam->siteLong, &glbParam->aZenith[glbParam->evSel] ) ;
			// fscanf( fid, "%s %10s %08s %10s %08s %lf %lf %lf %lf ", 
			// glbParam->site, glbParam->StartDate, glbParam->StartTime, glbParam->StopDate, glbParam->StopTime, 
			// &glbParam->siteASL, &glbParam->siteLat, &glbParam->siteLong, &glbParam->aZenith[glbParam->evSel] ) ;
		}
		// printf("\nReadLicelTime_and_Coord()") ;
		// printf("\n glbParam->site: %s	lidarHeaderData.StartD: %s	lidarHeaderData.StartT: %s	lidarHeaderData.EndD: %s	lidarHeaderData.EndT: %s\n",
		//  		glbParam->site, glbParam->StartDate, glbParam->StartTime, glbParam->StopDate, glbParam->StopTime ) ;
		glbParam->aAzimuth[glbParam->evSel] 	 = (double)0 ;
		//! IMPLEMENT THIS: IF THE FILE IS LICEL TYPE, SET THE TEMP AND PRESSURE FROM THE CONFIGURATION FILE.
		//! SAVE THIS ARRAYS IN DATA LEVEL 0 
		// ReadAnalisysParameter( (const char*) glbParam->FILE_PARAMETERS, "Temperature_at_Lidar_Station_K", "double", (double*)&glbParam->temp_K_agl[glbParam->evSel] ) ;
		// ReadAnalisysParameter( (const char*) glbParam->FILE_PARAMETERS, "Pressure_at_Lidar_Station_Pa"   , "double", (double*)&glbParam->pres_Pa_agl[glbParam->evSel]     ) ;
		glbParam->temp_K_agl[glbParam->evSel] = (double)-999.0 ;
		glbParam->pres_Pa_agl[glbParam->evSel]     = (double)-999.0 ;
	}
	else if ( strcmp(glbParam->inputDataFileFormat, "RAYMETRIC_FILE") ==0 )
	{
		fscanf( fid, "%s %10s %08s %10s %08s %lf %lf %lf %lf %lf %lf %lf ", 
		glbParam->site, glbParam->StartDate, glbParam->StartTime, glbParam->StopDate, glbParam->StopTime, 
		&glbParam->siteASL, &glbParam->siteLat, &glbParam->siteLong, &glbParam->aZenith[glbParam->evSel], 
		&glbParam->aAzimuth[glbParam->evSel], &glbParam->temp_K_agl[glbParam->evSel], &glbParam->pres_Pa_agl[glbParam->evSel] ) ;
	}
		// printf("\nReadLicelTime_and_Coord()") ;
		// printf("\n glbParam->site: %s	lidarHeaderData.StartD: %s	lidarHeaderData.StartT: %s	lidarHeaderData.EndD: %s	lidarHeaderData.EndT: %s\n",
		//  		glbParam->site, glbParam->StartDate, glbParam->StartTime, glbParam->StopDate, glbParam->StopTime ) ;

	// StartDate = 05/08/2020
	sprintf( &glbParam->StartDate[2], "%s", &glbParam->StartDate[3] ) ; // 05/08/2020 --> 0508/2020
	sprintf( &glbParam->StartDate[4], "%s", &glbParam->StartDate[5] ) ; // 0508/2020  --> 05082020
	// StoptDate = 05/08/2020
	sprintf( &glbParam->StopDate[2], "%s", &glbParam->StopDate[3] ) ; // 05/08/2020 --> 0508/2020
	sprintf( &glbParam->StopDate[4], "%s", &glbParam->StopDate[5] ) ; // 0508/2020  --> 05082020

	// StartTime = 00:00:30
	sprintf( (char*)&glbParam->StartTime[2], "%s", (char*)&glbParam->StartTime[3] ) ; // 0000:30
	sprintf( (char*)&glbParam->StartTime[4], "%s", (char*)&glbParam->StartTime[5] ) ; // 000030
	// StoptDate = 00:00:40
	sprintf( (char*)&glbParam->StopTime[2], "%s", (char*)&glbParam->StopTime[3] ) ; // 0000:30
	sprintf( (char*)&glbParam->StopTime[4], "%s", (char*)&glbParam->StopTime[5] ) ; // 000030
		// printf("\nReadLicelTime_and_Coord()") ;
		// printf("\n glbParam->site: %s	lidarHeaderData.StartD: %s	lidarHeaderData.StartT: %s	\nlidarHeaderData.EndD: %s	lidarHeaderData.EndT: %s\n",
		//  		glbParam->site, glbParam->StartDate, glbParam->StartTime, glbParam->StopDate, glbParam->StopTime ) ;

}

void CLicel_DataFile_Handling::ReadLicelGobalParameters( char *lidarFile, strcGlobalParameters *glbParam )
{
	FILE *fid = fopen(lidarFile, "r");
	if ( fid == NULL )
		printf("\n Failed at opening the file %s \n", lidarFile) ;
	assert( fid !=NULL ) ;

	glbParam->nAnCh   = 0 ;
	glbParam->nPhotCh = 0 ;

	char strDump[20] ;

	glbParam->aAzimuth 	   = (double*) new double[glbParam->nEvents] ; memset( (double*)glbParam->aAzimuth    , 0, (sizeof(double)*glbParam->nEvents) ) ;
	glbParam->aZenith      = (double*) new double[glbParam->nEvents] ; memset( (double*)glbParam->aZenith     , 0, (sizeof(double)*glbParam->nEvents) ) ;
	glbParam->temp_K_agl = (double*) new double[glbParam->nEvents] ; memset( (double*)glbParam->temp_K_agl, 0, (sizeof(double)*glbParam->nEvents) ) ;
	glbParam->pres_Pa_agl     = (double*) new double[glbParam->nEvents] ; memset( (double*)glbParam->pres_Pa_agl    , 0, (sizeof(double)*glbParam->nEvents) ) ;
	glbParam->aAzimuthAVG  	  = (double*) new double[glbParam->nEventsAVG] ; memset( (double*)glbParam->aAzimuthAVG	   , 0, (sizeof(double)*glbParam->nEventsAVG) ) ;	
	glbParam->aZenithAVG   	  = (double*) new double[glbParam->nEventsAVG] ; memset( (double*)glbParam->aZenithAVG     , 0, (sizeof(double)*glbParam->nEventsAVG) ) ;
	glbParam->temp_K_agl_AVG = (double*) new double[glbParam->nEventsAVG] ; memset( (double*)glbParam->temp_K_agl_AVG, 0, (sizeof(double)*glbParam->nEventsAVG) ) ;
	glbParam->pres_Pa_agl_AVG     = (double*) new double[glbParam->nEventsAVG] ; memset( (double*)glbParam->pres_Pa_agl_AVG    , 0, (sizeof(double)*glbParam->nEventsAVG) ) ;
	glbParam->indxEndSig_ev   = (int*) new int[glbParam->nEventsAVG] ;

	strcLicelDataFile lidarHeaderData ;
// LINE 1: File name
	fscanf( fid, "%s ", lidarHeaderData.Name )  ;
// LINE 2:
	if ( strcmp( glbParam->inputDataFileFormat, "LICEL_FILE" ) ==0 )
	{
		fscanf( fid, "%s", strDump ) ;
		if ( strcmp( strDump, "Sao" ) ==0 )
		{
			strcpy( glbParam->site, "Sao Paul" ) ;
			fscanf( fid, "%s %10s %08s %10s %08s %lf %lf %lf %lf ", 
			strDump, lidarHeaderData.StartD, lidarHeaderData.StartT, lidarHeaderData.EndD, lidarHeaderData.EndT, 
			&glbParam->siteASL, &glbParam->siteLat, &glbParam->siteLong, &glbParam->aZenith[0] ) ;
		}
		else
		{
			strcpy( glbParam->site, strDump ) ;
			fscanf( fid, "%10s %08s %10s %08s %lf %lf %lf %lf ", 
			lidarHeaderData.StartD, lidarHeaderData.StartT, lidarHeaderData.EndD, lidarHeaderData.EndT, 
			&glbParam->siteASL, &glbParam->siteLat, &glbParam->siteLong, &glbParam->aZenith[0] ) ;
			// fscanf( fid, "%s %10s %08s %10s %08s %lf %lf %lf %lf ", 
			// glbParam->site, lidarHeaderData.StartD, lidarHeaderData.StartT, lidarHeaderData.EndD, lidarHeaderData.EndT, 
			// &glbParam->siteASL, &glbParam->siteLat, &glbParam->siteLong, &glbParam->aZenith[0] ) ;
		}
// printf("\n lidarHeaderData.Name: %s	glbParam->site: %s	lidarHeaderData.StartD: %s	lidarHeaderData.StartT: %s	lidarHeaderData.EndD: %s	lidarHeaderData.EndT: %s\n",
		// lidarHeaderData.Name, glbParam->site, lidarHeaderData.StartD, lidarHeaderData.StartT, lidarHeaderData.EndD, lidarHeaderData.EndT ) ;
	}
	else if ( strcmp(glbParam->inputDataFileFormat, "RAYMETRIC_FILE") ==0 )
	{
		fscanf( fid, "%s %10s %08s %10s %08s %lf %lf %lf %lf %lf %lf %lf ", 
		glbParam->site, lidarHeaderData.StartD, lidarHeaderData.StartT, lidarHeaderData.EndD, lidarHeaderData.EndT, 
		&glbParam->siteASL, &glbParam->siteLat, &glbParam->siteLong, &glbParam->aZenith[0], &glbParam->aAzimuth[0], &glbParam->temp_K_agl[0], &glbParam->pres_Pa_agl[0] ) ;
// printf("\n glbParam->aZenith[0]: %lf \t glbParam->aAzimuth[0]: %lf \t glbParam->temp_K_agl: %lf \t glbParam->pres_Pa_agl: %lf \n", glbParam->aZenith[0], glbParam->aAzimuth[0], glbParam->temp_K_agl, glbParam->pres_Pa_agl) ;
	}
// LINE 3:
	fscanf( fid, "%07d %04d %07d %04d %02d ", 
	&glbParam->Accum_Pulses[0], &glbParam->Laser_Frec[0], &glbParam->Accum_Pulses[1], &glbParam->Laser_Frec[1], &glbParam->nCh ) ;
// printf("\n glbParam->Accum_Pulses[0]: %d	glbParam->Laser_Frec[0]: %d		glbParam->Accum_Pulses[1]: %d	glbParam->Laser_Frec[1]: %d		glbParam->nCh: %d \n",
// 		glbParam->Accum_Pulses[0], glbParam->Laser_Frec[0], glbParam->Accum_Pulses[1], glbParam->Laser_Frec[1], glbParam->nCh ) ;

	glbParam->DAQ_Type     = (int*)    new int [ glbParam->nCh ] ;
	glbParam->Laser_Src   = (int*)    new int [ glbParam->nCh ] ;
	glbParam->nBinsRaw_Ch = (int*)    new int [ glbParam->nCh ] ;
	glbParam->PMT_Voltage = (int*)    new int [ glbParam->nCh ] ;
	glbParam->iLambda     = (int*)    new int [ glbParam->nCh ] ;
	glbParam->iADCbits    = (int*)    new int [ glbParam->nCh ] ;
	glbParam->nShots	  = (int*) 	  new int [ glbParam->nCh ] ;
	glbParam->sPol        = (char*)   new char[ glbParam->nCh ] ;
	glbParam->iMax_mVLic  = (double*) new double [ glbParam->nCh ] ;

// DATASETS
	for ( int i=0 ; i<glbParam->nCh ; i++ )
	{
		fscanf(fid,"%s %d %d %05d 1 %d %lf %05d.%s 0 0 00 000 %d %06d %lf %s", lidarHeaderData.sAct[i], &glbParam->DAQ_Type[i], &glbParam->Laser_Src[i],
		&glbParam->nBinsRaw_Ch[i], &glbParam->PMT_Voltage[i], &glbParam->dr, &glbParam->iLambda[i], lidarHeaderData.sPol[i], &glbParam->iADCbits[i], 
		&glbParam->nShots[i], &glbParam->iMax_mVLic[i], lidarHeaderData.sDescrp[i] ) ;

		glbParam->sPol[i]     = lidarHeaderData.sPol[i][0] ;
		// glbParam->DAQ_Type[i] --> 0: ANALOG	1:PHOTONCOUNTING
		if ( glbParam->DAQ_Type[i] ==0 )
		{
			glbParam->nAnCh++   ; // CONTADOR DE CANALES ANALOGICOS
			glbParam->iMax_mVLic[i] = glbParam->iMax_mVLic[i] *1000 ;
		}
		else if ( glbParam->DAQ_Type[i] ==1 ) 	glbParam->nPhotCh++ ; // CONTADOR DE CANALES DE FOTOCONTEO
	}
	if ( fclose(fid) != 0 )
		printf("\n Failed to close the lidar file.\n\n") ;

	glbParam->nLambda   = glbParam->nCh;
	glbParam->nBinsRaw  = *min_element( glbParam->nBinsRaw_Ch, glbParam->nBinsRaw_Ch +glbParam->nCh ) ;
	glbParam->nBins		= glbParam->nBinsRaw ;
	glbParam->nBins_in_File = glbParam->nBinsRaw ;
	strcpy( glbParam->fileName, lidarHeaderData.Name ) ;
}

void CLicel_DataFile_Handling::ReadLicelData( char *lidarFile, strcGlobalParameters *glbParam, strcLidarDataFile *dataFile )
{
	FILE *fid = fopen(lidarFile, "r");
	if ( fid == NULL )
		printf("\n Failed at opening the file %s \n", lidarFile) ;

	ReadLicelTime_and_Coord( (FILE*)fid, (strcGlobalParameters*)glbParam ) ;

	// int cAn, cPh ;
	// cAn =0 ; cPh =0 ;

	for ( int c=0 ; c<glbParam->nCh ; c++ )
	{
		fseek( fid, 80*(3+glbParam->nCh)+2 + c*(glbParam->nBinsRaw_Ch[c] * sizeof(int)+2), SEEK_SET ) ;
		fread ( (int*)&dataFile->db_ADC[c][0], sizeof(int), glbParam->nBinsRaw, fid ) ;
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
	}
	if ( fclose(fid) != 0 )
		printf("\n Failed to close the lidar file.\n\n") ;
} // FIN DEL ReadLidarData()

void ReadLicelTime_and_Coord( FILE *fid, strcGlobalParameters *glbParam )
{
	assert( fid !=NULL ) ;

	char strMisc[50] ;

// LINE 1: File name
	fscanf( fid, "%s ", strMisc )  ;
// LINE 2:
	if ( strcmp( glbParam->inputDataFileFormat, "LICEL_FILE" ) ==0 )
	{
		fscanf( fid, "%s ", strMisc ) ;
		if ( strcmp( strMisc, "Sao" ) ==0 )
		{
			sprintf( glbParam->site, "Sao Paul" ) ;
			fscanf( fid, "%s %10s %08s %10s %08s %lf %lf %lf %lf ", 
			strMisc, glbParam->StartDate, glbParam->StartTime, glbParam->StopDate, glbParam->StopTime, 
			&glbParam->siteASL, &glbParam->siteLat, &glbParam->siteLong, &glbParam->aZenith[glbParam->evSel] ) ;
		}
		else
		{
			sprintf( glbParam->site, "%s", strMisc ) ;
			fscanf( fid, "%10s %08s %10s %08s %lf %lf %lf %lf ",
					glbParam->StartDate, glbParam->StartTime, glbParam->StopDate, glbParam->StopTime, 
					&glbParam->siteASL, &glbParam->siteLat, &glbParam->siteLong, &glbParam->aZenith[glbParam->evSel] ) ;
			// fscanf( fid, "%s %10s %08s %10s %08s %lf %lf %lf %lf ", 
			// glbParam->site, glbParam->StartDate, glbParam->StartTime, glbParam->StopDate, glbParam->StopTime, 
			// &glbParam->siteASL, &glbParam->siteLat, &glbParam->siteLong, &glbParam->aZenith[glbParam->evSel] ) ;
		}
		// printf("\nReadLicelTime_and_Coord()") ;
		// printf("\n glbParam->site: %s	lidarHeaderData.StartD: %s	lidarHeaderData.StartT: %s	lidarHeaderData.EndD: %s	lidarHeaderData.EndT: %s\n",
		//  		glbParam->site, glbParam->StartDate, glbParam->StartTime, glbParam->StopDate, glbParam->StopTime ) ;
		glbParam->aAzimuth[glbParam->evSel] 	 = (double)0 ;
		//! IMPLEMENT THIS: IF THE FILE IS LICEL TYPE, SET THE TEMP AND PRESSURE FROM THE CONFIGURATION FILE.
		//! SAVE THIS ARRAYS IN DATA LEVEL 0 
		// ReadAnalisysParameter( (const char*) glbParam->FILE_PARAMETERS, "Temperature_at_Lidar_Station_K", "double", (double*)&glbParam->temp_K_agl[glbParam->evSel] ) ;
		// ReadAnalisysParameter( (const char*) glbParam->FILE_PARAMETERS, "Pressure_at_Lidar_Station_Pa"   , "double", (double*)&glbParam->pres_Pa_agl[glbParam->evSel]     ) ;
		glbParam->temp_K_agl[glbParam->evSel] = (double)-999.0 ;
		glbParam->pres_Pa_agl[glbParam->evSel]     = (double)-999.0 ;
	}
	else if ( strcmp(glbParam->inputDataFileFormat, "RAYMETRIC_FILE") ==0 )
	{
		fscanf( fid, "%s %10s %08s %10s %08s %lf %lf %lf %lf %lf %lf %lf ", 
		glbParam->site, glbParam->StartDate, glbParam->StartTime, glbParam->StopDate, glbParam->StopTime, 
		&glbParam->siteASL, &glbParam->siteLat, &glbParam->siteLong, &glbParam->aZenith[glbParam->evSel], 
		&glbParam->aAzimuth[glbParam->evSel], &glbParam->temp_K_agl[glbParam->evSel], &glbParam->pres_Pa_agl[glbParam->evSel] ) ;
	}
		// printf("\nReadLicelTime_and_Coord()") ;
		// printf("\n glbParam->site: %s	lidarHeaderData.StartD: %s	lidarHeaderData.StartT: %s	lidarHeaderData.EndD: %s	lidarHeaderData.EndT: %s\n",
		//  		glbParam->site, glbParam->StartDate, glbParam->StartTime, glbParam->StopDate, glbParam->StopTime ) ;

	// StartDate = 05/08/2020
	sprintf( &glbParam->StartDate[2], "%s", &glbParam->StartDate[3] ) ; // 05/08/2020 --> 0508/2020
	sprintf( &glbParam->StartDate[4], "%s", &glbParam->StartDate[5] ) ; // 0508/2020  --> 05082020
	// StoptDate = 05/08/2020
	sprintf( &glbParam->StopDate[2], "%s", &glbParam->StopDate[3] ) ; // 05/08/2020 --> 0508/2020
	sprintf( &glbParam->StopDate[4], "%s", &glbParam->StopDate[5] ) ; // 0508/2020  --> 05082020

	// StartTime = 00:00:30
	sprintf( (char*)&glbParam->StartTime[2], "%s", (char*)&glbParam->StartTime[3] ) ; // 0000:30
	sprintf( (char*)&glbParam->StartTime[4], "%s", (char*)&glbParam->StartTime[5] ) ; // 000030
	// StoptDate = 00:00:40
	sprintf( (char*)&glbParam->StopTime[2], "%s", (char*)&glbParam->StopTime[3] ) ; // 0000:30
	sprintf( (char*)&glbParam->StopTime[4], "%s", (char*)&glbParam->StopTime[5] ) ; // 000030
		// printf("\nReadLicelTime_and_Coord()") ;
		// printf("\n glbParam->site: %s	lidarHeaderData.StartD: %s	lidarHeaderData.StartT: %s	\nlidarHeaderData.EndD: %s	lidarHeaderData.EndT: %s\n",
		//  		glbParam->site, glbParam->StartDate, glbParam->StartTime, glbParam->StopDate, glbParam->StopTime ) ;
}

void CLicel_DataFile_Handling::check_Lidar_Files_Consistency( strcGlobalParameters *glbParam, char **inputFilesInTime )
{

}
