/**
 * @file mergeLidarFiles.cpp
 * @author Juan V. Pallotta (juanpallotta@gmail.com)
 * @brief Main code to process lidar signals.
 * @version 0.1
 * @date 2021-06-08
 * @copyright Copyright (c) 2021
 */

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <cstdlib>
#include <sys/stat.h>
#include <ctime>
#include <assert.h> 

// LIDAR LIBRARY ANALISYS
#include "../libLidar/libLidar.hpp"
#include "../libLidar/CDataLevel_1.hpp"
#include "../libLidar/CMolecularData_USStd.hpp"
#include "../libLidar/CNetCDF_Lidar.hpp"

int main( int argc, char *argv[] )
{
	int 	    offsetFileName ;
    ofstream    fpOutFile ;
    strcGlobalParameters    glbParam  ;

	char	PathFile_IN_FULL [100] ; // , PathINPUT [100] ; // lidarFileIN [50],
	char	FolderOUTPUT[70], PathFile_OUT_FULL[100], L1[5], L2[5] ;
    char    **inputFilesInTime ;
    int     nFilesInTime=0 ;

	sprintf( PathFile_IN_FULL, "%s", argv[1] ) ;
	sprintf( glbParam.FILE_PARAMETERS , "%s", argv[2] ) ;

    printf("\n PathFile_IN_FULL: %s \n", PathFile_IN_FULL) ;

    struct stat pathFileInput_stat ;
    stat( PathFile_IN_FULL, &pathFileInput_stat ) ;

    if ( S_ISREG(pathFileInput_stat.st_mode) )
    { // A *FILE* WAS PASSED AS AN ARGUMENT
        inputFilesInTime    = (char**) new char* [1] ;
        inputFilesInTime[0] = (char*)  new char [200] ;
        sprintf(inputFilesInTime[0], "%s", PathFile_IN_FULL ) ;
        printf("\n File to analyze: %s \n", inputFilesInTime[0]) ;
        nFilesInTime = 1 ;
    }
    else if ( S_ISDIR(pathFileInput_stat.st_mode) )
    { // A *FOLDER* WAS PASSED AS AN ARGUMENT --> analyze all the files within the time bin set in analysisParameter.dat
        int     nFilesInInputFolder =0 ;
        DIR *d;
        struct dirent *dir;
        // GET THE NUMBER OF FILES IN THE INPUT FOLDER
        d = opendir(PathFile_IN_FULL) ;
        while ( (dir = readdir(d)) != NULL )
        { // GET THE NUMBER OF FILES INSIDE THE FOLDER
            if ( ( dir->d_type == DT_REG ) && ( strcmp(dir->d_name, "..") !=0 ) && ( strcmp(dir->d_name, ".") !=0 ) )
                nFilesInInputFolder++;
        }
        printf("\n nFilesInInputFolder: %d \n", nFilesInInputFolder) ;
        if ( nFilesInInputFolder ==0 )
        {
            printf("\n There are not Licel files in the folder (see dirFile.sh) \nBye...") ;
            return -1 ;
        }
        rewinddir(d) ;
        inputFilesInTime  = (char**) new char* [nFilesInInputFolder] ;
        for( int f=0 ; f<nFilesInInputFolder ; f++ )
            inputFilesInTime[f] = (char*) new char [200] ;
        // GET THE NUMBER OF FILES WITHIN THE TIME BIN SET IN analysisParameter.dat
        nFilesInTime = (int) getInputFilesInTimeRange((char*)PathFile_IN_FULL, (char**)inputFilesInTime, (char*)glbParam.FILE_PARAMETERS ) ;
    }
    assert( nFilesInTime !=0 ); // SI SE CUMPLE, SIGUE DE LARGO.

////// LICEL FILE READOUT ////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief
 * nFilesInTime --> will be clustered every numEventsToAvg = glbParam.nEvents
 * All the dataFile[glbParam.nEvents] will be averaged and saved in dataFileAVG[glbParam.nEventsAVG] 
 */ ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"numEventsToAvg", (const char*)"int", (int*)&glbParam.nEvents ) ;
    if ( nFilesInTime == 1 ) glbParam.nEvents = 1 ;

	ReadLicelDataHeader( (char*)inputFilesInTime[0], (strcGlobalParameters*)&glbParam ) ;
    
    glbParam.nEventsAVG = (int)floor(nFilesInTime /glbParam.nEvents) ;
    if (glbParam.nEventsAVG <1)
    {
        printf("\n Not enought files in time range to average %d files --> averaging all files in the time range set \n", glbParam.nEvents ) ;
        glbParam.nEventsAVG = 1 ;
    }
    else
    {
        printf("\n Number of Clusters: %d, each one with %d files averaged\n", glbParam.nEventsAVG, glbParam.nEvents ) ;
    }

