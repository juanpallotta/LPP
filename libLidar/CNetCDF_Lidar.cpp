
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

void CNetCDF_Lidar::PutVar_String( int nc_file_id, int var_id, int numElements, string *strToSave )
{
    char dataToSave[numElements][ strToSave[0].length() ] ;
    for( int i=0 ; i <numElements ; i++ )
        sprintf( dataToSave[i], "%s", strToSave[i].c_str() ) ;

    if ( (retval = nc_put_var_string ( (int)nc_file_id, (int)var_id, (const char**)dataToSave ) ) )
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

    if ( var_type == NC_INT  )
    {
        // cout << endl << strNameVars << "  --> DATA TYPE: INT" << endl;
        if ( ( retval = nc_get_var_int( (int)ncid, (int)var_id, (int*)dataRead ) ) )
            ERR(retval);
    }
    else if ( var_type == NC_DOUBLE )
    {
        // cout << endl << strNameVars << "  --> DATA TYPE: DOUBLE" << endl;
        if ( ( retval = nc_get_var_double( (int)ncid, (int)var_id, (double*)dataRead ) ) )
            ERR(retval);
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
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[0], (const char*)"units", (size_t)strlen("Averaged ADC Counts"), (const char*)"Averaged ADC Counts") ) )
       ERR(retval);
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[1], (const char*)"units", (size_t)strlen("seconds"), (const char*)"seconds") ) )
       ERR(retval);
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[2], (const char*)"units", (size_t)strlen("seconds"), (const char*)"seconds") ) )
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
}

void CNetCDF_Lidar::Set_LALINET_Units_L1( int ncid, int *var_ids )
{ //! CHECK INPLEMENTATION 
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[0], (const char*)"units", (size_t)strlen("Averaged ADC Counts"), (const char*)"Averaged ADC Counts") ) )
       ERR(retval);
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[1], (const char*)"units", (size_t)strlen("seconds"), (const char*)"seconds") ) )
       ERR(retval);
    if ( ( retval = nc_put_att_text ( (int)ncid, (int)var_ids[2], (const char*)"units", (size_t)strlen("seconds"), (const char*)"seconds") ) )
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
}

// ! SEE FOR DETAILS: --> https://docs.scc.imaa.cnr.it/en/latest/file_formats/netcdf_file.html
void CNetCDF_Lidar::Save_SCC_NCDF_Format( string Path_File_Out, strcGlobalParameters *glbParam, double ***dataToSave, int *Raw_Data_Start_Time, string *Raw_Data_Start_Time_str, int *Raw_Data_Stop_Time, string *Raw_Data_Stop_Time_str )
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
    strNameVars[10] = "Pressure_at_Lidar_Station" ;
    strNameVars[11] = "Temperature_at_Lidar_Station" ;
    strNameVars[12] = "Raw_Data_Start_Time" ;
    strNameVars[13] = "Raw_Data_Stop_Time" ;
    strNameVars[14] = "Laser_Pointing_Angle_of_Profiles" ;

    int     id_timescale[glbParam->nCh], channel_ID[glbParam->nCh], Laser_Shots[glbParam->nEventsAVG][glbParam->nCh], LR_Input[glbParam->nCh], Molecular_Calc, Laser_Pointing_Angle_of_Profiles[glbParam->nCh];
    double  Background_Low[glbParam->nCh], Background_High[glbParam->nCh], Pressure_at_Lidar_Station, Temperature_at_Lidar_Station ;
    ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"nBinsBkg", (const char*)"int"   , (int*)&glbParam->nBinsBkg ) ;
    // ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"rEndSig" , (const char*)"double", (int*)&glbParam->rEndSig  ) ;
    for( int c=0 ; c <glbParam->nCh ; c++ )
    {
        id_timescale[c]     = (int)0  ;
        LR_Input[c]         = (int)40 ;
        channel_ID[c]       = (int)42 ;
        Background_Low[c]   = (double) (glbParam->rEndSig) ;
        Background_High[c]  = (double) (glbParam->rEndSig - glbParam->nBinsBkg * glbParam->dr) ;
        Laser_Pointing_Angle_of_Profiles[c] = (int)0 ;
        for (int t =0; t <glbParam->nEventsAVG; t++)
            Laser_Shots[t][c] = (int)glbParam->nShots[c] ;
    }
    ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"Molecular_Calc"              , (const char*)"int"   , (int*)   &Molecular_Calc ) ;
    ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"Pressure_at_Lidar_Station"   , (const char*)"double", (double*)&Pressure_at_Lidar_Station    ) ;
    ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"Temperature_at_Lidar_Station", (const char*)"double", (double*)&Temperature_at_Lidar_Station ) ;

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
    strAttListName[0] = "System"                ;   strcpy ( (char*)strAttList[0].c_str(), glbParam->site )   ;
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
    PutVar( (int)ncid, (int)var_ids[10], (const char*)"double", (double*)&Pressure_at_Lidar_Station    ) ;
    PutVar( (int)ncid, (int)var_ids[11], (const char*)"double", (double*)&Temperature_at_Lidar_Station ) ;
    PutVar( (int)ncid, (int)var_ids[12], (const char*)"int"   , (int*)Raw_Data_Start_Time              ) ;
    PutVar( (int)ncid, (int)var_ids[13], (const char*)"int"   , (int*)Raw_Data_Stop_Time               ) ;
    PutVar( (int)ncid, (int)var_ids[14], (const char*)"int"   , (int*)Laser_Pointing_Angle_of_Profiles ) ;

    if ( (retval = nc_close(ncid)) )
        ERR(retval);
}

