
#include "libLidar.hpp"

const double DBL_EPS_COMP = 1 - DBL_EPSILON; // DBL_EPSILON is defined in <limits.h>.
inline double RandU()
{
    return DBL_EPSILON + ((double) rand()/RAND_MAX) ;
}
inline double RandN2(double mu, double sigma)
{
    return mu + (rand()%2 ? -1.0 : 1.0)*sigma*pow(-log(DBL_EPS_COMP*RandU()), 0.5) ;
}

int pstrcmp( const void* a, const void* b )
{
  return strcmp( *(const char**)a, *(const char**)b );
}

int getInputFilesInTimeRange( char *PathFile_IN_FULL, char **inputFilesInTime, strcGlobalParameters *glbParam ) 
{
    // int     nFilesInInputFolder =0 ;
	int 	nFilesInTime = 0 ;

    // GET THE NUMBERS minTime_num AND maxTime_num, WICH REPRESENT minTime AND maxTime STORED IN analysisParameters.dat
        char 	sMinTime[30], sMaxTime[30] ;
        ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, (const char*)"minTime", (const char*)"string", (char*)sMinTime ) ;
        ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, (const char*)"maxTime", (const char*)"string", (char*)sMaxTime ) ;
        struct tm		tmMin, tmMax ;
        sscanf(sMinTime, "%4d/%2d/%2d-%2d:%2d:%2d", &tmMin.tm_year, &tmMin.tm_mon, &tmMin.tm_mday, &tmMin.tm_hour, &tmMin.tm_min, &tmMin.tm_sec ) ;
        sscanf(sMaxTime, "%4d/%2d/%2d-%2d:%2d:%2d", &tmMax.tm_year, &tmMax.tm_mon, &tmMax.tm_mday, &tmMax.tm_hour, &tmMax.tm_min, &tmMax.tm_sec ) ;
        tmMin.tm_year = tmMin.tm_year -1900 ;   tmMin.tm_isdst = 0 ;
        tmMax.tm_year = tmMax.tm_year -1900 ;   tmMax.tm_isdst = 0 ;

        time_t	minTime_num = mktime( (tm*)&tmMin ) ; // time_t = long --> NUMBER OF SECONDS SINCE January 1,1970 (?)
        time_t	maxTime_num = mktime( (tm*)&tmMax ) ; // time_t = long --> NUMBER OF SECONDS SINCE January 1,1970 (?)
            printf("\n minTime (%ld): %04d/%02d/%02d-%02d:%02d:%02d"  , minTime_num, (tmMin.tm_year +1900), tmMin.tm_mon, tmMin.tm_mday, tmMin.tm_hour, tmMin.tm_min, tmMin.tm_sec ) ;
            printf("\n maxTime (%ld): %04d/%02d/%02d-%02d:%02d:%02d\n", maxTime_num, (tmMax.tm_year +1900), tmMax.tm_mon, tmMax.tm_mday, tmMax.tm_hour, tmMax.tm_min, tmMax.tm_sec ) ;

    // LOAD inputFilesInTime (STRING MATRIX) WITH THE INPUT FILES BETWEEN minTime AND maxTime RECORDED IN analysisParameters.dat
		DIR *d;
        d = opendir(PathFile_IN_FULL) ;
		struct dirent *dir;
        if (d)
        {   
            while ( (dir = readdir(d)) != NULL )
            {
                if ( ( dir->d_type == DT_REG ) && ( strcmp(dir->d_name, "..") !=0 ) && ( strcmp(dir->d_name, ".") !=0 ) )
                {
                    if ( ( strcmp( sMinTime, sMaxTime ) == 0 ) || ( isFileInTimeRange( (char*)dir->d_name, (time_t)minTime_num, (time_t)maxTime_num, (strcGlobalParameters*)glbParam ) == true ) )
                    {
                        sprintf(inputFilesInTime[nFilesInTime], "%s%s", PathFile_IN_FULL, dir->d_name ) ;
                        nFilesInTime = nFilesInTime +1 ;
                    }
                }
            }
            closedir(d);
        }
        qsort( inputFilesInTime, nFilesInTime, sizeof(inputFilesInTime[0]), pstrcmp );
    // }
	return nFilesInTime ;
}

bool isFileInTimeRange ( char *fileName, time_t minTime_num, time_t maxTime_num, strcGlobalParameters *glbParam )
{
// GET THE NUMBER WHICH REPRESENT THE FILE TIME
	struct 	tm		tmFile ;
	char	dumpChar1, dumpChar2 ;
	int		dumpInt  ;

	if ( strcmp(glbParam->inputDataFileFormat, "LICEL_FILE") ==0 )
		sscanf( fileName, "%c%2d%1x%2d%2d.%2d%2d%2d", &dumpChar1, &tmFile.tm_year, &tmFile.tm_mon, &tmFile.tm_mday, &tmFile.tm_hour, &tmFile.tm_min, &tmFile.tm_sec, &dumpInt ) ;
	else if ( strcmp(glbParam->inputDataFileFormat, "RAYMETRIC_FILE") ==0 )
		sscanf( fileName, "%c%c%2d%1x%2d%2d.%2d", &dumpChar1, &dumpChar2, &tmFile.tm_year, &tmFile.tm_mon, &tmFile.tm_mday, &tmFile.tm_hour, &tmFile.tm_min ) ;
	else
		printf("\n Wrong 'inputDataFileFormat' parameter in mergingParameters.conf file \n") ;

	if (tmFile.tm_year >=90)
		tmFile.tm_year = 1900 + tmFile.tm_year -1900 ;
	else
		tmFile.tm_year = 2000 + tmFile.tm_year -1900 ; // tmFile.tm_year: SHOULD BE STORED THE NUMBER OF YEARS SINCE 1900.

	tmFile.tm_isdst = 0 ;
	time_t	fileTime_num = (time_t) mktime( (tm*)&tmFile ) ;
	// printf("\n file time (%ld): %c%02d%1d%02d%02d.%02d%02d%02d (%s)", fileTime_num, dumpChar, (tmFile.tm_year +1900), tmFile.tm_mon, tmFile.tm_mday, tmFile.tm_hour, tmFile.tm_min, tmFile.tm_sec, dumpInt, fileName ) ;

// DEFINE IF THE FILE TIME IS IN THE RANGE [minTime-maxTime]
	if ( (minTime_num <= fileTime_num) && (fileTime_num <= maxTime_num ) )
		return true	;
	else
		return false ;
}

int findLastCharInString( char *strOrig, const char charToFind )
{
	int 	offsetLastChar 	  ;

	for( int i=0 ; i<=(int)strlen(strOrig) ; i++)
  	{
  		if( strOrig[i] == charToFind )  
		{
  			offsetLastChar = i ;
 		}
	}
	return (offsetLastChar +1) ;
}
   
int ReadAnalisysParameter( const char *fileName, const char *varToFind, const char *varType, void *var )
{
    ifstream    myfile (fileName) ;
    bool        found =false ;
	int			nElemVec;

    if (myfile.is_open())
    {
		string  line, var_name, dump, eq ;
		stringstream ss ;

		while ( !myfile.eof() )
		{
			getline (myfile, line) ;
			if ( ( (line.find(varToFind, 0)) != string::npos ) && (line[0] != '#') )
			{
				found = true ;
				ss << line ;
				// CHECK IF THE VARIABLE ITS A VECTOR
				nElemVec = count( line.begin(), line.end(), ':' ) ;
				nElemVec++ ;

				if ( strcmp( varType, "int" ) == 0 )
				{
					int var_int ;
					ss >> var_name >> eq ;
					for ( int e =0 ; e <nElemVec ; e++ )
					{// ss >> *((int*)var) ;
						ss >> var_int >> dump ;
						memcpy( (int*)var +e, (int*)&var_int, sizeof(int) ) ;
					}
				}
				else if ( strcmp( varType, "float" ) == 0 )
				{
					float var_float ;
					ss >> var_name >> eq ;
					for ( int e =0 ; e <nElemVec ; e++ )
					{	// ss >> *((float*)var) ;
						ss >> var_float >> dump ;
						memcpy( (float*)var +e, (float*)&var_float, sizeof(float) ) ;
					}
				}
				else if ( strcmp( varType, "double" ) == 0 )
				{
					double var_double ;
					ss >> var_name >> eq  ;
					for ( int e =0 ; e <nElemVec ; e++ )
					{//	ss >> *((double*)var) ;
						ss >> var_double >> dump ;
						memcpy( (double*)var +e, (double*)&var_double, sizeof(double) ) ;
					}
				}
				else if ( strcmp( varType, "string" ) == 0 )
				{
					ss >> var_name >> eq >> (char*)var ;
					// printf("\n var: %s \n", (char*)var) ;
					// string var_string ;
					// ss >> var_name >> eq ;
					// for ( int e =0 ; e <nElemVec ; e++ )
					// {
					// 	ss >> var_string >> dump ;
					// 	memcpy( (char*)var +e, (char*)var_string.c_str(), var_string.size() ) ;
					// }
				}
			}
		}
		myfile.close();
    }
    else 
	{
		cout << endl << "ReadAnalisysParameter() --> Unable to open file " << fileName << endl ; 
		return 0 ;
	}
    if ( found == false)
    {
        if ( strcmp( varType, "string" ) == 0 )
            strcpy( ((char*)var), "NOT_FOUND") ;
        else if ( strcmp( varType, "int" ) == 0 )
            *((int*)var) = -2000 ;
        else if ( strcmp( varType, "float" ) == 0 )
            *((int*)var) = -2000 ;
        else if ( strcmp( varType, "double" ) == 0 )
            *((int*)var) = -2000 ;
		
		return 0 ;
    }
	return nElemVec ;
}

/**
 * @brief 
 * All the files has to have the same header, and this information is taken from the first file in the time range.
 * @param lidarFile Lidar file to process.
 * @param glbParam General struct parameters where the parameters from the header will be stored.
 */
void ReadLicelGobalParameters( char *lidarFile, strcGlobalParameters *glbParam )
{
	FILE *fid = fopen(lidarFile, "r");
	if ( fid == NULL )
		printf("\n Failed at opening the file %s \n", lidarFile) ;
	assert( fid !=NULL ) ;

	glbParam->nAnCh   = 0 ;
	glbParam->nPhotCh = 0 ;

	// ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "indxOffset" , "int" , (double*)&glbParam->indxOffset ) ;
	char strDump[20] ;

	glbParam->aAzimuth 	   = (double*) new double[glbParam->nEvents] ; memset( (double*)glbParam->aAzimuth    , 0, (sizeof(double)*glbParam->nEvents) ) ;
	glbParam->aZenith      = (double*) new double[glbParam->nEvents] ; memset( (double*)glbParam->aZenith     , 0, (sizeof(double)*glbParam->nEvents) ) ;
	glbParam->temp_Celsius = (double*) new double[glbParam->nEvents] ; memset( (double*)glbParam->temp_Celsius, 0, (sizeof(double)*glbParam->nEvents) ) ;
	glbParam->pres_hPa     = (double*) new double[glbParam->nEvents] ; memset( (double*)glbParam->pres_hPa    , 0, (sizeof(double)*glbParam->nEvents) ) ;
	glbParam->aAzimuthAVG  	  = (double*) new double[glbParam->nEventsAVG] ; memset( (double*)glbParam->aAzimuthAVG	   , 0, (sizeof(double)*glbParam->nEventsAVG) ) ;	
	glbParam->aZenithAVG   	  = (double*) new double[glbParam->nEventsAVG] ; memset( (double*)glbParam->aZenithAVG     , 0, (sizeof(double)*glbParam->nEventsAVG) ) ;
	glbParam->temp_CelsiusAVG = (double*) new double[glbParam->nEventsAVG] ; memset( (double*)glbParam->temp_CelsiusAVG, 0, (sizeof(double)*glbParam->nEventsAVG) ) ;
	glbParam->pres_hPaAVG     = (double*) new double[glbParam->nEventsAVG] ; memset( (double*)glbParam->pres_hPaAVG    , 0, (sizeof(double)*glbParam->nEventsAVG) ) ;
	glbParam->indxEndSigEvnt  = (int*) new int[glbParam->nEventsAVG] ;

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
			fscanf( fid, "%s %10s %08s %10s %08s %lf %lf %lf %lf ", 
			glbParam->site, lidarHeaderData.StartD, lidarHeaderData.StartT, lidarHeaderData.EndD, lidarHeaderData.EndT, 
			&glbParam->siteASL, &glbParam->siteLat, &glbParam->siteLong, &glbParam->aZenith[0] ) ;
		}
		// printf("\n glbParam->site: %s	lidarHeaderData.StartD: %s	lidarHeaderData.StartT: %s	lidarHeaderData.EndD: %s	lidarHeaderData.EndT: %s\n",
		//  		glbParam->site, lidarHeaderData.StartD, lidarHeaderData.StartT, lidarHeaderData.EndD, lidarHeaderData.EndT ) ;
	}
	else if ( strcmp(glbParam->inputDataFileFormat, "RAYMETRIC_FILE") ==0 )
	{
		fscanf( fid, "%s %10s %08s %10s %08s %lf %lf %lf %lf %lf %lf %lf ", 
		glbParam->site, lidarHeaderData.StartD, lidarHeaderData.StartT, lidarHeaderData.EndD, lidarHeaderData.EndT, 
		&glbParam->siteASL, &glbParam->siteLat, &glbParam->siteLong, &glbParam->aZenith[0], &glbParam->aAzimuth[0], &glbParam->temp_Celsius[0], &glbParam->pres_hPa[0] ) ;
		// printf("\n glbParam->aZenith[0]: %lf \t glbParam->aAzimuth[0]: %lf \t glbParam->temp_Celsius: %lf \t glbParam->pres_hPa: %lf \n", glbParam->aZenith[0], glbParam->aAzimuth[0], glbParam->temp_Celsius, glbParam->pres_hPa) ;
	}
// LINE 3:
	fscanf( fid, "%07d %04d %07d %04d %02d ", 
	&glbParam->Accum_Pulses[0], &glbParam->Laser_Frec[0], &glbParam->Accum_Pulses[1], &glbParam->Laser_Frec[1], &glbParam->nCh ) ;
	// printf("\n glbParam->Accum_Pulses[0]: %d	glbParam->Laser_Frec[0]: %d		glbParam->Accum_Pulses[1]: %d	glbParam->Laser_Frec[1]: %d		glbParam->nCh: %d \n",
	// 		glbParam->Accum_Pulses[0], glbParam->Laser_Frec[0], glbParam->Accum_Pulses[1], glbParam->Laser_Frec[1], glbParam->nCh ) ;

	glbParam->iAnPhot     = (int*)    new int [ glbParam->nCh ] ;
	glbParam->Laser_Src   = (int*)    new int [ glbParam->nCh ] ;
	glbParam->nBinsRaw_Ch = (int*)    new int [ glbParam->nCh ] ;
	glbParam->PMT_Voltage = (int*)    new int [ glbParam->nCh ] ;
	glbParam->iLambda     = (int*)    new int [ glbParam->nCh ] ;
	glbParam->iADCbits    = (int*)    new int [ glbParam->nCh ] ;
	glbParam->sPol        = (char*)   new char[ glbParam->nCh ] ;
	glbParam->iMax_mVLic  = (double*) new double [ glbParam->nCh ] ;

// DATASETS
	for ( int i=0 ; i<glbParam->nCh ; i++ )
	{
		fscanf(fid,"%s %d %d %05d 1 %d %lf %05d.%s 0 0 00 000 %d %06d %lf %s", lidarHeaderData.sAct[i], &glbParam->iAnPhot[i], &glbParam->Laser_Src[i],
		&glbParam->nBinsRaw_Ch[i], &glbParam->PMT_Voltage[i], &glbParam->dr, &glbParam->iLambda[i], lidarHeaderData.sPol[i], &glbParam->iADCbits[i], 
		&glbParam->nShots[i], &glbParam->iMax_mVLic[i], lidarHeaderData.sDescrp[i] ) ;

		glbParam->sPol[i]     = lidarHeaderData.sPol[i][0] ;
		// glbParam->iAnPhot[i] --> 0: ANALOG	1:PHOTONCOUNTING
		if ( glbParam->iAnPhot[i] ==0 )
		{
			glbParam->nAnCh++   ; // CONTADOR DE CANALES ANALOGICOS
			glbParam->iMax_mVLic[i] = glbParam->iMax_mVLic[i] *1000 ;
		}
		else if ( glbParam->iAnPhot[i] ==1 ) 	glbParam->nPhotCh++ ; // CONTADOR DE CANALES DE FOTOCONTEO
	}
	if ( fclose(fid) != 0 )
		printf("\n Failed to close the lidar file.\n\n") ;

	glbParam->nLambda   = glbParam->nCh;
	glbParam->nBinsRaw = *min_element( glbParam->nBinsRaw_Ch, glbParam->nBinsRaw_Ch +glbParam->nCh ) ;
	strcpy( glbParam->fileName, lidarHeaderData.Name ) ;

	// ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, (const char*)"rInitSig", (const char*)"double" , (double*)&glbParam->rInitSig ) ;
	// glbParam->indxInitSig = (int)round( glbParam->rInitSig /glbParam->dr) ;
	// glbParam->indxInitInversion = glbParam->indxInitSig ;
	// glbParam->indxInitErr 		= glbParam->indxInitSig ;

	// ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, (const char*)"rEndSig" , (const char*)"double" , (double*)&glbParam->rEndSig ) ;
	// glbParam->indxEndSig = (int)round( glbParam->rEndSig /glbParam->dr ) ;
	// 	if( glbParam->indxEndSig > (glbParam->nBins-1) )
	// 		glbParam->indxEndSig = glbParam->nBins -1 ;

	// glbParam->r	= (double*) new double[glbParam->nBins] ;
	// for( int i=0 ; i < glbParam->nBins ; i++ )
	// 		glbParam->r[i] = (i+1) * glbParam->dr ;

	// ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, (const char*)"nBinsBkg" , (const char*)"int" , (double*)&glbParam->nBinsBkg ) ;
	// glbParam->nBinsBkg = (int)round( glbParam->nBinsBkg ) ;
}

