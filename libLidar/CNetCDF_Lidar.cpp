
#include "CNetCDF_Lidar.hpp"  

CNetCDF_Lidar::CNetCDF_Lidar( )
{
}
	
CNetCDF_Lidar::~CNetCDF_Lidar()
{
}

void CNetCDF_Lidar::DefineDims( int nc_file_id, char *strNameDims, int sizeDims, int *dim_ids )
{
    if ( ( retval = nc_def_dim( (int)nc_file_id, (char*)strNameDims, (int)sizeDims, (int*)dim_ids ) ) )
        ERR(retval) ;
}

void CNetCDF_Lidar::DefineVariable( int nc_file_id, char *strNameVar, const char *dataType, int ndimsVar, int *dim_ids, int *var_id )
{
    if ( strcmp(dataType, "int") ==0 )
    {
        if ( ( retval = nc_def_var( (int)nc_file_id, (char*)strNameVar, NC_INT, ndimsVar, dim_ids, (int*)var_id ) ) )
            ERR(retval) ;
    }
    else if ( strcmp(dataType, "double") ==0 )
    {
        if ( ( retval = nc_def_var( (int)nc_file_id, (char*)strNameVar, NC_DOUBLE, ndimsVar, dim_ids, (int*)var_id ) ) )
            ERR(retval) ;
    }
    else if ( strcmp(dataType, "string") ==0 )
    {
        if ( ( retval = nc_def_var( (int)nc_file_id, (char*)strNameVar, NC_STRING, ndimsVar, dim_ids, (int*)var_id ) ) )
            ERR(retval) ;
    }
    else if ( strcmp(dataType, "char") ==0 )
    {
        if ( ( retval = nc_def_var( (int)nc_file_id, (char*)strNameVar, NC_CHAR, ndimsVar, dim_ids, (int*)var_id ) ) )
            ERR(retval) ;
    }
}

void CNetCDF_Lidar::PutVar( int nc_file_id, int var_id,  const char *dataType, void *dataToSave )
{
    if ( strcmp(dataType, "int") ==0 )
    {
        if ( (retval = nc_put_var_int((int)nc_file_id, (int)var_id, (int*)dataToSave ) ) )
            ERR(retval);
    }
    else if ( strcmp(dataType, "long") ==0 )
    {
        if ( (retval = nc_put_var_long((int)nc_file_id, (int)var_id, (long*)dataToSave ) ) )
            ERR(retval);
    }
    else if ( strcmp(dataType, "double") ==0 )
    {
        if ( (retval = nc_put_var_double((int)nc_file_id, (int)var_id, (double*)dataToSave ) ) )
            ERR(retval);
    }
    else if ( strcmp(dataType, "char") ==0 )
    {
        if ( (retval = nc_put_var_text((int)nc_file_id, (int)var_id, (const char*)dataToSave ) ) )
            ERR(retval);
    }
}

void CNetCDF_Lidar::PutVar_String( int nc_file_id, int var_id, char **strToSave )
{
    if ( (retval = nc_put_var_string( (int)nc_file_id, (int)var_id, (const char**)strToSave ) ) )
        ERR(retval);
}

void CNetCDF_Lidar::DefineVarDims( int nc_file_id, int NDIMS, string *dimsName , int *dimsSize , int *dim_ids  , char *strNameVars,  const char *dataType, int *var_id )
{
    for ( int d=0 ; d <NDIMS ; d++ )
        DefineDims( (int)nc_file_id, (char*)dimsName[d].c_str(), (int)dimsSize[d], (int*)&dim_ids[d] ) ;

    DefineVariable( (int)nc_file_id, (char*)strNameVars  , (const char*)dataType, (int)NDIMS, (int*)dim_ids     , (int*)var_id ) ;
}

void CNetCDF_Lidar::CloseFile( int nc_file_id )
{
    if ( ( retval = nc_close( (int)nc_file_id) ) )
        ERR(retval);
}

void CNetCDF_Lidar::ReadVar( int ncid, const char *strNameVars, void *dataRead )
{
    int     var_id ;
    nc_type var_type ;

    if ( ( retval = nc_inq_varid( (int)ncid, (const char*)strNameVars, (int*)&var_id ) ) )
        ERR(retval);

    if ( ( retval = nc_inq_vartype( (int)ncid, (int)var_id, (nc_type*)&var_type ) ) )
            ERR(retval);

    if ( var_type == NC_INT )
    {
        // cout << endl << strNameVars << "  --> DATA TYPE: INT" << endl;
        if ( (retval = nc_get_var_int( (int)ncid, (int)var_id, (int*)dataRead ) ) )
            ERR(retval);
    }
    else if ( var_type == NC_LONG )
    {
        if ( ( retval = nc_get_var_long( (int)ncid, (int)var_id, (long*)dataRead ) ) )
            ERR(retval);
    }
    else if ( var_type == NC_DOUBLE )
    {
        // cout << endl << strNameVars << "  --> DATA TYPE: DOUBLE" << endl;
        if ( ( retval = nc_get_var_double( (int)ncid, (int)var_id, (double*)dataRead ) ) )
            ERR(retval);
    }
}

void CNetCDF_Lidar::Read_Bkg_Noise( int ncid, strcGlobalParameters *glbParam, int id_var_noise, double **data_Noise )
{
    double buff ;

    size_t start_noise[2], count_noise[2];
    start_noise[0] = 0;   count_noise[0] = 1 ; // glbParam->nCh; 
    start_noise[1] = 0;   count_noise[1] = glbParam->nBins ;

    for ( int c=0 ; c <glbParam->nCh ; c++ )
    {
        start_noise[0] =c ;
        if ( (retval = nc_get_vara_double((int)ncid, (int)id_var_noise, start_noise, count_noise, (double*)&data_Noise[c][0] ) ) )
            ERR(retval);    

            // OFFSET CORRECTION FOR THE BACKGROUND SIGNALS
            if ( glbParam->indxOffset[c] >=0 ) // PHOTON-CURRENT SIGNALS --> THE SIGNAL HAVE TO MOVE *BACKWARD* glbParam.indxOffset[c] BINS
            {
                for(int b =0; b <(glbParam->nBins -glbParam->indxOffset[c]); b++)
                {
                    buff = (double)data_Noise[c][b +glbParam->indxOffset[c]] ;
                    data_Noise[c][b] = (double)buff ;
                }
                for ( int b=(glbParam->nBins -glbParam->indxOffset[c]) ; b <glbParam->nBins ; b++ )
                {
                    buff = (double)data_Noise[c][glbParam->nBins -glbParam->indxOffset[c]] ;
                    data_Noise[c][b] = buff ; 
                }
            }
            else // glbParam->indxOffset[c] <0 // PHOTON-COUNTING SIGNALS --> THE SIGNAL HAVE TO MOVE *FORWARD* glbParam.indxOffset[c] BINS
            {
                for ( int b=(glbParam->nBins-1) ; b >=(-1*glbParam->indxOffset[c]) ; b-- )
                    data_Noise[c][b] = (double)data_Noise[c][b +glbParam->indxOffset[c]] ;
                for(int b =0 ; b <(-1*glbParam->indxOffset[c]) ; b++)
                    data_Noise[c][b] = (double)0.0 ;
            }
    }
    glbParam->is_Noise_Data_Loaded = true ;
}

void CNetCDF_Lidar::Read_Overlap( int ncid, strcGlobalParameters *glbParam, int id_var_ovlp, double **ovlp )
{
    size_t start_ovlp[2], count_ovlp[2];
    start_ovlp[0] = 0;   count_ovlp[0] = 1 ; // glbParam->nCh; 
    start_ovlp[1] = 0;   count_ovlp[1] = glbParam->nBins ;
    for ( int c=0 ; c <glbParam->nCh ; c++ )
    {
        start_ovlp[0] =c ;
        count_ovlp[1] = glbParam->nBins_Ch[c] ;
        if ( (retval = nc_get_vara_double((int)ncid, (int)id_var_ovlp, start_ovlp, count_ovlp, (double*)&(ovlp[c][0]) ) ) )
            ERR(retval);    
    }
    glbParam->is_Ovlp_Data_Loaded = true ;
}

void CNetCDF_Lidar::Read_GlbParameters( int ncid, strcGlobalParameters *glbParam )
{
    // READ VARIABLES FROM DE NETCDF INPUT FILE
    int id_var_pr ;
    if ( ( retval = nc_inq_varid( (int)ncid, (const char*)"Raw_Lidar_Data", (int*)&id_var_pr ) ) )
        ERR(retval) ;
    int num_dim_var ;
    if ( ( retval = nc_inq_varndims( (int)ncid, (int)id_var_pr, (int*)&num_dim_var ) ) )
        ERR(retval);
    // cout << endl << "num_dim_var: " << num_dim_var << "     id_var_pr: " << id_var_pr << endl ;
    assert( num_dim_var ==3 ) ; // SI SE CUMPLE, SIGUE.

    int id_dim[num_dim_var] ;
    if ( ( retval = nc_inq_vardimid( (int)ncid, (int)id_var_pr, (int*)id_dim ) ) )
        ERR(retval);

    int size_dim[num_dim_var] ;
        for ( int d=0 ; d <num_dim_var ; d++ ) size_dim[d] =(int)1 ;
    for( int d=0 ; d <num_dim_var ; d++ )
    {
        if ( ( retval = nc_inq_dimlen( (int)ncid, (int)id_dim[d], (size_t*)&size_dim[d] ) ) )
            ERR(retval);
        // printf( "\nsize_dim[%d]: %d", d, size_dim[d] ) ;
    }
    glbParam->nEvents  = size_dim[0] ; glbParam->nEventsAVG = glbParam->nEvents ; // 'time' DIMENSION
    glbParam->evSel    = -1 ; // DEFAULT VALUE, IF <0, ANALYZE VERTICAL PROFILE
    glbParam->nCh      = size_dim[1] ; glbParam->nLambda    = glbParam->nCh     ; // IT SHOULD BE CALCULATE BASED ON *DIFFERENTS* WAVELENGHS.
    glbParam->nBins    = size_dim[2] ; // 'range' DIMENSION
    glbParam->nBins_Ch = (int*) new int[ glbParam->nCh ] ;

    glbParam->nEventsAVG     = (int)round( glbParam->nEvents /glbParam->numEventsToAvg ) ;
// printf("\n PDL1: glbParam->nEvents= %d \t glbParam->numEventsToAvg= %d \t glbParam->nEventsAVG= %d \n", glbParam->nEvents, glbParam->numEventsToAvg, glbParam->nEventsAVG) ;
    glbParam->aZenith     = (double*) new double [glbParam->nEvents]    ;
    glbParam->aAzimuth    = (double*) new double [glbParam->nEvents]    ;
    glbParam->aZenithAVG  = (double*) new double [glbParam->nEventsAVG] ;     memset( (double*)glbParam->aZenithAVG , 0, (sizeof(double)*glbParam->nEventsAVG) ) ;
    glbParam->aAzimuthAVG = (double*) new double [glbParam->nEventsAVG] ;     memset( (double*)glbParam->aAzimuthAVG, 0, (sizeof(double)*glbParam->nEventsAVG) ) ;
    ReadVar( (int)ncid, (const char*)"Zenith" , (double*)glbParam->aZenith  ) ;
    ReadVar( (int)ncid, (const char*)"Azimuth", (double*)glbParam->aAzimuth ) ;

    glbParam->temp_K_agl      = (double*) new double [glbParam->nEvents]     ;
    glbParam->pres_Pa_agl     = (double*) new double [glbParam->nEvents]     ;
    glbParam->temp_K_agl_AVG  = (double*) new double [glbParam->nEventsAVG]  ;   memset( (double*)glbParam->temp_K_agl_AVG, 0, (sizeof(double)*glbParam->nEventsAVG) ) ;
    glbParam->pres_Pa_agl_AVG = (double*) new double [glbParam->nEventsAVG]  ;   memset( (double*)glbParam->pres_Pa_agl_AVG    , 0, (sizeof(double)*glbParam->nEventsAVG) ) ;
    ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"Temperature_at_Lidar_Station_K", (const char*)"double", (int*)&glbParam->temp_K_agl[0] ) ;
    ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"Pressure_at_Lidar_Station_Pa"   , (const char*)"double", (int*)&glbParam->pres_Pa_agl[0]     ) ;
    for (  int i =1 ; i < glbParam->nEvents ; i++ )
    {   //* TO BE UPDATED WITH /GetRadiosounding/get_Meteodata.py
        glbParam->temp_K_agl[i]  = glbParam->temp_K_agl[0]       ;
        glbParam->pres_Pa_agl[i] = glbParam->pres_Pa_agl[0] ;
    }

    if ( ( retval = nc_get_att_double(	(int)ncid, (int)NC_GLOBAL, (const char*)"Range_Resolution"  , (double*)&glbParam->dr) )            )
        ERR(retval);
    if ( ( retval = nc_get_att_int(	(int)ncid, (int)NC_GLOBAL, (const char*)"Altitude_meter_asl", (int*)&glbParam->siteASL) )       )
        ERR(retval);
    if ( ( retval = nc_get_att_int(	(int)ncid, (int)NC_GLOBAL, (const char*)"Laser_Frec_1"      , (int*)&glbParam->Laser_Frec[0]) ) )
        ERR(retval);
    if ( ( retval = nc_get_att_int(	(int)ncid, (int)NC_GLOBAL, (const char*)"Laser_Frec_2"      , (int*)&glbParam->Laser_Frec[1]) ) )
        ERR(retval);

    glbParam->indxOffset = (int*) new int [ glbParam->nCh ] ;
    ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"nBinsBkg"        , (const char*)"int"   , (int*)   &glbParam->nBinsBkg        ) ;
    ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"range_Bkg_Start" , (const char*)"double", (double*)&glbParam->range_Bkg_Start ) ;
    ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"range_Bkg_Stop"  , (const char*)"double", (double*)&glbParam->range_Bkg_Stop  ) ;
    glbParam->indx_range_Bkg_Start = (int)round( glbParam->range_Bkg_Start /glbParam->dr ) ;
    glbParam->indx_range_Bkg_Stop  = (int)round( glbParam->range_Bkg_Stop  /glbParam->dr ) ;

    int num_Ch = ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"indxOffset" , (const char*)"int"   , (int*)    glbParam->indxOffset ) ;
    if ( num_Ch != glbParam->nCh )
    {
        printf( "\n ERROR: indxOffset MUST have the same number of elemenst as the number of channels (%d)... bye", glbParam->nCh) ;
        exit(1) ;
    }
    ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"rInitSig"   , (const char*)"double", (double*)&glbParam->rInitSig   ) ;

    ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"indxWL_PDL1", (const char*)"int", (int*)&glbParam->indxWL_PDL1 ) ;
    ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"indxWL_PDL2", (const char*)"int", (int*)&glbParam->indxWL_PDL2 ) ;

    glbParam->indxInitSig = (int)round( glbParam->rInitSig /glbParam->dr ) ;
    glbParam->iLambda = (int*) new int [glbParam->nCh] ;
    ReadVar( (int)ncid, (const char*)"Wavelengths", (double*)&glbParam->iLambda[0] ) ;
 
    glbParam->DAQ_Type = (int*) new int [glbParam->nCh] ;
    ReadVar( (int)ncid, (const char*)"DAQ_type", (int*)&glbParam->DAQ_Type[0] ) ;
    glbParam->iMax_mVLic = (double*) new double [glbParam->nCh] ;
    ReadVar( (int)ncid, (const char*)"DAQ_Range", (double*)glbParam->iMax_mVLic ) ;
    glbParam->nShots = (int*) new int [glbParam->nCh] ;
    ReadVar( (int)ncid, (const char*)"Accumulated_Pulses", (int*)&glbParam->nShots[0] ) ;

    ReadVar( (int)ncid, (const char*)"nBins_Ch", (int*)&glbParam->nBins_Ch[0] ) ;

    glbParam->indxEndSig_ev_ch = (int**)    new int*    [ glbParam->nEventsAVG ] ;
    glbParam->rEndSig_ev_ch    = (double**) new double* [ glbParam->nEventsAVG ] ;
    for ( int e =0 ; e <glbParam->nEventsAVG ; e++ )
    {
        glbParam->indxEndSig_ev_ch[e] = (int*)    new int   [ glbParam->nCh ] ;
        glbParam->rEndSig_ev_ch   [e] = (double*) new double[ glbParam->nCh ] ;

        for (int c =0 ; c <glbParam->nCh ; c++)
        {
            glbParam->rEndSig_ev_ch   [e][c] = (double) -1.0 ; // (glbParam->nBins-1) * glbParam->dr  ;
            glbParam->indxEndSig_ev_ch[e][c] = (int)    -1   ; // glbParam->nBins-1                  ;
        }
    }

    glbParam->r     = (double*) new double[glbParam->nBins] ;
    glbParam->r_avg = (double*) new double[glbParam->nBins] ;

    for( int i=0 ; i <glbParam->nBins ; i++ )
    {
        glbParam->r[i]     = (i+1)*glbParam->dr - glbParam->dr /2 ; // glbParam->r[0] = 3.75 (ie)
        glbParam->r_avg[i] = glbParam->r[i] ;
    }
    glbParam->tBin_us = pow(10, 6) * 2*glbParam->dr /(3*pow(10, 8)) ; // uSec
    glbParam->avg_Points_Fernald = (int*) new int[glbParam->nCh] ;

