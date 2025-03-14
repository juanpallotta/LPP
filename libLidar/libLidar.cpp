
#include "libLidar.hpp"


// const double DBL_EPS_COMP = 1 - DBL_EPSILON; // DBL_EPSILON is defined in <limits.h>.
// inline double RandU()
// {
//     return DBL_EPSILON + ((double) rand()/RAND_MAX) ;
// }
// inline double RandN2(double mu, double sigma)
// {
//     return mu + (rand()%2 ? -1.0 : 1.0)*sigma*pow(-log(DBL_EPS_COMP*RandU()), 0.5) ;
// }


int pstrcmp( const void* a, const void* b )
{
  return strcmp( *(const char**)a, *(const char**)b );
}

int getInputFilesInTimeRange( char *PathFile_IN_FULL, char **inputFilesInTime, strcGlobalParameters *glbParam ) 
{
	int 	nFilesInTime = 0 ;

    // GET THE NUMBERS minTime_num AND maxTime_num, WICH REPRESENT minTime AND maxTime STORED IN THE CONFIGURATION FILE
        char 	sMinTime[30], sMaxTime[30] ;
        ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, (const char*)"minTime", (const char*)"string", (char*)sMinTime ) ;
        ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, (const char*)"maxTime", (const char*)"string", (char*)sMaxTime ) ;
        struct tm		tmMin, tmMax ;
        sscanf(sMinTime, "%4d/%2d/%2d-%2d:%2d:%2d", &tmMin.tm_year, &tmMin.tm_mon, &tmMin.tm_mday, &tmMin.tm_hour, &tmMin.tm_min, &tmMin.tm_sec ) ;
        sscanf(sMaxTime, "%4d/%2d/%2d-%2d:%2d:%2d", &tmMax.tm_year, &tmMax.tm_mon, &tmMax.tm_mday, &tmMax.tm_hour, &tmMax.tm_min, &tmMax.tm_sec ) ;
        tmMin.tm_year = tmMin.tm_year -1900 ;   tmMin.tm_isdst = 0 ;	tmMin.tm_mon = tmMin.tm_mon -1 ;
        tmMax.tm_year = tmMax.tm_year -1900 ;   tmMax.tm_isdst = 0 ;	tmMax.tm_mon = tmMax.tm_mon -1 ;

//    						  (time_t)timegm( (tm*)tmFile_start ) + (int)round(glbParam->Time_Zone *60*60) ; // SECONDS IN UTC TIME CONVERSION
        time_t	minTime_num = (time_t)timegm( (tm*)&tmMin ) ; // minTime IS IN UTC
        time_t	maxTime_num = (time_t)timegm( (tm*)&tmMax ) ; // maxTime IS IN UTC
        // time_t	minTime_num = (time_t)mktime( (tm*)&tmMin ) ; // time_t = long --> NUMBER OF SECONDS SINCE January 1,1970
        // time_t	maxTime_num = (time_t)mktime( (tm*)&tmMax ) ; // time_t = long --> NUMBER OF SECONDS SINCE January 1,1970
            printf("\n minTime (UTC) (%ld): %04d/%02d/%02d-%02d:%02d:%02d"  , minTime_num, (tmMin.tm_year +1900), tmMin.tm_mon+1, tmMin.tm_mday, tmMin.tm_hour, tmMin.tm_min, tmMin.tm_sec ) ;
            printf("\n maxTime (UTC) (%ld): %04d/%02d/%02d-%02d:%02d:%02d\n", maxTime_num, (tmMax.tm_year +1900), tmMax.tm_mon+1, tmMax.tm_mday, tmMax.tm_hour, tmMax.tm_min, tmMax.tm_sec ) ;

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
						// printf("\n nFilesInTime: %d \t PathFile_IN_FULL: %s%s", nFilesInTime, PathFile_IN_FULL, dir->d_name ) ;
                        sprintf(inputFilesInTime[nFilesInTime], "%s%s", PathFile_IN_FULL, dir->d_name ) ;
                        nFilesInTime = nFilesInTime +1 ;
                    }
                }
            }
            closedir(d);
        }
        qsort( inputFilesInTime, nFilesInTime, sizeof(inputFilesInTime[0]), pstrcmp );
	printf("\n\tFound %d files in the time-bin selected", nFilesInTime) ; 
	return nFilesInTime ;
}

