
#include "CDataLevel_2.hpp"  

CDataLevel_2::CDataLevel_2( strcGlobalParameters *glbParam )
{
	LRM		   = 8*M_PI/3 ;
	pr2        = (double***) new double**[glbParam->nEventsAVG] ;
    alpha_Aer  = (double***) new double**[glbParam->nEventsAVG] ;
    beta_Aer   = (double***) new double**[glbParam->nEventsAVG] ;
    alpha_Aer_synergy  = (double**) new double*[glbParam->nEventsAVG] ;
    beta_Aer_synergy   = (double**) new double*[glbParam->nEventsAVG] ;
    AOD_LR	   = (double**)  new double *[glbParam->nEventsAVG] ;
    pr		   = (double**)  new double *[glbParam->nEventsAVG] ;
    dummy      = (double*)   new double  [glbParam->nBins ]     ;	memset( (double*)dummy  , 0, sizeof(double)*glbParam->nBins 	 ) ;
    pr2_s      = (double*)   new double  [glbParam->nBins ]     ;	memset( (double*)pr2_s  , 0, sizeof(double)*glbParam->nBins 	 ) ;
	LR_inv	   = (double*)   new double  [glbParam->nEventsAVG] ;	
	AOD_inv	   = (double*)   new double  [glbParam->nEventsAVG] ;	
	for (int i=0; i<glbParam->nEventsAVG; i++)
	{
		LR_inv[i]  = -1 ;
		AOD_inv[i] = -1 ;
	}

// FERNALD INVERSION VECTORS
	pr2n 			= (double*) new double[glbParam->nBins ] ;
	phi  			= (double*) new double[glbParam->nBins ] ;
	p 	  			= (double*) new double[glbParam->nBins ] ;
	ip	   			= (double*) new double[glbParam->nBins ] ;
	ipN   			= (double*) new double[glbParam->nBins ] ;
	betaT  			= (double*) new double[glbParam->nBins ] ;
	betaMol_Fit		= (double*) new double[glbParam->nBins ] ;
	intAlphaMol_Ref_r = (double*) new double[glbParam->nBins] ;
	// pr2Fit 		= (double*) new double[glbParam->nBins] ;

	for ( int e=0 ; e <glbParam->nEventsAVG ; e++ )
	{
		pr2[e] 		 = (double**) new double*[glbParam->nCh] ;
		for ( int c=0 ; c <glbParam->nCh ; c++ )
		{
			pr2[e][c] 		= (double*) new double[glbParam->nBins] ;
			for(int b =0 ; b <glbParam->nBins ; b++)
				pr2      [e][c][b] = (double)0 ;
		}
	}

	data_File_L2 = (double***) new double**[glbParam->nEventsAVG] ;
	for ( int e=0 ; e <glbParam->nEventsAVG ; e++ )
	{
		data_File_L2[e] = (double**) new double*[glbParam->nCh] ;
		for ( int c=0 ; c <glbParam->nCh ; c++ )
		{
			data_File_L2[e][c] = (double*) new double[glbParam->nBins] ;
				for(int b =0 ; b <glbParam->nBins ; b++)
					data_File_L2[e][c][b] = (double)0.0 ;
		}
	}
	Start_Time_AVG_L2 = (int*) new int [glbParam->nEventsAVG] ;   memset( (int*)Start_Time_AVG_L2, 0, (sizeof(int)*glbParam->nEventsAVG) ) ;
	Stop_Time_AVG_L2  = (int*) new int [glbParam->nEventsAVG] ;   memset( (int*)Stop_Time_AVG_L2 , 0, (sizeof(int)*glbParam->nEventsAVG) ) ;

	nLRs = ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "LR ", "double", (double*)LR ) ;

	if ( nLRs ==-2000 )
	{
		LR_loop_Max = 2 ; // SEARCH FOR THE BEST LR TO MATCH THE AERONET DATA.
		nLRs = 36 ; // (36-1)*5 = 175--> MAX LR
		for (int i =0; i <nLRs; i++)
			LR[i] = (i+1)*5 ;
	}
	else
		LR_loop_Max = 1 ;

	for ( int e=0 ; e <glbParam->nEventsAVG ; e++ )
	{
		pr[e]		 = (double*) new double[glbParam->nBins] ;
		alpha_Aer[e] = (double**) new double*[nLRs] ;
		beta_Aer [e] = (double**) new double*[nLRs] ;
		AOD_LR   [e] = (double*)  new double [nLRs] ;
		alpha_Aer_synergy[e] = (double*) new double[glbParam->nBins] ;
		beta_Aer_synergy [e] = (double*) new double[glbParam->nBins] ;
		for (int i =0; i <glbParam->nBins; i++)
		{
			alpha_Aer_synergy[e][i] = (double) 0.0 ;
			beta_Aer_synergy [e][i] = (double) 0.0 ;
		}
		for ( int l=0 ; l <nLRs ; l++ )
		{
			alpha_Aer[e][l] = (double*) new double[glbParam->nBins] ;
			beta_Aer [e][l] = (double*) new double[glbParam->nBins] ;

			for(int b =0 ; b <glbParam->nBins ; b++)
			{
				alpha_Aer[e][l][b] = (double)0 ;
				beta_Aer [e][l][b] = (double)0 ;
			}
		}
	}
	indxRef_Fernald 	  = (int*) new int [ glbParam->nEventsAVG ] ;
	indxRef_Fernald_Start = (int*) new int [ glbParam->nEventsAVG ] ;
	indxRef_Fernald_Stop  = (int*) new int [ glbParam->nEventsAVG ] ;

	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "R_ref", "double" , (double*)&R_ref ) ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "avg_Points_Fernald", "int", (int*)glbParam->avg_Points_Fernald ) ;

// 	AERONET VARIABLES
	aeronet_file = (char*) new char [ 400 ] ;
    ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"AERONET_FILE", (const char*)"string", (char*)aeronet_file ) ;
    // if ( strcmp(aeronet_file, "NOT_FOUND") ==0 )
	// 	printf("\nNo AERONET data set in the configuration file. ") ;
	// else
	// 	printf("\nAERONET data file to use: %s\n", aeronet_file) ;

	aeronet_path = (char*) new char [ 300 ] ;
    ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"AERONET_PATH", (const char*)"string", (char*)aeronet_path ) ;

	aeronet_data_level = (char*) new char [ 10 ] ;
    ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"AERONET_DATA_LEVEL", (const char*)"string", (char*)aeronet_data_level ) ;

	aeronet_site_name = (char*) new char [ 30 ] ;
    ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, (const char*)"AERONET_SITE_NAME", (const char*)"string", (char*)aeronet_site_name ) ;


	oLOp = (CLidar_Operations*) new CLidar_Operations( (strcGlobalParameters*)glbParam ) ;
}
	
CDataLevel_2::~CDataLevel_2()
{

}

void CDataLevel_2::FernaldInversion( strcGlobalParameters *glbParam, strcMolecularData *dataMol)
{
	LRM = (double) dataMol->LR_mol ;

	for (int i =1; i <glbParam->nBins; i++)
	{
		dataMol->alphaMol_avg[i] = dataMol->alphaMol[i] ;
		dataMol->betaMol_avg[i]  = dataMol->betaMol [i] ;
		dataMol->pr2Mol_avg [i]  = dataMol->pr2Mol  [i] ;
		glbParam->r_avg[i] 		 = glbParam->r[i]		;
	}

	if ( (heightRef_Inversion_Start_ASL >0) && (heightRef_Inversion_Stop_ASL >0) && (heightRef_Inversion_Stop_ASL > heightRef_Inversion_Start_ASL) )
	{
		heightRef_Inversion_ASL = ( heightRef_Inversion_Stop_ASL + heightRef_Inversion_Start_ASL )/2 ;
		indxRef_Fernald_Start[glbParam->evSel] = (int)round( ( heightRef_Inversion_Start_ASL - glbParam->siteASL)/dzr ) ;
		indxRef_Fernald_Stop [glbParam->evSel] = (int)round( ( heightRef_Inversion_Stop_ASL  - glbParam->siteASL)/dzr ) ;
		heightRef_Inversion_ASL = (double) (glbParam->siteASL + glbParam->dzr*( indxRef_Fernald_Start[glbParam->evSel] + indxRef_Fernald_Stop[glbParam->evSel] )/2) ;
	}
	else
		Find_Ref_Range( (strcGlobalParameters*)glbParam, (strcMolecularData*)dataMol ) ;

	if ( strcmp( reference_method.c_str(), "MEAN" ) ==0 )
	{
			sum( (double*)&pr2[glbParam->evSel][glbParam->chSel][0], (int)indxRef_Fernald_Start[glbParam->evSel], (int)indxRef_Fernald_Stop[glbParam->evSel], (double*)&pr2_Ref ) ;
			pr2_Ref = pr2_Ref /(indxRef_Fernald_Stop[glbParam->evSel] - indxRef_Fernald_Start[glbParam->evSel] +1) ;
			indxRef_Fernald[glbParam->evSel] = (indxRef_Fernald_Start[glbParam->evSel] + indxRef_Fernald_Stop[glbParam->evSel])/2 ;
	}
	else if ( strcmp( reference_method.c_str(), "FIT" ) ==0 )
	{
		memset( (double*)dummy, 0, sizeof(double)*glbParam->nBins ) ;
		strcFitParam fitParam ;
		fitParam.indxInitFit = indxRef_Fernald_Start[glbParam->evSel] ;
		fitParam.indxEndFit  = indxRef_Fernald_Stop [glbParam->evSel] ;
		fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1;
		// do
		// {
			oLOp->Fit( (double*)&(pr2[glbParam->evSel][glbParam->chSel][0]), (double*)&(dataMol->pr2Mol_avg[0]), fitParam.nFit , "wB", "NOTall", (strcFitParam*)&fitParam, (double*)&dummy[0] ) ;

			double *absDiff = (double*) new double[ fitParam.nFit ] ;
			for (int i =0 ; i <fitParam.nFit ; i++)
				absDiff[i] = fabs( pr2[glbParam->evSel][glbParam->chSel][fitParam.indxInitFit +i] - dummy[fitParam.indxInitFit +i] ) ;

			findIndxMin( (double*)absDiff, (int)0, (int)(fitParam.nFit -1), (int*)&indxMin_absDiff, (double*)&minDiff ) ;
			indxRef_Fernald[glbParam->evSel] = fitParam.indxInitFit + indxMin_absDiff ;
			// indxRef_Fernald[glbParam->evSel] = round( (indxRef_Fernald_Start[glbParam->evSel] + indxRef_Fernald_Stop[glbParam->evSel])/2 ) ;
			pr2_Ref = dummy[indxRef_Fernald[glbParam->evSel]] ;
			delete absDiff ;
		// 	if ( pr2_Ref <=0 )
		// 	{
		// 		fitParam.indxInitFit++ ;
		// 		fitParam.indxEndFit-- ;
		// 		fitParam.nFit = fitParam.indxEndFit - fitParam.indxInitFit +1;
		// 	}
		// } while ( (pr2_Ref <=0) && (fitParam.nFit >10) ) ;
	}
		pr2[glbParam->evSel][glbParam->chSel][indxRef_Fernald[glbParam->evSel]] = pr2_Ref ;

	printf("\n FernaldInversion()--> \nMax. Range= %lf ---- Ref. ranges: %lf - %lf m - Inversion range value: %f m \t Pr2(ref)= %lf", 
				glbParam->dr * glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel], glbParam->dzr*indxRef_Fernald_Start[glbParam->evSel], 
				glbParam->dzr*indxRef_Fernald_Stop[glbParam->evSel], indxRef_Fernald[glbParam->evSel] *glbParam->dzr, pr2_Ref ) ;

	int indx_integral_max_range_for_AOD ;
	// int integral_max_range_for_AOD ;