// cout<<endl;
// cout<<endl<<"(0) glbParam->r[0]: "<< glbParam->r[0] << endl;
// cout<<endl<<"(0) glbParam->r[1]: "<< glbParam->r[1] << endl;
// cout<<endl<<"(0) glbParam->r[2]: "<< glbParam->r[2] << endl;
// cout<<endl<<"(0) glbParam->nEvents: "<< glbParam->nEvents << endl;
// cout<<endl<<"(0) glbParam->nEventsAVG: "<< glbParam->nEventsAVG << endl;
// cout<<endl<<"(0) glbParam->nCh: "<< glbParam->nCh << endl;
// cout<<endl<<"(0) glbParam->nBins: "<< glbParam->nBins << endl;
}

void CNetCDF_Lidar::Read_L0_into_L2( int ncid, strcGlobalParameters *glbParam, CDataLevel_2 *oDL2 )
{
    // READ VARIABLES FROM THE L0 DATA GROUP INTO L2 OBJECT TO BE AVERAGED LATER
    int id_var_pr ;
    if ( ( retval = nc_inq_varid( (int)ncid, (const char*)"Raw_Lidar_Data", (int*)&id_var_pr ) ) )
        ERR(retval) ;

    oDL2->data_File_L0  = (double***) new double**[glbParam->nEvents];
    for ( int e=0 ; e <glbParam->nEvents ; e++ )
    {
        oDL2->data_File_L0[e] = (double**) new double*[glbParam->nCh] ;
        for ( int c=0 ; c <glbParam->nCh ; c++ )
        {
            oDL2->data_File_L0[e][c] = (double*) new double[glbParam->nBins] ;
            for(int b =0 ; b <glbParam->nBins ; b++)
                oDL2->data_File_L0[e][c][b] = (double)0;
        }
    }

    size_t startDF[3], countDF[3];
    startDF[0] = 0;   countDF[0] = 1 ; // glbParam->nEvents ; 
    startDF[1] = 0;   countDF[1] = 1 ; // glbParam->nCh; 
    startDF[2] = 0;   countDF[2] = glbParam->nBins;
    for( int e=0 ; e <glbParam->nEvents ; e++ )
    {
        startDF[0] =e ;
        for ( int c=0 ; c <glbParam->nCh ; c++ )
        {
            startDF[1] =c ;
            if ( (retval = nc_get_vara_double((int)ncid, (int)id_var_pr, startDF, countDF, (double*)&oDL2->data_File_L0[e][c][0] ) ) )
                ERR(retval);    
        }
    }

    oDL2->Start_Time_L0 = (int*) new int[ glbParam->nEvents ] ;
    oDL2->Stop_Time_L0  = (int*) new int[ glbParam->nEvents ] ;
    ReadVar( (int)ncid, (const char*)"Raw_Data_Start_Time", (int*)oDL2->Start_Time_L0 ) ;
    ReadVar( (int)ncid, (const char*)"Raw_Data_Stop_Time" , (int*)oDL2->Stop_Time_L0  ) ;

    ReadVar( (int)ncid, (const char*)"Azimuth" , (double*)glbParam->aAzimuth ) ;
    ReadVar( (int)ncid, (const char*)"Zenith"  , (double*)glbParam->aZenith  ) ;
}

void CNetCDF_Lidar::Read_L1_into_L2( int ncid_L1_Data, strcGlobalParameters *glbParam, CDataLevel_2 *oDL2 )
{
    int id_var_pr_L1 ;
    if ( ( retval = nc_inq_varid( (int)ncid_L1_Data, (const char*)"Raw_Lidar_Data_L1", (int*)&id_var_pr_L1 ) ) )
        ERR(retval) ;

    int id_var_rEndSig_ev_ch ;
    if ( ( retval = nc_inq_varid( (int)ncid_L1_Data, (const char*)"MaxRangeAnalysis", (int*)&id_var_rEndSig_ev_ch ) ) )
        ERR(retval) ;

    size_t startDF[3], countDF[3];
    size_t startES[2], countES[2];
    startDF[0] = 0;   countDF[0] = 1 ; // glbParam->nEvents ; 
    startDF[1] = 0;   countDF[1] = 1 ; // glbParam->nCh; 
    startDF[2] = 0;   countDF[2] = glbParam->nBins;

    startES[0] = 0;   countES[0] = 1 ; // EVENTS
    startES[1] = 0;   countES[1] = glbParam->nCh ; // CHANNELS

    for( int e=0 ; e <glbParam->nEventsAVG ; e++ )
    {
        startDF[0] =e ;
        for ( int c=0 ; c <glbParam->nCh ; c++ )
        {
            startDF[1] =c ;
            if ( (retval = nc_get_vara_double((int)ncid_L1_Data, (int)id_var_pr_L1, startDF, countDF, (double*)&oDL2->data_File_L2[e][c][0] ) ) )
                ERR(retval);    
        }
        startES[0] =e ;
        if ( (retval = nc_get_vara_double((int)ncid_L1_Data, (int)id_var_rEndSig_ev_ch, startES, countES, (double*)&glbParam->rEndSig_ev_ch[e][0] ) ) )
            ERR(retval);    
    }
    ReadVar( (int)ncid_L1_Data, (const char*)"Start_Time_AVG_L1", (int*)oDL2->Start_Time_AVG_L2 ) ;
    ReadVar( (int)ncid_L1_Data, (const char*)"Stop_Time_AVG_L1" , (int*)oDL2->Stop_Time_AVG_L2  ) ;

    ReadVar( (int)ncid_L1_Data, (const char*)"Azimuth_AVG_L1", (double*)glbParam->aAzimuthAVG ) ;
    ReadVar( (int)ncid_L1_Data, (const char*)"Zenith_AVG_L1" , (double*)glbParam->aZenithAVG  ) ;

    for ( int e =0; e <glbParam->nEventsAVG; e++ )
    {
        glbParam->temp_K_agl_AVG[e]  = (double)glbParam->temp_K_agl[e] ;
        glbParam->pres_Pa_agl_AVG[e] = (double)glbParam->pres_Pa_agl[e]     ;
        for (int c =0; c < glbParam->nCh; c++)
            glbParam->indxEndSig_ev_ch[e][c]   = (int)round(glbParam->rEndSig_ev_ch[e][c] /glbParam->dr) ;
    }
    
    // Cloud_Mask VARIABLE IS ALWAY PRESENT IN THE L1 DATA GROUP.
    int id_var_lm ;
    if ( ( retval = nc_inq_varid( (int)ncid_L1_Data, (const char*)"Cloud_Mask", (int*)&id_var_lm ) ) )
        ERR(retval) ;

    oDL2->layer_mask = (int**) new int*[ glbParam->nEventsAVG ] ; // TIME DIMENSION
    for ( int e=0 ; e <glbParam->nEventsAVG ; e++ )
        oDL2->layer_mask[e] = (int*) new int[ glbParam->nBins ] ; // RANGE DIMESIONS

    size_t start_lm[2], count_lm[2] ;
    start_lm[0] = 0;    count_lm[0] = 1 ;
    start_lm[1] = 0;    count_lm[1] = glbParam->nBins ;
    for ( int t=0 ; t <glbParam->nEventsAVG ; t++ )
    {
        start_lm[0] =t ;
        if ( (retval = nc_get_vara_int((int)ncid_L1_Data, (int)id_var_lm, start_lm, count_lm, (int*)&oDL2->layer_mask[t][0] ) ) )
            ERR(retval) ;
    }
}

void CNetCDF_Lidar::Putt_Bulk_Att_Text( int ncid, int id_var, int size_list, string *strAttListName, string *strAttList )
{
    for ( int c=0 ; c <size_list ; c++ )
    {
        if ( ( retval = nc_put_att_text( (int)ncid, (int)id_var, (const char*)strAttListName[c].c_str(), (size_t)strAttListName[c].length(), (const char*)strAttList[c].c_str() ) ) )
            ERR(retval);
    }
}

void CNetCDF_Lidar::Putt_Bulk_Att_Double( int ncid, int id_var, int size_list, string *strAttListName, double *dblAttList )
{
    for ( int c=0 ; c <size_list ; c++ )
    {
        if ( ( retval = nc_put_att_double( (int)ncid, (int)id_var, (const char*)strAttListName[c].c_str(), NC_DOUBLE, 1, (const double*)&dblAttList[c] ) ) )
            ERR(retval);
    }
}

