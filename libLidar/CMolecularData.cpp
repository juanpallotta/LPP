/*
 * CMolecularData.cpp
 * Copyright 2021 Juan V. Pallotta
 */

#include "CMolecularData.hpp"
#include "lidarMathFunc.hpp"
#include "CNetCDF_Lidar.hpp"

CMolecularData::CMolecularData( strcGlobalParameters *glbParam )
{
	GetMem_dataMol( (strcGlobalParameters*)glbParam ) ; // MEMORY ALLOCATION FOR dataMol STRUCTURE.
}

CMolecularData::~CMolecularData()
{

}

void CMolecularData::GetMem_dataMol( strcGlobalParameters *glbParam )
{
		dataMol.nBins	  	 = (int)glbParam->nBins ;
		dataMol.zr		  	 = (double*) new double [glbParam->nBins] ; memset( dataMol.zr	     	, 0, ( sizeof(double)*glbParam->nBins) ) ;
		dataMol.z_asl	  	 = (double*) new double [glbParam->nBins] ; memset( dataMol.z_asl	 	, 0, ( sizeof(double)*glbParam->nBins) ) ;
		dataMol.betaMol   	 = (double*) new double [glbParam->nBins] ; memset( dataMol.betaMol  	, 0, ( sizeof(double)*glbParam->nBins) ) ;
		dataMol.alphaMol  	 = (double*) new double [glbParam->nBins] ; memset( dataMol.alphaMol 	, 0, ( sizeof(double)*glbParam->nBins) ) ;
		dataMol.betaRam  	 = (double*) new double [glbParam->nBins] ; memset( dataMol.betaRam  	, 0, ( sizeof(double)*glbParam->nBins) ) ;
		dataMol.betaMol_avg  = (double*) new double [glbParam->nBins] ; memset( dataMol.betaMol_avg , 0, ( sizeof(double)*glbParam->nBins) ) ;
		dataMol.alphaMol_avg = (double*) new double [glbParam->nBins] ; memset( dataMol.alphaMol_avg, 0, ( sizeof(double)*glbParam->nBins) ) ;
		dataMol.nMol  	  	 = (double*) new double [glbParam->nBins] ; memset( dataMol.nMol     	, 0, ( sizeof(double)*glbParam->nBins) ) ;
		dataMol.prMol	  	 = (double*) new double [glbParam->nBins] ; memset( dataMol.prMol    	, 0, ( sizeof(double)*glbParam->nBins) ) ;
		dataMol.prMol_avg 	 = (double*) new double [glbParam->nBins] ; memset( dataMol.prMol_avg	, 0, ( sizeof(double)*glbParam->nBins) ) ;
		dataMol.pr2Mol	  	 = (double*) new double [glbParam->nBins] ; memset( dataMol.pr2Mol   	, 0, ( sizeof(double)*glbParam->nBins) ) ;
		dataMol.pr2Mol_avg 	 = (double*) new double [glbParam->nBins] ; memset( dataMol.pr2Mol   	, 0, ( sizeof(double)*glbParam->nBins) ) ;
		dataMol.zenith	  	 = 0 ;

		dataMol.tK	 = (double*) new double [glbParam->nBins] ; memset( dataMol.tK , 0, ( sizeof(double) * glbParam->nBins) ) ;
		dataMol.pPa	 = (double*) new double [glbParam->nBins] ; memset( dataMol.pPa, 0, ( sizeof(double) * glbParam->nBins) ) ;

		dataMol.last_Indx_Mol_Low  = (int**) new int* [glbParam->nEventsAVG] ;
		dataMol.last_Indx_Mol_High = (int**) new int* [glbParam->nEventsAVG] ;
		dataMol.last_Indx_Bkg_Low  = (int**) new int* [glbParam->nEventsAVG] ;
		dataMol.last_Indx_Bkg_High = (int**) new int* [glbParam->nEventsAVG] ;

		dataMol.last_Range_Mol_Low  = (double**) new double* [glbParam->nEventsAVG] ;
		dataMol.last_Range_Mol_High = (double**) new double* [glbParam->nEventsAVG] ;
		dataMol.last_Range_Bkg_Low  = (double**) new double* [glbParam->nEventsAVG] ;
		dataMol.last_Range_Bkg_High = (double**) new double* [glbParam->nEventsAVG] ;	
		for ( int e=0 ; e <glbParam->nEventsAVG ; e++ )
		{
			dataMol.last_Indx_Mol_Low [e]  = (int*) new int [glbParam->nCh] ;			memset( dataMol.last_Indx_Mol_Low [e] , 0, ( sizeof(int) * glbParam->nCh) ) ;
			dataMol.last_Indx_Mol_High[e]  = (int*) new int [glbParam->nCh] ;			memset( dataMol.last_Indx_Mol_High[e] , 0, ( sizeof(int) * glbParam->nCh) ) ;
			dataMol.last_Indx_Bkg_Low [e]  = (int*) new int [glbParam->nCh] ;			memset( dataMol.last_Indx_Bkg_Low [e] , 0, ( sizeof(int) * glbParam->nCh) ) ;
			dataMol.last_Indx_Bkg_High[e]  = (int*) new int [glbParam->nCh] ;			memset( dataMol.last_Indx_Bkg_High[e] , 0, ( sizeof(int) * glbParam->nCh) ) ;

			dataMol.last_Range_Mol_Low [e]  = (double*) new double [glbParam->nCh] ;	memset( dataMol.last_Range_Mol_Low [e] , 0, ( sizeof(double) * glbParam->nCh) ) ;	
			dataMol.last_Range_Mol_High[e]  = (double*) new double [glbParam->nCh] ;	memset( dataMol.last_Range_Mol_High[e] , 0, ( sizeof(double) * glbParam->nCh) ) ;
			dataMol.last_Range_Bkg_Low [e]  = (double*) new double [glbParam->nCh] ;	memset( dataMol.last_Range_Bkg_Low [e] , 0, ( sizeof(double) * glbParam->nCh) ) ;
			dataMol.last_Range_Bkg_High[e]  = (double*) new double [glbParam->nCh] ;	memset( dataMol.last_Range_Bkg_High[e] , 0, ( sizeof(double) * glbParam->nCh) ) ;	
		}
}

// READ THE RADIOSONDE/MODEL DATA, FILL THE dataMol STRUCTURE AND TRANSFORM IT IN HIGH RESOLUTION
void CMolecularData::Get_Mol_Data_L1( strcGlobalParameters *glbParam )
{
	char 	ch ;
	int 	lines=0 ;
	char 	lineaRad[100], strDump[100] ;

	string radFile ;
	char radFilePath[512] = {0} ;
	ReadAnalysisParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"Path_To_Molecular_File", (const char*)"string", (char*)radFilePath, sizeof(radFilePath) ) ;
	radFile.assign(radFilePath) ;

	FILE *fid = fopen(radFile.c_str(), "r");
	if ( fid == NULL )
	{
		printf("\n Get_Mol_Data_L1(...): File %s can't be opened... exiting the execution \n", radFile.c_str() ) ;
		exit(1) ;
	}

	int indx_range =0, indx_Temp =0, indx_Pres =0, num_Header_Lines =0 ;
	ReadAnalysisParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"Range_column_index_in_File", (const char*)"int", (int*)&indx_range ) ;
	ReadAnalysisParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"Temp_column_index_in_File" , (const char*)"int", (int*)&indx_Temp  ) ;
	ReadAnalysisParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"Pres_column_index_in_File" , (const char*)"int", (int*)&indx_Pres  ) ;
	ReadAnalysisParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"num_Lines_in_Headers_Molecular_File" , (const char*)"int", (int*)&num_Header_Lines ) ;