// ********************** FERNALD INVERSION **********************
	for ( int lrLoop=0 ; lrLoop <LR_loop_Max ; lrLoop++ )
	{
		for ( int l=0 ; l <nLRs ; l++ ) // LOOP ACROSS LRs
		{
			FernaldInversion_Core( (strcGlobalParameters*)glbParam, (int)l, (strcMolecularData*)dataMol, (int)glbParam->indxInitSig, (int)glbParam->nBins ) ;

			// ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "integral_max_range_for_AOD", "int", (int*)&integral_max_range_for_AOD ) ;
			// indx_integral_max_range_for_AOD = (int)round( integral_max_range_for_AOD /glbParam->dr ) ;
			indx_integral_max_range_for_AOD = (int)round((indxRef_Fernald_Start[glbParam->evSel] + indxRef_Fernald_Stop[glbParam->evSel]) /2) ;

			smooth( (double*)&beta_Aer [glbParam->evSel][l][0], (int)0, (int)(glbParam->nBins-1), (int)glbParam->avg_Points_Fernald[glbParam->chSel], (double*)&beta_Aer [glbParam->evSel][l][0] ) ;
			smooth( (double*)&alpha_Aer[glbParam->evSel][l][0], (int)0, (int)(glbParam->nBins-1), (int)glbParam->avg_Points_Fernald[glbParam->chSel], (double*)&alpha_Aer[glbParam->evSel][l][0] ) ;
			sum(    (double*)&alpha_Aer[glbParam->evSel][l][0], (int)0, (int)indx_integral_max_range_for_AOD, (double*)&AOD_LR[glbParam->evSel][l] ) ;
			AOD_LR[glbParam->evSel][l] = AOD_LR[glbParam->evSel][l] * glbParam->dr ;
			// printf("\nAOD@LR = %lf --> %lf", fabs(LR[l]), AOD_LR[glbParam->evSel][l]) ;
		} // for ( int l=0 ; l <nLRs ; l++ ) // LOOP ACROSS LRs

		if ( (strcmp( aeronet_file, "NOT_FOUND") !=0) )
		{	// IF THERE IS AERONET DATA
			if ( AOD_Lidar_Time[glbParam->evSel] >=0 )
			{
				if ( lrLoop ==0 )	printf("\nThere is AERONET DATA TO COMPARE --> Searching the best LR...\n") ;

					double 	*diff_AOD = (double*) new double[nLRs] ;
					int		indxMin_diff_AOD 	;
					double	min_diff_AOD 		; 
					for ( int l=0 ; l <nLRs ; l++ )
						diff_AOD[l] = fabs( AOD_Lidar_Time[glbParam->evSel] - AOD_LR[glbParam->evSel][l] ) ;

					findIndxMin( (double*)diff_AOD, (int)0, (int)(nLRs-1), (int*)&indxMin_diff_AOD, (double*)&min_diff_AOD ) ;
					LR_inv[glbParam->evSel] = (double) LR[indxMin_diff_AOD] ;
					// printf("Closest lidar's AOD to AERONET's AOD ==> AOD LIDAR(best LR: %lf) =%lf \t AOD AERONET =%lf  ", LR_inv[glbParam->evSel], AOD_LR[glbParam->evSel][indxMin_diff_AOD], AOD_Lidar_Time[glbParam->evSel] ) ;
					AOD_inv[glbParam->evSel] = (double)AOD_LR[glbParam->evSel][indxMin_diff_AOD] ;
					for (int i =0; i <glbParam->nBins_Ch[glbParam->chSel]; i++)
					{
						alpha_Aer_synergy[glbParam->evSel][i] = (double) alpha_Aer[glbParam->evSel][indxMin_diff_AOD][i] ;
						beta_Aer_synergy[glbParam->evSel][i]  = (double) beta_Aer [glbParam->evSel][indxMin_diff_AOD][i] ;
					}
					if ( LR_loop_Max ==2 )
					{
						if ( lrLoop ==0 )
						{   // SET LRs IN HIGH RESOLUTION FOR THE SECOND LOOP
							// printf("\nSecond round of LRs:") ;
							int	   dLR		= 5 ;
							double lr_min 	= LR_inv[glbParam->evSel] -dLR ;
							nLRs = 2*dLR +1 ;
							for ( int l =0; l <nLRs; l++ )
								LR[l] = (double) lr_min + l ;
						}
						else if ( lrLoop ==1 )
						{
							printf("Closest lidar's AOD to AERONET's AOD ==> AOD LIDAR =%lf using LR: %lf \t AOD AERONET =%lf  ", AOD_inv[glbParam->evSel], LR_inv[glbParam->evSel], AOD_Lidar_Time[glbParam->evSel] ) ;
							// SET LR AND nLRs IN LOW RESOLUTION FOR THE NEXT LIDAR SIGNAL
							nLRs = ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "LR ", "double", (double*)LR ) ;
							if ( nLRs ==-2000 )
							{
								nLRs = 36 ; // (36-1)*5 = 175--> MAX LR
								for (int i =0; i <nLRs; i++)
									LR[i] = (i+1)*5 ;
							}
						}
					} // if ( LR_loop_Max ==2 )
					else
						printf("Closest lidar's AOD to AERONET's AOD ==> AOD LIDAR =%lf using LR: %lf \t AOD AERONET =%lf  ", AOD_inv[glbParam->evSel], LR_inv[glbParam->evSel], AOD_Lidar_Time[glbParam->evSel] ) ;
			} // if ( AOD_Lidar_Time[glbParam->evSel] >=0 )
			else
			{
				printf("\nThere is *NOT* AERONET data to compare at the lidar time. Lidar profiles inverted with fixed LRs.\n") ;	
				break ; // BREAK THE for ( int lrLoop=0 ; lrLoop <LR_loop_Max ; lrLoop++ )
			}
		} // if ( strcmp( aeronet_file, "NOT_FOUND") !=0 )
		else
		{	// THERE'S NO AERONET FILE --> QUIT THE LRs LOOP -> for ( int lrLoop=0 ; lrLoop<2 ; lrLoop++ )
			printf("\nThere is *NOT* AERONET file to compare with lidar data. Lidar profiles inverted with fixed LRs.\n") ;	
			break ; // BREAK THE for ( int lrLoop=0 ; lrLoop <LR_loop_Max ; lrLoop++ )
		}
	} // for ( int lrLoop=0 ; lrLoop <LR_loop_Max ; lrLoop++ )
} // void CDataLevel_2::FernaldInversion( strcGlobalParameters *glbParam, strcMolecularData *dataMol)

// USED FOR MONTE CARLO ERROR ESTIMATION
void CDataLevel_2::FernaldInversion( double *pr2_, strcMolecularData *dataMol, strcGlobalParameters *glbParam )
{
	LRM = (double) dataMol->LR_mol ;
/*    COPIAR DE VUELTA ESTE METODO (ESTA ARRIBA)
	if ( glbParam->avg_Points_Fernald[c] >1 )
	{
		smooth( (double*)&dataMol->alphaMol[0], (int)0, (int)(glbParam->nBins-1), (int)glbParam->avg_Points_Fernald[c], (double*)&dataMol->alphaMol_avg[0] ) ;
		smooth( (double*)&dataMol->betaMol [0], (int)0, (int)(glbParam->nBins-1), (int)glbParam->avg_Points_Fernald[c], (double*)&dataMol->betaMol_avg [0] ) ;
		smooth( (double*)&dataMol->pr2Mol  [0], (int)0, (int)(glbParam->nBins-1), (int)glbParam->avg_Points_Fernald[c], (double*)&dataMol->pr2Mol_avg  [0] ) ;

		sum(glbParam->r, 0, glbParam->avg_Points_Fernald[c]-1, &glbParam->r_avg[0] ) ;
		glbParam->r_avg[0] = glbParam->r_avg[0] /glbParam->avg_Points_Fernald[c] ;
		for (int i =1; i <glbParam->nBins; i++)
			glbParam->r_avg[i] = glbParam->r_avg[0] + i*glbParam->dr ;
	}
	else
	{
		for (int i =1; i <glbParam->nBins; i++)
		{
			dataMol->alphaMol_avg[i] = dataMol->alphaMol[i] ;
			dataMol->betaMol_avg[i]  = dataMol->betaMol [i] ;
			dataMol->pr2Mol_avg [i]  = dataMol->pr2Mol  [i] ;
			glbParam->r_avg[i] 		 = glbParam->r[i]		;
		}
	}
	
	if ( (heightRef_Inversion_Start_ASL >0) && (heightRef_Inversion_Stop_ASL >0) && (heightRef_Inversion_Stop_ASL > heightRef_Inversion_Start_ASL) )
	{
		heightRef_Inversion_ASL = ( heightRef_Inversion_Stop_ASL + heightRef_Inversion_Start_ASL )/2 ;
		indxRef_Fernald[glbParam->evSel] 	   = (int)round( ( heightRef_Inversion_ASL - glbParam->siteASL)/dzr 	  ) ;
		indxRef_Fernald_Start[glbParam->evSel] = (int)round( ( heightRef_Inversion_Start_ASL - glbParam->siteASL)/dzr ) ;
		indxRef_Fernald_Stop [glbParam->evSel] = (int)round( ( heightRef_Inversion_Stop_ASL  - glbParam->siteASL)/dzr ) ;
	}
	else
		Find_Ref_Range( (strcGlobalParameters*)glbParam, (int)t ) ;

printf("\tRef. ranges: %lf - %lf", glbParam->dzr*indxRef_Fernald_Start[glbParam->evSel], glbParam->dzr*indxRef_Fernald_Stop[glbParam->evSel]) ;

	if ( strcmp( reference_method.c_str(), "MEAN" ) ==0 )
	{
			sum( (double*)&pr2[t][c][0], (int)indxRef_Fernald_Start[glbParam->evSel], (int)indxRef_Fernald_Stop[glbParam->evSel], (double*)&pr2_Ref ) ;
			pr2_Ref = pr2_Ref /(indxRef_Fernald_Stop[glbParam->evSel] - indxRef_Fernald_Start[glbParam->evSel] +1) ;
	}
	else if ( strcmp( reference_method.c_str(), "FIT" ) ==0 )
	{
		memset( (double*)dummy, 0, sizeof(double)*glbParam->nBins ) ;
		strcFitParam fitParam ;
		fitParam.indxInitFit = indxRef_Fernald_Start[glbParam->evSel] ;
		fitParam.indxEndFit  = indxRef_Fernald_Stop [glbParam->evSel] ;
		fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1;
			oLOp->Fit( (double*)&pr2[t][c][0], (double*)dataMol->pr2Mol_avg, glbParam->nBins , "wOutB", "NOTall", (strcFitParam*)&fitParam, (double*)dummy ) ;
		double *absDiff = (double*) new double[ fitParam.nFit ] ;
		for (int i =0 ; i <fitParam.nFit ; i++)
			absDiff[i] = fabs( pr2[t][c][fitParam.indxInitFit +i] - dummy[fitParam.indxInitFit +i] ) ;
		
		findIndxMin( (double*)absDiff, (int)0, (int)(fitParam.nFit -1), (int*)&indxMin_absDiff, (double*)&minDiff ) ;
		indxRef_Fernald[glbParam->evSel] = fitParam.indxInitFit + indxMin_absDiff ;
		pr2_Ref = dummy[indxRef_Fernald[glbParam->evSel]] ;

		delete absDiff ;
	}
		pr2[t][c][indxRef_Fernald[glbParam->evSel]] = pr2_Ref ;

	int indx_integral_max_range_for_AOD ;
	int integral_max_range_for_AOD ;
// ********************** FERNALD INVERSION **********************
	for ( int l=0 ; l <nLRs ; l++ ) // LOOP ACROSS LRs
	{
		// ********************** FERNALD INVERSION: TEST REFERENCE VALUE **********************
		// FernaldInversion_Test_Ref_Value( (strcGlobalParameters*)glbParam, (int)t, (int)c, (int)l, (strcMolecularData*)dataMol, (double)fabs(LR[l]),
		// 	(int)indxRef_Fernald_Start[glbParam->evSel], (int)indxRef_Fernald_Stop[glbParam->evSel] ) ;

		FernaldInversion_Core( (strcGlobalParameters*)glbParam, (int)t, (int)c, (int)l, (strcMolecularData*)dataMol, (double)LR[l], (int)0, (int)glbParam->nBins ) ;

		ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "integral_max_range_for_AOD", "int", (int*)&integral_max_range_for_AOD ) ;
		indx_integral_max_range_for_AOD = (int)round( integral_max_range_for_AOD /glbParam->dr ) ;

		smooth( (double*)&beta_Aer[t][l][0] , (int)0, (int)(glbParam->nBins-1), (int)glbParam->avg_Points_Fernald[c], (double*)&beta_Aer[t][l][0]  ) ;
		smooth( (double*)&alpha_Aer[t][l][0], (int)0, (int)(glbParam->nBins-1), (int)glbParam->avg_Points_Fernald[c], (double*)&alpha_Aer[t][l][0] ) ;
		sum(    (double*)&alpha_Aer[t][l][0], (int)0, (int)indx_integral_max_range_for_AOD, (double*)&AOD_LR[t][l] ) ;
		AOD_LR[t][l] = AOD_LR[t][l] * glbParam->dr ;
		printf("\nAOD@LR = %lf --> %lf", fabs(LR[l]), AOD_LR[t][l]) ;

	} // for ( int l=0 ; l <nLRs ; l++ ) // LOOP ACROSS LRs
*/
}