void ReadLicelData( char *lidarFile, strcGlobalParameters *glbParam, strcLidarDataFile *dataFile )
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
		fread ( dataFile->db_ADC[c], sizeof(int), glbParam->nBinsRaw, fid ) ;
		// if ( glbParam->iAnPhot[c] ==0 ) // ANALOG
		// {
		// 	glbParam->ScaleFactor_Analog = ( glbParam->iMax_mVLic[c] / pow(2, glbParam->iADCbits[c]) ) ;
		// 	for( int b=0 ; b<glbParam->nBinsRaw ; b++ )
		// 		dataFile->db_mV[cAn][b] = (double) ( dataFile->db_ADC[c][b] * glbParam->ScaleFactor_Analog ) ;
		// 	cAn++ ;
		// }
		// else if ( glbParam->iAnPhot[c] ==1 ) // PHOTONCOUNTING
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
			&glbParam->siteASL, &glbParam->siteLat, &glbParam->siteLong, &glbParam->aZenith[glbParam->event_analyzed] ) ;
		}
		else
		{
			fscanf( fid, "%s %10s %08s %10s %08s %lf %lf %lf %lf ", 
			glbParam->site, glbParam->StartDate, glbParam->StartTime, glbParam->StopDate, glbParam->StopTime, 
			&glbParam->siteASL, &glbParam->siteLat, &glbParam->siteLong, &glbParam->aZenith[glbParam->event_analyzed] ) ;
		}
		// printf("\nReadLicelTime_and_Coord()") ;
		// printf("\n glbParam->site: %s	lidarHeaderData.StartD: %s	lidarHeaderData.StartT: %s	lidarHeaderData.EndD: %s	lidarHeaderData.EndT: %s\n",
		//  		glbParam->site, glbParam->StartDate, glbParam->StartTime, glbParam->StopDate, glbParam->StopTime ) ;
		glbParam->aAzimuth[glbParam->event_analyzed] 	 = (double)0 ;
		glbParam->temp_Celsius[glbParam->event_analyzed] = (double)-999.0 ;
		glbParam->pres_hPa[glbParam->event_analyzed]     = (double)-999.0 ;
	}
	else if ( strcmp(glbParam->inputDataFileFormat, "RAYMETRIC_FILE") ==0 )
	{
		fscanf( fid, "%s %10s %08s %10s %08s %lf %lf %lf %lf %lf %lf %lf ", 
		glbParam->site, glbParam->StartDate, glbParam->StartTime, glbParam->StopDate, glbParam->StopTime, 
		&glbParam->siteASL, &glbParam->siteLat, &glbParam->siteLong, &glbParam->aZenith[glbParam->event_analyzed], 
		&glbParam->aAzimuth[glbParam->event_analyzed], &glbParam->temp_Celsius[glbParam->event_analyzed], &glbParam->pres_hPa[glbParam->event_analyzed] ) ;
	}

	// StartDate = 05/08/2020
	sprintf( &glbParam->StartDate[2], "%s", &glbParam->StartDate[3] ) ; // 05/08/2020 --> 0508/2020
	sprintf( &glbParam->StartDate[4], "%s", &glbParam->StartDate[5] ) ; // 0508/2020  --> 05082020
	// StoptDate = 05/08/2020
	strcpy( &glbParam->StopDate[2], &glbParam->StopDate[3] ) ; // 0508/2020
	strcpy( &glbParam->StopDate[4], &glbParam->StopDate[5] ) ; // 05082020

	// StartTime = 00:00:30
	sprintf( (char*)&glbParam->StartTime[2], "%s", (char*)&glbParam->StartTime[3] ) ; // 0000:30
	sprintf( (char*)&glbParam->StartTime[4], "%s", (char*)&glbParam->StartTime[5] ) ; // 000030
	// StoptDate = 00:00:40
	strcpy( &glbParam->StopTime[2], &glbParam->StopTime[3] ) ; // 0000:30
	strcpy( &glbParam->StopTime[4], &glbParam->StopTime[5] ) ; // 000030
}

void RayleighFit( double *sig, double *sigMol, int nBins, const char *modeBkg, const char *modeRangesFit, strcFitParam *fitParam, double *sigFil )
{
	fitParam->sumsq_m = (double)0  ;

	// cout<<"---- RayleighFit" ;
	// printf("\n\nfitParam->indxInicFit: %d\nfitParam->indxEndFit: %d\nfitParam->nFit: %d\n\n", fitParam->indxInicFit, fitParam->indxEndFit, fitParam->nFit) ;

	if ( strcmp( modeBkg, "wB" ) == 0 )
	{
		double *coeff = (double*) new double[2] ;
		polyfitCoeff( (const double* const) &sigMol[fitParam->indxInicFit], // X DATA
		  			  (const double* const) &sig[fitParam->indxInicFit], // Y DATA
					  (unsigned int       ) fitParam->nFit,
					  (unsigned int		  ) 1,
					  (double*			  ) coeff	 ) ;
		fitParam->m = (double)coeff[1] ;
		fitParam->b = (double)coeff[0] ;
		delete coeff ;

		if ( strcmp( modeRangesFit, "all" ) == 0 )
		{
			for ( int i=0 ; i < nBins ; i++ )
			{
				sigFil[i] = (double) ( sigMol[i] * fitParam->m + fitParam->b ) ;
				fitParam->sumsq_m = fitParam->sumsq_m + pow( (sigFil[i] - sig[i]), 2 ) ;
			}
		}
		else
		{ // "NOTall"
			for ( int i=fitParam->indxInicFit ; i <=fitParam->indxEndFit ; i++ )
			{
				sigFil[i] = (double) ( sigMol[i] * fitParam->m + fitParam->b ) ;
				fitParam->sumsq_m = fitParam->sumsq_m + pow( (sigFil[i] - sig[i]), 2 ) ;
			}
		}
	}
	else if ( strcmp( modeBkg, "wOutB" ) == 0 )
	{
		double	mNum=0, mDen=0 ;
		for ( int i=fitParam->indxInicFit ; i <= fitParam->indxEndFit ; i++ )
		{
			mNum = mNum + sig   [i]*sigMol[i] ;
			mDen = mDen + sigMol[i]*sigMol[i] ;
		}
		fitParam->m = mNum/mDen ;
		fitParam->b = 0     	;

		if ( strcmp( modeRangesFit, "all" ) == 0 )
		{
			for ( int i=0 ; i < nBins ; i++ )
			{
				sigFil[i] = (double) ( sigMol[i] * fitParam->m ) ;
				fitParam->sumsq_m = fitParam->sumsq_m + pow( (sigFil[i] - sig[i]), 2 ) ;
			}
		}
		else
		{
			for ( int i=fitParam->indxInicFit ; i <=fitParam->indxEndFit ; i++ )
			{
				sigFil[i] = (double) ( sigMol[i] * fitParam->m ) ;
				fitParam->sumsq_m = fitParam->sumsq_m + pow( (sigFil[i] - sig[i]), 2 ) ;
			}
		}
	}
}

// void GetCloudLimits( strcGlobalParameters *glbParam, strcMolecularData *dataMol, strcCloudProfiles *cloudProfiles, strcIndexMol *indxMol )
// {
// 	double *dco   = (double*) new double[glbParam->nBins] ;
// 	memset( (double*)dco  , 0, (sizeof(double) * glbParam->nBins) ) ;
// 	for ( int i = 0 ; i<=glbParam->indxEndSig ; i++ )
// 		dco[i] = (double)( cloudProfiles->clouds_ON[i+1] - cloudProfiles->clouds_ON[i] ) ;

// 	int nInicCloud=0, nEndCloud=0 ;
// 	for (int i =0; i <=glbParam->indxEndSig; i++)
// 	{
// 		if ( dco[i] ==(double)BIN_CLOUD )
// 			nInicCloud++ ;
// 		else if ( dco[i]==(double)(-BIN_CLOUD) )
// 			nEndCloud++ ;
// 	}
// 	if ( nInicCloud == nEndCloud )
// 	{
// 		memset( cloudProfiles->indxInitClouds, 0, ( sizeof(int) * NMAXCLOUDS ) ) ;
// 		memset( cloudProfiles->indxEndClouds , 0, ( sizeof(int) * NMAXCLOUDS ) ) ;
// 		memset( indxMol->indxInicMol, 0, ( sizeof(int) * MAX_MOL_RANGES  ) ) ;
// 		memset( indxMol->indxEndMol , 0, ( sizeof(int) * MAX_MOL_RANGES  ) ) ;
// 		cloudProfiles->nClouds  = 0 ; // USED AS INDEX AND THEN, AS TOTAL NUMBER.
// 		indxMol->nMolRanges 	= 0 ; // USED AS INDEX AND THEN, AS TOTAL NUMBER.

// 		//CLOUDS DETECTION
// 		for( int i=0 ; i <=(glbParam->indxEndSig-1) ; i++ )
// 		{
// 			if( dco[i] == (double)BIN_CLOUD ) // INIT CLOUD
// 			{
// 				cloudProfiles->indxInitClouds[cloudProfiles->nClouds] = i ;
// 				for (int j =i ; j <=(glbParam->indxEndSig-1) ; j++)
// 				{	// SEARCH THE END OF THE CLOUD
// 					if( dco[j] == (double)(-BIN_CLOUD) ) // END CLOUD
// 					{
// 						cloudProfiles->indxEndClouds[cloudProfiles->nClouds] = j -1;
// 						cloudProfiles->nClouds++ ; // COUNT CLOUDS
// 						break ;
// 					}
// 				}
// 			}
// 		}
// 		// MOLECULAR RANGES DETECTION
// 		indxMol->nMolRanges = cloudProfiles->nClouds +1 ;
// 		if ( indxMol->nMolRanges ==1 )
// 		{
// 			int 	heightRef_Inversion_ASL ;
// 			ReadAnalisysParameter( glbParam->FILE_PARAMETERS, "heightRef_Inversion_ASL" , "int" , (int*)&heightRef_Inversion_ASL ) ;
// 			indxMol->indxRef = (int)round(heightRef_Inversion_ASL /dataMol->dzr) ;
// 				if ( indxMol->indxRef > (glbParam->nBins-1) )
// 					indxMol->indxRef = glbParam->nBins -10 ;

// 			for (int i =0; i < NMAXCLOUDS ; i++)
// 			{
// 				cloudProfiles->indxInitClouds[i] = 0 ;
// 				cloudProfiles->indxEndClouds [i] = 0 ;
// 				indxMol->indxInicMol[i] = 0 ;
// 				indxMol->indxEndMol[i]  = glbParam->indxEndSig  ;
// 			}
// 		}
// 		else
// 		{
// 			for( int i=0 ; i <indxMol->nMolRanges ; i++ )
// 			{
// 				if (i==0)
// 				{
// 					indxMol->indxInicMol[i] = 0 ;
// 					indxMol->indxEndMol[i]  = cloudProfiles->indxInitClouds[i] -1  ;
// 				}
// 				else
// 				{
// 					indxMol->indxInicMol[i] = cloudProfiles->indxEndClouds[i-1] +1 ;
// 					if ( i == (indxMol->nMolRanges-1) )
// 						indxMol->indxEndMol[i]  = glbParam->indxEndSig ;
// 					else
// 						indxMol->indxEndMol[i]  = cloudProfiles->indxInitClouds[i] -1 ;
// 				}
// 			}
// 		}
// 	}
// 	else
// 		printf("\nGetCloudLim(...) --> error\n") ;

// 	delete dco   ;
// }

void TransmissionMethod_pr( double *pr, strcGlobalParameters *glbParam, strcMolecularData *dataMol, int indxBefCloud, int indxAftCloud, double *VOD )
{
	double *prFit = (double*) new double [ glbParam->nBins ] ;

	int DELTA_RANGE_LIM_BINS ;
	ReadAnalisysParameter( (const char*) glbParam->FILE_PARAMETERS, "DELTA_RANGE_LIM_BINS", "int", (int*)&DELTA_RANGE_LIM_BINS ) ;

	strcFitParam fitParam_before_cloud ;
	strcFitParam fitParam_after_cloud  ;
	double prBeforeCloud, prAfterCloud ;
	int indxA, indxB ;
	do
	{
		fitParam_before_cloud.indxInicFit = indxBefCloud - (int)round(DELTA_RANGE_LIM_BINS) ;
		fitParam_before_cloud.indxEndFit  = indxBefCloud - (int)round(DELTA_RANGE_LIM_BINS/5) ;
		fitParam_before_cloud.nFit = fitParam_before_cloud.indxEndFit - fitParam_before_cloud.indxInicFit ;
			RayleighFit( (double*)pr, (double*)dataMol->prMol, glbParam->nBins, "wB", "NOTall", (strcFitParam*)&fitParam_before_cloud, (double*)prFit ) ;
			indxB = indxBefCloud -(int)round(DELTA_RANGE_LIM_BINS/2) ;
			prBeforeCloud = prFit[indxB] ;
		fitParam_after_cloud.indxInicFit = indxAftCloud + (int)round(DELTA_RANGE_LIM_BINS/5) ;
		fitParam_after_cloud.indxEndFit  = indxAftCloud + (int)round(DELTA_RANGE_LIM_BINS) ;
		fitParam_after_cloud.nFit = fitParam_after_cloud.indxEndFit - fitParam_after_cloud.indxInicFit ;
			RayleighFit( (double*)pr, (double*)dataMol->prMol, glbParam->nBins, "wB", "NOTall", (strcFitParam*)&fitParam_after_cloud, (double*)prFit ) ;
			indxA = indxAftCloud +(int)round(DELTA_RANGE_LIM_BINS/2) ;
			prAfterCloud = prFit[indxA] ;
		// DELTA_RANGE_LIM_BINS = DELTA_RANGE_LIM_BINS -3 ;
		indxBefCloud = indxBefCloud -1 ;
		indxAftCloud = indxAftCloud +1 ;
	} while( (prBeforeCloud < prAfterCloud) ) ;

	double rPr 		= prAfterCloud / prBeforeCloud ;
	double rPrMol   = dataMol->prMol[indxA] / dataMol->prMol[indxB] ;
	double rR2 		= pow(glbParam->r[indxA], 2) / pow(glbParam->r[indxB], 2) ;
	double Tp2Cloud	= rPr / (rPrMol * rR2) ;
//  printf("\t\t %d-%d --- rPr: %lf - rPrMol: %lf - Tp2Cloud: %lf \n", indxBefCloud, indxAftCloud, rPr, rPrMol, Tp2Cloud) ;
	*VOD = -0.5*log(Tp2Cloud) *cos(dataMol->zenith*PI/180) ;
	delete prFit ;
}

void ODcut( double *prS, strcMolecularData *dataMol, strcGlobalParameters *glbParam, strcFitParam *fitParam, int *clouds_ON )
{
	double OD =0 ; // CLOUD TRANSMISSION
	// double *pr2	   = (double*) new double [ glbParam->nBins ] ;

	// 	MakeRangeCorrected_array( (double*)prS, (strcGlobalParameters*)glbParam, (strcMolecularData*)dataMol, (double*)pr2 ) ;

	int indxBefCloud, indxAftCloud ;
	indxBefCloud = fitParam->indxInicFit ;
	indxAftCloud = fitParam->indxEndFit ;
		TransmissionMethod_pr( (double*)prS, (strcGlobalParameters*)glbParam, (strcMolecularData*)dataMol, (int)indxBefCloud, (int)indxAftCloud, (double*)&OD ) ;

	double OD_cut ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "OD_cut", "double", (double*)&OD_cut ) ;

	if( (OD >=0) && (OD <= OD_cut) )
	{
		printf("\n<%d> filtering cloud due to OD (%lf)...\n", glbParam->event_analyzed, OD) ;
		for (int i =fitParam->indxEndFit; i <=fitParam->indxEndFit ; i++)
			clouds_ON[i] =0;
	}
	// else
	// 	printf("\nOptical Depth of the cloud: %lf \t Altitude (asl): %lf - %lf\n", OD, glbParam->siteASL + fitParam->indxInicFit * dataMol->dzr, glbParam->siteASL + fitParam->indxEndFit * dataMol->dzr ) ;

	// delete pr2 ;
}

void Elastic_Rayleigh_Lidar_Signal ( strcMolecularData *dataMol, double *r )
{
	double *MOD = (double*) new double[ dataMol->nBins ] ;

	cumtrapz( (r[1]-r[0]), dataMol->alphaMol, 0, dataMol->nBins-1, (double*)MOD ) ;

	for ( int b=0 ; b < dataMol->nBins  ; b++ )
	{
		dataMol->pr2Mol[b] 	= (double)  dataMol->betaMol[b] * exp( -2*MOD[b] ) ;
		dataMol->prMol [b] 	= (double) ( dataMol->pr2Mol[b]/(r[b]*r[b]) ) ;
	}
	delete MOD ;
}

// void RadLowToHighRes( int nLR, double *xLR, double *yLR, int nHR, double *xHR, double *yHR )
// {
// 	double *coeff = (double*) new double[3 +1] ;
	
// 	polyfitCoeff( (const double* const) xLR, // X DATA
// 			    (const double* const  ) yLR, // Y DATA
// 			    (unsigned int         ) nLR,
// 			    (unsigned int		  ) 3,
// 			    (double*			  ) coeff	 ) ;