bool isFileInTimeRange ( char *fileName, time_t minTime_num, time_t maxTime_num, strcGlobalParameters *glbParam )
{
// GET THE NUMBER WHICH REPRESENT THE FILE TIME
	struct 	tm		tmFile ;
	char	dumpChar1, dumpChar2 ;
	int		dumpInt  ;

	if ( strcmp(glbParam->inputDataFileFormat, "LICEL_FILE") ==0 )
	{
		if ( isalpha(fileName[0]) && isalpha(fileName[1]) )
			sscanf( fileName, "%c%c%2d%1x%2d%2d.%2d", &dumpChar1, &dumpChar2, &tmFile.tm_year, &tmFile.tm_mon, &tmFile.tm_mday, &tmFile.tm_hour, &tmFile.tm_min ) ;
		else
			sscanf( fileName, "%c%2d%1x%2d%2d.%2d%2d%2d", &dumpChar1, &tmFile.tm_year, &tmFile.tm_mon, &tmFile.tm_mday, &tmFile.tm_hour, &tmFile.tm_min, &tmFile.tm_sec, &dumpInt ) ;
	}
	else
	{
		printf("\n Wrong 'inputDataFileFormat' parameter in %s file. (inputDataFileFormat=%s) \n EXIT...", glbParam->FILE_PARAMETERS, glbParam->inputDataFileFormat ) ;
		exit(1) ;
	}

	if (tmFile.tm_year >=90)
		tmFile.tm_year = 1900 + tmFile.tm_year -1900 ;
	else
		tmFile.tm_year = 2000 + tmFile.tm_year -1900 ; // tmFile.tm_year: SHOULD BE STORED THE NUMBER OF YEARS SINCE 1900.

	tmFile.tm_mon = tmFile.tm_mon -1 ;

	tmFile.tm_isdst = 0 ;
	time_t	fileTime_num = (time_t) timegm( (tm*)&tmFile ) - (int)round(glbParam->Time_Zone *60*60)  ;

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
				nElemVec = count( line.begin(), line.end(), ',' ) ;
				// nElemVec = count( line.begin(), line.end(), ':' ) ;
				nElemVec++ ;

				if ( strcmp( varType, "int" ) == 0 )
				{
					int var_int ;
					ss >> var_name >> eq ;
					for ( int e =0 ; e <nElemVec ; e++ )
					{
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
					// printf("\n ReadAnalisysParameter() --> var: %s \n", (char*)var) ;
					// string var_string ;
					// ss >> var_name >> eq ;
					// for ( int e =0 ; e <nElemVec ; e++ )
					// {
						// ss >> var_string >> dump ;
						// memcpy( (char*)var, (char*)var_string.c_str(), var_string.size() ) ;
						// memcpy( (char*)var +e, (char*)var_string.c_str(), var_string.size() ) ;
					// }
					// printf("\n ReadAnalisysParameter() tokenize string variable \n") ;
					// string token ;
					// while( getline( ss, token, '=') )
					// {
    				// 	printf("\n\t --> %s \n", token.c_str()) ;
					// 	// std::cout << token << '\n';
					// }
				}
			}
		} // while ( !myfile.eof() )
		myfile.close();
    }
    else 
	{
		printf("\nReadAnalisysParameter() --> Unable to open file %s \n", fileName) ;
		return 0 ;
	}
    if ( found == false)
    {
		// printf("\n Variable *%s* not found or commented in setting file *%s*  \n\n", varToFind, fileName ) ;
        if ( strcmp( varType, "string" ) == 0 )
			strcpy( ((char*)var), "NOT_FOUND") ;
        else if ( strcmp( varType, "int" ) == 0 )
            *((int*)var) = -2000 ;
        else if ( strcmp( varType, "float" ) == 0 )
            *((float*)var) = -2000 ;
        else if ( strcmp( varType, "double" ) == 0 )
            *((double*)var) = -2000 ;
		
		return -2000 ;
    }
	return nElemVec ;
}
// int ReadAnalisysParameter FROM ChatGPT

// #include <iostream>
// #include <fstream>
// #include <sstream>
// #include <string>
// #include <vector>

// int main() {
//   // Open the file
//   std::ifstream file("data.csv");

//   // Check if the file is open
//   if (!file.is_open()) {
//     std::cerr << "Error opening file" << std::endl;
//     return 1;
//   }

//   // Read the file line by line
//   std::string line;
//   while (std::getline(file, line)) {
//     // Create a string stream from the line
//     std::stringstream lineStream(line);

//     // Declare a vector to store the values
//     std::vector<std::string> values;

//     // Read the values, separated by commas
//     std::string value;
//     while (std::getline(lineStream, value, ',')) {
//       values.push_back(value);
//     }

//     // Print the values
//     for (const auto& val : values) {
//       std::cout << val << " ";
//     }
//     std::cout << std::endl;
//   }

//   // Close the file
//   file.close();

//   return 0;
// }

/*
void RayleighFit( double *sig, double *sigMol, int nBins, const char *modeBkg, const char *modeRangesFit, strcFitParam *fitParam, double *sigFil )
{
	fitParam->squared_sum_fit = (double)0.0  ;

	if ( strcmp( modeBkg, "wB" ) == 0 )
	{
		double *coeff = (double*) new double[2] ;
		polyfitCoeff( (const double* const) &sigMol[fitParam->indxInitFit], // X DATA
		  			  (const double* const) &sig[fitParam->indxInitFit], // Y DATA
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
				fitParam->squared_sum_fit = fitParam->squared_sum_fit + pow( (sigFil[i] - sig[i]), 2 ) ;
			}
		}
		else
		{ // "NOTall"
			for ( int i=fitParam->indxInitFit ; i <=fitParam->indxEndFit ; i++ )
			{
				sigFil[i] = (double) ( sigMol[i] * fitParam->m + fitParam->b ) ;
				fitParam->squared_sum_fit = fitParam->squared_sum_fit + pow( (sigFil[i] - sig[i]), 2 ) ;
			}
		}
	}
	else if ( strcmp( modeBkg, "wOutB" ) == 0 )
	{
		double	mNum=0, mDen=0 ;
		for ( int i=fitParam->indxInitFit ; i <= fitParam->indxEndFit ; i++ )
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
				fitParam->squared_sum_fit = fitParam->squared_sum_fit + pow( (sigFil[i] - sig[i]), 2 ) ;
			}
		}
		else
		{
			for ( int i=fitParam->indxInitFit ; i <=fitParam->indxEndFit ; i++ )
			{
				sigFil[i] = (double) ( sigMol[i] * fitParam->m ) ;
				fitParam->squared_sum_fit = fitParam->squared_sum_fit + pow( (sigFil[i] - sig[i]), 2 ) ;
			}
		}
	}
}
*/