void CDataLevel_2::FernaldInversion_Core( strcGlobalParameters *glbParam, int l, strcMolecularData *dataMol, int indxStart, int indxStop )
{
	memset( (double*)phi		  , 0, sizeof(double)*glbParam->nBins ) ;
	memset( (double*)p  		  , 0, sizeof(double)*glbParam->nBins ) ;
	memset( (double*)ip 		  , 0, sizeof(double)*glbParam->nBins ) ;
	memset( (double*)ipN		  , 0, sizeof(double)*glbParam->nBins ) ;
	memset( (double*)betaT		  , 0, sizeof(double)*glbParam->nBins ) ;
	memset( (double*)intAlphaMol_Ref_r, 0, sizeof(double)*glbParam->nBins ) ;
	memset( (double*)alpha_Aer[glbParam->evSel][l], 0, sizeof(double)*glbParam->nBins ) ;
	memset( (double*)beta_Aer [glbParam->evSel][l], 0, sizeof(double)*glbParam->nBins ) ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "R_ref", "double" , (double*)&R_ref ) ;

	ka  = 1/fabs(LR[l]) ;
	KM_ = 1/dataMol->LR_mol ;

	cumtrapz_norm( (double)glbParam->dr, (double*)dataMol->alphaMol_avg, (int)indxStart, (int)indxRef_Fernald[glbParam->evSel], (int)indxStop, (double*)intAlphaMol_Ref_r ) ; // INTEGRALS ARE THRU SLANT PATH -> dr

	for( int i=indxStart ; i <indxStop ; i++  )
	{
		pr2n[i] = pr2[glbParam->evSel][glbParam->chSel][i] /pr2_Ref ;
		phi[i]	= 2*((KM_-ka)/ka) * intAlphaMol_Ref_r[i]  ;
		p[i]   	= pr2n[i] * exp(-phi[i]) ;
	}

	cumtrapz_norm( (double)glbParam->dr, (double*)p, (int)indxStart, (int)indxRef_Fernald[glbParam->evSel], (int)indxStop, (double*)ipN ) ;

	for ( int i=indxStart ; i <indxStop ; i++ )
	{
		betaT[i] = p[i] / ( (1/(R_ref * dataMol->betaMol_avg[indxRef_Fernald[glbParam->evSel]])) - 2 * ipN[i]/ka ) ;
		if ( fpclassify(betaT[i]) == FP_NAN )
			betaT[i] = (double)DBL_MAX ;
	}

		// // FIT beta_mol TO betaT FOR A MORE CONSISTENT RETRIEVAL OF beta_Aer
		// 	// double sumOut =0;
		// 	// int    s =0 ;
		// 	strcFitParam fitParam ;
		// 	fitParam.indxEndFit  = indxRef_Fernald_Stop [glbParam->evSel] ;
		// 	// do
		// 	// {
		// 		fitParam.indxInitFit = indxRef_Fernald_Start[glbParam->evSel]    ;
		// 		// fitParam.indxInitFit = indxRef_Fernald_Start[glbParam->evSel] -s*100 ;
		// 		// fitParam.indxEndFit  = indxRef_Fernald_Stop [glbParam->evSel] -s*100 ;
		// 		fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1 ;
		// 		oLOp->Fit( (double*)&betaT[0], (double*)&dataMol->betaMol_avg[0], glbParam->nBins , "wOutB", "all", (strcFitParam*)&fitParam, (double*)betaMol_Fit ) ;
		// 		// LR = (double)fabs(LR) ;

		// 		// sumOut =0;
		// 		// for (int i =(fitParam.indxInitFit - round(fitParam.indxInitFit *0.10)) ; i <fitParam.indxInitFit ; i++)
		// 			// sumOut = sumOut + betaT[i] - betaMol_Fit[i] ;
		// 		// s++ ;
		// 	// } while( (sumOut <0) && (fitParam.indxInitFit >glbParam->indxInitSig) ) ;

	for ( int i=indxStart ; i <indxStop ; i++ )
	{
		beta_Aer [glbParam->evSel][l][i] = betaT[i] - dataMol->betaMol_avg[i] ; // betaMol_Fit[i] ; // 
		alpha_Aer[glbParam->evSel][l][i] = beta_Aer[glbParam->evSel][l][i] *LR[l] ; // r
	}
	for (int i =0 ; i <=indxStart; i++)
	{
		alpha_Aer[glbParam->evSel][l][i] = alpha_Aer[glbParam->evSel][l][indxStart+1] ;
		beta_Aer [glbParam->evSel][l][i] = beta_Aer [glbParam->evSel][l][indxStart+1] ;
	}
}

void CDataLevel_2::Find_Ref_Range( strcGlobalParameters *glbParam, strcMolecularData *dataMol )
{
	// CHECK IF THERE ARE CLOUDS IN THE PROFILE ANALIZED
	for ( int i= glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] ; i >=0 ; i-- )
	{
		if ( layer_mask[glbParam->evSel][i] ==1 )
		{
			indx_Top_Cloud = (int)i ;
			break ;
		}
	}
// printf("\n\nFind_Ref_Range(): indx_Top_Cloud: %d \tMax Range Detected: %d\n", indx_Top_Cloud, glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] ) ;

	// double	sum_diff = 0.0 ;
	// double  stdRef 	 = 0.0 ;
	int 	nWinSize ; // SIZE OF THE SCANNING WINDOWS
	int 	nWin = (glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] - indx_Top_Cloud) *glbParam->dr /3750 ; // NUMBER OF WINDOWS IN THE LAST MOLECULAR RANGE
	if ( nWin <1 )
		nWinSize = glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] - indx_Top_Cloud +1 ;
	else if ( nWin <=2 )
		nWinSize = 500 ;
	else if( nWin <=4 )
		nWinSize= 1000 ;
	else if( nWin <=8 )
		nWinSize= 2000 ;
	else
		nWinSize= 2000 ;

	int nSteps = glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] - indx_Top_Cloud +1 - nWinSize ; // +1;
	// printf("\nFind_Ref_Range(): ||| indx_Top_Cloud: %d ||| glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel]: %d  ||| nWinSize: %d ||| nSteps: %d\n", indx_Top_Cloud, glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel], nWinSize, nSteps ) ;
 
	strcFitParam	fitParam 											;
	double		 	*stdPr 			= (double*) new double	[nSteps] 	; // for (int i =0; i <nSteps; i++) stdPr[i] = (double)DBL_MAX ;
	int		 		*indxRef_Start 	= (int*) 	new int		[nSteps] 	;
	int		 		*indxRef_Stop  	= (int*) 	new int		[nSteps] 	;
	double 			*prFit = (double*) new double[glbParam->nBins] 		;
	fitParam.indxInitFit = indx_Top_Cloud 									;
	fitParam.indxEndFit  = fitParam.indxInitFit + nWinSize					;
	fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1	;

	int indxMin_Std, minStd ;
	// do
	// {
		for( int i=0 ; i <nSteps ; i++ )
		{
			oLOp->Fit( (double*)&pr[glbParam->evSel][0], (double*)&dataMol->prMol[0], fitParam.nFit, "wOutB", "NOTall", (strcFitParam*)&fitParam, (double*)prFit ) ;
			stdPr[i] 		 = fitParam.std			;
			indxRef_Start[i] = fitParam.indxInitFit ;
			indxRef_Stop [i] = fitParam.indxEndFit  ;
			fitParam.indxInitFit = fitParam.indxInitFit +1							;
			fitParam.indxEndFit  = fitParam.indxInitFit + nWinSize 					;
			fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1	;
		}
		// stdRef = stdPr[nSteps-1] ;

		findIndxMin( (double*)stdPr, (int)0, (int)(nSteps -1), (int*)(&indxMin_Std), (double*)(&minStd) ) ;

			indxRef_Fernald_Start[glbParam->evSel] = indxRef_Start[indxMin_Std] ;
			indxRef_Fernald_Stop [glbParam->evSel] = indxRef_Stop [indxMin_Std] ;
			heightRef_Inversion_ASL = (double) (glbParam->siteASL + glbParam->dzr*( indxRef_Fernald_Start[glbParam->evSel] + indxRef_Fernald_Stop[glbParam->evSel] )/2) ;

		// CHECK IF THE REFERENCE RANGES SELECTED ARE OK FOR THE REST OF THE LIDAR SIGNAL
			// CHECK IF THE FITTED SIGNAL IS *HIGHER* THAN THE LIDAR SIGNAL
			// fitParam.indxInitFit = indxRef_Fernald_Start[glbParam->evSel] 			;
			// fitParam.indxEndFit  = indxRef_Fernald_Stop [glbParam->evSel] 			;
			// fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1	;
			// oLOp->Fit( (double*)&pr[glbParam->evSel][0], (double*)&dataMol->prMol[0], fitParam.nFit, "wOutB", "all", (strcFitParam*)&fitParam, (double*)prFit ) ;
			// for (int j =0; j <glbParam->nBins_Ch[glbParam->evSel]; j++)
			// 	dummy[j] = pr[glbParam->evSel][j] - prFit[j] ;
			// sum( (double*)&dummy[0], (int)indx_Top_Cloud, (int)indxRef_Fernald_Start[glbParam->evSel], (double*)&sum_diff ) ;

			// CHECK IF THE FITTED SIGNAL IS *LOWER* THAN THE LIDAR SIGNAL
			// fitParam.indxInitFit = indx_Top_Cloud 									;
			// fitParam.indxEndFit  = indxRef_Fernald_Start[glbParam->evSel] 			;
			// fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1	;
			// oLOp->Fit( (double*)&pr[glbParam->evSel][0], (double*)&dataMol->prMol[0], fitParam.nFit, "wOutB", "NOTall", (strcFitParam*)&fitParam, (double*)prFit ) ;

		// SET THE PARAMETERS FOR THE NEXT SEARCH
		// fitParam.indxInitFit = indx_Top_Cloud																		;
		// fitParam.indxEndFit  = (indxRef_Fernald_Start[glbParam->evSel] + indxRef_Fernald_Stop[glbParam->evSel]) /2 	;
		// fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1										;
	// } while ( ((sum_diff <0) ) && ( fitParam.indxEndFit >round(5000/glbParam->dr) ) ) ; // || (fitParam.std >(3*stdRef))

// printf("\n\nFind_Ref_Range(): parametros del fiteo minimo ==> indxRef_Start[%d]: %d \t indxRef_Stop[%d]: %d\n", indxMin_Std, indxRef_Start[indxMin_Std], indxMin_Std, indxRef_Stop[indxMin_Std] ) ;
// printf("Index Min stdPr[%d]= %2.3e - Num of elements of stdPr= %d\n", indxMin_Std, stdPr[indxMin_Std], nSteps ) ;

	delete  prFit ;
// printf("\nFind_Ref_Range(): ||| indxRef_Fernald_Start[%d]: %d  ||| indxRef_Fernald_Stop[%d]: %d \n",  glbParam->evSel, indxRef_Fernald_Start[glbParam->evSel], 
// 																									  glbParam->evSel, indxRef_Fernald_Stop [glbParam->evSel] ) ;
}

/* BACKUP
void CDataLevel_2::Find_Ref_Range( strcGlobalParameters *glbParam, strcMolecularData *dataMol )
{
	int indx_Top_Cloud = 0 ;

	// CHECK IF THERE ARE CLOUDS IN THE PROFILE ANALIZED
	for ( int i= glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] ; i >=0 ; i-- )
	{
		if ( layer_mask[glbParam->evSel][i] ==1 )
		{
			indx_Top_Cloud = (int)i ;
			break ;
		}
	}
printf("\n\nFind_Ref_Range(): indx_Top_Cloud: %d \tMax Range Detected: %d\n", indx_Top_Cloud, glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] ) ;

	int 	nWinSize ; // SIZE OF THE SCANNING WINDOWS
	int 	nWin = (glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] - indx_Top_Cloud) *glbParam->dr /3750 ; // NUMBER OF WINDOWS IN THE LAST MOLECULAR RANGE
	if ( nWin <1 )
		nWinSize = glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] - indx_Top_Cloud +1 ;
	else if ( nWin <=2 )
		nWinSize = 500 ;
	else if( nWin <=4 )
		nWinSize= 1000 ;
	else if( nWin <=8 )
		nWinSize= 2000 ;
	else
		nWinSize= 2000 ;

	int nSteps = glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] - indx_Top_Cloud +1 - nWinSize ; // +1;
	// printf("\nFind_Ref_Range(): ||| indx_Top_Cloud: %d ||| glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel]: %d  ||| nWinSize: %d ||| nSteps: %d\n", indx_Top_Cloud, glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel], nWinSize, nSteps ) ;
 
	strcFitParam	fitParam 											;
	double		 	*stdPr 			= (double*) new double	[nSteps] 	; // for (int i =0; i <nSteps; i++) stdPr[i] = (double)DBL_MAX ;
	int		 		*indxRef_Start 	= (int*) 	new int		[nSteps] 	;
	int		 		*indxRef_Stop  	= (int*) 	new int		[nSteps] 	;
	double 			*prFit = (double*) new double[glbParam->nBins] 		;
	fitParam.indxInitFit = indx_Top_Cloud 									;
	fitParam.indxEndFit  = fitParam.indxInitFit + nWinSize					;
	fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1	;
	// int i=0 ;
	// do
	for( int i=0 ; i <nSteps ; i++ )
	{
		oLOp->Fit( (double*)&pr[glbParam->evSel][0], (double*)&dataMol->prMol[0], fitParam.nFit, "wOutB", "NOTall", (strcFitParam*)&fitParam, (double*)prFit ) ;
		stdPr[i] 		 = fitParam.std			;
		indxRef_Start[i] = fitParam.indxInitFit ;
		indxRef_Stop [i] = fitParam.indxEndFit  ;
		fitParam.indxInitFit = fitParam.indxInitFit +1							;
		fitParam.indxEndFit  = fitParam.indxInitFit + nWinSize 					;
		fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1	;
		// i++ ;
	} // while( fitParam.indxEndFit <= glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] ) ;
	delete  prFit ;

	int indxMin_Std, minStd ;
	findIndxMin( (double*)stdPr, (int)0, (int)(nSteps -1), (int*)(&indxMin_Std), (double*)(&minStd) ) ;

	indxRef_Fernald_Start[glbParam->evSel] = indxRef_Start[indxMin_Std] ;
	indxRef_Fernald_Stop [glbParam->evSel] = indxRef_Stop [indxMin_Std] ;

printf("\n\nFind_Ref_Range(): parametros del fiteo minimo ==> indxRef_Start[%d]: %d \t indxRef_Stop[%d]: %d\n", indxMin_Std, indxRef_Start[indxMin_Std], indxMin_Std, indxRef_Stop[indxMin_Std] ) ;
printf("Index Min stdPr[%d]= %2.3e - Num of elements of stdPr= %d\n", indxMin_Std, stdPr[indxMin_Std], nSteps ) ;

	heightRef_Inversion_ASL = (double) (glbParam->siteASL + glbParam->dzr*( indxRef_Fernald_Start[glbParam->evSel] + indxRef_Fernald_Stop[glbParam->evSel] )/2) ;
// printf("\nFind_Ref_Range(): ||| indxRef_Fernald_Start[%d]: %d  ||| indxRef_Fernald_Stop[%d]: %d \n",  glbParam->evSel, indxRef_Fernald_Start[glbParam->evSel], 
// 																									  glbParam->evSel, indxRef_Fernald_Stop [glbParam->evSel] ) ;
}*/

