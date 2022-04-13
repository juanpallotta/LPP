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
    double          *noiseFit, *dummy   ;
    // double          **data_Noise        ;

    // void    MakeRangeCorrected( strcLidarSignal*, strcGlobalParameters*                                         ) ;
    // void    MakeRangeCorrected( strcLidarSignal*, strcGlobalParameters*, double**                               ) ;
    void    MakeRangeCorrected( strcLidarSignal*, strcGlobalParameters*, strcMolecularData*                     ) ;
    void    MakeRangeCorrected( strcLidarSignal*, strcGlobalParameters*, double**, strcMolecularData*           ) ;
    void    FindBias_Pr       ( double*, double*           , strcGlobalParameters*, strcFitParam*, int, double*, double*   ) ;
    void    FindBias_Pr       ( double*, strcMolecularData*, strcGlobalParameters*, strcFitParam*, int, double*, double*   ) ;
    void 	bkgSubstraction_Mean( double*, strcFitParam*, strcGlobalParameters*, double*                        ) ;
    void 	bkgSubstraction_MolFit(strcMolecularData*, const double*, strcFitParam*, double*                    ) ;
    void 	bkgSubstraction_BkgFile( const double*, strcFitParam*, double**, strcGlobalParameters*, double*     ) ;

    void    Average_in_Time_Lidar_Profiles( strcGlobalParameters*, double***, double***, int*, int*, int*, int* ) ;

private:
    double  *pr_NObkg_i, *pr2_i ;
    double  *errRMS_Bias        ;

};

#endif