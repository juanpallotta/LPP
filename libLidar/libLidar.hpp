
/* GENERAL LIDAR LIBRARY */
// #pragma once

#ifndef LIBLIDAR_H
#define LIBLIDAR_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cmath>
#include <ctime>
#include <limits.h>
#include <cfloat>
#include <dirent.h>
#include <cstdio>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdbool.h>
#include <assert.h> 
#include <bits/stdc++.h>
#include <sys/stat.h>

#include "lidarDataTypes.hpp" // LIDAR DATA TYPE DEFINITIONS
#include "lidarMathFunc.hpp"

// BIT MANIPULATION
//#define SetBit(A,k)     ( A[(k/32)] |= (1 << (k%32))  )
#define ClearBit(A,k)   ( A[(k/32)] &= ~(1 << (k%32)) )
//#define TestBit(A,k)    ( A[(k/32)] & (1 << (k%32))   )

using namespace std;
// using std::cout   ;
// using std::endl   ;
// using std::string ;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS TO MANIPULATE LIDAR SIGNALS

bool 	isFileInTimeRange			( char*, time_t, time_t, strcGlobalParameters*) ;
int  	getInputFilesInTimeRange  	( char*, char**, strcGlobalParameters* ) ;
int 	ReadAnalisysParameter	   	( const char*, const char*, const char*, void* ) ;
// template<typename T>
// int 	ReadAnalisysParameter_template ( const char*, const char*, const char*, T& ) ;
int 	findLastCharInString		( char*, const char ) ;

long 	FileSearch					( FILE*, const char*, long, const int ) ;

void 	ReadLicelGobalParameters    ( char*, strcGlobalParameters* ) ;
void 	ReadLicelData				( char*, strcGlobalParameters*, strcLidarDataFile* ) ;
int     Read_Bkg_Data_Files         ( char*, strcGlobalParameters *, double** ) ;
void    ReadLicelTime_and_Coord     ( FILE*, strcGlobalParameters* ) ;
void    check_Lidar_Files_Consistency( strcGlobalParameters*, char** ) ;
void    ReadLicel_AzZth             ( FILE*, strcGlobalParameters* ) ;
void    fill_up_line                (char*, int, char) ;

void 	molDataUsStd				( strcGlobalParameters*, strcMolecularData*, strcRadioSoundingData*, int ) ;
void 	ReadUsSTDfile				( const char*, strcRadioSoundingData*, strcMolecularData* ) ;
void 	RadLowToHighRes 			( int, double*, double*, int, double*, double* ) ;

void 	RayleighFit 				( double*, double*, int, const char*, const char*, strcFitParam*, double* ) ;

// void 	GetCloudLimits				( strcGlobalParameters*, strcMolecularData*, strcCloudProfiles*, strcIndexMol* ) ;

void 	TransmissionMethod_pr		( double*, strcGlobalParameters*, strcMolecularData*, int, int, double* ) ;
void 	ODcut						( double*, strcMolecularData*, strcGlobalParameters*, strcFitParam*, int* ) ;

void 	Elastic_Rayleigh_Lidar_Signal_ ( strcMolecularData*, double* ) ;

// MEMORY GETTERS 
void    GetMem_DataFile             	( strcLidarDataFile*, strcGlobalParameters* ) ;
void    GetMem_cloudProfiles        	( strcCloudProfiles*, strcGlobalParameters* ) ;
void 	GetMem_indxMol					( strcIndexMol*, strcGlobalParameters* ) ;
void    GetMem_evSig                	( strcLidarSignal*, strcGlobalParameters* ) ;
void    GetMem_dataMol              	( strcMolecularData*, strcGlobalParameters* ) ;
void	GetMemVectorsFernaldInversion 	( strcFernaldInversion*, int ) ;
void    GetMem_dataAer                  ( strcAerosolData*, strcGlobalParameters* ) ;
void  	GetMemStrcErrorSigSet			( strcErrorSignalSet*, int, int ) ;