void CDataLevel_2::MonteCarloRandomError( strcGlobalParameters *glbParam, strcMolecularData *dataMol)
{
	strcAerosolData dataAerErr ;
	dataAerErr.alphaAer	= (double*) malloc( glbParam->nBins * sizeof(double) ) ;  memset( dataAerErr.alphaAer, 0, ( glbParam->nEventsAVG * sizeof(double) ) ) ;
	dataAerErr.betaAer	= (double*) malloc( glbParam->nBins * sizeof(double) ) ;  memset( dataAerErr.betaAer , 0, ( glbParam->nEventsAVG * sizeof(double) ) ) ;
	dataAerErr.VAODr	= (double*) malloc( glbParam->nBins * sizeof(double) ) ;  memset( dataAerErr.VAODr   , 0, ( glbParam->nEventsAVG * sizeof(double) ) ) ;

	strcErrorSignalSet errSigSet ;
		GetMemStrcErrorSigSet( (strcErrorSignalSet*)&errSigSet, (int)glbParam->MonteCarlo_N_SigSet_Err, (int)glbParam->nBins ) ;

	strcFitParam	fitParam ;
	fitParam.indxInitFit = dataMol->nBins - glbParam->nBinsBkg  	  ;
	fitParam.indxEndFit  = dataMol->nBins -1 						  ;
	fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1 ;
	double *prFit = (double*) new double[glbParam->nBins] ;
		// oLOp->Fit( (double*)&dataMol->prMol[0], (double*)&pr[0], fitParam.nFit   , "wB", "NOTall", (strcFitParam*)&fitParam, (double*)prFit ) ;
		oLOp->Fit( (double*)&pr[0], (double*)&dataMol->prMol[0], fitParam.nFit   , "wB", "NOTall", (strcFitParam*)&fitParam, (double*)prFit ) ;
	delete  prFit ;	
	double 	stdPr = sqrt( fitParam.squared_sum_fit/(fitParam.nFit -1) ) ;

	int 	spamAvgWin ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "spamAvgWin", "int", (int*)&spamAvgWin ) ;

	// PRODUCE THE LIDAR SIGNAL SET FROM pr[glbParam->evSel][:]


		// SEGUIR ACA!!!!!!!!!!!!!!!!!!!!

	for( int m=0 ; m < glbParam->MonteCarlo_N_SigSet_Err ; m++ )
	{
		smooth( (double*)&pr[glbParam->evSel][0], (int)0, (int)(glbParam->nBins-1), (int)spamAvgWin, (double*)&errSigSet.pr_smooth[m][0] ) ;

		for ( int i=0 ; i < glbParam->nBins ; i++ )
		{
			// errSigSet.prNoisy [m][i] = RandN2( pr[glbParam->evSel][i], stdPr ) ; // ADD ERROR TO pr
			errSigSet.prNoisy [m][i] = RandN2( errSigSet.pr_smooth[m][i], stdPr ) ; // ADD ERROR TO pr
			errSigSet.pr2Noisy[m][i] = errSigSet.prNoisy[m][i] * (glbParam->r[i]*glbParam->r[i]) ; // MAKE pr2 NOISY
		}
		// smooth( (double*)&errSigSet.pr2Noisy[m][0], (int)0, (int)(glbParam->nBins-1), (int)spamAvgWin, (double*)errSigSet.pr2Noisy[m] ) ;
		// FERNALD INVERSION
			FernaldInversion( (double*)&errSigSet.pr2Noisy[m][0], (strcMolecularData*)dataMol, (strcGlobalParameters*)&glbParam ) ;

		for( int b=0 ; b<glbParam->nBins ; b++ )
		{
			errSigSet.alphaAer_ErrSet[m][b] = alpha_Aer[glbParam->evSel][0][b] ; // !! SOLO UTILIZO EL PRIMER LR !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			// errSigSet.VAODr_ErrSet[m][b]    = VAODr   [b] ;
		}
	} // for( int m=0 ; m < MonteCarlo_N_SigSet_Err ; m++ )
	// OBTAIN MEAN AND STANDAR DEVIATION
		GetErrSetParam( (char*)glbParam->FILE_PARAMETERS, (int)glbParam->MonteCarlo_N_SigSet_Err, (int)glbParam->nBins, (double)dataMol->dzr, (strcErrorSignalSet*)&errSigSet ) ;
} // void CDataLevel_2::MonteCarloRandomError( strcGlobalParameters *glbParam, int t, int c, strcMolecularData *dataMol)

/*
void CDataLevel_2::MonteCarloSystematicError( double *pr2Glued, strcGlobalParameters *glbParam, strcMolecularData *dataMol, strcIndexMol *indxMol, strcFernaldInversion *fernaldVectors, strcErrorSignalSet *sysErrSigSet )
{
	strcAerosolData dataAerErr ;
	dataAerErr.alphaAer	= (double*) malloc( glbParam->nBins * sizeof(double) ) ;
	dataAerErr.betaAer	= (double*) malloc( glbParam->nBins * sizeof(double) ) ;
	dataAerErr.VAODr	= (double*) malloc( glbParam->nBins * sizeof(double) ) ;

	double 	LR_init, LR_end, LR_step ;
		ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "LR_init", "double" , (double*)&LR_init ) ;
		ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "LR_end" , "double" , (double*)&LR_end  ) ;
	int 	MonteCarlo_N_SigSet_Err ;
		ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "MonteCarlo_N_SigSet_Err", "int" , (int*)&MonteCarlo_N_SigSet_Err ) ;

	double 	*ka_ite = (double*) malloc( MonteCarlo_N_SigSet_Err * sizeof(double) ) ;
	LR_step = (double) 1 ; // ((LR_end - LR_init) /MonteCarlo_N_SigSet_Err) ;
	for ( int j=0 ; j<MonteCarlo_N_SigSet_Err ; j++ )		ka_ite[j] = (double) 1/(LR_init + j*LR_step) ;
//	double ka_ite[10] = { 0.015, 0.018, 0.020, 0.025, 0.03, 0.04, 0.05, 0.06, 0.07, 0.08 } ;

	for( int j=0 ; j<MonteCarlo_N_SigSet_Err ; j++ )
	{
// FERNALD INVERSION
	FernaldInversion( (double*)pr2Glued, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (int)indxMol->indxInicMol[0]+110, (double)ka_ite[j], (strcFernaldInversion*)fernaldVectors , (strcAerosolData*)&dataAerErr ) ;
	//FernaldInversion( (double*)pr2Glued, (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (int)indxMol->indxInicMol[0]+110, (double)ka_ite[j], (strcAerosolData*)&dataAerErr ) ;
		for( int b=0 ; b<glbParam->nBins ; b++ )
		{
			sysErrSigSet->alphaAer_ErrSet[j][b] = dataAerErr.alphaAer[b] ;
			sysErrSigSet->   VAODr_ErrSet[j][b] = dataAerErr.VAODr   [b] ;
		}
	}
// OBTAIN MEAN AND STANDAR DEVIATION
	GetErrSetParam( (char*)glbParam->FILE_PARAMETERS, (int)MonteCarlo_N_SigSet_Err, (int)glbParam->nBins, (double)dataMol->dzr, (strcErrorSignalSet*)sysErrSigSet ) ;

// FREE MEM FROM fernaldVectors
	//FreeMemVectorsFernaldInversion( (strcFernaldInversion*)&fernaldVectors ) ;
}
*/
/*
void CDataLevel_2::FernaldInversion_Test_Ref_Value( strcGlobalParameters *glbParam, int t, int c, int l, strcMolecularData *dataMol, double LR, int indxStart, int indxStop )
{
	int nSamples ;
	ReadAnalisysParameter( (char*)glbParam->FILE_PARAMETERS, "nBiasRes_Auto", "int" , (int*)&nSamples ) ;

	double *coeff     = (double*) new double[3 +1] ;
	double *alpha_sum = (double*) new double [nSamples] ;

	float 	maxFactor = 1.1 ;
	float 	minFactor = 0.9 ;

	double pr2_Ref_Max 		= maxFactor *pr2_Ref ;
	double pr2_Ref_Min	 	= minFactor *pr2_Ref ;
	double pr2_Ref_step		= (pr2_Ref_Max - pr2_Ref_Min) /nSamples ;
	double *pr2_Ref_Samples = (double*) new double[ nSamples ] ;
	for ( int s =0; s <nSamples; s++ )
		pr2_Ref_Samples[s] = (double) pr2_Ref_Min + s *pr2_Ref_step ;

	for( int iLoop=0 ; iLoop<3 ; iLoop++ )
	{
		for( int iSample=0 ; iSample<(nSamples) ; iSample++ )
		{
			pr2_Ref = pr2_Ref_Samples[iSample] ;
			FernaldInversion_Core( (strcGlobalParameters*)glbParam, (int)t, (int)c, (int)l, (strcMolecularData*)dataMol, (double)LR, (int)indxStart, (int)indxStop ) ;
			sum( (double*)&alpha_Aer[t][l][0], (int)indxStart, (int)indxStop, (double*)&alpha_sum[iSample] ) ;
			alpha_sum[iSample] = alpha_sum[iSample] * alpha_sum[iSample] ;
		}
		polyfitCoeff( 	(const double* const  ) pr2_Ref_Samples , // X DATA
						(const double* const  ) alpha_sum	 	, // Y DATA
						(unsigned int         ) nSamples 		,
						(unsigned int		  ) 2			 	,
						(double*			  ) coeff	 ) 	 	;

		pr2_Ref = (double) -coeff[1] /(2*coeff[2]) ;
		pr2_Ref_Max 	= maxFactor *pr2_Ref ;
		pr2_Ref_Min	 	= minFactor *pr2_Ref ;
		pr2_Ref_step	= (pr2_Ref_Max - pr2_Ref_Min) /nSamples ;
		for ( int s =0; s <nSamples; s++ )
			pr2_Ref_Samples[s] = (double) pr2_Ref_Min + s *pr2_Ref_step ;
	}
}
*/

