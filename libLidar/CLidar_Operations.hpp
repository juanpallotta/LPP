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
    double          prMean, prMolMean       ;
    double          *dummy, *dummy1         ;
    int 	        nBiasRes_Auto =5        ; 
    double          bias_pre_trigger = 0.0, bkg_mean = 0.0, bkg_std = 0.0  ;
    int 	        nLoopFindBias =5        ;

    int                 avg_Points_Cloud_Mask, deltaNorm, stepScanCloud, DELTA_RANGE_LIM_BINS, CLOUD_MIN_THICK ;
    double              std_factor_layer_mask   ;
    string	            compute_pbl_mask, compute_layer_mask ;
    strcCloudProfiles 	*cloudProfiles 		;
    strcIndexMol      	*indxMol  	 		;
    void    Layer_Mask			  ( const double*, strcMolecularData*   , strcGlobalParameters*                 ) ;
    void    GetCloudLimits        ( strcGlobalParameters*                                                       ) ;
    void    FilterThinClouds      ( strcGlobalParameters*, int*                                                 ) ;

    void    MakeRangeCorrected( strcLidarSignal*, strcGlobalParameters*, strcMolecularData*                     ) ;
    void    MakeRangeCorrected( strcLidarSignal*, strcGlobalParameters*, double**, strcMolecularData*           ) ;

    void    BiasCorrection    ( double*, strcGlobalParameters*, strcMolecularData*                              ) ;

    void    Find_Max_Range        ( double*, strcMolecularData*, strcGlobalParameters*                          ) ;
    void    Find_Max_Mol_Range    ( double*, strcMolecularData*, strcGlobalParameters*, int                     ) ;
    

    // void    Remove_Cloud_Mol_Range( double*, strcGlobalParameters*, strcMolecularData*                          ) ;
    void    Remove_Bkg_Mol_Range  ( double*, strcGlobalParameters*, strcMolecularData*                          ) ;
    void    Bias_Substraction_Auto( double*, strcMolecularData*, strcGlobalParameters*, double*, double*        ) ;
    void 	Bias_Substraction_Mean( double*, strcMolecularData*, strcGlobalParameters*, double*                 ) ;
    void 	Bias_Substraction_Pre_Trigger( double*, strcGlobalParameters*, double*          ) ;
    void 	Bias_Substraction_MolFit(strcMolecularData*, const double*, strcGlobalParameters*, double*          ) ;

    void    Lidar_Signals_Corrections( strcGlobalParameters*, CMolecularData*, double**, double**, double***, double***, double*** ) ;

    void    Average_in_Time_Lidar_Profiles( strcGlobalParameters*, double***, double***, int*, int*, int*, int* ) ;
    void    GluingLidarSignals( strcGlobalParameters*, double*** ) ;

    void    Fit         ( double*, double*, int, const char*, const char*, strcFitParam*, double* ) ;
    void    RayleighFit ( double*, double*, int,                           strcFitParam*, double* ) ;

    void 	TransmissionMethod_pr( double*, strcGlobalParameters*, strcMolecularData*, int, int, double* ) ;

    void 	ODcut	 			 ( double*, strcMolecularData*, strcGlobalParameters*, strcFitParam*, int* ) ;

private:
    void	GetMem_indxMol      ( strcGlobalParameters* ) ;
    void 	GetMem_cloudProfiles( strcGlobalParameters* ) ;

    double  *pr_misc, *pr2_i, *prS  ;

	double 	*errRMS_Bias, *coeff ;
	double  *b_i                 ;
};

#endif