// void Elastic_Rayleigh_Lidar_Signal_ ( strcMolecularData *dataMol, double *r )
// {
// 	double *MOD = (double*) new double[ dataMol->nBins ] ;

// 	cumtrapz( (r[1]-r[0]), dataMol->alphaMol, 0, dataMol->nBins-1, (double*)MOD ) ;

// 	for ( int b=0 ; b < dataMol->nBins  ; b++ )
// 	{
// 		dataMol->pr2Mol[b] 	= (double)  dataMol->betaMol[b] * exp( -2*MOD[b] ) ;
// 		dataMol->prMol [b] 	= (double) ( dataMol->pr2Mol[b]/(r[b]*r[b]) ) ;
// 	}
// 	delete MOD ;
// }


int Read_Overlap_File( char *ovlp_File, strcGlobalParameters *glbParam, double **ovlp )
{
	char 	line[1024] ;
	int		nRows_ovlpFile =0, nCols_ovlpFile =0 ;
	char 	*v ;

	FILE *fid = fopen( ovlp_File, "r") ;
	if ( fid == NULL )
		printf("\n Read_Overlap_File(...): Overlap file %s can't be openend\n", ovlp_File ) ;

// GET THE NUMBER OF LINES (BINS) IN THE OVERLAP FILE
	while( !feof(fid) )
	{
		fgets(line, 1024, fid) ;
		nRows_ovlpFile++       ;
	}
	nRows_ovlpFile--; // REMOVE THE HEADER
// GET THE NUMBER OF COLUMNS (CHANNELS) IN THE OVERLAP FILE
	v = strtok( line, ",") ;
	while(v)
	{
		v = strtok(NULL, ",") ;
		nCols_ovlpFile++ ;
	}
	nCols_ovlpFile--; // REMOVE THE RANGE COLUMN

	if ( (nRows_ovlpFile !=glbParam->nBins ) || ( nCols_ovlpFile !=glbParam->nCh ) )
	{
		printf( "\n\n*** Read_Overlap_File() --> Overlap file (%s) contain %d lines and %d channels saved, and is inconsistent with the number of bins (%d) and/or channels (%d) of the signals***\n\n", ovlp_File, nRows_ovlpFile, nCols_ovlpFile, glbParam->nBins, glbParam->nCh ) ;
		return -1 ;
	}

	fseek( fid, 0, 0 ) ; // RESET THE POINTER

	// READ THE HEADER
	fgets(line, 1024, fid) ;
	int 	c=0, i=0 ;
	// char	*endptr ;
	while( fgets(line, 1024, fid ) )
	{
		v = strtok( line, "," ) ; // TOKENIZE THE READ LINE
		v = strtok( NULL, "," ) ; // JUMP THE FIRST ELEMENT (RANGE)
		while(v)
		{
			ovlp[c][i] = (double)atof(v)  ;
			v = strtok(NULL, ",") ; // JUMP TO THE NEXT ELEMENT (CHANNEL)
			c++ ;
		}
		c =0 ;
		i++  ;
	}

	for ( c =0; c < glbParam->nCh; c++)
	{
		for (i = 0; i < glbParam->nBins_Ch[c]; i++)
		{
			if ( (ovlp[c][i] ==0) && (i >=1) && (i <=glbParam->nBins_Ch[c]) )
				ovlp[c][i] = (ovlp[c][i-1] + ovlp[c][i+1]) /2 ;
		}
	}
	return 10 ;
}

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
    evSig->pr			= (double*) new double [glbParam->nBins] ; memset( evSig->pr	   , 0, ( sizeof(double) * glbParam->nBins ) ) ;
    evSig->pr_no_DarkCur 	= (double*) new double [glbParam->nBins] ; memset( evSig->pr_no_DarkCur , 0, ( sizeof(double) * glbParam->nBins ) ) ;
    evSig->pr_noBias 	= (double*) new double [glbParam->nBins] ; memset( evSig->pr_noBias, 0, ( sizeof(double) * glbParam->nBins ) ) ;
    evSig->pr2			= (double*) new double [glbParam->nBins] ; memset( evSig->pr2	   , 0, ( sizeof(double) * glbParam->nBins ) ) ;
    evSig->prFit		= (double*) new double [glbParam->nBins] ; memset( evSig->prFit	   , 0, ( sizeof(double) * glbParam->nBins ) ) ;
    evSig->pr2Fit		= (double*) new double [glbParam->nBins] ; memset( evSig->pr2Fit   , 0, ( sizeof(double) * glbParam->nBins ) ) ;
    evSig->pr2Glued		= (double*) new double [glbParam->nBins] ; memset( evSig->pr2Glued , 0, ( sizeof(double) * glbParam->nBins ) ) ;
    evSig->prGlued 		= (double*) new double [glbParam->nBins] ; memset( evSig->prGlued  , 0, ( sizeof(double) * glbParam->nBins ) ) ;
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