void CDataLevel_2::GetMemStrcErrorSigSet( strcErrorSignalSet *errSigSet, int MonteCarlo_N_SigSet_Err, int nBins )
{
		errSigSet->pr_smooth  	   = (double**) new double*[MonteCarlo_N_SigSet_Err] ;
		errSigSet->prNoisy  	   = (double**) new double*[MonteCarlo_N_SigSet_Err] ;
		errSigSet->pr2Noisy 	   = (double**) new double*[MonteCarlo_N_SigSet_Err] ;
		errSigSet->alphaAer_ErrSet = (double**) new double*[MonteCarlo_N_SigSet_Err] ;
		errSigSet->VAODr_ErrSet    = (double**) new double*[MonteCarlo_N_SigSet_Err] ;

		for( int j=0 ; j < MonteCarlo_N_SigSet_Err ; j++ )
		{
			errSigSet->pr_smooth		  [j] = (double*) new double[nBins] ;
			errSigSet->prNoisy		  [j] = (double*) new double[nBins] ;
			errSigSet->pr2Noisy		  [j] = (double*) new double[nBins] ;
			errSigSet->alphaAer_ErrSet[j] = (double*) new double[nBins] ;
			errSigSet->   VAODr_ErrSet[j] = (double*) new double[nBins] ;
		}

		errSigSet->VAODr_meanErrSet	   = (double*) new double[nBins] ; 	memset( errSigSet->VAODr_meanErrSet   , 0, sizeof(double) * nBins ) ;
		errSigSet-> VAODr_stdErrSet	   = (double*) new double[nBins] ; 	memset( errSigSet->VAODr_stdErrSet    , 0, sizeof(double) * nBins ) ;
		errSigSet->alphaAer_meanErrSet = (double*) new double[nBins] ; 	memset( errSigSet->alphaAer_meanErrSet, 0, sizeof(double) * nBins ) ;
		errSigSet->alphaAer_stdErrSet  = (double*) new double[nBins] ;	memset( errSigSet->alphaAer_stdErrSet , 0, sizeof(double) * nBins ) ;

		memset( errSigSet->hVAOD_meanErrSet, 0, sizeof(double) * 4 ) ;
		memset( errSigSet-> hVAOD_stdErrSet, 0, sizeof(double) * 4 ) ;
}

void CDataLevel_2::GetErrSetParam( char *FILE_PARAMETERS, int MonteCarlo_N_SigSet_Err, int nBins, double dzr, strcErrorSignalSet *errSigSet )
{
	float 	VAODheigh ;
// r_meanErrSet
	memset( errSigSet->alphaAer_meanErrSet, 0, sizeof(double) * nBins ) ;
	memset( errSigSet->VAODr_meanErrSet   , 0, sizeof(double) * nBins ) ;
		for ( int b=0 ; b<nBins ; b++ )
		{
			for ( int j=0 ; j < MonteCarlo_N_SigSet_Err ; j++ )
			{
				errSigSet->alphaAer_meanErrSet[b] = errSigSet->alphaAer_meanErrSet[b] + errSigSet->alphaAer_ErrSet[j][b] ;
				errSigSet->   VAODr_meanErrSet[b] = errSigSet->   VAODr_meanErrSet[b] + errSigSet->   VAODr_ErrSet[j][b] ;
			}
			errSigSet->alphaAer_meanErrSet[b] = errSigSet->alphaAer_meanErrSet[b] / MonteCarlo_N_SigSet_Err ;
			errSigSet->   VAODr_meanErrSet[b] = errSigSet->   VAODr_meanErrSet[b] / MonteCarlo_N_SigSet_Err ;
		}
// VAODmean OF THE DATASET AT DIFFERENT HEIGHTS
	ReadAnalisysParameter( (const char*)FILE_PARAMETERS, "VAOD_HEIGH0", "float", (float*)&VAODheigh ) ;		errSigSet->hVAOD_meanErrSet[0] = errSigSet->VAODr_meanErrSet[(int)round(VAODheigh/dzr)] ;
	ReadAnalisysParameter( (const char*)FILE_PARAMETERS, "VAOD_HEIGH1", "float", (float*)&VAODheigh ) ;		errSigSet->hVAOD_meanErrSet[1] = errSigSet->VAODr_meanErrSet[(int)round(VAODheigh/dzr)] ;
	ReadAnalisysParameter( (const char*)FILE_PARAMETERS, "VAOD_HEIGH2", "float", (float*)&VAODheigh ) ;		errSigSet->hVAOD_meanErrSet[2] = errSigSet->VAODr_meanErrSet[(int)round(VAODheigh/dzr)] ;
	ReadAnalisysParameter( (const char*)FILE_PARAMETERS, "VAOD_HEIGH3", "float", (float*)&VAODheigh ) ;		errSigSet->hVAOD_meanErrSet[3] = errSigSet->VAODr_meanErrSet[(int)round(VAODheigh/dzr)] ;

// r_stdErrSet STANDAR ERROR MEAN OF THE DATASET AT DIFFERENT HEIGHTS
		memset( errSigSet->alphaAer_stdErrSet, 0, sizeof(double) * nBins ) ;
		memset( errSigSet->VAODr_stdErrSet   , 0, sizeof(double) * nBins ) ;
		for ( int b=0 ; b<nBins ; b++ )
		{
			for ( int j=0 ; j < MonteCarlo_N_SigSet_Err ; j++ )
			{
				errSigSet->alphaAer_stdErrSet[b] = errSigSet->alphaAer_stdErrSet[b] + pow( (errSigSet->alphaAer_ErrSet[j][b] - errSigSet->alphaAer_meanErrSet[b]), 2) ;
				errSigSet->   VAODr_stdErrSet[b] = errSigSet->   VAODr_stdErrSet[b] + pow( (errSigSet->   VAODr_ErrSet[j][b] - errSigSet->   VAODr_meanErrSet[b]), 2) ;
			}
			errSigSet->alphaAer_stdErrSet[b] = sqrt(errSigSet->alphaAer_stdErrSet[b] / (MonteCarlo_N_SigSet_Err-1) ) ;
			errSigSet->   VAODr_stdErrSet[b] = sqrt(errSigSet->   VAODr_stdErrSet[b] / (MonteCarlo_N_SigSet_Err-1) ) ;
		}
// VAODstd OF THE DATASET
	ReadAnalisysParameter( (const char*)FILE_PARAMETERS, "VAOD_HEIGH0", "float", (float*)&VAODheigh ) ;		errSigSet->hVAOD_stdErrSet[0] = errSigSet->VAODr_stdErrSet[(int)round(VAODheigh/dzr )] ;
	ReadAnalisysParameter( (const char*)FILE_PARAMETERS, "VAOD_HEIGH1", "float", (float*)&VAODheigh ) ;		errSigSet->hVAOD_stdErrSet[1] = errSigSet->VAODr_stdErrSet[(int)round(VAODheigh/dzr )] ;
	ReadAnalisysParameter( (const char*)FILE_PARAMETERS, "VAOD_HEIGH2", "float", (float*)&VAODheigh ) ;		errSigSet->hVAOD_stdErrSet[2] = errSigSet->VAODr_stdErrSet[(int)round(VAODheigh/dzr )] ;
	ReadAnalisysParameter( (const char*)FILE_PARAMETERS, "VAOD_HEIGH3", "float", (float*)&VAODheigh ) ;		errSigSet->hVAOD_stdErrSet[3] = errSigSet->VAODr_stdErrSet[(int)round(VAODheigh/dzr )] ;
}

int CDataLevel_2::Download_AERONET_Data( strcGlobalParameters *glbParam )
{
	// RUN THE AERONET DOWNLOADER
	FILE *fp;
	char path[1000];

//! CHECK IF aeronet_site_name IS DEFINED IN THE FILE download_aeronet/aeronet_sites_2024.txt

	// AERONET DATE TO DOWNLOAD
	time_t t = (time_t) Start_Time_AVG_L2[ (int)round(glbParam->nEventsAVG/2) ] ;
	// struct tm* timeInfo = localtime( (const time_t*)&t );
	struct tm* timeInfo = gmtime( (const time_t*)&t );
	char date_initial[20], date_final[20] ;
	sprintf( date_initial, "%04d-%02d-%02d", timeInfo->tm_year+1900, timeInfo->tm_mon+1, timeInfo->tm_mday ) ;
	sprintf( date_final  , "%04d-%02d-%02d", timeInfo->tm_year+1900, timeInfo->tm_mon+1, timeInfo->tm_mday ) ;
    printf("\nAERONET date to Download (date_initial): %s \t date_final= %s", date_initial, date_final);

	// AERONET DATA LEVEL TO DOWNLOAD
	if ( strcmp(aeronet_data_level, "L1.0") || strcmp(aeronet_data_level, "L10") )
		sprintf(aeronet_data_level, "10") ;
	else if ( strcmp(aeronet_data_level, "L1.5") || strcmp(aeronet_data_level, "L15") )
		sprintf(aeronet_data_level, "15") ;		
	else if ( strcmp(aeronet_data_level, "L2.0") || strcmp(aeronet_data_level, "L20") )
		sprintf(aeronet_data_level, "20") ;
	else
	{
		printf("\nWrong AERONET data level set in the configuration file (%s)\nSet to L0", aeronet_data_level) ;
		sprintf(aeronet_data_level, "10") ;
	}

	// FORMATING THE COMMAND TO RUN
	sprintf(path, "python3 download_aeronet/aeraod.py \"%s\" \"%s\" \"%s\" \"Aerosol Optical Depth (AOD) with Precipitable Water and Angstrom Parameter\" \"All points\" \"%s\" \"%s\"", aeronet_site_name, date_initial, date_final, aeronet_data_level, aeronet_path) ;

	// Open the command for reading 
	fp = popen(path, "r");
	if (fp == NULL)
	{
		printf("Error opening pipe!\n");
		return -1;
	}
	// Read the output a line at a time - output it 
	char buffer[1024];
	while (fgets(buffer, 1024, fp)!= NULL)
	{
		if ( strncmp(buffer, "Error downloading data:", 23) ==0 )
		{
			printf("\nError downloading AERONET data\n") ;
			sprintf(aeronet_file, "NOT_FOUND") ;
		}
		else if ( strncmp(buffer, "AERONET file already exist:", 27) ==0 )
		{
			sscanf(buffer, "AERONET file already exist: %s", aeronet_file ) ; // sprintf(aeronet_file, "/mnt/Disk-1_8TB/Argentina/AERONET_LPP/CEILAP-Comodoro_2020-03-05_to_2020-03-05_AOD10_All_points.txt") ;
			printf("\nAERONET file already exist ==> %s\n", aeronet_file) ;
		}
		else if ( strncmp(buffer, "Downloaded data to:", 19) ==0 )
		{
			sscanf(buffer, "Downloaded data to: %s", aeronet_file ) ;
			printf("\nDownloaded AERONET data to ==> %s\n", aeronet_file) ;
		}
	} // while (fgets(buffer, 1024, fp)!= NULL)

	// THERE IS A AERONET FILE (DOWNLOADED OR ALREADY SAVED)
	if ( strcmp(aeronet_file, "NOT_FOUND") !=0  )
	{
		if ( Check_AERONET_Data( (char*)aeronet_file) ==1 )
			Load_AERONET_Data( (strcGlobalParameters*)glbParam ) ;
	}
	
	pclose(fp);
	sleep(2) ;
	return 0 ;
}

int CDataLevel_2::Check_AERONET_Data( char *aeronet_file )
{
	FILE *file = fopen( aeronet_file, "r");
	if (file == NULL)
	{
		perror("Error opening file");
		return -1; // Return -1 to indicate an error
	}

	int lineCount = 0, data_level_found =0 ;
	char buffer[1024], *position; // Buffer to hold each line

	// Read the file line by line using fgets
	while (fgets(buffer, sizeof(buffer), file) != NULL)
	{
		lineCount++;
		
		position = strstr( buffer, "AERONET Data Download (Version 3 Direct Sun)" ) ;
		if (position !=NULL)
			data_level_found =1 ;
	}

	if ( data_level_found ==0 )
	{
		printf("\nAERONET data level must be \"Version 3 Direct Sun\" to work properly with LPP\n") ;
		fclose(file);
		sprintf(aeronet_file, "NOT_FOUND") ;
	}
	if ( lineCount <=8 )
	{
		printf("\nAERONET file (%s) seems to be corrupted. No AERONET data will be used.\n", aeronet_file) ;
		fclose(file);
		sprintf(aeronet_file, "NOT_FOUND") ;
	}

	if ( strcmp(aeronet_file, "NOT_FOUND") !=0 )
		fclose(file);

	if ( (lineCount >8) && (data_level_found ==1) )
		return 1 ;
	else
		return -1 ;
}

