
/* GENERAL LIDAR LIBRARY */
// #pragma once

#ifndef LIBLIDAR_H
#define LIBLIDAR_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cmath>
#include <ctime>
#include <ctype.h>
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

void 	Elastic_Rayleigh_Lidar_Signal_ ( strcMolecularData*, double* ) ;

// MEMORY GETTERS 
void    GetMem_DataFile             	( strcLidarDataFile*, strcGlobalParameters* ) ;
void 	GetMem_indxMol					( strcIndexMol*, strcGlobalParameters* ) ;
void    GetMem_evSig                	( strcLidarSignal*, strcGlobalParameters* ) ;
void    GetMem_dataAer                  ( strcAerosolData*, strcGlobalParameters* ) ;
// void  	GetMemStrcErrorSigSet			( strcErrorSignalSet*, int, int ) ;

int 	CheckLidarDataBaseIntegrity ( strcLidarDataFile*, strcGlobalParameters*, int* ) ;
int 	CheckLidarDataBaseIntegrityAVG ( strcLidarDataFile*, strcGlobalParameters*, int* ) ;
// void 	checkUnderShoot				( double*, strcGlobalParameters*, strcMolecularData*, int* ) ;

// void 	FindMolecularRange_v2		( double*, strcMolecularData*, strcGlobalParameters*, strcIndexMol*) ;

int     Read_Overlap_File           (char *, strcGlobalParameters*, double **) ;
void	overlapCorrection			( double*, int, const char * ) ;
void	overlapCorrectionSignal		( double*, int ) ;

int     GetBinOffset                ( strcLidarDataFile*, strcGlobalParameters* ) ;
void 	chooseEventNumber			( strcGlobalParameters *, strcTheta*, int*, double* ) ;

void 	GetMemDataToSave				( strcDataToSave*, strcGlobalParameters* ) ;

void	FreeMemVectorsFernaldInversion 	( strcFernaldInversion* ) ;

// void 	LowRangeCorrection			( strcGlobalParameters*, double* )  ;

/////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