// void GetMemVectorsFernaldInversion( strcFernaldInversion *fernaldVectors, int nBins )
// {
// 	fernaldVectors->pr2n 	   	  = (double*) new double [ nBins ] ;	memset( fernaldVectors->pr2n  		 , 0, sizeof(double) * nBins ) ;
// 	fernaldVectors->prn 	   	  = (double*) new double [ nBins ] ;	memset( fernaldVectors->prn  		 , 0, sizeof(double) * nBins ) ;
// 	fernaldVectors->phi  	 	  = (double*) new double [ nBins ] ;	memset( fernaldVectors->phi   		 , 0, sizeof(double) * nBins ) ;
// 	fernaldVectors->p 		  	  = (double*) new double [ nBins ] ;	memset( fernaldVectors->p   		 , 0, sizeof(double) * nBins ) ;
// 	fernaldVectors->ip		  	  = (double*) new double [ nBins ] ;	memset( fernaldVectors->ip    		 , 0, sizeof(double) * nBins ) ;
// 	fernaldVectors->ipN 		  = (double*) new double [ nBins ] ;	memset( fernaldVectors->ipN   		 , 0, sizeof(double) * nBins ) ;
// 	fernaldVectors->betaT 		  = (double*) new double [ nBins ] ;	memset( fernaldVectors->betaT 		 , 0, sizeof(double) * nBins ) ;
// 	fernaldVectors->pr2Fit		  = (double*) new double [ nBins ] ;	memset( fernaldVectors->pr2Fit		 , 0, sizeof(double) * nBins ) ;
// 	fernaldVectors->prFit		  = (double*) new double [ nBins ] ;	memset( fernaldVectors->prFit		 , 0, sizeof(double) * nBins ) ;
// 	fernaldVectors->intAlphaMol_r = (double*) new double [ nBins ] ;	memset( fernaldVectors->intAlphaMol_r, 0, sizeof(double) * nBins ) ;
// 	fernaldVectors->created = true ;
// }

void GetMem_dataAer( strcAerosolData *dataAer, strcGlobalParameters *glbParam )
{
    dataAer->alphaAer	 = (double*) new double [glbParam->nBins]   ; memset( dataAer->alphaAer, 0, ( glbParam->nBins * sizeof(double) ) ) ;
    dataAer->betaAer	 = (double*) new double [glbParam->nBins]   ; memset( dataAer->betaAer , 0, ( glbParam->nBins * sizeof(double) ) ) ;
    dataAer->VAODr		 = (double*) new double [glbParam->nBins]   ; memset( dataAer->VAODr   , 0, ( glbParam->nBins * sizeof(double) ) ) ;
}

// BACKUP
// void GetMemStrcErrorSigSet( strcErrorSignalSet *errSigSet, int MonteCarlo_N_SigSet_Err, int nBins )
// {
// 		errSigSet->prGlued  	   = (double**) new double*[MonteCarlo_N_SigSet_Err] ;
// 		errSigSet->prNoisy  	   = (double**) new double*[MonteCarlo_N_SigSet_Err] ;
// 		errSigSet->pr2Noisy 	   = (double**) new double*[MonteCarlo_N_SigSet_Err] ;
// 		errSigSet->alphaAer_ErrSet = (double**) new double*[MonteCarlo_N_SigSet_Err] ;
// 		errSigSet->VAODr_ErrSet    = (double**) new double*[MonteCarlo_N_SigSet_Err] ;

// 		for( int j=0 ; j < MonteCarlo_N_SigSet_Err ; j++ )
// 		{
// 			errSigSet->prGlued		  [j] = (double*) new double[nBins] ;
// 			errSigSet->prNoisy		  [j] = (double*) new double[nBins] ;
// 			errSigSet->pr2Noisy		  [j] = (double*) new double[nBins] ;
// 			errSigSet->alphaAer_ErrSet[j] = (double*) new double[nBins] ;
// 			errSigSet->   VAODr_ErrSet[j] = (double*) new double[nBins] ;
// 		}

// 		errSigSet->VAODr_meanErrSet	   = (double*) new double[nBins] ; 	memset( errSigSet->VAODr_meanErrSet   , 0, sizeof(double) * nBins ) ;
// 		errSigSet-> VAODr_stdErrSet	   = (double*) new double[nBins] ; 	memset( errSigSet->VAODr_stdErrSet    , 0, sizeof(double) * nBins ) ;
// 		errSigSet->alphaAer_meanErrSet = (double*) new double[nBins] ; 	memset( errSigSet->alphaAer_meanErrSet, 0, sizeof(double) * nBins ) ;
// 		errSigSet->alphaAer_stdErrSet  = (double*) new double[nBins] ;	memset( errSigSet->alphaAer_stdErrSet , 0, sizeof(double) * nBins ) ;

