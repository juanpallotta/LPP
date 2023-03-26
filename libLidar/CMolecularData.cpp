/*
 * CMolecularData.cpp
 * Copyright 2021 Juan V. Pallotta
 */

#include "CMolecularData.hpp"


CMolecularData::CMolecularData( strcGlobalParameters *glbParam )
{
	GetMem_dataMol( (int)glbParam->nBins ) ; // MEMORY ALLOCATION FOR dataMol STRUCTURE.
}

CMolecularData::~CMolecularData()
{

}

void CMolecularData::GetMem_dataMol( int nBins )
{
		dataMol.nBins	 = (int)nBins ;
		dataMol.zr		 = (double*) new double [nBins] ; memset( dataMol.zr	  , 0, ( sizeof(double) * nBins) ) ;
		dataMol.z_asl	 = (double*) new double [nBins] ; memset( dataMol.z_asl	  , 0, ( sizeof(double) * nBins) ) ;
		dataMol.betaMol  = (double*) new double [nBins] ; memset( dataMol.betaMol , 0, ( sizeof(double) * nBins) ) ;
		dataMol.alphaMol = (double*) new double [nBins] ; memset( dataMol.alphaMol, 0, ( sizeof(double) * nBins) ) ;
		dataMol.nMol  	 = (double*) new double [nBins] ; memset( dataMol.nMol    , 0, ( sizeof(double) * nBins) ) ;
		dataMol.prMol	 = (double*) new double [nBins] ; memset( dataMol.prMol   , 0, ( sizeof(double) * nBins) ) ;
		dataMol.pr2Mol	 = (double*) new double [nBins] ; memset( dataMol.pr2Mol  , 0, ( sizeof(double) * nBins) ) ;
		dataMol.zenith	 = 0 ;

		dataMol.tK	 = (double*) new double [nBins] ; memset( dataMol.tK , 0, ( sizeof(double) * nBins) ) ;
		dataMol.pPa	 = (double*) new double [nBins] ; memset( dataMol.pPa, 0, ( sizeof(double) * nBins) ) ;
}

void CMolecularData::Get_Mol_Data_L1( strcGlobalParameters *glbParam )
{
	char 	ch ;
	int 	lines=0 ;
	char 	lineaRad[100], strDump[100] ;

	string radFile ;
	ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"Path_To_Molecular_File", (const char*)"string", (char*)radFile.c_str() ) ;

	FILE *fid = fopen(radFile.c_str(), "r");
	if ( fid == NULL )
		printf("\n ReadUsSTDfile(...): No se puede abrir el archivo %s \n", radFile.c_str() ) ;

// OBTENGO LA CANTIDAD DE FILAS, O CANTIDAD DE BINES DEL RADIOSONDEO (BAJA RESOLUCION).
	while( !feof(fid) )
	{
	  ch = fgetc(fid);
	  if(ch == '\n')	lines++ ;
	}
	fseek( fid, 0, 0 ) ; // RESET THE POINTER