// OBTENGO LA CANTIDAD DE FILAS, O CANTIDAD DE BINES DEL RADIOSONDEO (BAJA RESOLUCION).
	while( !feof(fid) )
	{
	  ch = fgetc(fid);
	  if(ch == '\n')	lines++ ;
	}
	fseek( fid, 0, 0 ) ; // RESET THE POINTER

	// READ RADIOSONDE DATA REFERENCED AT *ASL* AND LOW RESOLUTION (LR)
	RadSondeData.nBinsLR = lines - num_Header_Lines ;
	RadSondeData.zLR = (double*) new double [RadSondeData.nBinsLR] ;
	RadSondeData.pLR = (double*) new double [RadSondeData.nBinsLR] ;
	RadSondeData.tLR = (double*) new double [RadSondeData.nBinsLR] ;
	RadSondeData.nLR = (double*) new double [RadSondeData.nBinsLR] ;
	RadSondeData.alpha_mol = (double*) new double [RadSondeData.nBinsLR] ;
	RadSondeData.beta_mol  = (double*) new double [RadSondeData.nBinsLR] ;

	double *rad_Data_file = (double*) new double [3] ; 	memset( rad_Data_file, 0, (sizeof(double) *3) ) ;

	double hPa2Pa = 1.0 ;
	for ( int l=0 ; l<lines ; l++ ) // READ LINE BY LINE
	{
		// Skip header lines before reading actual data lines from the molecular file
		if (l < num_Header_Lines)
		{
			fgets(lineaRad, 100, fid) ;
		}
		else
		{		
			fgets(lineaRad, 100, fid) ;
			sscanf(lineaRad, "%lf,%lf,%lf,%s", &rad_Data_file[0], &rad_Data_file[1], &rad_Data_file[2], strDump ) ;
			RadSondeData.zLR[l-num_Header_Lines] = (double)rad_Data_file[indx_range] ;
			RadSondeData.tLR[l-num_Header_Lines] = (double)rad_Data_file[indx_Temp]  ;
			RadSondeData.pLR[l-num_Header_Lines] = (double)rad_Data_file[indx_Pres]  ;
			if ( (l==0) && (RadSondeData.pLR[0] >500) && (RadSondeData.pLR[0] <3000) ) // PRESSURE IS IN hPa --> MOVE IT TO Pa
				hPa2Pa = 100 ;
			RadSondeData.pLR[l-num_Header_Lines] = (double)RadSondeData.pLR[l-num_Header_Lines] * hPa2Pa ;

			// NORMALIZE THE TEMPERATURE AND PRESSURE TO THE GROUND LEVEL VALUES
			// RadSondeData.tLR[l] = (double) ( glbParam->temp_K_agl_AVG [0] * RadSondeData.tLR[l]/RadSondeData.tLR[0] ) ;
			// RadSondeData.pLR[l] = (double) ( glbParam->pres_Pa_agl_AVG[0] * RadSondeData.pLR[l]/RadSondeData.pLR[0] ) ;
		}
	}
	// printf( "\n\n Get_Mol_Data_L1: glbParam->temp_K_agl_AVG [0]= %lf\t glbParam->pres_Pa_agl_AVG [0]= %lf", glbParam->temp_K_agl_AVG[0], glbParam->pres_Pa_agl_AVG[0] ) ;
	// printf( "\n\n Get_Mol_Data_L1: RadSondeData.pLR[1]= %lf\t RadSondeData.tLR[1]= %lf", RadSondeData.pLR[1], RadSondeData.tLR[1] ) ;
	// printf( "\n Get_Mol_Data_L1: Range_column_index_in_File= %d \t Temp_column_index_in_File= %d \t Pres_column_index_in_File= %d", indx_range, indx_Temp, indx_Pres ) ;

	dataMol.z_Mol_Max = round(RadSondeData.zLR[RadSondeData.nBinsLR-1]) ;

	// search in the char* glbParam.Path_In if the string "holger" is found. If yes, then the data is from Holger Baars
	if ( (strstr(glbParam->Path_File_In, "holger") != NULL) || (strstr(glbParam->Path_File_In, "lpp_sim") != NULL)  )
	{
		// printf("\nUSING A SIMPLER MOLECULAR MODEL SINCE A LIDAR SIGNAL SIMULATION IS USED.\n") ;
		// GET RadSondeData.nLR, RadSondeData.alpha_mol AND RadSondeData.beta_mol IN THE SAME RESOLUTION OF 
		// RadSondeData.pLR AND RadSondeData.tLR PASSED AS THE FIRST AND SECOND ARGUMENTS (LR=Low Resolution)
		TemK_PresPa_to_N_Alpha_Beta_MOL_simple( (double*)RadSondeData.pLR, (double*)RadSondeData.tLR, (double)glbParam->iLambda[glbParam->indxWL_PDL1],
												(int)RadSondeData.nBinsLR, (double*)RadSondeData.nLR, (double*)RadSondeData.alpha_mol,
												(double*)RadSondeData.beta_mol, (double*)&dataMol.LR_mol ) ;
	}
	else
	{
		double co2_ppmv = 392 ;
		double wl_factor ;
		ReadAnalysisParameter( (char*)glbParam->FILE_SOFT_CODED_VALUES, (const char*)"wl_factor", (const char*)"double", (double*)&wl_factor ) ;

		// GET RadSondeData.nLR, RadSondeData.alpha_mol AND RadSondeData.beta_mol IN THE SAME RESOLUTION OF 
		// RadSondeData.pLR AND RadSondeData.tLR PASSED AS THE FIRST AND SECOND ARGUMENTS (LR=Low Resolution)
		// printf("\nUSING A COMPLEX MOLECULAR MODEL SINCE A REAL LIDAR SIGNAL IS USED.\n") ;
		TemK_PresPa_to_N_Alpha_Beta_MOL ( (double*)RadSondeData.pLR, (double*)RadSondeData.tLR, (double)(wl_factor*glbParam->iLambda[glbParam->indxWL_PDL1]*1e-9),
										  (double)co2_ppmv,	(int)RadSondeData.nBinsLR, (double*)RadSondeData.nLR, (double*)RadSondeData.alpha_mol,
										  (double*)RadSondeData.beta_mol, (double*)&dataMol.LR_mol ) ;
		// TemK_PresPa_to_N_Alpha_Beta_MOL ( (double*)RadSondeData.pLR, (double*)RadSondeData.tLR, (double)glbParam->iLambda[glbParam->indxWL_PDL1]*1e-9,
		// 								  (double)co2_ppmv,	(int)RadSondeData.nBinsLR, (double*)RadSondeData.nLR, (double*)RadSondeData.alpha_mol,
		// 								  (double*)RadSondeData.beta_mol, (double*)&dataMol.LR_mol ) ;
	}