void CNetCDF_Lidar::Save_LALINET_NCDF_PDL0( string Path_File_Out, strcGlobalParameters *glbParam, double ***dataToSave, int *Raw_Data_Start_Time_sec, int *Raw_Data_Stop_Time_sec )
{
    int  ncid ;
    int  retval ;

    if ( (retval = nc_create( Path_File_Out.c_str(), NC_NETCDF4, &ncid)) )
        ERR(retval);

    // DIMENSIONS INFO FOR pr VARIABLE
    string      dimsName [NDIMS_LALINET] ;
    int         dimsSize[NDIMS_LALINET] ;
    dimsName[0] = "time" ;                  dimsSize[0] = glbParam->nEventsAVG   ;
    dimsName[1] = "channels" ;              dimsSize[1] = glbParam->nCh          ;
    dimsName[2] = "points" ;                dimsSize[2] = glbParam->nBinsRaw     ;

    int  dim_ids[NDIMS_LALINET];
    // int  dim_ids_StartEnd_time[2];

    int  var_ids[NVARS_LALINET] ;
    string  strNameVars[NVARS_LALINET] ;
    strNameVars[0]  = "Raw_Lidar_Data" ;
    strNameVars[1]  = "Raw_Data_Start_Time" ;
    strNameVars[2]  = "Raw_Data_Stop_Time" ;
    strNameVars[3]  = "Wavelenghts" ;
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
    strNameVars[15] = "Number_Of_Bins" ;

    // DEFINE Raw_Lidar_Data VARIABLE
    DefineVarDims( (int)ncid, (int)3, (string*)dimsName, (int*)dimsSize, (int*)dim_ids, (char*)strNameVars[0].c_str(), (const char*)"double", (int*)&var_ids[0] ) ; // Raw_Lidar_Data

    DefineVariable( (int)ncid, (char*)strNameVars[1].c_str() , (const char*)"int"   , (int)1, (int*)&dim_ids[0], (int*)&var_ids[1]  ) ; // Raw_Data_Start_Time
    DefineVariable( (int)ncid, (char*)strNameVars[2].c_str() , (const char*)"int"   , (int)1, (int*)&dim_ids[0], (int*)&var_ids[2]  ) ; // Raw_Data_Stop_Time
    DefineVariable( (int)ncid, (char*)strNameVars[3].c_str() , (const char*)"int"   , (int)1, (int*)&dim_ids[1], (int*)&var_ids[3]  ) ; // Wavelenghts
    DefineVariable( (int)ncid, (char*)strNameVars[4].c_str() , (const char*)"char"  , (int)1, (int*)&dim_ids[1], (int*)&var_ids[4]  ) ; // Polarization
    DefineVariable( (int)ncid, (char*)strNameVars[5].c_str() , (const char*)"double", (int)1, (int*)&dim_ids[1], (int*)&var_ids[5]  ) ; // DAQ_Range
    DefineVariable( (int)ncid, (char*)strNameVars[6].c_str() , (const char*)"int"   , (int)1, (int*)&dim_ids[1], (int*)&var_ids[6]  ) ; // DAQ_type
    DefineVariable( (int)ncid, (char*)strNameVars[7].c_str() , (const char*)"int"   , (int)1, (int*)&dim_ids[1], (int*)&var_ids[7]  ) ; // PMT_Voltage
    DefineVariable( (int)ncid, (char*)strNameVars[8].c_str() , (const char*)"int"   , (int)1, (int*)&dim_ids[1], (int*)&var_ids[8]  ) ; // Accumulated_Pulses
    DefineVariable( (int)ncid, (char*)strNameVars[9].c_str() , (const char*)"double", (int)1, (int*)&dim_ids[0], (int*)&var_ids[9]  ) ; // Zenith
    DefineVariable( (int)ncid, (char*)strNameVars[10].c_str(), (const char*)"double", (int)1, (int*)&dim_ids[0], (int*)&var_ids[10] ) ; // Azimuth
    DefineVariable( (int)ncid, (char*)strNameVars[13].c_str(), (const char*)"int"   , (int)1, (int*)&dim_ids[1], (int*)&var_ids[13] ) ; // ADC_Bits
    DefineVariable( (int)ncid, (char*)strNameVars[14].c_str(), (const char*)"int"   , (int)1, (int*)&dim_ids[1], (int*)&var_ids[14] ) ; // Laser_Source
    DefineVariable( (int)ncid, (char*)strNameVars[15].c_str(), (const char*)"int"   , (int)1, (int*)&dim_ids[1], (int*)&var_ids[15] ) ; // Number_Of_Bins

    // TEXT GLOBAL ATTRIBUTES
    string strAttListName[10], strAttList ;
    strAttListName[0] = "Site_Name"             ;   strcpy ( (char*)strAttList.c_str(), glbParam->site )  ;
        Putt_Bulk_Att_Text( (int)ncid, (int)NC_GLOBAL, (int)1, (string*)strAttListName, (string*)&strAttList ) ;

    // DOUBLE GLOBAL ATTRIBUTES
    double dblAttList[5] ;
    strAttListName[0] = "Altitude_meter_asl"     ;   dblAttList[0] = (double)glbParam->siteASL  ;
    strAttListName[1] = "Latitude_degrees_north" ;   dblAttList[1] = (double)glbParam->siteLat  ;
    strAttListName[2] = "Longitude_degrees_east" ;   dblAttList[2] = (double)glbParam->siteLong ;
    strAttListName[3] = "Range_Resolution"       ;   dblAttList[3] = (double)glbParam->dr       ;
        Putt_Bulk_Att_Double( (int)ncid, (int)NC_GLOBAL, (int)4, (string*)strAttListName, (double*)dblAttList ) ;

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

    PutVar( (int)ncid, (int)var_ids[1] , (const char*)"int"   , (int*)Raw_Data_Start_Time_sec       ) ; 
    PutVar( (int)ncid, (int)var_ids[2] , (const char*)"int"   , (int*)Raw_Data_Stop_Time_sec        ) ;
    PutVar( (int)ncid, (int)var_ids[3] , (const char*)"int"   , (int*)glbParam->iLambda             ) ;
    PutVar( (int)ncid, (int)var_ids[4] , (const char*)"char"  , (char*)glbParam->sPol               ) ;
    PutVar( (int)ncid, (int)var_ids[5] , (const char*)"double", (double*)glbParam->iMax_mVLic       ) ;
    PutVar( (int)ncid, (int)var_ids[6] , (const char*)"int"   , (int*)glbParam->iAnPhot             ) ;
    PutVar( (int)ncid, (int)var_ids[7] , (const char*)"int"   , (int*)glbParam->PMT_Voltage         ) ;
    PutVar( (int)ncid, (int)var_ids[8] , (const char*)"int"   , (int*)glbParam->nShots              ) ;
    PutVar( (int)ncid, (int)var_ids[9] , (const char*)"double", (double*)glbParam->aZenith          ) ; // PutVar( (int)ncid, (int)var_ids[9] , (const char*)"double", (double*)glbParam->aZenithAVG       ) ;
    PutVar( (int)ncid, (int)var_ids[10], (const char*)"double", (double*)glbParam->aAzimuth         ) ; // PutVar( (int)ncid, (int)var_ids[10], (const char*)"double", (double*)glbParam->aAzimuthAVG      ) ;
    PutVar( (int)ncid, (int)var_ids[13], (const char*)"int"   , (int*)glbParam->iADCbits            ) ;
    PutVar( (int)ncid, (int)var_ids[14], (const char*)"int"   , (int*)glbParam->Laser_Src           ) ;
    PutVar( (int)ncid, (int)var_ids[15], (const char*)"int"   , (int*)glbParam->nBinsRaw_Ch         ) ;

    if ( (retval = nc_close(ncid)) )
        ERR(retval);
}