// 		memset( errSigSet->hVAOD_meanErrSet, 0, sizeof(double) * 4 ) ;
// 		memset( errSigSet-> hVAOD_stdErrSet, 0, sizeof(double) * 4 ) ;
// }

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

		for ( int i=(glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel]-nBinsMean) ; i < (glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel]-1) ; i++ ) 
			meanB = meanB + s[i] ;
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
/*
void checkUnderShoot( double *pr, strcGlobalParameters *glbParam, strcMolecularData *dataMol, int* evStatusAVG )
{
	strcFitParam	fitParam ;
	int 			stepIndxEnd =0;
	double 			*prFit 		= (double*) new double[glbParam->nBins] ;

	double nBinsFactor ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "nBinsFactor", "double", (int*)&nBinsFactor ) ;

SET_NEW_INDX_END_SIG:	
	fitParam.indxEndFit  = glbParam->indxEndSig -stepIndxEnd;
	fitParam.indxInitFit = fitParam.indxEndFit - glbParam->nBinsBkg ; //glbParam->nBins - 1 - ; //  glbParam->nBins - 1
	fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit ;
		RayleighFit( (double*)pr, (double*)dataMol->prMol, glbParam->nBins, "wB", "NOTall", (strcFitParam*)&fitParam, (double*)prFit ) ;	
	if( fitParam.m <0 )
	{
		stepIndxEnd = stepIndxEnd +100 ;
		goto SET_NEW_INDX_END_SIG ;
	}
	else
	{
		if ( fitParam.indxInitFit > (int)round(15000/glbParam->dr) ) // if ( fitParam.indxEndFit > (int)round(15000/glbParam->dr) )
		{ // EVENT PROBABLY WITH UNDERSHOOT, BUT STILL USEFULL...
			glbParam->indxEndSig_ev[glbParam->evSel] = (int)round(fitParam.indxEndFit *nBinsFactor) ;
			glbParam->rEndSig    	 = (double)(glbParam->indxEndSig_ev[glbParam->evSel] * glbParam->dr) ;
		}
		else // EVENT WITH UNDERSHOOT, UN-USEFULL...
		{
			evStatusAVG[glbParam->evSel] = EVNT_WRONG_USHOOT ;
			glbParam->indxEndSig_ev[glbParam->evSel] = (int)round(fitParam.indxEndFit *nBinsFactor) ;
			glbParam->rEndSig    	 = (double)(glbParam->indxEndSig_ev[glbParam->evSel] * glbParam->dr) ;
		}
	}

	// printf("(%d) NEW VALUES --> glbParam->nBins: %d \t glbParam->rEndSig: %lf \t glbParam->indxEndSig: %d", glbParam->evSel, glbParam->nBins, glbParam->rEndSig, glbParam->indxEndSig_ev[glbParam->evSel] ) ;
	delete prFit ;

	// cout << endl <<"-------------------- checkUnderShoot -> e: " << glbParam->evSel << endl ;
	// printf("glbParam->indxEndSig_ev[glbParam->evSel]: %d\n", glbParam->indxEndSig_ev[glbParam->evSel] ) ;
}
*/

// int GetIndxEvntToAvg_DiscScan( strcGlobalParameters *glbParam, strcIndxLimToAvg *indxEvntToAvg )
// {
// 	double 	*dZeniths = (double*) new double [glbParam->nEvents -1] ;
// 	diffPr( (double*)glbParam->aZenith, (int)(glbParam->nEvents-1), (double*)dZeniths ); //  [i]= [i+1] - [i]

// 	int 	c = 0 ;
// 	indxEvntToAvg[c].indxEvntInit = 0 ; // indxEvntToAvg[glbParam.numClusters]
// 	for( int e=0 ; e<glbParam->nEvents-1 ; e++ )
// 	{
// 		if ( fabs(dZeniths[e]) >2 )
// 		{
// 			indxEvntToAvg[c].indxEvntEnd = e ;
// 			c++ ;
// 			indxEvntToAvg[c].indxEvntInit = e+1 ;
// 		}
// 	}
// 	indxEvntToAvg[c].indxEvntEnd = glbParam->nEvents-1 ;

// 	return 0 ;
// }

// void GetIndxEvntToAvg_ContScan( strcGlobalParameters *glbParam, strcIndxLimToAvg *indxEvntToAvg )
// {
// 	int indxS =0 ;
// 	int dIndx =1 ;
// 	// int c =0 ;

// 	glbParam->nEventsAVG =0 ;
// 	indxEvntToAvg[glbParam->nEventsAVG].indxEvntInit =0 ;

// 	int deltaZenithAvg ;
// 	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "deltaZenithAvg", "int", (int*)&deltaZenithAvg ) ;

