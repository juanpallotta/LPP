
#include "CLidar_Operations.hpp"
#include "lidarMathFunc.hpp"
#include "CMolecularData.hpp"

CLidar_Operations::CLidar_Operations(strcGlobalParameters *glbParam) 
{
  dummy   = (double *)new double[glbParam->nBins];
  pr_misc = (double *)new double[glbParam->nBins];
  pr2_i   = (double *)new double[glbParam->nBins];
  prS     = (double *)new double[glbParam->nBins];

  errRMS_Bias = (double *)new double[nBiasRes_Auto];
  b_i = (double *)new double[nBiasRes_Auto + 1];
  coeff = (double *)new double[2];

  ReadAnalysisParameter(glbParam->FILE_SOFT_CODED_VALUES, "deltaNorm", "int", (int *)&deltaNorm); 
  ReadAnalysisParameter(glbParam->FILE_SOFT_CODED_VALUES, "avg_Points_Cloud_Mask", "int", (int *)&avg_Points_Cloud_Mask);
  ReadAnalysisParameter(glbParam->FILE_SOFT_CODED_VALUES, "std_factor_layer_mask", "double", (double *)&std_factor_layer_mask);
  ReadAnalysisParameter(glbParam->FILE_SOFT_CODED_VALUES, "stepScanCloud", "int", (int *)&stepScanCloud);
  ReadAnalysisParameter(glbParam->FILE_SOFT_CODED_VALUES, "DELTA_RANGE_LIM_BINS", "int", (int *)&DELTA_RANGE_LIM_BINS);
  ReadAnalysisParameter(glbParam->FILE_SOFT_CODED_VALUES, "CLOUD_MIN_THICK", "int", (int *)&CLOUD_MIN_THICK);

  char compute_pbl_mask_buf[128] = {0};
  ReadAnalysisParameter(glbParam->FILE_SOFT_CODED_VALUES, "COMPUTE_PBL_MASK", "string", compute_pbl_mask_buf, sizeof(compute_pbl_mask_buf));
  compute_pbl_mask.assign(compute_pbl_mask_buf);
  char compute_layer_mask_buf[128] = {0};
  ReadAnalysisParameter(glbParam->FILE_SOFT_CODED_VALUES, "COMPUTE_LAYER_MASK", "string", compute_layer_mask_buf, sizeof(compute_layer_mask_buf));
  compute_layer_mask.assign(compute_layer_mask_buf);

  GetMem_cloudProfiles((strcGlobalParameters *)glbParam);

  GetMem_indxMol((strcGlobalParameters *)glbParam);
}

CLidar_Operations::~CLidar_Operations()
{
  delete[] dummy;
  delete[] pr_misc;
  delete[] pr2_i;
  delete[] prS;
  delete[] errRMS_Bias;
  delete[] b_i;
  delete[] coeff;
}

void CLidar_Operations::BiasCorrection(double *pr, strcGlobalParameters *glbParam, strcMolecularData *dataMol) 
{
  if ((strcmp(glbParam->BkgCorrMethod, "MEAN") == 0) || (strcmp(glbParam->BkgCorrMethod, "mean") == 0))
    Bias_Substraction_Mean((double *)pr, (strcMolecularData *)dataMol, (strcGlobalParameters *)glbParam, (double *)pr);

  else if ((strcmp(glbParam->BkgCorrMethod, "FIT") == 0) || (strcmp(glbParam->BkgCorrMethod, "fit") == 0))
    Bias_Substraction_MolFit((strcMolecularData *)dataMol, (const double *)pr, (strcGlobalParameters *)glbParam, (double *)pr);

  else if ((strcmp(glbParam->BkgCorrMethod, "AUTO") == 0) || (strcmp(glbParam->BkgCorrMethod, "auto") == 0)) 
  {
    double Bias_Pr;
    Bias_Substraction_Auto((double *)pr, (strcMolecularData *)dataMol, (strcGlobalParameters *)glbParam, (double *)pr, (double *)&Bias_Pr);
  }
  else if ((strcmp(glbParam->BkgCorrMethod, "PRE_TRIGGER") == 0) || (strcmp(glbParam->BkgCorrMethod, "pre_trigger") == 0)) 
  {
    if (glbParam->is_Dark_Current_Loaded == true)
      Bias_Substraction_Pre_Trigger( (double *)pr, (strcGlobalParameters *)glbParam, (double *)pr);
    else
      Bias_Substraction_Pre_Trigger( (double *)pr, (strcGlobalParameters *)glbParam, (double *)pr);
  }
  else 
  {
    printf("\n CLidar_Operations::BiasCorrection(): BkgCorrMethod = %s but MEAN method is used.' \n", glbParam->BkgCorrMethod);
    Bias_Substraction_Mean((double *)pr, (strcMolecularData *)dataMol, (strcGlobalParameters *)glbParam, (double *)pr);
  }
}

void CLidar_Operations::Bias_Substraction_Auto(double *pr, strcMolecularData *dataMol, strcGlobalParameters *glbParam, double *pr_noBias, double *Bias_Pr) 
{
  if (glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] < 0)
    Find_Max_Mol_Range((double *)pr, (strcMolecularData *)dataMol,(strcGlobalParameters *)glbParam,(int)avg_Points_Cloud_Mask);

  if ((dataMol->last_Indx_Bkg_Low[glbParam->evSel][glbParam->chSel] > 0) &&
      (dataMol->last_Indx_Bkg_High[glbParam->evSel][glbParam->chSel] > 0) &&
      (dataMol->last_Indx_Mol_Low[glbParam->evSel][glbParam->chSel] > 0) &&
      (dataMol->last_Indx_Mol_High[glbParam->evSel][glbParam->chSel] > 0))
  {
    // FIRST BIAS GUESS OBTAINED FROM THE MEAN IN THE RANGES
    // dataMol->last_Indx_Bkg_Low - dataMol->last_Indx_Bkg_High
    // AND dataMol->last_Indx_Mol_Low - dataMol->last_Indx_Mol_High ARE USED FOR
    // THE FITTING.
    *Bias_Pr = 0.0;
    for (int j = dataMol->last_Indx_Bkg_Low[glbParam->evSel][glbParam->chSel]; j <= dataMol->last_Indx_Bkg_High[glbParam->evSel][glbParam->chSel]; j++)
      *Bias_Pr = *Bias_Pr + pr[j];
    *Bias_Pr = *Bias_Pr / (dataMol->last_Indx_Bkg_High[glbParam->evSel][glbParam->chSel] - dataMol->last_Indx_Bkg_Low[glbParam->evSel][glbParam->chSel] + 1);
    
    printf("\n CLidar_Operations::Bias_Substraction_Auto() ==> MEAN METHOD IS " "USED ==> bias= %lf.\n", *Bias_Pr);
    /*
                    double 	b_ref_max = 0.0 ;
                    double 	b_ref_min = 0.0 ;
                    double 	b_step    = 0.0 ;

                    fitParam.indxInitFit = dataMol->last_Indx_Mol_Low  ;
                    fitParam.indxEndFit  = dataMol->last_Indx_Mol_High ;
                    fitParam.nFit	  	 = fitParam.indxEndFit -
       fitParam.indxInitFit +1 ; b_ref_max = 1.5* *Bias_Pr ; b_ref_min = 0.5*
       *Bias_Pr ; b_step	  = (b_ref_max - b_ref_min) /nBiasRes_Auto ;

                    for ( int s =0; s <nBiasRes_Auto; s++ )
                            b_i[s] = (double) b_ref_min + s *b_step ;
                    b_i[nBiasRes_Auto] = 0 ; // IN CASE OF UNBIASED LIDAR SIGNAL

                    for( int l=0 ; l< nLoopFindBias ; l++ )
                    {
                            for (int b =0; b <nBiasRes_Auto; b++)
                            {
                                    for ( int i =0 ; i <glbParam->nBins ; i++ )
                                    {
                                            pr_misc[i]  = pr[i] - b_i[b] ;
                                            pr2_i[i]       = pr_misc[i] *
       pow(glbParam->r[i], 2) ;
                                    }
                                    Fit( (double*)pr2_i,
       (double*)dataMol->pr2Mol, glbParam->nBins , "wOutB", "NOTall",
       (strcFitParam*)&fitParam, (double*)dummy ) ; errRMS_Bias[b] =
       fitParam.squared_sum_fit ;
                            }
                                    polyfitCoeff( (const double* const) b_i
       , // X DATA (const double* const  ) errRMS_Bias		, // Y DATA
                                                            (unsigned int )
       nBiasRes_Auto	, (unsigned int		  ) 2 , (double*
       ) coeff	 ) 		; *Bias_Pr = (double) -coeff[1] /(2*coeff[2]) ;

                                    b_ref_max = 1.5* (*Bias_Pr) ;
                                    b_ref_min = 0.5* (*Bias_Pr) ;
                                    b_step 	= (b_ref_max - b_ref_min)
       /nBiasRes_Auto ; for ( int b =0; b <nBiasRes_Auto; b++ ) b_i[b] =
       (double) b_ref_min + b *b_step ; } // for( int l=0 ; l< nLoopFindBias ;
       l++ )
    */
  } // if 	( 	(dataMol->last_Indx_Bkg_Low >0) &&
    // (dataMol->last_Indx_Bkg_High >0) &&
  else if ((dataMol->last_Indx_Bkg_Low[glbParam->evSel][glbParam->chSel] < 0) &&
           (dataMol->last_Indx_Bkg_High[glbParam->evSel][glbParam->chSel] < 0) &&
           (dataMol->last_Indx_Mol_Low[glbParam->evSel][glbParam->chSel] > 0) &&
           (dataMol->last_Indx_Mol_High[glbParam->evSel][glbParam->chSel] > 0))
  { // IF ONLY MOLECULAR PROFILES WERE DETECTED: THE BIAS IS
    // OBTAINED FROM THE PRE-TRIGGER OR RAYLEIGH FIT
    if (glbParam->indxOffset[glbParam->chSel] > 30)
    {
      *Bias_Pr = bias_pre_trigger;
      printf("\n CLidar_Operations::Bias_Substraction_Auto() ==> PRE-TRIGGER METHOD IS USED FOR BIAS CORRECTION ==> bias= %lf.\n", *Bias_Pr);
    }
    else
    {
      fitParam.indxInitFit = dataMol->last_Indx_Mol_Low[glbParam->evSel][glbParam->chSel];
      fitParam.indxEndFit = dataMol->last_Indx_Mol_High[glbParam->evSel][glbParam->chSel];
      fitParam.nFit = fitParam.indxEndFit - fitParam.indxInitFit + 1;

      Fit((double *)pr, (double *)dataMol->pr2Mol, glbParam->nBins, "wOutB", "NOTall", (strcFitParam *)&fitParam, (double *)dummy);
      *Bias_Pr = fitParam.b;
      printf("\n CLidar_Operations::Bias_Substraction_Auto() ==> RAYLEIGH-FIT METHOD IS USED FOR BIAS DETECTION ==> bias= %lf.\n", *Bias_Pr);
    }
  } else { // LIDAR SIGNAL CORRUPTED, ALL DATA FILLED WITH ZEROS
    *Bias_Pr = 0.0;
    printf("\n CLidar_Operations::Bias_Substraction_Auto() - NO BIAS IS "
           "SUBSTRACTED, POSSIBLE DAMAGED SIGNAL.\n");
  }

  for (int i = 0; i < glbParam->nBins_Ch_eff[glbParam->chSel]; i++)
    pr_noBias[i] = (pr[i] - *Bias_Pr);
}

// void CLidar_Operations::Bias_Substraction_Auto( double *pr, strcMolecularData
// *dataMol, strcGlobalParameters *glbParam, double *pr_noBias, double *Bias_Pr
// )
// {
// 	Find_Max_Range( (double*)pr, (strcMolecularData*)dataMol,
// (strcGlobalParameters*)glbParam) ;

// 	// printf( "\nBias_Substraction_Auto() ====> \n
// dataMol->last_Indx_Mol_Low[m]= %lf\n dataMol->last_Indx_Mol_High[m]= %lf\n
// dataMol->last_Indx_Bkg_Low[m]= %lf\n dataMol->last_Indx_Bkg_High[m]= %lf\n",
// 	//
// dataMol->last_Indx_Mol_Low *glbParam->dzr, dataMol->last_Indx_Mol_High
// *glbParam->dzr, dataMol->last_Indx_Bkg_Low *glbParam->dzr,
// dataMol->last_Indx_Bkg_High *glbParam->dzr ) ;

// 	double 	b_ref_max = 0.0 ;
// 	double 	b_ref_min = 0.0 ;
// 	double 	b_step    = 0.0 ;

// 	if ( (dataMol->last_Indx_Bkg_Low >0) && (dataMol->last_Indx_Bkg_High >0)
// ) // && (dataMol->last_Indx_Bkg_Low >0) && (dataMol->last_Indx_Bkg_High >0)
// 	{	// FIRST BIAS GUESS OBTAINED FROM THE MEAN VALUE OF THE LAST
// BINS 		fitParam.indxInitFit = dataMol->last_Indx_Bkg_Low  ; 		fitParam.indxEndFit
// = dataMol->last_Indx_Bkg_High ; 		fitParam.nFit	  	 =
// fitParam.indxEndFit - fitParam.indxInitFit +1 ;
// 	}
// 	else if( (dataMol->last_Indx_Mol_Low >0) && (dataMol->last_Indx_Mol_High
// >0) ) 	{	// FIRST BIAS GUESS OBTAINED FROM THE FIT IN THE MOLECULAR
// RANGES 		fitParam.indxInitFit = dataMol->last_Indx_Mol_Low  ;
// 		fitParam.indxEndFit  = dataMol->last_Indx_Mol_High ;
// 		fitParam.nFit	  	 = fitParam.indxEndFit -
// fitParam.indxInitFit +1 ;
// 	}
// 	else
// 	{	// LIDAR SIGNAL CORRUPTED, ALL DATA FILLED WITH ZEROS
// 		*Bias_Pr = 0.0 	;
// 		printf("\n CLidar_Operations::Bias_Substraction_Auto() - NO BIAS
// IS SUBSTRACTED, POSSIBLE DAMAGED SIGNAL.\n") ; 		goto 	NO_BIAS	;
// 	}

// 		Fit( (double*)pr, (double*)dataMol->prMol, dataMol->nBins ,
// "wB", "NOTall", (strcFitParam*)&fitParam, (double*)dummy ) ;

// 		b_ref_max = 1.5* fitParam.b ;
// 		b_ref_min = 0.5* fitParam.b ;
// 		b_step	  = (b_ref_max - b_ref_min) /nBiasRes_Auto ;

// 		for ( int s =0; s <nBiasRes_Auto; s++ )
// 			b_i[s] = (double) b_ref_min + s *b_step ;
// 		b_i[nBiasRes_Auto] = 0 ; // IN CASE OF UNBIASED LIDAR SIGNAL

