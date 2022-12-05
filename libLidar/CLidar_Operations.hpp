#pragma once

#ifndef CLIDAR_OPERATIONS_H
#define CLIDAR_OPERATIONS_H

#include "libLidar.hpp" // LIDAR DATA TYPE DEFINITIONS

class CLidar_Operations
{
public:
    CLidar_Operations( strcGlobalParameters* ) ;
    ~CLidar_Operations();

    strcFitParam    fitParam            ;
    double          *dummy   ;

    void    MakeRangeCorrected( strcLidarSignal*, strcGlobalParameters*, strcMolecularData*                     ) ;
    void    MakeRangeCorrected( strcLidarSignal*, strcGlobalParameters*, double**, strcMolecularData*           ) ;
    void    BiasCorrection    ( strcLidarSignal*, strcGlobalParameters*, strcMolecularData*                     ) ;
    void    BiasCorrection    ( strcLidarSignal*, strcGlobalParameters*, double**, strcMolecularData*           ) ;
    // void 	Bias_Residual_Correction( const double*, strcGlobalParameters*, strcMolecularData*, double*         ) ;
    void    Find_Max_Range    ( double*, double*, strcGlobalParameters*, int*                                   ) ;
    void    Bias_Substraction_Auto( double*, strcMolecularData*, strcGlobalParameters*, double*, double*        ) ;
    void 	Bias_Substraction_Mean( double*, strcMolecularData*, strcGlobalParameters*, double*                 ) ;
    void 	Bias_Substraction_MolFit(strcMolecularData*, const double*, strcGlobalParameters*, double*          ) ;
    // void 	bkgSubstraction_BkgFile( const double*, strcFitParam*, double**, strcGlobalParameters*, double*     ) ;

    void    Average_in_Time_Lidar_Profiles( strcGlobalParameters*, double***, double***, int*, int*, int*, int* ) ;
    void    GluingLidarSignals( strcGlobalParameters*, double*** ) ;

private:
    double  *pr_NObkg_i, *pr2_i ;
    double  *errRMS_mol, *errRMS_k, *rate, *k_ones ;
    int 	nBiasRes_Auto ; 
    int 	nLoopFindBias =3 ;

	double 	*errRMS_Bias, *coeff ;
	double  *b_i                 ;
};

#endif