void CNetCDF_Lidar::Putt_Bulk_Att_Int( int ncid, int id_var, int size_list, string *strAttListName, int *intAttList )
{
    for ( int c=0 ; c <size_list ; c++ )
    {
        if ( ( retval = nc_put_att_int( (int)ncid, (int)id_var, (const char*)strAttListName[c].c_str(), NC_INT, 1, (const int*)&intAttList[c] ) ) )
            ERR(retval);
    }
}

void CNetCDF_Lidar::Set_LALINET_Units_L0( int ncid, int *var_ids )
{
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[0], (const char*)"units", (size_t)strlen("Averaged ADC Counts / MHz"), (const char*)"Averaged ADC Counts / MHz") ) )
       ERR(retval);
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[1], (const char*)"units", (size_t)strlen("seconds since 01/Jan/1970"), (const char*)"seconds since 01/Jan/1970") ) )
       ERR(retval);
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[2], (const char*)"units", (size_t)strlen("seconds since 01/Jan/1970"), (const char*)"seconds since 01/Jan/1970") ) )
       ERR(retval);
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[3], (const char*)"units", (size_t)strlen("nanometers"), (const char*)"nanometers") ) )
       ERR(retval);
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[4], (const char*)"units", (size_t)strlen("o: no polarisation, s = perpendicular, l = parallel"), (const char*)"o: no polarisation, s = perpendikular, l = parallel") ) )
       ERR(retval);
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[5], (const char*)"units", (size_t)strlen("mV - discriminator level"), (const char*)"mV - discriminator level") ) )
       ERR(retval);
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[6], (const char*)"units", (size_t)strlen("0:Analog 1:Digital"), (const char*)"0:Analog 1:Digital") ) )
       ERR(retval);
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[7], (const char*)"units", (size_t)strlen("mV"), (const char*)"mV") ) )
       ERR(retval);
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[9], (const char*)"units", (size_t)strlen("degrees"), (const char*)"degrees") ) )
       ERR(retval);
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[10], (const char*)"units", (size_t)strlen("degrees"), (const char*)"degrees") ) )
       ERR(retval);
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[17], (const char*)"units", (size_t)strlen("meters"), (const char*)"meters") ) )
       ERR(retval);
}

void CNetCDF_Lidar::Set_LALINET_Units_L1( int ncid, int *var_ids )
{
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[0], (const char*)"units", (size_t)strlen("Averaged ADC Counts / MHz"), (const char*)"Averaged ADC Counts / MHz") ) )
       ERR(retval);
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[1], (const char*)"units", (size_t)strlen("seconds since 01/Jan/1970"), (const char*)"seconds since 01/Jan/1970") ) )
       ERR(retval);
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[2], (const char*)"units", (size_t)strlen("seconds since 01/Jan/1970"), (const char*)"seconds since 01/Jan/1970") ) )
       ERR(retval);
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[3], (const char*)"units", (size_t)strlen("degrees"), (const char*)"degrees") ) )
       ERR(retval);
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[4], (const char*)"units", (size_t)strlen("degrees"), (const char*)"degrees") ) )
       ERR(retval);
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[5], (const char*)"units", (size_t)strlen("Bins"), (const char*)"Bins") ) )
       ERR(retval);
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[6], (const char*)"units", (size_t)strlen("m"), (const char*)"m") ) )
       ERR(retval);
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[7], (const char*)"units", (size_t)strlen("K"), (const char*)"K") ) )
       ERR(retval);
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[8], (const char*)"units", (size_t)strlen("Pa"), (const char*)"Pa") ) )
       ERR(retval);
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[9], (const char*)"units", (size_t)strlen("K"), (const char*)"K") ) )
       ERR(retval);
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[10], (const char*)"units", (size_t)strlen("Pa"), (const char*)"Pa") ) )
       ERR(retval);
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[11], (const char*)"units", (size_t)strlen("meters"), (const char*)"meters") ) )
       ERR(retval);
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[12], (const char*)"units", (size_t)strlen("0:NO LAYER 1:LAYER"), (const char*)"-") ) )
       ERR(retval);
}

void CNetCDF_Lidar::Set_LALINET_Units_L2( int ncid, int *var_ids )
{
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[0], (const char*)"units", (size_t)strlen("1/m"), (const char*)"1/m") ) )
       ERR(retval);
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[1], (const char*)"units", (size_t)strlen("1/m*sr"), (const char*)"1/m*sr") ) )
       ERR(retval);
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[2], (const char*)"units", (size_t)strlen("a.u."), (const char*)"a.u.") ) )
       ERR(retval);
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[3], (const char*)"units", (size_t)strlen("seconds since 01/Jan/1970"), (const char*)"seconds since 01/Jan/1970") ) )
       ERR(retval);
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[4], (const char*)"seconds", (size_t)strlen("seconds since 01/Jan/1970"), (const char*)"seconds since 01/Jan/1970") ) )
       ERR(retval);
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[5], (const char*)"units", (size_t)strlen("degrees"), (const char*)"degrees") ) )
       ERR(retval);
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[6], (const char*)"units", (size_t)strlen("degrees"), (const char*)"degrees") ) )
       ERR(retval);
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[7], (const char*)"units", (size_t)strlen("m"), (const char*)"m") ) )
       ERR(retval);
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[8], (const char*)"units", (size_t)strlen("1/sr"), (const char*)"1/sr") ) )
       ERR(retval);
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[9], (const char*)"units", (size_t)strlen("-"), (const char*)"-") ) )
       ERR(retval);
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[10], (const char*)"units", (size_t)strlen("meters"), (const char*)"meters") ) )
       ERR(retval);
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[12], (const char*)"units", (size_t)strlen("meters"), (const char*)"meters") ) )
       ERR(retval);
}

// ! SEE FOR DETAILS: --> https://docs.scc.imaa.cnr.it/en/latest/file_formats/netcdf_file.html
void CNetCDF_Lidar::Save_SCC_NCDF_Format( string Path_File_Out, strcGlobalParameters *glbParam, double ***dataToSave, long *Raw_Data_Start_Time, string *Raw_Data_Start_Time_str, long *Raw_Data_Stop_Time, string *Raw_Data_Stop_Time_str )
{
    int  ncid ;
    int  retval ;

    if ( (retval = nc_create( Path_File_Out.c_str(), NC_NETCDF4, &ncid)) )
        ERR(retval);

    // DIMENSIONS INFO FOR pr VARIABLE
    string      dimsName [NDIMS_SCC] ;
    int         dimsSize[NDIMS_SCC] ;
    dimsName[0] = "time" ;                  dimsSize[0] = glbParam->nEventsAVG   ;
    dimsName[1] = "channels" ;              dimsSize[1] = glbParam->nCh          ;
    dimsName[2] = "points" ;                dimsSize[2] = glbParam->nBinsRaw     ;
    dimsName[3] = "scan_angles" ;           dimsSize[3] = 1                      ;
    dimsName[4] = "nb_of_time_scales" ;     dimsSize[4] = 1                      ;

    int  dim_ids[NDIMS_SCC];
    int  dim_ids_StartEnd_time[2];

    int  var_ids[NVARS_SCC] ;
    string  strNameVars[NVARS_SCC] ;
    strNameVars[0]  = "Raw_Lidar_Data" ;
    strNameVars[1]  = "id_timescale" ;
    strNameVars[2]  = "Background_Low" ;
    strNameVars[3]  = "Background_High" ;
    strNameVars[4]  = "channel_ID" ;
    strNameVars[5]  = "DAQ_Range" ;
    strNameVars[6]  = "Laser_Pointing_Angle" ;
    strNameVars[7]  = "Laser_Shots" ;
    strNameVars[8]  = "LR_Input" ;
    strNameVars[9]  = "Molecular_Calc" ;
    strNameVars[10] = "Pressure_at_Lidar_Station_Pa" ;
    strNameVars[11] = "Temperature_at_Lidar_Station_K" ;
    strNameVars[12] = "Raw_Data_Start_Time" ;
    strNameVars[13] = "Raw_Data_Stop_Time" ;
    strNameVars[14] = "Laser_Pointing_Angle_of_Profiles" ;

    int     id_timescale[glbParam->nCh], channel_ID[glbParam->nCh], Laser_Shots[glbParam->nEventsAVG][glbParam->nCh], LR_Input[glbParam->nCh], Molecular_Calc, Laser_Pointing_Angle_of_Profiles[glbParam->nCh];
    double  Background_Low[glbParam->nCh], Background_High[glbParam->nCh], Pressure_at_Lidar_Station_Pa, Temperature_at_Lidar_Station_K ;
    ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"nBinsBkg", (const char*)"int"   , (int*)&glbParam->nBinsBkg ) ;
    for( int c=0 ; c <glbParam->nCh ; c++ )
    {
        id_timescale[c]     = (int)0  ;
        LR_Input[c]         = (int)40 ;
        channel_ID[c]       = (int)42 ;
        Background_Low[c]   = (double) (glbParam->rEndSig_ev_ch[0][c] ) ;
        Background_High[c]  = (double) (glbParam->rEndSig_ev_ch[0][c] - glbParam->nBinsBkg * glbParam->dr) ;
        Laser_Pointing_Angle_of_Profiles[c] = (int)0 ;
        for (int t =0; t <glbParam->nEventsAVG; t++)
            Laser_Shots[t][c] = (int)glbParam->nShots[c] ;
    }
    ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"Molecular_Calc"              , (const char*)"int"   , (int*)   &Molecular_Calc ) ;
    ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"Pressure_at_Lidar_Station_Pa"   , (const char*)"double", (double*)&Pressure_at_Lidar_Station_Pa    ) ;
    ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"Temperature_at_Lidar_Station_K", (const char*)"double", (double*)&Temperature_at_Lidar_Station_K ) ;

    // DEFINE Raw_Lidar_Data VARIABLE
    DefineVarDims( (int)ncid, (int)3, (string*)dimsName, (int*)dimsSize, (int*)dim_ids, (char*)strNameVars[0].c_str(), (const char*)"double", (int*)&var_ids[0] ) ;

    DefineDims( (int)ncid, (char*)dimsName[3].c_str(), (int)dimsSize[3], (int*)&dim_ids[3] ) ;
    DefineDims( (int)ncid, (char*)dimsName[4].c_str(), (int)dimsSize[4], (int*)&dim_ids[4] ) ;

    dim_ids_StartEnd_time[0] = dim_ids[0] ;
    dim_ids_StartEnd_time[1] = dim_ids[4];

    // THE DIMENSION IS ALREADY CREATED, JUST DEFINE THE VARIABLE
    DefineVariable( (int)ncid, (char*)strNameVars[1].c_str() , (const char*)"int"   , (int)1, (int*)&dim_ids[0], (int*)&var_ids[1]  ) ;
    DefineVariable( (int)ncid, (char*)strNameVars[2].c_str() , (const char*)"double", (int)1, (int*)&dim_ids[1], (int*)&var_ids[2]  ) ;
    DefineVariable( (int)ncid, (char*)strNameVars[3].c_str() , (const char*)"double", (int)1, (int*)&dim_ids[1], (int*)&var_ids[3]  ) ;
    DefineVariable( (int)ncid, (char*)strNameVars[4].c_str() , (const char*)"int"   , (int)1, (int*)&dim_ids[1], (int*)&var_ids[4]  ) ;
    DefineVariable( (int)ncid, (char*)strNameVars[5].c_str() , (const char*)"double", (int)1, (int*)&dim_ids[1], (int*)&var_ids[5]  ) ;
    DefineVariable( (int)ncid, (char*)strNameVars[6].c_str() , (const char*)"double", (int)1, (int*)&dim_ids[3], (int*)&var_ids[6]  ) ;
    DefineVariable( (int)ncid, (char*)strNameVars[7].c_str() , (const char*)"int"   , (int)2, (int*)&dim_ids[0], (int*)&var_ids[7]  ) ;
    DefineVariable( (int)ncid, (char*)strNameVars[8].c_str() , (const char*)"int"   , (int)1, (int*)&dim_ids[1], (int*)&var_ids[8]  ) ;
    DefineVariable( (int)ncid, (char*)strNameVars[9].c_str() , (const char*)"int"   , (int)0, (int*)&dim_ids[0], (int*)&var_ids[9]  ) ;
    DefineVariable( (int)ncid, (char*)strNameVars[10].c_str(), (const char*)"double", (int)0, (int*)&dim_ids[0], (int*)&var_ids[10] ) ;
    DefineVariable( (int)ncid, (char*)strNameVars[11].c_str(), (const char*)"double", (int)0, (int*)&dim_ids[0], (int*)&var_ids[11] ) ;
    DefineVariable( (int)ncid, (char*)strNameVars[12].c_str(), (const char*)"int"   , (int)2, (int*)&dim_ids_StartEnd_time[0], (int*)&var_ids[12] ) ;
    DefineVariable( (int)ncid, (char*)strNameVars[13].c_str(), (const char*)"int"   , (int)2, (int*)&dim_ids_StartEnd_time[0], (int*)&var_ids[13] ) ;
    DefineVariable( (int)ncid, (char*)strNameVars[14].c_str(), (const char*)"int"   , (int)2, (int*)&dim_ids_StartEnd_time[0], (int*)&var_ids[14] ) ;
    string strAttListName[5], strAttList[5] ;
    strAttListName[0] = "System"                ;   strcpy ( (char*)strAttList[0].c_str(), glbParam->siteName )   ;
    strAttListName[1] = "Measurement_ID"        ;   strAttList[1].assign( Raw_Data_Start_Time_str[0], 0, 8)  ; strAttList[1].append("arge") ;
    strAttListName[2] = "RawData_Start_Date"    ;   strAttList[2].assign( Raw_Data_Start_Time_str[0], 0, 8)  ;
    strAttListName[3] = "RawData_Start_Time_UT" ;   strAttList[3].assign( Raw_Data_Start_Time_str[0], 8, 6)  ;
    strAttListName[4] = "RawData_Stop_Time_UT"  ;   strAttList[4].assign( Raw_Data_Stop_Time_str[glbParam->nEventsAVG -1], 8, 6) ;

    int size_strAttListName = sizeof(strAttListName)/sizeof(*strAttListName) ;
        Putt_Bulk_Att_Text( (int)ncid, (int)NC_GLOBAL, (int)size_strAttListName, (string*)strAttListName, (string*)strAttList ) ;

    double dblAttList[3] ;
    strAttListName[0] = "Latitude_degrees_north" ;   dblAttList[0] = (double)glbParam->siteLat  ;
    strAttListName[1] = "Longitude_degrees_east" ;   dblAttList[1] = (double)glbParam->siteLong ;
    strAttListName[2] = "Altitude_meter_asl"     ;   dblAttList[2] = (double)glbParam->siteASL  ;
        Putt_Bulk_Att_Double( (int)ncid, (int)NC_GLOBAL, (int)3, (string*)strAttListName, (double*)dblAttList ) ;

    if ( (retval = nc_enddef(ncid)) )
        ERR(retval);

    size_t start[3], count[3];
    start[0] = 0;   count[0] = 1 ; // glbParam.nEventsAVG; 
    start[1] = 0;   count[1] = 1 ; // glbParam.nCh; 
    start[2] = 0;   count[2] = glbParam->nBinsRaw ;
    for( int e=0 ; e <glbParam->nEventsAVG ; e++ )
    {
        start[0] =e ;
        for ( int c=0 ; c <glbParam->nCh ; c++ )
        {
            start[1] =c ;
            if ( (retval = nc_put_vara_double( (int)ncid, (int)var_ids[0], start, count, (double*)&dataToSave[e][c][0] ) ) )
                ERR(retval);
        }
    }
    PutVar( (int)ncid, (int)var_ids[1] , (const char*)"int"   , (int*)id_timescale                     ) ;
    PutVar( (int)ncid, (int)var_ids[2] , (const char*)"double", (double*)Background_Low                ) ;
    PutVar( (int)ncid, (int)var_ids[3] , (const char*)"double", (double*)Background_High               ) ;
    PutVar( (int)ncid, (int)var_ids[4] , (const char*)"int"   , (int*)channel_ID                       ) ;
    PutVar( (int)ncid, (int)var_ids[5] , (const char*)"double", (int*)glbParam->iMax_mVLic             ) ;
    PutVar( (int)ncid, (int)var_ids[6] , (const char*)"double", (int*)&glbParam->aZenith[0]            ) ;
    PutVar( (int)ncid, (int)var_ids[7] , (const char*)"int"   , (int*)Laser_Shots                      ) ;
    PutVar( (int)ncid, (int)var_ids[8] , (const char*)"int"   , (int*)LR_Input                         ) ;
    PutVar( (int)ncid, (int)var_ids[9] , (const char*)"int"   , (int*)&Molecular_Calc                  ) ;
    PutVar( (int)ncid, (int)var_ids[10], (const char*)"double", (double*)&Pressure_at_Lidar_Station_Pa    ) ;
    PutVar( (int)ncid, (int)var_ids[11], (const char*)"double", (double*)&Temperature_at_Lidar_Station_K ) ;
    PutVar( (int)ncid, (int)var_ids[12], (const char*)"long"  , (int*)Raw_Data_Start_Time              ) ;
    PutVar( (int)ncid, (int)var_ids[13], (const char*)"long"  , (int*)Raw_Data_Stop_Time               ) ;
    PutVar( (int)ncid, (int)var_ids[14], (const char*)"int"   , (int*)Laser_Pointing_Angle_of_Profiles ) ;

    if ( (retval = nc_close(ncid)) )
        ERR(retval);
}