// 	COUNT_nEventsAVG:
// 						if ( (indxS + dIndx) <glbParam->nEvents )
// 						{
// 							if ( fabs(glbParam->aZenith[indxS] - glbParam->aZenith[indxS + dIndx]) >(int)deltaZenithAvg )
// 							{
// 								indxEvntToAvg[glbParam->nEventsAVG].indxEvntEnd = indxS + dIndx -1 ;
// 								glbParam->nEventsAVG++ ;
// 								indxS = indxS + dIndx ;
// 								indxEvntToAvg[glbParam->nEventsAVG].indxEvntInit = indxS ;
// 								dIndx =1 ;
// 								goto COUNT_nEventsAVG ;
// 							}
// 							else
// 							{
// 								dIndx++;
// 								goto COUNT_nEventsAVG ;
// 							}
// 						}
// 						else
// 						{
// 							indxEvntToAvg[glbParam->nEventsAVG].indxEvntEnd = glbParam->nEvents -1 ;
// 							if ( indxEvntToAvg[glbParam->nEventsAVG].indxEvntInit == indxEvntToAvg[glbParam->nEventsAVG].indxEvntEnd )
// 							{
// 								glbParam->nEventsAVG-- ;
// 								indxEvntToAvg[glbParam->nEventsAVG].indxEvntEnd = glbParam->nEvents -1 ;
// 								glbParam->nEventsAVG++ ;
// 							}
// 							else
// 								glbParam->nEventsAVG++ ;
// 						}
// }

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

// int AverageLidarSignal( strcLidarDataFile *dataFile, strcGlobalParameters *glbParam, int *evStatus, strcIndxLimToAvg *indxEvntToAvg, strcLidarDataFile *dataFileAVG )
// {
// 	int         evntOKcounter = 0 ;
//     long int    gpsTime_EventCluster = 0 ;

// 	for( int c=0 ; c<glbParam->nEventsAVG ; c++ ) // CLUSTER: CLUSTER OF *EVENTS*
// 	{
// 		indxEvntToAvg[c].zenithClusterEvnt = 0 ;
// 		for( int b=0 ; b<glbParam->nBinsRaw ; b++ )
// 		{
// 			for( int e=indxEvntToAvg[c].indxEvntInit ; e<=indxEvntToAvg[c].indxEvntEnd ; e++ )
// 			{
//                     if ( b==0 )
//                     {
//                         indxEvntToAvg[c].zenithClusterEvnt  = (double)indxEvntToAvg[c].zenithClusterEvnt + (double)glbParam->aZenith[e] ;
//                         gpsTime_EventCluster = gpsTime_EventCluster + (long int)glbParam->event_gps_sec[e] ;
//                     }
// 				dataFileAVG[glbParam->chSel].db_mV 		  [c][b] = (double)dataFileAVG[glbParam->chSel].db_mV[c][b] 	   + (double)dataFile[glbParam->chSel].db_mV[e][b]	;
// 				dataFileAVG[glbParam->chSel].db_CountsMHz [c][b] = (double)dataFileAVG[glbParam->chSel].db_CountsMHz[c][b] + (double)dataFile[glbParam->chSel].db_CountsMHz[e][b]	;

// 					if ( evStatus[e] == EVNT_OK ) // IF EVENT IS WRONG --> dataFile[glbParam->chSel].db_mV [e][:] = 0 
// 						evntOKcounter++ ;
// 			}
// 			if( evntOKcounter ==0 ) evntOKcounter =1 ;
// 			dataFileAVG[glbParam->chSel].db_mV[c][b]  		= (double)dataFileAVG[glbParam->chSel].db_mV[c][b]         / evntOKcounter ;
// 			dataFileAVG[glbParam->chSel].db_CountsMHz[c][b] = (double)dataFileAVG[glbParam->chSel].db_CountsMHz[c][b]  / evntOKcounter ;
//             evntOKcounter = 0 ;
// 		}
//         glbParam->event_gps_sec[c] = (int) round(gpsTime_EventCluster / (indxEvntToAvg[c].indxEvntEnd - indxEvntToAvg[c].indxEvntInit +1)) ;
//         gpsTime_EventCluster = 0 ;
// 		indxEvntToAvg[c].zenithClusterEvnt = indxEvntToAvg[c].zenithClusterEvnt / (indxEvntToAvg[c].indxEvntEnd - indxEvntToAvg[c].indxEvntInit +1) ;
// 	}
//     memset( (int*)&glbParam->event_gps_sec[glbParam->nEventsAVG], 0, sizeof(int)*(glbParam->nEvents - glbParam->nEventsAVG) );

// 	// memset( glbParam->aZenith, 0, sizeof(double)*glbParam->nEvents ) ;
// 	// glbParam->nEvents = glbParam->nEventsAVG ;
// 	for( int c=0 ; c<glbParam->nEventsAVG ; c++ )
// 		glbParam->aZenithAVG[c] = indxEvntToAvg[c].zenithClusterEvnt ;

// 	for( int c=glbParam->nEventsAVG ; c<glbParam->nEvents ; c++ )
// 		glbParam->aZenithAVG[c] = -1 ; // FILL THE REST

// 	return 0 ;    
// }

// int ReadChannelSelected ( strcGlobalParameters *glbParam ) // tel_range = used_channel = long_range_2 - long_range_1 - nothing
// {
// 	char 	strSearch[50] ;
// 	int		used_channel  ;

// 	sprintf( strSearch, "%s.%d.used_channel", glbParam->siteName, glbParam->year ) ;
// 	// printf("\n ReadChannelSelected() --> strSearch: %s \n", strSearch) ;
// 	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, strSearch, "int" , (int*)&used_channel ) ;
// 	// printf("\n ReadChannelSelected() --> used_channel: %d \n", used_channel) ;