// 		for( int l=0 ; l< nLoopFindBias ; l++ ) // nLoopFindBias = 5 3
// 		{
// 			for (int b =0; b <nBiasRes_Auto; b++)
// 			{
// 				for ( int i =0 ; i <glbParam->nBins ; i++ )
// 				{
// 					pr_misc[i]  = pr[i] - b_i[b] ;
// 					pr2_i[i]       = pr_misc[i] *
// pow(glbParam->r[i], 2) ;
// 				}
// 				Fit( (double*)pr2_i, (double*)dataMol->pr2Mol,
// glbParam->nBins , "wOutB", "NOTall", (strcFitParam*)&fitParam, (double*)dummy
// ) ; 				errRMS_Bias[b] = fitParam.squared_sum_fit ;
// 			}
// 				polyfitCoeff( (const double* const) b_i
// , // X DATA 							(const double* const  ) errRMS_Bias		, // Y DATA
// 							(unsigned int         )
// nBiasRes_Auto	, 							(unsigned int		  ) 2 , 							(double*
// ) coeff	 ) 		; 				*Bias_Pr = (double) -coeff[1] /(2*coeff[2]) ;

// 				b_ref_max = 1.5* (*Bias_Pr) ;
// 				b_ref_min = 0.5* (*Bias_Pr) ;
// 				b_step 	= (b_ref_max - b_ref_min) /nBiasRes_Auto
// ; 				for ( int b =0; b <nBiasRes_Auto; b++ ) 					b_i[b] = (double) b_ref_min + b
// *b_step ;
// 		}

// NO_BIAS:
// 	for ( int i =0; i <glbParam->nBins ; i++)
// 		pr_noBias[i] = (pr[i] - *Bias_Pr);

// 	// } // if ( indxMaxRange <= (glbParam->nBins-1) )
// 	// else
// 	// {
// 	// 	printf("\n CLidar_Operations::Bias_Substraction_Auto()
// (glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] >
// (glbParam->nBins-1)) - Background substraction metod set is %s, but MEAN
// method is applied because nBins= %d and indxMaxRange= %d).\n",
// glbParam->BkgCorrMethod, glbParam->nBins,
// glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] ) ;
// 	// 	Bias_Substraction_Mean( (double*)pr,
// (strcMolecularData*)dataMol, (strcGlobalParameters*)glbParam,
// (double*)pr_noBias) ;
// 	// }
// }

void CLidar_Operations::Find_Max_Range(double *pr, strcMolecularData *dataMol,
                                       strcGlobalParameters *glbParam) {
  dataMol->last_Indx_Mol_Low[glbParam->evSel][glbParam->chSel] = -1;
  dataMol->last_Indx_Mol_High[glbParam->evSel][glbParam->chSel] = -1;
  dataMol->last_Indx_Bkg_Low[glbParam->evSel][glbParam->chSel] = -1;
  dataMol->last_Indx_Bkg_High[glbParam->evSel][glbParam->chSel] = -1;

  double R2_Max_Range = 0.3; // winzize = 3000 m --> este es el valor que se usa
                             // en el programa de matlab para SNR=1
  int winSize = (int)round(3000 / glbParam->dr);

  int indxInit_Search;
  if (dataMol->z_Mol_Max > glbParam->r[glbParam->nBins_Ch[glbParam->chSel]])
    indxInit_Search = glbParam->nBins_Ch[glbParam->chSel] - 1;
  else
    indxInit_Search = round(dataMol->z_Mol_Max / glbParam->dr);

  indxInit_Search = round(25000 / glbParam->dr);
  printf("\nFind_Max_Range() ==> indxInit_Search= %d \n", indxInit_Search);

  for (int i = indxInit_Search; i >= (glbParam->indxInitSig + winSize); i--) {
    fitParam.indxEndFit = i;
    fitParam.indxInitFit = fitParam.indxEndFit - winSize;
    fitParam.nFit = fitParam.indxEndFit - fitParam.indxInitFit + 1;
    fitParam.indxMidFit =
        (int)round((fitParam.indxInitFit + fitParam.indxEndFit) / 2);

    memset((double *)dummy, 0,
           sizeof(double) * glbParam->nBins_Ch[glbParam->chSel]);
    RayleighFit((double *)pr, (double *)dataMol->prMol,
                glbParam->nBins_Ch[glbParam->chSel], (strcFitParam *)&fitParam,
                (double *)dummy);
    // Fit( (double*)pr, (double*)dataMol->prMol,
    // glbParam->nBins_Ch[glbParam->chSel] , "wB", "NOTall",
    // (strcFitParam*)&fitParam, (double*)dummy ) ; if ( i==
    // (glbParam->nBins_Ch[glbParam->chSel] -1) ) 	printf("\n Find_Max_Range(1)
    // --> fitParam.indxInitFit= %d  fitParam.indxEndFit= %d  fitParam.R2= %lf",
    // fitParam.indxInitFit, fitParam.indxEndFit, fitParam.R2 ) ;

    // if ( (fitParam.R2 >R2_NO_Signal) && (dataMol->last_Indx_Bkg_Low <0) &&
    // (fitParam.m >0) )
    // {
    // 	count_R2_NO_Signal++ ;
    // 	if ( count_R2_NO_Signal ==1 )
    // 	{
    // 		dataMol->last_Indx_Bkg_Low  = fitParam.indxInitFit
    // ; 		dataMol->last_Indx_Bkg_High = glbParam->nBins_Ch[glbParam->chSel] -1 ;
    // 		// printf("\n Find_Max_Range(2) setting
    // dataMol->last_Indx_Bkg_Low -> fitParam.indxInitFit= %d
    // fitParam.indxEndFit= %d  fitParam.R2= %lf", fitParam.indxInitFit,
    // fitParam.indxEndFit, fitParam.R2 ) ;
    // 	}
    // }

    if ((fitParam.R2_adj > R2_Max_Range) &&
        (dataMol->last_Indx_Mol_Low[glbParam->evSel][glbParam->chSel] < 0) &&
        (fitParam.m > 0)) {
      dataMol->last_Indx_Mol_Low[glbParam->evSel][glbParam->chSel] =
          fitParam.indxInitFit;
      dataMol->last_Indx_Mol_High[glbParam->evSel][glbParam->chSel] =
          fitParam.indxEndFit;
      dataMol->last_Indx_Bkg_Low[glbParam->evSel][glbParam->chSel] =
          round((dataMol->last_Indx_Mol_High[glbParam->evSel][glbParam->chSel] +
                 glbParam->nBins_Ch[glbParam->chSel] -
                 glbParam->indxOffset[glbParam->chSel]) /
                2);
      dataMol->last_Indx_Bkg_High[glbParam->evSel][glbParam->chSel] =
          glbParam->nBins_Ch[glbParam->chSel] - 1 -
          glbParam->indxOffset[glbParam->chSel];
      // printf("\n Find_Max_Range(3) setting dataMol->last_Indx_Mol_Low ->
      // fitParam.indxInitFit= %d  fitParam.indxEndFit= %d  fitParam.R2= %lf",
      // fitParam.indxInitFit, fitParam.indxEndFit, fitParam.R2 ) ;

      // IF THE BACKGROUND RANGE IS TOO SMALL, THEN IT IS NOT CONSIDERED.
      // THIS HAPPENS WHEN THE MOLECULAR SIGNAL IS TOO CLOSE TO THE END OF THE
      // RANGE AND NO CONSTANT BACKGROUND IS FOUND.
      if ((dataMol->last_Indx_Bkg_High[glbParam->evSel][glbParam->chSel] -
           dataMol->last_Indx_Bkg_Low[glbParam->evSel][glbParam->chSel]) <
          winSize) {
        dataMol->last_Indx_Bkg_Low[glbParam->evSel][glbParam->chSel] = -1;
        dataMol->last_Indx_Bkg_High[glbParam->evSel][glbParam->chSel] = -1;
      }
    }

    // if ( (dataMol->last_Indx_Mol_Low == dataMol->last_Indx_Bkg_Low) &&
    // (dataMol->last_Indx_Mol_Low >0) ) {   // IF THE FIRST FIT PRODUCE A R2
    // >R2_Max_Range (ALSO R2 >R2_NO_Signal) ==> IT ALREADY WENT INTO THE LAST
    // TWO if()... 	dataMol->last_Indx_Mol_Low  = fitParam.indxInitFit 	;
    // 	dataMol->last_Indx_Mol_High = fitParam.indxEndFit  	;
    // 	dataMol->last_Indx_Bkg_Low  = -1 ; 	dataMol->last_Indx_Bkg_High = -1
    // ;
    // 	// printf("\n Find_Max_Range(4) -> dataMol->last_Indx_Mol_Low ==
    // dataMol->last_Indx_Bkg_Low \n") ;
    // }

    if (dataMol->last_Indx_Mol_Low[glbParam->evSel][glbParam->chSel] > 0) {
      // printf("\n Find_Max_Range(5) ==> exiting with fitparam.R2_adj= %lf --
      // Max Range: %lf \n", fitParam.R2_adj, dataMol->last_Indx_Mol_High
      // *glbParam->dzr ) ;
      break;
    }
  } // for ( int i = indxInit_Search ; i >=(glbParam->indxInitSig +winSize) ;
    // i-- )

  // if (dataMol->last_Indx_Mol_Low <round(glbParam->indxInitSig /glbParam->dzr)
  // ) 	dataMol->last_Indx_Mol_Low = *indxMaxRange ;

  // if( (dataMol->last_Indx_Mol_Low >0) && (dataMol->last_Indx_Mol_High>0) )
  // {
  // 	Remove_Cloud_Mol_Range( (double*)pr, (strcGlobalParameters*)glbParam,
  // (strcMolecularData*)dataMol ) ;

  // 	dataMol->last_Indx_Mol_High = dataMol->last_Indx_Mol_Low + winSize ; 	if
  // ( dataMol->last_Indx_Mol_High > (glbParam->nBins_Ch[glbParam->chSel] -1) )
  // 		dataMol->last_Indx_Mol_High =
  // glbParam->nBins_Ch[glbParam->chSel] -1 ;
  // }

  glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] =
      dataMol->last_Indx_Mol_High[glbParam->evSel][glbParam->chSel];
  glbParam->rEndSig_ev_ch[glbParam->evSel][glbParam->chSel] =
      glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] *
      glbParam->dr;

  printf(
      "CLidar_Operations::Find_Max_Range() --> ev: %d \n\t\t\t "
      "dataMol->last_Indx_Mol_Low[m]= %lf \t dataMol->last_Indx_Mol_High[m]= "
      "%lf \n\t\t\t dataMol->last_Indx_Bkg_Low[m]= %lf \t "
      "dataMol->last_Indx_Bkg_High[m]= %lf \n",
      glbParam->evSel,
      dataMol->last_Indx_Mol_Low[glbParam->evSel][glbParam->chSel] *
          glbParam->dzr,
      dataMol->last_Indx_Mol_High[glbParam->evSel][glbParam->chSel] *
          glbParam->dzr,
      dataMol->last_Indx_Bkg_Low[glbParam->evSel][glbParam->chSel] *
          glbParam->dzr,
      dataMol->last_Indx_Bkg_High[glbParam->evSel][glbParam->chSel] *
          glbParam->dzr);
} // void CLidar_Operations::Find_Max_Range( double *pr, double *prMol,
  // strcGlobalParameters *glbParam )