void CNetCDF_Lidar::Save_LALINET_NCDF_PDL0( string Path_File_Out, strcGlobalParameters *glbParam, double ***dataToSave, long *Raw_Data_Start_Time_sec, long *Raw_Data_Stop_Time_sec, char **inputFilesInTime  )
{
    int  ncid ;
    int  retval ;

    if ( (retval = nc_create( Path_File_Out.c_str(), NC_NETCDF4, &ncid)) )
        ERR(retval);

    // DIMENSIONS INFO FOR pr VARIABLE
    string      dimsName [NDIMS_LALINET_L0] ;
    int         dimsSize [NDIMS_LALINET_L0] ;
    dimsName[0] = "time"     ;              dimsSize[0] = glbParam->nEventsAVG   ;
    dimsName[1] = "channels" ;              dimsSize[1] = glbParam->nCh          ;
    dimsName[2] = "range"    ;              dimsSize[2] = glbParam->nBinsRaw     ;
// printf("\n\n Save_LALINET_NCDF_PDL0 --> glbParam->nBinsRaw= %d \n", glbParam->nBinsRaw) ;
    // int  dim_ids_StartEnd_time[2];
    int  dim_ids[NDIMS_LALINET_L0];
    int  var_ids[NVARS_LALINET_L0] ;
    string  strNameVars[NVARS_LALINET_L0] ;
    strNameVars[0]  = "Raw_Lidar_Data" ;
    strNameVars[1]  = "Raw_Data_Start_Time" ;
    strNameVars[2]  = "Raw_Data_Stop_Time" ;
    strNameVars[3]  = "Wavelengths" ;
    strNameVars[4]  = "Polarization" ;
    strNameVars[5]  = "DAQ_Range" ;
    strNameVars[6]  = "DAQ_type" ;
    strNameVars[7]  = "PMT_Voltage" ;
    strNameVars[8]  = "Accumulated_Pulses" ;
    strNameVars[9]  = "Zenith" ;
    strNameVars[10] = "Azimuth" ;
    strNameVars[11] = "Temperature" ;
    strNameVars[12] = "Pressure" ;
    strNameVars[13] = "ADC_Bits" ;
    strNameVars[14] = "Laser_Source" ;
    strNameVars[15] = "nBins_Ch" ; 
    strNameVars[16] = "File_Names" ;
    strNameVars[17] = "range" ;
// printf("\n Save_LALINET_NCDF_PDL0 --> glbParam->nCh= %d \n", glbParam->nCh) ;
    // DEFINE Raw_Lidar_Data VARIABLE
    DefineVarDims( (int)ncid, (int)3, (string*)dimsName, (int*)dimsSize, (int*)dim_ids, (char*)strNameVars[0].c_str(), (const char*)"double", (int*)&var_ids[0] ) ; // Raw_Lidar_Data

    DefineVariable( (int)ncid, (char*)strNameVars[1].c_str() , (const char*)"int"   , (int)1, (int*)&dim_ids[0], (int*)&var_ids[1]  ) ; // Raw_Data_Start_Time
    DefineVariable( (int)ncid, (char*)strNameVars[2].c_str() , (const char*)"int"   , (int)1, (int*)&dim_ids[0], (int*)&var_ids[2]  ) ; // Raw_Data_Stop_Time
    DefineVariable( (int)ncid, (char*)strNameVars[3].c_str() , (const char*)"int"   , (int)1, (int*)&dim_ids[1], (int*)&var_ids[3]  ) ; // Wavelengths
    DefineVariable( (int)ncid, (char*)strNameVars[4].c_str() , (const char*)"char"  , (int)1, (int*)&dim_ids[1], (int*)&var_ids[4]  ) ; // Polarization
    DefineVariable( (int)ncid, (char*)strNameVars[5].c_str() , (const char*)"double", (int)1, (int*)&dim_ids[1], (int*)&var_ids[5]  ) ; // DAQ_Range
    DefineVariable( (int)ncid, (char*)strNameVars[6].c_str() , (const char*)"int"   , (int)1, (int*)&dim_ids[1], (int*)&var_ids[6]  ) ; // DAQ_type
    DefineVariable( (int)ncid, (char*)strNameVars[7].c_str() , (const char*)"int"   , (int)1, (int*)&dim_ids[1], (int*)&var_ids[7]  ) ; // PMT_Voltage
    DefineVariable( (int)ncid, (char*)strNameVars[8].c_str() , (const char*)"int"   , (int)1, (int*)&dim_ids[1], (int*)&var_ids[8]  ) ; // Accumulated_Pulses
    DefineVariable( (int)ncid, (char*)strNameVars[9].c_str() , (const char*)"double", (int)1, (int*)&dim_ids[0], (int*)&var_ids[9]  ) ; // Zenith
    DefineVariable( (int)ncid, (char*)strNameVars[10].c_str(), (const char*)"double", (int)1, (int*)&dim_ids[0], (int*)&var_ids[10] ) ; // Azimuth
    DefineVariable( (int)ncid, (char*)strNameVars[13].c_str(), (const char*)"int"   , (int)1, (int*)&dim_ids[1], (int*)&var_ids[13] ) ; // ADC_Bits
    DefineVariable( (int)ncid, (char*)strNameVars[14].c_str(), (const char*)"int"   , (int)1, (int*)&dim_ids[1], (int*)&var_ids[14] ) ; // Laser_Source
    DefineVariable( (int)ncid, (char*)strNameVars[15].c_str(), (const char*)"int"   , (int)1, (int*)&dim_ids[1], (int*)&var_ids[15] ) ; // nBins_Ch
    DefineVariable( (int)ncid, (char*)strNameVars[16].c_str(), (const char*)"string", (int)1, (int*)&dim_ids[0], (int*)&var_ids[16] ) ; // File_Names
    DefineVariable( (int)ncid, (char*)strNameVars[17].c_str(), (const char*)"double", (int)1, (int*)&dim_ids[2], (int*)&var_ids[17] ) ; // range (ex points)
// printf("\n Save_LALINET_NCDF_PDL0 --> \n") ;
    // TEXT GLOBAL ATTRIBUTES
    string strAttListName[10], strAttList ;
    strAttListName[0] = "Site_Name"             ;   strcpy ( (char*)strAttList.c_str(), glbParam->siteName )  ;
        Putt_Bulk_Att_Text( (int)ncid, (int)NC_GLOBAL, (int)1, (string*)strAttListName, (string*)&strAttList ) ;

    // DOUBLE GLOBAL ATTRIBUTES
    double dblAttList[6] ;
    strAttListName[0] = "Altitude_meter_asl"     ;   dblAttList[0] = (double)glbParam->siteASL       ;
    strAttListName[1] = "Latitude_degrees_north" ;   dblAttList[1] = (double)glbParam->siteLat       ;
    strAttListName[2] = "Longitude_degrees_east" ;   dblAttList[2] = (double)glbParam->siteLong      ;
    strAttListName[3] = "Range_Resolution"       ;   dblAttList[3] = (double)glbParam->dr            ;
    strAttListName[4] = "Laser_Frec_1"           ;   dblAttList[4] = (double)glbParam->Laser_Frec[0] ;
    strAttListName[5] = "Laser_Frec_2"           ;   dblAttList[5] = (double)glbParam->Laser_Frec[1] ;
        Putt_Bulk_Att_Double( (int)ncid, (int)NC_GLOBAL, (int)6, (string*)strAttListName, (double*)dblAttList ) ;
// printf("\n Save_LALINET_NCDF_PDL0 --> (1) \n") ;
    // UNITS OF THE VARIABLES
        Set_LALINET_Units_L0( (int)ncid, (int*)var_ids ) ;
    if ( (retval = nc_enddef(ncid)) )
        ERR(retval);

    // PUTTING VARIABLES
    size_t start[3], count[3];
    start[0] = 0;   count[0] = 1 ; // glbParam.nEventsAVG; 
    start[1] = 0;   count[1] = 1 ; // glbParam.nCh; 
    start[2] = 0;   count[2] = glbParam->nBinsRaw ;
// printf("\n Save_LALINET_NCDF_PDL0 --> (2) - glbParam->nEventsAVG = %d \n", glbParam->nEventsAVG) ;
    for( int e=0 ; e <glbParam->nEventsAVG ; e++ )
    {
        start[0] =e ;
        for ( int c=0 ; c <glbParam->nCh ; c++ )
        {
            start[1] =c ;
            if ( (retval = nc_put_vara_double((int)ncid, (int)var_ids[0], start, count, (double*)&dataToSave[e][c][0] ) ) )
                ERR(retval);
        }
    }
// printf("\n Save_LALINET_NCDF_PDL0 --> (3) \n") ;
    PutVar( (int)ncid, (int)var_ids[1] , (const char*)"long"  , (long*)Raw_Data_Start_Time_sec      ) ; 
    PutVar( (int)ncid, (int)var_ids[2] , (const char*)"long"  , (long*)Raw_Data_Stop_Time_sec       ) ;
    PutVar( (int)ncid, (int)var_ids[3] , (const char*)"int"   , (int*)glbParam->iLambda             ) ;
    PutVar( (int)ncid, (int)var_ids[4] , (const char*)"char"  , (char*)glbParam->sPol               ) ;
    PutVar( (int)ncid, (int)var_ids[5] , (const char*)"double", (double*)glbParam->iMax_mVLic       ) ;
    PutVar( (int)ncid, (int)var_ids[6] , (const char*)"int"   , (int*)glbParam->DAQ_Type            ) ;
    PutVar( (int)ncid, (int)var_ids[7] , (const char*)"int"   , (int*)glbParam->PMT_Voltage         ) ;
    PutVar( (int)ncid, (int)var_ids[8] , (const char*)"int"   , (int*)glbParam->nShots              ) ;
    PutVar( (int)ncid, (int)var_ids[9] , (const char*)"double", (double*)glbParam->aZenithAVG       ) ;
    PutVar( (int)ncid, (int)var_ids[10], (const char*)"double", (double*)glbParam->aAzimuthAVG      ) ;
    // PutVar( (int)ncid, (int)var_ids[9] , (const char*)"double", (double*)glbParam->aZenith          ) ;
    // PutVar( (int)ncid, (int)var_ids[10], (const char*)"double", (double*)glbParam->aAzimuth         ) ;
    PutVar( (int)ncid, (int)var_ids[13], (const char*)"int"   , (int*)glbParam->iADCbits            ) ;
    PutVar( (int)ncid, (int)var_ids[14], (const char*)"int"   , (int*)glbParam->Laser_Src           ) ;
    PutVar( (int)ncid, (int)var_ids[15], (const char*)"int"   , (int*)glbParam->nBins_Ch            ) ;
    PutVar_String( (int)ncid, (int)var_ids[16], (char**)inputFilesInTime     ) ;
    PutVar( (int)ncid, (int)var_ids[17], (const char*)"double", (double*)glbParam->r                ) ;
// printf("\n Save_LALINET_NCDF_PDL0 --> (1) \n") ;
    if ( (retval = nc_close(ncid)) )
        ERR(retval);
}
/*
void CNetCDF_Lidar::Save_LALINET_NCDF_PDL0( string Path_File_Out, strcGlobalParameters *glbParam, double ***dataToSave, long *Raw_Data_Start_Time_sec, long *Raw_Data_Stop_Time_sec )
{
    int  ncid ;
    int  retval ;

    if ( (retval = nc_create( Path_File_Out.c_str(), NC_NETCDF4, &ncid)) )
        ERR(retval);

    // DIMENSIONS INFO FOR pr VARIABLE
    string      dimsName [NDIMS_LALINET_L0] ;
    int         dimsSize[NDIMS_LALINET_L0] ;
    dimsName[0] = "time" ;                  dimsSize[0] = glbParam->nEventsAVG   ;
    dimsName[1] = "channels" ;              dimsSize[1] = glbParam->nCh          ;
    dimsName[2] = "points" ;                dimsSize[2] = glbParam->nBinsRaw     ;

    // int  dim_ids_StartEnd_time[2];
    int  dim_ids[NDIMS_LALINET_L0];
    int  var_ids[NVARS_LALINET_L0] ;
    string  strNameVars[NVARS_LALINET_L0] ;
    strNameVars[0]  = "Raw_Lidar_Data" ;
    strNameVars[1]  = "Raw_Data_Start_Time" ;
    strNameVars[2]  = "Raw_Data_Stop_Time" ;
    strNameVars[3]  = "Wavelengths" ;
    strNameVars[4]  = "Polarization" ;
    strNameVars[5]  = "DAQ_Range" ;
    strNameVars[6]  = "DAQ_type" ;
    strNameVars[7]  = "PMT_Voltage" ;
    strNameVars[8]  = "Accumulated_Pulses" ;
    strNameVars[9]  = "Zenith" ;
    strNameVars[10] = "Azimuth" ;
    strNameVars[11] = "Temperature" ;
    strNameVars[12] = "Pressure" ;
    strNameVars[13] = "ADC_Bits" ;
    strNameVars[14] = "Laser_Source" ;
    strNameVars[15] = "nBins_Ch" ;

    // DEFINE Raw_Lidar_Data VARIABLE
    DefineVarDims( (int)ncid, (int)3, (string*)dimsName, (int*)dimsSize, (int*)dim_ids, (char*)strNameVars[0].c_str(), (const char*)"double", (int*)&var_ids[0] ) ; // Raw_Lidar_Data

    DefineVariable( (int)ncid, (char*)strNameVars[1].c_str() , (const char*)"int"   , (int)1, (int*)&dim_ids[0], (int*)&var_ids[1]  ) ; // Raw_Data_Start_Time
    DefineVariable( (int)ncid, (char*)strNameVars[2].c_str() , (const char*)"int"   , (int)1, (int*)&dim_ids[0], (int*)&var_ids[2]  ) ; // Raw_Data_Stop_Time
    DefineVariable( (int)ncid, (char*)strNameVars[3].c_str() , (const char*)"int"   , (int)1, (int*)&dim_ids[1], (int*)&var_ids[3]  ) ; // Wavelengths
    DefineVariable( (int)ncid, (char*)strNameVars[4].c_str() , (const char*)"char"  , (int)1, (int*)&dim_ids[1], (int*)&var_ids[4]  ) ; // Polarization
    DefineVariable( (int)ncid, (char*)strNameVars[5].c_str() , (const char*)"double", (int)1, (int*)&dim_ids[1], (int*)&var_ids[5]  ) ; // DAQ_Range
    DefineVariable( (int)ncid, (char*)strNameVars[6].c_str() , (const char*)"int"   , (int)1, (int*)&dim_ids[1], (int*)&var_ids[6]  ) ; // DAQ_type
    DefineVariable( (int)ncid, (char*)strNameVars[7].c_str() , (const char*)"int"   , (int)1, (int*)&dim_ids[1], (int*)&var_ids[7]  ) ; // PMT_Voltage
    DefineVariable( (int)ncid, (char*)strNameVars[8].c_str() , (const char*)"int"   , (int)1, (int*)&dim_ids[1], (int*)&var_ids[8]  ) ; // Accumulated_Pulses
    DefineVariable( (int)ncid, (char*)strNameVars[9].c_str() , (const char*)"double", (int)1, (int*)&dim_ids[0], (int*)&var_ids[9]  ) ; // Zenith
    DefineVariable( (int)ncid, (char*)strNameVars[10].c_str(), (const char*)"double", (int)1, (int*)&dim_ids[0], (int*)&var_ids[10] ) ; // Azimuth
    DefineVariable( (int)ncid, (char*)strNameVars[13].c_str(), (const char*)"int"   , (int)1, (int*)&dim_ids[1], (int*)&var_ids[13] ) ; // ADC_Bits
    DefineVariable( (int)ncid, (char*)strNameVars[14].c_str(), (const char*)"int"   , (int)1, (int*)&dim_ids[1], (int*)&var_ids[14] ) ; // Laser_Source
    DefineVariable( (int)ncid, (char*)strNameVars[15].c_str(), (const char*)"int"   , (int)1, (int*)&dim_ids[1], (int*)&var_ids[15] ) ; // nBins_Ch

    // TEXT GLOBAL ATTRIBUTES
    string strAttListName[10], strAttList ;
    strAttListName[0] = "Site_Name"             ;   strcpy ( (char*)strAttList.c_str(), glbParam->siteName )  ;
        Putt_Bulk_Att_Text( (int)ncid, (int)NC_GLOBAL, (int)1, (string*)strAttListName, (string*)&strAttList ) ;

    // DOUBLE GLOBAL ATTRIBUTES
    double dblAttList[6] ;
    strAttListName[0] = "Altitude_meter_asl"     ;   dblAttList[0] = (double)glbParam->siteASL       ;
    strAttListName[1] = "Latitude_degrees_north" ;   dblAttList[1] = (double)glbParam->siteLat       ;
    strAttListName[2] = "Longitude_degrees_east" ;   dblAttList[2] = (double)glbParam->siteLong      ;
    strAttListName[3] = "Range_Resolution"       ;   dblAttList[3] = (double)glbParam->dr            ;
    strAttListName[4] = "Laser_Frec_1"           ;   dblAttList[4] = (double)glbParam->Laser_Frec[0] ;
    strAttListName[5] = "Laser_Frec_2"           ;   dblAttList[5] = (double)glbParam->Laser_Frec[1] ;
        Putt_Bulk_Att_Double( (int)ncid, (int)NC_GLOBAL, (int)6, (string*)strAttListName, (double*)dblAttList ) ;

    // UNITS OF THE VARIABLES
        Set_LALINET_Units_L0( (int)ncid, (int*)var_ids ) ;

    if ( (retval = nc_enddef(ncid)) )
        ERR(retval);

    // PUTTING VARIABLES
    size_t start[3], count[3];
    start[0] = 0;   count[0] = 1 ; // glbParam.nEventsAVG; 
    start[1] = 0;   count[1] = 1 ; // glbParam.nCh; 
    start[2] = 0;   count[2] = glbParam->nBinsRaw ;
    for( int e=0 ; e <glbParam->nEventsAVG ; e++ )
    {
        start[0] =e ;
        for ( int c=0 ; c <glbParam->nCh ; c++ )
        {
            start[1] =c ;
            if ( (retval = nc_put_vara_double((int)ncid, (int)var_ids[0], start, count, (double*)&dataToSave[e][c][0] ) ) )
                ERR(retval);
        }
    }

    PutVar( (int)ncid, (int)var_ids[1] , (const char*)"long"  , (long*)Raw_Data_Start_Time_sec       ) ; 
    PutVar( (int)ncid, (int)var_ids[2] , (const char*)"long"  , (long*)Raw_Data_Stop_Time_sec        ) ;
    PutVar( (int)ncid, (int)var_ids[3] , (const char*)"int"   , (int*)glbParam->iLambda             ) ;
    PutVar( (int)ncid, (int)var_ids[4] , (const char*)"char"  , (char*)glbParam->sPol               ) ;
    PutVar( (int)ncid, (int)var_ids[5] , (const char*)"double", (double*)glbParam->iMax_mVLic       ) ;
    PutVar( (int)ncid, (int)var_ids[6] , (const char*)"int"   , (int*)glbParam->DAQ_Type             ) ;
    PutVar( (int)ncid, (int)var_ids[7] , (const char*)"int"   , (int*)glbParam->PMT_Voltage         ) ;
    PutVar( (int)ncid, (int)var_ids[8] , (const char*)"int"   , (int*)glbParam->nShots              ) ;
    PutVar( (int)ncid, (int)var_ids[9] , (const char*)"double", (double*)glbParam->aZenith          ) ;
    PutVar( (int)ncid, (int)var_ids[10], (const char*)"double", (double*)glbParam->aAzimuth         ) ;
    PutVar( (int)ncid, (int)var_ids[13], (const char*)"int"   , (int*)glbParam->iADCbits            ) ;
    PutVar( (int)ncid, (int)var_ids[14], (const char*)"int"   , (int*)glbParam->Laser_Src           ) ;
    PutVar( (int)ncid, (int)var_ids[15], (const char*)"int"   , (int*)glbParam->nBins_Ch         ) ;

    if ( (retval = nc_close(ncid)) )
        ERR(retval);
}
*/