// 	for (int i =0 ; i <nHR ; i++ )
// 	{
// 		yHR[i] = pow(xHR[i], 3) *coeff[3] + pow(xHR[i], 2) *coeff[2] + xHR[i]*coeff[1] + coeff[0] ;
// 	}
// 	delete coeff ;	
// }

// void ReadUsSTDfile( const char *radFile, strcRadioSoundingData *RadData, strcMolecularData *dataMol )
// {
// 	char 	ch ;
// 	int 	lines=0 ;
// 	char 	lineaRad[100], strDump[100] ;

// 	FILE *fid = fopen(radFile, "r");
// 	if ( fid == NULL )
// 		printf("\n ReadUsSTDfile(...): No se puede abrir el archivo %s \n", radFile) ;

// // OBTENGO LA CANTIDAD DE FILAS, O CANTIDAD DE BINES DEL RADIOSONDEO (BAJA RESOLUCION).
// 	while( !feof(fid) )
// 	{
// 	  ch = fgetc(fid);
// 	  if(ch == '\n')	lines++ ;
// 	}
// 	fseek( fid, 0, 0 ) ; // RESET THE POINTER

// // READ RADIOSONDE DATA REFERENCED AT *ASL* AND LOW RESOLUTION (LR)
// 	double *zLR = (double*) new double [lines] ;
// 	double *pLR = (double*) new double [lines] ;
// 	double *tLR = (double*) new double [lines] ;
// 	double *nLR = (double*) new double [lines] ;
// 	for ( int l=0 ; l<lines ; l++ ) // BARRO LAS LINEAS.
// 	{
// 		fgets(lineaRad, 100, fid) ;
// 		sscanf(lineaRad, "%lf,%lf,%lf,%s", &zLR[l], &tLR[l], &pLR[l], strDump ) ; // a-t-p
// 	}
// // SITE INDEX ALTITUDE (indxMin)
// // FIND THE INDEX OF zLR WHERE zLR=siteASL AND WHEN zLR=glbParam->rEndSig
// 	double  diff ;
// 	double  diffMin, diffMax ;
// 	int 	indxMin =0, indxMax =0 ;
// 	for ( int i=0 ; i<lines ; i++ )
// 	{
// 		diff = fabs( dataMol->zr[0] - zLR[i] ) ;
// 		if ( i==0 )
// 		{	
// 			diffMin	= diff 	;
// 			indxMin = i		;
// 		}
// 		else if ( diff < diffMin )
// 		{	
// 			diffMin = diff 	;
// 			indxMin = i 	;
// 		}
// 		diff = fabs( float(dataMol->zr[dataMol->nBins-1]) - zLR[i] ) ;
// 		if ( i==0 )
// 		{	
// 			diffMax	= diff 	;
// 			indxMax = i		;
// 		}
// 		else if ( diff < diffMax )
// 		{	
// 			diffMax = diff 	;
// 			indxMax = i 	;
// 		}
// 	}
// 	RadData->nBinsLR = indxMax - indxMin +1 ; // lines - indxMin ;
// 	RadData->zLR = (double*) new double [RadData->nBinsLR] ;
// 	RadData->pLR = (double*) new double [RadData->nBinsLR] ;
// 	RadData->tLR = (double*) new double [RadData->nBinsLR] ;
// 	RadData->nLR = (double*) new double [RadData->nBinsLR] ;

// 	for ( int b=0 ; b<RadData->nBinsLR ; b++ ) // BARRO LAS LINEAS.
// 	{ // RadData->xLR[0] CORRESPOND TO THE POINT OF THE SITE ALTITUDE ASL.
// 		RadData->zLR[b] = zLR[b+indxMin] ; // RadData->zLR --> ASL
// 		RadData->tLR[b] = tLR[b+indxMin] ; // RadData->tLR --> ASL
// 		RadData->pLR[b] = pLR[b+indxMin] ; // RadData->pLR --> ASL
// 	}
// 	// PARA EL SPLINE, INICIO Y FIN DE LA ALTURA TIENEN QUE SER IGUALES
// 	RadData->zLR[0] = dataMol->zr[0] ;
// 	RadData->zLR[RadData->nBinsLR-1] = dataMol->zr[dataMol->nBins-1] ;

// 	fclose(fid) ;
// 	delete zLR ;
// 	delete pLR ;
// 	delete tLR ;
// 	delete nLR ;
// }


// MEMORY GETTERS 
// void GetMem_cloudProfiles( strcCloudProfiles *cloudProfiles, strcGlobalParameters *glbParam )
// {
// 	for( int e=0 ; e<glbParam->nEventsAVG ; e++ )
// 	{
// 		cloudProfiles[e].clouds_ON 	    = (int*) new int [glbParam->nBins]   	 ; memset( cloudProfiles[e].clouds_ON     , 0, ( sizeof(int) * glbParam->nBins) ) ;
// 		cloudProfiles[e].indxInitClouds = (int*) new int [NMAXCLOUDS]        	 ; memset( cloudProfiles[e].indxInitClouds, 0, ( sizeof(int) * NMAXCLOUDS	    ) ) ;
// 		cloudProfiles[e].indxEndClouds  = (int*) new int [NMAXCLOUDS]        	 ; memset( cloudProfiles[e].indxEndClouds , 0, ( sizeof(int) * NMAXCLOUDS	    ) ) ;
// 		cloudProfiles[e].VOD_cloud      = (double*) new double [NMAXCLOUDS]  	 ; memset( cloudProfiles[e].VOD_cloud     , 0, ( sizeof(double) * NMAXCLOUDS  ) ) ;
// 		cloudProfiles[e].test_1  		= (double*) new double [glbParam->nBins] ; memset( cloudProfiles[e].test_1		  , 0, ( sizeof(double) * glbParam->nBins ) ) ;
// 		cloudProfiles[e].test_2  		= (double*) new double [glbParam->nBins] ; memset( cloudProfiles[e].test_2		  , 0, ( sizeof(double) * glbParam->nBins ) ) ;
// 		cloudProfiles[e].nClouds		= (int)0 ;
// 	}
// }

void GetMem_DataFile( strcLidarDataFile *dataFile, strcGlobalParameters *glbParam )
{
 	for (int e = 0; e <glbParam->nEvents ; e++)
	{ // EVENT=FILE --- nEvents : NUMBER OF FILES TO AVERAGE. ALL WILL BE SAVED IN dataFile[e]. 
		dataFile[e].db_ADC = (int**) new int* [ glbParam->nCh ] ;
		for ( int c=0 ; c <glbParam->nCh ; c++ )
		{
			dataFile[e].db_ADC[c] = (int*)  new int  [ glbParam->nBinsRaw ] ;
				memset( (int*)dataFile[e].db_ADC[c], 0, sizeof(int)*glbParam->nBinsRaw ) ;
		}
        // if ( glbParam->nAnCh !=0 )
        // {
        //     dataFile[e].db_mV = (double**) new double* [ glbParam->nAnCh ] ;
        //     for ( int c=0 ; c <glbParam->nAnCh ; c++ )
        //     {
        //         dataFile[e].db_mV[c] = (double*) new double [ glbParam->nBinsRaw ] ;
        //             memset( (double*)dataFile[e].db_mV[c], 0, sizeof(double)*glbParam->nBinsRaw ) ;
        //     }
        // }
        // if ( glbParam->nPhotCh !=0 )
        // {
        //     dataFile[e].db_CountsMHz = (double**) new double* [ glbParam->nPhotCh ] ;
        //     for ( int c=0 ; c <glbParam->nPhotCh ; c++ )
        //     {
        //         dataFile[e].db_CountsMHz[c] = (double*) new int [ glbParam->nBinsRaw ] ;
        //             memset( (int*)dataFile[e].db_CountsMHz[c], 0, sizeof(double)*glbParam->nBinsRaw ) ;
        //     }
        // }
	}
}

void GetMem_indxMol( strcIndexMol *indxMol, strcGlobalParameters *glbParam )
{
	for( int e=0 ; e<glbParam->nEventsAVG ; e++ )
	{
		indxMol[e].indxMolON   = (int*)    new int   [glbParam->nBins] ;	memset( indxMol[e].indxMolON  , 0, ( sizeof(int)    * glbParam->nBins ) ) ;
		indxMol[e].errRMS      = (double*) new double[glbParam->nBins] ;	memset( indxMol[e].errRMS     , 0, ( sizeof(double) * glbParam->nBins ) ) ;
		indxMol[e].indxInicMol = (int*)    new int   [MAX_MOL_RANGES]  ;	memset( indxMol[e].indxInicMol, 0, ( sizeof(int)    * MAX_MOL_RANGES  ) ) ;
		indxMol[e].indxEndMol  = (int*)    new int   [MAX_MOL_RANGES]  ;	memset( indxMol[e].indxEndMol , 0, ( sizeof(int)    * MAX_MOL_RANGES  ) ) ;
		indxMol[e].nMolRanges  = (int)0	; // CONTAIN THE NUMBER OF MOLECULAR RANGES DETECTED.
    	indxMol[e].indxRef	   = (int)0 ;
		indxMol[e].ablHeight   = (int)0 ;
		indxMol[e].endRayFit   = (int)0 ;
	}
}

void GetMem_evSig( strcLidarSignal *evSig, strcGlobalParameters *glbParam )
{
    evSig->pr			= (double*) new double [glbParam->nBins] ; memset( evSig->pr	  , 0, ( sizeof(double) * glbParam->nBins ) ) ;
    evSig->pr_noBkg 	= (double*) new double [glbParam->nBins] ; memset( evSig->pr_noBkg, 0, ( sizeof(double) * glbParam->nBins ) ) ;
    evSig->pr2			= (double*) new double [glbParam->nBins] ; memset( evSig->pr2	  , 0, ( sizeof(double) * glbParam->nBins ) ) ;
    evSig->prFit		= (double*) new double [glbParam->nBins] ; memset( evSig->prFit	  , 0, ( sizeof(double) * glbParam->nBins ) ) ;
    evSig->pr2Fit		= (double*) new double [glbParam->nBins] ; memset( evSig->pr2Fit  , 0, ( sizeof(double) * glbParam->nBins ) ) ;
    evSig->pr2Glued		= (double*) new double [glbParam->nBins] ; memset( evSig->pr2Glued, 0, ( sizeof(double) * glbParam->nBins ) ) ;
    evSig->prGlued 		= (double*) new double [glbParam->nBins] ; memset( evSig->prGlued , 0, ( sizeof(double) * glbParam->nBins ) ) ;
}

// void GetMem_dataMol( strcMolecularData *dataMol, strcGlobalParameters *glbParam )
// {
// 		dataMol->nBins		= glbParam->nBins ;
// 		dataMol->zr			= (double*) new double [glbParam->nBins] ; memset( dataMol->zr		, 0, ( sizeof(double) * glbParam->nBins) ) ;
// 		dataMol->betaMol 	= (double*) new double [glbParam->nBins] ; memset( dataMol->betaMol , 0, ( sizeof(double) * glbParam->nBins) ) ;
// 		// dataMol->betaRam   	= (double*) new double [glbParam->nBins] ; memset( dataMol->betaRam , 0, ( sizeof(double) * glbParam->nBins) ) ;
// 		dataMol->alphaMol	= (double*) new double [glbParam->nBins] ; memset( dataMol->alphaMol, 0, ( sizeof(double) * glbParam->nBins) ) ;
// 		dataMol->nMol		= (double*) new double [glbParam->nBins] ; memset( dataMol->nMol	, 0, ( sizeof(double) * glbParam->nBins) ) ;
// 		dataMol->prMol	 	= (double*) new double [glbParam->nBins] ; memset( dataMol->prMol	, 0, ( sizeof(double) * glbParam->nBins) ) ;
// 		dataMol->pr2Mol	 	= (double*) new double [glbParam->nBins] ; memset( dataMol->pr2Mol	, 0, ( sizeof(double) * glbParam->nBins) ) ;
// 		dataMol->zenith		= 0 ;
// }

void GetMemVectorsFernaldInversion( strcFernaldInversion *fernaldVectors, int nBins )
{
	fernaldVectors->pr2n 	   	  = (double*) new double [ nBins ] ;	memset( fernaldVectors->pr2n  		 , 0, sizeof(double) * nBins ) ;
	fernaldVectors->prn 	   	  = (double*) new double [ nBins ] ;	memset( fernaldVectors->prn  		 , 0, sizeof(double) * nBins ) ;
	fernaldVectors->phi  	 	  = (double*) new double [ nBins ] ;	memset( fernaldVectors->phi   		 , 0, sizeof(double) * nBins ) ;
	fernaldVectors->p 		  	  = (double*) new double [ nBins ] ;	memset( fernaldVectors->p   		 , 0, sizeof(double) * nBins ) ;
	fernaldVectors->ip		  	  = (double*) new double [ nBins ] ;	memset( fernaldVectors->ip    		 , 0, sizeof(double) * nBins ) ;
	fernaldVectors->ipN 		  = (double*) new double [ nBins ] ;	memset( fernaldVectors->ipN   		 , 0, sizeof(double) * nBins ) ;
	fernaldVectors->betaT 		  = (double*) new double [ nBins ] ;	memset( fernaldVectors->betaT 		 , 0, sizeof(double) * nBins ) ;
	fernaldVectors->pr2Fit		  = (double*) new double [ nBins ] ;	memset( fernaldVectors->pr2Fit		 , 0, sizeof(double) * nBins ) ;
	fernaldVectors->prFit		  = (double*) new double [ nBins ] ;	memset( fernaldVectors->prFit		 , 0, sizeof(double) * nBins ) ;
	fernaldVectors->intAlphaMol_r = (double*) new double [ nBins ] ;	memset( fernaldVectors->intAlphaMol_r, 0, sizeof(double) * nBins ) ;
	fernaldVectors->created = true ;
}

void GetMem_dataAer( strcAerosolData *dataAer, strcGlobalParameters *glbParam )
{
    dataAer->alphaAer	 = (double*) new double [glbParam->nBins]   ; memset( dataAer->alphaAer, 0, ( glbParam->nBins * sizeof(double) ) ) ;
    dataAer->betaAer	 = (double*) new double [glbParam->nBins]   ; memset( dataAer->betaAer , 0, ( glbParam->nBins * sizeof(double) ) ) ;
    dataAer->VAODr		 = (double*) new double [glbParam->nBins]   ; memset( dataAer->VAODr   , 0, ( glbParam->nBins * sizeof(double) ) ) ;
}

void GetMemStrcErrorSigSet( strcErrorSignalSet *errSigSet, int nSigSetErr, int nBins )
{
		errSigSet->prGlued  	   = (double**) new double*[nSigSetErr] ;
		errSigSet->prNoisy  	   = (double**) new double*[nSigSetErr] ;
		errSigSet->pr2Noisy 	   = (double**) new double*[nSigSetErr] ;
		errSigSet->alphaAer_ErrSet = (double**) new double*[nSigSetErr] ;
		errSigSet->VAODr_ErrSet    = (double**) new double*[nSigSetErr] ;

		for( int j=0 ; j < nSigSetErr ; j++ )
		{
			errSigSet->prGlued		  [j] = (double*) new double[nBins] ;
			errSigSet->prNoisy		  [j] = (double*) new double[nBins] ;
			errSigSet->pr2Noisy		  [j] = (double*) new double[nBins] ;
			errSigSet->alphaAer_ErrSet[j] = (double*) new double[nBins] ;
			errSigSet->   VAODr_ErrSet[j] = (double*) new double[nBins] ;
		}

		errSigSet->VAODr_meanErrSet	   = (double*) new double[nBins] ; 	memset( errSigSet->VAODr_meanErrSet   , 0, sizeof(double) * nBins ) ;
		errSigSet-> VAODr_stdErrSet	   = (double*) new double[nBins] ; 	memset( errSigSet->VAODr_stdErrSet    , 0, sizeof(double) * nBins ) ;
		errSigSet->alphaAer_meanErrSet = (double*) new double[nBins] ; 	memset( errSigSet->alphaAer_meanErrSet, 0, sizeof(double) * nBins ) ;
		errSigSet->alphaAer_stdErrSet  = (double*) new double[nBins] ;	memset( errSigSet->alphaAer_stdErrSet , 0, sizeof(double) * nBins ) ;

		memset( errSigSet->hVAOD_meanErrSet, 0, sizeof(double) * 4 ) ;
		memset( errSigSet-> hVAOD_stdErrSet, 0, sizeof(double) * 4 ) ;
}

int CheckLidarDataBaseIntegrity( strcLidarDataFile *dataFile, strcGlobalParameters *glbParam, int *evStatus )
{
	int nBinsMean ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "meanAmeanBsize", "int" , (int*)&nBinsMean ) ;

	int 	spamAvgWin ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "spamAvgWin", "int", (int*)&spamAvgWin ) ;
	double *s = (double*) new double[glbParam->nBins] ;
	double meanA, meanB ;
	meanA = meanB = 0   ;

	for ( int e=0 ; e < (glbParam->nEvents) ; e++ )
	{
		smooth( (double*)dataFile[glbParam->chSel].db_mV[e], (int)0, (int)(glbParam->nBins-1), (int)spamAvgWin, (double*)s ); // s -> LIDAR SIGNAL SMOOTHED

		for ( int i=round(glbParam->indxInitSig/2) ; i<(round(glbParam->indxInitSig/2)+nBinsMean) ; i++ )	meanA = meanA + s[i] ;
		meanA = meanA /nBinsMean ;

		for ( int i=(glbParam->indxEndSig-nBinsMean) ; i < (glbParam->indxEndSig-1) ; i++ ) meanB = meanB + s[i] ;
		meanB = meanB /nBinsMean ;

		if ( meanA < (1.1*meanB) ) // IF BOTH VALUES ARE TOO CLOSE ---> NOISE --> FLUSH THE EVENT
		{
			evStatus[e] = (int)EVNT_WRONG ; // LIDAR SIGNAL COMPLETELY DESTROYED - NOISE
			memset( (double*)dataFile[glbParam->chSel].db_mV[e], (double)0, (sizeof(double)*glbParam->nBins) ); // SET THE EVENT WITH ZEROS
		}
		meanA = 0 ;
		meanB = 0 ;
	}

	delete s ;
	return 0 ;
}