//     return used_channel ;
// }

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
/*
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
		fitParam.indxInitFit = indxRef - 10 ;
		fitParam.indxEndFit  = indxRef + 10 ;
		fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1 ;
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
		fitParam.indxInitFit = indxRef - 10 ;
		fitParam.indxEndFit  = indxRef + 10 ;
		fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1 ;
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
*/

/*
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
*/

// void bkgSubstractionMolFit (strcMolecularData *dataMol, const double *prEl, strcFitParam *fitParam, double *pr_no_DarkCur)
// {
// 	// cout<<"----------- bkgSubstractionMolFit" ;
// 	// printf("\n\nfitParam->indxInitFit: %d\nfitParam->indxEndFit: %d\nfitParam->nFit: %d\n\n", fitParam->indxInitFit, fitParam->indxEndFit, fitParam->nFit) ;

// 	double *dummy = (double*) new double[dataMol->nBins] ; 
// 	RayleighFit( (double*)prEl, (double*)dataMol->prMol, dataMol->nBins , "wB", "NOTall", (strcFitParam*)fitParam, (double*)dummy ) ;
// 		for ( int i=0 ; i<dataMol->nBins ; i++ ) 	pr_no_DarkCur[i] = (double)(prEl[i] - fitParam->b) ; 

// 	// printf( "\nbkgSubstractionMolFit() --> fitParam->b: %lf \nfitParam->indxInitFit: %d \nfitParam->indxInitFit: %d \n", fitParam->b, fitParam->indxInitFit, fitParam->indxEndFit ) ;

// 	delete dummy ;
// }

// void bkgSubstractionMolFit_unCorr( strcMolecularData *dataMol, double *prEl, strcFitParam *fitParam, double *pr)
// { // 
// // GET m AND b UNCORRELATED (m IS SLIGTHLY CORRELATED WITH b)

// 	double 	*prDiff	= (double*) malloc( dataMol->nBins * sizeof(double) ) ;
// 	diffPr( prEl, dataMol->nBins, prDiff ) ;
// 	double 	*prMolDiff	= (double*) malloc( dataMol->nBins * sizeof(double) ) ;
// 	diffPr( dataMol->prMol, dataMol->nBins, prMolDiff ) ;

// 	// GET m
// 	double 	*prMolDiffFit	= (double*) malloc( dataMol->nBins * sizeof(double) ) ;
// 	RayleighFit( prDiff, prMolDiff, dataMol->nBins, "wOutB", "NOTall", fitParam, prMolDiffFit ) ;

// 	double 	*m_prMol	= (double*) malloc( dataMol->nBins * sizeof(double) ) ;
// 	for( int j=0 ; j<dataMol->nBins ; j++ )		m_prMol[j] = dataMol->prMol[j] * fitParam->m ;
// 	// GET b
// 	RayleighFit( (double*)prEl, (double*)m_prMol, (int)dataMol->nBins, "wB", "NOTall", fitParam, (double*)pr ) ;

// 		for ( int i=0 ; i<dataMol->nBins ; i++ ) 	pr[i] = (double)( prEl[i] - fitParam->b ) ;

// 	free(prDiff) 		;
// 	free(prMolDiff)		;
// 	free(prMolDiffFit) 	;
// 	free(m_prMol) 		;
// }

// double bkgSubstractionMean( double *sig, int binInitMean, int binEndMean, int nBins, double *pr_no_DarkCur)
// {
// 	int nFit = binEndMean - binInitMean ;

// 	double 	bkgMean = 0 ;

// 	for( int j=binInitMean ; j<binEndMean ; j++ ) bkgMean = bkgMean + sig[j] ;

// 	bkgMean = bkgMean /nFit ;

// 	for ( int i=0 ; i<nBins ; i++ ) 	pr_no_DarkCur[i] = (double)(sig[i] - bkgMean) ;

// 	// printf( "\n bkgMean: %lf \n", bkgMean ) ;

// 	return bkgMean ;
// }

// void Average_In_Time_Lidar_Profiles( strcGlobalParameters *glbParam, double ***dataFile, double ***dataFile_AVG, 
//                                     int *Raw_Data_Start_Time    , int *Raw_Data_Stop_Time, 
//                                     int *Raw_Data_Start_Time_AVG, int *Raw_Data_Stop_Time_AVG	)
// {
// 	printf( "\n\n" ) ;
//     for ( int fC=0 ; fC <glbParam->nEventsAVG ; fC++ )
//     {
//         printf("Averaging in time Cluster Nº %d \n", fC ) ;