void CNetCDF_Lidar::Add_Noise_LALINET_NCDF_PDL0( string *Path_File_Out, strcGlobalParameters *glbParam, double **data_Noise )
{
    int retval, nc_id ;
    if ( ( retval = nc_open( Path_File_Out->c_str(), NC_WRITE, &nc_id ) ) )
    {
        printf("\n (1) \n") ;
        ERR(retval) ;
    }
    if ( ( retval = nc_redef( (int)nc_id ) ) )
    {
        printf("\n (2) \n") ;
        ERR(retval);
    }

    int id_dim_noise[2], id_var_noise ; 
    if ( (retval = nc_inq_dimid ( nc_id, "channels", (int*)&id_dim_noise[0] ) ) )
            ERR(retval);
    if ( (retval = nc_inq_dimid ( nc_id, "range", (int*)&id_dim_noise[1] ) ) )
            ERR(retval);
  
    DefineVariable( (int)nc_id, (char*)"Bkg_Noise", (const char*)"double", (int)2, (int*)&id_dim_noise[0], (int*)&id_var_noise ) ;

    if ( (retval = nc_enddef(nc_id)) )
        ERR(retval);

    size_t start_noise[2], count_noise[2];
    start_noise[0] = 0;   count_noise[0] = 1 ;
    start_noise[1] = 0;   count_noise[1] = glbParam->nBins ;
    for ( int c=0 ; c <glbParam->nCh ; c++ )
    {
        start_noise[0] =c ;
        if ( (retval = nc_put_vara_double( (int)nc_id, (int)id_var_noise, start_noise, count_noise, (double*)&data_Noise[c][0] ) ) )
            ERR(retval);
    }

    if ( (retval = nc_close(nc_id) ) )
        ERR(retval);

}