int CheckLidarDataBaseIntegrityAVG( strcLidarDataFile *dataFileAVG, strcGlobalParameters *glbParam, int *evStatusAVG )
{
	double sumOut ;
	// double nBinsFactor ;
	// ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "nBinsFactor", "double", (int*)&nBinsFactor ) ;

	for ( int c=0 ; c < (glbParam->nEventsAVG) ; c++ )
	{// THE PROFILES WERE ALREADY CHECKED IN dataFile[]. IF WERE FOUND WRONG, WERE SET TO ZERO
		sumOut = 0 ;
		sum( (double*)dataFileAVG[glbParam->chSel].db_mV[c], (int)0, (int)(glbParam->nBins-1), (double*)&sumOut ) ;

		if ( sumOut == 0 ) // IF BOTH VALUES ARE TOO CLOSE ---> NOISE --> FLUSH THE EVENT
			evStatusAVG[c] = (int)EVNT_WRONG ; // LIDAR SIGNAL COMPLETELY DESTROYED - NOISE
		else
		{
			evStatusAVG[c] = EVNT_OK ; // (int)checkUnderShoot( (strcLidarDataFile*)dataFileAVG, (strcGlobalParameters*)glbParam, (int)c ) ;

			// if ( glbParam->nBinsEvnt[c] < (15000/glbParam->dr) )
			// {
			// 	// glbParam->nBinsEvnt[c] = (int)round(10000/glbParam->dr) ; // glbParam->nBins ;
			// 	glbParam->nBinsEvnt[c] =(int) round(glbParam->nBinsEvnt[c] *nBinsFactor) ;
			// }
			// else 
			// {
				// ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "deltaUnderShoot", "int" , (int*)&deltaUnderShoot ) ;
				// sum( (double*)dataFileAVG[glbParam->chSel].db_mV_TEST[e], (int)(glbParam->nBinsEvnt[e] -deltaUnderShoot), (int)(indxMinPr +deltaUnderShoot), (double*)&sTestSum ) ;
				// if ( sTestSum <0 )
				// 	return EVNT_WRONG_USHOOT ;
				// else
				// glbParam->nBinsEvnt[c] =(int)round(glbParam->nBinsEvnt[c]) ; //  *nBinsFactor
			// }
		}
	}

	return 0 ;
}

void checkUnderShoot( double *pr, strcGlobalParameters *glbParam, strcMolecularData *dataMol, int* evStatusAVG )
{
	strcFitParam	fitParam ;
	int 			stepIndxEnd =0;
	double 			*prFit 		= (double*) new double[glbParam->nBins] ;

	double nBinsFactor ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "nBinsFactor", "double", (int*)&nBinsFactor ) ;

SET_NEW_INDX_END_SIG:	
	fitParam.indxEndFit  = glbParam->indxEndSig -stepIndxEnd;
	fitParam.indxInicFit = fitParam.indxEndFit - glbParam->nBinsBkg ; //glbParam->nBins - 1 - ; //  glbParam->nBins - 1
	fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInicFit ;
		RayleighFit( (double*)pr, (double*)dataMol->prMol, glbParam->nBins, "wB", "NOTall", (strcFitParam*)&fitParam, (double*)prFit ) ;	
	if( fitParam.m <0 )
	{
		stepIndxEnd = stepIndxEnd +100 ;
		goto SET_NEW_INDX_END_SIG ;
	}
	else
	{
		if ( fitParam.indxInicFit > (int)round(15000/glbParam->dr) ) // if ( fitParam.indxEndFit > (int)round(15000/glbParam->dr) )
		{ // EVENT PROBABLY WITH UNDERSHOOT, BUT STILL USEFULL...
			glbParam->indxEndSigEvnt[glbParam->event_analyzed] = (int)round(fitParam.indxEndFit *nBinsFactor) ;
			glbParam->rEndSig    	 = (double)(glbParam->indxEndSigEvnt[glbParam->event_analyzed] * glbParam->dr) ;
		}
		else // EVENT WITH UNDERSHOOT, UN-USEFULL...
		{
			evStatusAVG[glbParam->event_analyzed] = EVNT_WRONG_USHOOT ;
			glbParam->indxEndSigEvnt[glbParam->event_analyzed] = (int)round(fitParam.indxEndFit *nBinsFactor) ;
			glbParam->rEndSig    	 = (double)(glbParam->indxEndSigEvnt[glbParam->event_analyzed] * glbParam->dr) ;
		}
	}

	// printf("(%d) NEW VALUES --> glbParam->nBins: %d \t glbParam->rEndSig: %lf \t glbParam->indxEndSig: %d", glbParam->event_analyzed, glbParam->nBins, glbParam->rEndSig, glbParam->indxEndSigEvnt[glbParam->event_analyzed] ) ;
	delete prFit ;

	// cout << endl <<"-------------------- checkUnderShoot -> e: " << glbParam->event_analyzed << endl ;
	// printf("glbParam->indxEndSigEvnt[glbParam->event_analyzed]: %d\n", glbParam->indxEndSigEvnt[glbParam->event_analyzed] ) ;
}

// int checkUnderShoot( strcLidarDataFile *dataFileAVG, strcGlobalParameters *glbParam, int e )
// {
// 	int 	deltaUnderShoot ;
// 	double 	minPr, sTestSum, bkg ;

// 	strcFitParam	fitParam ;
// 	fitParam.indxInicFit = glbParam->nBins - glbParam->nBinsBkg ; // glbParam->indxEndSig - glbParam->nBinsBkg ; // 
// 	fitParam.indxEndFit  = glbParam->nBins ; // glbParam->indxEndSig ; // 
// 	fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInicFit ;

// 	int 	avg_check_undershoot ;
// 	ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "avg_check_undershoot", "int", (int*)&avg_check_undershoot ) ;
// 	smoothGSL( (double*)dataFileAVG[glbParam->chSel].db_mV[e], (int)(glbParam->nBins -1), (int)avg_check_undershoot, (double*)dataFileAVG[glbParam->chSel].db_mV_TEST[e] ) ;

// 	// findIndxMin( (double*)dataFileAVG[glbParam->chSel].db_mV_TEST[e], (int)(glbParam->indxInitSig), (int)(glbParam->nBins-avg_check_undershoot), (int*)&glbParam->nBinsEvnt[e], (double*)&minPr ) ;
// 	findIndxMin( (double*)dataFileAVG[glbParam->chSel].db_mV_TEST[e], (int)(glbParam->indxInitSig), (int)glbParam->indxEndSig, (int*)&glbParam->nBinsEvnt[e], (double*)&minPr ) ;
// 	// glbParam->nBinsEvnt[e] = glbParam->nBinsEvnt[e] +1 ;
// 	// printf( "\n e: %d \t glbParam->nBinsEvnt[%d]: %d ", e, e, glbParam->nBinsEvnt[e] ) ;

// 		return EVNT_OK ;
// }

int GetIndxEvntToAvg_DiscScan( strcGlobalParameters *glbParam, strcIndxLimToAvg *indxEvntToAvg )
{
	double 	*dZeniths = (double*) new double [glbParam->nEvents -1] ;
	diffPr( (double*)glbParam->aZenith, (int)(glbParam->nEvents-1), (double*)dZeniths ); //  [i]= [i+1] - [i]

	int 	c = 0 ;
	indxEvntToAvg[c].indxEvntInit = 0 ; // indxEvntToAvg[glbParam.numClusters]
	for( int e=0 ; e<glbParam->nEvents-1 ; e++ )
	{
		if ( fabs(dZeniths[e]) >2 )
		{
			indxEvntToAvg[c].indxEvntEnd = e ;
			c++ ;
			indxEvntToAvg[c].indxEvntInit = e+1 ;
		}
	}
	indxEvntToAvg[c].indxEvntEnd = glbParam->nEvents-1 ;

	return 0 ;
}

void GetIndxEvntToAvg_ContScan( strcGlobalParameters *glbParam, strcIndxLimToAvg *indxEvntToAvg )
{
	int indxS =0 ;
	int dIndx =1 ;
	// int c =0 ;

	glbParam->nEventsAVG =0 ;
	indxEvntToAvg[glbParam->nEventsAVG].indxEvntInit =0 ;

	int deltaZenithAvg ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "deltaZenithAvg", "int", (int*)&deltaZenithAvg ) ;

	COUNT_nEventsAVG:
						if ( (indxS + dIndx) <glbParam->nEvents )
						{
							if ( fabs(glbParam->aZenith[indxS] - glbParam->aZenith[indxS + dIndx]) >(int)deltaZenithAvg )
							{
								indxEvntToAvg[glbParam->nEventsAVG].indxEvntEnd = indxS + dIndx -1 ;
								glbParam->nEventsAVG++ ;
								indxS = indxS + dIndx ;
								indxEvntToAvg[glbParam->nEventsAVG].indxEvntInit = indxS ;
								dIndx =1 ;
								goto COUNT_nEventsAVG ;
							}
							else
							{
								dIndx++;
								goto COUNT_nEventsAVG ;
							}
						}
						else
						{
							indxEvntToAvg[glbParam->nEventsAVG].indxEvntEnd = glbParam->nEvents -1 ;
							if ( indxEvntToAvg[glbParam->nEventsAVG].indxEvntInit == indxEvntToAvg[glbParam->nEventsAVG].indxEvntEnd )
							{
								glbParam->nEventsAVG-- ;
								indxEvntToAvg[glbParam->nEventsAVG].indxEvntEnd = glbParam->nEvents -1 ;
								glbParam->nEventsAVG++ ;
							}
							else
								glbParam->nEventsAVG++ ;
						}
}

int GetBinOffset( strcLidarDataFile *dataFile, strcGlobalParameters *glbParam )
{
	double 	*prDiff	= (double*) new double [ glbParam->nBinsRaw ] ;
	memset( (double*)prDiff, 0, sizeof(double)*glbParam->nBinsRaw );

	int 	indxMaxLidarSig ;
	int 	indxMinLaser, indxMaxLaser ;
	double 	max, minLaser, maxLaser ;
	int 	binOffset =0 ;

	for( int e=0 ; e<glbParam->nEventsAVG ; e++ ) // for( int e=0 ; e<glbParam->nEvents ; e++ ) 
	{
		diffPr( dataFile[glbParam->chSel].db_mV[e], glbParam->nBinsRaw, prDiff ) ;

		findIndxMax( (double*)prDiff, (int)0, (int)glbParam->indxInitSig, (int*)&indxMaxLidarSig, (double*)&max ) ;

		findIndxMin( (double*)prDiff, (int)0, (int)indxMaxLidarSig, (int*)&indxMinLaser, (double*)&minLaser ) ;
		findIndxMax( (double*)prDiff, (int)0, (int)indxMaxLidarSig, (int*)&indxMaxLaser, (double*)&maxLaser ) ;

		binOffset = binOffset + (int)round( (double)((indxMinLaser + indxMaxLaser)/2) ) ;
	}
	
	for (int c =0 ; c <glbParam->nCh ; c++)
		glbParam->indxOffset[c] = (int) glbParam->indxOffset[glbParam->chSel] ;

	delete prDiff ;

	return 0 ;
}

int AverageLidarSignal( strcLidarDataFile *dataFile, strcGlobalParameters *glbParam, int *evStatus, strcIndxLimToAvg *indxEvntToAvg, strcLidarDataFile *dataFileAVG )
{
	int         evntOKcounter = 0 ;
    long int    gpsTime_EventCluster = 0 ;

	for( int c=0 ; c<glbParam->nEventsAVG ; c++ ) // CLUSTER: CLUSTER OF *EVENTS*
	{
		indxEvntToAvg[c].zenithClusterEvnt = 0 ;
		for( int b=0 ; b<glbParam->nBinsRaw ; b++ )
		{
			for( int e=indxEvntToAvg[c].indxEvntInit ; e<=indxEvntToAvg[c].indxEvntEnd ; e++ )
			{
                    if ( b==0 )
                    {
                        indxEvntToAvg[c].zenithClusterEvnt  = (double)indxEvntToAvg[c].zenithClusterEvnt + (double)glbParam->aZenith[e] ;
                        gpsTime_EventCluster = gpsTime_EventCluster + (long int)glbParam->event_gps_sec[e] ;
                    }
				dataFileAVG[glbParam->chSel].db_mV 		  [c][b] = (double)dataFileAVG[glbParam->chSel].db_mV[c][b] 	   + (double)dataFile[glbParam->chSel].db_mV[e][b]	;
				dataFileAVG[glbParam->chSel].db_CountsMHz [c][b] = (double)dataFileAVG[glbParam->chSel].db_CountsMHz[c][b] + (double)dataFile[glbParam->chSel].db_CountsMHz[e][b]	;

					if ( evStatus[e] == EVNT_OK ) // IF EVENT IS WRONG --> dataFile[glbParam->chSel].db_mV [e][:] = 0 
						evntOKcounter++ ;
			}
			if( evntOKcounter ==0 ) evntOKcounter =1 ;
			dataFileAVG[glbParam->chSel].db_mV[c][b]  		= (double)dataFileAVG[glbParam->chSel].db_mV[c][b]         / evntOKcounter ;
			dataFileAVG[glbParam->chSel].db_CountsMHz[c][b] = (double)dataFileAVG[glbParam->chSel].db_CountsMHz[c][b]  / evntOKcounter ;
            evntOKcounter = 0 ;
		}
        glbParam->event_gps_sec[c] = (int) round(gpsTime_EventCluster / (indxEvntToAvg[c].indxEvntEnd - indxEvntToAvg[c].indxEvntInit +1)) ;
        gpsTime_EventCluster = 0 ;
		indxEvntToAvg[c].zenithClusterEvnt = indxEvntToAvg[c].zenithClusterEvnt / (indxEvntToAvg[c].indxEvntEnd - indxEvntToAvg[c].indxEvntInit +1) ;
	}
    memset( (int*)&glbParam->event_gps_sec[glbParam->nEventsAVG], 0, sizeof(int)*(glbParam->nEvents - glbParam->nEventsAVG) );

	// memset( glbParam->aZenith, 0, sizeof(double)*glbParam->nEvents ) ;
	// glbParam->nEvents = glbParam->nEventsAVG ;
	for( int c=0 ; c<glbParam->nEventsAVG ; c++ )
		glbParam->aZenithAVG[c] = indxEvntToAvg[c].zenithClusterEvnt ;

	for( int c=glbParam->nEventsAVG ; c<glbParam->nEvents ; c++ )
		glbParam->aZenithAVG[c] = -1 ; // FILL THE REST

	return 0 ;    
}

int selectChannel ( strcGlobalParameters *glbParam, const char *tel_range ) // tel_range = used_channel = long_range_2 - long_range_1 - nothing
{
	char 	strSearch[20], strInfo[20] ;

	for ( int i=0 ; i<glbParam->nChMax ; i++ )
	{
		sprintf( strSearch, "%s.%d.ch%d", glbParam->site, glbParam->year, i ) ;

			ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, strSearch, "string" , (char*)strInfo ) ;
		if ( strcmp( strInfo, tel_range ) == 0 )
			return i ;
	}
    return -1 ;
}

int ReadChannelSelected ( strcGlobalParameters *glbParam ) // tel_range = used_channel = long_range_2 - long_range_1 - nothing
{
	char 	strSearch[50] ;
	int		used_channel  ;

	sprintf( strSearch, "%s.%d.used_channel", glbParam->site, glbParam->year ) ;
	// printf("\n ReadChannelSelected() --> strSearch: %s \n", strSearch) ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, strSearch, "int" , (int*)&used_channel ) ;
	// printf("\n ReadChannelSelected() --> used_channel: %d \n", used_channel) ;

    return used_channel ;
}

void GetMemDataToSave( strcDataToSave *dataToSave, strcGlobalParameters *glbParam )
{
	dataToSave->nEventsSaved 	= -1 ;
    dataToSave->zr				= (double**) new double*[glbParam->nEventsAVG] ;
    dataToSave->pr				= (double**) new double*[glbParam->nEventsAVG] ;
    dataToSave->phMHz	 		= (double**) new double*[glbParam->nEventsAVG] ;
    dataToSave->pr2				= (double**) new double*[glbParam->nEventsAVG] ;
    dataToSave->pr2Mol			= (double**) new double*[glbParam->nEventsAVG] ;
    dataToSave->alphaAerCloud	= (double**) new double*[glbParam->nEventsAVG] ;

    dataToSave->zeniths			= (double*) new double[glbParam->nEventsAVG] ;	memset( dataToSave->zeniths     , 0, sizeof(double)*glbParam->nEventsAVG ) ;
    dataToSave->azimuth 		= (double*) new double[glbParam->nEventsAVG] ;	memset( dataToSave->azimuth     , 0, sizeof(double)*glbParam->nEventsAVG ) ;
    dataToSave->VAOD_Fernald	= (double*) new double[glbParam->nEventsAVG] ;	memset( dataToSave->VAOD_Fernald, 0, sizeof(double)*glbParam->nEventsAVG ) ;
    dataToSave->LR 				= (double*) new double[glbParam->nEventsAVG] ;	memset( dataToSave->LR			, 0, sizeof(double)*glbParam->nEventsAVG ) ;
    dataToSave->GPSTime 		= (int*)    new int   [glbParam->nEventsAVG] ;	memset( dataToSave->GPSTime 	, 0, sizeof(int)   *glbParam->nEventsAVG ) ;

	for ( int i=0 ; i<glbParam->nEventsAVG ; i++ )
	{
		dataToSave->zr[i]			 = (double*) new double[glbParam->nBins] ;	memset( dataToSave->zr[i]           , 0, sizeof(double)*glbParam->nBins ) ;
		dataToSave->pr[i] 	 	   	 = (double*) new double[glbParam->nBins] ;	memset( dataToSave->pr[i]           , 0, sizeof(double)*glbParam->nBins ) ;
		dataToSave->phMHz[i]		 = (double*) new double[glbParam->nBins] ;	memset( dataToSave->phMHz[i]        , 0, sizeof(double)*glbParam->nBins ) ;
		dataToSave->pr2[i]    		 = (double*) new double[glbParam->nBins] ;	memset( dataToSave->pr2[i]          , 0, sizeof(double)*glbParam->nBins ) ;
		dataToSave->pr2Mol[i] 		 = (double*) new double[glbParam->nBins] ;	memset( dataToSave->pr2Mol[i]       , 0, sizeof(double)*glbParam->nBins ) ;
		dataToSave->alphaAerCloud[i] = (double*) new double[glbParam->nBins] ;	memset( dataToSave->alphaAerCloud[i], 0, sizeof(double)*glbParam->nBins ) ;
	}
}