// 		for ( int c=0 ; c <glbParam->nCh ; c++ )
// 		{
// 			for ( int b=0 ; b <glbParam->nBins ; b++ )
// 			{
// 				for ( int t=0 ; t <glbParam->numEventsToAvg ; t++ )
// 				{
// 					dataFile_AVG[fC][c][b] = (double) dataFile_AVG[fC][c][b] + dataFile[ fC *glbParam->numEventsToAvg +t ][c][b] ;
// 					if( (b==0) && (c==0) )
// 					{
// 					// TIME IS *NOT* AVERAGED!!!!
// 						// Raw_Data_Start_Time_AVG[fC]	  = (int)( (int)Raw_Data_Start_Time_AVG[fC] + (int)Raw_Data_Start_Time[fC*glbParam->numEventsToAvg +t] ) ;
// 						// Raw_Data_Stop_Time_AVG[fC] 	  = (int)( (int)Raw_Data_Stop_Time_AVG [fC] + (int)Raw_Data_Stop_Time[fC*glbParam->numEventsToAvg +t] ) ;
// 						if ( t ==0 )
// 							Raw_Data_Start_Time_AVG[fC]	  = (int)Raw_Data_Start_Time[ fC *glbParam->numEventsToAvg ] ;
// 						if ( t ==(glbParam->numEventsToAvg -1) )
// 							Raw_Data_Stop_Time_AVG[fC] 	  = (int)Raw_Data_Stop_Time[ fC *glbParam->numEventsToAvg +t ] ;

// 						glbParam->aAzimuthAVG[fC] 	  = glbParam->aAzimuthAVG[fC] + glbParam->aAzimuth[fC*glbParam->numEventsToAvg +t] ;
// 						glbParam->aZenithAVG[fC]  	  = glbParam->aZenithAVG[fC]  + glbParam->aZenith [fC*glbParam->numEventsToAvg +t]  ;
// 						glbParam->temp_K_agl_AVG[fC] = glbParam->temp_K_agl_AVG[fC] + glbParam->temp_K_agl[fC*glbParam->numEventsToAvg +t] ;
// 						glbParam->pres_Pa_agl_AVG[fC]     = glbParam->pres_Pa_agl_AVG[fC]     + glbParam->pres_Pa_agl[fC*glbParam->numEventsToAvg +t] 	  ;
// 					}
// 				}
// 					dataFile_AVG[fC][c][b]      = (double)(dataFile_AVG[fC][c][b] /glbParam->numEventsToAvg) ;
// 					if( (b==0) && (c==0) )
// 					{
// 						glbParam->aAzimuthAVG[fC] 	  = glbParam->aAzimuthAVG[fC] /glbParam->numEventsToAvg ;
// 						glbParam->aZenithAVG[fC]  	  = glbParam->aZenithAVG[fC]  /glbParam->numEventsToAvg ;
// 						glbParam->temp_K_agl_AVG[fC] = glbParam->temp_K_agl_AVG[fC] /glbParam->numEventsToAvg ;
// 						glbParam->pres_Pa_agl_AVG[fC]     = glbParam->pres_Pa_agl_AVG[fC]     /glbParam->numEventsToAvg ;
// 					}
// 			}
// 		}
// 	}
// }


// void bkgSubstractionLinearFit ( double *r, double *prEl, int nBins, strcFitParam *fitParam, double *pr)
// {
// // GET m AND b WITH A LINEAR FIT IN (CORRELATED)

// 	int binInitFit = (int) fitParam->indxInitFit ;
// 	int nFit = fitParam->nFit ;

// 	gsl_fit_linear( &r[binInitFit], 1, &prEl[binInitFit], 1, nFit, &fitParam->b, &fitParam->m, &fitParam->cov00, &fitParam->cov01, &fitParam->cov11, &fitParam->squared_sum_fit ) ;
// 	// RayleighFit( (double*)&dataMol->prMol[fitParam->indxInitFit], (double*)&prEl[fitParam->indxInitFit], dataMol->nBins, "wB", "NOTall", (strcFitParam*)fitParam, (double*)pr ) ;

// 		for ( int i=0 ; i<nBins ; i++ ) 	pr[i] = (double)(prEl[i] - ( fitParam->m * r[i] + fitParam->b ) ) ;
// }

/*
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
		fitParam.indxInitFit = i ;
		fitParam.indxEndFit  = fitParam.indxInitFit + dNfit[idrf] ;
		fitParam.nFit		 = fitParam.indxEndFit  - fitParam.indxInitFit ;
			RayleighFit( (double*)pr2, (double*)dataMol->pr2Mol, dataMol->nBins, "wOutB", "NOTall", (strcFitParam*)&fitParam, (double*)pr2Fil ) ;

		indxMol->errRMS[i]   = fitParam.squared_sum_fit ; // r
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
*/

/*
void RayleighFit_Factor( double *sig, double *sigMol, strcFitParam *fitParam, double *sigFil_Factor )
{ // PASTE MOLECULAR PROFILE IN THE RANGES DEFINED BY fitParam (DOES'T NOT FIT!!)

	fitParam->f = (double)sig[fitParam->indxInitFit]/sigMol[fitParam->indxInitFit] ;
	fitParam->squared_sum_fit = 0 ;

	for ( int i=fitParam->indxInitFit ; i <= fitParam->indxEndFit ; i++ )
	{
		sigFil_Factor[i] = (double) (sigMol[i] * fitParam->f ) ;
		fitParam->squared_sum_fit = fitParam->squared_sum_fit + pow( (sig[i] - sigFil_Factor[i]), 2 ) ;
	}
	fitParam->squared_sum_fit = sqrt(fitParam->squared_sum_fit) ;
}
*/

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