void CNetCDF_Lidar::Add_Overlap_LALINET_NCDF_PDL0( string *Path_File_Out, strcGlobalParameters *glbParam, double **ovlp )
{
    int retval, nc_id ;
    if ( ( retval = nc_open( Path_File_Out->c_str(), NC_WRITE, &nc_id ) ) )
    {
        printf("\nAdd_Overlap_LALINET_NCDF_PDL0() (1) \n") ;
        ERR(retval) ;
    }
    if ( ( retval = nc_redef( (int)nc_id ) ) )
    {
        printf("\nAdd_Overlap_LALINET_NCDF_PDL0() (2) \n") ;
        ERR(retval);
    }

    int id_dim_ovlp[2], id_var_ovlp ; 
    if ( (retval = nc_inq_dimid ( nc_id, "channels", (int*)&id_dim_ovlp[0] ) ) )
            ERR(retval);
    if ( (retval = nc_inq_dimid ( nc_id, "range", (int*)&id_dim_ovlp[1] ) ) )
            ERR(retval);
  
    DefineVariable( (int)nc_id, (char*)"Overlap", (const char*)"double", (int)2, (int*)&id_dim_ovlp[0], (int*)&id_var_ovlp ) ;

    if ( (retval = nc_enddef(nc_id)) )
        ERR(retval);

    size_t start_ovlp[2], count_ovlp[2];
    start_ovlp[0] = 0 ; count_ovlp[0] = 1 ; // glbParam.nCh; 
    start_ovlp[1] = 0 ; count_ovlp[1] = glbParam->nBins ;
    for ( int c=0 ; c <glbParam->nCh ; c++ )
    {
        start_ovlp[0] =c ;
        count_ovlp[1] = glbParam->nBins_Ch[c] ;
        if ( (retval = nc_put_vara_double( (int)nc_id, (int)id_var_ovlp, start_ovlp, count_ovlp, (double*)&(ovlp[c][0]) ) ) )
            ERR(retval);
    }

    if ( (retval = nc_close(nc_id) ) )
        ERR(retval);

}

void CNetCDF_Lidar::Save_LALINET_NCDF_PDL1( char *Path_File_Out, strcGlobalParameters *glbParam, double **RMSElay, double *RMSerr_Ref, int **Cloud_Profiles, 
                                            double ***pr_corr, int *Start_Time_AVG, int *Stop_Time_AVG, CMolecularData *oMolData )
{
    int retval, nc_id ;
    if ( ( retval = nc_open( Path_File_Out, NC_WRITE, &nc_id ) ) )
    {
        ERR(retval) ;
    }
    if ( ( retval = nc_redef( (int)nc_id ) ) )
    {
        ERR(retval);
    }

    int nc_id_group_L1 ;
    if ( (retval = nc_def_grp ( (int)nc_id, (const char*)"L1_Data", (int*)&nc_id_group_L1 ) ) )
    {
        ERR(retval) ;
    }

    int var_ids[NVARS_LALINET_L1] ;
    int dim_ids[NDIMS_LALINET_L1] ; // 0: TIME    1: CHANNELS     2:RANGE
    string      dimsName [NDIMS_LALINET_L1] ;
    int         dimsSize [NDIMS_LALINET_L1] ;
    dimsName[0] = "time"     ;              dimsSize[0] = glbParam->nEventsAVG   ;
    dimsName[1] = "channels" ;              dimsSize[1] = glbParam->nCh          ;
    dimsName[2] = "range"    ;              dimsSize[2] = glbParam->nBins        ;

    // DEFINE Raw_Lidar_Data_L1 AND ITS DIMENSIONS
    DefineVarDims( (int)nc_id_group_L1, (int)3, (string*)dimsName, (int*)dimsSize, (int*)dim_ids, (char*)"Raw_Lidar_Data_L1", (const char*)"double", (int*)&var_ids[0] ) ;

    DefineVariable( (int)nc_id_group_L1, (char*)"Start_Time_AVG_L1"       , (const char*)"int"   , (int)1, (int*)&dim_ids[0], (int*)&var_ids[1] ) ; // Start_Time_AVG_L1
    DefineVariable( (int)nc_id_group_L1, (char*)"Stop_Time_AVG_L1"        , (const char*)"int"   , (int)1, (int*)&dim_ids[0], (int*)&var_ids[2] ) ; // StOP_Time_AVG_L1
    DefineVariable( (int)nc_id_group_L1, (char*)"Azimuth_AVG_L1"          , (const char*)"double", (int)1, (int*)&dim_ids[0], (int*)&var_ids[3] ) ; // Azimuth_AVG_L1
    DefineVariable( (int)nc_id_group_L1, (char*)"Zenith_AVG_L1"           , (const char*)"double", (int)1, (int*)&dim_ids[0], (int*)&var_ids[4] ) ; // Zenith_AVG_L1"
    DefineVariable( (int)nc_id_group_L1, (char*)"Laser_Zero_Bin_Offset"   , (const char*)"int"   , (int)1, (int*)&dim_ids[1], (int*)&var_ids[5] ) ; // Laser_Zero_Bin_Offset
    DefineVariable( (int)nc_id_group_L1, (char*)"MaxRangeAnalysis"        , (const char*)"double", (int)2, (int*)&dim_ids[0], (int*)&var_ids[6] ) ; // MaxRangeAnalysis

    DefineVariable( (int)nc_id_group_L1, (char*)"Temperature_K"           , (const char*)"double", (int)1, (int*)&dim_ids[2], (int*)&var_ids[7] ) ; // Molecular profile
    DefineVariable( (int)nc_id_group_L1, (char*)"Pressure_Pa"             , (const char*)"double", (int)1, (int*)&dim_ids[2], (int*)&var_ids[8] ) ; // Molecular profile

    DefineVariable( (int)nc_id_group_L1, (char*)"Temperature_Ground_Level", (const char*)"double", (int)1, (int*)&dim_ids[0], (int*)&var_ids[9]  ) ; // Pressure_Ground_Level
    DefineVariable( (int)nc_id_group_L1, (char*)"Pressure_Ground_Level"   , (const char*)"double", (int)1, (int*)&dim_ids[0], (int*)&var_ids[10] ) ; // Temperature_Ground_Level

    DefineVariable( (int)nc_id_group_L1, (char*)"range"                   , (const char*)"double", (int)1, (int*)&dim_ids[2], (int*)&var_ids[11] ) ; // range (ex points)

    int dim_ids_CM[2] ;
    dim_ids_CM[0] = dim_ids[0] ; // TIME
    dim_ids_CM[1] = dim_ids[2] ; // POINTS
    DefineVariable( (int)nc_id_group_L1, (char*)"Cloud_Mask", (const char*)"int", (int)2, (int*)&dim_ids_CM[0], (int*)&var_ids[12] ) ;

    string strAttListName[5] ;
    int     intAttList   [5] ;
    int     indxWL_PDL1 ;
    ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"indxWL_PDL1"          , (const char*)"int", (int*)&indxWL_PDL1           ) ;
    int avg_Points_Cloud_Mask ;
    ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"avg_Points_Cloud_Mask", (const char*)"int", (int*)&avg_Points_Cloud_Mask ) ;
    strAttListName[0] = "indxChannel_for_Cloud_Mask"; intAttList[0] = (int)indxWL_PDL1                  ;
    strAttListName[1] = "num_Points_Bkg"            ; intAttList[1] = (int)glbParam->nBinsBkg           ;
    strAttListName[2] = "Averaged_Profiles_L1"      ; intAttList[2] = (int)glbParam->numEventsToAvg     ;
    strAttListName[3] = "avg_Points_Cloud_Mask"     ; intAttList[3] = (int)avg_Points_Cloud_Mask        ;
    strAttListName[4] = "Max_Mol_Range"             ; intAttList[4] = (int)oMolData->dataMol.z_Mol_Max  ;
        Putt_Bulk_Att_Int( (int)nc_id_group_L1, (int)NC_GLOBAL, (int)5, (string*)strAttListName, (int*)intAttList ) ;

    Set_LALINET_Units_L1( (int)nc_id_group_L1, (int*)var_ids ) ;

        if ( (retval = nc_enddef(nc_id_group_L1)) )
        {
            ERR(retval);
        }

    // WRITE LASER ZERO BIN OFFSET
    PutVar( (int)nc_id_group_L1, (int)var_ids[5], (const char*)"int", (int*)glbParam->indxOffset ) ;

    // WRITE TEMPERATURE AND PRESSURE PROFILES FROM THE RADIOSONDE IN HIGH RESOLUTION
    PutVar( (int)nc_id_group_L1, (int)var_ids[7], (const char*)"double", (double*)&oMolData->dataMol.tK[0]  ) ;
    PutVar( (int)nc_id_group_L1, (int)var_ids[8], (const char*)"double", (double*)&oMolData->dataMol.pPa[0] ) ;
    // PutVar( (int)nc_id_group_L1, (int)var_ids[7], (const char*)"double", (double*)&oMolData->dataMol.prMol[0] ) ;
    // PutVar( (int)nc_id_group_L1, (int)var_ids[8], (const char*)"double", (double*)&oMolData->dataMol.pr2Mol[0] ) ;
    // PutVar( (int)nc_id_group_L1, (int)var_ids[7], (const char*)"double", (double*)&oMolData->dataMol.betaMol[0] ) ;
    // PutVar( (int)nc_id_group_L1, (int)var_ids[8], (const char*)"double", (double*)&oMolData->dataMol.alphaMol[0] ) ;

    size_t start_CM[2], count_CM[2];
    size_t start_ES[2], count_ES[2];
    start_CM[0] = 0;   count_CM[0] = 1 ;  // TIME
    start_CM[1] = 0;   count_CM[1] = glbParam->nBins ; // POINTS
    start_ES[0] = 0;   count_ES[0] = 1 ;  // TIME
    start_ES[1] = 0;   count_ES[1] = glbParam->nCh ; // CHANNELS
    for( int e=0 ; e <glbParam->nEventsAVG  ; e++ )
    {
        start_CM[0] =e ;
        if ( (retval = nc_put_vara_int( (int)nc_id_group_L1, (int)var_ids[12] , start_CM, count_CM, (int*)&Cloud_Profiles[e][0] ) ) )
            ERR(retval);

        start_ES[0] =e ;
        if ( (retval = nc_put_vara_double( (int)nc_id_group_L1, (int)var_ids[6], start_ES, count_ES, (double*)&glbParam->rEndSig_ev_ch[e][0] ) ) )
            ERR(retval);
    }

    PutVar( (int)nc_id_group_L1, (int)var_ids[3] , (const char*)"double", (double*)glbParam->aAzimuthAVG ) ;
    PutVar( (int)nc_id_group_L1, (int)var_ids[4] , (const char*)"double", (double*)glbParam->aZenithAVG  ) ;

    PutVar( (int)nc_id_group_L1, (int)var_ids[9]  , (const char*)"double", (double*)glbParam->temp_K_agl_AVG   ) ;
    PutVar( (int)nc_id_group_L1, (int)var_ids[10] , (const char*)"double", (double*)glbParam->pres_Pa_agl_AVG  ) ;

    PutVar( (int)nc_id_group_L1, (int)var_ids[1] , (const char*)"int"   , (long*)  Start_Time_AVG        ) ;
    PutVar( (int)nc_id_group_L1, (int)var_ids[2] , (const char*)"int"   , (long*)  Stop_Time_AVG         ) ;
    PutVar( (int)nc_id_group_L1, (int)var_ids[11], (const char*)"double", (double*)glbParam->r           ) ;

    // WRITE CLOUD RAW LIDAR DATA CORRECTED
    size_t start_pr[3], count_pr[3];
    start_pr[0] = 0;   count_pr[0] = 1 ; // glbParam.nEventsAVG; 
    start_pr[1] = 0;   count_pr[1] = 1 ; // glbParam.nCh; 
    start_pr[2] = 0;   count_pr[2] = glbParam->nBins ;
    for( int e=0 ; e <glbParam->nEventsAVG ; e++ )
    {
        start_pr[0] =e ;
        for ( int c=0 ; c <glbParam->nCh ; c++ )
        {
            start_pr[1] =c ;
            if ( (retval = nc_put_vara_double((int)nc_id_group_L1, (int)var_ids[0], start_pr, count_pr, (double*)&pr_corr[e][c][0] ) ) )
                ERR(retval);
        }
    }

    if ( (retval = nc_close(nc_id) ) )
        ERR(retval);
}