void CLidar_Operations::Find_Max_Mol_Range(double *pr,
                                           strcMolecularData *dataMol,
                                           strcGlobalParameters *glbParam,
                                           int avg_points) {
  dataMol->last_Indx_Mol_Low[glbParam->evSel][glbParam->chSel] = (int)-1;
  dataMol->last_Indx_Mol_High[glbParam->evSel][glbParam->chSel] = (int)-1;
  dataMol->last_Range_Mol_Low[glbParam->evSel][glbParam->chSel] = (double)-1.0;
  dataMol->last_Range_Mol_High[glbParam->evSel][glbParam->chSel] = (double)-1.0;

  // IF THE BACKGROUND RANGES ARE SET IN AUTOMATIC MODE: RESET THE VARIABLES
  if ((glbParam->range_Bkg_Start < 0) || (glbParam->range_Bkg_Stop < 0) ||
      (glbParam->range_Bkg_Start >
       glbParam->range_Bkg_Stop)) { // BACKGROUND RANGES SET IN AUTOMATIC MODE
                                    // AND MUST BE FILLED IN THIS METHOD
    dataMol->last_Indx_Bkg_Low[glbParam->evSel][glbParam->chSel] = (int)-1;
    dataMol->last_Indx_Bkg_High[glbParam->evSel][glbParam->chSel] = (int)-1;
    dataMol->last_Range_Bkg_Low[glbParam->evSel][glbParam->chSel] =
        (double)-1.0;
    dataMol->last_Range_Bkg_High[glbParam->evSel][glbParam->chSel] =
        (double)-1.0;
  } else { // BACKGROUND RANGES SET IN FIXED MODE. SET THE VALUES FROM THE
           // CONFIGURATION FILE (ALREADY LOADED IN glbParam)
    dataMol->last_Range_Bkg_Low[glbParam->evSel][glbParam->chSel] =
        (double)glbParam->range_Bkg_Start;
    dataMol->last_Range_Bkg_High[glbParam->evSel][glbParam->chSel] =
        (double)glbParam->range_Bkg_Stop;
  }

  double bkg_mean = (double)0.0;
  double bkg_std = (double)0.0;

  // CHECK IF THE SIGNAL IS ALREADY BACKGROUND CORRECTED
  bkg_mean = (double)0.0;
  bkg_std = (double)0.0;
  // MEAN VALUE OF THE LAST 100 BINS
  for (int j = (glbParam->nBins_Ch_eff[glbParam->chSel] - 100);
       j <= glbParam->nBins_Ch_eff[glbParam->chSel]; j++)
    bkg_mean = bkg_mean + pr[j];
  bkg_mean = bkg_mean / 100;
  // STANDARD DEVIATION OF THE LAST 100 BINS
  for (int j = (glbParam->nBins_Ch_eff[glbParam->chSel] - 100);
       j <= glbParam->nBins_Ch_eff[glbParam->chSel]; j++)
    bkg_std = bkg_std + (pr[j] - bkg_mean) * (pr[j] - bkg_mean);
  bkg_std = bkg_std / 100;
  bkg_std = sqrt(bkg_std);
  // CHECK IF THE BACKGROUND HAS BEEN ALREADY CORRECTED
  memset((double *)pr_misc, 0,
         sizeof(double) * glbParam->nBins_Ch[glbParam->chSel]);
  // if ( ((bkg_mean+bkg_std) >0) && ((bkg_mean-bkg_std) <0) ) // BACKGROUND
  // ALREADY CORRECTED 	for ( int i=0 ; i<glbParam->nBins_Ch_eff[glbParam->chSel]
  // ; i++ ) 	pr_misc[i] = (double) pr[i] ; else // BACKGROUND NOT CORRECTED
  for (int i = 0; i < glbParam->nBins_Ch_eff[glbParam->chSel]; i++)
    pr_misc[i] = (double)(pr[i] - bkg_mean);

  memset((double *)prS, 0,
         sizeof(double) * glbParam->nBins_Ch[glbParam->chSel]);
  if (avg_points > 1)
    smooth((double *)pr_misc, (int)0,
           (int)(glbParam->nBins_Ch_eff[glbParam->chSel] - 1), (int)avg_points,
           (double *)prS);
  else
    for (int i = 0; i < glbParam->nBins_Ch_eff[glbParam->chSel]; i++)
      prS[i] = (double)pr_misc[i];

  findIndxFirstNeg(
      (double *)prS, (int)glbParam->indxInitSig,
      (int)(glbParam->nBins_Ch_eff[glbParam->chSel] - 1),
      (int *)(&dataMol->last_Indx_Mol_High[glbParam->evSel][glbParam->chSel]),
      (double *)&bkg_mean);
  glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] =
      dataMol->last_Indx_Mol_High[glbParam->evSel][glbParam->chSel];
  glbParam->rEndSig_ev_ch[glbParam->evSel][glbParam->chSel] =
      glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] *
      glbParam->dr;

  dataMol->last_Indx_Mol_Low[glbParam->evSel][glbParam->chSel] =
      (int)dataMol->last_Indx_Mol_High[glbParam->evSel][glbParam->chSel] -
      (int)round(1000 / glbParam->dr);
  dataMol->last_Range_Mol_High[glbParam->evSel][glbParam->chSel] =
      (double)(dataMol->last_Indx_Mol_High[glbParam->evSel][glbParam->chSel] *
               glbParam->dzr);

  if ((glbParam->range_Bkg_Start < 0) || (glbParam->range_Bkg_Stop < 0) ||
      (glbParam->range_Bkg_Start >
       glbParam->range_Bkg_Stop)) { // IF THE MAXIMUN RANGE IS SET AUTOMATICALLY
                                    // ---> SET THE BACKGROUND LIMITS
    // CHECK THE ROOM BETWEEN NUMBERS OF BINS AND last_Indx_Mol_High
    if ((glbParam->nBins_Ch_eff[glbParam->chSel] -
         dataMol->last_Indx_Mol_High[glbParam->evSel][glbParam->chSel]) > 100) {
      glbParam->nBinsBkg =
          (glbParam->nBins_Ch_eff[glbParam->chSel] -
           dataMol->last_Indx_Mol_High[glbParam->evSel][glbParam->chSel]) /
          2;
      dataMol->last_Indx_Bkg_High[glbParam->evSel][glbParam->chSel] =
          glbParam->nBins_Ch_eff[glbParam->chSel] - 1;
      dataMol->last_Indx_Bkg_Low[glbParam->evSel][glbParam->chSel] =
          dataMol->last_Indx_Bkg_High[glbParam->evSel][glbParam->chSel] -
          glbParam->nBinsBkg;
      dataMol->last_Range_Bkg_High[glbParam->evSel][glbParam->chSel] =
          dataMol->last_Indx_Bkg_High[glbParam->evSel][glbParam->chSel] *
          glbParam->dzr;
      dataMol->last_Range_Bkg_Low[glbParam->evSel][glbParam->chSel] =
          dataMol->last_Indx_Bkg_Low[glbParam->evSel][glbParam->chSel] *
          glbParam->dzr;
      // printf( "\n CLidar_Operations::Find_Max_Mol_Range() (%lf)==>
      // last_Range_Bkg_Low= %lf  \t last_Range_Bkg_High= %lf\n",
      // 								dataMol->last_Range_Mol_High[glbParam->evSel][glbParam->chSel],
      // 								dataMol->last_Range_Bkg_Low[glbParam->evSel][glbParam->chSel],
      // dataMol->last_Range_Bkg_High[glbParam->evSel][glbParam->chSel] ) ;
    } else {
      // glbParam->nBinsBkg = 100 ;
      dataMol->last_Indx_Bkg_High[glbParam->evSel][glbParam->chSel] = -10;
      dataMol->last_Indx_Bkg_Low[glbParam->evSel][glbParam->chSel] = -10;
      dataMol->last_Range_Bkg_High[glbParam->evSel][glbParam->chSel] = -10;
      dataMol->last_Range_Bkg_Low[glbParam->evSel][glbParam->chSel] = -10;
      // printf( "\n CLidar_Operations::Find_Max_Mol_Range() ==> WARNING!!!:
      // Modulated lidar signal at the end of the range. MEAN method for
      // background correction is blocked." ) ;
    }
  } // if ( (glbParam->range_Bkg_Start <0) || (glbParam->range_Bkg_Stop <0) || (
    // glbParam->range_Bkg_Start > glbParam->range_Bkg_Stop ) )

  // printf( "\n CLidar_Operations::Find_Max_Mol_Range() ==>
  // rEndSig_ev_ch[%d][%d]= %lf", glbParam->evSel, glbParam->chSel,
  // glbParam->rEndSig_ev_ch[glbParam->evSel][glbParam->chSel] ) ; printf( "\n
  // CLidar_Operations::Find_Max_Mol_Range() ==> (%d)
  // dataMol->last_Indx_Mol_High*dr= %lf \n", glbParam->chSel ,
  // dataMol->last_Indx_Mol_High[glbParam->evSel][glbParam->chSel]* glbParam->dr
  // ) ;
} // void CLidar_Operations::Find_Max_Mol_Range( double *pr, strcMolecularData
  // *dataMol, strcGlobalParameters *glbParam )

void CLidar_Operations::Find_Ref_Range( double *pr2_i, strcGlobalParameters *glbParam, strcMolecularData *dataMol, int *indxRef_Fernald_Start, int *indxRef_Fernald_Stop) 
{
  nWinSize = 0 ;
  for( int i=glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] ; i >glbParam->indxInitSig ; i--)
  { // SEARCH FOR THE TOP OF THE HIGHEST CLOUD
    if ( cloudProfiles[glbParam->evSel].clouds_ON[i] == BIN_CLOUD )
    {
      nWinSize = (int) round( (glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] - i) /3 ) ;
      break ;
    }
  }
  if ( nWinSize == 0 )
    nWinSize = (int) round( (glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] - glbParam->indxInitSig) /3 ) ;
  if ( nWinSize == 0 )
  {
    nWinSize = (int) 100 ;
    printf("\n\033[31mFind_Ref_Range --> WARNING: Window size forced to %d. Problems could happen during the procedure... \nCheck in the NetCDF file the health of the lidar signals (analog and photon-counting channels)\033[0m\n\n", nWinSize) ;
  }

  int   nSteps   = glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] - nWinSize +1 ;
  strcFitParam	fitParam  ; 
  double		*stdPr2	    		= (double*) new double  [nSteps] 	        ; for (int i = 0; i < nSteps; i++)  stdPr2[i] = DBL_MAX;
  memset( (double*)pr_misc, 0, sizeof(double)*glbParam->nBins) ;

  // FIND THE LOWEST MOLECULAR RANGES USING A WINDOWS OF nWinSize POINTS
  int indxMin_Std, minStd ;
  for( int i=glbParam->indxInitSig ; i <nSteps ; i++ )
  {
    fitParam.indxInitFit = i ;
    fitParam.indxEndFit  = fitParam.indxInitFit + nWinSize ;
    fitParam.nFit	    	 = fitParam.indxEndFit  - fitParam.indxInitFit +1 ;

      RayleighFit ( (double*)&pr2_i[0], (double*)&dataMol->pr2Mol_avg[0], fitParam.nFit, (strcFitParam*)&fitParam, (double*)pr_misc ) ;
      stdPr2[i] = fitParam.std ;
  }
  findIndxMin( (double*)stdPr2, (int)0, (int)(nSteps -1), (int*)&indxMin_Std, (double*)(&minStd) ) ;
  
  *indxRef_Fernald_Start = indxMin_Std + (int) round( (glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel]-indxMin_Std)/4.0 ) ;
  *indxRef_Fernald_Stop  = *indxRef_Fernald_Start + nWinSize ;
}

void CLidar_Operations::Layer_Mask(const double *pr, strcMolecularData *dataMol,  strcGlobalParameters *glbParam) 
{
  // fitParam.indxInitFit = (int)round((glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] + glbParam->nBins_Ch[glbParam->chSel]) /2);
  // fitParam.indxEndFit  = glbParam->nBins_Ch[glbParam->chSel] - 1  ;
  // fitParam.nFit = fitParam.indxEndFit - fitParam.indxInitFit + 1 ;
  // double  rayFit_gap = 0.0 ;
  // int     indxMax = 0 ;
  // findIndxMax( (double*)&pr[0], (int)fitParam.indxInitFit, (int)fitParam.indxEndFit, (int*)&indxMax, (double*)&rayFit_gap ) ;

  // pr IS ALREADY BACKGROUND CORRECTED
  if (avg_Points_Cloud_Mask > 1) 
  {
    smooth((double *)pr             , (int)0, (int)(glbParam->nBins - 1), (int)avg_Points_Cloud_Mask, (double *)prS);
    smooth((double *)glbParam->r    , (int)0, (int)(glbParam->nBins - 1), (int)avg_Points_Cloud_Mask, (double *)glbParam->r_avg);
    smooth((double *)dataMol->prMol , (int)0, (int)(glbParam->nBins - 1), (int)avg_Points_Cloud_Mask, (double *)dataMol->prMol_avg);
    smooth((double *)dataMol->pr2Mol, (int)0, (int)(glbParam->nBins - 1), (int)avg_Points_Cloud_Mask, (double *)dataMol->pr2Mol_avg);
  }
  else
  {
    for (int b = 0; b < glbParam->nBins; b++) 
    {
      prS[b]                 = (double)pr[b];
      dataMol->prMol_avg[b]  = dataMol->prMol[b];
      dataMol->pr2Mol_avg[b] = dataMol->pr2Mol[b];
      glbParam->r_avg[b]     = glbParam->r[b];
    }
  }
 
  if (glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] <= 0) 
  {
    printf("\nLayer_Mask(): Finding last molecular ranges\n");
    Find_Max_Mol_Range((double *)prS, (strcMolecularData *)dataMol, (strcGlobalParameters *)glbParam, (int)0);
  }
  // GET THE REFERENCE-STANDARD DEVIATION FOR THE SIGNAL
  fitParam.indxInitFit = (int)round((glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] + glbParam->nBins_Ch[glbParam->chSel]) /2);
  fitParam.indxEndFit  = glbParam->nBins_Ch[glbParam->chSel] - 1  ;
  fitParam.nFit = fitParam.indxEndFit - fitParam.indxInitFit + 1 ;
  // for (int j = fitParam.indxInitFit; j <= fitParam.indxEndFit; j++)
  //   fitParam.std += pow(prS[j], 2); // prS IS ALREADY BACGROUND CORRECTED THATS WHY THE MEAN IS NOT SUBSTRACTED TO THE prS.
  // fitParam.std = sqrt(fitParam.std / (fitParam.nFit - 1));

  double  rayFit_gap = 0.0 ;
  int     indxMax = 0 ;
  findIndxMax( (double*)&prS[0], (int)fitParam.indxInitFit, (int)fitParam.indxEndFit, (int*)&indxMax, (double*)&rayFit_gap ) ;
  // rayFit_gap = ( rayFit_gap + fitParam.std ) /2 ;

  // SET THE INITIAL FITTING RANGES FOR THE LAYER DETECTION ALGORITHM
  fitParam.indxEndFit  = glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] - deltaNorm;
  fitParam.indxInitFit = fitParam.indxEndFit - deltaNorm;
  fitParam.indxMidFit  = (int)round((fitParam.indxInitFit + fitParam.indxEndFit) / 2);
  fitParam.nFit        = fitParam.indxEndFit - fitParam.indxInitFit + 1;
  memset(cloudProfiles[glbParam->evSel].clouds_ON, 0, (sizeof(int) * glbParam->nBins));
  while (fitParam.indxInitFit >= glbParam->indxInitSig)
  {
    memset((double*)pr_misc, 0, sizeof(double)*glbParam->nBins);
    RayleighFit((double*)prS, (double*)dataMol->prMol_avg, glbParam->nBins, (strcFitParam*)&fitParam, (double*)pr_misc);

    for (int j = fitParam.indxInitFit; j < glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel]; j++) 
    {
      if ( prS[j] > (pr_misc[j] + rayFit_gap * std_factor_layer_mask) )
        cloudProfiles[glbParam->evSel].clouds_ON[j] = (int)BIN_CLOUD;
    }
    fitParam.indxEndFit  = fitParam.indxEndFit - stepScanCloud; // glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel]// - deltaNorm -i*stepScanCloud ;
    fitParam.indxInitFit = fitParam.indxEndFit - deltaNorm; // DELTA NOMALIZATION STABLISHED
    fitParam.indxMidFit  = (int)round((fitParam.indxInitFit + fitParam.indxEndFit) / 2);
    fitParam.nFit = fitParam.indxEndFit - fitParam.indxInitFit + 1;
    FilterThinClouds((strcGlobalParameters *)glbParam, (int *)cloudProfiles[glbParam->evSel].clouds_ON);
  }

  GetCloudLimits((strcGlobalParameters *)glbParam); // ONLY LIMITS OF THE CLOUDS, *NOT* PBL!!!!

  //! PBL HEIGH - START
  if (strcmp(glbParam->exeFile, "./lpp1") == 0) 
  {
    compute_pbl_mask.assign("NO");
    // compute_pbl_mask.assign("YES");
  }
  else if (strcmp(glbParam->exeFile, "./lpp2") == 0) 
  {
    compute_pbl_mask.assign("YES");
    fitParam.indxInitFit = (int)-10;
    fitParam.indxEndFit = (int)-10;
    fitParam.indxMidFit = (int)-10;
    fitParam.nFit = fitParam.indxEndFit - fitParam.indxInitFit + 1;
  }

  if (strcmp(compute_pbl_mask.c_str(), "YES") == 0) 
  {
    for (int j = 0; j < glbParam->nBins_Ch[glbParam->chSel]; j++)
      pr2_i[j] = (double)prS[j] * glbParam->r[j] * glbParam->r[j]; // RANGE CORRECTED LIDAR SIGNAL

    Get_PBL_Mask( (double*)pr2_i, (strcGlobalParameters*)glbParam, (strcMolecularData*)dataMol, (double)rayFit_gap ) ;

    FilterThinClouds((strcGlobalParameters *)glbParam, (int *)cloudProfiles[glbParam->evSel].clouds_ON);
    GetCloudLimits((strcGlobalParameters *)glbParam);
  } // if ( strcmp( strCompPBL.c_str(), "YES" ) ==0 )
  //! PBL HEIGH - END
 } // void CDataLevel_1::Layer_Mask( const double *pr, strcMolecularData *dataMol, strcGlobalParameters *glbParam )