void FreeMemVectorsFernaldInversion( strcFernaldInversion *fernaldVectors )
{
	delete fernaldVectors->pr2n  	    ;
	delete fernaldVectors->phi  		;
	delete fernaldVectors->p 	   		;
	delete fernaldVectors->ip	   		;
	delete fernaldVectors->ipN   		;
	delete fernaldVectors->betaT 		;
	delete fernaldVectors->pr2Fit		;
	delete fernaldVectors->prFit		;
	delete fernaldVectors->intAlphaMol_r ;
}

void FernaldInversion( double *pr2, strcMolecularData *dataMol, strcGlobalParameters *glbParam, int indxRef, double ka, strcFernaldInversion *fernaldVectors, strcAerosolData *dataAer )
{
	cumtrapz( (double)glbParam->dr, (double*)dataMol->alphaMol, (int)glbParam->indxInitInversion, (int)glbParam->indxEndSig , fernaldVectors->intAlphaMol_r   ) ; // INTEGRALS ARE THRU SLANT PATH -> dr
	trapz	( (double)glbParam->dr, (double*)dataMol->alphaMol, (int)glbParam->indxInitInversion, (int)indxRef		        , (double*)&fernaldVectors->intAlphaMol_Ref ) ; // INTEGRALS ARE THRU SLANT PATH -> dr

// REFERENCE POINT CALCULATION
	char sigTypeInversionUsed[10];
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "sigTypeInversionUsed", "string" , (char*)sigTypeInversionUsed ) ;
	double pr2RefValue ;
	if ( strcmp(sigTypeInversionUsed, "Fil") ==0 )
	{
		pr2RefValue = pr2[indxRef] ;
	}
	else
	{
		strcFitParam	fitParam ;
		fitParam.indxInicFit = indxRef - 10 ;
		fitParam.indxEndFit  = indxRef + 10 ;
		fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInicFit +1 ;
			RayleighFit   ( (double*)pr2, (double*)dataMol->pr2Mol, dataMol->nBins, "wB", "NOTall", (strcFitParam*)&fitParam, (double*)fernaldVectors->pr2Fit ) ;
		pr2RefValue = fernaldVectors->pr2Fit[indxRef] ;
	}

	for ( int i=glbParam->indxInitInversion ; i < glbParam->nBins ; i++ )
	{
		fernaldVectors->pr2n[i] = pr2[i] / pr2RefValue ;
		fernaldVectors->phi[i]	= 2*((KM-ka)/ka) * (fernaldVectors->intAlphaMol_r[i] - fernaldVectors->intAlphaMol_Ref) ;
		fernaldVectors->p[i]   	= fernaldVectors->pr2n[i]*exp(-fernaldVectors->phi[i]) ;
	}
	cumtrapz( glbParam->dr, fernaldVectors->p, glbParam->indxInitInversion, (dataMol->nBins-1),  fernaldVectors->ip     ) ;
	trapz   ( glbParam->dr, fernaldVectors->p, glbParam->indxInitInversion, indxRef 		  , &fernaldVectors->ipNref ) ;
	for ( int i=glbParam->indxInitInversion ; i < glbParam->nBins ; i++ )
	{
		fernaldVectors->ipN[i] 	 = fernaldVectors->ip[i] - fernaldVectors->ipNref ;
		fernaldVectors->betaT[i] = fernaldVectors->p[i] / ( (1/dataMol->betaMol[indxRef]) - (2/ka)* fernaldVectors->ipN[i] ) ;

		dataAer->betaAer [i] = fernaldVectors->betaT[i] - dataMol->betaMol[i] ; // r
		dataAer->alphaAer[i] = dataAer->betaAer[i]/ka  ; // r
	}
	int 	spamAvgWin ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "spamAvgWin", "int", &spamAvgWin ) ;
	// smoothGSL( (double*)dataAer->alphaAer, (int)(glbParam->nBins), (int)spamAvgWin, (double*)dataAer->alphaAer ) ;
	smooth( (double*)dataAer->alphaAer, (int)0, (int)(glbParam->nBins-1), (int)spamAvgWin, (double*)dataAer->alphaAer ) ;
	for( int i=0 ; i <glbParam->indxInitSig ; i++ )
		dataAer->alphaAer[i] = dataAer->alphaAer[glbParam->indxInitSig] ;

// VAODr: VAOD PROFILE
		cumtrapz( dataMol->dzr, dataAer->alphaAer, 0, (glbParam->nBins -1), dataAer->VAODr ) ; // VAODr=VAODh
		// cumtrapz( dataMol->dzr, dataAer->alphaAer, 0, indxRef, dataAer->VAODr ) ; // VAODr=VAODh

	int 	VAODheigh ; // ALS: ABOVE LIDAR SITE
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "VAOD_HEIGH0"	, "int", (int*)&VAODheigh ) ; dataAer->aVAOD[0] = dataAer->VAODr[ (int)round((VAODheigh-glbParam->siteASL)/dataMol->dzr) ] ; // OK
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "VAOD_HEIGH1"	, "int", (int*)&VAODheigh ) ; dataAer->aVAOD[1] = dataAer->VAODr[ (int)round((VAODheigh-glbParam->siteASL)/dataMol->dzr) ] ; // OK
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "VAOD_HEIGH2"	, "int", (int*)&VAODheigh ) ; dataAer->aVAOD[2] = dataAer->VAODr[ (int)round((VAODheigh-glbParam->siteASL)/dataMol->dzr) ] ; // OK
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "VAOD_HEIGH3"	, "int", (int*)&VAODheigh ) ; dataAer->aVAOD[3] = dataAer->VAODr[ (int)round((VAODheigh-glbParam->siteASL)/dataMol->dzr) ] ; // OK
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "VAOD_HEIGH_TOT", "int", (int*)&VAODheigh ) ; dataAer->VAOD     = dataAer->VAODr[ (int)round((VAODheigh-glbParam->siteASL)/dataMol->dzr) ] ; // OK
	dataAer->SAOD  = dataAer->VAOD / cos( dataMol->zenith * PI/180 ) ;
}

void FernaldInversion_pr( double *pr, strcMolecularData *dataMol, strcGlobalParameters *glbParam, int indxRef, double ka, strcFernaldInversion *fernaldVectors, strcAerosolData *dataAer )
{
	cumtrapz( (double)glbParam->dr, (double*)dataMol->alphaMol, (int)glbParam->indxInitInversion, (int)glbParam->indxEndSig , fernaldVectors->intAlphaMol_r   ) ; // INTEGRALS ARE THRU SLANT PATH -> dr
	trapz	( (double)glbParam->dr, (double*)dataMol->alphaMol, (int)glbParam->indxInitInversion, (int)indxRef		        , (double*)&fernaldVectors->intAlphaMol_Ref ) ; // INTEGRALS ARE THRU SLANT PATH -> dr

// REFERENCE POINT CALCULATION
	char sigTypeInversionUsed[10];
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "sigTypeInversionUsed", "string" , (char*)sigTypeInversionUsed ) ;
	double prRefValue ;
	if ( strcmp(sigTypeInversionUsed, "Fil") ==0 )
		prRefValue = pr[indxRef] ;
	else
	{
		strcFitParam	fitParam ;
		fitParam.indxInicFit = indxRef - 10 ;
		fitParam.indxEndFit  = indxRef + 10 ;
		fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInicFit +1 ;
			RayleighFit( (double*)pr, (double*)dataMol->prMol, dataMol->nBins, "wB", "NOTall", (strcFitParam*)&fitParam, (double*)fernaldVectors->prFit ) ;
		prRefValue = fernaldVectors->prFit[indxRef] ;
	}
	double *prN = (double*) new double[ glbParam->nBins ] ;
	for( int b=0 ; b <glbParam->nBins ; b++ )
	{
		prN[b] = pr[b] /prRefValue ;
		fernaldVectors->pr2n[b] = prN[b] * pow( glbParam->r[b]/glbParam->r[indxRef], 2 ) ;
	}

	for ( int i=glbParam->indxInitInversion ; i < glbParam->nBins ; i++ )
	{
		fernaldVectors->phi[i]	= 2*((KM-ka)/ka) * (fernaldVectors->intAlphaMol_r[i] - fernaldVectors->intAlphaMol_Ref) ;
		fernaldVectors->p[i]   	= fernaldVectors->pr2n[i]*exp(-fernaldVectors->phi[i]) ;
	}
	cumtrapz( glbParam->dr, fernaldVectors->p, glbParam->indxInitInversion, (dataMol->nBins-1),  fernaldVectors->ip     ) ;
	trapz   ( glbParam->dr, fernaldVectors->p, glbParam->indxInitInversion, indxRef 		  , &fernaldVectors->ipNref ) ;
	for ( int i=glbParam->indxInitInversion ; i < glbParam->nBins ; i++ )
	{
		fernaldVectors->ipN[i] 	 = fernaldVectors->ip[i] - fernaldVectors->ipNref ;
		fernaldVectors->betaT[i] = fernaldVectors->p[i] / ( (1/dataMol->betaMol[indxRef]) - (2/ka)* fernaldVectors->ipN[i] ) ;

		dataAer->betaAer [i] = fernaldVectors->betaT[i] - dataMol->betaMol[i] ; // r
		dataAer->alphaAer[i] = dataAer->betaAer[i]/ka  ; // r
	}
	int 	spamAvgWin ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "spamAvgWin", "int", (int*)&spamAvgWin ) ;
	// smoothGSL( (double*)dataAer->alphaAer, (int)(glbParam->nBins), (int)spamAvgWin, (double*)dataAer->alphaAer ) ;
	smooth( (double*)dataAer->alphaAer, (int)0, (int)(glbParam->nBins-1), (int)spamAvgWin, (double*)dataAer->alphaAer ) ;
	for( int i=0 ; i <glbParam->indxInitInversion ; i++ ) // for( int i=0 ; i <glbParam->indxInitSig ; i++ )
		dataAer->alphaAer[i] = dataAer->alphaAer[glbParam->indxInitSig] ;

// VAODr: VAOD PROFILE
		cumtrapz( dataMol->dzr, dataAer->alphaAer, 0, (glbParam->nBins -1),  dataAer->VAODr ) ; // VAODr=VAODh
		trapz   ( glbParam->dr, dataAer->alphaAer, 0, indxRef             , &dataAer->SAOD  ) ; // VAODr=VAODh

	int 	VAODheigh ; // ALS: ABOVE LIDAR SITE
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "VAOD_HEIGH0"	, "int", (int*)&VAODheigh ) ; dataAer->aVAOD[0] = dataAer->VAODr[ (int)round((VAODheigh-glbParam->siteASL)/dataMol->dzr) ] ; // OK
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "VAOD_HEIGH1"	, "int", (int*)&VAODheigh ) ; dataAer->aVAOD[1] = dataAer->VAODr[ (int)round((VAODheigh-glbParam->siteASL)/dataMol->dzr) ] ; // OK
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "VAOD_HEIGH2"	, "int", (int*)&VAODheigh ) ; dataAer->aVAOD[2] = dataAer->VAODr[ (int)round((VAODheigh-glbParam->siteASL)/dataMol->dzr) ] ; // OK
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "VAOD_HEIGH3"	, "int", (int*)&VAODheigh ) ; dataAer->aVAOD[3] = dataAer->VAODr[ (int)round((VAODheigh-glbParam->siteASL)/dataMol->dzr) ] ; // OK
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "VAOD_HEIGH_TOT", "int", (int*)&VAODheigh ) ; dataAer->VAOD     = dataAer->VAODr[ (int)round((VAODheigh-glbParam->siteASL)/dataMol->dzr) ] ; // OK
}

void ProcessFernaldInversion( strcFernaldInversion *fernaldVectors, strcGlobalParameters *glbParam, strcMolecularData *dataMol, strcIndexMol *indxMol, strcLidarSignal *evSig, strcAerosolData *dataAer, strcErrorSignalSet *rndErrSigSet, strcErrorSignalSet *sysErrSigSet )
{
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "LR", "double" , (double*)&glbParam->LR ) ;
	glbParam->ka = (double)(1/glbParam->LR) ;
	printf("\nLR used: %d\n", (int)glbParam->LR) ;

	char sigTypeInversionUsed[10];
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "sigTypeInversionUsed", "string" , (char*)sigTypeInversionUsed ) ;
	int 	heightRef_Inversion_ASL ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "heightRef_Inversion_ASL" , "int" , (int*)&heightRef_Inversion_ASL ) ;
	if ( heightRef_Inversion_ASL >0 )
		indxMol->indxRef = (int)round( ( (heightRef_Inversion_ASL-glbParam->siteASL) /cos(dataMol->zenith*PI/180)) /glbParam->dr ) ;	

	if ( strcmp(sigTypeInversionUsed, "Fil") ==0 )
		FernaldInversion_pr( (double*)evSig->prGlued, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (int)indxMol->indxRef, (double)glbParam->ka, (strcFernaldInversion*)fernaldVectors, (strcAerosolData*)dataAer ) ;
	else
    	FernaldInversion_pr( (double*)evSig->pr		, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (int)indxMol->indxRef, (double)glbParam->ka, (strcFernaldInversion*)fernaldVectors, (strcAerosolData*)dataAer ) ;

// RANDOM ERROR CALCULATION
	// MonteCarloRandomError    ( (double*)evSig->pr2Glued, (double*)evSig->pr, (strcGlobalParameters*)glbParam, (strcMolecularData*)dataMol, (strcIndexMol*)indxMol, (strcFernaldInversion*)fernaldVectors, (strcErrorSignalSet*)rndErrSigSet ) ;
// SYSTEMATIC ERROR CALCULATION
	// MonteCarloSystematicError( (double*)evSig->pr2Glued,                     (strcGlobalParameters*)glbParam, (strcMolecularData*)dataMol, (strcIndexMol*)indxMol, (strcFernaldInversion*)fernaldVectors, (strcErrorSignalSet*)sysErrSigSet ) ;

} // ProcessFernaldInversion(...)

/* strcIndexMol Pre_Procesing_LidarSignal( .... )
 * BACKGROUND SUBSTRACTION
 * RANGE CORRECTED
 * FIND PURE MOLECULAR DATA. ablHeigh AND endRayleighFit
 * PRODUCE pr2Glued
 * */
void PreProcesingLidarSignal( strcGlobalParameters *glbParam, strcMolecularData *dataMol, strcLidarSignal *evSig, strcIndexMol *indxMol )
{
// BACKGROUND SUPRESSION FIT LIMITS
	strcFitParam	fitParam ;
		MakeRangeCorrected( (strcGlobalParameters*)glbParam, (strcMolecularData*)dataMol, (strcLidarSignal*)evSig )  ;
       	// LowRangeCorrection( (strcGlobalParameters*)glbParam, (double*)evSig->pr2 ) ;

	int 	spamAvgWin ;	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "spamAvgWin", "int", &spamAvgWin ) ;
	// smoothGSL( (double*)evSig->pr2, (int)(glbParam->nBins), (int)spamAvgWin, (double*)evSig->pr2 ) ;
	smooth( (double*)evSig->pr2, (int)0, (int)(glbParam->nBins-1), (int)spamAvgWin, (double*)evSig->pr2 ) ;

	glbParam->indxInitErr = glbParam->indxInitInversion ; // glbParam->indxInitSig ;

	char Rayleight_Fit_TYPE[10] ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "Rayleight_Fit_TYPE", "string", (char*)Rayleight_Fit_TYPE ) ;

//RAYLEIGH_FIT_ANALISYS:
for( int i=0 ; i<glbParam->nBins ; i++ )	evSig->pr2Glued[i] = evSig->pr2[i] ;

	indxMol->endRayFit = glbParam->siteASL + cos(dataMol->zenith*PI/180) * glbParam->r[ indxMol->indxEndMol [0] ] ; // ASL!!!!!!!!!!!