void CNetCDF_Lidar::Save_LALINET_NCDF_PDL2( char *Path_File_Out, strcGlobalParameters *glbParam, CDataLevel_2 *oDL2 )
{
    int retval, nc_id ;
    if ( ( retval = nc_open( Path_File_Out, NC_WRITE, &nc_id ) ) )
        ERR(retval);
    if ( ( retval = nc_redef( (int)nc_id ) ) )
        ERR(retval);

    int nc_id_group_L2 ;
    if ( (retval = nc_def_grp ( (int)nc_id, (const char*)"L2_Data", (int*)&nc_id_group_L2 ) ) )
        ERR(retval) ;

// id_dims_aer[0]: nEventsAVG
// id_dims_aer[1]: nLRs
// id_dims_aer[2]: nBins
    int num_dim_var = 3 ;
    int *id_dims_aer     = (int*) new int [num_dim_var] ;
    int *id_dims_pr2     = (int*) new int [num_dim_var] ;
    DefineDims( (int)nc_id_group_L2, (char*)"time"        , (int)glbParam->nEventsAVG    , (int*)&id_dims_aer[0] ) ;
    DefineDims( (int)nc_id_group_L2, (char*)"LRs"         , (int)oDL2->nLRs              , (int*)&id_dims_aer[1] ) ;
    DefineDims( (int)nc_id_group_L2, (char*)"range"       , (int)glbParam->nBins         , (int*)&id_dims_aer[2] ) ;

    id_dims_pr2[0] = id_dims_aer[0] ;
    DefineDims( (int)nc_id_group_L2, (char*)"channels", (int)glbParam->nCh, (int*)&id_dims_pr2[1] ) ;
    id_dims_pr2[2] = id_dims_aer[2] ;

    int id_dims_AERONET[2], id_dims_LIDAR_AERONET[2] ;
    int nc_id_group_L2_AERONET, nc_id_group_L2_LIDAR_AERONET ;
    if ( strcmp( oDL2->aeronet_file, "NOT_FOUND") !=0 )
    {
        if ( (retval = nc_def_grp ( (int)nc_id_group_L2, (const char*)"AERONET_Data", (int*)&nc_id_group_L2_AERONET ) ) )
            ERR(retval) ;
        DefineDims( (int)nc_id_group_L2_AERONET, (char*)"time_AERONET", (int)oDL2->i_Num_AERONET_data, (int*)&id_dims_AERONET[0] ) ;
        DefineDims( (int)nc_id_group_L2_AERONET, (char*)"time_LIDAR"  , (int)glbParam->nEventsAVG    , (int*)&id_dims_AERONET[1] ) ;

        if ( (retval = nc_def_grp ( (int)nc_id_group_L2, (const char*)"LIDAR_AERONET_synergy", (int*)&nc_id_group_L2_LIDAR_AERONET ) ) )
            ERR(retval) ;
        DefineDims( (int)nc_id_group_L2_LIDAR_AERONET, (char*)"time_LIDAR", (int)glbParam->nEventsAVG, (int*)&id_dims_LIDAR_AERONET[0] ) ;
        DefineDims( (int)nc_id_group_L2_LIDAR_AERONET, (char*)"range"     , (int)glbParam->nBins     , (int*)&id_dims_LIDAR_AERONET[1] ) ;
    }

    int var_ids[NVARS_LALINET_L2] ;
    DefineVariable( (int)nc_id_group_L2, (char*)"Range_Corrected_Lidar_Signal_L2", (const char*)"double", (int)num_dim_var, (int*)&id_dims_pr2[0]    , (int*)&var_ids[2]  ) ;
    DefineVariable( (int)nc_id_group_L2, (char*)"Start_Time_AVG_L2"              , (const char*)"int"   , (int)1          , (int*)&id_dims_aer[0]    , (int*)&var_ids[3]  ) ;
    DefineVariable( (int)nc_id_group_L2, (char*)"Stop_Time_AVG_L2"               , (const char*)"int"   , (int)1          , (int*)&id_dims_aer[0]    , (int*)&var_ids[4]  ) ;
    DefineVariable( (int)nc_id_group_L2, (char*)"Zenith_AVG_L2"                  , (const char*)"double", (int)1          , (int*)&id_dims_pr2[0]    , (int*)&var_ids[5]  ) ;
    DefineVariable( (int)nc_id_group_L2, (char*)"Azimuth_AVG_L2"                 , (const char*)"double", (int)1          , (int*)&id_dims_pr2[0]    , (int*)&var_ids[6]  ) ;
    DefineVariable( (int)nc_id_group_L2, (char*)"MaxRangeAnalysis"               , (const char*)"double", (int)2          , (int*)&id_dims_pr2[0]    , (int*)&var_ids[7]  ) ;
    DefineVariable( (int)nc_id_group_L2, (char*)"LRs"                            , (const char*)"double", (int)1          , (int*)&id_dims_aer[1]    , (int*)&var_ids[8]  ) ;
    DefineVariable( (int)nc_id_group_L2, (char*)"AOD_LR"                         , (const char*)"double", (int)2          , (int*)&id_dims_aer[0]    , (int*)&var_ids[9]  ) ;
    DefineVariable( (int)nc_id_group_L2, (char*)"Ref_Range_ASL_m"                , (const char*)"double", (int)1          , (int*)&id_dims_aer[0]    , (int*)&var_ids[10] ) ;
    DefineVariable( (int)nc_id_group_L2, (char*)"Fernald_smooth_bins"            , (const char*)"int"   , (int)1          , (int*)&id_dims_pr2[1]    , (int*)&var_ids[11] ) ;
    DefineVariable( (int)nc_id_group_L2, (char*)"range"                          , (const char*)"double", (int)1          , (int*)&id_dims_aer[2]    , (int*)&var_ids[12] ) ;

    if ( (glbParam->iLambda[glbParam->indxWL_PDL2] == 355) )
    {
        DefineVariable( (int)nc_id_group_L2, (char*)"Aerosol_Extinction_355nm"    , (const char*)"double", (int)num_dim_var, (int*)&id_dims_aer[0], (int*)&var_ids[0]  ) ;
        DefineVariable( (int)nc_id_group_L2, (char*)"Aerosol_Backscattering_355nm", (const char*)"double", (int)num_dim_var, (int*)&id_dims_aer[0], (int*)&var_ids[1]  ) ;
        if ( strcmp( oDL2->aeronet_file, "NOT_FOUND") !=0 )
        {
            DefineVariable( (int)nc_id_group_L2_AERONET      , (char*)"AERONET_Time"                     , (const char*)"double", (int)1, (int*)&id_dims_AERONET[0], (int*)&var_ids[13] ) ;
            DefineVariable( (int)nc_id_group_L2_AERONET      , (char*)"AERONET_AOD_340nm"                , (const char*)"double", (int)1, (int*)&id_dims_AERONET[0], (int*)&var_ids[14] ) ;
            DefineVariable( (int)nc_id_group_L2_AERONET      , (char*)"AERONET_AOD_355nm_at_LidarTime"   , (const char*)"double", (int)1, (int*)&id_dims_AERONET[1], (int*)&var_ids[16] ) ;
            DefineVariable( (int)nc_id_group_L2_AERONET      , (char*)"AERONET_Angstrom_Exponent_340-440", (const char*)"double", (int)1, (int*)&id_dims_AERONET[0], (int*)&var_ids[18] ) ;
            DefineVariable( (int)nc_id_group_L2_LIDAR_AERONET, (char*)"LR_inv_355nm"                     , (const char*)"double", (int)1, (int*)&id_dims_LIDAR_AERONET[0], (int*)&var_ids[15] ) ;
            DefineVariable( (int)nc_id_group_L2_LIDAR_AERONET, (char*)"AOD_Lidar_355nm_vs_AERONET"       , (const char*)"double", (int)1, (int*)&id_dims_LIDAR_AERONET[0], (int*)&var_ids[17] ) ;
            DefineVariable( (int)nc_id_group_L2_LIDAR_AERONET, (char*)"Aerosol_Extinction_355nm_synergy" , (const char*)"double", (int)2, (int*)&id_dims_LIDAR_AERONET[0], (int*)&var_ids[19] ) ;
            DefineVariable( (int)nc_id_group_L2_LIDAR_AERONET, (char*)"Aerosol_Backscattering_355nm_synergy", (const char*)"double", (int)2, (int*)&id_dims_LIDAR_AERONET[0], (int*)&var_ids[20] ) ;
            DefineVariable( (int)nc_id_group_L2_LIDAR_AERONET, (char*)"range"                            , (const char*)"double", (int)1, (int*)&id_dims_aer[2], (int*)&var_ids[21] ) ;
        }
    }
    else if ( (glbParam->iLambda[glbParam->indxWL_PDL2] == 351) )
    {
        DefineVariable( (int)nc_id_group_L2, (char*)"Aerosol_Extinction_351nm"    , (const char*)"double", (int)num_dim_var, (int*)&id_dims_aer[0], (int*)&var_ids[0]  ) ;
        DefineVariable( (int)nc_id_group_L2, (char*)"Aerosol_Backscattering_351nm", (const char*)"double", (int)num_dim_var, (int*)&id_dims_aer[0], (int*)&var_ids[1]  ) ;
        if ( strcmp( oDL2->aeronet_file, "NOT_FOUND") !=0 )
        {
            DefineVariable( (int)nc_id_group_L2_AERONET      , (char*)"AERONET_Time"                     , (const char*)"double", (int)1, (int*)&id_dims_AERONET[0], (int*)&var_ids[13] ) ;
            DefineVariable( (int)nc_id_group_L2_AERONET      , (char*)"AERONET_AOD_340nm"                , (const char*)"double", (int)1, (int*)&id_dims_AERONET[0], (int*)&var_ids[14] ) ;
            DefineVariable( (int)nc_id_group_L2_AERONET      , (char*)"AERONET_AOD_351nm_at_LidarTime"   , (const char*)"double", (int)1, (int*)&id_dims_AERONET[1], (int*)&var_ids[16] ) ;
            DefineVariable( (int)nc_id_group_L2_AERONET      , (char*)"AERONET_Angstrom_Exponent_340-440", (const char*)"double", (int)1, (int*)&id_dims_AERONET[0], (int*)&var_ids[18] ) ;
            DefineVariable( (int)nc_id_group_L2_LIDAR_AERONET, (char*)"LR_inv_351nm"                     , (const char*)"double", (int)1, (int*)&id_dims_LIDAR_AERONET[0], (int*)&var_ids[15] ) ;
            DefineVariable( (int)nc_id_group_L2_LIDAR_AERONET, (char*)"AOD_Lidar_351nm_vs_AERONET"       , (const char*)"double", (int)1, (int*)&id_dims_LIDAR_AERONET[0], (int*)&var_ids[17] ) ;
            DefineVariable( (int)nc_id_group_L2_LIDAR_AERONET, (char*)"Aerosol_Extinction_351nm_synergy" , (const char*)"double", (int)2, (int*)&id_dims_LIDAR_AERONET[0], (int*)&var_ids[19] ) ;
            DefineVariable( (int)nc_id_group_L2_LIDAR_AERONET, (char*)"Aerosol_Backscattering_351nm_synergy", (const char*)"double", (int)2, (int*)&id_dims_LIDAR_AERONET[0], (int*)&var_ids[20] ) ;
            DefineVariable( (int)nc_id_group_L2_LIDAR_AERONET, (char*)"range"                            , (const char*)"double", (int)1, (int*)&id_dims_aer[2], (int*)&var_ids[21] ) ;
        }
    }
    else if ( glbParam->iLambda[glbParam->indxWL_PDL2] == 532 )
    {
        DefineVariable( (int)nc_id_group_L2, (char*)"Aerosol_Extinction_532nm"    , (const char*)"double", (int)num_dim_var, (int*)&id_dims_aer[0], (int*)&var_ids[0]  ) ;
        DefineVariable( (int)nc_id_group_L2, (char*)"Aerosol_Backscattering_532nm", (const char*)"double", (int)num_dim_var, (int*)&id_dims_aer[0], (int*)&var_ids[1]  ) ;
        if ( strcmp( oDL2->aeronet_file, "NOT_FOUND") !=0 )
        {
            DefineVariable( (int)nc_id_group_L2_AERONET      , (char*)"AERONET_Time"                     , (const char*)"double", (int)1, (int*)&id_dims_AERONET[0], (int*)&var_ids[13] ) ;
            DefineVariable( (int)nc_id_group_L2_AERONET      , (char*)"AERONET_AOD_500nm"                , (const char*)"double", (int)1, (int*)&id_dims_AERONET[0], (int*)&var_ids[14] ) ;
            DefineVariable( (int)nc_id_group_L2_AERONET      , (char*)"AERONET_AOD_532nm_at_LidarTime"   , (const char*)"double", (int)1, (int*)&id_dims_AERONET[1], (int*)&var_ids[16] ) ;
            DefineVariable( (int)nc_id_group_L2_AERONET      , (char*)"AERONET_Angstrom_Exponent_440-675", (const char*)"double", (int)1, (int*)&id_dims_AERONET[0], (int*)&var_ids[18] ) ;
            DefineVariable( (int)nc_id_group_L2_LIDAR_AERONET, (char*)"LR_inv_532nm"                     , (const char*)"double", (int)1, (int*)&id_dims_LIDAR_AERONET[0], (int*)&var_ids[15] ) ;
            DefineVariable( (int)nc_id_group_L2_LIDAR_AERONET, (char*)"AOD_Lidar_532nm_vs_AERONET"       , (const char*)"double", (int)1, (int*)&id_dims_LIDAR_AERONET[0], (int*)&var_ids[17] ) ;
            DefineVariable( (int)nc_id_group_L2_LIDAR_AERONET, (char*)"Aerosol_Extinction_532nm_synergy" , (const char*)"double", (int)2, (int*)&id_dims_LIDAR_AERONET[0], (int*)&var_ids[19] ) ;
            DefineVariable( (int)nc_id_group_L2_LIDAR_AERONET, (char*)"Aerosol_Backscattering_532nm_synergy", (const char*)"double", (int)2, (int*)&id_dims_LIDAR_AERONET[0], (int*)&var_ids[20] ) ;
            DefineVariable( (int)nc_id_group_L2_LIDAR_AERONET, (char*)"range"                            , (const char*)"double", (int)1, (int*)&id_dims_aer[2], (int*)&var_ids[21] ) ;
        }
    }
    else if ( glbParam->iLambda[glbParam->indxWL_PDL2] == 1064 )
    {
        DefineVariable( (int)nc_id_group_L2, (char*)"Aerosol_Extinction_1064nm"    , (const char*)"double", (int)num_dim_var, (int*)&id_dims_aer[0], (int*)&var_ids[0]  ) ;
        DefineVariable( (int)nc_id_group_L2, (char*)"Aerosol_Backscattering_1064nm", (const char*)"double", (int)num_dim_var, (int*)&id_dims_aer[0], (int*)&var_ids[1]  ) ;
        if ( strcmp( oDL2->aeronet_file, "NOT_FOUND") !=0 )
        {
            DefineVariable( (int)nc_id_group_L2_AERONET      , (char*)"AERONET_Time"                     , (const char*)"double", (int)1, (int*)&id_dims_AERONET[0], (int*)&var_ids[13] ) ;
            DefineVariable( (int)nc_id_group_L2_AERONET      , (char*)"AERONET_AOD_1020nm"               , (const char*)"double", (int)1, (int*)&id_dims_AERONET[0], (int*)&var_ids[14] ) ;
            DefineVariable( (int)nc_id_group_L2_AERONET      , (char*)"AERONET_AOD_1064nm_at_LidarTime"  , (const char*)"double", (int)1, (int*)&id_dims_AERONET[1], (int*)&var_ids[16] ) ;
            DefineVariable( (int)nc_id_group_L2_AERONET      , (char*)"AERONET_Angstrom_Exponent_500-870", (const char*)"double", (int)1, (int*)&id_dims_AERONET[0], (int*)&var_ids[18] ) ;
            DefineVariable( (int)nc_id_group_L2_LIDAR_AERONET, (char*)"LR_inv_1064nm"                    , (const char*)"double", (int)1, (int*)&id_dims_LIDAR_AERONET[0], (int*)&var_ids[15] ) ;
            DefineVariable( (int)nc_id_group_L2_LIDAR_AERONET, (char*)"AOD_Lidar_1064nm_vs_AERONET"      , (const char*)"double", (int)1, (int*)&id_dims_LIDAR_AERONET[0], (int*)&var_ids[17] ) ;
            DefineVariable( (int)nc_id_group_L2_LIDAR_AERONET, (char*)"Aerosol_Extinction_1064nm_synergy" , (const char*)"double", (int)2, (int*)&id_dims_LIDAR_AERONET[0], (int*)&var_ids[19] ) ;
            DefineVariable( (int)nc_id_group_L2_LIDAR_AERONET, (char*)"Aerosol_Backscattering_1064nm_synergy", (const char*)"double", (int)2, (int*)&id_dims_LIDAR_AERONET[0], (int*)&var_ids[20] ) ;
            DefineVariable( (int)nc_id_group_L2_LIDAR_AERONET, (char*)"range"                            , (const char*)"double", (int)1, (int*)&id_dims_aer[2], (int*)&var_ids[21] ) ;
        }
    }

    double *Ref_Range = (double*) new double[ glbParam->nEventsAVG ]  ;
    for (int i =0; i <glbParam->nEventsAVG; i++)
        Ref_Range[i] = oDL2->indxRef_Fernald[i] *glbParam->dr ; // + glbParam->siteASL ;

    int     indxWL_PDL2 ;
    double  R_ref       ;
    ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, (const char*)"indxWL_PDL2", (const char*)"int"    , (int*)&indxWL_PDL2 ) ;
    ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, (const char*)"R_ref"      , (const char*)"double" , (double*)&R_ref    ) ;

    if ( ( retval = nc_put_att_int( (int)nc_id_group_L2, (int)NC_GLOBAL, (const char*)"indxChannel_for_Fernald_inv", NC_INT, 1, (const int*)&indxWL_PDL2 ) ) )
        ERR(retval);
    if ( ( retval = nc_put_att_int( (int)nc_id_group_L2, (int)NC_GLOBAL, (const char*)"Wavelength_Inverted", NC_INT, 1, (const int*)&glbParam->iLambda[indxWL_PDL2] ) ) )
        ERR(retval);
    if ( ( retval = nc_put_att_int( (int)nc_id_group_L2, (int)NC_GLOBAL, (const char*)"Averaged_Profiles_L2", NC_INT, 1, (const int*)&glbParam->numEventsToAvg ) ) )
        ERR(retval);
    if ( ( retval = nc_put_att_double( (int)nc_id_group_L2, (int)NC_GLOBAL, (const char*)"R_Ref", NC_DOUBLE, 1, (const double*)&R_ref ) ) )
        ERR(retval);

    Set_LALINET_Units_L2( (int)nc_id_group_L2, (int*)var_ids ) ;

                if ( (retval = nc_enddef(nc_id_group_L2)) )
                    ERR(retval);

    // WRITE CLOUD RAW LIDAR DATA CORRECTED
    size_t start[3], count[3];
    size_t start_ES[2], count_ES[2];
    size_t start_LA[2], count_LA[2];
    start[0] = 0;   count[0] = 1 ; // glbParam.nEventsAVG; 
    start[1] = 0;   count[1] = 1 ; // glbParam.nCh; 
    start[2] = 0;   count[2] = glbParam->nBins ;

    start_ES[0] = 0;   count_ES[0] = 1 ;  // TIME
    start_ES[1] = 0;   count_ES[1] = glbParam->nCh ; // CHANNELS

    // LA: LIDAR - AERONET
    start_LA[0] = 0;   count_LA[0] = 1 ;  // TIME
    start_LA[1] = 0;   count_LA[1] = glbParam->nBins ; // BINS

    for( int e=0 ; e <glbParam->nEventsAVG ; e++ )
    {
        start[0] =e ;
        for ( int l=0 ; l <oDL2->nLRs ; l++ )
        {
            start[1] =l ;
            if ( (retval = nc_put_vara_double((int)nc_id_group_L2, (int)var_ids[0], start, count, (double*)&oDL2->alpha_Aer[e][l][0] ) ) )
                ERR(retval);

            if ( (retval = nc_put_vara_double( (int)nc_id_group_L2, (int)var_ids[1], start, count, (double*)&oDL2->beta_Aer[e][l][0] ) ) )
                ERR(retval);

            if ( (retval = nc_put_vara_double( (int)nc_id_group_L2, (int)var_ids[9], start, count, (double*)&oDL2->AOD_LR[e][l] ) ) )
                ERR(retval);
        }
        for ( int c=0 ; c <glbParam->nCh ; c++ )
        {
            start[1] =c ;
            if ( (retval = nc_put_vara_double((int)nc_id_group_L2, (int)var_ids[2], start, count, (double*)&oDL2->pr2[e][c][0])))
                ERR(retval);
        }

        start_ES[0] =e ;
        if ( (retval = nc_put_vara_double( (int)nc_id_group_L2, (int)var_ids[7], start_ES, count_ES, (double*)&glbParam->rEndSig_ev_ch[e][0] ) ) )
            ERR(retval) ;

        if ( strcmp( oDL2->aeronet_file, "NOT_FOUND") !=0 )
        {
            start_LA[0] =e ;
            if ( ( retval = nc_put_vara_double( (int)nc_id_group_L2_LIDAR_AERONET, var_ids[19], start_LA, count_LA, (double*)&oDL2->alpha_Aer_synergy[e][0] ) ) )
                ERR(retval) ;

            if ( (retval = nc_put_vara_double((int)nc_id_group_L2_LIDAR_AERONET, var_ids[20], start_LA, count_LA, (double*)&oDL2->beta_Aer_synergy[e][0] ) ) )
                ERR(retval) ;
        }
    }

    PutVar( (int)nc_id_group_L2, (int)var_ids[5] , (const char*)"double", (double*)glbParam->aZenithAVG  ) ;
    PutVar( (int)nc_id_group_L2, (int)var_ids[6] , (const char*)"double", (double*)glbParam->aAzimuthAVG ) ;

    PutVar( (int)nc_id_group_L2, (int)var_ids[8], (const char*)"double", (double*)oDL2->LR ) ;

    PutVar( (int)nc_id_group_L2, (int)var_ids[10], (const char*)"double", (double*)Ref_Range  ) ;

    PutVar( (int)nc_id_group_L2, (int)var_ids[3], (const char*)"int", (int*)oDL2->Start_Time_AVG_L2 ) ;
    PutVar( (int)nc_id_group_L2, (int)var_ids[4], (const char*)"int", (int*)oDL2->Stop_Time_AVG_L2 ) ;

    PutVar( (int)nc_id_group_L2, (int)var_ids[11], (const char*)"int", (int*)glbParam->avg_Points_Fernald ) ;

    PutVar( (int)nc_id_group_L2, (int)var_ids[12], (const char*)"double", (double*)&glbParam->r[0] ) ;

    if ( strcmp( oDL2->aeronet_file, "NOT_FOUND") !=0 )
    {
        PutVar( (int)nc_id_group_L2_LIDAR_AERONET, (int)var_ids[21], (const char*)"double", (double*)&glbParam->r[0]      ) ;
        PutVar( (int)nc_id_group_L2_AERONET      , (int)var_ids[13], (const char*)"int"   , (int*)   oDL2->AERONET_time   ) ;
        PutVar( (int)nc_id_group_L2_AERONET      , (int)var_ids[14], (const char*)"double", (double*)oDL2->AERONET_AOD    ) ;
        PutVar( (int)nc_id_group_L2_LIDAR_AERONET, (int)var_ids[15], (const char*)"double", (double*)oDL2->LR_inv         ) ;
        PutVar( (int)nc_id_group_L2_AERONET      , (int)var_ids[16], (const char*)"double", (double*)oDL2->AOD_Lidar_Time ) ;
        PutVar( (int)nc_id_group_L2_LIDAR_AERONET, (int)var_ids[17], (const char*)"double", (double*)oDL2->AOD_inv        ) ;
        PutVar( (int)nc_id_group_L2_AERONET      , (int)var_ids[18], (const char*)"double", (double*)oDL2->AERONET_Angs   ) ;
    }

            if ( (retval = nc_close(nc_id) ) )
                ERR(retval);
}