void CLidar_Operations::GetCloudLimits(strcGlobalParameters *glbParam) 
{
  int maxCloud, indxMax;
  findIndxMax_void( (int *)&cloudProfiles[glbParam->evSel].clouds_ON[0], (const char *)"int", 0, (glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel]), (int *)&indxMax, (int *)&maxCloud);
  // ( maxCloud == 1 ) ? printf("\nThere *ARE* clouds at event number %d",
  // glbParam->evSel) : printf("\nThere are *NOT* clouds at event number %d",
  // glbParam->evSel) ;

  // BASIC CLOUDLESS SETTINGS
  memset(cloudProfiles[glbParam->evSel].indxInitClouds, 0, (sizeof(int) * NMAXCLOUDS));
  memset(cloudProfiles[glbParam->evSel].indxEndClouds, 0, (sizeof(int) * NMAXCLOUDS));
  memset(indxMol[glbParam->evSel].indxInicMol, 0, (sizeof(int) * MAX_MOL_RANGES));
  memset(indxMol[glbParam->evSel].indxEndMol, 0, (sizeof(int) * MAX_MOL_RANGES));
  cloudProfiles[glbParam->evSel].nClouds = 0; // USED AS INDEX AND THEN, AS TOTAL NUMBER.
  indxMol[glbParam->evSel].nMolRanges = 0; // USED AS INDEX AND THEN, AS TOTAL NUMBER.

  int *dco = (int *)malloc(sizeof(int) * glbParam->nBins);
  if (maxCloud == 1) // IF THERE ARE CLOUDS IN THE EVENT UNDER ANALYSIS --> GETS THEIRS LIMITS
  {
    memset((int *)dco, 0, (sizeof(int) * glbParam->nBins));

    for (int i = 0; i <= glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel]; i++)
      dco[i] = (int)(cloudProfiles[glbParam->evSel].clouds_ON[i + 1] - cloudProfiles[glbParam->evSel].clouds_ON[i]);

    // CHECK CONSISTENCY --> nInicCloud = nEndCloud
    int nInicCloud = 0, nEndCloud = 0;
    for (int i = 0; i < glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] - 1; i++)
    {
      if (dco[i] == (int)BIN_CLOUD)
        nInicCloud++;
      else if (dco[i] == (int)(-BIN_CLOUD))
        nEndCloud++;
    }

    int s = 0; // CHECK IF PBL WAS DETECTED
    sum_int((int *)&cloudProfiles[glbParam->evSel].clouds_ON[0], 0, glbParam->indxInitSig, (int *)&s);
    if (s == (glbParam->indxInitSig + 1))
      nInicCloud++;

    // if ( nInicCloud == nEndCloud ) // IN CASE OF CLOUDY PROFILE --> GET THEIR
    // LIMITS
    if (true) // IN CASE OF CLOUDY PROFILE --> GET THEIR LIMITS
    {
      // CLOUDS DETECTION
      for (int i = 0; i <= (glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel]); i++) 
      {
        if (cloudProfiles[glbParam->evSel].nClouds < NMAXCLOUDS) //! NO TIENE SENTIDO ESTO ACA
        {
          if (dco[i] == (int)BIN_CLOUD) // INIT CLOUD
          {
            cloudProfiles[glbParam->evSel].indxInitClouds[cloudProfiles[glbParam->evSel].nClouds] = i; // USED AS INDEX (INITIALIZED WITH 0) AND THEN, AS TOTAL NUMBER.
            for (int j = i; j <=glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel]; j++) 
            {                        // SEARCH THE END OF THE CLOUD
              if (dco[j] == (int)(-BIN_CLOUD)) // END CLOUD
              {
                cloudProfiles[glbParam->evSel].indxEndClouds[cloudProfiles[glbParam->evSel].nClouds] = j - 1;
                cloudProfiles[glbParam->evSel].nClouds++; // COUNT CLOUDS
                i = j + 1;
                break;
              }
            }
          }
        }
      }
      indxMol[glbParam->evSel].nMolRanges = cloudProfiles[glbParam->evSel].nClouds + 1;
      // CLOUDS DETECTED --> MOLECULAR RANGES SETTINGS
      for (int i = 0; i < indxMol[glbParam->evSel].nMolRanges; i++) 
      {
        if (i == 0) 
        {
          indxMol[glbParam->evSel].indxInicMol[i] = 0;
          indxMol[glbParam->evSel].indxEndMol[i] = cloudProfiles[glbParam->evSel].indxInitClouds[i] - 1;
        } 
        else
        {
          indxMol[glbParam->evSel].indxInicMol[i] = cloudProfiles[glbParam->evSel].indxEndClouds[i - 1] + 1;
          if (i == (indxMol[glbParam->evSel].nMolRanges - 1))
            indxMol[glbParam->evSel].indxEndMol[i] = glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel];
          else
            indxMol[glbParam->evSel].indxEndMol[i] = cloudProfiles[glbParam->evSel].indxInitClouds[i] - 1;
        }
      }
    } // if ( nInicCloud == nEndCloud )
    else
    {
      printf("\033[31m\nGetCloudLim(...) --> Inconsistent cloud profile detected (nInicCloud: %d, nEndCloud: %d) \n\033[0m", nInicCloud, nEndCloud);
    }
  } // if ( maxCloud ==1 ) // IF THERE ARE CLOUDS --> GETS THEIRS LIMITS
  else // NO CLOUDS DETECTED
  {
    cloudProfiles[glbParam->evSel].nClouds = 0;
    indxMol[glbParam->evSel].nMolRanges = 1;

    // MOLECULAR RANGES SETTINGS
    if (indxMol[glbParam->evSel].nMolRanges == 1)
    {
      double heightRef_Inversion_ASL, heightRef_Inversion_Start_ASL, heightRef_Inversion_Stop_ASL ;
      ReadAnalysisParameter(glbParam->FILE_PARAMETERS, "heightRef_Inversion_Start_ASL", "double", (int *)&heightRef_Inversion_Start_ASL);
      ReadAnalysisParameter(glbParam->FILE_PARAMETERS, "heightRef_Inversion_Stop_ASL", "double", (int *)&heightRef_Inversion_Stop_ASL);
      heightRef_Inversion_ASL = (fabs(heightRef_Inversion_Start_ASL) + fabs(heightRef_Inversion_Stop_ASL)) /2;
 
      indxMol[glbParam->evSel].indxRef = (int)round(heightRef_Inversion_ASL / glbParam->dzr);
      if (indxMol[glbParam->evSel].indxRef > (glbParam->nBins - 1))
        indxMol[glbParam->evSel].indxRef = glbParam->nBins - 10;

      for (int i = 0; i < NMAXCLOUDS; i++) 
      {
        cloudProfiles[glbParam->evSel].indxInitClouds[i] = 0;
        cloudProfiles[glbParam->evSel].indxEndClouds[i] = 0;
        indxMol[glbParam->evSel].indxInicMol[i] = 0;
        indxMol[glbParam->evSel].indxEndMol[i] =
            glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel];
      }
    } // if ( indxMol[glbParam->evSel].nMolRanges ==1 )
  }

  //! SEARCH FOR THE END OF THE PBL (FIRST CLOUD-FREE BIN)
  for (int i = 0; i <= glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel]; i++)
  {
    if ( cloudProfiles[glbParam->evSel].clouds_ON[i] == (int)0 )
    {
      cloudProfiles[glbParam->evSel].indxEndPBL = (int)i;
      cloudProfiles[glbParam->evSel].rangeEndPBL = (double)i * glbParam->dr;
      break;
    }
  }

  // printf("\n\t CDataLevel_1::GetCloudLimits() " ) ;
  // printf("\n\t cloudProfiles[%d].nClouds: %d ", glbParam->evSel,
  // cloudProfiles[glbParam->evSel].nClouds ) ; printf("\n\t
  // cloudProfiles[%d].indxInitClouds[0]: %d ", glbParam->evSel,
  // cloudProfiles[glbParam->evSel].indxInitClouds[0] ) ; printf("\n\t
  // cloudProfiles[%d].indxEndClouds[0] : %d ", glbParam->evSel,
  // cloudProfiles[glbParam->evSel].indxEndClouds[0]  ) ; printf("\n\t
  // cloudProfiles[%d].indxEndPBL: %d "    , glbParam->evSel,
  // cloudProfiles[glbParam->evSel].indxEndPBL    ) ; printf("\n\t
  // cloudProfiles[%d].rangeEndPBL: %lf \n", glbParam->evSel,
  // cloudProfiles[glbParam->evSel].rangeEndPBL ) ;

  // printf("\n\t CORREGIR!! indxMol[%d].indxInicMol[0]: %d ", glbParam->evSel,
  // indxMol[glbParam->evSel].indxInicMol[0] ) ; printf("\n\t CORREGIR!!
  // indxMol[%d].indxEndMol [0]: %d ", glbParam->evSel,
  // indxMol[glbParam->evSel].indxEndMol[0]  ) ;
}

void CLidar_Operations::FilterThinClouds(strcGlobalParameters *glbParam, int *layer_mask) 
{
  for (int b = 0;
       b <= (glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] -
             CLOUD_MIN_THICK - 1);
       b++) {
    if ((layer_mask[b] == (int)0) && (layer_mask[b + 1] == (int)BIN_CLOUD)) {
      int sum_lay = 0;
      sum_int((int *)layer_mask, (int)(b + 1), (int)(b + CLOUD_MIN_THICK),
              (int *)&sum_lay);

      if (sum_lay < CLOUD_MIN_THICK) {
        for (int k = (b + 1); k <= (b + CLOUD_MIN_THICK); k++)
          layer_mask[k] = (int)0;
      } else if (layer_mask[b + CLOUD_MIN_THICK + 1] == (int)0) {
        for (int k = (b + 1); k <= (b + CLOUD_MIN_THICK); k++)
          layer_mask[k] = (int)0;
      }
    }
  }
}

void CLidar_Operations::Get_PBL_Mask( double *pr2_i, strcGlobalParameters *glbParam, strcMolecularData *dataMol, double rayFit_gap) 
{
  // printf("\nGetting pbl\n") ;
  // DEFINE THE WINDOWS OF THE SCAN
  nWinSize = 0 ;
  for( int i=glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] ; i >glbParam->indxInitSig ; i--)
  {
    if ( cloudProfiles[glbParam->evSel].clouds_ON[i] == BIN_CLOUD )
    {
      nWinSize = (int) round( (glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] - i) /3 ) ;
      break ;
    }
  }
  if ( nWinSize == 0 )
    nWinSize = (int) round( (glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] - glbParam->indxInitSig) /3 ) ;

  int   nSteps   = glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] - nWinSize +1 ;
  strcFitParam	fitParam  ; 
  double		*stdPr2	    		= (double*) new double  [nSteps] 	        ; for (int i = 0; i < nSteps; i++)  stdPr2[i] = DBL_MAX;
  // double 		*prFit          = (double*) new double  [glbParam->nBins]	; memset( (double*)prFit, 0, sizeof(double)*glbParam->nBins ) ;
  memset( (double*)pr_misc, 0, sizeof(double)*glbParam->nBins) ;

  // FIND THE LOWEST MOLECULAR RANGES USING A WINDOWS OF nWinSize
  int indxMin_Std, minStd ;
  for( int i=glbParam->indxInitSig ; i <nSteps ; i++ )
  {
    fitParam.indxInitFit = i ;
    fitParam.indxEndFit  = fitParam.indxInitFit + nWinSize ;
    fitParam.nFit	    	 = fitParam.indxEndFit  - fitParam.indxInitFit +1 ;

      RayleighFit ( (double*)&pr2_i[0], (double*)&dataMol->pr2Mol_avg[0], fitParam.nFit, (strcFitParam*)&fitParam, (double*)pr_misc ) ;
      stdPr2[i] = fitParam.std ;
  }
  findIndxMin( (double*)stdPr2, (int)0, (int)(nSteps -1), (int*)&indxMin_Std, (double*)(&minStd) ) ;
  
  // FIT THE LIDAR SIGNAL IN THE LOWEST MOLECULAR RANGE TO GET THE PBL MASK
  fitParam.indxInitFit = indxMin_Std+glbParam->indxInitSig ;
  fitParam.indxEndFit  = fitParam.indxInitFit + nWinSize   ;
  fitParam.nFit        = fitParam.indxEndFit  - fitParam.indxInitFit +1 ;
  // memset( (double*)prFit, 0, sizeof(double)*glbParam->nBins ) ;
  memset( (double*)pr_misc, 0, sizeof(double)*glbParam->nBins ) ;

  RayleighFit( (double*)&pr2_i[0], (double*)&dataMol->pr2Mol_avg[0], fitParam.indxInitFit+1, (strcFitParam*)&fitParam, (double*)pr_misc ) ;

  // MASKING THE PBL
  for (int i = glbParam->indxInitSig; i < fitParam.indxEndFit; i++) 
  {
    if (pr2_i[i] > (pr_misc[i] + rayFit_gap * std_factor_layer_mask ) )
      cloudProfiles[glbParam->evSel].clouds_ON[i] = (int)BIN_CLOUD;
    else
      cloudProfiles[glbParam->evSel].clouds_ON[i] = (int)0;
  }
  for (int i = 0; i <= glbParam->indxInitSig; i++)
    cloudProfiles[glbParam->evSel].clouds_ON[i] = (int)BIN_CLOUD;

} // Get_PBL_Mask

void CLidar_Operations::Remove_Bkg_Mol_Range(double *pr, strcGlobalParameters *glbParam, strcMolecularData *dataMol) 
{
  int nSteps = dataMol->last_Indx_Mol_High[glbParam->evSel][glbParam->chSel] - dataMol->last_Indx_Mol_Low[glbParam->evSel][glbParam->chSel] + 1;

  strcFitParam fitParam;
  double *R2 = (double *)new double[nSteps];
  memset(R2, 0, (sizeof(double) * nSteps));
  int *indxRef_Stop = (int *)new int[nSteps];
  double *prFit = (double *)new double[glbParam->nBins];
  fitParam.indxEndFit = dataMol->last_Indx_Mol_High[glbParam->evSel][glbParam->chSel] ;
  fitParam.indxInitFit = dataMol->last_Indx_Mol_Low[glbParam->evSel][glbParam->chSel] ;
  fitParam.nFit = fitParam.indxEndFit - fitParam.indxInitFit + 1;
  int i = 0;
  do {
      Fit((double *)&pr[0], (double *)&(dataMol->prMol[0]), fitParam.nFit, "wB", "NOTall", (strcFitParam *)&fitParam, (double *)prFit);
      R2[i] = fitParam.R2;
      indxRef_Stop[i] = fitParam.indxEndFit;
      // printf( "\nRange Stop[i]= %lf \t R2[i]= %lf",
      // indxRef_Stop[i]*glbParam->dr, R2[i] ) ;

      fitParam.indxEndFit = fitParam.indxEndFit - 1;
      fitParam.nFit = fitParam.indxEndFit - fitParam.indxInitFit + 1;
      i++;
  } while (i < (nSteps - 5));
  delete[] prFit;

  int indxMin_Std, minStd;
  findIndxMax((double *)R2, (int)0, (int)(nSteps - 1), (int *)(&indxMin_Std), (double *)(&minStd));

  // printf( "\nindxRef_Stop[%d]= %d", indxMin_Std, indxRef_Stop[indxMin_Std] ) ;
  dataMol->last_Indx_Mol_High[glbParam->evSel][glbParam->chSel] = indxRef_Stop[indxMin_Std];
}