int 	CheckLidarDataBaseIntegrity ( strcLidarDataFile*, strcGlobalParameters*, int* ) ;
int 	CheckLidarDataBaseIntegrityAVG ( strcLidarDataFile*, strcGlobalParameters*, int* ) ;
// int 	checkUnderShoot				   ( strcLidarDataFile*, strcGlobalParameters*, int ) ;
void 	checkUnderShoot				( double*, strcGlobalParameters*, strcMolecularData*, int* ) ;
int 	GetIndxEvntToAvg_DiscScan   ( strcGlobalParameters*, strcIndxLimToAvg* ) ;
void 	GetIndxEvntToAvg_ContScan   ( strcGlobalParameters*, strcIndxLimToAvg* ) ;
int 	AverageLidarSignal		    ( strcLidarDataFile*, strcGlobalParameters*, int*, strcIndxLimToAvg*, strcLidarDataFile* ) ;
// int 	selectChannel				( strcGlobalParameters*, const char* ) ;
int 	ReadChannelSelected			( strcGlobalParameters*              ) ;
void	bkgSubstractionMolFit		( strcMolecularData*, const double*, strcFitParam*, double* ) ;
void 	bkgSubstractionMolFit_unCorr( strcMolecularData*, double*, strcFitParam*, double* ) ;
double  bkgSubstractionMean			( double*, int, int, int, double*) ;
// void	bkgSubstractionLinearFit 	( double*, double*, int, strcFitParam*, double*) ;
// void 	RayleighFit_GSL				( double*, double*, int, const char*, const char*, strcFitParam*, double* ) ;
void    Average_In_Time_Lidar_Profiles( strcGlobalParameters*, double***, double***, int*, int*, int*, int* ) ;

void	RayleighFit_Factor			( double*, double*, strcFitParam*, double* ) ;
// void 	RayleighFit_unCorr			( double*, double*, int, const char*, strcFitParam*, double* ) ;
// void 	RayleighFit_M				( double*, double*, int, strcFitParam*, double* ) ;
//void 	FindMolecularRange			( double*, strcMolecularData*, strcGlobalParameters*, strcIndexMol*) ;
void 	FindMolecularRange_v2		( double*, strcMolecularData*, strcGlobalParameters*, strcIndexMol*) ;

int     Read_Overlap_File           (char *, strcGlobalParameters*, double **) ;
void	overlapCorrection			( double*, int, const char * ) ;
void	overlapCorrectionSignal		( double*, int ) ;

int     GetBinOffset                ( strcLidarDataFile*, strcGlobalParameters* ) ;
void 	chooseEventNumber			( strcGlobalParameters *, strcTheta*, int*, double* ) ;

void 	FernaldInversion			  	( double*, strcMolecularData*, strcGlobalParameters*, int, double, strcFernaldInversion*, strcAerosolData* ) ;
void 	FernaldInversion_pr			  	( double*, strcMolecularData*, strcGlobalParameters*, int, double, strcFernaldInversion*, strcAerosolData* ) ;


void 	GetMemDataToSave				( strcDataToSave*, strcGlobalParameters* ) ;

void	FreeMemVectorsFernaldInversion 	( strcFernaldInversion* ) ;
// void	SinteticLidarSignal 		( strcGlobalParameters*, strcMolecularData*, strcAerosolData*, strcLidarSignal* ) ;

void 	ProcessFernaldInversion		( strcFernaldInversion*, strcGlobalParameters*, strcMolecularData*, strcIndexMol*, strcLidarSignal*, strcAerosolData*, strcErrorSignalSet*, strcErrorSignalSet* ) ;
// void    ProcessFernaldInversion		( strcFernaldInversion*, strcGlobalParameters*, strcMolecularData*, strcLidarSignal*, strcAerosolData*, strcErrorSignalSet*, strcErrorSignalSet*) ;
// void	PreProcesingLidarSignal		( strcGlobalParameters*, strcMolecularData*, strcCloudProfiles*, strcLidarSignal*, strcIndexMol* ) ;
// void	PreProcesingLidarSignal     ( strcGlobalParameters*, strcMolecularData*, strcLidarSignal*, strcIndexMol* ) ;
// void 	MakeRangeCorrected			( strcGlobalParameters*, strcMolecularData*, strcLidarSignal* )  ;
// void 	MakeRangeCorrected_array	( double*, strcGlobalParameters*, strcMolecularData*, double* ) ;

void 	LowRangeCorrection			( strcGlobalParameters*, double* )  ;

void 	MonteCarloRandomError		( double*, double*, strcGlobalParameters*, strcMolecularData*, strcIndexMol*, strcFernaldInversion*, strcErrorSignalSet* ) ;
void 	MonteCarloSystematicError	( double*,          strcGlobalParameters*, strcMolecularData*, strcIndexMol*, strcFernaldInversion*, strcErrorSignalSet* ) ;
void 	GetErrSetParam				( char*, int, int, double, strcErrorSignalSet* ) ;
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