for ( int i=0 ; i<MAX_MOL_RANGES ; i++ ) // for ( int i=0 ; i<1 ; i++ ) // 
{ // FINAL FIT ACROSS MOLECULAR RANGES DETECTED
	if ( indxMol->indxInicMol[i] != 0 ) // IF THERE ARE MOLECULAR RANGES DETECTED...
	{
		fitParam.indxInicFit = indxMol->indxInicMol[i] ; // r 
		fitParam.indxEndFit  = indxMol->indxEndMol [i] ; // fitParam.indxInicFit +1000 ; // glbParam->indxEndSig ; // glbParam->nBins-1 ; //
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
// void PreProcesingLidarSignal( strcGlobalParameters *glbParam, strcMolecularData *dataMol, strcCloudProfiles *cloudProfiles, strcLidarSignal *evSig, strcIndexMol *indxMol )
// { // NNNNNNNNNNNNNNNNNNNNNOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO   REVISAR TODA
// 	double 	*p = (double*) new double [glbParam->nBins] ;
// 	for (int i=0 ; i <glbParam->nBins ; i++ )
// 		p[i] = evSig->pr[i] ;

// 	int 	spamAvgWin ;	ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "spamAvgWin", "int", (int*)&spamAvgWin ) ;
// 	// smoothGSL( (double*)p, (int)(glbParam->nBins), (int)spamAvgWin, (double*)evSig->pr ) ;
// 	// smoothGSL( (double*)evSig->pr, (int)(glbParam->nBins), (int)spamAvgWin, (double*)evSig->pr ) ;
// 	smooth( (double*)p, (int)0, (int)(glbParam->nBins-1), (int)spamAvgWin, (double*)evSig->pr ) ;
// 	delete p ;

// 	MakeRangeCorrected( (strcGlobalParameters*)glbParam, (strcMolecularData*)dataMol, (strcLidarSignal*)evSig ) ;
// 	LowRangeCorrection( (strcGlobalParameters*)glbParam, (double*)evSig->pr2 ) ;

// //RAYLEIGH_FIT_ANALISYS:
// for( int i=0 ; i<glbParam->nBins ; i++ )	evSig->pr2Glued[i] = evSig->pr2[i] ;

// 	memset( indxMol->indxInicMol, 0, sizeof(int)*MAX_MOL_RANGES ) ;
// 	memset( indxMol->indxEndMol , 0, sizeof(int)*MAX_MOL_RANGES ) ;
// 	memset( indxMol->errRMS	   , 0, sizeof(double) * glbParam->nBins ) ;
// 		FindMolecularRange_v2( (double*)evSig->pr2, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (strcIndexMol*)indxMol ) ;
// 		indxMol->ablHeight = glbParam->siteASL + cos(dataMol->zenith*PI/180) * glbParam->r[ indxMol->indxInicMol[0] ] ; // ASL!!!!!!!!!!!
// 		indxMol->endRayFit = glbParam->siteASL + cos(dataMol->zenith*PI/180) * glbParam->r[ indxMol->indxEndMol [0] ] ; // ASL!!!!!!!!!!!

// //for ( int i=0 ; i<MAX_MOL_RANGES ; i++ ) m[i] = 0 ;
// // BACKGROUND SUPRESSION FIT LIMITS
// strcFitParam	fitParam ;
// for ( int i=0 ; i<MAX_MOL_RANGES ; i++ ) // for ( int i=0 ; i<1 ; i++ ) // 
// { // FINAL FIT ACROSS MOLECULAR RANGES DETECTED
// 	if ( indxMol->indxInicMol[i] != 0 ) // IF THERE ARE MOLECULAR RANGES DETECTED...
// 	{
// // FIT_AGAIN:
// 		// SET THE FITTING PARAMETERS fitParam[] WITH THE MOLECULAR INDEXES FOUND IN FindMolecularRange(...). USED LATTER FOR THE RAYLEIGH-FIT METHOD
// 				fitParam.indxInicFit = indxMol->indxInicMol[i] ; // r 
// 				fitParam.indxEndFit  = indxMol->indxEndMol [i] ; // fitParam.indxInicFit +1000 ; // glbParam->indxEndSig ; // glbParam->nBins-1 ; //
// 				fitParam.nFit		 = fitParam.indxEndFit - fitParam.indxInicFit ;
// 			   RayleighFit( (double*)evSig->pr2, (double*)dataMol->pr2Mol, dataMol->nBins, "wOutB", "NOTall", (strcFitParam*)&fitParam, (double*)evSig->pr2Glued ) ;
// 			// RayleighFit_Factor( (double*)evSig->pr2, (double*)dataMol->pr2Mol, 						(strcFitParam*)&fitParam, (double*)evSig->pr2Glued ) ;
// 			//m[i] = fitParam.m ;

// 		  //// RAYLEIGH FIT CHECK
// 			// for( int j=glbParam->indxInitSig ; j<=fitParam.indxInicFit ; j++ )
// 			// {
// 			// 	sTest[j] = (evSig->pr2[j] - fitParam.m * dataMol->pr2Mol[j]) ;
// 			// }
// 			// 	sum( (double*)sTest, (int)glbParam->indxInitSig, (int)fitParam.indxInicFit, (double*)&sTestSum );
// 			// if ( sTestSum<0 ) // ( evSig->pr2Glued[fitParam.indxInicFit] > evSig->pr2Glued[fitParam.indxInicFit -1] )
// 			// {
// 			// // AGAIN: evSig->pr2Glued[i] = evSig->pr2[i] ACROSS FITTED VALUES
// 			// 	for( int g=fitParam.indxInicFit ; g<fitParam.indxEndFit ; g++ )		evSig->pr2Glued[g] = evSig->pr2[g] ;
// 			// 	if ( (indxMol->indxInicMol[i] +10) < (indxMol->indxEndMol[i]-500) )
// 			// 		{
// 			// 			indxMol->indxInicMol[i] = indxMol->indxInicMol[i] +10 ;
// 			// 			goto FIT_AGAIN ;
// 			// 		}
// 			// 	else
// 			// 		goto END_FIT ;
// 			// }
// 	//if (indxMol->nMolRanges >= 2)
// 	//{
// 		//if (m[1] > m[0])
// 		//{
// 			//glbParam->dNFit = glbParam->dNFit +10 ;
// 			//printf("\n Wrong fit performed before (%lf) and after (%lf) the cloud. Running the analisys again...", m[0], m[1]);
// 			//goto RAYLEIGH_FIT_ANALISYS ;
// 		//}
// 	//}
// 	}
// 	//glbParam->indxInitErr = glbParam->indxInitErr +10 ;
// //} while ( evSig->pr2Glued[fitParam.indxInicFit] >= evSig->pr2[fitParam.indxInicFit] ) ;

// // GLUE pr2 TO pr2Fit TO GENERATE THE FINAL VERSION OF THE SIGNAL TO PROCESS: pr2Glued
// 	//RayleighFit_Gluing( (double*)evSig->pr2, (double*)evSig->pr2Fit, fitParam.indxInicFit, (glbParam->nBins-1), (double*)evSig->pr2Glued ) ;
// 	//RayleighFit_Gluing_v2( (double*)evSig->pr2, (double*)evSig->pr2Fit, (strcFitParam)fitParam, (double*)evSig->pr2Glued ) ;
// }
// // END_FIT:
// 	for( int i=0 ; i<glbParam->nBins ; i++ )
// 	{
// 		evSig->prGlued[i] = evSig->pr2Glued[i] / pow(glbParam->r[i], 2) ;
// 		evSig->pr[i] 	  = evSig->pr2[i]      / pow(glbParam->r[i], 2) ;
// 	}


// 	if ( cloudProfiles->nClouds >=1 )
// 		indxMol->indxRef = (int) round( ( indxMol->indxInicMol[0] + cloudProfiles->indxInitClouds[0] )/2 ) ;
// 	else
// 		indxMol->indxRef = (int) (indxMol->indxInicMol[0] + 100) ;

// 	// return indxMol ;
// 	// delete sTestSum ;
// }

void MakeRangeCorrected( strcGlobalParameters *glbParam, strcMolecularData *dataMol, strcLidarSignal *evSig )
{
	strcFitParam	fitParam ;
	char 			BkgCorrMethod[10] ;

	fitParam.indxInicFit = glbParam->nBins - glbParam->nBinsBkg ; // glbParam->indxEndSig - glbParam->nBinsBkg ; // 
	fitParam.indxEndFit  = glbParam->nBins ; // glbParam->indxEndSig ; // 
	fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInicFit ;

	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "BkgCorrMethod", "string" , (char*)BkgCorrMethod ) ;

	if ( strcmp( BkgCorrMethod, "MEAN" ) ==0 )
	{
		bkgSubstractionMean( (double*)evSig->pr, fitParam.indxInicFit, fitParam.indxEndFit, dataMol->nBins, (double*)evSig->pr_noBkg ) ;
	}
	else if ( strcmp( BkgCorrMethod, "FIT" ) ==0 )
	{
	// // printf("\nMakeRangeCorrected ANTES de bkgSubstractionMolFit : \t dataMol->zr[1000]: %lf \t evSig->pr_noBkg[1000]: %lf", dataMol->zr[1000], evSig->pr_noBkg[1000] ) ;
		bkgSubstractionMolFit ( (strcMolecularData*)dataMol, (const double*)evSig->pr, (strcFitParam*)&fitParam, (double*)evSig->pr_noBkg ) ;
	// // printf("\nMakeRangeCorrected: DESPUES de bkgSubstractionMolFit: \t dataMol->zr[1000]: %lf \t evSig->pr_noBkg[1000]: %lf", dataMol->zr[1000], evSig->pr_noBkg[1000] ) ;
	//    	// bkgSubstractionMolFit_unCorr( (strcMolecularData*)&dataMol, (double*)evSig->pr, (strcFitParam*)&fitParam, (double*)evSig->pr_noBkg ) ;
	// 		//bkgSubstractionLinearFit 	( (double*)glbParam->r, (double*)evSig->pr, (int)glbParam->nBins, (strcFitParam*)&fitParam, (double*)evSig->pr_noBkg ) ;
	}
	else
	{
		printf("\n Wrong setting of 'Background Correction Method (BkgCorrMethod) in %s. Used FIT method' \n", (char*)glbParam->FILE_PARAMETERS) ;
		bkgSubstractionMolFit ( (strcMolecularData*)dataMol, (double*)evSig->pr, (strcFitParam*)&fitParam, (double*)evSig->pr_noBkg ) ;
	}
// RANGE CORRECTED
	for ( int i=0 ; i<glbParam->nBins ; i++ ) 	evSig->pr2[i] = evSig->pr_noBkg[i] * pow(glbParam->r[i], 2) ;
}

void MakeRangeCorrected_array( double *pr, strcGlobalParameters *glbParam, strcMolecularData *dataMol, double *pr2 )
{
	strcFitParam	fitParam ;
	char 			BkgCorrMethod[10] ;

	fitParam.indxInicFit = glbParam->indxEndSig - glbParam->nBinsBkg ;
	fitParam.indxEndFit  = glbParam->indxEndSig ;
	fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInicFit ;

	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "BkgCorrMethod", "string" , (char*)BkgCorrMethod ) ;

	if ( strcmp( BkgCorrMethod, "MEAN" ) ==0 )
		bkgSubstractionMean( (double*)pr, fitParam.indxInicFit, fitParam.indxEndFit, glbParam->nBins, (double*)pr ) ;
	else if ( strcmp( BkgCorrMethod, "FIT" ) ==0 )
		bkgSubstractionMolFit ( (strcMolecularData*)dataMol, (const double*)pr, (strcFitParam*)&fitParam, (double*)pr ) ;
	else
	{
		printf("\n Wrong setting of 'Background Correction Method (BkgCorrMethod) in %s. Used FIT method' \n", (char*)glbParam->FILE_PARAMETERS) ;
		bkgSubstractionMolFit ( (strcMolecularData*)dataMol, (double*)pr, (strcFitParam*)&fitParam, (double*)pr ) ;
	}
// RANGE CORRECTED
	for ( int i=0 ; i<glbParam->nBins ; i++ ) 	pr2[i] = pr[i] * pow(glbParam->r[i], 2) ;
}

void LowRangeCorrection( strcGlobalParameters *glbParam, double *sig )
{
    // double a = evSig->pr2[glbParam->indxInitSig] / evSig->pr2[ glbParam->indxInitSig *2 -1 ] ;
    double a = sig[glbParam->indxInitSig] / sig[ glbParam->indxInitSig *2 -1 ] ;

    for( int i=0 ; i<glbParam->indxInitSig ; i++ )
    {
    //a = pr2_532( glbParam.indxInitSig : (glbParam.indxInitSig + glbParam.indxInitSig-1), :) ;
    //pr2_532(1:glbParam.indxInitSig, :) = a .* (ones(glbParam.indxInitSig, 1) * a(1, :)./a(end, :) ) ;        
        // evSig->pr2[i] = evSig->pr2[i+glbParam->indxInitSig] * a ;
        sig[i] = sig[i+glbParam->indxInitSig] * a ;
    }
    glbParam->indxInitInversion = 0 ;
}

void MonteCarloRandomError( double *pr2Glued, double *pr, strcGlobalParameters *glbParam, strcMolecularData *dataMol, strcIndexMol *indxMol, strcFernaldInversion *fernaldVectors, strcErrorSignalSet *rndErrSigSet )
{
	strcAerosolData dataAerErr ;
	dataAerErr.alphaAer	= (double*) malloc( glbParam->nBins * sizeof(double) ) ;  memset( dataAerErr.alphaAer, 0, ( glbParam->nEventsAVG * sizeof(double) ) ) ;
	dataAerErr.betaAer	= (double*) malloc( glbParam->nBins * sizeof(double) ) ;  memset( dataAerErr.betaAer , 0, ( glbParam->nEventsAVG * sizeof(double) ) ) ;
	dataAerErr.VAODr	= (double*) malloc( glbParam->nBins * sizeof(double) ) ;  memset( dataAerErr.VAODr   , 0, ( glbParam->nEventsAVG * sizeof(double) ) ) ;

	int 	nSigSetErr ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "nSigSetErr", "int" , (int*)&nSigSetErr ) ;
// GET THE STANDAR DEVIATION TO GENERATE THE RANDOM ERROR IN THE LIDAR SIGNALS SET
	strcFitParam	fitParam ;
	fitParam.indxInicFit = dataMol->nBins - glbParam->nBinsBkg  	  ;
	fitParam.indxEndFit  = dataMol->nBins -1 						  ;
	fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInicFit ;
	double *prFit = (double*) new double[glbParam->nBins] ;
	// gsl_fit_linear( &dataMol->prMol[fitParam.indxInicFit], 1, &pr[fitParam.indxInicFit], 1, fitParam.nFit, &fitParam.b, &fitParam.m, &fitParam.cov00, &fitParam.cov01, &fitParam.cov11, &fitParam.sumsq_m ) ;
		RayleighFit( (double*)&dataMol->prMol[fitParam.indxInicFit], (double*)&pr[fitParam.indxInicFit], fitParam.nFit, "wB", "NOTall", (strcFitParam*)&fitParam, (double*)prFit ) ;
	delete prFit ;	
	double 	stdPr = sqrt(fitParam.sumsq_m/(fitParam.nFit-1)) ;
	int 	spamAvgWin ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "spamAvgWin", "int", (int*)&spamAvgWin ) ;
	for( int j=0 ; j < nSigSetErr ; j++ )
	{
		for ( int i=0 ; i < glbParam->nBins ; i++ )
		{
			rndErrSigSet->prGlued [j][i] = pr2Glued[i] / (glbParam->r[i]*glbParam->r[i]) ;
			rndErrSigSet->prNoisy [j][i] = RandN2( rndErrSigSet->prGlued[j][i], stdPr ) ;
			rndErrSigSet->pr2Noisy[j][i] = rndErrSigSet->prNoisy[j][i] * (glbParam->r[i]*glbParam->r[i]) ;
		}
		// smoothGSL( (double*)rndErrSigSet->pr2Noisy[j], (int)(glbParam->nBins), (int)spamAvgWin, (double*)rndErrSigSet->pr2Noisy[j] ) ;
		smooth( (double*)rndErrSigSet->pr2Noisy[j], (int)0, (int)(glbParam->nBins-1), (int)spamAvgWin, (double*)rndErrSigSet->pr2Noisy[j] ) ;
// FERNALD INVERSION
	FernaldInversion( (double*)rndErrSigSet->pr2Noisy[j], (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (int)indxMol->indxInicMol[0]+110, (double)glbParam->ka, (strcFernaldInversion*)fernaldVectors, (strcAerosolData*)&dataAerErr ) ;
		for( int b=0 ; b<glbParam->nBins ; b++ )
		{
			rndErrSigSet->alphaAer_ErrSet[j][b] = dataAerErr.alphaAer[b] ;
			rndErrSigSet->   VAODr_ErrSet[j][b] = dataAerErr.VAODr   [b] ;
		}
	}
// OBTAIN MEAN AND STANDAR DEVIATION
		GetErrSetParam( (char*)glbParam->FILE_PARAMETERS, (int)nSigSetErr, (int)glbParam->nBins, (double)dataMol->dzr, (strcErrorSignalSet*)rndErrSigSet ) ;
}

void MonteCarloSystematicError( double *pr2Glued, strcGlobalParameters *glbParam, strcMolecularData *dataMol, strcIndexMol *indxMol, strcFernaldInversion *fernaldVectors, strcErrorSignalSet *sysErrSigSet )
{
	strcAerosolData dataAerErr ;
	dataAerErr.alphaAer	= (double*) malloc( glbParam->nBins * sizeof(double) ) ;
	dataAerErr.betaAer	= (double*) malloc( glbParam->nBins * sizeof(double) ) ;
	dataAerErr.VAODr	= (double*) malloc( glbParam->nBins * sizeof(double) ) ;

	double 	LR_init, LR_end, LR_step ;
		ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "LR_init", "double" , (double*)&LR_init ) ;
		ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "LR_end" , "double" , (double*)&LR_end  ) ;
	int 	nSigSetErr ;
		ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "nSigSetErr", "int" , (int*)&nSigSetErr ) ;

	double 	*ka_ite = (double*) malloc( nSigSetErr * sizeof(double) ) ;
	LR_step = (double) 1 ; // ((LR_end - LR_init) /nSigSetErr) ;
	for ( int j=0 ; j<nSigSetErr ; j++ )		ka_ite[j] = (double) 1/(LR_init + j*LR_step) ;