// BIAS SUBSTRACTION METHODS
void CLidar_Operations::Bias_Substraction_Mean(double *sig, strcMolecularData *dataMol, strcGlobalParameters *glbParam, double *pr_noBias) 
{
  if ((glbParam->range_Bkg_Start < 0) || (glbParam->range_Bkg_Stop < 0) || (glbParam->range_Bkg_Start > glbParam->range_Bkg_Stop))
  {
    if (glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] < 0)
      Find_Max_Mol_Range((double *)sig, (strcMolecularData *)dataMol, (strcGlobalParameters *)glbParam, (int)avg_Points_Cloud_Mask);
    // printf( "\nBias_Substraction_Mean() ====> \n
    // dataMol->last_Indx_Mol_Low[m]= %lf\n dataMol->last_Indx_Mol_High[m]=
    // %lf\n dataMol->last_Indx_Bkg_Low[m]= %lf\n
    // dataMol->last_Indx_Bkg_High[m]= %lf\n", 					dataMol->last_Indx_Mol_Low
    // *glbParam->dzr, dataMol->last_Indx_Mol_High *glbParam->dzr,
    // dataMol->last_Indx_Bkg_Low *glbParam->dzr, dataMol->last_Indx_Bkg_High
    // *glbParam->dzr ) ;
  }
  else
  { // BACKGROUND RANGE IS SET BY THE USER
    dataMol->last_Indx_Bkg_Low[glbParam->evSel][glbParam->chSel]  = (int)round(glbParam->range_Bkg_Start / glbParam->dr);
    dataMol->last_Indx_Bkg_High[glbParam->evSel][glbParam->chSel] = (int)round(glbParam->range_Bkg_Stop  / glbParam->dr);
  }

  double bkgMean = 0.0;

  // IF Find_Max_Mol_Range() DETECTED MODULATED SIGNAL AT THE END OF THE TRACK.
  // THE BIAS IS OBTAINED FROM THE FIT METHOD
  if ((dataMol->last_Indx_Bkg_Low[glbParam->evSel][glbParam->chSel] < 0) && (dataMol->last_Indx_Bkg_High[glbParam->evSel][glbParam->chSel] < 0))
  {
    printf("\nBias_Substraction_Mean() ==> Channel %d ==> Moved to FIT method due to modulation in the end of the recorded lidar signal.\n", glbParam->chSel);
    Bias_Substraction_MolFit((strcMolecularData *)dataMol, (const double *)sig, (strcGlobalParameters *)glbParam, (double *)pr_noBias);
  }
  else
  { // CALCULATE THE MEAN VALUE OF THE BACKGROUND AND SUBSTRACT IT FROM THE SIGNAL.
    for (int j = dataMol->last_Indx_Bkg_Low[glbParam->evSel][glbParam->chSel]; j <= dataMol->last_Indx_Bkg_High[glbParam->evSel][glbParam->chSel]; j++)
      bkgMean = bkgMean + sig[j];

    bkgMean = bkgMean / (dataMol->last_Indx_Bkg_High[glbParam->evSel][glbParam->chSel] - dataMol->last_Indx_Bkg_Low[glbParam->evSel][glbParam->chSel] + 1);
    for (int i = 0; i < glbParam->nBins_Ch_eff[glbParam->chSel]; i++)
      pr_noBias[i] = (double)(sig[i] - bkgMean);
  }
  // printf("\n CLidar_Operations::Bias_Substraction_Mean() -> biasMean= %lf",
  // bkgMean ) ; printf("\n CLidar_Operations::Bias_Substraction_Mean()[%d][%d]
  // -> dataMol->last_Indx_Bkg_Low[m]= %lf \t dataMol->last_Indx_Bkg_High[m]=
  // %lf \n", glbParam->evSel, glbParam->chSel,
  // dataMol->last_Indx_Bkg_Low[glbParam->evSel][glbParam->chSel]
  // *glbParam->dzr,
  // dataMol->last_Indx_Bkg_High[glbParam->evSel][glbParam->chSel]
  // *glbParam->dzr ) ;
}

void CLidar_Operations::Bias_Substraction_MolFit(strcMolecularData *dataMol,
                                                 const double *prEl,
                                                 strcGlobalParameters *glbParam,
                                                 double *pr_no_DarkCur) {
  if ((glbParam->range_Bkg_Start < 0) || (glbParam->range_Bkg_Stop < 0) ||
      (glbParam->range_Bkg_Start > glbParam->range_Bkg_Stop)) {
    if (glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] < 0)
      Find_Max_Mol_Range((double *)prEl, (strcMolecularData *)dataMol,
                         (strcGlobalParameters *)glbParam,
                         (int)avg_Points_Cloud_Mask);
  } else {
    // BACKGROUND RANGE IS SET BY THE USER. RUN Find_Max_Mol_Range() FOR FILLING
    // THE VARIABLES dataMol->last_Indx_Bkg_Low dataMol->last_Indx_Bkg_High
    if (glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] < 0)
      Find_Max_Mol_Range((double *)prEl, (strcMolecularData *)dataMol,
                         (strcGlobalParameters *)glbParam,
                         (int)avg_Points_Cloud_Mask);
    dataMol->last_Indx_Mol_Low[glbParam->evSel][glbParam->chSel] =
        (int)round(glbParam->range_Bkg_Start / glbParam->dr);
    dataMol->last_Indx_Mol_High[glbParam->evSel][glbParam->chSel] =
        (int)round(glbParam->range_Bkg_Stop / glbParam->dr);
  }

  double Bias_Pr = 0.0;

  if ((dataMol->last_Indx_Mol_Low[glbParam->evSel][glbParam->chSel] < 0) &&
      (dataMol->last_Indx_Mol_High[glbParam->evSel][glbParam->chSel] < 0))
    Bias_Substraction_Auto((double *)prEl, (strcMolecularData *)dataMol,
                           (strcGlobalParameters *)glbParam,
                           (double *)pr_no_DarkCur, (double *)&Bias_Pr);
  else {
    fitParam.indxInitFit =
        dataMol->last_Indx_Mol_Low[glbParam->evSel][glbParam->chSel];
    fitParam.indxEndFit =
        dataMol->last_Indx_Mol_High[glbParam->evSel][glbParam->chSel];
    fitParam.nFit = fitParam.indxEndFit - fitParam.indxInitFit + 1;
    // printf("\nBias_Substraction_MolFit() ==> fitParam.indxInitFit[m]= %lf \t
    // fitParam.indxEndFit[m]= %lf \n", fitParam.indxInitFit *glbParam->dzr,
    // fitParam.indxEndFit*glbParam->dzr ) ;

    Fit((double *)prEl, (double *)dataMol->prMol, dataMol->nBins, "wB",
        "NOTall", (strcFitParam *)&fitParam, (double *)dummy);
    for (int i = 0; i < glbParam->nBins_Ch_eff[glbParam->chSel]; i++)
      pr_no_DarkCur[i] = (double)(prEl[i] - fitParam.b);
  }
  printf("\n CLidar_Operations::Bias_Substraction_MolFit() -> bias= %lf.\n",
         fitParam.b);
}

void CLidar_Operations::Bias_Substraction_Pre_Trigger(
    double *sig, strcGlobalParameters *glbParam, double *pr_noBias) {
  for (int i = 0; i < (glbParam->nBins_Ch[glbParam->chSel] -
                       glbParam->indxOffset[glbParam->chSel]);
       i++)
    pr_noBias[i] = (double)(sig[i] - bias_pre_trigger);

  printf(
      "\n CLidar_Operations::Bias_Substraction_Pre_Trigger() -> bias= %lf.\n",
      bias_pre_trigger);
}

void CLidar_Operations::Average_in_Time_Lidar_Profiles( strcGlobalParameters *glbParam, double ***dataFile, double ***dataFile_AVG, int *Raw_Data_Start_Time, int *Raw_Data_Stop_Time, int *Raw_Data_Start_Time_AVG, int *Raw_Data_Stop_Time_AVG) 
{
  printf("\n\n");
  for (int fC = 0; fC < glbParam->nEventsAVG; fC++) 
  {
    printf("Averaging in time Cluster Nº %d \n", fC);
    for (int c = 0; c < glbParam->nCh; c++) 
    {
      for (int b = 0; b < glbParam->nBins; b++) 
      {
        for (int t = 0; t < glbParam->numEventsToAvg; t++) 
        {
          dataFile_AVG[fC][c][b] = (double)dataFile_AVG[fC][c][b] +  dataFile[fC * glbParam->numEventsToAvg + t][c][b];
          if ((b == 0) && (c == 0)) 
          { // TIME IS *NOT* AVERAGED, ITS SAVED THE START AND END TIME OF EACH CLUSTER
            if (t == 0)
                Raw_Data_Start_Time_AVG[fC] = (long)Raw_Data_Start_Time[fC * glbParam->numEventsToAvg];
            if (t == (glbParam->numEventsToAvg - 1))
                Raw_Data_Stop_Time_AVG[fC] = (long)Raw_Data_Stop_Time[  fC * glbParam->numEventsToAvg + t];
          }
        }
        if (glbParam->DAQ_Type[c] == 0) // IF IS A PHOTON-CURRENT CHANNEL, THE SIGNALS HAVE TO BE AVERAGED
          dataFile_AVG[fC][c][b] = (double)(dataFile_AVG[fC][c][b] / glbParam->numEventsToAvg);
      }
    } // for ( int c=0 ; c <glbParam->nCh ; c++ )

    for (int t = 0; t < glbParam->numEventsToAvg; t++) 
    {
      glbParam->aAzimuthAVG    [fC] = glbParam->aAzimuthAVG    [fC] + glbParam->aAzimuth   [fC * glbParam->numEventsToAvg + t] ;
      glbParam->aZenithAVG     [fC] = glbParam->aZenithAVG     [fC] + glbParam->aZenith    [fC * glbParam->numEventsToAvg + t] ;
      glbParam->temp_K_agl_AVG [fC] = glbParam->temp_K_agl_AVG [fC] + glbParam->temp_K_agl [fC * glbParam->numEventsToAvg + t] ;
      glbParam->pres_Pa_agl_AVG[fC] = glbParam->pres_Pa_agl_AVG[fC] + glbParam->pres_Pa_agl[fC * glbParam->numEventsToAvg + t] ;
    }
    glbParam->aAzimuthAVG    [fC] = glbParam->aAzimuthAVG    [fC] / glbParam->numEventsToAvg ;
    glbParam->aZenithAVG     [fC] = glbParam->aZenithAVG     [fC] / glbParam->numEventsToAvg ;
    glbParam->temp_K_agl_AVG [fC] = glbParam->temp_K_agl_AVG [fC] / glbParam->numEventsToAvg ;
    glbParam->pres_Pa_agl_AVG[fC] = glbParam->pres_Pa_agl_AVG[fC] / glbParam->numEventsToAvg ;
    // printf("\n CLidar_Operations::Average_in_Time_Lidar_Profiles() -->
    // glbParam->aZenithAVG[%d]= %lf\n", fC, glbParam->aZenithAVG[fC] ) ;
  } // for ( int fC=0 ; fC <glbParam->nEventsAVG ; fC++ )
}

// VERSION CON LA APLICACION DE DARK CURRENT ANTES DE LAS DEMAS CORRECCIONES
void CLidar_Operations::Lidar_Signals_Corrections( strcGlobalParameters *glbParam, CMolecularData *oMolData, double **ovlp, double **data_Noise, double ***data_File_Lx, double ***pr_corr, double ***pr2) 
{
  // strcLidarSignal evSig;
  bool *ch_offset_corrected = (bool *)new bool[glbParam->nCh];

  for (int c = 0; c < glbParam->nCh; c++)
    ch_offset_corrected[c] = false;

  // GetMem_evSig((strcLidarSignal *)&evSig, (strcGlobalParameters *)glbParam);

  ReadAnalysisParameter( (char *)glbParam->FILE_PARAMETERS, (const char *)"PHO_MAX_COUNT_MHz", (const char *)"double", (double *)&glbParam->PHO_MAX_COUNT_MHz);
  ReadAnalysisParameter((char *)glbParam->FILE_PARAMETERS, "BkgCorrMethod", "string", (char *)glbParam->BkgCorrMethod, sizeof(glbParam->BkgCorrMethod));

  for (int e = 0; e < glbParam->nEventsAVG; e++) 
  {
    glbParam->evSel = e;
    printf("\n\n\n====================================> Applying corrections to the lidar event number: %d/%d (zenith: %lf deg) <===========================\n", glbParam->evSel, (glbParam->nEventsAVG - 1), glbParam->aZenithAVG[e]);
    for (int c = 0; c < glbParam->nCh; c++) 
    {
      glbParam->chSel = (int)c;
      switch (glbParam->DAQ_Type[c]) 
      {
        case 0:
                printf("\n Channel: %02d - %04d nm - (Analog) - Correction Applied -------------------> ", c, glbParam->iLambda[c]);
                break;
        case 1:
                printf("\n Channel: %02d - %04d nm - (Photon Counting) - Correction Applied ----------> ", c, glbParam->iLambda[c]);
                break;
        case 2:
                printf("\n Channel: %02d - %04d nm - (Analog Squared) - Correction Applied -----------> ", c, glbParam->iLambda[c]);
                break;
        case 3:
                printf("\n Channel: %02d - %04d nm - (Photon Counting Squared) - Correction Applied --> ", c, glbParam->iLambda[c]);
                break;
        case 4:
                printf("\n Channel: %02d - %04d nm - (Power Meter) - Correction Applied --------------> ", c, glbParam->iLambda[c]);
                break;
        case 5:
                printf("\n Channel: %02d - %04d nm - (Overflow) - Correction Applied -----------------> ", c, glbParam->iLambda[c]);
                break;
      }; // switch ( glbParam->DAQ_Type[c] )

      // DARK CURRENT CORRECTIONS
      // -------------------------------------------------------------
      if (glbParam->is_Dark_Current_Loaded == true) 
      {
        for (int i = 0; i < glbParam->nBins; i++)
          pr_corr[e][c][i] = (double)(data_File_Lx[e][c][i] - data_Noise[c][i]);
      } else 
      {
        for (int i = 0; i < glbParam->nBins; i++)
          pr_corr[e][c][i] = (double)data_File_Lx[e][c][i];
      }
      // OFFSET CORRECTIONS
      // ------------------------------------------------------------------------
      // PHOTON-CURRENT SIGNALS --> THE SIGNAL HAVE TO MOVE *BACKWARD*
      // glbParam->indxOffset[c] BINS
      printf("| Offset | ");
      if (glbParam->indxOffset[c] >= 0) // PRE-TRIGGER BIAS CORRECTION
      {
        if ((strcmp(glbParam->BkgCorrMethod, "PRE_TRIGGER") == 0) || (strcmp(glbParam->BkgCorrMethod, "pre_trigger") == 0) ||
            (strcmp(glbParam->BkgCorrMethod, "AUTO"       ) == 0) || (strcmp(glbParam->BkgCorrMethod, "auto"       ) ==0))
        { // BEFORE SHIFTING THE OFFSET, THE BIAS IS CALCULATED FROM THE PRE-TRIGGER REGION
          bias_pre_trigger = 0.0;
          for (int i = 0; i <= glbParam->indxOffset[c]; i++)
            bias_pre_trigger = bias_pre_trigger + (double)pr_corr[e][c][i];
          bias_pre_trigger = bias_pre_trigger / (glbParam->indxOffset[c] + 1);
        }
        // SHIFT THE SIGNAL glbParam->indxOffset[c] BINS TO THE LEFT
        for (int b = 0; b < glbParam->nBins_Ch_eff[c]; b++)
          pr_corr[e][c][b] = (double)pr_corr[e][c][b + glbParam->indxOffset[c]];
        for (int b = glbParam->nBins_Ch_eff[c]; b < glbParam->nBins_Ch[c]; b++)
          pr_corr[e][c][b] = (double)0.0;
      } // if (glbParam->indxOffset[c] >= 0)
      else  // glbParam->indxOffset[c] >=0 // PHOTON-COUNTING SIGNALS --> THE SIGNAL HAVE SHIFTED glbParam->indxOffset[c] BINS TO THE RIGHT
      {
        for (int b = (glbParam->nBins_Ch[c] - 1); b >= (-1 * glbParam->indxOffset[c]); b--)
          pr_corr[e][c][b] = (double)pr_corr[e][c][b + glbParam->indxOffset[c]]; // pr_corr[e][c][b] = (double)evSig.pr_no_DarkCur[b + glbParam->indxOffset[c]] ;
        for (int b = 0; b < (-1 * glbParam->indxOffset[c]); b++)
          pr_corr[e][c][b] = (double)0.0;
      } // --------------------------------------------------------------------------------------------

      //! DESATURATION OF THE PHOTON COUNTING CHANNELS
      //! ------------------------------------------------
      if (glbParam->DAQ_Type[c] == 1)
      {
        // COUNTS TO MHZ
        for (int b = 0; b < glbParam->nBins; b++) // COUNTS TO MHZ
          pr_corr[e][c][b] = (double)(pr_corr[e][c][b] / (glbParam->numEventsToAvg * glbParam->nShots[c] * glbParam->tBin_us)); // [MHz]

        if (glbParam->PHO_MAX_COUNT_MHz < 0)
          printf("| Dead-Time Correction *NOT* applied (converted to MHz only due to PHO_MAX_COUNT_MHz<0) |\t");
        else
        {
          printf("| Dead-Time |  ");
          for (int b = 0; b < glbParam->nBins; b++)
            pr_corr[e][c][b] = (double)(pr_corr[e][c][b] / (1.0 - pr_corr[e][c][b] / glbParam->PHO_MAX_COUNT_MHz)); // Non-paralyzable correction
        }
      } // if (glbParam->DAQ_Type[c] == 1) --------------------------------------------------------------------------------------------

      if ((glbParam->DAQ_Type[c] == 0) || (glbParam->DAQ_Type[c] == 1))
      { // ANALOG OR PHOTON-COUNTING LIDAR SIGNALS DATA TYPES
        if (glbParam->is_Ovlp_Data_Loaded == true) 
        { // OVERLAP CORRECTION --------------------------------------------------------------
          printf("| Overlap |  ");
          for (int i = 0; i < glbParam->nBins; i++)
            pr_corr[e][c][i] = (double)((double)pr_corr[e][c][i] / (double)(ovlp[c][i]));
        }
        // BIAS CORRECTION
        // --------------------------------------------------------------
        if (strcmp(glbParam->exeFile, "./lpp1") == 0)
          oMolData->Fill_dataMol_L1((strcGlobalParameters *)glbParam);
        else if (strcmp(glbParam->exeFile, "./lpp2") == 0)
          oMolData->Fill_dataMol_L2((strcGlobalParameters *)glbParam);
        else 
        {
          printf("\n\nLidar_Signals_Corrections(...): Error: Unknown executable file name: %s\n", glbParam->exeFile);
          exit(1);
        }

        // FIND THE MAXIMUM MOLECULAR RANGE IF IT HAS NOT BEEN SEARCHED BEFORE
        if (glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] < 0)
          Find_Max_Mol_Range( (double *)pr_corr[e][c], (strcMolecularData *)&oMolData->dataMol, (strcGlobalParameters *)glbParam, (int)avg_Points_Cloud_Mask);

        printf("| Bias |  ");
        BiasCorrection((double *)&pr_corr[e][c][0], (strcGlobalParameters *)glbParam, (strcMolecularData *)&oMolData->dataMol);
      } // if ( (glbParam->DAQ_Type[c] == 0) || (glbParam->DAQ_Type[c] == 1) )

      // LOAD IN THE RETURNED VARIABLES pr_corr AND pr2 THE CORRECTED SIGNALS
      for (int i = 0; i < glbParam->nBins; i++)
        pr2[e][c][i] = (double)pr_corr[e][c][i] * pow(glbParam->r[i], 2);
    } // for ( int c=0 ; c <glbParam->nCh ; c++ )
  } // for ( int e=0 ; e <glbParam->nEventsAVG ; e++ )b
}

