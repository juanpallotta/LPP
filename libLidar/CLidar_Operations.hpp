#ifndef CLIDAR_OPERATIONS_H
#define CLIDAR_OPERATIONS_H

#include "libLidar.hpp" // LIDAR DATA TYPE DEFINITIONS

#pragma once

class CLidar_Operations
{
public:
    CLidar_Operations( strcGlobalParameters* ) ;
    ~CLidar_Operations();

    strcFitParam    fitParam            ;
    double          **data_Noise        ;
    double          *noiseFit, *dummy   ;

    void    MakeRangeCorrected( strcLidarSignal*, strcGlobalParameters*, strcMolecularData*         ) ;
    void    MakeRangeCorrected( strcLidarSignal*, strcGlobalParameters*                             ) ;
    void 	bkgSubstraction_Mean( double*, int, int, strcGlobalParameters*, double*                 ) ;
    void 	bkgSubstraction_MolFit(strcMolecularData*, const double*, strcFitParam*, double*        ) ;
    void 	bkgSubstraction_BkgFile( const double*, strcFitParam*, strcGlobalParameters*, double*   ) ;

private:

};

#endif