// TILL HERE, THE PROFILES ARE IN LOW RESOLUTION (OR THE SAME RESOLUTION AS THE RADIOSOUND/MODEL)
// FIRST: CHECK IF THE LOW RESOLUTION IS THE SAME AS THE LIDAR. IF YES, SO THERE IS NO NEED TO RESAMPLE.
// IF NOT, THEN I HAVE TO RESAMPLE THE PROFILES IN HIGH RESOLUTION (HR) AND IN THE SLANT PATH DEFINED BY THE ZENITHAL ANGLE dataMol.zenith

	// CHECK IF THE RADIOSONDE RESOLUTION IS THE SAME AS THE LIDAR. IF YES, SO THERE IS NO NEED TO RESAMPLE
	//! VERIFICAR ESTO
	if ( (RadSondeData.zLR[2] - RadSondeData.zLR[1] ) == glbParam->dr )
		// FILL dataMol WHITHOUT RESAMPLING USING THE POLYNOMIAL FIT
		Fill_dataMol_L1_from_RadSondeData( (strcGlobalParameters*)glbParam ) ;
	else
	{
		// RESAMPLE dataMol IN HIGH RESOLUTION AND AT ZENITHAL ANGLE
		Fill_dataMol_L1( (strcGlobalParameters*)glbParam ) ;

		// RESAMPLE dataMol.pPa AND dataMol.tK IN HIGH RESOLUTION TO BE SAVED LATER
		Tem_Pres_to_HR_Lin( ) ;
	}
		// printf( "\n\nGet_Mol_Data_L1() despues de Tem_Pres_to_HR() --> RadSondeData.tLR[0]= %lf \t dataMol.tK[0]= %lf \n", RadSondeData.tLR[0], dataMol.tK [0] ) ;

	fclose(fid) ;
}

// FILL dataMol STRUCTURE FROM dataMol.tK AND dataMol.pPa ALREADY LOADED
void CMolecularData::Get_Mol_Data_L2( strcGlobalParameters *glbParam, CNetCDF_Lidar *oNCL, int ncid_L1_Data )
{
    int ncid_L1_MolData, retval ;
    if ( ( retval = nc_inq_grp_ncid( (int)ncid_L1_Data, (const char*)"Molecular_Data", (int*)&ncid_L1_MolData ) ) )
        ERR(retval);
	oNCL->ReadVar( (int)ncid_L1_MolData, (const char*)"Molecular_Extinction" , (double*)&dataMol.alphaMol[0] ) ;
    oNCL->ReadVar( (int)ncid_L1_MolData, (const char*)"Molecular_Backscatter", (double*)&dataMol.betaMol [0] ) ;
    oNCL->ReadVar( (int)ncid_L1_MolData, (const char*)"N_mol"    , (double*)&dataMol.nMol  [0] ) ;
    oNCL->ReadVar( (int)ncid_L1_MolData, (const char*)"prMol"    , (double*)&dataMol.prMol [0] ) ;
    oNCL->ReadVar( (int)ncid_L1_MolData, (const char*)"pr2Mol"   , (double*)&dataMol.pr2Mol[0] ) ;
    oNCL->ReadVar( (int)ncid_L1_MolData, (const char*)"LR_mol"   , (double*)&dataMol.LR_mol    ) ;

	// Get_Mol_Data_L2( (strcGlobalParameters*)glbParam ) ;
	Fill_dataMol_L2( (strcGlobalParameters*)glbParam ) ;

	for (int i = 0; i < dataMol.nBins; i++)
		dataMol.betaRam[i]  = (double) dataMol.nMol[i] * 1 ; // RAMAN BACKSCATTER
}

void CMolecularData::Fill_dataMol_L2( strcGlobalParameters *glbParam )
{
	int 	i ;

	if ( glbParam->evSel <0 )
		dataMol.zenith = (int)0 ;
	else
	{
		dataMol.zenith = (int)glbParam->aZenithAVG[glbParam->evSel] ;
		if ( dataMol.zenith == -90 )
			dataMol.zenith = (int) 0 ;
	}
	// VALUES ASL
	dataMol.nBins = glbParam->nBins ;

	for ( i=0 ; i < dataMol.nBins ; i++ )
		dataMol.zr[i] = (double)(glbParam->siteASL + glbParam->r[i] * cos(dataMol.zenith *M_PI/180)) ; // zr = ASL
	dataMol.dzr = (double)(dataMol.zr[1] - dataMol.zr[0]) ; // [m]
	glbParam->dzr = dataMol.dzr ;

	if ( dataMol.zenith != 0 )
		Molecular_Profile_Resampled_Zenithal_Lin( (strcGlobalParameters*)glbParam ) ; // RE-SAMPLE THE dataMol.nMol, dataMol.alphaMol AND dataMol.betaMol PROFILE TO THE ZENITHAL ANGLE

	Elastic_Rayleigh_Lidar_Signal ( (double*)glbParam->r ) ;
}

// FILL dataMol FROM LOW TO HIGH RESOLUTION AND ZENITHAL ANGLE
void CMolecularData::Fill_dataMol_L1( strcGlobalParameters *glbParam )
{
	if ( glbParam->evSel <0 )
		dataMol.zenith = (int)0 ;
	else
	{
		dataMol.zenith = (int)glbParam->aZenithAVG[glbParam->evSel] ;
		if ( dataMol.zenith == -90 )
			dataMol.zenith = (int) 0 ;
	}

	// VALUES FROM ASL
	dataMol.nBins = glbParam->nBins ;
	for ( int i=0 ; i < glbParam->nBins ; i++ )
		dataMol.zr[i] = (double)(glbParam->siteASL + glbParam->r[i] * cos(dataMol.zenith *M_PI/180)) ; // zr = ASL

	dataMol.dzr = (double)(dataMol.zr[1] - dataMol.zr[0]) ; // [m]
	glbParam->dzr = dataMol.dzr ;

	// RESAMPLED RadSondeData.nLR, RadSondeData.alpha_mol y RadSondeData.beta_mol TO HIGH RESOLUTION: dataMol.nMol, dataMol.alphaMol AND dataMol.betaMol
	// AND IN THE SLANT PATH DEFINED BY THE ZENITHAL ANGLE dataMol.zenith
	// Mol_Low_To_High_Res_Spl( ) ;
	Mol_Low_to_High_Res_Lin( ) ;

	// for ( i =0 ; i <glbParam->nBins ; i++ )
	// 	dataMol.nMol[i] = RadSondeData.nHR[i] ;
		// double N2_shift = 2331e2 ;
		// double N2_XS_BS = 3.5e-34 * pow( ( (1/glbParam->iLambda)-N2_shift ), 4) / pow( (1e9/337.1-N2_shift), 4 ) ;

	Elastic_Rayleigh_Lidar_Signal ( (double*)glbParam->r ) ;
}