void CDataLevel_2::Load_AERONET_Data( strcGlobalParameters *glbParam )
{
	char 	line[3000] ;
	char	sAOD_Aero[20], sHeader_Angstrom[50] ; // = {"440-675_Angstrom_Exponent"} 
	int		nAero_col =1 ;
	char	*l, strDate_AERONET[15], strTime_AERONET[15] ;
	char 	sAERONET_AOD_Angs[30] ;
	int 	indx_AOD_AERONET=-1, indx_Angs_AERONET=-1, indx_Date_AERONET=-1, indx_Time_AERONET=-1 ;

	i_Num_AERONET_data = 0 	;

 	FILE *fp = fopen( aeronet_file, "r");
    if (!fp)
	{
		printf( "Can't open %s file... exit\nAERONET data is no used and saved in the output NetCDF file.", aeronet_file );
		sprintf( aeronet_file, "NOT_FOUND") ;
	}
	else
	{
		// COUNT THE NUMBER OF DATA IN THE AERONET DATASET
		while( fgets(line, sizeof(line), fp)!= NULL )
        	i_Num_AERONET_data++ ;
		fseek(fp, 0, SEEK_SET) ;
		i_Num_AERONET_data = i_Num_AERONET_data -6 ; // AUTOMATIC

		// SKIP THE FIRST 5 LINES TILL THE HEADER, SO I CAN READ IT
		for ( int i=0 ; i<5 ; i++ )
			fgets(line, sizeof(line), fp) ;
		// READ THE AERONET HEADER
		fgets(line, sizeof(line), fp) ;

		// COUNT THE NUMBER OF COLUMNS
		for (int i = 0; line[i] != '\0'; i++)
		{
			if (line[i] == ',')
				nAero_col++;
        }
		// printf("\nAERONET lines: %d \t nAero_col: %d\n", i_Num_AERONET_data, nAero_col) ;

		switch ( glbParam->iLambda[glbParam->chSel] )
		{
		case 351:
					sprintf( sAOD_Aero , "AOD_340nm" ) ;
					sprintf( sHeader_Angstrom, "340-440_Angstrom_Exponent" ) ;
					break;
		case 355:
					sprintf( sAOD_Aero , "AOD_340nm" ) ;
					sprintf( sHeader_Angstrom, "340-440_Angstrom_Exponent" ) ;
					break;
		case 532:
					sprintf( sAOD_Aero, "AOD_500nm" ) ;
					sprintf( sHeader_Angstrom, "440-675_Angstrom_Exponent" ) ;
					break;
		case 1064:
					sprintf( sAOD_Aero, "AOD_1020nm" ) ;
					sprintf( sHeader_Angstrom, "500-870_Angstrom_Exponent" ) ;
					break;
		};

		// FIND COLUMN INDEX FOR THE DATE, TIME, AOD AND ANGSTROM EXPONENT
		l = strtok( line, "," ) ; // TOKENIZE THE READ LINE
		for (int c =0; c <nAero_col ; c++)
		{
			if( (strncmp( l, "Date(dd:mm:yyyy)", 16 ) ==0) && (indx_Date_AERONET <0) )
				indx_Date_AERONET = c ;
			else if( (strncmp( l, "Time(hh:mm:ss)", 14 ) ==0) && (indx_Time_AERONET <0) )
				indx_Time_AERONET = c ;
			else if( (strncmp( l, sAOD_Aero, 10 ) ==0) && (indx_AOD_AERONET <0) )
				indx_AOD_AERONET = c ;
			else if( (strncmp( l, sHeader_Angstrom, 25 ) ==0) && (indx_Angs_AERONET <0) )
				indx_Angs_AERONET = c ;

			l = strtok( NULL, "," ) ;
		}
		// printf("\nsAOD_Aero= %s \t sHeader_Angstrom= %s", sAOD_Aero, sHeader_Angstrom) ;
		// printf( "\nindx_Date_AERONET= %d \t indx_Time_AERONET= %d \t indx_AOD_AERONET= %d \t indx_Angs_AERONET= %d \n", indx_Date_AERONET, indx_Time_AERONET, indx_AOD_AERONET , indx_Angs_AERONET ) ;

		if ( AERONET_time ==NULL )
		{
			AERONET_AOD  = (double*) new double [ i_Num_AERONET_data ] ;
			AERONET_Angs = (double*) new double [ i_Num_AERONET_data ] ;
			AERONET_time = (int*   ) new int    [ i_Num_AERONET_data ] ;

			// AERONET'S AOD TO LIDAR'S TIME AOD
			AOD_Lidar_Time = (double*) new double [glbParam->nEventsAVG] ;
				for (int i =0; i < glbParam->nEventsAVG; i++) AOD_Lidar_Time[i] =-1 ;
		}

	    struct tm   tm_Time_Aeronet ;
		tm_Time_Aeronet.tm_isdst = 0 ;    tm_Time_Aeronet.tm_gmtoff = round(glbParam->Time_Zone *60*60) ;

		for (int r =0; r <i_Num_AERONET_data ; r++)
		{
			fgets(line, sizeof(line), fp) ;

			l = strtok( line, "," ) ; // TOKENIZE THE READ LINE
			for (int c =0; c <nAero_col; c++)
			{
				if( c == indx_Date_AERONET )
				{
					sprintf( strDate_AERONET, "%s", l ) ; // READ AND FORMAT THE DATE
					sscanf ( strDate_AERONET, "%2d:%2d:%4d", &tm_Time_Aeronet.tm_mday, &tm_Time_Aeronet.tm_mon, &tm_Time_Aeronet.tm_year ) ;
					tm_Time_Aeronet.tm_mon  = tm_Time_Aeronet.tm_mon  -1	;
					tm_Time_Aeronet.tm_year = tm_Time_Aeronet.tm_year -1900 ;
				}
				else if( c == indx_Time_AERONET )
				{
					// int i =0;
						sprintf( strTime_AERONET, "%s", l) ; // READ AND FORMAT THE TIME
						sscanf ( strTime_AERONET, "%2d:%2d:%2d", &tm_Time_Aeronet.tm_hour, &tm_Time_Aeronet.tm_min, &tm_Time_Aeronet.tm_sec ) ;
						AERONET_time[r] =(int)((int)timegm( (tm*)&tm_Time_Aeronet ) - (int)round(glbParam->Time_Zone *60*60)) ; // SECONDS IN UTC TIME CONVERSION
					// printf("\nAERONET TIME:i= %d\ntm_Time_Aeronet.tm_mon= %d\ntm_Time_Aeronet.tm_year= %d\n", i, tm_Time_Aeronet.tm_mon, tm_Time_Aeronet.tm_year ) ;
				}
				else if( c == indx_AOD_AERONET )
				{
					sprintf(sAERONET_AOD_Angs, "%s", l) ;
					AERONET_AOD[r] = (double)atof( sAERONET_AOD_Angs ) ; 
				}
				else if ( c == indx_Angs_AERONET )
				{
					sprintf(sAERONET_AOD_Angs, "%s", l) ;
					AERONET_Angs[r] = (double)atof( sAERONET_AOD_Angs ) ; 
					break ;
				}
				l = strtok( NULL, "," ) ;
			}
			// printf("\n\t%s - %s (%d) -- %lf - %lf", strDate_AERONET, strTime_AERONET, AERONET_time[r], AERONET_AOD[r], AERONET_Angs[r] ) ;
		} // for (int r =0; r <i_Num_AERONET_data ; r++)

		if ( (Start_Time_AVG_L2[0] > AERONET_time[i_Num_AERONET_data-1]) || (Start_Time_AVG_L2[glbParam->nEventsAVG-1] < AERONET_time[0]) )
		{
			printf("\n\nIt seems that the AERONET data does not correspond to the lidar measurement day. Lidar inversion versus AERONET data won't be computed.") ;
			printf("\n Start_Time_AVG_L2[0]= %d \t Start_Time_AVG_L2[glbParam->nEventsAVG-1]= %d", Start_Time_AVG_L2[0], Start_Time_AVG_L2[glbParam->nEventsAVG-1]) ;
			printf("\n AERONET_time[0]= %d \t AERONET_time[i_Num_AERONET_data-1]= %d", AERONET_time[0], AERONET_time[i_Num_AERONET_data-1] ) ;
			printf("\n strTime_AERONET= %s \n", strTime_AERONET ) ;
			sprintf( aeronet_file, "NOT_FOUND") ;
		}
		else
		{
			double *err = new double [i_Num_AERONET_data] ;
			double 	min_err			;
			double 	lambda, lambda1, lambda2, lambda3	;
			int 	indx_err_min 	;
			int 	indx_time_AERO_after, indx_time_AERO_before 	;
			int 	mean_lidar_Time_AVG_L2 ;
			for (int t =0; t <glbParam->nEventsAVG ; t++)
			{
				mean_lidar_Time_AVG_L2 = (int) round( Start_Time_AVG_L2[t]/2 + Stop_Time_AVG_L2[t]/2 ) ;
// printf("\n Start_Time_AVG_L2/Stop_Time_AVG_L2 = %d / %d ", Start_Time_AVG_L2[t], Stop_Time_AVG_L2[t] ) ;
// printf("\n AERONET_time before / after= %d / %d", AERONET_time[indx_time_AERO_before], AERONET_time[indx_time_AERO_after]) ;
// printf("\n mean_lidar_Time_AVG_L2= %d \n", mean_lidar_Time_AVG_L2 ) ;
				// FIND THE CLOSEST AERONET TIME FOR Start_Time_AVG_L2[t]
				// for (int at =0; at <i_Num_AERONET_data; at++)	err[at] = fabs(Start_Time_AVG_L2[t] - AERONET_time[at]) ;
				for (int at =0; at <i_Num_AERONET_data; at++)	err[at] = fabs(mean_lidar_Time_AVG_L2 - AERONET_time[at]) ;
				findIndxMin( (double*)&err[0], (int)0, (int)(i_Num_AERONET_data-1), (int*)&indx_err_min, (double*)&min_err ) ;
				// if ( (indx_err_min ==0) && (Start_Time_AVG_L2[t]<AERONET_time[0]) )
				if ( (indx_err_min ==0) && (mean_lidar_Time_AVG_L2<AERONET_time[0]) ) // IF LIDAR TIME IS BEFORE AERONET DATA MEASUREMENT TIME
					AOD_Lidar_Time[t] = -1 ;
				// else if ( (indx_err_min == (i_Num_AERONET_data-1)) && ( Start_Time_AVG_L2[t] > AERONET_time[i_Num_AERONET_data-1] ) )
				else if ( (indx_err_min == (i_Num_AERONET_data-1)) && ( mean_lidar_Time_AVG_L2 > AERONET_time[i_Num_AERONET_data-1] ) ) // IF LIDAR TIME IS AFTER AERONET DATA MEASUREMENT TIME
					AOD_Lidar_Time[t] = -1 ;
				else
				{
					// if ( Start_Time_AVG_L2[t] < AERONET_time[indx_err_min] )
					if ( mean_lidar_Time_AVG_L2 < AERONET_time[indx_err_min] )
					{
						indx_time_AERO_before  = indx_err_min -1  ;
						indx_time_AERO_after   = indx_err_min     ;
					}
					else
					{
						indx_time_AERO_before = indx_err_min   	;
						indx_time_AERO_after  = indx_err_min +1	;
					}
					// lambda1 = (double)(Start_Time_AVG_L2[t] - AERONET_time[indx_time_AERO_before]) ;
					lambda1 = (double)(AERONET_time[indx_time_AERO_after] - AERONET_time[indx_time_AERO_before]) ;
					lambda2 = (double)( AERONET_AOD[indx_time_AERO_after] - AERONET_AOD[indx_time_AERO_before]) ;
					lambda  = (double)(lambda2 / lambda1) ;
					AOD_Lidar_Time[t] = lambda * ( mean_lidar_Time_AVG_L2 - AERONET_time[indx_time_AERO_before] ) + AERONET_AOD[indx_time_AERO_before] ;
// printf("\t AOD_Lidar_Time= %lf (%lf)\n", AOD_Lidar_Time[t], AERONET_Angs[t] ) ;
					lambda1 = (double)(glbParam->iLambda[glbParam->chSel]/500.0) ;
					lambda2 = (double) pow( (double)lambda1, double(-AERONET_Angs[t]) ) ;
					lambda3 = (double) AOD_Lidar_Time[t] * lambda2 ;
					AOD_Lidar_Time[t] = double( lambda3 ) ;
				}
			} // for (int t =0; t <glbParam->nEventsAVG ; t++)
		} // else-if ( (Start_Time_AVG_L2[0] > AERONET_time[i_Num_AERONET_data-1]) || (Start_Time_AVG_L2[glbParam->nEventsAVG-1] < AERONET_time[0]) )
		// sprintf( aeronet_file,"NOT_FOUND" ) ;
	}
	fclose(fp);
}

// void CDataLevel_2::Load_AERONET_Data( strcGlobalParameters *glbParam )
// {
// 	char 	line[3000] ;
// 	char	sAOD_Aero[20], sHeader_Angstrom[50] ; // = {"440-675_Angstrom_Exponent"} 
// 	int		nAero_col =1 ;
// 	char	*l, strDate_AERONET[15], strTime_AERONET[15] ;
// 	char 	sAERONET_AOD_Angs[30] ;
// 	int 	indx_AOD_AERONET=-1, indx_Angs_AERONET=-1, indx_Date_AERONET=-1, indx_Time_AERONET=-1 ;

// 	i_Num_AERONET_data = 0 	;