void CNetCDF_Lidar::Save_LALINET_NCDF_PDL1( string *Path_File_In, string *Path_File_Out, strcGlobalParameters *glbParam, int **clouds_ON_mtx, double ***pr_corr, double ***pr2, CMolecularData_USStd *oMolData )
{
    int retval, nc_id ;
    if ( ( retval = nc_open( Path_File_Out->c_str(), NC_WRITE, &nc_id ) ) )
        ERR(retval);
    if ( ( retval = nc_redef( (int)nc_id ) ) )
        ERR(retval);

    int var_id_pr_corr, var_id_pr2, var_id_laser_zero_bin_offset, var_id_mol_backscattering, var_id_mol_extinction, var_id_Temp_Pres[2], var_id_Zen_Azm[2] ;
    int dims_ids_pr_corr[3] ; // 0: TIME    1: CHANNELS     2:POINTS

    int nc_id_group_L1 ;
    if ( (retval = nc_def_grp ( (int)nc_id, (const char*)"L1_Data", (int*)&nc_id_group_L1 ) ) )
        ERR(retval) ;

    string      dimsName [NDIMS_LALINET] ;
    int         dimsSize[NDIMS_LALINET] ;
    dimsName[0] = "time" ;                  dimsSize[0] = glbParam->nEventsAVG   ;
    dimsName[1] = "channels" ;              dimsSize[1] = glbParam->nCh          ;
    dimsName[2] = "points" ;                dimsSize[2] = glbParam->nBins        ;

    // DEFINE Raw_Lidar_Data VARIABLE
    DefineVarDims( (int)nc_id_group_L1, (int)3, (string*)dimsName, (int*)dimsSize, (int*)dims_ids_pr_corr, (char*)"Raw_Lidar_Data_L1", (const char*)"double", (int*)&var_id_pr_corr ) ; // Raw_Lidar_Data

    DefineVariable( (int)nc_id_group_L1, (char*)"Range_Corrected_Lidar_Signal_L1", (const char*)"double", (int)3, (int*)&dims_ids_pr_corr[0], (int*)&var_id_pr2 ) ;
    DefineVariable( (int)nc_id_group_L1, (char*)"Laser_Zero_Bin_Offset"          , (const char*)"int"   , (int)1, (int*)&dims_ids_pr_corr[1], (int*)&var_id_laser_zero_bin_offset ) ;
    DefineVariable( (int)nc_id_group_L1, (char*)"Molecular_Extinction"           , (const char*)"double", (int)2, (int*)&dims_ids_pr_corr[1], (int*)&var_id_mol_extinction ) ;
    DefineVariable( (int)nc_id_group_L1, (char*)"Molecular_Backscattering"       , (const char*)"double", (int)2, (int*)&dims_ids_pr_corr[1], (int*)&var_id_mol_backscattering ) ;
    DefineVariable( (int)nc_id_group_L1, (char*)"Temperature_ground_level"       , (const char*)"double", (int)1, (int*)&dims_ids_pr_corr[0], (int*)&var_id_Temp_Pres[0] ) ;
    DefineVariable( (int)nc_id_group_L1, (char*)"Pressure_ground_level"          , (const char*)"double", (int)1, (int*)&dims_ids_pr_corr[0], (int*)&var_id_Temp_Pres[1] ) ;

    DefineVariable( (int)nc_id_group_L1, (char*)"Zenith_AVG"                     , (const char*)"double", (int)1, (int*)&dims_ids_pr_corr[0], (int*)&var_id_Zen_Azm[0] ) ;
    DefineVariable( (int)nc_id_group_L1, (char*)"Azimuth_AVG"                    , (const char*)"double", (int)1, (int*)&dims_ids_pr_corr[0], (int*)&var_id_Zen_Azm[1] ) ;

    int indxWL_PDL1 ;
    ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"indxWL_PDL1", (const char*)"int", (int*)&indxWL_PDL1 ) ;
    indxWL_PDL1++ ;
    if ( ( retval = nc_put_att_int( (int)nc_id_group_L1, (int)NC_GLOBAL, (const char*)"Channel_Number_Cloud_Mask", NC_INT, 1, (const int*)&indxWL_PDL1 ) ) )
        ERR(retval);