/**
 * @brief 
 * dataFile[e]: CONTAIN THE glbParam.nEvents LIDAR SIGNALS TO BE AVERAGED.
 * All dataFile[glbParam.nEvents] events are averaged and saved in dataFileAVG[glbParam.nEventsAVG]
 * Declaration and memory allocation.
 */ strcLidarDataFile	*dataFile    = (strcLidarDataFile*) new strcLidarDataFile[ glbParam.nEvents ] ;
    GetMem_DataFile( (strcLidarDataFile*)dataFile, (strcGlobalParameters*)&glbParam, (int)glbParam.nEvents ) ;
    
    strcLidarDataFile	*dataFileAVG = (strcLidarDataFile*) new strcLidarDataFile[glbParam.nEventsAVG] ;
    GetMem_DataFile( (strcLidarDataFile*)dataFileAVG, (strcGlobalParameters*)&glbParam, (int)glbParam.nEventsAVG ) ;

    struct tm   *tmFile = (struct tm*) new struct tm [1] ;    tmFile->tm_isdst = 0 ;
    time_t  fileOutputAvgTime_num ;
    char	dumpChar ;
    int		dumpInt  ;

	strcLidarSignal 	evSig ;
        GetMem_evSig( (strcLidarSignal*) &evSig, (strcGlobalParameters*) &glbParam );

// MOLECULAR DATA READOUT FOR EACH CHANNEL (MUST BE FOR EACH LAMBDA)
    char radFile[100] ;
    sprintf( radFile, "./US-StdA_DB_CEILAP.csv") ;
    CMolecularData_USStd *oMolData = (CMolecularData_USStd*) new CMolecularData_USStd( (char*)radFile, (strcGlobalParameters*)&glbParam ) ;
    for (int i = 0; i < glbParam.nLambda ; i++)
        oMolData->Fill_dataMol( (strcGlobalParameters*)&glbParam, (int)i ) ;

    ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"L1", (const char*)"string", (char*)L1 ) ;
    ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, (const char*)"L2", (const char*)"string", (char*)L2 ) ;

    strcAerosolData		  *dataAer      = (strcAerosolData*)    new strcAerosolData    [glbParam.nCh] ;
    strcErrorSignalSet	  *rndErrSigSet = (strcErrorSignalSet*) new strcErrorSignalSet [glbParam.nCh] ;
    strcErrorSignalSet    *sysErrSigSet = (strcErrorSignalSet*) new strcErrorSignalSet [glbParam.nCh] ;
    strcFernaldInversion  fernaldVectors 	;
    if ( (strcmp(L2, "SI") ==0) || (strcmp(L2, "YES") ==0) || (strcmp(L2, "si") ==0) || (strcmp(L2, "yes") ==0) )
    {
        GetMemVectorsFernaldInversion( (strcFernaldInversion*)&fernaldVectors, (int)glbParam.nBins ) ;
        for( int c=0 ; c <glbParam.nCh ; c++ )
        {
            int 	nSigSetErr ;
            ReadAnalisysParameter( (char*)glbParam.FILE_PARAMETERS, "nSigSetErr", "int" , (int*)&nSigSetErr ) ;

            GetMem_dataAer( (strcAerosolData*)&dataAer[c], (strcGlobalParameters*) &glbParam );
            GetMemStrcErrorSigSet( (strcErrorSignalSet*)&rndErrSigSet[c], (int)nSigSetErr, (int)glbParam.nBins ) ;
            GetMemStrcErrorSigSet( (strcErrorSignalSet*)&sysErrSigSet[c], (int)nSigSetErr, (int)glbParam.nBins ) ;
        }
    }