void CLidar_Operations::Gluing_Procedure(strcGlobalParameters *glbParam, CMolecularData *oMolData, double ***pr_corr) 
{
  glbParam->indx_gluing_Low_AN   = (int *)new int[glbParam->nCh];
  glbParam->indx_gluing_High_PHO = (int *)new int[glbParam->nCh];

  ReadAnalysisParameter( (char *)glbParam->FILE_PARAMETERS, (const char *)"MAX_TOGGLE_RATE_MHZ",(const char *)"double", (double *)&glbParam->MAX_TOGGLE_RATE_MHZ);
  ReadAnalysisParameter( (char *)glbParam->FILE_PARAMETERS, (const char *)"MIN_TOGGLE_RATE_MHZ",(const char *)"double", (double *)&glbParam->MIN_TOGGLE_RATE_MHZ);

  int nIndxsToGlue_Low_AN = ReadAnalysisParameter( (char *)glbParam->FILE_PARAMETERS, (const char *)"indx_Gluing_Low_AN", (const char *)"int", (int *)glbParam->indx_gluing_Low_AN);
  int nIndxsToGlue_High_PHO = ReadAnalysisParameter( (char *)glbParam->FILE_PARAMETERS, (const char *)"indx_Gluing_High_PHO",(const char *)"int", (int *)glbParam->indx_gluing_High_PHO);
  // CHECK IF THE GLUING INFORMATION IS CORRECTLY SET

  if (((nIndxsToGlue_Low_AN == nIndxsToGlue_High_PHO) &&
       (nIndxsToGlue_Low_AN > 0) && (nIndxsToGlue_High_PHO > 0)) &&
      ((glbParam->MAX_TOGGLE_RATE_MHZ > 0) &&
       (glbParam->MIN_TOGGLE_RATE_MHZ > 0) &&
       (glbParam->MIN_TOGGLE_RATE_MHZ < glbParam->MAX_TOGGLE_RATE_MHZ))) {
    printf("\n\n========================================> Gluing <================================================================================\n");
    for (int c = 0; c < nIndxsToGlue_High_PHO; c++) 
    {
      if ((glbParam->iLambda[glbParam->indx_gluing_Low_AN[c]] == glbParam->iLambda[glbParam->indx_gluing_High_PHO[c]]) &&
          (glbParam->DAQ_Type[glbParam->indx_gluing_Low_AN[c]] == 0) && (glbParam->DAQ_Type[glbParam->indx_gluing_High_PHO[c]] == 1)) 
      {
        glbParam->nPair_Ch_to_Glue = nIndxsToGlue_Low_AN;
        for (int e = 0; e < glbParam->nEventsAVG; e++) 
        {
          glbParam->evSel = e;
          // Find_Gluing_Ranges( (strcGlobalParameters*)&glbParam, (double***)pr_corr ) ;
          GluingLidarSignals((strcGlobalParameters *)glbParam, (double ***)pr_corr);

          if (glbParam->indx_gluing_Low_AN[c] == glbParam->indxWL_PDL2)
          {
            glbParam->chSel = glbParam->indxWL_PDL2; //!!!!!!!! poder seleccionar indxWL_PDL1 tambien
            oMolData->Fill_dataMol_L2((strcGlobalParameters *)glbParam);
            Find_Max_Mol_Range((double *)pr_corr[e][glbParam->indxWL_PDL2], (strcMolecularData *)&oMolData->dataMol, (strcGlobalParameters *)glbParam, (int)avg_Points_Cloud_Mask);
          } // if ( glbParam->indx_gluing_Low_AN[c] == glbParam->indxWL_PDL2 )
        }
      }
      else 
      {
        printf("\nGLUING: indexes %d of the gluing data do not correspond to the same wavelength and/or acquitition type.\n", c);
        printf("\t indx_Gluing_Low_AN[%d]   --> %d nm\n", c, glbParam->iLambda[glbParam->indx_gluing_Low_AN[c]]);
        printf("\t indx_Gluing_High_PHO[%d] --> %d nm\n", c, glbParam->iLambda[glbParam->indx_gluing_High_PHO[c]]);
      }
    } // for (int c =0; c <nIndxsToGlue_High_PHO ; c++)
  }
  else 
  {
    printf("\nGluing was *NOT* performed due to its configuration variables in: %s", glbParam->FILE_PARAMETERS);
    if (nIndxsToGlue_Low_AN < 0)
      printf("\n\t Variable indx_Gluing_Low_AN is commented or not set in setting file.");
    if (nIndxsToGlue_High_PHO < 0)
      printf("\n\t Variable indx_Gluing_High_PHO is commented or not set in setting file.");
    if ((nIndxsToGlue_Low_AN != nIndxsToGlue_High_PHO) && (nIndxsToGlue_Low_AN > 0) && (nIndxsToGlue_High_PHO > 0))
      printf("\n\t Different numbers of elements in the arrarys indx_Gluing_Low_AN and indx_Gluing_High_PHO");
    printf("\n\t NO gluing is applied for this analysis.\n\n");
  }
}

void CLidar_Operations::GluingLidarSignals(strcGlobalParameters *glbParam,
                                           double ***pr_corr) {
  double MHz_Max, MHz_Min, a;
  int indx_MHz_Max, indx_MHz_Min;
  int indxs_fit[3]; // INDEXES TO PERFORM THE FIT FOR THE GLUING

  for (int c = 0; c < glbParam->nPair_Ch_to_Glue; c++) {
    findIndxMin((double *)&pr_corr[glbParam->evSel]
                                  [glbParam->indx_gluing_High_PHO[c]][0],
                (glbParam->nBins_Ch[glbParam->chSel] - 1 - glbParam->nBinsBkg),
                (glbParam->nBins_Ch[glbParam->chSel] - 1), (int *)&indx_MHz_Min,
                (double *)&MHz_Min);
    findIndxMax((double *)&pr_corr[glbParam->evSel]
                                  [glbParam->indx_gluing_High_PHO[c]][0],
                0, (glbParam->nBins_Ch[glbParam->chSel] - 1),
                (int *)&indx_MHz_Max, (double *)&MHz_Max);

    if ((MHz_Min <= glbParam->MIN_TOGGLE_RATE_MHZ) && (MHz_Max >= glbParam->MAX_TOGGLE_RATE_MHZ)) 
    {
      printf("Event= %d --> Gluing channels %d and %d (%d nm)", glbParam->evSel, glbParam->indx_gluing_Low_AN[c], glbParam->indx_gluing_High_PHO[c], glbParam->iLambda[glbParam->indx_gluing_High_PHO[c]]);
      // CORRECTED PHOTON COUNTING VALUES HIGHER THAN MAX_TOGGLE_RATE_MHZ
      memset(dummy, 0, (sizeof(double) * glbParam->nBins_Ch[glbParam->chSel]));
      for (int b = 0; b < glbParam->nBins_Ch[glbParam->chSel]; b++)
        dummy[b] = fabs(
            pr_corr[glbParam->evSel][glbParam->indx_gluing_High_PHO[c]][b] -
            glbParam->MAX_TOGGLE_RATE_MHZ);
      findIndxMin((double *)&dummy[0], 0, indx_MHz_Max, (int *)&indxs_fit[0],
                  (double *)&a); // indxs_fit[0] DISCARDED
      findIndxMin((double *)&dummy[0], indx_MHz_Max,
                  (glbParam->nBins_Ch[glbParam->chSel] - 1),
                  (int *)&indxs_fit[1], (double *)&a);

      // CORRECTED PHOTON COUNTING VALUES LOWER THAN MIN_TOGGLE_RATE_MHZ
      memset(dummy, 0, (sizeof(double) * glbParam->nBins_Ch[glbParam->chSel]));
      for (int b = 0; b < glbParam->nBins_Ch[glbParam->chSel]; b++)
        dummy[b] = fabs(
            pr_corr[glbParam->evSel][glbParam->indx_gluing_High_PHO[c]][b] -
            glbParam->MIN_TOGGLE_RATE_MHZ);
      findIndxMin((double *)&dummy[0], indxs_fit[1],
                  (glbParam->nBins_Ch[glbParam->chSel] - 1),
                  (int *)&indxs_fit[2], (double *)&a);

      memset(dummy, 0, (sizeof(double) * glbParam->nBins));
      fitParam.indxInitFit = indxs_fit[1];
      fitParam.indxEndFit = indxs_fit[2];
      fitParam.nFit = fitParam.indxEndFit - fitParam.indxInitFit + 1;
      printf(" across %lf - %lf m\n", indxs_fit[1] * glbParam->dzr,
             indxs_fit[2] * glbParam->dzr);

      if (indxs_fit[1] < indxs_fit[2]) {
        Fit((double *)&pr_corr[glbParam->evSel]
                              [glbParam->indx_gluing_High_PHO[c]][0],
            (double *)&pr_corr[glbParam->evSel][glbParam->indx_gluing_Low_AN[c]]
                              [0],
            glbParam->nBins_Ch[glbParam->chSel], "wB", "all",
            (strcFitParam *)&fitParam, (double *)dummy);
      } else {
        printf("\nEvent= %d --> There is no fitting range to glue the channels "
               "%d and %d\n",
               glbParam->evSel, glbParam->indx_gluing_Low_AN[c],
               glbParam->indx_gluing_High_PHO[c]);
        indxs_fit[1] =
            indxs_fit[2] - 10; // ARBITRARY VALUE TO AVOID THE FITTING IN CASE
                               // THERE IS NO FITTING RANGE
      }

      // dummy: ANALOG SIGNAL FITTED TO THE PHOTON-COUNTING SIGNAL IN THE RANGES
      // indxs_fit[1]-indxs_fit[2]
      for (int b = 0; b < indxs_fit[1]; b++)
        pr_corr[glbParam->evSel][glbParam->indx_gluing_Low_AN[c]][b] =
            (double)dummy[b];
      for (int b = indxs_fit[1]; b < glbParam->nBins_Ch[glbParam->chSel]; b++)
        pr_corr[glbParam->evSel][glbParam->indx_gluing_Low_AN[c]][b] = (double)
            pr_corr[glbParam->evSel][glbParam->indx_gluing_High_PHO[c]][b];

    } // if ( (MHz_Min <= glbParam->MIN_TOGGLE_RATE_MHZ) && (MHz_Max >=
      // glbParam->MAX_TOGGLE_RATE_MHZ) )
    else
      printf(
          "\nEvent= %d --> There is no need to glue the channels %d and %d\n",
          glbParam->evSel, glbParam->indx_gluing_Low_AN[c],
          glbParam->indx_gluing_High_PHO[c]);
  } // for ( int c =0 ; c <glbParam->nPair_Ch_to_Glue ; c++ )
  printf("\n");
}