void CMolecularData::Fill_dataMol_L1_from_RadSondeData( strcGlobalParameters *glbParam )
{
	if ( glbParam->evSel <0 )
		dataMol.zenith = (int)0 ;
	else
	{
		dataMol.zenith = (int)glbParam->aZenithAVG[glbParam->evSel] ;
		if ( dataMol.zenith == -90 )
			dataMol.zenith = (int) 0 ;
	}

	// VALUES FROM ASL
	dataMol.nBins = glbParam->nBins ;
	for ( int i=0 ; i < glbParam->nBins ; i++ )
		// dataMol.zr[i] = (double) glbParam->siteASL + glbParam->r[i] * cos(dataMol.zenith) ; // zr = ASL     *PI/180
		dataMol.zr[i] = (double)(glbParam->siteASL + glbParam->r[i] * cos(dataMol.zenith *M_PI/180)) ; // zr = ASL
	dataMol.dzr = (double)(dataMol.zr[1] - dataMol.zr[0]) ; // [m]
	glbParam->dzr = dataMol.dzr ;

 	// RESAMPLED RadSondeData.nLR, RadSondeData.alpha_mol y RadSondeData.beta_mol TO HIGH RESOLUTION: dataMol.nMol, dataMol.alphaMol AND dataMol.betaMol
	// AND IN THE SLANT PATH DEFINED BY THE ZENITHAL ANGLE dataMol.zenith
	// Mol_Low_To_High__Res( (strcGlobalParameters*)glbParam ) ;

	for ( int i =0 ; i <RadSondeData.nBinsLR ; i++ )
	{
		dataMol.nMol    [i] = (double) RadSondeData.nLR[i] 	     ;
		dataMol.alphaMol[i] = (double) RadSondeData.alpha_mol[i] ;
		dataMol.betaMol [i] = (double) RadSondeData.beta_mol[i]  ;

		dataMol.tK[i]  = (double) RadSondeData.tLR[i] ;
		dataMol.pPa[i] = (double) RadSondeData.pLR[i] ;
	}
	for (int i =RadSondeData.nBinsLR ; i <dataMol.nBins ; i++ )
	{
		dataMol.nMol    [i] = (double) dataMol.nMol    [RadSondeData.nBinsLR-1] ;
		dataMol.alphaMol[i] = (double) dataMol.alphaMol[RadSondeData.nBinsLR-1] ;
		dataMol.betaMol [i] = (double) dataMol.betaMol [RadSondeData.nBinsLR-1] ;

		dataMol.tK[i]  = (double) dataMol.tK [RadSondeData.nBinsLR-1] ;
		dataMol.pPa[i] = (double) dataMol.pPa[RadSondeData.nBinsLR-1] ;
	}

	// for ( i =0 ; i <glbParam->nBins ; i++ )
	// 	dataMol.nMol[i] = RadSondeData.nHR[i] ;
		// double N2_shift = 2331e2 ;
		// double N2_XS_BS = 3.5e-34 * pow( ( (1/glbParam->iLambda)-N2_shift ), 4) / pow( (1e9/337.1-N2_shift), 4 ) ;

	Elastic_Rayleigh_Lidar_Signal ( (double*)glbParam->r ) ;
}

void CMolecularData::Fill_dataMol_L2_from_RadSondeData( strcGlobalParameters *glbParam )
{
	if ( glbParam->evSel <0 )
		dataMol.zenith = (int)0 ;
	else
	{
		dataMol.zenith = (int)glbParam->aZenithAVG[glbParam->evSel] ;
		if ( dataMol.zenith == -90 )
			dataMol.zenith = (int) 0 ;
	}

	// VALUES FROM ASL
	dataMol.nBins = glbParam->nBins ;
	for ( int i=0 ; i < glbParam->nBins ; i++ )
		// dataMol.zr[i] = (double) glbParam->siteASL + glbParam->r[i] * cos(dataMol.zenith) ; // zr = ASL    *PI/180
		dataMol.zr[i] = (double)(glbParam->siteASL + glbParam->r[i] * cos(dataMol.zenith *M_PI/180)) ; // zr = ASL
	dataMol.dzr = (double)(dataMol.zr[1] - dataMol.zr[0]) ; // [m]
	glbParam->dzr = dataMol.dzr ;

	// for ( int i =0 ; i <RadSondeData.nBinsLR ; i++ )
	for ( int i =0 ; i <dataMol.nBins ; i++ )
	{
		dataMol.nMol    [i] = (double) RadSondeData.nHR[i] 	     ;
		dataMol.alphaMol[i] = (double) RadSondeData.alpha_mol[i] ;
		dataMol.betaMol [i] = (double) RadSondeData.beta_mol[i]  ;
	}
	// for (int i =RadSondeData.nBinsLR ; i <dataMol.nBins ; i++ )
	// {
	// 	dataMol.nMol    [i] = (double) dataMol.nMol    [RadSondeData.nBinsLR-1] ;
	// 	dataMol.alphaMol[i] = (double) dataMol.alphaMol[RadSondeData.nBinsLR-1] ;
	// 	dataMol.betaMol [i] = (double) dataMol.betaMol [RadSondeData.nBinsLR-1] ;
	// }

	// for ( i =0 ; i <glbParam->nBins ; i++ )
	// 	dataMol.nMol[i] = RadSondeData.nHR[i] ;
		// double N2_shift = 2331e2 ;
		// double N2_XS_BS = 3.5e-34 * pow( ( (1/glbParam->iLambda)-N2_shift ), 4) / pow( (1e9/337.1-N2_shift), 4 ) ;

	Elastic_Rayleigh_Lidar_Signal ( (double*)glbParam->r ) ;
}

/*
// FILL dataMol STRUCTURE IN HIGH RESOLUTION AND AT THE ZENITHAL ANGLE SELECTED.
void CMolecularData::Fill_dataMol_L2( strcGlobalParameters *glbParam )
{
	int 	i ;

	if ( glbParam->evSel <0 )
		dataMol.zenith = (int)0 ;
	else
	{
		dataMol.zenith = (int)glbParam->aZenithAVG[glbParam->evSel] ;
		if ( dataMol.zenith == -90 )
			dataMol.zenith = (int) 0 ;
	}
	// VALUES FROM ASL

	dataMol.nBins = glbParam->nBins ;

	for ( i=0 ; i < dataMol.nBins ; i++ )
		// dataMol.zr[i] = (double)(glbParam->siteASL + glbParam->r[i] * cos(dataMol.zenith)) ; // zr = ASL
		dataMol.zr[i] = (double)(glbParam->siteASL + glbParam->r[i] * cos(dataMol.zenith *M_PI/180)) ; // zr = ASL
	dataMol.dzr = (double)(dataMol.zr[1] - dataMol.zr[0]) ; // [m]
	glbParam->dzr = dataMol.dzr ;

	// printf( "\n ev= %d\tzenith= %lf\t glbParam->r[1000]= %lf \tzr[1000]= %lf\n", glbParam->evSel, dataMol.zenith, glbParam->r[1000], dataMol.zr[1000]) ;
	// printf( " dataMol.zenith *M_PI/180= %lf \t cos(dataMol.zenith *M_PI/180)= %lf\n", dataMol.zenith *M_PI/180, cos(dataMol.zenith *M_PI/180) ) ;
	// printf( " dataMol.zenith= %lf \t cos(dataMol.zenith)= %lf\n", dataMol.zenith, cos(dataMol.zenith) ) ;

	if ( dataMol.zenith != 0 )
		Molecular_Profile_Resampled_Zenithal( (strcGlobalParameters*)glbParam ) ; // RE-SAMPLE THE dataMol.nMol, dataMol.alphaMol AND dataMol.betaMol PROFILE TO THE ZENITHAL ANGLE
	else
	{
		for ( i=0 ; i < dataMol.nBins ; i++ )
		{
			dataMol.zr[i] 		= (double)(glbParam->siteASL + glbParam->r[i]) ; // zr = ASL
			dataMol.nMol[i] 	= (double) RadSondeData.nHR[i] 		 ;
			dataMol.alphaMol[i] = (double) RadSondeData.alpha_mol[i] ;
			dataMol.betaMol[i]	= (double) RadSondeData.beta_mol [i] ;
		}
	}

	Elastic_Rayleigh_Lidar_Signal ( (double*)glbParam->r ) ;
}
*/