//	double ka_ite[10] = { 0.015, 0.018, 0.020, 0.025, 0.03, 0.04, 0.05, 0.06, 0.07, 0.08 } ;

	for( int j=0 ; j<nSigSetErr ; j++ )
	{
// FERNALD INVERSION
	FernaldInversion( (double*)pr2Glued, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (int)indxMol->indxInicMol[0]+110, (double)ka_ite[j], (strcFernaldInversion*)fernaldVectors , (strcAerosolData*)&dataAerErr ) ;
	//FernaldInversion( (double*)pr2Glued, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (int)indxMol->indxInicMol[0]+110, (double)ka_ite[j], (strcAerosolData*)&dataAerErr ) ;
		for( int b=0 ; b<glbParam->nBins ; b++ )
		{
			sysErrSigSet->alphaAer_ErrSet[j][b] = dataAerErr.alphaAer[b] ;
			sysErrSigSet->   VAODr_ErrSet[j][b] = dataAerErr.VAODr   [b] ;
		}
	}
// OBTAIN MEAN AND STANDAR DEVIATION
	GetErrSetParam( (char*)glbParam->FILE_PARAMETERS, (int)nSigSetErr, (int)glbParam->nBins, (double)dataMol->dzr, (strcErrorSignalSet*)sysErrSigSet ) ;

// FREE MEM FROM fernaldVectors
	//FreeMemVectorsFernaldInversion( (strcFernaldInversion*)&fernaldVectors ) ;
}

void GetErrSetParam( char *FILE_PARAMETERS, int nSigSetErr, int nBins, double dzr, strcErrorSignalSet *errSigSet )
{
	float 	VAODheigh ;
// r_meanErrSet
	memset( errSigSet->alphaAer_meanErrSet, 0, sizeof(double) * nBins ) ;
	memset( errSigSet->VAODr_meanErrSet   , 0, sizeof(double) * nBins ) ;
		for ( int b=0 ; b<nBins ; b++ )
		{
			for ( int j=0 ; j < nSigSetErr ; j++ )
			{
				errSigSet->alphaAer_meanErrSet[b] = errSigSet->alphaAer_meanErrSet[b] + errSigSet->alphaAer_ErrSet[j][b] ;
				errSigSet->   VAODr_meanErrSet[b] = errSigSet->   VAODr_meanErrSet[b] + errSigSet->   VAODr_ErrSet[j][b] ;
			}
			errSigSet->alphaAer_meanErrSet[b] = errSigSet->alphaAer_meanErrSet[b] / nSigSetErr ;
			errSigSet->   VAODr_meanErrSet[b] = errSigSet->   VAODr_meanErrSet[b] / nSigSetErr ;
		}
// VAODmean OF THE DATASET AT DIFFERENT HEIGHTS
	ReadAnalisysParameter( (const char*)FILE_PARAMETERS, "VAOD_HEIGH0", "float", (float*)&VAODheigh ) ;		errSigSet->hVAOD_meanErrSet[0] = errSigSet->VAODr_meanErrSet[(int)round(VAODheigh/dzr)] ;
	ReadAnalisysParameter( (const char*)FILE_PARAMETERS, "VAOD_HEIGH1", "float", (float*)&VAODheigh ) ;		errSigSet->hVAOD_meanErrSet[1] = errSigSet->VAODr_meanErrSet[(int)round(VAODheigh/dzr)] ;
	ReadAnalisysParameter( (const char*)FILE_PARAMETERS, "VAOD_HEIGH2", "float", (float*)&VAODheigh ) ;		errSigSet->hVAOD_meanErrSet[2] = errSigSet->VAODr_meanErrSet[(int)round(VAODheigh/dzr)] ;
	ReadAnalisysParameter( (const char*)FILE_PARAMETERS, "VAOD_HEIGH3", "float", (float*)&VAODheigh ) ;		errSigSet->hVAOD_meanErrSet[3] = errSigSet->VAODr_meanErrSet[(int)round(VAODheigh/dzr)] ;

// r_stdErrSet STANDAR ERROR MEAN OF THE DATASET AT DIFFERENT HEIGHTS
		memset( errSigSet->alphaAer_stdErrSet, 0, sizeof(double) * nBins ) ;
		memset( errSigSet->VAODr_stdErrSet   , 0, sizeof(double) * nBins ) ;
		for ( int b=0 ; b<nBins ; b++ )
		{
			for ( int j=0 ; j < nSigSetErr ; j++ )
			{
				errSigSet->alphaAer_stdErrSet[b] = errSigSet->alphaAer_stdErrSet[b] + pow( (errSigSet->alphaAer_ErrSet[j][b] - errSigSet->alphaAer_meanErrSet[b]), 2) ;
				errSigSet->   VAODr_stdErrSet[b] = errSigSet->   VAODr_stdErrSet[b] + pow( (errSigSet->   VAODr_ErrSet[j][b] - errSigSet->   VAODr_meanErrSet[b]), 2) ;
			}
			errSigSet->alphaAer_stdErrSet[b] = sqrt(errSigSet->alphaAer_stdErrSet[b] / (nSigSetErr-1) ) ;
			errSigSet->   VAODr_stdErrSet[b] = sqrt(errSigSet->   VAODr_stdErrSet[b] / (nSigSetErr-1) ) ;
		}
// VAODstd OF THE DATASET
	ReadAnalisysParameter( (const char*)FILE_PARAMETERS, "VAOD_HEIGH0", "float", (float*)&VAODheigh ) ;		errSigSet->hVAOD_stdErrSet[0] = errSigSet->VAODr_stdErrSet[(int)round(VAODheigh/dzr )] ;
	ReadAnalisysParameter( (const char*)FILE_PARAMETERS, "VAOD_HEIGH1", "float", (float*)&VAODheigh ) ;		errSigSet->hVAOD_stdErrSet[1] = errSigSet->VAODr_stdErrSet[(int)round(VAODheigh/dzr )] ;
	ReadAnalisysParameter( (const char*)FILE_PARAMETERS, "VAOD_HEIGH2", "float", (float*)&VAODheigh ) ;		errSigSet->hVAOD_stdErrSet[2] = errSigSet->VAODr_stdErrSet[(int)round(VAODheigh/dzr )] ;
	ReadAnalisysParameter( (const char*)FILE_PARAMETERS, "VAOD_HEIGH3", "float", (float*)&VAODheigh ) ;		errSigSet->hVAOD_stdErrSet[3] = errSigSet->VAODr_stdErrSet[(int)round(VAODheigh/dzr )] ;
}

void bkgSubstractionMolFit (strcMolecularData *dataMol, const double *prEl, strcFitParam *fitParam, double *pr_noBkg)
{
	// cout<<"----------- bkgSubstractionMolFit" ;
	// printf("\n\nfitParam->indxInicFit: %d\nfitParam->indxEndFit: %d\nfitParam->nFit: %d\n\n", fitParam->indxInicFit, fitParam->indxEndFit, fitParam->nFit) ;

	double *dummy = (double*) new double[dataMol->nBins] ; 
	RayleighFit( (double*)prEl, (double*)dataMol->prMol, dataMol->nBins , "wB", "NOTall", (strcFitParam*)fitParam, (double*)dummy ) ;
		for ( int i=0 ; i<dataMol->nBins ; i++ ) 	pr_noBkg[i] = (double)(prEl[i] - fitParam->b) ; 

	// printf( "\nbkgSubstractionMolFit() --> fitParam->b: %lf \nfitParam->indxInicFit: %d \nfitParam->indxInicFit: %d \n", fitParam->b, fitParam->indxInicFit, fitParam->indxEndFit ) ;

	delete dummy ;
}

void bkgSubstractionMolFit_unCorr( strcMolecularData *dataMol, double *prEl, strcFitParam *fitParam, double *pr)
{ // 
// GET m AND b UNCORRELATED (m IS SLIGTHLY CORRELATED WITH b)

	double 	*prDiff	= (double*) malloc( dataMol->nBins * sizeof(double) ) ;
	diffPr( prEl, dataMol->nBins, prDiff ) ;
	double 	*prMolDiff	= (double*) malloc( dataMol->nBins * sizeof(double) ) ;
	diffPr( dataMol->prMol, dataMol->nBins, prMolDiff ) ;

	// GET m
	double 	*prMolDiffFit	= (double*) malloc( dataMol->nBins * sizeof(double) ) ;
	RayleighFit( prDiff, prMolDiff, dataMol->nBins, "wOutB", "NOTall", fitParam, prMolDiffFit ) ;

	double 	*m_prMol	= (double*) malloc( dataMol->nBins * sizeof(double) ) ;
	for( int j=0 ; j<dataMol->nBins ; j++ )		m_prMol[j] = dataMol->prMol[j] * fitParam->m ;
	// GET b
	RayleighFit( (double*)prEl, (double*)m_prMol, (int)dataMol->nBins, "wB", "NOTall", fitParam, (double*)pr ) ;

		for ( int i=0 ; i<dataMol->nBins ; i++ ) 	pr[i] = (double)( prEl[i] - fitParam->b ) ;

	free(prDiff) 		;
	free(prMolDiff)		;
	free(prMolDiffFit) 	;
	free(m_prMol) 		;
}

double bkgSubstractionMean( double *sig, int binInitMean, int binEndMean, int nBins, double *pr_noBkg)
{
	int nFit = binEndMean - binInitMean ;

	double 	bkgMean = 0 ;

	for( int j=binInitMean ; j<binEndMean ; j++ ) bkgMean = bkgMean + sig[j] ;

	bkgMean = bkgMean /nFit ;

	for ( int i=0 ; i<nBins ; i++ ) 	pr_noBkg[i] = (double)(sig[i] - bkgMean) ;

	// printf( "\n bkgMean: %lf \n", bkgMean ) ;

	return bkgMean ;
}

// void bkgSubstractionLinearFit ( double *r, double *prEl, int nBins, strcFitParam *fitParam, double *pr)
// {
// // GET m AND b WITH A LINEAR FIT IN (CORRELATED)

// 	int binInitFit = (int) fitParam->indxInicFit ;
// 	int nFit = fitParam->nFit ;

// 	gsl_fit_linear( &r[binInitFit], 1, &prEl[binInitFit], 1, nFit, &fitParam->b, &fitParam->m, &fitParam->cov00, &fitParam->cov01, &fitParam->cov11, &fitParam->sumsq_m ) ;
// 	// RayleighFit( (double*)&dataMol->prMol[fitParam->indxInicFit], (double*)&prEl[fitParam->indxInicFit], dataMol->nBins, "wB", "NOTall", (strcFitParam*)fitParam, (double*)pr ) ;

// 		for ( int i=0 ; i<nBins ; i++ ) 	pr[i] = (double)(prEl[i] - ( fitParam->m * r[i] + fitParam->b ) ) ;
// }

void FindMolecularRange_v2( double *pr2, strcMolecularData *dataMol, strcGlobalParameters *glbParam, strcIndexMol *indxMol )
{
	double *pr2Fil 	= (double*) new double [dataMol->nBins] ;
	int 			indxInitErr = glbParam->indxInitErr ;
	int 			indxEndErr 	= glbParam->indxEndSig  ;
	int	*diffIndxMolON = (int*) new int[glbParam->nBins] ;

	strcFitParam	fitParam ;
	double 	drFit ;		ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "drFit", "double", (double*)&drFit ) ;
	int 	dNfit[1] ; 
	//for ( int i=0 ; i<NUMELEM(dNfit) ; i++ )		dNfit[i] = (int) round(dZfit[i]/glbParam->dr); // IN CASE OF USING DIFFERENTS drFit
	dNfit[0] = (int) round(drFit/glbParam->dr);

// errRMS(r) COMPUTATION
for ( int idrf = 0 ; idrf<(int)NUMELEM(dNfit) ; idrf++ )	
{
	for( int i=indxInitErr ; i<=indxEndErr ; i++ )
	{
		fitParam.indxInicFit = i ;
		fitParam.indxEndFit  = fitParam.indxInicFit + dNfit[idrf] ;
		fitParam.nFit		 = fitParam.indxEndFit  - fitParam.indxInicFit ;
			RayleighFit( (double*)pr2, (double*)dataMol->pr2Mol, dataMol->nBins, "wOutB", "NOTall", (strcFitParam*)&fitParam, (double*)pr2Fil ) ;

		indxMol->errRMS[i]   = fitParam.sumsq_m ; // r
	}
	double maxErr ;
	int indxMaxErr = 0 ;
	findIndxMax( (double*)indxMol->errRMS, indxInitErr, indxEndErr, (int*)&indxMaxErr, (double*)&maxErr ) ;
// UNUSED RANGES OF errRMS
	for( int i=0 ; i<indxInitErr ; i++ )
		indxMol->errRMS[i] = maxErr ;
	for( int i=(indxEndErr+1) ; i<(dataMol->nBins) ; i++ )
		indxMol->errRMS[i] = maxErr ;

// SET THE MOLECULAR BINS *ON* ACROSS THE BINS indxMinErr-indxEndErr
	double min ;
	int indxMinErr = 0 ;
//  for( int i=indxInitErr ; (i+dNfit[idrf])<=indxEndErr ; i=(indxMinErr+dNfit-1) ) // r[i]
	for( int i=indxInitErr ; (i+dNfit[idrf])<=indxEndErr ; i++ ) // r[i]     =(indxMinErr+ (int)round(dNfit/2) )
	{
		findIndxMin( indxMol->errRMS, i, indxEndErr, &indxMinErr, &min ) ; // indxMinErr REFERENCED TO [0-nBins]
			for( int b=0 ; b<dNfit[idrf] ; b++ ) // indxMol->indxMolON[i+indxMinErr+b] = MOL_ON ; // INDEX REFERENCED TO [0-(nBins-1)]
				indxMol->indxMolON[indxMinErr+b] = indxMol->indxMolON[indxMinErr+b] + MOL_ON ; // INDEX REFERENCED TO [0-(nBins-1)] ACROSS r
	}
	indxMol->indxMolON[glbParam->nBins-1] = 0 ;
} // for ( int idrf = 0 ; idrf<10 ; idrf++ )

	for( int i=0 ; i<glbParam->nBins ; i++ )
	{
		if ( indxMol->indxMolON[i] >=1 ) // IF MORE THAN 1 TIMES, THE RANGE WAS MARQUED AS PURE MOLECULAR --> ITS MOLECULAR
			indxMol->indxMolON[i] = MOL_ON ;
		else
			indxMol->indxMolON[i] = 0 ;
	}
// GET THE MOLECULAR RANGES LIMITS
	for ( int b=0 ; b<(glbParam->nBins-2) ; b++ )
	{ // diffIndxMolON=1 -> START MOL RANGE    diffIndxMolON=-1 -> END MOL RANGE
		diffIndxMolON[b] = 0 ;
		diffIndxMolON[b] = (int)(indxMol->indxMolON[b+1] - indxMol->indxMolON[b]) ; // DIFF
	}
// diffIndxMolON CONSISTENCY
	int inicMolCount=0, endMolCount=0 ;
	for ( int i=0 ; i <indxEndErr ; i++ )
	{
		if ( diffIndxMolON[i] > 0 )	inicMolCount++ ;
		if ( diffIndxMolON[i] < 0 )	endMolCount++ ;
	}
	if ( inicMolCount != endMolCount ) // SHOULD BE: inicMolCount = endMolCount. IF NOT...
	{ // IN THIS CASE, SHOULD BE:    inicMolCount +1 = endMolCount
		endMolCount++ ;
		diffIndxMolON[indxEndErr] = -MOL_ON ; // diffIndxMolON[indxEndErr] DEFINED AS THE END OF THE MOLECULAR RANGE
	}

// FIX THE GAP BETWEEN TWO CONSECUTIVES MOLECULAR RANGES LOWER THAN MOL_BIN_GAP BINS.
	//for ( int i=0 ; i <indxEndErr ; i++ )
	//{
		//if ( diffIndxMolON[i] < 0 )
		//{
			//for( int j=i ; j<(i+MOL_BIN_GAP) ; j++ ) // MOL_BIN_GAP=200 200*7.5 = 1400 m
			//{
				//if ( diffIndxMolON[j] > 0 )
				//{
					//diffIndxMolON[i] = 0 ;
					//diffIndxMolON[j] = 0 ;
				//}
			//}
		//}
	//}
// GET INIC/END MOLECULAR RANGES INDEXES AND nMolRanges
	indxMol->nMolRanges = 0 ;
	for ( int b=0 ; b <= indxEndErr ; b++ ) // r[b] --> r[ indxInicMol[...] ]
	{
		if ( diffIndxMolON[b] > 0 )
			indxMol->indxInicMol[indxMol->nMolRanges] = b+1 ;
		if ( diffIndxMolON[b] < 0 )
		{
			indxMol->indxEndMol[indxMol->nMolRanges] = b ;
			indxMol->nMolRanges++ ;
		}
	}
	// delete 	pr2Fil	;
}


// void RayleighFit_GSL( double *sig, double *sigMol, int nBins, const char *modeBkg, const char *modeRangesFit, strcFitParam *fitParam, double *sigFil )
// {
// 	fitParam->sumsq_m = 0  ;