// READ RADIOSONDE DATA REFERENCED AT *ASL* AND LOW RESOLUTION (LR)
	RadSondeData.nBinsLR = lines ;
	RadSondeData.zLR = (double*) new double [RadSondeData.nBinsLR] ;
	RadSondeData.pLR = (double*) new double [RadSondeData.nBinsLR] ;
	RadSondeData.tLR = (double*) new double [RadSondeData.nBinsLR] ;
	RadSondeData.nLR = (double*) new double [RadSondeData.nBinsLR] ;
	RadSondeData.alpha_mol = (double*) new double [RadSondeData.nBinsLR] ;
	RadSondeData.beta_mol  = (double*) new double [RadSondeData.nBinsLR] ;

	double *rad_Data_file = (double*) new double [3] ; 	memset( rad_Data_file, 0, (sizeof(double) *3) ) ;

	int indx_range =0, indx_Temp =0, indx_Pres =0 ;
	ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"Range_column_index_in_File", (const char*)"int", (int*)&indx_range ) ;
	ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"Temp_column_index_in_File" , (const char*)"int", (int*)&indx_Temp  ) ;
	ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"Pres_column_index_in_File" , (const char*)"int", (int*)&indx_Pres  ) ;

	for ( int l=0 ; l<RadSondeData.nBinsLR ; l++ ) // BARRO LAS LINEAS.
	{
		fgets(lineaRad, 100, fid) ;
		sscanf(lineaRad, "%lf,%lf,%lf,%s", &rad_Data_file[0], &rad_Data_file[1], &rad_Data_file[2], strDump ) ;
		RadSondeData.zLR[l] = (double)rad_Data_file[indx_range] ;
		RadSondeData.tLR[l] = (double)rad_Data_file[indx_Temp]  ;
		RadSondeData.pLR[l] = (double)rad_Data_file[indx_Pres] *100  ; 
	}

	double co2_ppmv = 400 ;
	TemK_PresPa_to_N_Alpha_Beta_MOL ( (double*)RadSondeData.pLR, (double*)RadSondeData.tLR, (double)glbParam->iLambda[glbParam->indxWL_PDL1]*1e-9, (double)co2_ppmv, (int)RadSondeData.nBinsLR, 
  									  (double*)RadSondeData.nLR, (double*)RadSondeData.alpha_mol, (double*)RadSondeData.beta_mol, (double*)&dataMol.LR_mol ) ;

	Fill_dataMol_L1( (strcGlobalParameters*)glbParam ) ;

	// RadSondeData.radSoundingDataLOADED = true ;
	Tem_Pres_to_HR( ) ;

	fclose(fid) ;
}

void CMolecularData::Get_Mol_Data_L2( strcGlobalParameters *glbParam )
{
	double co2_ppmv = 400 ;
	RadSondeData.nHR 		= (double*) new double [dataMol.nBins] ;
	RadSondeData.alpha_mol 	= (double*) new double [dataMol.nBins] ;
	RadSondeData.beta_mol  	= (double*) new double [dataMol.nBins] ;

	TemK_PresPa_to_N_Alpha_Beta_MOL ( (double*)dataMol.pPa, (double*)dataMol.tK, (double)glbParam->iLambda[glbParam->indxWL_PDL2]*1e-9, (double)co2_ppmv, (int) dataMol.nBins, 
  									  (double*)RadSondeData.nHR, (double*)RadSondeData.alpha_mol, (double*)RadSondeData.beta_mol, (double*)&dataMol.LR_mol ) ;

	Fill_dataMol_L2( (strcGlobalParameters*)glbParam ) ;
}

void CMolecularData::Fill_dataMol_L1( strcGlobalParameters *glbParam )
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
	for ( i=0 ; i < glbParam->nBins ; i++ )
		dataMol.zr[i] = (double) glbParam->siteASL + glbParam->r[i] * cos(dataMol.zenith *PI/180) ; // zr = ASL
	dataMol.dzr = (double)(dataMol.zr[1] - dataMol.zr[0]) ; // [m]
	glbParam->dzr = dataMol.dzr ;

	Mol_Low_To_High_Res() ; // RESAMPLED RadSondeData.nLR TO HIGH RESOLUTION (RadSondeData.nHR) AND IN THE SLANT PATH AT THE ZENITHAL ANGLE dataMol.zenith

	// for ( i =0 ; i <glbParam->nBins ; i++ )
	// 	dataMol.nMol[i] = RadSondeData.nHR[i] ;
		// double N2_shift = 2331e2 ;
		// double N2_XS_BS = 3.5e-34 * pow( ( (1/glbParam->iLambda)-N2_shift ), 4) / pow( (1e9/337.1-N2_shift), 4 ) ;

	Elastic_Rayleigh_Lidar_Signal ( (double*)glbParam->r ) ;
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

	// VALUES FROM ASL
	dataMol.nBins = glbParam->nBins ;
	for ( i=0 ; i < glbParam->nBins ; i++ )
		dataMol.zr[i] = (double)(glbParam->siteASL + glbParam->r[i] * cos(dataMol.zenith *PI/180)) ; // zr = ASL
	dataMol.dzr = (double)(dataMol.zr[1] - dataMol.zr[0]) ; // [m]
	glbParam->dzr = dataMol.dzr ;

	Molecular_Profile_Resampled_Zenithal( (strcGlobalParameters*)glbParam ) ; // RE-SAMPLE THE nMol PROFILE TO THE ZENITHAL ANGLE

	Elastic_Rayleigh_Lidar_Signal ( (double*)glbParam->r ) ;
}

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
	delete MOD ;
}