// void CMolecularData::Alpha_Beta_Mol_from_N_Mol( strcGlobalParameters *glbParam )
// {
//  	for( int i=0 ; i < glbParam->nBins ; i++ )
// 	{
// 		dataMol.betaMol[i]  = (double)(dataMol.nMol[i] * ( 5.45 * pow(10, -32) * pow((550.0/glbParam->iLambda[glbParam->chSel] ), 4) ) ) ; // r [1/m*sr]
// 		dataMol.alphaMol[i] = (double)(dataMol.betaMol[i] * 8.0 * 3.1415/3.0) ; // r [1/m]
// 	}
// 	Elastic_Rayleigh_Lidar_Signal ( (double*)glbParam->r ) ;
// }

void CMolecularData::Elastic_Rayleigh_Lidar_Signal ( double *r )
{
	double *MOD = (double*) new double[ dataMol.nBins ] ;

	cumtrapz( (r[1]-r[0]), dataMol.alphaMol, 0, dataMol.nBins-1, (double*)MOD ) ;

	for ( int b=0 ; b < dataMol.nBins  ; b++ )
	{
		dataMol.pr2Mol[b] 	= (double)  dataMol.betaMol[b] * exp( -2*MOD[b] ) ;
		dataMol.prMol [b] 	= (double) ( dataMol.pr2Mol[b]/(r[b]*r[b]) ) ;
	}
	// delete MOD ;
}

void CMolecularData::Mol_Low_To_High_Res_Spl( ) // USED IN lpp1.cpp
{
	if ( RadSondeData.nBinsLR < 2 || dataMol.nBins < 1 )
		return ;

	// Normalize input data to avoid numerical precision loss in spline interpolation
	// Large values (e.g., 2.55e+25 for molecular density) cause NaN in polynomial evaluation
	double norm_n 		= RadSondeData.nLR		[RadSondeData.nBinsLR - 1] ;
	double norm_alpha 	= RadSondeData.alpha_mol[RadSondeData.nBinsLR - 1] ;
	double norm_beta 	= RadSondeData.beta_mol	[RadSondeData.nBinsLR - 1] ;

	double *nLR_norm      = (double*) new double [RadSondeData.nBinsLR] ;
	double *alpha_norm    = (double*) new double [RadSondeData.nBinsLR] ;
	double *beta_norm     = (double*) new double [RadSondeData.nBinsLR] ;

	for ( int i = 0 ; i < RadSondeData.nBinsLR ; i++ )
	{
		nLR_norm[i]   = RadSondeData.nLR      [i] / norm_n 	   ;
		alpha_norm[i] = RadSondeData.alpha_mol[i] / norm_alpha ;
		beta_norm[i]  = RadSondeData.beta_mol [i] / norm_beta  ;
	}

	Spline *spl_n 			= (Spline *)malloc(sizeof(Spline) * (RadSondeData.nBinsLR - 1)) ;
	Spline *spl_alpha_mol 	= (Spline *)malloc(sizeof(Spline) * (RadSondeData.nBinsLR - 1)) ;
	Spline *spl_beta_mol 	= (Spline *)malloc(sizeof(Spline) * (RadSondeData.nBinsLR - 1)) ;

	// Create splines from normalized data
	create_natural_cubic_spline(RadSondeData.zLR, nLR_norm		, RadSondeData.nBinsLR, spl_n		  ) ;
	create_natural_cubic_spline(RadSondeData.zLR, alpha_norm	, RadSondeData.nBinsLR, spl_alpha_mol ) ;
	create_natural_cubic_spline(RadSondeData.zLR, beta_norm		, RadSondeData.nBinsLR, spl_beta_mol  ) ;

	// Valid spline range: [RadSondeData.zLR[0], RadSondeData.zLR[nBinsLR-1]]
	// Clamp query points to stay within valid interpolation bounds to avoid NaN.
	double zMinLR      = RadSondeData.zLR[0] ;
	double zMaxLR      = RadSondeData.zLR[RadSondeData.nBinsLR - 1] ;

	for ( int i = 0 ; i < dataMol.nBins ; i++ )
	{
		double z_query = dataMol.zr[i] ;

		// Clamp query point to valid spline bounds to prevent NaN from evaluate_spline
		if ( z_query < zMinLR )
			z_query = zMinLR ;
		else if ( z_query > zMaxLR )
			z_query = zMaxLR ;

		// Evaluate spline on normalized data, then denormalize
		double nMol_norm    = evaluate_spline( spl_n		, RadSondeData.nBinsLR, z_query ) ;
		double alphaMol_norm = evaluate_spline( spl_alpha_mol, RadSondeData.nBinsLR, z_query ) ;
		double betaMol_norm  = evaluate_spline( spl_beta_mol	, RadSondeData.nBinsLR, z_query ) ;

		dataMol.nMol    [i] = nMol_norm * norm_n ;
		dataMol.alphaMol[i] = alphaMol_norm * norm_alpha ;
		dataMol.betaMol [i] = betaMol_norm * norm_beta ;
	}

	delete[] nLR_norm ;
	delete[] alpha_norm ;
	delete[] beta_norm ;

	free(spl_n)			;
	free(spl_alpha_mol) ;
	free(spl_beta_mol)  ;
}

void CMolecularData::Mol_Low_to_High_Res_Lin( ) // USED IN lpp1.cpp - Linear interpolation version
{
	if ( RadSondeData.nBinsLR < 2 || dataMol.nBins < 1 )
		return ;

	// Valid interpolation range: [RadSondeData.zLR[0], RadSondeData.zLR[nBinsLR-1]]
	double zMinLR = RadSondeData.zLR[0] ;
	double zMaxLR = RadSondeData.zLR[RadSondeData.nBinsLR - 1] ;

	for ( int i = 0 ; i < dataMol.nBins ; i++ )
	{
		double z_query = dataMol.zr[i] ;

		// Clamp query point to valid interpolation bounds
		if ( z_query < zMinLR )
			z_query = zMinLR ;
		else if ( z_query > zMaxLR )
			z_query = zMaxLR ;

		int idx = 0;
		dataMol.nMol    [i] = LinearInterpolation( RadSondeData.zLR, RadSondeData.nLR	   , RadSondeData.nBinsLR, z_query, &idx ) ;
		dataMol.alphaMol[i] = LinearInterpolation( RadSondeData.zLR, RadSondeData.alpha_mol, RadSondeData.nBinsLR, z_query, &idx ) ;
		dataMol.betaMol [i] = LinearInterpolation( RadSondeData.zLR, RadSondeData.beta_mol , RadSondeData.nBinsLR, z_query, &idx ) ;
	}
}