// 	if ( strcmp( modeBkg, "wB" ) == 0 )
// 	{
// 		gsl_fit_linear( (double*)&sigMol[fitParam->indxInicFit], 1, (double*)&sig[fitParam->indxInicFit], 1, fitParam->nFit,
// 						(double*)&(fitParam->b), (double*)&(fitParam->m), (double*)&(fitParam->cov00), (double*)&(fitParam->cov01), (double*)&(fitParam->cov11), (double*)&(fitParam->sumsq_m) ) ;
// 		if ( strcmp( modeRangesFit, "all" ) == 0 )
// 		{
// 			for ( int i=0 ; i < nBins ; i++ )
// 			{
// 				sigFil[i] = (double) ( sigMol[i] * fitParam->m + fitParam->b ) ;
// 				fitParam->sumsq_m = fitParam->sumsq_m + pow( (sigFil[i] - sig[i]), 2 ) ;
// 			}
// 		}
// 		else
// 		{
// 			for ( int i=fitParam->indxInicFit ; i <=fitParam->indxEndFit ; i++ )
// 			{
// 				sigFil[i] = (double) ( sigMol[i] * fitParam->m + fitParam->b ) ;
// 				fitParam->sumsq_m = fitParam->sumsq_m + pow( (sigFil[i] - sig[i]), 2 ) ;
// 			}
// 		}
// 	}
// 	else if ( strcmp( modeBkg, "wOutB" ) == 0 )
// 	{
// 		gsl_fit_mul ( (double*)&sigMol[fitParam->indxInicFit], 1, (double*)&sig[fitParam->indxInicFit], 1, (size_t)fitParam->nFit,
// 					  (double*)&fitParam->m, (double*)&fitParam->cov11, (double*)&fitParam->sumsq_m ) ;

// 		if ( strcmp( modeRangesFit, "all" ) == 0 )
// 		{
// 			for ( int i=0 ; i < nBins ; i++ )
// 			{
// 				sigFil[i] = (double) ( sigMol[i] * fitParam->m ) ;
// 				fitParam->sumsq_m = fitParam->sumsq_m + pow( (sigFil[i] - sig[i]), 2 ) ;
// 			}
// 		}
// 		else
// 		{
// 			for ( int i=fitParam->indxInicFit ; i <=fitParam->indxEndFit ; i++ )
// 			{
// 				sigFil[i] = (double) ( sigMol[i] * fitParam->m ) ;
// 				fitParam->sumsq_m = fitParam->sumsq_m + pow( (sigFil[i] - sig[i]), 2 ) ;
// 			}
// 		}
// 	}
// }

void RayleighFit_Factor( double *sig, double *sigMol, strcFitParam *fitParam, double *sigFil_Factor )
{ // PASTE MOLECULAR PROFILE IN THE RANGES DEFINED BY fitParam (DOES'T NOT FIT!!)

	fitParam->f = (double)sig[fitParam->indxInicFit]/sigMol[fitParam->indxInicFit] ;
	fitParam->sumsq_f = 0 ;

	for ( int i=fitParam->indxInicFit ; i <= fitParam->indxEndFit ; i++ )
	{
		sigFil_Factor[i] = (double) (sigMol[i] * fitParam->f ) ;
		fitParam->sumsq_f = fitParam->sumsq_f + pow( (sig[i] - sigFil_Factor[i]), 2 ) ;
	}
	fitParam->sumsq_f = sqrt(fitParam->sumsq_f) ;
}

// void R_ayleighFit_unCorr( double *prEl, double *prMol, int nBins, const char *modeBkg, strcFitParam *fitParam, double *sigFil )
// {
// /*
//  * m AND b UNCORRELATED, OBTAINED WITH ITS OWN ECUATIONS
// */
// 	// GET m USING THE DIFFERENTIALS VECTORS OF pr AND prMol
// 	double 	*prDiff			= (double*) malloc( nBins * sizeof(double) ) ;
// 	double 	*prMolDiff		= (double*) malloc( nBins * sizeof(double) ) ;
// 	double 	mNum, mDen, bNum, bDen ;
// 	diffPr( prEl , nBins, prDiff ) ;
// 	diffPr( prMol, nBins, prMolDiff ) ;

// 	mNum = 0 ;	mDen = 0 ;
// 	bNum = 0 ;	bDen = 0 ;

// 	for ( int i=fitParam->indxInicFit ; i<fitParam->indxEndFit ; i++ )
// 	{
// 			mNum = mNum + prDiff[i]    * prMolDiff[i] ;
// 			mDen = mDen + prMolDiff[i] * prMolDiff[i] ;
// 	}
// 			fitParam->m =  mNum / mDen ;

// 	double 	*m_prMol	= (double*) malloc( nBins * sizeof(double) ) ;
// 	for( int i=0 ; i<nBins ; i++ )		m_prMol[i] = fitParam->m * prMol[i] ;

// 	for ( int i=fitParam->indxInicFit ; i<fitParam->indxEndFit ; i++ )		bNum = bNum + m_prMol[i] - prEl[i] ;

// 	bDen 		= fitParam->nFit ;
// 	fitParam->b = - bNum / bDen   ;

// 	if ( strcmp( modeBkg, "wB" ) == 0 )
// 		for ( int i=0 ; i < nBins ; i++ )
// 			sigFil[i] = (double) ( m_prMol[i] + fitParam->b ) ;
// 	else if ( strcmp( modeBkg, "wOutB" ) == 0 )
// 		for ( int i=0 ; i < nBins ; i++ )
// 			sigFil[i] = (double) m_prMol[i] ;

// 	free(prDiff) 		;
// 	free(prMolDiff)		;
// }

// // GET THE RAYLEIGH-FIT AND ITS PARAMETERS ACROSS [ fitParam->indxInicFit - fitParam->indxEndFit ]
// void R_ayleighFit_M( double *sig, double *sigMol, int nBins, strcFitParam *fitParam, double *sigFil )
// {
// 	double	mNum=0, mDen=0 ;

// 	for ( int i=fitParam->indxInicFit ; i <= fitParam->indxEndFit ; i++ )
// 	{
// 		mNum = mNum + sig   [i]*sigMol[i] ;
// 		mDen = mDen + sigMol[i]*sigMol[i] ;
// 	}
// 	fitParam->m = mNum/mDen ;
// 	fitParam->b = 0     	;

// 	for ( int i=0 ; i <nBins ; i++ ) // for ( int i=fitParam->indxInicFit ; i <nBins ; i++ )
// 	{
// 		sigFil[i] 		= (double) ( sigMol[i] * fitParam->m ) ;
// 	}
// 	fitParam->sumsq_m = 0 ;
// 	for ( int i=fitParam->indxInicFit ; i <= fitParam->indxEndFit ; i++ )
// 	{
// 		fitParam->sumsq_m = fitParam->sumsq_m + pow( (sig[i] - sigFil[i]), 2 ) ;
// 	}
// }




// void GetCloudLimits( strcGlobalParameters *glbParam, strcMolecularData *dataMol, strcCloudProfiles *cloudProfiles, strcIndexMol *indxMol )
// {
// 	double *dco   = (double*) new double[glbParam->nBins] ;
// 	memset( (double*)dco  , 0, (sizeof(double) * glbParam->nBins) ) ;
// 	for ( int i = 0 ; i<=glbParam->indxEndSig ; i++ )
// 		dco[i] = (double)( cloudProfiles->clouds_ON[i+1] - cloudProfiles->clouds_ON[i] ) ;
// 	cloudProfiles->nClouds  = 0 ; // USED AS INDEX AND THEN, AS TOTAL NUMBER.
// 	indxMol->nMolRanges 	= 0    ; // USED AS INDEX AND THEN, AS TOTAL NUMBER.

// 	int DELTA_RANGE_LIM_BINS ;
// 	ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "DELTA_RANGE_LIM_BINS", "int", (int*)&DELTA_RANGE_LIM_BINS ) ;

// 	// int initCloudCounter=0, endCloudCounter=0 ;

// 	for( int i=0 ; i <=(glbParam->indxEndSig-1) ; i++ )
// 	{
// 		if( dco[i] == BIN_CLOUD ) // INIT CLOUD --- END MOLECULAR RANGE
// 		{
// 			indxMol->indxEndMol[indxMol->nMolRanges] = i -1 	; // indxMol->indxEndMol[initCloudCounter] 			= i -1 	;
// 			indxMol->nMolRanges++ ; // COUNT MOLECULAR RANGES WHEN IT END.

// 			cloudProfiles->indxInitClouds[cloudProfiles->nClouds] = i 	; // cloudProfiles->indxInitClouds[initCloudCounter] = i 	;
// 			cloudProfiles->nClouds++ ; // COUNT CLOUDS WHEN IT START
// 		}
// 		else if( dco[i] == (-BIN_CLOUD) ) // END CLOUD --- INIT MOLECULAR RANGE
// 		{
// 			indxMol->indxInicMol[indxMol->nMolRanges] = i +1 ; // cloudProfiles->indxEndClouds[endCloudCounter] +1 ;

// 			if ( cloudProfiles->nClouds >0 )
// 			{
// 				cloudProfiles->indxEndClouds[cloudProfiles->nClouds-1] = i ;
// 				cloudProfiles->nClouds++ ;
// 			}
// 		}
// 	}
// 		cloudProfiles->nClouds = cloudProfiles->nClouds -2;
// 		indxMol->nMolRanges --;
// 		// cloudProfiles->indxInitClouds[initCloudCounter-1] = 0 ; // RESET THE LAST CLOUD INDEX INITIAL.
// 		// initCloudCounter = initCloudCounter -1 ; // initCloudCounter -2 ; // ABL AND END OF THE SIGNAL.
// 		// endCloudCounter  = endCloudCounter  -1 ; // ABL SUBSTRACTED.

// 	if ( cloudProfiles->nClouds ==0 )
// 	{
// 		for (int i =0; i < NMAXCLOUDS ; i++)
// 		{
// 			cloudProfiles->indxInitClouds[i] = 0 ;
// 			cloudProfiles->indxEndClouds [i] = 0 ;
// 			indxMol->indxInicMol[i] = glbParam->indxInitSig ;
// 			indxMol->indxEndMol[i]  = glbParam->indxEndSig  ;
// 		}
// 	}

// 	// printf("\n Evnt: %d \t initCloudCounter: %d \t endCloudCounter: %d \t cloudProfiles->nClouds: %d", glbParam->event_analyzed, initCloudCounter, endCloudCounter, cloudProfiles->nClouds) ;
// 	// printf("\n\t indxMol->indxInicMol[0]: %d   indxMol->indxEndMol[0]: %d", indxMol->indxInicMol[0], indxMol->indxEndMol[0] ) ;

// 	delete dco   ;
// }

void overlapCorrection( double *pr, int nBins, const char *ovlpFile )
{
	FILE 	*fp   = fopen( ovlpFile, "r" ) ;
	double 	*ovlp = (double*) malloc( nBins * sizeof(double) ) ;
	char	sOvlp[20] ;

	for ( int i=0 ; i<nBins ; i++ )
	{
		if( i<22 )
		{
			fscanf(fp, " %s ", sOvlp)   ;
			pr[i] = 0 ; // prCorr[i] = 0 ;
		}
		else
		{
			fscanf(fp, " %s ", sOvlp)   ;
			ovlp[i] 	= atof(sOvlp)   ;
			pr[i] 		= pr[i]/ovlp[i] ; // prCorr[i] 	= pr[i]/ovlp[i] ;
		}
	}
	fclose(fp) ;
}

void overlapCorrectionSignal( double *pr2, int nBins )
{
	double 	*ovlp = (double*) malloc( nBins * sizeof(double) ) ;
	int 	indxMax = 106 ;
	//double 	fMax 	;

//	findIndxMax( (double*)pr2, 0, 300, (int*)&indxMax, (double*)&fMax ) ;

	for ( int i=0 ; i<nBins ; i++ )
	{
		if ( i<indxMax )
			ovlp[i] = pr2[i]/pr2[indxMax] ;
		else
			ovlp[i] = 1 ;

		pr2[i] = pr2[i]/ovlp[i] ;
	}
//	for( int i=0 ; i<nBins ; i++ ) // 48
//		pr2[i] = pr2[i]/ovlp[i] ;
}

void chooseEventNumber	( strcGlobalParameters *glbParam, strcTheta *theta, int *ev, double *zenEv )
{
	double 	minZthEv = 1000 ;
		for ( int e=0 ; e < glbParam->nEvents ; e++ )
		{ // PICK FIRST EVENT (CLOSEST TO theta->thetaRef)
			if ( fabs(glbParam->aZenith[e] - theta->thetaRef) < minZthEv )
			{
				minZthEv = fabs(glbParam->aZenith[e] - theta->thetaRef) ;
				ev[0] = e ;
			}
		}
		minZthEv = 1000 ;
		for ( int e=0 ; e < glbParam->nEvents ; e++ )
		{ // PICK SECOND EVENT (ZENITHAL DIFFERENCE CLOSER theta->tethaDiff)
			if (  fabs(glbParam->aZenith[ev[0]] + theta->thetaDiff - glbParam->aZenith[e]) <= minZthEv  )
			{
				minZthEv = fabs(glbParam->aZenith[ev[0]] + theta->thetaDiff - glbParam->aZenith[e]) ;
				ev[1] = e ;
			}
		}

		zenEv[0] = glbParam->aZenith[ev[0]] ; // [DEG]
		zenEv[1] = glbParam->aZenith[ev[1]] ; // [DEG]
}



// void SinteticLidarSignal ( strcGlobalParameters *glbParam, strcMolecularData *dataMol, strcAerosolData *dataAer, strcLidarSignal *evSig )
// {
// 	double 	*iaMol = (double*) malloc( dataMol[0].nBins * sizeof(double) ) ;
// 	double 	*iaAer = (double*) malloc( dataMol[0].nBins * sizeof(double) ) ;

// 	double 	*aAer = (double*) malloc( dataMol[0].nBins * sizeof(double) ) ;
// 	double 	*bAer = (double*) malloc( dataMol[0].nBins * sizeof(double) ) ;
// 	double 	*ovlp = (double*) malloc( dataMol[0].nBins * sizeof(double) ) ;
// 	double 	kp    = 0.018 ;

// //	TCanvas 		*cSin			= (TCanvas*)     new TCanvas("cSin","Senal lidar", 200, 10, 700, 500) ;
// //	TMultiGraph 	*multiGrphSin	= (TMultiGraph*) new TMultiGraph ;

// 	for( int e=0 ; e < 3 ; e++ )
// 	{
// 		for( int i=0 ; i<glbParam->nBins ; i++ )
// 		{
// 			if ( i < ceil(500/cos( dataMol[e].zenith*PI/180 ) ) ) // 500*3.75 ~ 1875
// 			{
// 				aAer[i] = 2*pow(10, -5) ; // r
// 				bAer[i] = aAer[i] * kp  ; // r
// 			}
// 			else
// 			{
// 				aAer[i] = 0 ;
// 				bAer[i] = 0 ;
// 			}
// 			if( i<60 )	ovlp[i] = 0 ; // 60*3.75 = 225
// 			else 		ovlp[i] = 1 ;
// 		}

// 		cumtrapz( glbParam->dr, dataMol[e].alphaMol, 0, (dataMol[0].nBins-1), iaMol ) ;
// 		cumtrapz( glbParam->dr, aAer               , 0, (dataMol[0].nBins-1), iaAer ) ;

// 		for( int i=0 ; i<glbParam->nBins ; i++ )
// 		{
// 			evSig[e].pr[i] = pow(100, 3) * (1/pow(glbParam->r[i],2)) * ovlp[i] * (dataMol[e].betaMol[i] + bAer[i]) * exp( -2*(iaMol[i] + iaAer[i]) ) + pow(1, -3) ;
// 		//	evSig[e].pr[i] = RandN2( evSig[e].pr[i], 0.00000001 ) ; // pow(1, -8)
// 		}

// /*			evSig->pr[e][i] = ovlp[i] * (dataMol[e].betaMol[i] + bAer[i]) * exp( -2*(iaMol[i] + iaAer[i]) ) ;

// 			if (e==0)
// 				MultiPlot( (double*) aAer, dataMol[e].zr, glbParam->nBins, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, multiGrphSin, kBlack, "Alpha Aer Sintetic", "Heigh [m]", "Alpha Aer Sintetic" ) ;
// 			if (e==1)
// 				MultiPlot( (double*) aAer, dataMol[e].zr, glbParam->nBins, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, multiGrphSin, kRed, "Alpha Aer Sintetic", "Heigh [m]", "Alpha Aer Sintetic" ) ;
// 			if (e==2)
// 				MultiPlot( (double*) aAer, dataMol[e].zr, glbParam->nBins, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, multiGrphSin, kBlue, "Alpha Aer Sintetic", "Heigh [m]", "Alpha Aer Sintetic" ) ;
// */
// 	}
// 		double dr, AOD ;
// 		ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "dr", "double", (double*)&dr ) ;
// 		trapz( dr, aAer, 0, glbParam->nBins, &AOD ) ;
// 		printf("\n\t  Simulated AOD: %f \n", AOD) ;

// /*
// 	double	MOD_V_zm, VAOD ;
// 	int 	zVAOD = 3000 ;
// 	int 	binZm	= round( zVAOD/glbParam->dr ) ;
//  	trapz( glbParam->dr, dataMol[2].alphaMol, 0, binZm, &MOD_V_zm ) ;
// 	double 	zenEv[2] ;
// 			zenEv[0] = dataMol[0].zenith ; // [DEG]
// 			zenEv[1] = dataMol[1].zenith ; // [DEG]
// 	int		binVAODev1 	= (int) round( zVAOD / (glbParam->dr *cos( dataMol[0].zenith *PI/180)) ) ;
// 	int 	binVAODev2 	= (int) round( zVAOD / (glbParam->dr *cos( dataMol[1].zenith *PI/180)) ) ;
// 	tam_vaod( evSig->pr[0][binVAODev1], evSig->pr[1][binVAODev2], zenEv, MOD_V_zm, &VAOD ) ; // CON ESTE FUNCIONA
// 	printf("\n TAM-VAOD (in SinteticLidarSignal(...)): %.2f (@ %i m with %fº and %fº) \n", VAOD, zVAOD, zenEv[0], zenEv[1]) ;
// */

// 	free(iaMol) ;
// 	free(iaAer) ;
// 	free(aAer)  ;
// 	free(bAer)  ;
// 	free(ovlp)  ;
// }