void CMolecularData::Mol_Low_To_High_Res() // USED IN lidarAnalysis_PDL1.cpp
{
	double *coeff_N     = (double*) new double[3 +1] ;
	double *coeff_alpha = (double*) new double[3 +1] ;
	double *coeff_beta  = (double*) new double[3 +1] ;

	polyfitCoeff( (const double* const) RadSondeData.zLR, // X DATA
			      (const double* const) RadSondeData.nLR, // Y DATA
			      (unsigned int       ) RadSondeData.nBinsLR,
			      (unsigned int		  ) 3,
			      (double*			  ) coeff_N	 ) ;

	polyfitCoeff( (const double* const) RadSondeData.zLR, // X DATA
			      (const double* const) RadSondeData.alpha_mol, // Y DATA
			      (unsigned int       ) RadSondeData.nBinsLR,
			      (unsigned int		  ) 3,
			      (double*			  ) coeff_alpha	 ) ;

	polyfitCoeff( (const double* const) RadSondeData.zLR, // X DATA
			      (const double* const) RadSondeData.beta_mol, // Y DATA
			      (unsigned int       ) RadSondeData.nBinsLR,
			      (unsigned int		  ) 3,
			      (double*			  ) coeff_beta	 ) ;

	for (int i =0 ; i <dataMol.nBins ; i++ )
	{
		dataMol.nMol[i] 	= (double)( pow(dataMol.zr[i], 3) *coeff_N[3]     + pow(dataMol.zr[i], 2) *coeff_N[2]     + dataMol.zr[i]*coeff_N[1]     + coeff_N[0]     ) ;
		dataMol.alphaMol[i] = (double)( pow(dataMol.zr[i], 3) *coeff_alpha[3] + pow(dataMol.zr[i], 2) *coeff_alpha[2] + dataMol.zr[i]*coeff_alpha[1] + coeff_alpha[0] ) ;
		dataMol.betaMol[i]  = (double)( pow(dataMol.zr[i], 3) *coeff_beta[3]  + pow(dataMol.zr[i], 2) *coeff_beta[2]  + dataMol.zr[i]*coeff_beta[1]  + coeff_beta[0]  ) ;
	}
	delete coeff_N     ;
	delete coeff_alpha ;
	delete coeff_beta  ;
}

void CMolecularData::Tem_Pres_to_HR() // USED IN lidarAnalysis_PDL1.cpp
{
	double *coeff_Temp   = (double*) new double[3 +1] ;
	double *coeff_Pres   = (double*) new double[3 +1] ;

	polyfitCoeff( (const double* const) RadSondeData.zLR, // X DATA
			      (const double* const) RadSondeData.tLR, // Y DATA
			      (unsigned int       ) RadSondeData.nBinsLR,
			      (unsigned int		  ) 3,
			      (double*			  ) coeff_Temp	 ) ;

	polyfitCoeff( (const double* const) RadSondeData.zLR, // X DATA
			      (const double* const) RadSondeData.pLR, // Y DATA
			      (unsigned int       ) RadSondeData.nBinsLR,
			      (unsigned int		  ) 3,
			      (double*			  ) coeff_Pres	 ) ;

	for (int i =0 ; i <dataMol.nBins ; i++ )
	{
		dataMol.tK[i]  = (double)( pow(dataMol.zr[i], 3) *coeff_Temp[3] + pow(dataMol.zr[i], 2) *coeff_Temp[2] + dataMol.zr[i]*coeff_Temp[1] + coeff_Temp[0] ) ;
		dataMol.pPa[i] = (double)( pow(dataMol.zr[i], 3) *coeff_Pres[3] + pow(dataMol.zr[i], 2) *coeff_Pres[2] + dataMol.zr[i]*coeff_Pres[1] + coeff_Pres[0] ) ;
	}

	delete coeff_Temp     ;
	delete coeff_Pres ;
}