// CLOUD MASK VARIABLE DEFINITION
    int         dims_ids_CM[2] ;
    dims_ids_CM[0] = dims_ids_pr_corr[0] ; // TIME
    dims_ids_CM[1] = dims_ids_pr_corr[2] ; // POINTS
    int         var_id_CM ;
    DefineVariable( (int)nc_id_group_L1, (char*)"Cloud_Mask", (const char*)"int", (int)2, (int*)&dims_ids_CM[0], (int*)&var_id_CM  ) ;

                if ( (retval = nc_enddef(nc_id_group_L1)) )
                    ERR(retval);

    // WRITE LASER ZERO BIN OFFSET
    if ( (retval = nc_put_var_int( (int)nc_id_group_L1, (int)var_id_laser_zero_bin_offset, (int*)&glbParam->indxOffset ) ) )
        ERR(retval);

    // WRITE MOLECULAR INFORMATION
    size_t start_mol[2], count_mol[2];
    start_mol[0] = 0;   count_mol[0] = 1 ;  // CHANNEL
    start_mol[1] = 0;   count_mol[1] = glbParam->nBins ; // BINS
    for( int c=0 ; c <glbParam->nCh  ; c++ )
    {
        start_mol[0] =c ;
        if ( (retval = nc_put_vara_double( (int)nc_id_group_L1, (int)var_id_mol_extinction, start_mol, count_mol, (double*)&oMolData->dataMol[c].alphaMol[0]  ) ) )
            ERR(retval) ;

        if ( (retval = nc_put_vara_double( (int)nc_id_group_L1, (int)var_id_mol_backscattering, start_mol, count_mol, (double*)&oMolData->dataMol[c].betaMol[0] ) ) )
            ERR(retval) ;
    }

    // WRITE CLOUD MASK VARIABLE
    size_t start_CM[2], count_CM[2];
    start_CM[0] = 0;   count_CM[0] = 1 ;  // TIME
    start_CM[1] = 0;   count_CM[1] = glbParam->nBins ; // POINTS
    for( int e=0 ; e <glbParam->nEventsAVG  ; e++ )
    {
        start_CM[0] =e ;
        if ( (retval = nc_put_vara_int( (int)nc_id_group_L1, (int)var_id_CM, start_CM, count_CM, (int*)&clouds_ON_mtx[e][0] ) ) )
            ERR(retval);

        if ( (retval = nc_put_vara_double( (int)nc_id_group_L1, (int)var_id_Temp_Pres[0], start_CM, count_CM, (double*)&glbParam->temp_CelsiusAVG[e] ) ) )
            ERR(retval);
        if ( (retval = nc_put_vara_double( (int)nc_id_group_L1, (int)var_id_Temp_Pres[1], start_CM, count_CM, (double*)&glbParam->pres_hPaAVG[e]     ) ) )
            ERR(retval);
        if ( (retval = nc_put_vara_double( (int)nc_id_group_L1, (int)var_id_Zen_Azm[0], start_CM, count_CM, (double*)&glbParam->aZenithAVG[e] ) ) )
            ERR(retval);
        if ( (retval = nc_put_vara_double( (int)nc_id_group_L1, (int)var_id_Zen_Azm[1], start_CM, count_CM, (double*)&glbParam->aAzimuth[e]   ) ) )
            ERR(retval);
    }
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
            if ( (retval = nc_put_vara_double((int)nc_id_group_L1, (int)var_id_pr_corr, start_pr, count_pr, (double*)&pr_corr[e][c][0] ) ) )
                ERR(retval);

            if ( (retval = nc_put_vara_double( (int)nc_id_group_L1, (int)var_id_pr2, start_pr, count_pr, (double*)&pr2[e][c][0] ) ) )
                ERR(retval);
        }
    }
    if ( (retval = nc_close(nc_id) ) )
        ERR(retval);
}