//  	FILE *fp = fopen( aeronet_file, "r");
//     if (!fp)
// 	{
// 		printf( "Can't open %s file... exit\nAERONET data is no used and saved in the output NetCDF file.", aeronet_file );
// 		sprintf( aeronet_file, "NOT_FOUND") ;
// 	}
// 	else
// 	{
// 		// COUNT THE NUMBER OF DATA IN THE AERONET DATASET
// 		while( fgets(line, sizeof(line), fp)!= NULL )
//         	i_Num_AERONET_data++ ;
// 		fseek(fp, 0, SEEK_SET) ;
// 		i_Num_AERONET_data = i_Num_AERONET_data -6 ; // AUTOMATIC

// 		// SKIP THE FIRST 5 LINES TILL THE HEADER, SO I CAN READ IT
// 		for ( int i=0 ; i<5 ; i++ )
// 			fgets(line, sizeof(line), fp) ;
// 		// READ THE AERONET HEADER
// 		fgets(line, sizeof(line), fp) ;

// 		// COUNT THE NUMBER OF COLUMNS
// 		for (int i = 0; line[i] != '\0'; i++)
// 		{
// 			if (line[i] == ',')
// 				nAero_col++;
//         }
// 		// printf("\nAERONET lines: %d \t nAero_col: %d\n", i_Num_AERONET_data, nAero_col) ;

// 		switch ( glbParam->iLambda[glbParam->chSel] )
// 		{
// 		case 351:
// 					sprintf( sAOD_Aero , "AOD_340nm" ) ;
// 					sprintf( sHeader_Angstrom, "340-440_Angstrom_Exponent" ) ;
// 					break;
// 		case 355:
// 					sprintf( sAOD_Aero , "AOD_340nm" ) ;
// 					sprintf( sHeader_Angstrom, "340-440_Angstrom_Exponent" ) ;
// 					break;
// 		case 532:
// 					sprintf( sAOD_Aero, "AOD_500nm" ) ;
// 					sprintf( sHeader_Angstrom, "440-675_Angstrom_Exponent" ) ;
// 					break;
// 		case 1064:
// 					sprintf( sAOD_Aero, "AOD_1020nm" ) ;
// 					sprintf( sHeader_Angstrom, "500-870_Angstrom_Exponent" ) ;
// 					break;
// 		};

// 		// FIND COLUMN INDEX FOR THE DATE, TIME, AOD AND ANGSTROM EXPONENT
// 		l = strtok( line, "," ) ; // TOKENIZE THE READ LINE
// 		for (int c =0; c <nAero_col ; c++)
// 		{
// 			if( (strncmp( l, "Date(dd:mm:yyyy)", 16 ) ==0) && (indx_Date_AERONET <0) )
// 				indx_Date_AERONET = c ;
// 			else if( (strncmp( l, "Time(hh:mm:ss)", 14 ) ==0) && (indx_Time_AERONET <0) )
// 				indx_Time_AERONET = c ;
// 			else if( (strncmp( l, sAOD_Aero, 10 ) ==0) && (indx_AOD_AERONET <0) )
// 				indx_AOD_AERONET = c ;
// 			else if( (strncmp( l, sHeader_Angstrom, 25 ) ==0) && (indx_Angs_AERONET <0) )
// 				indx_Angs_AERONET = c ;

// 			l = strtok( NULL, "," ) ;
// 		}
// 		// printf("\nsAOD_Aero= %s \t sHeader_Angstrom= %s", sAOD_Aero, sHeader_Angstrom) ;
// 		// printf( "\nindx_Date_AERONET= %d \t indx_Time_AERONET= %d \t indx_AOD_AERONET= %d \t indx_Angs_AERONET= %d \n", indx_Date_AERONET, indx_Time_AERONET, indx_AOD_AERONET , indx_Angs_AERONET ) ;

// 		if ( AERONET_time ==NULL )
// 		{
// 			AERONET_AOD  = (double*) new double [ i_Num_AERONET_data ] ;
// 			AERONET_Angs = (double*) new double [ i_Num_AERONET_data ] ;
// 			AERONET_time = (int*   ) new int    [ i_Num_AERONET_data ] ;

// 			// AERONET'S AOD TO LIDAR'S TIME AOD
// 			AOD_Lidar_Time = (double*) new double [glbParam->nEventsAVG] ;
// 				for (int i =0; i < glbParam->nEventsAVG; i++) AOD_Lidar_Time[i] =-1 ;
// 		}

// 	    struct tm   tm_Time_Aeronet ;
// 		tm_Time_Aeronet.tm_isdst = 0 ;    tm_Time_Aeronet.tm_gmtoff = round(glbParam->Time_Zone *60*60) ;

// 		for (int r =0; r <i_Num_AERONET_data ; r++)
// 		{
// 			fgets(line, sizeof(line), fp) ;

// 			l = strtok( line, "," ) ; // TOKENIZE THE READ LINE
// 			for (int c =0; c <nAero_col; c++)
// 			{
// 				if( c == indx_Date_AERONET )
// 				{
// 					sprintf( strDate_AERONET, "%s", l ) ; // READ AND FORMAT THE DATE
// 					sscanf ( strDate_AERONET, "%2d:%2d:%4d", &tm_Time_Aeronet.tm_mday, &tm_Time_Aeronet.tm_mon, &tm_Time_Aeronet.tm_year ) ;
// 					tm_Time_Aeronet.tm_mon  = tm_Time_Aeronet.tm_mon  -1	;
// 					tm_Time_Aeronet.tm_year = tm_Time_Aeronet.tm_year -1900 ;
// 				}
// 				else if( c == indx_Time_AERONET )
// 				{
// 					// int i =0;
// 						sprintf( strTime_AERONET, "%s", l) ; // READ AND FORMAT THE TIME
// 						sscanf ( strTime_AERONET, "%2d:%2d:%2d", &tm_Time_Aeronet.tm_hour, &tm_Time_Aeronet.tm_min, &tm_Time_Aeronet.tm_sec ) ;
// 						AERONET_time[r] =(int)((int)timegm( (tm*)&tm_Time_Aeronet ) - (int)round(glbParam->Time_Zone *60*60)) ; // SECONDS IN UTC TIME CONVERSION
// 					// printf("\nAERONET TIME:i= %d\ntm_Time_Aeronet.tm_mon= %d\ntm_Time_Aeronet.tm_year= %d\n", i, tm_Time_Aeronet.tm_mon, tm_Time_Aeronet.tm_year ) ;
// 				}
// 				else if( c == indx_AOD_AERONET )
// 				{
// 					sprintf(sAERONET_AOD_Angs, "%s", l) ;
// 					AERONET_AOD[r] = (double)atof( sAERONET_AOD_Angs ) ; 
// 				}
// 				else if ( c == indx_Angs_AERONET )
// 				{
// 					sprintf(sAERONET_AOD_Angs, "%s", l) ;
// 					AERONET_Angs[r] = (double)atof( sAERONET_AOD_Angs ) ; 
// 					break ;
// 				}
// 				l = strtok( NULL, "," ) ;
// 			}
// 			// printf("\n\t%s - %s (%d) -- %lf - %lf", strDate_AERONET, strTime_AERONET, AERONET_time[r], AERONET_AOD[r], AERONET_Angs[r] ) ;
// 		} // for (int r =0; r <i_Num_AERONET_data ; r++)

// 		if ( (Start_Time_AVG_L2[0] > AERONET_time[i_Num_AERONET_data-1]) || (Start_Time_AVG_L2[glbParam->nEventsAVG-1] < AERONET_time[0]) )
// 		{
// 			printf("\n\nIt seems that the AERONET data does not correspond to the lidar measurement day. Lidar inversion versus AERONET data won't be computed.") ;
// 			printf("\n Start_Time_AVG_L2[0]= %d \t Start_Time_AVG_L2[glbParam->nEventsAVG-1]= %d", Start_Time_AVG_L2[0], Start_Time_AVG_L2[glbParam->nEventsAVG-1]) ;
// 			printf("\n AERONET_time[0]= %d \t AERONET_time[i_Num_AERONET_data-1]= %d", AERONET_time[0], AERONET_time[i_Num_AERONET_data-1] ) ;
// 			printf("\n strTime_AERONET= %s \n", strTime_AERONET ) ;
// 			sprintf( aeronet_file, "NOT_FOUND") ;
// 		}
// 		else
// 		{
// 			double *err = new double [i_Num_AERONET_data] ;
// 			double 	min_err			;
// 			double 	lambda, lambda1, lambda2, lambda3	;
// 			int 	indx_err_min 	;
// 			int 	indx_time_AERO_after, indx_time_AERO_before 	;
// 			int 	mean_lidar_Time_AVG_L2 ;
// 			for (int t =0; t <glbParam->nEventsAVG ; t++)
// 			{
// 				mean_lidar_Time_AVG_L2 = (int)round((Start_Time_AVG_L2[t] + Stop_Time_AVG_L2[t]) /2) ;
// 				// FIND THE CLOSEST AERONET TIME FOR Start_Time_AVG_L2[t]
// 				// for (int at =0; at <i_Num_AERONET_data; at++)	err[at] = fabs(Start_Time_AVG_L2[t] - AERONET_time[at]) ;
// 				for (int at =0; at <i_Num_AERONET_data; at++)	err[at] = fabs(mean_lidar_Time_AVG_L2 - AERONET_time[at]) ;
// 				findIndxMin( (double*)&err[0], (int)0, (int)(i_Num_AERONET_data-1), (int*)&indx_err_min, (double*)&min_err ) ;
// 				// if ( (indx_err_min ==0) && (Start_Time_AVG_L2[t]<AERONET_time[0]) )
// 				if ( (indx_err_min ==0) && (mean_lidar_Time_AVG_L2<AERONET_time[0]) ) // IF LIDAR TIME IS BEFORE AERONET DATA MEASUREMENT TIME
// 					AOD_Lidar_Time[t] = -1 ;
// 				// else if ( (indx_err_min == (i_Num_AERONET_data-1)) && ( Start_Time_AVG_L2[t] > AERONET_time[i_Num_AERONET_data-1] ) )
// 				else if ( (indx_err_min == (i_Num_AERONET_data-1)) && ( mean_lidar_Time_AVG_L2 > AERONET_time[i_Num_AERONET_data-1] ) ) // IF LIDAR TIME IS AFTER AERONET DATA MEASUREMENT TIME
// 					AOD_Lidar_Time[t] = -1 ;
// 				else
// 				{
// 					// if ( Start_Time_AVG_L2[t] < AERONET_time[indx_err_min] )
// 					if ( mean_lidar_Time_AVG_L2 < AERONET_time[indx_err_min] )
// 					{
// 						indx_time_AERO_before  = indx_err_min -1  ;
// 						indx_time_AERO_after   = indx_err_min     ;
// 					}
// 					else
// 					{
// 						indx_time_AERO_before = indx_err_min   	;
// 						indx_time_AERO_after  = indx_err_min +1	;
// 					}
// 					// printf("\n Start_Time_AVG_L2= %d \t AERONET_time before / after= %d / %d", Start_Time_AVG_L2[t], AERONET_time[indx_time_AERO_before], AERONET_time[indx_time_AERO_after]) ;
// 					// lambda1 = (double)(Start_Time_AVG_L2[t] - AERONET_time[indx_time_AERO_before]) ;
// 					lambda1 = (double)(mean_lidar_Time_AVG_L2 - AERONET_time[indx_time_AERO_before]) ;
// 					lambda2 = (double)(AERONET_time[indx_time_AERO_after] - AERONET_time[indx_time_AERO_before]) ;
// 					lambda  = (double)(lambda1 / lambda2) ;
// 					AOD_Lidar_Time[t] = lambda * ( AERONET_AOD[indx_time_AERO_after] - AERONET_AOD[indx_time_AERO_before] ) + AERONET_AOD[indx_time_AERO_before] ;
// 					printf("\t AOD_Lidar_Time= %lf (%lf)", AOD_Lidar_Time[t], AERONET_Angs[t] ) ;
// 					lambda1 = (double)(glbParam->iLambda[glbParam->chSel]/500.0) ;
// 					lambda2 = (double) pow( (double)lambda1, double(-AERONET_Angs[t]) ) ;
// 					lambda3 = (double) AOD_Lidar_Time[t] * lambda2 ;
// 					AOD_Lidar_Time[t] = double( lambda3 ) ;
// 				}
// 			} // for (int t =0; t <glbParam->nEventsAVG ; t++)
// 		} // else-if ( (Start_Time_AVG_L2[0] > AERONET_time[i_Num_AERONET_data-1]) || (Start_Time_AVG_L2[glbParam->nEventsAVG-1] < AERONET_time[0]) )
// 		// sprintf( aeronet_file,"NOT_FOUND" ) ;
// 	}
// 	fclose(fp);
// }