void CLidar_Operations::Find_Gluing_Ranges(strcGlobalParameters *glbParam, double ***pr_corr) 
{
  int scanWinSize = 500; // SIZE OF THE SCANNING WINDOW TO FIND THE FITTING
                         // RANGES FOR THE GLUING
  ReadAnalysisParameter((char *)glbParam->FILE_SOFT_CODED_VALUES, "winGluing", "int", (int *)&scanWinSize);

  memset(dummy, 0, (sizeof(double) * glbParam->nBins));
  for (int i = 0; i < glbParam->nBins; i++)
    dummy[i] = DBL_MAX;

  for (int c = 0; c < glbParam->nPair_Ch_to_Glue; c++) {
    for (int b = 0; b < glbParam->nBins_Ch[glbParam->chSel] - scanWinSize - 1; b++)
    {
      memset(dummy, 0, (sizeof(double) * glbParam->nBins));
      fitParam.indxInitFit = glbParam->indxInitSig + b;
      fitParam.indxEndFit = fitParam.indxInitFit + scanWinSize;
      fitParam.nFit = fitParam.indxEndFit - fitParam.indxInitFit + 1;

      RayleighFit((double *)&pr_corr[glbParam->evSel][glbParam->indx_gluing_Low_AN[c]][0], (double *)&pr_corr[glbParam->evSel][glbParam->indx_gluing_High_PHO[c]][0], glbParam->nBins_Ch[glbParam->chSel], (strcFitParam *)&fitParam, (double *)dummy);

      dummy[b] = (double)fitParam.std;
    }
  }
  int indx_Min;
  double a;
  findIndxMin((double *)&dummy[0], 0, glbParam->nBins_Ch[glbParam->chSel] - 1, (int *)&indx_Min, (double *)&a);
  printf("\nBest fitting range for the gluing: %lf - %lf m\n", (glbParam->indxInitSig + indx_Min) * glbParam->dzr, (glbParam->indxInitSig + indx_Min + scanWinSize) * glbParam->dzr);
}

void CLidar_Operations::RayleighFit(double *sig, double *sigMol, int nBins, strcFitParam *fitParam, double *sigFit)
{
  prMean    = (double)0.0 ;
  prMolMean = (double)0.0 ;
  mean(sig   , fitParam->indxInitFit, fitParam->indxEndFit, &prMean   ) ;
  mean(sigMol, fitParam->indxInitFit, fitParam->indxEndFit, &prMolMean) ;

  for (int i = 0; i < nBins; i++)
    sigFit[i] = sigMol[i] * prMean / prMolMean;

  fitParam->squared_sum_fit = (double)0.0;
  for (int i = fitParam->indxInitFit; i <= fitParam->indxEndFit; i++)
    fitParam->squared_sum_fit = fitParam->squared_sum_fit + pow((sigFit[i] - sig[i]), 2); // RSS

  fitParam->var = (double)fitParam->squared_sum_fit / (fitParam->nFit - 1);
  fitParam->std = (double)sqrt(fitParam->var);

  fitParam->mean_sig = (double)0.0;
  fitParam->s = (double)0.0;
  sum((double *)sig, (int)fitParam->indxInitFit, (int)fitParam->indxEndFit, (double *)&fitParam->s);
  if (fitParam->s != 0) 
  {
    fitParam->mean_sig = (double)fitParam->s / fitParam->nFit;
    fitParam->squared_sum_sig_vs_Mean = (double)0.0;
    for (int b = fitParam->indxInitFit; b <= fitParam->indxEndFit; b++)
      fitParam->squared_sum_sig_vs_Mean = fitParam->squared_sum_sig_vs_Mean + pow((sig[b] - fitParam->mean_sig), 2); // TSS

    // R2: COEFFICIENT OF DETERMINATION = 1 - RSS/TSS
    // RSS = Residuals Squared Sum  = ( sig - sigFit   )^2 --> SQUARED SUM OF
    // THE ERROR OF THE MODEL TSS = Total Squares Sum      = ( sig - mean_sig
    // )^2 --> SQUARED SUM OF THE ERROR OF THE MEAN dfTSS = nFit -1 dfRSS = nFit
    // -2 -1 R2_adj = 1 - ( RSS / (nFit -2) ) / ( TSS / (nFit -1) )
    double dfTSS = (double)fitParam->nFit - 1;
    double dfRSS = (double)fitParam->nFit - 2 - 1;
    fitParam->R2 = (double)(fitParam->squared_sum_sig_vs_Mean - fitParam->squared_sum_fit) / fitParam->squared_sum_sig_vs_Mean  ;
    fitParam->R2_adj = (double)1 - ((fitParam->squared_sum_fit / dfRSS) / (fitParam->squared_sum_sig_vs_Mean / dfTSS))          ;
    fitParam->F = (double)(fitParam->squared_sum_sig_vs_Mean - fitParam->squared_sum_fit) / fitParam->squared_sum_fit           ;
  }
  else
  {
    fitParam->R2     = (double)0.0;
    fitParam->F      = (double)0.0;
    fitParam->R2_adj = (double)0.0;
  }
}

void CLidar_Operations::Fit(double *sig, double *sigMol, int nBins, const char *modeBkg, const char *modeRangesFit, strcFitParam *fitParam, double *sigFit)
{
  fitParam->squared_sum_fit = (double)0.0; // RSS

  if (strcmp(modeBkg, "wB") == 0) 
  {
    double *coeff = (double *)new double[2];
    polyfitCoeff((const double *const)&sigMol[fitParam->indxInitFit], // X DATA
                 (const double *const)&sig[fitParam->indxInitFit],    // Y DATA
                 (unsigned int)fitParam->nFit, (unsigned int)1,
                 (double *)coeff);
    fitParam->m = (double)coeff[1];
    fitParam->b = (double)coeff[0];
    delete[] coeff;

    if (strcmp(modeRangesFit, "all") == 0) 
    {
      for (int i = 0; i < nBins; i++) 
      {
        sigFit[i] = (double)(sigMol[i] * fitParam->m + fitParam->b);
        fitParam->squared_sum_fit = fitParam->squared_sum_fit + pow((sigFit[i] - sig[i]), 2); // RSS
      }
    } 
    else
    {
      for (int i = fitParam->indxInitFit; i <= fitParam->indxEndFit; i++) 
      {
        sigFit[i] = (double)(sigMol[i] * fitParam->m + fitParam->b);
        fitParam->squared_sum_fit = fitParam->squared_sum_fit + pow((sigFit[i] - sig[i]), 2); // RSS
      }
    }
  } 
  else if (strcmp(modeBkg, "wOutB") == 0) 
  {
    double mNum = 0, mDen = 0;
    for (int i = fitParam->indxInitFit; i <= fitParam->indxEndFit; i++) 
    {
      mNum = mNum + sig[i] * sigMol[i];
      mDen = mDen + sigMol[i] * sigMol[i];
    }
    fitParam->m = mNum / mDen;
    fitParam->b = 0;

    if (strcmp(modeRangesFit, "all") == 0) 
    {
      for (int i = 0; i < nBins; i++) 
      {
        sigFit[i] = (double)(sigMol[i] * fitParam->m);
        fitParam->squared_sum_fit = fitParam->squared_sum_fit + pow((sigFit[i] - sig[i]), 2); // RSS
      }
    } 
    else
    {
      for (int i = fitParam->indxInitFit; i <= fitParam->indxEndFit; i++) 
      {
        sigFit[i] = (double)(sigMol[i] * fitParam->m);
        fitParam->squared_sum_fit = fitParam->squared_sum_fit + pow((sigFit[i] - sig[i]), 2); // RSS
      }
    }
  }

  if (fpclassify(fitParam->squared_sum_fit) == FP_NAN) 
  {
    fitParam->var = (double)DBL_MAX;
    fitParam->std = (double)DBL_MAX;
    printf("\n\tFit(): a NAN value was obtained during the fit... "
           "(fitParam->indxInitFit= %d - fitParam->indxEndFit= %d)\n ",
           fitParam->indxInitFit, fitParam->indxEndFit);

    for (int i = 0; i < nBins; i++) {
      if ((fpclassify(sigMol[i]) == FP_NAN) || (fpclassify(sig[i]) == FP_NAN)) {
        printf("\n\t\t sig[%d] = %lf ", i, sig[i]);
        break;
      }
    }
  } 
  else 
  {
    fitParam->var = (double)fitParam->squared_sum_fit / (fitParam->nFit - 1);
    fitParam->std = (double)sqrt(fitParam->var);
  }

  fitParam->mean_sig = (double)0.0;
  fitParam->s = (double)0.0;
  // printf("\n\tFit(): fitParam->indxInitFit = %d \t fitParam->indxEndFit= %d
  // \n", fitParam->indxInitFit , fitParam->indxEndFit ) ;
  sum((double *)sig, (int)fitParam->indxInitFit, (int)fitParam->indxEndFit, (double *)&fitParam->s);

  if (fitParam->s != 0) 
  {
    fitParam->mean_sig = (double)fitParam->s / fitParam->nFit;
    fitParam->squared_sum_sig_vs_Mean = (double)0.0;
    for (int b = fitParam->indxInitFit; b <= fitParam->indxEndFit; b++)
    {
      fitParam->squared_sum_sig_vs_Mean = fitParam->squared_sum_sig_vs_Mean + pow((sig[b] - fitParam->mean_sig), 2); // TSS
    } 

    // R2: COEFFICIENT OF DETERMINATION = 1 - RSS/TSS
    // RSS = Residuals Squared Sum  = ( sig - sigFit   )^2 --> SQUARED SUM OF
    // THE ERROR OF THE MODEL TSS = Total Squares Sum      = ( sig - mean_sig
    // )^2 --> SQUARED SUM OF THE ERROR OF THE MEAN dfTSS = nFit -1 dfRSS = nFit
    // -2 -1 R2_adj = 1 - ( RSS / (nFit -2) ) / ( TSS / (nFit -1) )
    double dfTSS = (double)fitParam->nFit - 1;
    double dfRSS = (double)fitParam->nFit - 2 - 1;
    fitParam->R2 = (double)(fitParam->squared_sum_sig_vs_Mean -
                            fitParam->squared_sum_fit) /
                   fitParam->squared_sum_sig_vs_Mean;
    fitParam->R2_adj =
        (double)1 - ((fitParam->squared_sum_fit / dfRSS) /
                     (fitParam->squared_sum_sig_vs_Mean / dfTSS));
    fitParam->F = (double)(fitParam->squared_sum_sig_vs_Mean -
                           fitParam->squared_sum_fit) /
                  fitParam->squared_sum_fit;
  } else {
    fitParam->R2 = (double)0.0;
    fitParam->F = (double)0.0;
  }
  // printf( "\n Fit() ==> fitParam->squared_sum_sig_vs_Mean= %e \t
  // fitParam->squared_sum_fit= %e \t fitParam->R2= %e \t fitParam->mean_sig= %e
  // \n", 					  fitParam->squared_sum_sig_vs_Mean, fitParam->squared_sum_fit,
  // fitParam->R2, fitParam->mean_sig ) ;
} // void CLidar_Operations::Fit( double *sig, double *sigMol, int nBins, const
  // char *modeBkg, const char *modeRangesFit, strcFitParam *fitParam, double
  // *sigFit )

void CLidar_Operations::TransmissionMethod_pr(double *pr,
                                              strcGlobalParameters *glbParam,
                                              strcMolecularData *dataMol,
                                              int indxBefCloud,
                                              int indxAftCloud, double *VOD) {
  double *prFit = (double *)new double[glbParam->nBins];

  int DELTA_RANGE_LIM_BINS;
  ReadAnalysisParameter((const char *)glbParam->FILE_SOFT_CODED_VALUES,
                        "DELTA_RANGE_LIM_BINS", "int",
                        (int *)&DELTA_RANGE_LIM_BINS);

  strcFitParam fitParam_before_cloud;
  strcFitParam fitParam_after_cloud;
  double prBeforeCloud, prAfterCloud;
  int indxA, indxB;
  do {
    fitParam_before_cloud.indxInitFit =
        indxBefCloud - (int)round(DELTA_RANGE_LIM_BINS);
    fitParam_before_cloud.indxEndFit =
        indxBefCloud - (int)round(DELTA_RANGE_LIM_BINS / 5);
    fitParam_before_cloud.nFit = fitParam_before_cloud.indxEndFit -
                                 fitParam_before_cloud.indxInitFit + 1;
    Fit((double *)pr, (double *)dataMol->prMol, glbParam->nBins, "wB", "NOTall",
        (strcFitParam *)&fitParam_before_cloud, (double *)prFit);
    indxB = indxBefCloud - (int)round(DELTA_RANGE_LIM_BINS / 2);
    prBeforeCloud = prFit[indxB];
    // printf("\n prFit[indxB]= %lf \t fitParam_before_cloud.m= %lf \n",
    // prFit[indxB], fitParam_before_cloud.m ) ; printf(" pr[%d]= %lf \n",
    // indxB, pr[indxB] ) ;
    fitParam_after_cloud.indxInitFit =
        indxAftCloud + (int)round(DELTA_RANGE_LIM_BINS / 5);
    fitParam_after_cloud.indxEndFit =
        indxAftCloud + (int)round(DELTA_RANGE_LIM_BINS);
    fitParam_after_cloud.nFit =
        fitParam_after_cloud.indxEndFit - fitParam_after_cloud.indxInitFit + 1;
    Fit((double *)pr, (double *)dataMol->prMol, glbParam->nBins, "wB", "NOTall",
        (strcFitParam *)&fitParam_after_cloud, (double *)prFit);
    indxA = indxAftCloud + (int)round(DELTA_RANGE_LIM_BINS / 2);
    prAfterCloud = prFit[indxA];
    // printf(" prFit[indxA]= %lf \t fitParam_after_cloud.m= %lf \n",
    // prFit[indxA], fitParam_after_cloud.m ) ; printf(" pr[%d]= %lf \n", indxA,
    // pr[indxA] ) ;
    // DELTA_RANGE_LIM_BINS = DELTA_RANGE_LIM_BINS -3 ;
    indxBefCloud = indxBefCloud - 1;
    indxAftCloud = indxAftCloud + 1;
  } while ((prBeforeCloud < prAfterCloud));

  double rPr = prAfterCloud / prBeforeCloud;
  double rPrMol = dataMol->prMol[indxA] / dataMol->prMol[indxB];
  double rR2 = pow(glbParam->r[indxA], 2) / pow(glbParam->r[indxB], 2);
  double Tp2Cloud = rPr / (rPrMol * rR2);
  // printf("\t\t rPr: %lf - prAfterCloud[%d]: %lf - prBeforeCloud[%d]: %lf \n",
  // rPr, indxA, prAfterCloud, indxB, prBeforeCloud ) ; printf("\t\t %d-%d ---
  // rPr: %lf - rPrMol: %lf - Tp2Cloud: %lf \n", indxBefCloud, indxAftCloud,
  // rPr, rPrMol, Tp2Cloud) ;
  *VOD = -0.5 * log(Tp2Cloud) * cos(dataMol->zenith * PI / 180);
  delete[] prFit;
}