void CNetCDF_Lidar::Save_LALINET_NCDF_PDL2( string *Path_File_Out, strcGlobalParameters *glbParam, CDataLevel_2 *oDL2 )
{
    int retval, nc_id ;
    if ( ( retval = nc_open( Path_File_Out->c_str(), NC_WRITE, &nc_id ) ) )
        ERR(retval);
    if ( ( retval = nc_redef( (int)nc_id ) ) )
        ERR(retval);

    int ncid_L1_Data ;
    if ( ( retval = nc_inq_grp_ncid( (int)nc_id, (const char*)"L1_Data", (int*)&ncid_L1_Data ) ) )
        ERR(retval);

    string  strNameVars[NVARS_LALINET] ;
    strNameVars[0] = "Range_Corrected_Lidar_Signal_L2" ;
    strNameVars[1] = "Molecular_Extinction" ;
    strNameVars[2] = "Molecular_Backscattering" ;
    int id_var_RCLS, id_var_beta_mol, id_var_alpha_mol  ;
    if ( ( retval = nc_inq_varid( (int)ncid_L1_Data, (const char*)strNameVars[0].c_str(), (int*)&id_var_RCLS ) ) )
        ERR(retval);

    int num_dim_var ;
    if ( ( retval = nc_inq_varndims( (int)ncid_L1_Data, (int)id_var_RCLS, (int*)&num_dim_var ) ) )
        ERR(retval);

    int id_dim_RCLS[num_dim_var] ;
    if ( ( retval = nc_inq_vardimid( (int)ncid_L1_Data, (int)id_var_RCLS, (int*)id_dim_RCLS ) ) )
        ERR(retval) ;

    int nc_id_group_L2 ;
    if ( (retval = nc_def_grp ( (int)nc_id, (const char*)"L2_Data", (int*)&nc_id_group_L2 ) ) )
        ERR(retval) ;

    DefineVariable( (int)nc_id_group_L2, (char*)"Aerosol_Extinction"    , (const char*)"double", (int)3, (int*)&id_dim_RCLS[0], (int*)&id_var_alpha_mol ) ;
    DefineVariable( (int)nc_id_group_L2, (char*)"Aerosol_Backscattering", (const char*)"double", (int)3, (int*)&id_dim_RCLS[0], (int*)&id_var_beta_mol  ) ;

                if ( (retval = nc_enddef(nc_id_group_L2)) )
                    ERR(retval);

    // WRITE CLOUD RAW LIDAR DATA CORRECTED
    size_t start[3], count[3];
    start[0] = 0;   count[0] = 1 ; // glbParam.nEventsAVG; 
    start[1] = 0;   count[1] = 1 ; // glbParam.nCh; 
    start[2] = 0;   count[2] = glbParam->nBins ;
    for( int e=0 ; e <glbParam->nEventsAVG ; e++ )
    {
        start[0] =e ;
        for ( int c=0 ; c <glbParam->nCh ; c++ )
        {
            start[1] =c ;
            if ( (retval = nc_put_vara_double((int)nc_id_group_L2, (int)id_var_alpha_mol, start, count, (double*)&oDL2->alpha_Aer[e][c][0] ) ) )
                ERR(retval);

            if ( (retval = nc_put_vara_double( (int)nc_id_group_L2, (int)id_var_beta_mol, start, count, (double*)&oDL2->beta_Aer[e][c][0] ) ) )
                ERR(retval);
        }
    }

    if ( (retval = nc_close(nc_id) ) )
        ERR(retval);
}