void CMolecularData::Molecular_Profile_Resampled_Zenithal( strcGlobalParameters *glbParam ) // USED IN lidarAnalysis_PDL2.cpp
{
	double *coeff_N     = (double*) new double[3 +1] ;
	double *coeff_alpha = (double*) new double[3 +1] ;
	double *coeff_beta  = (double*) new double[3 +1] ;

	for ( int i=0 ; i < glbParam->nBins ; i++ )
		dataMol.z_asl[i] = (double)( glbParam->siteASL + glbParam->r[i] ) ;

	polyfitCoeff( (const double* const) dataMol.z_asl 	 ,
			      (const double* const) RadSondeData.nHR ,
			      (unsigned int       ) dataMol.nBins    ,
			      (unsigned int		  ) 3				 ,
			      (double*			  ) coeff_N	 )	;

	polyfitCoeff( (const double* const) dataMol.z_asl     			,
			      (const double* const) RadSondeData.alpha_mol 	,
			      (unsigned int       ) dataMol.nBins 	  			,
			      (unsigned int		  ) 3							,
			      (double*			  ) coeff_alpha	 )	;

	polyfitCoeff( (const double* const) dataMol.z_asl    			,
			      (const double* const) RadSondeData.beta_mol 	,
			      (unsigned int       ) dataMol.nBins 	 			,
			      (unsigned int		  ) 3							,
			      (double*			  ) coeff_beta	 )	;

	for (int i =0 ; i <dataMol.nBins ; i++ )
	{
		dataMol.nMol[i]     = (double)( pow(dataMol.zr[i], 3) *coeff_N[3] + pow(dataMol.zr[i], 2) *coeff_N[2] + dataMol.zr[i]*coeff_N[1] + coeff_N[0] ) ;
		dataMol.alphaMol[i] = (double)( pow(dataMol.zr[i], 3) *coeff_alpha[3] + pow(dataMol.zr[i], 2) *coeff_alpha[2] + dataMol.zr[i]*coeff_alpha[1] + coeff_alpha[0] ) ;
		dataMol.betaMol[i]  = (double)( pow(dataMol.zr[i], 3) *coeff_beta[3]  + pow(dataMol.zr[i], 2) *coeff_beta[2]  + dataMol.zr[i]*coeff_beta[1]  + coeff_beta[0]  ) ;
	}

	delete coeff_N 		;
	delete coeff_alpha 	;
	delete coeff_beta  	;
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
//    lambda_m         - wavelength vector                                 [m]
//    co2_ppmv        - CO2 concentration                              [ppmv]
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
//    Bodhaine et al, 1999: J. Atmos. Ocea. Tech, v. 16, p.1854
//    Bucholtz, 1995: App. Opt., v. 34 (15), p. 2765
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

// Molecular density at Tstd and Pstd
double Nstd = (Na/Mvol)*(T0/Tstd);                                      // [# m^-3]

//% -----------------------------------------------------------------------
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
    dn300 = (8060.51 + 2480990  / (132.274 - 1 /  pow(lambda_m*1e6, 2) ) + 14455.7  / (39.32957 - 1/  pow(lambda_m*1e6, 2)) ) *1e-8;

//  Correct for different concentration of CO2
//  Bodhaine et al (1999), Eq (19)
double dnAir = dn300 * (1 + (0.54 * (co2_ppmv*1e-6 - 0.0003))) ;

//  Actual index of refraction at 300 ppmv CO2
double nAir = 1 + dnAir;

//% -----------------------------------------------------------------------
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
	alpha_mol[i] = pres_PA[i] /temp_K[i] *Tstd/Pstd * alpha_std ; // molecular extinction [m^-1]
	N_mol[i]	 = alpha_mol[i] / sigma_std ;
}

//% -----------------------------------------------------------------------
//  RAYLEIGH ANGULAR VOLUME-SCATTERING COEFFICIENT
// ------------------------------------------------------------------------

// Rayleigh extinction to backscatter ratio
*LR_mol = (4*M_PI)/Pf_mol;                         //Rayleigh lidar ratio [sr]

// In traditional lidar notation, Bucholtz (1995) eq (14) defines the
// backscattering coeficient. Here the usual greek letter 'beta' is
// used as in Bucholtz.

// Multiply by phase function for -180deg and divide by 4pi steradians
// Units: [m]-1 [sr]-1
for( int i=0 ; i<nBins ; i++ )
	beta_mol[i] = alpha_mol[i] /(*LR_mol) ;       // molecular backscatter [m^-1]

}