// MAIN LOOP ACROSS THE CLUSTER FILES ////////////////////////////////////////////////////////////////////////////
    CDataLevel_1 oDL1 = CDataLevel_1( (strcGlobalParameters*)&glbParam ) ;
    for ( int fC=0 ; fC <glbParam.nEventsAVG ; fC++ )
    {
        glbParam.event_analyzed = fC;
        printf("\n Cluster files Nº %d \n", fC ) ;

        fileOutputAvgTime_num =0 ;
        for ( int f=0 ; f <glbParam.nEvents ; f++ ) // glbParam.nEvents = NUMBER OF FILES TO AVERAGE.
        {
            ReadLicelData ( (char*)inputFilesInTime[fC*glbParam.nEvents +f], (strcGlobalParameters*)&glbParam, (strcLidarDataFile*)&dataFile[f] ) ;
                // GET THE OUTPUT FILE NAME BASED ON THE AVERAGED TIME OF THE INPUT FILES STORED IN inputFilesInTime.
                offsetFileName = findLastCharInString( (char*)inputFilesInTime[fC*glbParam.nEvents +f], '/' ) ;
                sscanf( &inputFilesInTime[fC*glbParam.nEvents +f][offsetFileName], "%c%2d%1x%2d%2d.%2d%2d%2d", &dumpChar, &tmFile->tm_year, &tmFile->tm_mon, &tmFile->tm_mday, &tmFile->tm_hour, &tmFile->tm_min, &tmFile->tm_sec, &dumpInt ) ;
                if (tmFile->tm_year >=90)
                    tmFile->tm_year = 1900 + tmFile->tm_year -1900 ;
                else
                    tmFile->tm_year = 2000 + tmFile->tm_year -1900 ; // tmFile->tm_year: SHOULD BE STORED THE NUMBER OF YEARS SINCE 1900.

                fileOutputAvgTime_num = fileOutputAvgTime_num + (time_t) mktime( (tm*)tmFile ) ;
                printf( "\t %s \t (sec: %ld) \n", inputFilesInTime[fC*glbParam.nEvents +f], fileOutputAvgTime_num ) ;
        }
                // FOLDER AND FILE NAME GENERATION.
                fileOutputAvgTime_num = (time_t) round( fileOutputAvgTime_num /glbParam.nEvents ) ;
                *tmFile = *localtime( &fileOutputAvgTime_num ) ;
                tmFile->tm_year = 1900 + tmFile->tm_year ;

                FolderOUTPUT[0] = '\0' ;
                sprintf( FolderOUTPUT, "%s%04d%01x%02d_OUT/", FolderOUTPUT, tmFile->tm_year, tmFile->tm_mon, tmFile->tm_mday ) ;
                mkdir ( FolderOUTPUT, 0777 ) ;
                sprintf( PathFile_OUT_FULL, "%s%c%04d%01x%02d%02d.%02d%02d00.dat", FolderOUTPUT, dumpChar, tmFile->tm_year, tmFile->tm_mon, tmFile->tm_mday, tmFile->tm_hour, tmFile->tm_min, tmFile->tm_sec ) ;

                // if ( (strcmp(L2, "SI") ==0) || (strcmp(L2, "YES") ==0) || (strcmp(L2, "si") ==0) || (strcmp(L2, "yes") ==0) )
                // {
                //     sprintf( PathFile_OUT_FULL, "%s%c%04d%01x%02d%02d.%02d%02d00_OUT_L2.dat", FolderOUTPUT, dumpChar, tmFile->tm_year, tmFile->tm_mon, tmFile->tm_mday, tmFile->tm_hour, tmFile->tm_min, tmFile->tm_sec ) ;
                // }
                // else if ( (strcmp(L1, "SI") == 0) || (strcmp(L1, "YES") == 0) || (strcmp(L1, "si") == 0) || (strcmp(L1, "yes") == 0) ) 
                // {
                //     sprintf( PathFile_OUT_FULL, "%s%c%04d%01x%02d%02d.%02d%02d00_OUT_L1.dat", FolderOUTPUT, dumpChar, tmFile->tm_year, tmFile->tm_mon, tmFile->tm_mday, tmFile->tm_hour, tmFile->tm_min, tmFile->tm_sec ) ;
                // }
                // else
                // {
                //     printf("\n NO SE SELECCIONO NINGUN NIVEL DE SALIDA \n") ;
                //     return -1 ;
                // }
                printf( "\n OUT_FILE_DIR : %s \n\n", PathFile_OUT_FULL ) ;
                fpOutFile.open(PathFile_OUT_FULL) ;

        // AVERAGE OF THE ALL PROFILES IN dataFile[1:glbParam.nEvents] IN dataFile[.]
        int cAn, cPh ;
        cAn =0 ; cPh =0 ;
        for ( int c=0 ; c <glbParam.nCh ; c++ )
        {
            for ( int b=0 ; b <glbParam.nBins ; b++ )
            {
                for ( int f=0 ; f <glbParam.nEvents ; f++ )
                {
                    dataFileAVG[fC].db_ADC[c][b] = dataFileAVG[fC].db_ADC[c][b] + dataFile[f].db_ADC[c][b] ;
                    // if ( glbParam.iAnPhot[c] == 0 )
                    //     dataFileAVG[fC].db_mV[cAn][b] = dataFileAVG[fC].db_mV[c][b] + (double)dataFile[f].db_mV[c][b] ;
                    // if ( glbParam.iAnPhot[c] == 1 )
                    //     dataFileAVG[fC].db_CountsMHz[cPh][b] = dataFileAVG[fC].db_CountsMHz[c][b] + (double)dataFile[f].db_CountsMHz[c][b] ;
                }
                dataFileAVG[fC].db_ADC[c][b] = (int)round(dataFileAVG[fC].db_ADC[c][b] /glbParam.nEvents) ;
                // if ( glbParam.iAnPhot[c] == 0 )
                // {
                //     dataFileAVG[fC].db_mV[cAn][b] = dataFileAVG[fC].db_mV[cAn][b] /glbParam.nEvents ;
                //     cAn++ ;
                // }
                // if ( glbParam.iAnPhot[c] == 1 )
                // {
                //     dataFileAVG[fC].db_CountsMHz[cPh][b] = dataFileAVG[fC].db_CountsMHz[cPh][b] /glbParam.nEvents ;
                //     cPh++ ;
                // }
            }
        }

        // GetBinOffset( (strcLidarDataFile*)dataFile, (strcGlobalParameters*)&glbParam ) ;
        for ( int c=0 ; c<glbParam.nCh ; c++ )
        {
            memset( evSig.pr		 , 0, glbParam.nBins * sizeof(double) ) ;
            memset( evSig.pr_noBkg   , 0, glbParam.nBins * sizeof(double) ) ;
            memset( evSig.pr2   	 , 0, glbParam.nBins * sizeof(double) ) ;
            memset( evSig.prFit      , 0, glbParam.nBins * sizeof(double) ) ;
            memset( evSig.pr2Fit     , 0, glbParam.nBins * sizeof(double) ) ;

            for ( int i=0 ; i < (glbParam.nBins -glbParam.indxOffset) ; i++ )
            {// BIN OFFSET CORRECTION
                evSig.pr[i] = (double)dataFileAVG[fC].db_ADC[c][i] ;
            }

            if ( (strcmp(L2, "SI") ==0) || (strcmp(L2, "YES") ==0) || (strcmp(L2, "si") ==0) || (strcmp(L2, "yes") ==0) )
            {
                Get_L1_Products( (int)c, (strcLidarSignal*)&evSig, (strcGlobalParameters*)&glbParam, (strcMolecularData*)&dataMol[c], (strcIndexMol*)&indxMol, (strcCloudProfiles*)&cloudProfiles[fC] ) ;
                Get_L2_Products( (int)c, (strcFernaldInversion*)&fernaldVectors, (strcLidarSignal*)&evSig, (strcGlobalParameters*)&glbParam, (strcMolecularData*)&dataMol[c], (strcIndexMol*)&indxMol, (strcAerosolData*)&dataAer[c],
                                 (strcErrorSignalSet*)&rndErrSigSet[c], (strcErrorSignalSet*)&sysErrSigSet[c] ) ;
            }
            else if ( (strcmp(L1, "SI") == 0) || (strcmp(L1, "si") == 0) || (strcmp(L1, "YES") == 0) || (strcmp(L1, "YES") == 0) )
            {
                oDL1.MakeRangeCorrected    ( (strcLidarSignal*)&evSig, (strcGlobalParameters*)&glbParam, (strcMolecularData*)&oMolData->dataMol[c] ) ;
                oDL1.ScanCloud_RayleightFit( (const double*)evSig.pr , (strcGlobalParameters*)&glbParam, (strcMolecularData*)&oMolData->dataMol[c] ) ;
            }
        } // for ( int c=0 ; c<glbParam.nCh ; c++ )

        // HEADER
            fpOutFile << "% " << glbParam.nCh << " " ; // SAVE NUMBER OF CHANNELS.
            for( int c=0 ; c <glbParam.nCh ; c++ ) // SAVE CHANNEL'S WAVELENGHT.
                fpOutFile << glbParam.iLambda[c] << "." << glbParam.sPol[c] << " " ;
            
            // fprintf(fpOutFile, "\nABLheight [km]: %2.2f", oDL1.indxMol[fC].ablHeight/1000 ) ; // SAVE ABL HEIGHT
            // fprintf(fpOutFile, "\nnClouds: %d", oDL1.cloudProfiles[fC].nClouds ) ; // SAVE NUMBER OF CLOUDS
            // for ( int c=0 ; c<oDL1.cloudProfiles[fC].nClouds ; c++ ) // SAVE BASE-HEIGHT OF EACH CLOUD.
            //     fprintf(fpOutFile, " %2.2f-%2.2f", oDL1.cloudProfiles[fC].indxInitClouds[c]*glbParam.dr/1000, oDL1.cloudProfiles[fC].indxEndClouds[c]*glbParam.dr/1000 ) ;
        // PROFILES
        // if ( (strcmp(L2, "SI") ==0) || (strcmp(L2, "YES") ==0) || (strcmp(L2, "si") ==0) || (strcmp(L2, "yes") ==0) )
        // {
        //     fprintf(fpOutFile, "\n\nh[km]") ;
        //     for( int c=0 ; c <glbParam.nCh ; c++ )
        //         fprintf(fpOutFile, " P(r).%04d.%c P(r)r^2.%04d.%c alphaAer.%04d.%c betaAer.%04d.%c", glbParam.iLambda[c], glbParam.sPol[c], glbParam.iLambda[c], glbParam.sPol[c], glbParam.iLambda[c], glbParam.sPol[c], glbParam.iLambda[c], glbParam.sPol[c] ) ;

            for( int i=0 ; i<glbParam.nBins ; i++ )
            {
                fpOutFile << endl << glbParam.r[i]/1000 ;
                for( int c=0 ; c<glbParam.nCh ; c++ )
                    fpOutFile <<  " " << dataFileAVG[fC].db_ADC[c][i] ;
            }
        // }
        // else if ( (strcmp(L1, "SI") ==0) || (strcmp(L1, "YES") ==0) || (strcmp(L1, "si") ==0) || (strcmp(L1, "yes") ==0) )
        // {
        //     fprintf(fpOutFile, "\n\nh[km] \t Cloud Profile") ;

        //     for( int i=0 ; i<glbParam.nBins ; i++ )
        //     {
        //         if ( fC==0 )
        //         {
        //             fprintf	( fpOutFile, "\n%02.2f \t %5.10e", glbParam.r[i], dataFileAVG[fC].db_ADC[2][i] ) ;
        //             // fprintf	( fpOutFile, "\n%02.2f \t %5.10e", glbParam.r[i], pr2[fC][i] ) ;
        //             // fprintf	( fpOutFile, "\n%02.2f \t %d \t %5.10e \t %5.10e \t %5.10e \t %5.10e ", glbParam.r[i], oDL1.cloudProfiles[fC].clouds_ON[i],
        //             // oMolData->dataMol[fC].nMol[i], oMolData->dataMol[fC].alphaMol[i], oMolData->dataMol[fC].betaMol[i], oMolData->dataMol[fC].pr2Mol[i] ) ;
        //         }
        //         // fprintf	( fpOutFile, "\n%02.2f \t %d", glbParam.r[i]/1000, oDL1.cloudProfiles[fC].clouds_ON[i] ) ;
        //     }
        // }
        fpOutFile.close() ; // fclose(fpOutFile) ;

        // for ( int e=0 ; e <glbParam.nEvents ; e++ )
        //     for ( int c=0 ; c <glbParam.nCh ; c++ )
        //         memset( (int*)dataFile[e].db_ADC[c], 0, sizeof(int)*glbParam.nBinsRaw ) ;
    } // for ( int fC=0 ; fC <glbParam.nEventsAVG ; fC++ )

    // FREE MEM FROM fernaldVectors
    // if ( (strcmp(L2, "SI") ==0) || (strcmp(L2, "YES") ==0) )
	   //  FreeMemVectorsFernaldInversion( (strcFernaldInversion*)&fernaldVectors ) ;

	return 0 ;
}