void CMolecularData::Tem_Pres_to_HR_Lin() // USED IN lpp1.cpp
{
	int idx = 0;

	for (int i = 0; i < dataMol.nBins; i++)
	{
		double z_query = dataMol.zr[i];

		while (idx < RadSondeData.nBinsLR - 2 && z_query > RadSondeData.zLR[idx + 1])
			idx++;

		if (z_query <= RadSondeData.zLR[0])
		{
			dataMol.tK[i]  = RadSondeData.tLR[0];
			dataMol.pPa[i] = RadSondeData.pLR[0];
		}
		else if (z_query >= RadSondeData.zLR[RadSondeData.nBinsLR - 1])
		{
			int last = RadSondeData.nBinsLR - 1;
			dataMol.tK[i]  = RadSondeData.tLR[last];
			dataMol.pPa[i] = RadSondeData.pLR[last];
		}
		else
		{
			dataMol.tK [i] = LinearInterpolation(RadSondeData.zLR, RadSondeData.tLR, RadSondeData.nBinsLR, z_query, &idx);
			dataMol.pPa[i] = LinearInterpolation(RadSondeData.zLR, RadSondeData.pLR, RadSondeData.nBinsLR, z_query, &idx);
		}
	}
}

// void CMolecularData::Tem_Pres_to_HR_Spl() // USED IN lpp1.cpp
// {
// 	Spline *spl_t = (Spline *)malloc(sizeof(Spline) * (RadSondeData.nBinsLR - 1)) ;
// 	Spline *spl_p = (Spline *)malloc(sizeof(Spline) * (RadSondeData.nBinsLR - 1)) ;

// 	create_natural_cubic_spline(RadSondeData.zLR, RadSondeData.tLR, RadSondeData.nBinsLR, spl_t) ;
// 	create_natural_cubic_spline(RadSondeData.zLR, RadSondeData.pLR, RadSondeData.nBinsLR, spl_p) ;

// 	for (int i =0 ; i <dataMol.nBins ; i++ )
// 	{
// 		dataMol.tK [i] = evaluate_spline( spl_t, RadSondeData.nBinsLR, dataMol.zr[i] ) ;
// 		dataMol.pPa[i] = evaluate_spline( spl_p, RadSondeData.nBinsLR, dataMol.zr[i] ) ;
// 	}

// 	free(spl_t);
// 	free(spl_p);
// }

void CMolecularData::Molecular_Profile_Resampled_Zenithal( strcGlobalParameters *glbParam ) // USED IN lpp2.cpp
{
	Spline *spl_n 	  = (Spline *)malloc(sizeof(Spline) * (dataMol.nBins - 1)) ;
	Spline *spl_alpha = (Spline *)malloc(sizeof(Spline) * (dataMol.nBins - 1)) ;
	Spline *spl_beta  = (Spline *)malloc(sizeof(Spline) * (dataMol.nBins - 1)) ;

	create_natural_cubic_spline(RadSondeData.zHR, RadSondeData.nHR		, dataMol.nBins, spl_n	  ) ;
	create_natural_cubic_spline(RadSondeData.zHR, RadSondeData.alpha_mol, dataMol.nBins, spl_alpha ) ;
	create_natural_cubic_spline(RadSondeData.zHR, RadSondeData.beta_mol , dataMol.nBins, spl_beta  ) ;

	for (int i =0 ; i <dataMol.nBins ; i++ )
	{
		dataMol.nMol[i] 	= evaluate_spline( spl_n	, dataMol.nBins, dataMol.zr[i] );
		dataMol.alphaMol[i] = evaluate_spline( spl_alpha, dataMol.nBins, dataMol.zr[i] );
		dataMol.betaMol[i] 	= evaluate_spline( spl_beta , dataMol.nBins, dataMol.zr[i] );
	}
	free(spl_n)	    ;
	free(spl_alpha) ;
	free(spl_beta)  ;
}

void CMolecularData::Molecular_Profile_Resampled_Zenithal_Lin( strcGlobalParameters *glbParam ) // USED IN lpp2.cpp
{
	// Linear resampling along zenithal path using the existing LinearInterpolation
	if ( dataMol.nBins < 2 )
		return;

	// Create copies of the original profiles to avoid reading overwritten values
	double *src_n = (double*) new double [dataMol.nBins];
	double *src_a = (double*) new double [dataMol.nBins];
	double *src_b = (double*) new double [dataMol.nBins];
	for (int j = 0; j < dataMol.nBins; j++)
	{
		src_n[j] = dataMol.nMol[j];
		src_a[j] = dataMol.alphaMol[j];
		src_b[j] = dataMol.betaMol[j];
	}

	int idx_n = 0, idx_a = 0, idx_b = 0;
	for (int i = 0; i < dataMol.nBins; i++)
	{
		double zq = dataMol.zr[i];

		dataMol.nMol[i]     = LinearInterpolation(glbParam->r, src_n, dataMol.nBins, zq, &idx_n);
		dataMol.alphaMol[i] = LinearInterpolation(glbParam->r, src_a, dataMol.nBins, zq, &idx_a);
		dataMol.betaMol[i]  = LinearInterpolation(glbParam->r, src_b, dataMol.nBins, zq, &idx_b);
	}

	delete[] src_n;
	delete[] src_a;
	delete[] src_b;
}

void CMolecularData::Molecular_Profile_Resampled_Zenithal_v1( strcGlobalParameters *glbParam ) // USED IN lpp2.cpp
{
	Spline *spl_n 	  = (Spline *)malloc(sizeof(Spline) * (dataMol.nBins - 1)) ;
	Spline *spl_alpha = (Spline *)malloc(sizeof(Spline) * (dataMol.nBins - 1)) ;
	Spline *spl_beta  = (Spline *)malloc(sizeof(Spline) * (dataMol.nBins - 1)) ;

	create_natural_cubic_spline(glbParam->r, dataMol.nMol	 , dataMol.nBins, spl_n	    ) ;
	create_natural_cubic_spline(glbParam->r, dataMol.alphaMol, dataMol.nBins, spl_alpha ) ;
	create_natural_cubic_spline(glbParam->r, dataMol.betaMol , dataMol.nBins, spl_beta  ) ;

	for (int i =0 ; i <dataMol.nBins ; i++ )
	{
		dataMol.nMol[i] 	= evaluate_spline( spl_n	, dataMol.nBins, dataMol.zr[i] );
		dataMol.alphaMol[i] = evaluate_spline( spl_alpha, dataMol.nBins, dataMol.zr[i] );
		dataMol.betaMol[i] 	= evaluate_spline( spl_beta , dataMol.nBins, dataMol.zr[i] );
	}
	free(spl_n)	    ;
	free(spl_alpha) ;
	free(spl_beta)  ;
}