/* 		BACKUP 
void CDataLevel_2::MonteCarloRandomError( double *pr2Glued, double *pr, strcGlobalParameters *glbParam, strcMolecularData *dataMol, strcIndexMol *indxMol, strcFernaldInversion *fernaldVectors, strcErrorSignalSet *rndErrSigSet )
{
	strcAerosolData dataAerErr ;
	dataAerErr.alphaAer	= (double*) malloc( glbParam->nBins * sizeof(double) ) ;  memset( dataAerErr.alphaAer, 0, ( glbParam->nEventsAVG * sizeof(double) ) ) ;
	dataAerErr.betaAer	= (double*) malloc( glbParam->nBins * sizeof(double) ) ;  memset( dataAerErr.betaAer , 0, ( glbParam->nEventsAVG * sizeof(double) ) ) ;
	dataAerErr.VAODr	= (double*) malloc( glbParam->nBins * sizeof(double) ) ;  memset( dataAerErr.VAODr   , 0, ( glbParam->nEventsAVG * sizeof(double) ) ) ;

	int 	MonteCarlo_N_SigSet_Err ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "MonteCarlo_N_SigSet_Err", "int" , (int*)&MonteCarlo_N_SigSet_Err ) ;
// GET THE STANDAR DEVIATION TO GENERATE THE RANDOM ERROR IN THE LIDAR SIGNALS SET
	strcFitParam	fitParam ;
	fitParam.indxInitFit = dataMol->nBins - glbParam->nBinsBkg  	  ;
	fitParam.indxEndFit  = dataMol->nBins -1 						  ;
	fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1 ;
	double *prFit = (double*) new double[glbParam->nBins] ;
		oLOp->Fit( (double*)&dataMol->prMol[fitParam.indxInitFit], (double*)&pr[fitParam.indxInitFit], fitParam.nFit   , "wB", "NOTall", (strcFitParam*)&fitParam, (double*)prFit ) ;
	delete prFit ;	
	double 	stdPr = sqrt( fitParam.squared_sum_fit/(fitParam.nFit -1) ) ;
	int 	spamAvgWin ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "spamAvgWin", "int", (int*)&spamAvgWin ) ;

	for( int j=0 ; j < MonteCarlo_N_SigSet_Err ; j++ )
	{
		for ( int i=0 ; i < glbParam->nBins ; i++ )
		{
			rndErrSigSet->prGlued [j][i] = pr2Glued[i] / (glbParam->r[i]*glbParam->r[i]) ;
			rndErrSigSet->prNoisy [j][i] = RandN2( rndErrSigSet->prGlued[j][i], stdPr ) ;
			rndErrSigSet->pr2Noisy[j][i] = rndErrSigSet->prNoisy[j][i] * (glbParam->r[i]*glbParam->r[i]) ;
		}
		// smoothGSL( (double*)rndErrSigSet->pr2Noisy[j], (int)(glbParam->nBins), (int)spamAvgWin, (double*)rndErrSigSet->pr2Noisy[j] ) ;
		smooth( (double*)rndErrSigSet->pr2Noisy[j], (int)0, (int)(glbParam->nBins-1), (int)spamAvgWin, (double*)rndErrSigSet->pr2Noisy[j] ) ;
// FERNALD INVERSION
	// FernaldInversion( (double*)rndErrSigSet->pr2Noisy[j], (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam, (int)indxMol->indxInicMol[0]+110, (double)glbParam->ka, (strcFernaldInversion*)fernaldVectors, (strcAerosolData*)&dataAerErr ) ;
	FernaldInversion( (strcGlobalParameters*)&glbParam, (int)glbParam->evSel, (int)glbParam->indxWL_PDL2, (strcMolecularData*)dataMol ) ;
		for( int b=0 ; b<glbParam->nBins ; b++ )
		{
			rndErrSigSet->alphaAer_ErrSet[j][b] = dataAerErr.alphaAer[b] ;
			rndErrSigSet->   VAODr_ErrSet[j][b] = dataAerErr.VAODr   [b] ;
		}
	} // for( int j=0 ; j < MonteCarlo_N_SigSet_Err ; j++ )
// OBTAIN MEAN AND STANDAR DEVIATION
		// GetErrSetParam( (char*)glbParam->FILE_PARAMETERS, (int)MonteCarlo_N_SigSet_Err, (int)glbParam->nBins, (double)dataMol->dzr, (strcErrorSignalSet*)rndErrSigSet ) ;
}
*/

/*
void CDataLevel_2::Fernald_1983( strcGlobalParameters *glbParam, int t, int c, strcMolecularData *dataMol )
{
	LRM = (double) dataMol->LR_mol ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "R_ref", "double" , (double*)&R_ref ) ;

	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "avg_Points_Fernald", "int", (int*)&glbParam->avg_Points_Fernald ) ;
	if ( glbParam->avg_Points_Fernald[c] >1 )
	{
		smooth( (double*)&pr2[t][c][0], (int)0, (int)(glbParam->nBins-1), (int)glbParam->avg_Points_Fernald[c], (double*)pr2_s 			) ;
		smooth( (double*)glbParam->r  , (int)0, (int)(glbParam->nBins-1), (int)glbParam->avg_Points_Fernald[c], (double*)glbParam->r_avg ) ;
// printf("\nglbParam->r[0]: %lf\nglbParam->r[1]: %lf\nglbParam->r[2]: %lf\nglbParam->r[3]: %lf\n", glbParam->r[0], glbParam->r[1], glbParam->r[2], glbParam->r[3] ) ;
// printf("\nglbParam->r_avg[0]: %lf\nglbParam->r_avg[1]: %lf\nglbParam->r_avg[2]: %lf\nglbParam->r_avg[3]: %lf\n", glbParam->r_avg[0], glbParam->r_avg[1], glbParam->r_avg[2], glbParam->r_avg[3] ) ;

		for (int i =0; i <glbParam->nBins; i++)
		{
			pr2[t][c][i] 	= (double)pr2_s[i] 			 ;
			glbParam->r[i] 	= (double)glbParam->r_avg[i] ; 
		}
	}

	double 	pr2_Ref ;
	int 	avg_Half_Points_Fernald_Ref ;
	string 	reference_method ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "reference_method", "string", (char*)reference_method.c_str() ) ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "avg_Half_Points_Fernald_Ref", "int", (int*)&avg_Half_Points_Fernald_Ref ) ;

	double heightRef_Inversion_ASL ;
	ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "heightRef_Inversion_ASL" , "double" , (double*)&heightRef_Inversion_ASL ) ;

	if ( heightRef_Inversion_ASL >0 )
		indxRef_Fernald[glbParam->evSel] = (int)round( (heightRef_Inversion_ASL - glbParam->siteASL) /dzr ) ;
	else
	{
		indxRef_Fernald[glbParam->evSel] = (int)(glbParam->indxEndSig_ev[glbParam->evSel] - 2*avg_Half_Points_Fernald_Ref) ;
	}
	if ( strcmp( reference_method.c_str(), "MEAN" ) ==0 )
	{
			sum( (double*)&pr[t][0], (int)(indxRef_Fernald[glbParam->evSel] -avg_Half_Points_Fernald_Ref), (int)(indxRef_Fernald[glbParam->evSel] +avg_Half_Points_Fernald_Ref), (double*)&pr2_Ref ) ;
			pr2_Ref = pr2_Ref /(avg_Half_Points_Fernald_Ref +1) ;
			pr2_Ref = pr2_Ref * glbParam->r[indxRef_Fernald[glbParam->evSel]] * glbParam->r[indxRef_Fernald[glbParam->evSel]] ;
	}
	else if ( strcmp( reference_method.c_str(), "FIT" ) ==0 )
	{
		double *pr2Fit = (double*) new double[ glbParam->nBins ] ;
		strcFitParam fitParam ;
		fitParam.indxInitFit = indxRef_Fernald[glbParam->evSel] - avg_Half_Points_Fernald_Ref ;
		fitParam.indxEndFit  = indxRef_Fernald[glbParam->evSel] + avg_Half_Points_Fernald_Ref ;
		fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1;
		// 	RayleighFit( (double*)&pr2[t][c][0], (double*)dataMol->pr2Mol, glbParam->nBins , "wOutB", "NOTall", (strcFitParam*)&fitParam, (double*)pr2Fit ) ;
		// pr2_Ref = pr2Fit[indxRef_Fernald[glbParam->evSel]] ;
			oLOp->Fit( (double*)&pr[t][0], (double*)dataMol->prMol, glbParam->nBins , "wOutB", "NOTall", (strcFitParam*)&fitParam, (double*)pr2Fit ) ;
		pr2_Ref = pr2Fit[indxRef_Fernald[glbParam->evSel]] * glbParam->r[indxRef_Fernald[glbParam->evSel]] * glbParam->r[indxRef_Fernald[glbParam->evSel]] ;
		delete pr2Fit ;
	}
	pr2_Ref = pr2_Ref *1.0 ;

	for( int i=0 ; i <glbParam->nBins ; i++  )
		pr2n[i] = pr2[t][c][i] /pr2_Ref ;

	double phi, p, ip, CalTerm, beta_Tot ;

	for ( int l=0 ; l <nLRs ; l++ ) // LOOP ACROSS LRs
	{
		beta_Aer[t][l][indxRef_Fernald[glbParam->evSel]+1] = R_ref * dataMol->betaMol[indxRef_Fernald[glbParam->evSel]] - dataMol->betaMol[indxRef_Fernald[glbParam->evSel]] ;
		int invDirection = -1 ; // 1 = BACKWARD
		for( int i=indxRef_Fernald[glbParam->evSel] ; i >=indxInitSig ; i-- )
		{	// BACKWARD INVERSION - FERNALD 1983
			phi = (LR[l]-LRM) * ( dataMol->betaMol[i+1] + dataMol->betaMol[i] ) * invDirection * glbParam->dr ;
			p   = pr2n[i] * exp(-phi) ;

			CalTerm = pr2n[i+1] / ( beta_Aer[t][l][i+1] + dataMol->betaMol[i+1] ) ;
			ip = ( pr2n[i+1] + p ) * invDirection * glbParam->dr /2 ;

			beta_Tot = p / ( CalTerm - 2*LR[l] * ip ) ;
			beta_Aer[t][l][i]  = beta_Tot - dataMol->betaMol[i] ;
			alpha_Aer[t][l][i] = beta_Aer[t][l][i] * LR[l] ;
		}

		beta_Aer[t][l][indxRef_Fernald[glbParam->evSel]-1] = R_ref * dataMol->betaMol[indxRef_Fernald[glbParam->evSel]] - dataMol->betaMol[indxRef_Fernald[glbParam->evSel]] ;
		invDirection = 1 ; // 1 = FORWARD
		for ( int i=(indxRef_Fernald[glbParam->evSel]-1) ; i <=indxEndSig ; i++ )
		{	// FORWARD INVERSION - FERNALD 1983
			phi = (LR[l]-LRM) * ( dataMol->betaMol[i-1] + dataMol->betaMol[i] ) * invDirection * glbParam->dr ;
			p   = pr2n[i] * exp(-phi) ;
			
			CalTerm = pr2n[i-1] / ( beta_Aer[t][l][i-1] + dataMol->betaMol[i-1] ) ;
			ip = ( pr2n[i-1] + p ) * invDirection * glbParam->dr /2 ;

			beta_Tot = p / ( CalTerm - 2*LR[l] * ip ) ;
			beta_Aer[t][l][i]  = beta_Tot - dataMol->betaMol[i] ;
			alpha_Aer[t][l][i] = beta_Aer[t][l][i] * LR[l] ;
		}
		for (int i =0 ; i <indxInitSig; i++)
		{
			alpha_Aer[t][l][i] = alpha_Aer[t][l][indxInitSig] ;
			beta_Aer[t][l][i]  = beta_Aer[t][l][indxInitSig] ;
		}

		int indx_integral_max_range_for_AOD ;
		int integral_max_range_for_AOD ;
		ReadAnalisysParameter( (const char*)glbParam->FILE_PARAMETERS, "integral_max_range_for_AOD", "int", (int*)&integral_max_range_for_AOD ) ;
		indx_integral_max_range_for_AOD = (int)round(integral_max_range_for_AOD /glbParam->dr) ;

		smooth( (double*)&beta_Aer[t][l][0] , (int)0, (int)(glbParam->nBins-1), (int)glbParam->avg_Points_Fernald[c], (double*)&beta_Aer[t][l][0]  ) ;
		smooth( (double*)&alpha_Aer[t][l][0], (int)0, (int)(glbParam->nBins-1), (int)glbParam->avg_Points_Fernald[c], (double*)&alpha_Aer[t][l][0] ) ;
		sum(    (double*)&alpha_Aer[t][l][0], (int)0, (int)indx_integral_max_range_for_AOD, (double*)&AOD_LR[t][l] ) ;
		AOD_LR[t][l] = AOD_LR[t][l] * glbParam->dr ;
		printf("\nAOD@LR = %lf --> %lf", LR[l], AOD_LR[t][l]) ;

	} // for ( int l=0 ; l <nLRs ; l++ ) // LOOP ACROSS LRs
}
*/