void CLidar_Operations::ODcut(double *prS, strcMolecularData *dataMol,
                              strcGlobalParameters *glbParam,
                              strcFitParam *fitParam, int *clouds_ON) {
  double OD = 0; // CLOUD TRANSMISSION
  // double *pr2	   = (double*) new double [ glbParam->nBins ] ;

  // 	MakeRangeCorrected_array( (double*)prS, (strcGlobalParameters*)glbParam,
  // (strcMolecularData*)dataMol, (double*)pr2 ) ;

  int indxBefCloud, indxAftCloud;
  indxBefCloud = fitParam->indxInitFit;
  indxAftCloud = fitParam->indxEndFit;
  TransmissionMethod_pr((double *)prS, (strcGlobalParameters *)glbParam,
                        (strcMolecularData *)dataMol, (int)indxBefCloud,
                        (int)indxAftCloud, (double *)&OD);

  double OD_cut;
  ReadAnalysisParameter((const char *)glbParam->FILE_PARAMETERS, "OD_cut",
                        "double", (double *)&OD_cut);

  if ((OD >= 0) && (OD <= OD_cut)) {
    printf("\n<%d> filtering cloud due to OD (%lf)...\n", glbParam->evSel, OD);
    for (int i = fitParam->indxEndFit; i <= fitParam->indxEndFit; i++)
      clouds_ON[i] = 0;
  }
  // else
  // 	printf("\nOptical Depth of the cloud: %lf \t Altitude (asl): %lf -
  // %lf\n", OD, glbParam->siteASL + fitParam->indxInitFit * dataMol->dzr,
  // glbParam->siteASL + fitParam->indxEndFit * dataMol->dzr ) ;

  // delete pr2 ;
}

void CLidar_Operations::GetMem_cloudProfiles(strcGlobalParameters *glbParam) {
  cloudProfiles =
      (strcCloudProfiles *)new strcCloudProfiles[glbParam->nEventsAVG];
  for (int e = 0; e < glbParam->nEventsAVG; e++) {
    cloudProfiles[e].clouds_ON = (int *)new int[glbParam->nBins];
    memset(cloudProfiles[e].clouds_ON, 0, (sizeof(int) * glbParam->nBins));
    cloudProfiles[e].indxInitClouds = (int *)new int[NMAXCLOUDS];
    memset(cloudProfiles[e].indxInitClouds, 0, (sizeof(int) * NMAXCLOUDS));
    cloudProfiles[e].indxEndClouds = (int *)new int[NMAXCLOUDS];
    memset(cloudProfiles[e].indxEndClouds, 0, (sizeof(int) * NMAXCLOUDS));
    cloudProfiles[e].VOD_cloud = (double *)new double[NMAXCLOUDS];
    memset(cloudProfiles[e].VOD_cloud, 0, (sizeof(double) * NMAXCLOUDS));
    cloudProfiles[e].test_1 = (double *)new double[glbParam->nBins];
    memset(cloudProfiles[e].test_1, 0, (sizeof(double) * glbParam->nBins));
    cloudProfiles[e].test_2 = (double *)new double[glbParam->nBins];
    memset(cloudProfiles[e].test_2, 0, (sizeof(double) * glbParam->nBins));
    cloudProfiles[e].nClouds = (int)0;

    cloudProfiles[e].indxEndPBL = (int)0;
    cloudProfiles[e].rangeEndPBL = (double)0.0;
  }
}

void CLidar_Operations::GetMem_indxMol(strcGlobalParameters *glbParam) {
  indxMol = (strcIndexMol *)new strcIndexMol[glbParam->nEventsAVG];
  for (int e = 0; e < glbParam->nEventsAVG; e++) {
    indxMol[e].indxMolON = (int *)new int[glbParam->nBins];
    memset(indxMol[e].indxMolON, 0, (sizeof(int) * glbParam->nBins));
    indxMol[e].errRMS = (double *)new double[glbParam->nBins];
    memset(indxMol[e].errRMS, 0, (sizeof(double) * glbParam->nBins));
    indxMol[e].indxInicMol = (int *)new int[MAX_MOL_RANGES];
    memset(indxMol[e].indxInicMol, 0, (sizeof(int) * MAX_MOL_RANGES));
    indxMol[e].indxEndMol = (int *)new int[MAX_MOL_RANGES];
    memset(indxMol[e].indxEndMol, 0, (sizeof(int) * MAX_MOL_RANGES));
    indxMol[e].nMolRanges =
        (int)0; // CONTAIN THE NUMBER OF MOLECULAR RANGES DETECTED.
    indxMol[e].indxRef = (int)0;
    indxMol[e].ablHeight = (int)0;
    indxMol[e].endRayFit = (int)0;
  }
}

// void CLidar_Operations::Bias_Residual_Correction( const double *pr,
// strcGlobalParameters *glbParam, strcMolecularData *dataMol, double
// *pr_res_corr )
// {
// 	fitParam.indxInitFit = 300 ;
// 	fitParam.indxEndFit  = 900 ;
// 	fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1
// ; 	Fit( (double*)&pr[0], (double*)&dataMol->prMol[0], glbParam->nBins, "wB",
// "all", (strcFitParam*)&fitParam, (double*)dummy ) ;

// 	for (int i =0; i <glbParam->nBins; i++)
// 		pr_res_corr[i] = pr[i] - dummy[i] ;

// 	double *coeff = (double*) new double[2] ;
// 	polyfitCoeff( (const double* const) &sigMol[fitParam->indxInitFit], // X
// DATA 				  (const double* const) &sig[fitParam->indxInitFit], // Y DATA 				  (unsigned
// int       ) fitParam->nFit, 				  (unsigned int		  ) 1, 				  (double*
// ) coeff	 ) ; 	fitParam.m = (double)coeff[1] ; 	fitParam.b =
// (double)coeff[0] ; 	delete coeff ;
// }

/*
//! NO SE USA --> SACAR
void CLidar_Operations::Remove_Cloud_Mol_Range( double *pr, strcGlobalParameters
*glbParam, strcMolecularData *dataMol )
{
                int 	nWinSize = round(
(dataMol->last_Indx_Mol_High[glbParam->evSel][glbParam->chSel] -
dataMol->last_Indx_Mol_Low[glbParam->evSel][glbParam->chSel] +1)/2 ) ; int
nSteps   = dataMol->last_Indx_Mol_High[glbParam->evSel][glbParam->chSel] -
dataMol->last_Indx_Mol_Low[glbParam->evSel][glbParam->chSel] +1 - nWinSize   ;

                strcFitParam	fitParam
; double		 	*stdPr 			= (double*) new double
[nSteps] 	; int		 		*indxRef_Start 	= (int*)
new int		[nSteps] 	; int		 		*indxRef_Stop =
(int*) 	new int		[nSteps] 	; double 			*prFit =
(double*) new double[glbParam->nBins] 		; fitParam.indxInitFit =
dataMol->last_Indx_Mol_Low[glbParam->evSel][glbParam->chSel]	;
                fitParam.indxEndFit  = fitParam.indxInitFit + nWinSize
; fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1
; int i 			  	 = 0 ; do
                {
                        Fit( (double*)&pr[0], (double*)&(dataMol->prMol[0]),
fitParam.nFit, "wB", "NOTall", (strcFitParam*)&fitParam, (double*)prFit ) ;
                        stdPr[i] 		 = sqrt(
fitParam.squared_sum_fit/(fitParam.nFit -1) ) ; indxRef_Start[i] =
fitParam.indxInitFit ; indxRef_Stop [i] = fitParam.indxEndFit  ;

                        fitParam.indxInitFit = fitParam.indxInitFit +1
; fitParam.indxEndFit  = fitParam.indxInitFit + nWinSize
; fitParam.nFit	  	 = fitParam.indxEndFit - fitParam.indxInitFit +1
; i++ ; } while( fitParam.indxEndFit <=
dataMol->last_Indx_Mol_High[glbParam->evSel][glbParam->chSel] ) ; delete[] prFit
;

                int indxMin_Std, minStd ;
                findIndxMin( (double*)stdPr, (int)0, (int)(nSteps -1),
(int*)(&indxMin_Std), (double*)(&minStd) ) ;

                dataMol->last_Indx_Mol_Low[glbParam->evSel][glbParam->chSel]  =
indxRef_Start[indxMin_Std] ; dataMol->heightRef_Inversion_ASL = (double)
(glbParam->siteASL + glbParam->dzr*(
dataMol->last_Indx_Mol_Low[glbParam->evSel][glbParam->chSel] +
dataMol->last_Indx_Mol_High[glbParam->evSel][glbParam->chSel] )/2) ;
}
*/



    // int a, b;
    // // printf(" --> Getting PBL layer: \n") ;
    // if (cloudProfiles[glbParam->evSel].nClouds == 0) 
    // {
    //   a = (int)round( (glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] - glbParam->indxInitSig) * 1/4);
    //   b = (int)round( (glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] - glbParam->indxInitSig) * 3/8);
    //   // printf(" (1) Fit between the start range and the maximun range (%d,
    //   // %d).", a, b) ;
    // }
    // else if ((cloudProfiles[glbParam->evSel].nClouds >= 1)) 
    // {
    //   for (int c = 0; c < cloudProfiles[glbParam->evSel].nClouds; c++) // SEARCH FOR A CLOUD BASE HIGHER THAN 3 KM
    //   {
    //     if (cloudProfiles[glbParam->evSel].indxInitClouds[c] > (int)round(3000 / glbParam->dr)) 
    //     {
    //       a = (int)round((cloudProfiles[glbParam->evSel].indxInitClouds[c] - round(2000 / glbParam->dr)) * 1 / 4);
    //       b = (int)round((cloudProfiles[glbParam->evSel].indxInitClouds[c] - round(2000 / glbParam->dr)) * 3 / 8);
    //     }
    //   }
    //   if (fitParam.indxInitFit == 10) // IF THE CLOUDS ARE ALL BELOW 3 KM, THE PBL FIT IS DONE AT 3 KM
    //   {
    //     a = (int)-10;
    //     b = (int)-10;
    //     // a = (int)round(
    //     // (glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] -
    //     // round(2000/glbParam->dr)) *1/4 ) ; b = (int)round(
    //     // (glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel] -
    //     // round(2000/glbParam->dr)) *3/8 ) ; printf(" (3) Fit between the start
    //     // range and the maximun range (%d, %d).", a, b) ;
    //   }
    // } // else if ( (cloudProfiles[glbParam->evSel].nClouds >=1) )

    // if (a == -10) 
    // {
    //   fitParam.indxInitFit = (int)round(3000 / glbParam->dr);
    //   fitParam.indxEndFit = fitParam.indxInitFit + (int)deltaNorm;
    // }
    // else
    // {
    //   fitParam.indxInitFit = (int)round(2000 / glbParam->dr + b);
    //   fitParam.indxEndFit = fitParam.indxInitFit + a;

    //   if (fitParam.indxInitFit == fitParam.indxEndFit) 
    //     fitParam.indxEndFit = fitParam.indxInitFit + 1;
    // }
    // fitParam.indxMidFit = (int)round((fitParam.indxInitFit + fitParam.indxEndFit) / 2);
    // fitParam.nFit = fitParam.indxEndFit - fitParam.indxInitFit + 1;
    // // printf(" --> %f - %f m\n", fitParam.indxInitFit *glbParam->dr,
    // // fitParam.indxEndFit *glbParam->dr ) ;

    // memset((double *)pr_misc, 0, sizeof(double) * glbParam->nBins);
    // RayleighFit((double *)prS, (double *)dataMol->prMol_avg, glbParam->nBins, (strcFitParam *)&fitParam, (double *)pr_misc);

    // for (int i = glbParam->indxInitSig; i < fitParam.indxEndFit; i++) 
    // {
    //   if (prS[i] > (pr_misc[i] + std_ref * std_factor_layer_mask)) 
    //     cloudProfiles[glbParam->evSel].clouds_ON[i] = (int)1;
    //   else
    //     cloudProfiles[glbParam->evSel].clouds_ON[i] = (int)0;
    // }
    // for (int i = 0; i <= glbParam->indxInitSig; i++)
    //   cloudProfiles[glbParam->evSel].clouds_ON[i] = (int)1;

 /*
          // IF THERE ARE CLOUDS DETECTED --> ADJUST THE BOUNDARIES
          int sumLay =0 ;
          sum_int( (int*)&cloudProfiles[glbParam->evSel].clouds_ON[0], (int)0,
     (int)(glbParam->nBins_Ch[glbParam->chSel]-1), (int*)&sumLay ) ; if ( sumLay
     >0 )
          {
                  GetCloudLimits( (strcGlobalParameters*)glbParam ) ; // ONLY
     LIMITS OF THE CLOUDS, *NOT* PBL!!!!

                  for (int c =0; c < cloudProfiles[glbParam->evSel].nClouds;
     c++) {	//! CLOUD *BASE* HEIGH - START
     ////////////////////////////////////////////////////// int 	indxMin
     =0 ; double	min 	=0 ; findIndxMin( (double*)&pr2[0],
     (int)glbParam->indxInitSig,
     (int)cloudProfiles[glbParam->evSel].indxInitClouds[c], (int*)&indxMin,
     (double*)&min ) ;

                          printf("\n BEFORE ADJUSTING CLOUD --> Cloud %d base at
     %f m\n", c+1, cloudProfiles[glbParam->evSel].indxInitClouds[c] *
     glbParam->dr ) ; for (int j =indxMin; j <
     cloudProfiles[glbParam->evSel].indxInitClouds[c] ; j++)
                                  cloudProfiles[glbParam->evSel].clouds_ON[j] =
     (int)1 ; cloudProfiles[glbParam->evSel].indxInitClouds[c] = indxMin ;
                          printf(" AFTER ADJUSTING CLOUD--> Cloud %d base at %f
     m\n", c+1, cloudProfiles[glbParam->evSel].indxInitClouds[c] * glbParam->dr
     ) ;
                          //! CLOUD *BASE* HEIGH - END
     //////////////////////////////////////////////////////

                          //! CLOUD *TOP*  HEIGH - START
     //////////////////////////////////////////////////////
                          // FIT BEFORE THE CLOUD
                          fitParam.indxInitFit =
     cloudProfiles[glbParam->evSel].indxInitClouds[c] - round(deltaNorm*1.5) ;
                          fitParam.indxEndFit  = fitParam.indxInitFit +
     deltaNorm ; fitParam.indxMidFit  = (int)round( (fitParam.indxInitFit +
     fitParam.indxEndFit)/2 ) ; fitParam.nFit	  	 = fitParam.indxEndFit -
     fitParam.indxInitFit +1; memset( (double*)prFit, 0, sizeof(double) *
     glbParam->nBins ) ; RayleighFit( (double*)prS, (double*)dataMol->prMol_avg,
     glbParam->nBins, (strcFitParam*)&fitParam, (double*)prFit ) ;

                          // for (int j =fitParam.indxMidFit ; j <
     glbParam->indxEndSig_ev_ch[glbParam->evSel][glbParam->chSel]; j++ ) for
     (int j =fitParam.indxMidFit ; j <
     cloudProfiles[glbParam->evSel].indxEndClouds[c] + deltaNorm ; j++ )
                          {
                                  if ( prS[j] < prFit[j] )
                                  {
                                          cloudProfiles[glbParam->evSel].indxEndClouds[c]
     = j ; break ;
                                  }
                          }
                          //! CLOUD *TOP*  HEIGH - END
     ////////////////////////////////////////////////////// } // for (int c =0;
     c < cloudProfiles[glbParam->evSel].nClouds; c++) GetCloudLimits(
     (strcGlobalParameters*)glbParam ) ; } // if ( sumLay >0 ) else // NO CLOUDS
     DETECTED memset( (double*)&cloudProfiles[glbParam->evSel].clouds_ON[0], 0,
     sizeof(int)*glbParam->nBins ) ;
  */
