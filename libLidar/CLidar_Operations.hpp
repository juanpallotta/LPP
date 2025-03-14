#pragma once

#ifndef CLIDAR_OPERATIONS_H
#define CLIDAR_OPERATIONS_H

#include "libLidar.hpp" // LIDAR DATA TYPE DEFINITIONS
#include "CMolecularData.hpp"

class CLidar_Operations
{
public:
    CLidar_Operations( strcGlobalParameters* ) ;
    ~CLidar_Operations();

    strcFitParam    fitParam                ;
    double          *dummy, *dummy1         ;
    int 	        nBiasRes_Auto =5        ; 
    double          bias_pre_trigger = 0.0  ;
    int 	        nLoopFindBias =5        ;

    void    MakeRangeCorrected( strcLidarSignal*, strcGlobalParameters*, strcMolecularData*                     ) ;
    void    MakeRangeCorrected( strcLidarSignal*, strcGlobalParameters*, double**, strcMolecularData*           ) ;
    void    BiasCorrection    ( strcLidarSignal*, strcGlobalParameters*, strcMolecularData*                     ) ;
    // void    BiasCorrection    ( strcLidarSignal*, strcGlobalParameters*, double**, strcMolecularData*        ) ;
    // void 	Bias_Residual_Correction( const double*, strcGlobalParameters*, strcMolecularData*, double*     ) ;
    void    Find_Max_Range    ( double*, strcMolecularData*, strcGlobalParameters*                              ) ;
    void    Remove_Cloud_Mol_Range( double*, strcGlobalParameters*, strcMolecularData*                          ) ;
    void    Remove_Bkg_Mol_Range  ( double*, strcGlobalParameters*, strcMolecularData*                          ) ;
    void    Bias_Substraction_Auto( double*, strcMolecularData*, strcGlobalParameters*, double*, double*        ) ;
    void 	Bias_Substraction_Mean( double*, strcMolecularData*, strcGlobalParameters*, double*                 ) ;
    void 	Bias_Substraction_Pre_Trigger( double*, strcGlobalParameters*, double*          ) ;
    void 	Bias_Substraction_MolFit(strcMolecularData*, const double*, strcGlobalParameters*, double*          ) ;

    void    Lidar_Signals_Corrections( strcGlobalParameters*, CMolecularData*, double**, double**, double***, double***, double*** ) ;

    void    Average_in_Time_Lidar_Profiles( strcGlobalParameters*, double***, double***, int*, int*, int*, int* ) ;
    void    GluingLidarSignals( strcGlobalParameters*, double*** ) ;

    void    Fit ( double*, double*, int, const char*, const char*, strcFitParam*, double* ) ;

    void 	TransmissionMethod_pr( double*, strcGlobalParameters*, strcMolecularData*, int, int, double* ) ;

    void 	ODcut	 			 ( double*, strcMolecularData*, strcGlobalParameters*, strcFitParam*, int* ) ;

private:
    double  *pr_NObkg_i, *pr2_i ;
    // double  *errRMS_mol, *errRMS_k, *rate, *k_ones, *R2_array;

	double 	*errRMS_Bias, *coeff ;
	double  *b_i                 ;
};

#endif