void CMolecularData::TemK_PresPa_to_N_Alpha_Beta_MOL ( double *pres_PA, double *temp_K, double lambda_m, double co2_ppmv, int nBins, double *N_mol, double *alpha_mol, double *beta_mol, double *LR_mol )
{
// -----------------------------------------------------------------------
// Description:
//
//    Defines important physic's constants, most of which are used
//    by the molecular scattering calculation.
//
//    Then takes temperature and pressure from the reference sounding and
//    calculates the refractive index, king's correction factor,
//    depolarization ratio, molecular phase function and cross-section
//    and, finally, molecular backscatter and extinction coefficients.
//
// Usage:
//
//    [alpha_mol,beta_mol,LR_mol,Constants]=alphabeta(pres,temp,lambda_m,co2_ppmv);
//    Input the desired pressure, temperature, laser wavelength and
//    CO2 concentration and execute the M-file.
//
// Input:
//
//    pres           - Atmospheric pressure profile                      [P]
//    temp           - Atmospheric temperature profile                   [K]
//    lambda_m       - wavelength vector                                 [m]
//    co2_ppmv       - CO2 concentration                              [ppmv]
//
// Ouput:
//
//    alpha_mol        - extinction coefficient                       [m^-1]
//    beta_mol         - backscatter coefficient                [m^-1 sr^-1]
//    Constants.k      - Boltzmann constant                 [m^2 kg / s^2 K]
//    Constants.Na     - Avogadro's number                               [#]
//    Constants.Mvol   - Molar volume at 1013.25hPa and 273.15K   [L mol^-1]
//    Constants.Rgas   - Universal gas constant              [J K^-1 mol^-1]
//    Constants.Rair   - Dry air gas constant                 [J K^-1 kg^-1]
//    Constants.Tstd   - Temperature of standard atmosphere              [K]
//    Constants.Pstd   - Pressure of standard atmosphere                [Pa]
//    Constants.Nstd   - Molecular density at Tstd and Pstd         [# m^-3]
//
//    Atmospheric Constituents Concentration [ppv] and
//    Molecular weight for: N2, O2, Ar, Ne, He, Kr, H2 Xe         [g mol^-1]
//
//    Constants.lambda_m - wavelengths                                     [m]
//    Constants.nAir   - index of refraction                             [-]
//    Constants.fAir   - King factor of air                              [-]
//    Constants.rhoAir - depolarization factor of air                    [-]
//    Constants.Pf_mol - phase function at -180deg                       [-]
//    Constants.LR_mol - molecular lidar ratio                          [sr]
//
// Authors:
//
//    H. M. J. Barbosa (hbarbosa@if.usp.br), IF, USP, Brazil
//    B. Barja         (bbarja@gmail.com), GOAC, CMC, Cuba
//    R. Costa         (re.dacosta@gmail.com), IPEN, Brazil
//
// References:
//
//    Seinfeld and Pandis (1998)
//    Handbook of Physics and Chemistry (CRC 1997)
//    Wallace and Hobbs, Atmospheric science (2nd Edition)
//    Bodhaine et al, 1999: J. Atmos. Ocea. Tech, v. 16, p.1854
//
//    Bates, 1984: Planet. Space Sci., v. 32, p. 785
//    Bodhaine et al, 1999: J. Atmos. Ocea. Tech, v. 16, p.1854  --> https://journals.ametsoc.org/view/journals/atot/16/11/1520-0426_1999_016_1854_orodc_2_0_co_2.xml
//    Bucholtz, 1995: App. Opt., v. 34 (15), p. 2765  			 --> http://www.lalinet.org/viiiwlmla/Courses/Lecture4/Bucholtz_Applied%20optics_1995.pdf
//    Chandrasekhar, Radiative Transfer (Dover, New York, 1960)
//    Edlen, 1953: J. Opt. Soc. Amer., v. 43, p. 339
//    McCartney, E. J., 1976: Optics of the Atmosphere. Wiley, 408 pp.
//    Peck and Reeder, 1973: J. Opt. Soc. Ame., v 62 (8), p. 958
//
// ------------------------------------------------------------------------
// Fixed definitions
//------------------------------------------------------------------------
// Atmospheric Constituents Concentration                              [ppv]
// ref: Seinfeld and Pandis (1998)
double N2ppv   =    0.78084;
double O2ppv   =    0.20946;
double Arppv   =    0.00934;
// double Neppv   =    1.80    * 1e-5;
// double Heppv   =    5.20    * 1e-6;
// double Krppv   =    1.10    * 1e-6;
// double H2ppv   =    5.80    * 1e-7;
// double Xeppv   =    9.00    * 1e-8;
// double CO2ppv  = 	co2_ppmv* 1e-6;

// Atmospheric Constituents Molecular weight [g mol^-1]
// ref: Handbook of Physics and Chemistry (CRC 1997)
// double N2mwt  = 28.013	;
// double O2mwt  = 31.999	;
// double Armwt  = 39.948	;
// double Nemwt  = 20.18	;
// double Hemwt  = 4.003	;
// double Krmwt  = 83.8	;
// double H2mwt  = 2.016	;
// double Xemwt  = 131.29	;
// double CO2mwt = 44.01  	;

// Dry air molecular mass [g mol^-1]
// double Airmwt= 	(N2ppv*N2mwt + O2ppv*O2mwt + Arppv*Armwt + Neppv*Nemwt + Heppv*Hemwt + Krppv*Krmwt + H2ppv*H2mwt + Xeppv*Xemwt + CO2ppv*CO2mwt) /
		 		// ( N2ppv + O2ppv + Arppv + Neppv + Heppv + Krppv + H2ppv + Xeppv + CO2ppv );

// Physic Constants
// double k  = 1.3806503e-23; // Boltzmann Constant                           [J K^-1]
double Na = 6.0221367e+23;  // Avogadro's number                         [# mol^-1]

// Wallace and Hobbs, p. 65
// double Rgas = k*Na;            // Universal gas constant            [J K^-1 mol^-1]
// double Rair = Rgas/Airmwt*1e3; // Dry air gas constant               [J K^-1 kg^-1]

// Standard Atmosphere Reference Values
double T0   = 273.15 ; // zero deg celcius                                     [K]
double Tstd = 288.15 ; // Temperature 288.15 K = 15 C                          [K]
double Pstd = 101325 ; // Pressure                                            [Pa]

// Molar volume at Pstd and T0
// Bodhaine et al, 1999
double Mvol =  22.4141e-3;                                          // [m^3 mol^-1]
// Molecular density for standar air. From Eq 24 of Bodhaine et al (1999)
double Nstd = (Na/Mvol)*(T0/Tstd) ;                                 // [# m^-3]  --> VERSION LPP

// -----------------------------------------------------------------------
//  REFRACTIVE INDEX WITH CO2 CORRECTION
// ------------------------------------------------------------------------

// Calculate (n-1) at 300 ppmv CO2
// Peck and Reeder (1973), Eqs (2) and (3)
// or Bucholtz (1995), Eqs (4) and (5)
// or Bodhaine et al (1999), Eq (4)
double dn300 ;
if ( (lambda_m*1e6) > 0.23 )
    dn300 = (5791817 /  (238.0185 - 1/ pow(lambda_m*1e6, 2) ) + 167909  / (57.362 - 1 / pow(lambda_m*1e6, 2) ) ) *1e-8 ;
else
	// dn300 = (8060.51 + 2480990  / (132.274 - 1 /  pow(lambda_m*1e6, 2) ) + 14455.7  / (39.32957 - 1/  pow(lambda_m*1e6, 2)) ) *1e-8; // 14455.7 --> UTILIZADO EN LPP
       dn300 = (8060.51 + 2480990  / (132.274 - 1 /  pow(lambda_m*1e6, 2) ) + 17456.3  / (39.32957 - 1/  pow(lambda_m*1e6, 2)) ) *1e-8; // 17456.3 --> UTILIZADO EN GFATPY

//  Correct for different concentration of CO2
//  Bodhaine et al (1999), Eq (19)
double dnAir = dn300 * (1 + (0.54 * (co2_ppmv*1e-6 - 0.0003))) ;

//  Actual air's index of refraction at 300 ppmv CO2 (nS for SCC paper part 1)
double nAir = 1 + dnAir;

// -----------------------------------------------------------------------
//  KING FACTOR AND DEPOLARIZATION RATIO
// ------------------------------------------------------------------------
// Bates (1984), Eqs (13) and (14)
// or Bodhaine et al (1999), Eqs (5) and (6)

// Nitrogen
double fN2  = 1.034 + ( 3.17e-4 / pow( lambda_m*1e6, 2 ) ) ;
// Oxygen
double fO2  = 1.096 + (1.385e-3 / pow( lambda_m*1e6, 2 ) ) +  (1.448e-4 / pow( lambda_m*1e6, 4 ) );
// Argon
double fAr  = 1;
// Carbon dioxide
double fCO2 = 1.15;

// Bodhaine et al (1999) Eq (23)
// NOTE: numerator and denominator are not written in percent

// Standard dry air mixture with co2_ppmv
double fAir = 	(N2ppv*fN2 + O2ppv*fO2 + Arppv*fAr + co2_ppmv*1e-6*fCO2) /
    			(N2ppv     + O2ppv     + Arppv     + co2_ppmv*1e-6) ;
//
// Depolarization ratio estimated from King's factor
//
// hmjb - What is the correct way?
//rhoN2  = (6*fN2 -6)./(3+7*fN2 );
//rhoO2  = (6*fO2 -6)./(3+7*fO2 );
//rhoCO2 = (6*fCO2-6)./(3+7*fCO2);
//rhoAr  = (6*fAr -6)./(3+7*fAr );
//rhoAir = (0.78084*rhoN2+0.20946*rhoO2+0.00934*rhoAr+co2_ppmv*1e-6*rhoCO2)/...
//	  (0.78084      +0.20946      +0.00934      +co2_ppmv*1e-6)
//
// hmjb - What is the correct way?
double rhoAir = (6*fAir-6) /(3+7*fAir);

// -----------------------------------------------------------------------
//  RAYLEIGH PHASE FUNCTION AT 180deg
// ------------------------------------------------------------------------
// Chandrasekhar (Chap. 1, p. 49)
// or Bucholtz (1995), eqs (12) and (13)
double gammaAir = rhoAir /(2-rhoAir);
double Pf_mol = 0.75*( (1+3 *gammaAir) + (1-gammaAir) * (cos(M_PI)*cos(M_PI)) ) / (1 + 2*gammaAir) ;

//% -----------------------------------------------------------------------
//  RAYLEIGH TOTAL SCATERING CROSS SECTION
//  -----------------------------------------------------------------------
// McCartney (1976) --> Cross Section [m^2]
double sigma_std = 24 * (M_PI*M_PI*M_PI) * pow((nAir*nAir-1), 2) * fAir / ( pow(lambda_m, 4) * (Nstd*Nstd) * pow( ((nAir*nAir)+2), 2) );

// -----------------------------------------------------------------------
//  RAYLEIGH VOLUME-SCATTERING COEFFICIENT
//  -----------------------------------------------------------------------

// In traditional lidar notation, Bucholtz (1995) eqs (2), (9) and (10)
// defines the scattering part of the molecular extinction coeficient.
// Therefore, here the usual greek letter 'alpha' is used instead of
// 'beta' as in Bucholtz.

// Bucholtz (1995), eq (9)
double alpha_std = Nstd * sigma_std;      // extinction at std pres and temp [m^-1]

// Bucholtz (1995), eq (10), units [m^-1]
// scaling for each P and T in the column
for ( int i=0 ; i<nBins ; i++ )
{
	// --> ESTE ES EL DE LALINET --> 
	alpha_mol[i] = (pres_PA[i] /temp_K[i]) * (Tstd/Pstd) * alpha_std ; // molecular extinction [m^-1]
	// alpha_mol[i] = pres_PA[i] /temp_K[i]    ;
	// alpha_mol[i] = alpha_mol[i] *Tstd/Pstd  ;
	// alpha_mol[i] = alpha_mol[i] * alpha_std ; // molecular extinction [m^-1]
	N_mol[i]	 = alpha_mol[i] / sigma_std ;
}

//% -----------------------------------------------------------------------
//  RAYLEIGH ANGULAR VOLUME-SCATTERING COEFFICIENT
// ------------------------------------------------------------------------

// Rayleigh extinction to backscatter ratio
*LR_mol = (4*M_PI)/Pf_mol;                         //Rayleigh lidar ratio [sr]
// *LR_mol = (8*M_PI)/3 ;                         //Rayleigh lidar ratio [sr]

// In traditional lidar notation, Bucholtz (1995) eq (14) defines the
// backscattering coeficient. Here the usual greek letter 'beta' is
// used as in Bucholtz.

// Multiply by phase function for -180deg and divide by 4pi steradians
// Units: [m]-1 [sr]-1
for( int i=0 ; i<nBins ; i++ )
	beta_mol[i] = alpha_mol[i] /(*LR_mol) ;       // molecular backscatter [m^-1]

// printf( "\nTemK_PresPa_to_N_Alpha_Beta_MOL() --> Molecular density at Tstd and Pstd: Nstd= %e [# m^-3]", Nstd ) ;
// printf( "\nTemK_PresPa_to_N_Alpha_Beta_MOL() --> sigma_std= %e \n", sigma_std    ) ;
// printf( "\nTemK_PresPa_to_N_Alpha_Beta_MOL() --> alpha_mol[0]= %e", alpha_mol[0] ) ;
// printf( "\nTemK_PresPa_to_N_Alpha_Beta_MOL() --> beta_mol[0] = %e", beta_mol[0]  ) ;
}

void CMolecularData::TemK_PresPa_to_N_Alpha_Beta_MOL_simple ( double *pres_PA, double *temp_K, double lambda_nm, int nBins, double *N_mol, double *alpha_mol, double *beta_mol, double *LR_mol )
{
	double kboltz = 1.3807e-23 ;
	*LR_mol = 8*M_PI/3 		 ;

	for (int i = 0; i < nBins; i++)
	{
		N_mol[i]     = pres_PA[i] / (temp_K[i] *kboltz) ; // PRESSURE IN PASCALS. TEMPERATURE IN KELVIN.
		beta_mol[i]  = N_mol[i] * pow((550/lambda_nm), 4.09) * 5.45*pow(10, -32) ; // [m^-1*sr^-1]
		alpha_mol[i] = beta_mol[i] * (*LR_mol) ;
	}
}

