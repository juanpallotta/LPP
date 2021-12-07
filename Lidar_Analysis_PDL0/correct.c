#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>

#define DTOR 0.0174532925
#define RTOD 57.2957795

int max(int, int);
int min(int, int);
void readraw_char  (char*, char*, int, int);
void readraw_uint12(char*, unsigned short int*, int, int);
void readraw_uint16(char*, unsigned short int*, int, int);
void readraw_uint32(char*, unsigned long int*, int, int);
void readraw_float (char*, float*, int, int);
void read1or3panels       (int, int, char*, char*, char*, int , int , unsigned short int*);
void read1or3panels_uint32(int, int, char*, char*, char*, int , int , unsigned long int*);
void read1or3panels_float (int, int, char*, char*, char*, int , int , float*);
void read1or3panels_char  (int, int, char*, char*, char*, int , int , char*);

static const struct option longOpts[] = {
  {"panelxsize"       , required_argument, 0,  1},
  {"panelysize"       , required_argument, 0,  2},
  {"imagefilename"    , required_argument, 0,  3},
  {"outfilename"      , required_argument, 0,  4},
  {"inputtype"        , required_argument, 0,  5},
  {"integrationweight", required_argument, 0,  6},
  {"nheadlines"       , required_argument, 0,  7},
  {"verbose"          , no_argument      , 0,  8}, 
  {"darkoffset"       , required_argument, 0, 10},
  {"dark"             , required_argument, 0, 11},
  {"countdir"         , required_argument, 0, 12},
  {"flat"             , required_argument, 0, 21},
  {"nlcorC0"          , required_argument, 0, 30},
  {"nlcorC1"          , required_argument, 0, 31},
  {"nlcorC2"          , required_argument, 0, 32},
  {"calC0"            , required_argument, 0, 40},
  {"calC1"            , required_argument, 0, 41},
  {"defect"           , required_argument, 0, 50},
  {"defecttype"       , required_argument, 0, 51},
  {"blursize"         , required_argument, 0, 52},
  {"fillval"          , required_argument, 0, 53},
  {"alignment"        , required_argument, 0, 60},
  {"interpolate"      , required_argument, 0, 61},
  {0                  , 0                , 0,  0}
};

int main(int argv, char **argc)
{
  /* INPUT file ---------------------------------- */ 
  char *imagefilename1=0;          // file names
  char *imagefilename2=0;          
  char *imagefilename3=0;          
  //FILE *imagefile=0;              // fid
  unsigned short int *inimage=0;  // input data
  int panelxsize=0, panelysize=0;
  int inputtype=0;
  int integrationweightflag=0;
  float integrationweight=0;
  int nheadlines=0;
  // aux
  int nimages=0;
  int npanels=0;
  //int panelsize=0;
  int xsize=0, ysize=0, imagesize=0;

  /* OUTPUT file --------------------------------- */ 
  //char *outfilename=0;           // file names
  char *outfilename1=0;           // file names
  char *outfilename2=0;           // file names
  char *outfilename3=0;           // file names
  FILE *outfile=0;               // fid
  float *outimage=0;
  int noimages=0;

  /* DARK files ---------------------------------- */
  char *darkfilename1=0;        // file names
  char *darkfilename2=0;        // file names
  char *darkfilename3=0;        // file names
  unsigned short int *dark=0;  // input data
  int countdir=0; // +1 means higher counts, higher radiance. -1 for PACS
  int dodark=0;                // flags
  float darkoffset=0;
  int ndarks=0;

  /* FLAT files ---------------------------------- */
  char *flatdfilename1=0;
  char *flatdfilename2=0;
  char *flatdfilename3=0;
  //unsigned short int *inflatd=0;
  int doflat=0;                  
  // aux
  float *flatd=0;
  //float maxflatd=0;
  int nflatds=0;

  /* NON-LINEAR CORRECTION files ----------------- */
  char *nlcorC0filename1=0, *nlcorC0filename2=0, *nlcorC0filename3=0; // file names
  char *nlcorC1filename1=0, *nlcorC1filename2=0, *nlcorC1filename3=0;
  char *nlcorC2filename1=0, *nlcorC2filename2=0, *nlcorC2filename3=0;
  float *innlcorC0=0;      // input data
  float *innlcorC1=0;
  float *innlcorC2=0;
  int donlcor=0;           // flag
  int nnlcorC0s=0, nnlcorC1s=0, nnlcorC2s=0;

   /* CALIBRATION files -------------------------- */
  char *calC0filename1=0,*calC0filename2=0,*calC0filename3=0;   // file names
  char *calC1filename1=0,*calC1filename2=0,*calC1filename3=0;
  float *incalC0=0;        // input data
  float *incalC1=0;
  int docalib=0;           // flag
  int ncalC0s=0, ncalC1s=0;

   /* DEFECT files ------------------------------- */
  char *defectfilename1=0,*defectfilename2=0,*defectfilename3=0;  // file names
  char *mask=0;            // input data
  int dodefect=0;          // flag
  int ndefects=0;
  int defecttype=0;
  int blursize=0;
  unsigned short int fillval=0;
  // aux
  float *minusmask=0;
  float weightcount=0;
  int xdist=0, ydist=0, dist=0;
  int xoffset=0, yoffset=0;
  int *grid=0;
  int gridwidth=0;
  int gridsize=0;
  int centerpoint=0;
  float average=0;
  float maskmean=0;

  /* ALIGNMENT files ----------------------------- */
  char *alignmentfilename=0; // file names
  FILE *alignmentfile=0;     // fid
  int transx[3];             // input data
  int transy[3];
  float rotate[3];
  float scalex[3];
  float scaley[3];
  int doalignment=0;         // flags
  int dointerpolate=0;  
  //aux
  float *copyimage=0;
  int *copycount=0;
  int Tx=0, Ty=0;
  float theta=0, costheta=0, sintheta=0;
  float Sx=0, Sy=0;
  float midx=0, midy=0;
  float newx=0, newy=0;
  int neighborx=0, neighbory=0;
  float interpvalue=0;
  int interpcount=0;

  /* Other variables ----------------------------- */
  int opt=0;
  int longIndex=0;
  int result=0;
  int i=0, j=0, x=0, y=0;
  //char *afilename;
  char* afile=0;

  extern char *optarg;
  extern int optind;
  int opterror=0;
  int verboseflag=0;

  //----------------------------------------------------------------------------
  /*Default value for important parameters*/
  //----------------------------------------------------------------------------
  longIndex=0;
  opterror=0;
  defecttype=3;//default value
  blursize=5;//default value
  //hmjb this is VERY dangerous... why a fixed dark offset? if this is forgotten latter in data analysis, radiances will be wrong
  //darkoffset=20.0;//default value
  countdir=-1; // default is PACS, counts increase from 4095 to 0

  // =0 for 12-bit packed (PACS) =1 for 16-bit unpacked (AirHARP)
  inputtype=0;

  fillval=65535;//default value;

  verboseflag=0;

  integrationweightflag=0;
  integrationweight=1;

  npanels=3;

  //----------------------------------------------------------------------------
  /*Process arguments*/
  //----------------------------------------------------------------------------

  //hmjb - don't accept any short option
  //while((opt = getopt_long(argv, argc, "fr:", longOpts, &longIndex))!=-1)
  while((opt = getopt_long(argv, argc, "", longOpts, &longIndex))!=-1)
    {
      //hmjb organized in the order it is applied to the data
      switch(opt)
        {
        case 1: //panel x size
          panelxsize=atoi(optarg);
          break;
        case 2: //panel y size
          panelysize=atoi(optarg);
          break;
        case 3: //input file 
          nimages=nimages+1;
          if (nimages==1) imagefilename1=optarg;
          else if (nimages==2) imagefilename2=optarg;
          else if (nimages==3) imagefilename3=optarg;
          else { printf("ERROR: too many images= %d\n",nimages); return 1; }
          break;
        case 4: //output file
          noimages=noimages+1;
          if (noimages==1) outfilename1=optarg;
          else if (noimages==2) outfilename2=optarg;
          else if (noimages==3) outfilename3=optarg;
          else { printf("ERROR: too many output images= %d\n",noimages); return 1; }
          break;
        case 5: //input file type
          inputtype=atoi(optarg);
          break;
        case 6: //integration weight
          integrationweightflag=1;
          integrationweight=atof(optarg);
          break;
        case 7: //nheadlines
          nheadlines=atof(optarg);
          break;
        case 8: //verbose
          verboseflag=1;
          break;
        case 10: //dark offset
          darkoffset=atof(optarg);
          break;
        case 11: //dark
          dodark=1;
          ndarks=ndarks+1;
          if (ndarks==1) darkfilename1=optarg;
          else if (ndarks==2) darkfilename2=optarg;
          else if (ndarks==3) darkfilename3=optarg;
          else { printf("ERROR: too many darks= %d\n",ndarks); return 1; }
          break;
        case 12: //countdir
          countdir=atoi(optarg) < 0 ? -1 : 1;
          break;
        case 21: //flat
          doflat=1;
          nflatds=nflatds+1;
          if (nflatds==1) flatdfilename1=optarg;
          else if (nflatds==2) flatdfilename2=optarg;
          else if (nflatds==3) flatdfilename3=optarg;
          else { printf("ERROR: too many flatds= %d\n",nflatds); return 1; }
          break;
        case 30: //non-linear corr coefficient 0
          donlcor=1;
          nnlcorC0s=nnlcorC0s+1;
          if (nnlcorC0s==1) nlcorC0filename1=optarg;
          else if (nnlcorC0s==2) nlcorC0filename2=optarg;
          else if (nnlcorC0s==3) nlcorC0filename3=optarg;
          else { printf("ERROR: too many nlcorC0s= %d\n",nnlcorC0s); return 1; }
          break;
        case 31: //non-linear corr coefficient 1
          donlcor=1;
          nnlcorC1s=nnlcorC1s+1;
          if (nnlcorC1s==1) nlcorC1filename1=optarg;
          else if (nnlcorC1s==2) nlcorC1filename2=optarg;
          else if (nnlcorC1s==3) nlcorC1filename3=optarg;
          else { printf("ERROR: too many nlcorC1s= %d\n",nnlcorC1s); return 1; }
          break;
        case 32: //non-linear corr coefficient 2
          donlcor=1;
          nnlcorC2s=nnlcorC2s+1;
          if (nnlcorC2s==1) nlcorC2filename1=optarg;
          else if (nnlcorC2s==2) nlcorC2filename2=optarg;
          else if (nnlcorC2s==3) nlcorC2filename3=optarg;
          else { printf("ERROR: too many nlcorC2s= %d\n",nnlcorC2s); return 1; }
          break;
        case 40: //calibration coefficient 0
          docalib=1;
          ncalC0s=ncalC0s+1;
          if (ncalC0s==1) calC0filename1=optarg;
          else if (ncalC0s==2) calC0filename2=optarg;
          else if (ncalC0s==3) calC0filename3=optarg;
          else { printf("ERROR: too many calC0s= %d\n",ncalC0s); return 1; }
          break;
        case 41: //calibration coefficient 1
          docalib=1;
          ncalC1s=ncalC1s+1;
          if (ncalC1s==1) calC1filename1=optarg;
          else if (ncalC1s==2) calC1filename2=optarg;
          else if (ncalC1s==3) calC1filename3=optarg;
          else { printf("ERROR: too many calC1s= %d\n",ncalC1s); return 1; }
          break;
        case 50: //defect
          dodefect=1;
          ndefects=ndefects+1;
          if (ndefects==1) defectfilename1=optarg;
          else if (ndefects==2) defectfilename2=optarg;
          else if (ndefects==3) defectfilename3=optarg;
          break;
        case 51: //defect type
          defecttype=atoi(optarg);
          break;
        case 52: //blursize
          blursize=atoi(optarg);
          break;
        case 53: //fillval
          fillval=atoi(optarg);
          break;
        case 60: //alignment
          doalignment=1;
          alignmentfilename=optarg;
          break;
        case 61: //interpolate
          dointerpolate=1;
          break;
        case '?':
          opterror=1;
          break;
        }
    }

  // check options
  if (!((inputtype==0)||(inputtype==1)))
    { printf("ERROR: wrong inputtype= %d\n",inputtype); return 1;}
  if(panelxsize==0) 
    { printf("ERROR: --panelxsize is a mandatory option.\n"); return 1;}
  if(panelysize==0) 
    { printf("ERROR: --panelysize is a mandatory option.\n"); return 1;}
  if (!((nimages==1)||(nimages==3))) 
    { printf("ERROR: wrong number of input images= %d\n",nimages); return 1;}
  if (!((noimages==1)||(noimages==3))) 
    { printf("ERROR: wrong number of output images= %d\n",noimages); return 1;}
  if (dodark && !((ndarks==1)||(ndarks==3))) 
    { printf("ERROR: wrong number of darks= %d\n",ndarks); return 1;}
  if (!((countdir==-1)||(countdir==1)))
    { printf("ERROR: countdir should be -1 (PACS) or +1 (AirHARP)= %d\n",countdir); return 1;}
  if ((countdir==-1)&&(inputtype==1))
    { printf("ERROR: PACS (countdir=-1) with uint16 (inputtype=1)? unlikely...\n"); return 1;}
  if ((countdir==1)&&(inputtype==0))
    { printf("ERROR: AirHARP (countdir=+1) with packed uint12 (inputtype=0)? unlikely...\n"); return 1;}
  if (doflat && !((nflatds==1)||(nflatds==3))) 
    { printf("ERROR: wrong number of flats= %d\n",nflatds); return 1;}
  if (donlcor && !((nnlcorC0s==1)||(nnlcorC0s==3))) 
    { printf("ERROR: wrong number of nlcor C0= %d\n",nnlcorC0s); return 1;}
  if (donlcor && !((nnlcorC1s==1)||(nnlcorC1s==3))) 
    { printf("ERROR: wrong number of nlcor C1= %d\n",nnlcorC1s); return 1;}
  if (donlcor && !((nnlcorC2s==1)||(nnlcorC2s==3))) 
    { printf("ERROR: wrong number of nlcor C2= %d\n",nnlcorC2s); return 1;}
  if (docalib && !((ncalC0s==1)||(ncalC0s==3))) 
    { printf("ERROR: wrong number of cal C0= %d\n",ncalC0s); return 1;}
  if (docalib && !((ncalC1s==1)||(ncalC1s==3))) 
    { printf("ERROR: wrong number of cal C1= %d\n",ncalC1s); return 1;}
  
  //if (outfilename==NULL)
  //  { printf("ERROR: --outfilename is a mandatory option.\n"); return 1;}

  /*Get non-optional arguments*/
  if((opterror==1)||((argv - optind)!=0)) {
    printf("Usage: correct [--options-like-this]\n");
    return 1;}
  //else {
  //  panelxsize = atoi(argc[optind+0]);
  //  panelysize = atoi(argc[optind+1]);
  //  imagefilename = argc[optind+2];
  //  outfilename = argc[optind+3];}

  if (doalignment) {
    printf("--alignment feature removed from 'correct.c'\n");
    return 1;}

  //panelsize=panelxsize*panelysize;
  xsize=panelxsize*npanels;
  ysize=panelysize;
  imagesize = xsize*ysize;
  
  //----------------------------------------------------------------------------
  /*Open inimage, read to outimage*/
  //----------------------------------------------------------------------------

  // allocate memory of 3 panel image
  inimage = malloc(xsize*(ysize+nheadlines)*sizeof(unsigned short int));
  if(!inimage)
    {printf("ERROR: Couldn't allocate inimage\n"); return 1;}

  // user might have passed 1 or 3 files
  read1or3panels(inputtype, nimages, imagefilename1, imagefilename2, 
                 imagefilename3, panelxsize,  panelysize+nheadlines, inimage);

  //printf("xhmjb: %d @ %d \n",inimage[panelxsize/2+(nheadlines+508)*xsize],panelxsize/2+(nheadlines+508)*xsize);

  // final data will need to be float, so 
  outimage = malloc(imagesize*sizeof(float));
  if(!outimage)
    {printf("ERROR: Couldn't allocate outimage \n"); return 1;}

  // apply weight
  if(integrationweightflag)
    for(i=0;i<imagesize;i++) 
      // saturated pixels
      if (inimage[i+nheadlines*xsize]<16380)
        outimage[i]=((float)inimage[i+nheadlines*xsize])*integrationweight;
      else
        outimage[i]=fillval;
  else
    for(i=0;i<imagesize;i++) 
      if (inimage[i+nheadlines*xsize]<16380)
        outimage[i]=(float)inimage[i+nheadlines*xsize];
      else
        outimage[i]=fillval;

  //printf("xhmjb: %f @ %d\n",outimage[panelxsize/2+(508)*xsize],panelxsize/2+(508)*xsize); 
  //printf("%d %f\n",inimage[4*xsize+395], outimage[395]);
  //----------------------------------------------------------------------------
  /*Read and apply dark file*/
  //----------------------------------------------------------------------------
  if (dodark) {
    dark = malloc(imagesize*sizeof(unsigned short int));
    if(!dark)
      {printf("ERROR: Couldn't allocate dark\n"); return 1;}

    // user might have passed 1 or 3 files
    read1or3panels(inputtype, ndarks, darkfilename1, darkfilename2, 
                   darkfilename3, panelxsize,  panelysize, dark) ;

    //hmjb end
    if(integrationweightflag)
      for(i=0;i<imagesize;i++) 
        dark[i]=dark[i]*integrationweight;

    // count up from 0? or down from 4095?
    for(i=0;i<imagesize;i++) {
      //hmjb bug 2017-oct-5
      // zero or negative counts are not physically possible
      // sometimes we get 0 counts because of bad-blocks in the memory of the FPGA (or flash??). 
      // So here we set these values to fillval
      if (outimage[i]>0 && outimage[i]!=fillval)
        outimage[i] = (outimage[i] - (float)dark[i])*countdir + darkoffset;
      else
        outimage[i] = fillval;
      // hmjb bug?
      //if(outimage[i] < 0) {outimage[i] = 0.0;}
    }

    //j=99999;
    //for(i=0;i<imagesize;i++) if (inimage[i]<j) j=inimage[i];
    //printf("%d %f %f  min=%d\n",dark[24677], darkoffset, outimage[24677], j);

    free(dark);
  } else printf("SKIP: dark\n");

  //printf("xhmjb: %f \n",outimage[panelxsize/2+(508)*xsize]); 

  //----------------------------------------------------------------------------
  /*Read and apply flat*/
  //----------------------------------------------------------------------------
  if (doflat) {

    // allocate memory of 3 panel flat
    flatd = malloc(imagesize*sizeof(float));//unsigned short int));
    if(!flatd)
      {printf("ERROR: Couldn't allocate inflatd\n"); return 1;}

    // user might have passed 1 or 3 files
    // note: even for PACS, flat fields were always 16bit unpacked
    read1or3panels_float(3, nflatds, flatdfilename1, flatdfilename2, 
                   flatdfilename3, panelxsize,  panelysize, flatd) ;

    // final data will need to be float, so 
    //flatd = malloc(imagesize*sizeof(float));
    //if(!flatd)
    //  {printf("ERROR: Couldn't allocate flat\n"); return 1;}

    // Prepare flat files: Find max of flatd and normalize
    //maxflatd=0;
    //hmjb for(i=0;i<imagesize;i++){if(maxflatd<inflatd[i]){maxflatd=inflatd[i];}}
    //for(i=0;i<imagesize;i++){flatd[i]=(float)inflatd[i]/maxflatd;}
    //for(i=0;i<imagesize;i++){flatd[i]=(float)inflatd[i];}

    // Apply flat
    for(i=0;i<imagesize;i++)
      //hmjb bug 2017-oct-5
      // zero or negative counts are not physically possible
      // These were replaced by fillval above. So here we need to do the same
      if (flatd[i]!=0 && outimage[i]!=fillval)
        {outimage[i] = outimage[i]/flatd[i];}
      else
        {outimage[i] = fillval;}

    free(flatd);
    //free(inflatd);
  } else printf("SKIP: flat\n");

  //printf("xhmjb: %f \n",outimage[panelxsize/2+(508)*xsize]); 

  //----------------------------------------------------------------------------
  /*Read and apply non-linear correction coefficients*/
  //----------------------------------------------------------------------------
  if (donlcor) {
    // allocate memory of 3 panel image
    innlcorC0 = malloc(imagesize*sizeof(float));
    if(!innlcorC0) {printf("ERROR: Couldn't allocate innlcorC0\n"); return 1;}
    innlcorC1 = malloc(imagesize*sizeof(float));
    if(!innlcorC1) {printf("ERROR: Couldn't allocate innlcorC1\n"); return 1;}
    innlcorC2 = malloc(imagesize*sizeof(float));
    if(!innlcorC2) {printf("ERROR: Couldn't allocate innlcorC2\n"); return 1;}

    // user might have passed 1 or 3 files
    // note: coefficients are float, hence, inputtype=3
    read1or3panels_float(3, nnlcorC0s, nlcorC0filename1, nlcorC0filename2, 
                   nlcorC0filename3, panelxsize,  panelysize, innlcorC0) ;

    read1or3panels_float(3, nnlcorC1s, nlcorC1filename1, nlcorC1filename2, 
                   nlcorC1filename3, panelxsize,  panelysize, innlcorC1) ;

    read1or3panels_float(3, nnlcorC2s, nlcorC2filename1, nlcorC2filename2, 
                   nlcorC2filename3, panelxsize,  panelysize, innlcorC2) ;

    // apply calibration
    for(i=0;i<imagesize;i++)
      if (outimage[i]!=fillval)
        {outimage[i] = innlcorC0[i] + innlcorC1[i]*outimage[i]
            + innlcorC2[i]*outimage[i]*outimage[i];}

    free(innlcorC0);
    free(innlcorC1);
    free(innlcorC2);
  } else printf("SKIP: non-linear correction\n");

  //printf("xhmjb: %f \n",outimage[panelxsize/2+(508)*xsize]); 

  //----------------------------------------------------------------------------
  /*Read and apply calibration coefficients*/
  //----------------------------------------------------------------------------
  if (docalib) {
    // allocate memory of 3 panel image
    incalC0 = malloc(imagesize*sizeof(float));
    if(!incalC0) {printf("ERROR: Couldn't allocate incalC0\n"); return 1;}
    incalC1 = malloc(imagesize*sizeof(float));
    if(!incalC1) {printf("ERROR: Couldn't allocate incalC1\n"); return 1;}

    // user might have passed 1 or 3 files
    // note: coefficients are float, hence, inputtype=3
    read1or3panels_float(3, ncalC0s, calC0filename1, calC0filename2, 
                   calC0filename3, panelxsize,  panelysize, incalC0) ;

    read1or3panels_float(3, ncalC1s, calC1filename1, calC1filename2, 
                   calC1filename3, panelxsize,  panelysize, incalC1) ;

    // apply calibration
    for(i=0;i<imagesize;i++)
      if (outimage[i]!=fillval)
        {outimage[i] = incalC0[i] + incalC1[i]*outimage[i];}
    free(incalC0);
    free(incalC1);
  } else printf("SKIP: calibration\n");

  //printf("xhmjb: %f \n",outimage[panelxsize/2+(508)*xsize]); 

  //----------------------------------------------------------------------------
  /*Read defect mask and apply corrections*/
  //----------------------------------------------------------------------------

  if (dodefect) {

    // allocate memory of 3 panel flat
    mask = malloc(imagesize*sizeof(char));
    minusmask = malloc(imagesize*sizeof(float));
    if((!mask)||(!minusmask))
      {printf("ERROR: Couldn't allocate mask\n"); return 1;}

    // user might have passed 1 or 3 files
    // note: defects are char, hence, inputtype=-1
    read1or3panels_char(-1, ndefects, defectfilename1, defectfilename2, 
                   defectfilename3, panelxsize,  panelysize, mask) ;

    //Do defect correction
    if(defecttype==1)
      {
        
        //Calculate weighted array's size
        gridwidth=blursize*2 + 1;
        gridsize=gridwidth*gridwidth;
        centerpoint=blursize;
        
        //Generate weighted array
        grid=malloc(gridsize*sizeof(int));
        for(i=0;i<gridwidth;i++)
          {
            for(j=0;j<gridwidth;j++)
              {
                xdist=abs(j-centerpoint);
                ydist=abs(i-centerpoint);
                // hmjb why the maximum in 1-D ? bug? 
                // I think it should be the squared distance
                dist=centerpoint-max(xdist,ydist)+1;
                dist=dist*dist;
                grid[i*gridwidth+j]=dist;
              }
          }
        
        for(y=0;y<ysize;y++) {
          for(x=0;x<xsize;x++) {
            //hmjb, ok mask=0 means error
            if(mask[y*xsize+x]==0) {
              average=0; weightcount=0;
              for(i=0;i<=gridwidth;i++) {
                for(j=0;j<=gridwidth;j++) {
                  xoffset=centerpoint-i;
                  yoffset=centerpoint-j;
                  if((x+xoffset>=0)&&(x+xoffset<xsize)&&
                     (y+yoffset>=0)&&(y+yoffset<ysize)) {
                    if(mask[(y+yoffset)*xsize+(x+xoffset)]==1) {
                      average+=(float)(outimage[(y+yoffset)*xsize+(x+xoffset)])*
                        (float)(grid[i*gridwidth+j]);
                      weightcount+=(float)(grid[i*gridwidth+j]);
                    } 
                  }
                }
              }
              if(weightcount>0) {average=average/weightcount;}
              else {average=0;}
              outimage[y*xsize+x]=(unsigned short int) average;
              mask[y*xsize+x]=2;
            }
          }
        }

        free(grid);
      }
    else if(defecttype==2)
      {
        //Prepare mask: Convert mask to +/- mean values. #1: Calculate mean
        
        //hmjb !!! come on, we cant' do this loop without initializing
        //the variable first!!!!
        maskmean=0;//hmjb
        for(y=0;y<ysize;y++) {
          for(x=0;x<xsize;x++) {
            maskmean+=mask[y*xsize+x];}}
        maskmean=maskmean/imagesize;
        
        float maxminus=-999999999;
        float minminus=999999999;
        //Convert mask to +/- mean values. #2: Subtract mean from all mask pixels
        for(y=0;y<ysize;y++) {
          for(x=0;x<xsize;x++) {
            minusmask[y*xsize+x]=maskmean - mask[y*xsize+x];
            if(minusmask[y*xsize+x]>maxminus){maxminus=minusmask[y*xsize+x];}
            if(minusmask[y*xsize+x]<minminus){minminus=minusmask[y*xsize+x];}
          }
        }
        //Add minusmask to image
        for(i=0;i<imagesize;i++)
          {outimage[i] = (float)(outimage[i]+minusmask[i]);}
      }
    else if(defecttype==3) {
      for(y=0;y<ysize;y++) {
        for(x=0;x<xsize;x++) {
          // frank is trying to avoid that we use a fillval value that
          // is present in the data... however, we shouldn't change
          // the value in the image! never!!! We should HALT the
          // program and tell the user to use a different fillvalue.

          //hmjb 25/aug/2017
          // can't do that anymore, because now we are applying the flat field.
          // Where we don't have the flat, we already but fillval. So there will
          // be a lot of points where the image is == fillval
          //if(outimage[y*xsize+x]==fillval) {
          //  //hmjb outimage[y*xsize+x]=fillval-1;
          //  printf("ERROR: cannot use --fillval=%d because it is a valid image value!\n",fillval);
          //  printf("pos=%d value=%f\n",y*xsize+x,outimage[y*xsize+x]);
          //  return 1;
          //}
          if(mask[y*xsize+x]==0) {outimage[y*xsize+x]=fillval;}
        }
      }
    }
    else if(defecttype==4) {
      printf("The stdev-modified weighting algorithm?  Good choice! Not implemented yet though.\n");}
    free(mask);
    free(minusmask);
  } else printf("SKIP: defect\n");

  //printf("xhmjb: %f \n",outimage[panelxsize/2+(508)*xsize]); 

  //----------------------------------------------------------------------------
  //Finally, do alignment correction
  //----------------------------------------------------------------------------

  //printf("alignment=%d\n",doalignment);
  if(doalignment)
    {
      //printf("entrou...\n");
      //Malloc copy image
      copyimage=malloc(imagesize*sizeof(float));
      copycount=malloc(imagesize*sizeof(int));
      if((!copyimage)||(!copycount)){printf("ERROR: Couldn't allocate copyimage\n"); return 1;}
      for(i=0;i<imagesize;i++) {
        copyimage[i]=outimage[i];
        outimage[i]=0;
        copycount[i]=0;
      }
      
      //open alignment file, read arguments
      alignmentfile=fopen(alignmentfilename, "r");
      if(!alignmentfile)
        {printf("ERROR: Couldn't open alignmentfile %s\n", alignmentfilename); return 1;}
      for(i=0;i<npanels;i++) {
        fscanf(alignmentfile, "%d %d %f %f %f\n",
               transx+i, transy+i, rotate+i, scalex+i, scaley+i);

        //printf("i=%d  Tx=%d Ty=%d rot=%f %f %f\n",i,transx[i],transy[i],rotate[i],scalex[i],scaley[i]);
      }
      fclose(alignmentfile);
      
      for(i=0;i<npanels;i++) //panel loop
        {
          Tx=transx[i];
          Ty=transy[i];
          theta=rotate[i];
          costheta=cos(theta*DTOR);
          sintheta=sin(theta*DTOR);
          Sx=scalex[i];
          Sy=scaley[i];
          //printf("panel = %d   %d   %d   %d   %d\n",i, Tx, Ty, panelxsize, panelysize);

          for(y=0;y<panelysize;y++) {
            for(x=0;x<panelxsize;x++) {
              midx = x - (panelxsize/2) + Tx;
              midy = y - (panelysize/2) + Ty;
              newx=Sx*midx*costheta + Sy*midy*sintheta + (panelxsize/2);
              newy=-Sx*midx*sintheta + Sy*midy*costheta + (panelysize/2);
              if((newx>=0)&&(newx<panelxsize)&&
                 (newy>=0)&&(newy<panelysize)) {
                outimage[(((int)newy)*xsize)+(i*panelxsize)+((int)newx)]=
                  copyimage[(y*xsize)+(i*panelxsize)+x];
                copycount[(((int)newy)*xsize)+(i*panelxsize)+((int)newx)]=1;

                //if ((i==1) && (y==540) && (x==1570)) {
                //  printf("from=%d to=%d\n",(y*xsize)+(i*panelxsize)+x,(((int)newy)*xsize)+(i*panelxsize)+((int)newx));
                //}
              }
            }
          }
          /*
          if (i==1) {
            for(y=0;y<30;y++) {
              printf("y=%d  new=%f  orig=%f\n",y,
                     outimage[i*panelxsize+y], 
                     copyimage[i*panelxsize+y]);
            }


            //x=1570;
            //for(y=480;y<580;y++) {
            //  printf("x=%d y=%d  new=%f  orig=%f\n",x,y,
            //         outimage[(y*xsize)+(i*panelxsize)+x], 
            //         copyimage[(y*xsize)+(i*panelxsize)+x]);
            //}
            //
            //y=540;
            //for(x=1500;x<1600;x++) {
            //  printf("x=%d y=%d  new=%f  orig=%f\n",x,y,
            //         outimage[(y*xsize)+(i*panelxsize)+x],
            //         copyimage[(y*xsize)+(i*panelxsize)+x]);
            //}
            }*/
        }//End panel loop
      
      //printf("interp= %d\n",dointerpolate);
      if(dointerpolate) {
        for(y=0;y<ysize;y++) {
          for(x=0;x<xsize;x++) {
            if(copycount[y*xsize+x]==0) {
              interpvalue=0;
              interpcount=0;
              for(i=-1;i<=1;i++) {
                for(j=-1;j<1;j++) {
                  neighborx=x+j;
                  neighbory=y+i;
                  if((neighborx>=0)&&(neighborx<xsize)&&
                     (neighbory>=0)&&(neighbory<ysize)) {
                    interpvalue+=outimage[neighbory*xsize+neighbory];
                    interpcount++;
                  }
                }
              }
              outimage[y*xsize+x]=interpvalue/interpcount;
            }
          }
        }
      }
      
      free(copyimage);
      free(copycount);
    } //else printf("SKIP: alignment\n");
  /*
  for (i=0;i<3;i++) {
    printf("panel=%d\n",i);
    for(y=0;y<30;y++) {
      printf("y=%d  pos=%d new=%f  orig=%d\n",y,
             i*panelxsize+y,
             outimage[i*panelxsize+y], 
             inimage[i*panelxsize+y]);
    }
  }

  
  i=1;
  x=1570;
  for(y=480;y<580;y++) {
    printf("x=%d y=%d  new=%f  orig=%d\n",x,y,
           outimage[(y*xsize)+(i*panelxsize)+x], 
           inimage [(y*xsize)+(i*panelxsize)+x]);
  }
  
  y=540;
  for(x=1500;x<1600;x++) {
    printf("x=%d y=%d  new=%f  orig=%d\n",x,y,
           outimage[(y*xsize)+(i*panelxsize)+x],
           inimage [(y*xsize)+(i*panelxsize)+x]);
  }
  */
  //printf("xhmjb: %f \n",outimage[panelxsize/2+(508)*xsize]); 
  //----------------------------------------------------------------------------
  /*Write image to file*/
  //----------------------------------------------------------------------------
  //printf("chegou aqui... \n");
  //printf("%s\n",outfilename1);
  //printf("%s\n",outfilename2);
  //printf("%s\n",outfilename3);
  for (i=0; i<noimages; i++) {
    //printf("%d\n",i);
    if (i==0) afile=outfilename1;
    else if (i==1) afile=outfilename2;
    else if (i==2) afile=outfilename3;
    
    outfile=fopen(afile, "w");
    if(!outfile)
      {printf("ERROR: Couldn't open outfile %s\n", afile); return 1;}
    
    result=0;
    for(y=0;y<panelysize;y++) 
      for(x=0;x<panelxsize*3/noimages;x++) 
        result+=fwrite(&outimage[y*xsize+x+i*panelxsize],sizeof(float),1,outfile);

    if(result != imagesize/noimages) {
      printf("ERROR: Couldn't write to outfile %s, only %d of %d\n",
             afile, result, imagesize/noimages);
      return 1;}

    fclose(outfile);    
  }



  free(outimage);  
  return 0;
}

int max(int a, int b)
{
  if(a>b) {return a;}
  else {return b;}
}

int min(int a, int b)
{
  if(a<b) {return a;}
  else {return b;}
}


void read1or3panels_char(int inputtype, int nimages, char *file1, char*file2, char*file3,
                    int panelxsize, int panelysize, char*outimage)
{
  //int panelsize=panelxsize*panelysize;
  int xsize=panelxsize*3;
  int ysize=panelysize;
  int imagesize = xsize*ysize;
  int i, x, y;
  char* afile;

  char *aimage;
  
  // user might have passed 1 or 3 files
  for (i=0; i<nimages; i++) {
    if (i==0) afile=file1;
    else if (i==1) afile=file2;
    else if (i==2) afile=file3;

    // allocate enough memory for each user file
    aimage = malloc(imagesize*sizeof(char)/nimages);
    if(!aimage)
      {printf("ERROR: Couldn't allocate aimage \n"); return;}

    // integer 32bit unpacked
    if(inputtype==-1) {
      readraw_char(afile, aimage, xsize/nimages, ysize);
    }
    else
      {printf("ERROR: Invalid input type\n"); return;}

    // copy to correct position 
    for(y=0;y<panelysize;y++) 
      for(x=0;x<panelxsize*3/nimages;x++) 
        outimage[y*xsize+x+i*panelxsize]=aimage[y*panelxsize+x];

    free(aimage);
  }
}

void read1or3panels_float(int inputtype, int nimages, char *file1, char*file2, char*file3,
                    int panelxsize, int panelysize, float*outimage)
{
  //int panelsize=panelxsize*panelysize;
  int xsize=panelxsize*3;
  int ysize=panelysize;
  int imagesize = xsize*ysize;
  int i, x, y;
  char* afile;

  float *aimage;
  
  // user might have passed 1 or 3 files
  for (i=0; i<nimages; i++) {
    if (i==0) afile=file1;
    else if (i==1) afile=file2;
    else if (i==2) afile=file3;

    // allocate enough memory for each user file
    aimage = malloc(imagesize*sizeof(float)/nimages);
    if(!aimage)
      {printf("ERROR: Couldn't allocate aimage \n"); return;}

    // integer 32bit unpacked
    if(inputtype==3) {
      readraw_float(afile, aimage, xsize/nimages, ysize);
    }
    else
      {printf("ERROR: Invalid input type\n"); return;}

    // copy to correct position 
    for(y=0;y<panelysize;y++) 
      for(x=0;x<panelxsize*3/nimages;x++) 
        outimage[y*xsize+x+i*panelxsize]=aimage[y*panelxsize+x];

    free(aimage);
  }
}

void read1or3panels_uint32(int inputtype, int nimages, char *file1, char*file2, char*file3,
                    int panelxsize, int panelysize, unsigned long int*outimage)
{
  //int panelsize=panelxsize*panelysize;
  int xsize=panelxsize*3;
  int ysize=panelysize;
  int imagesize = xsize*ysize;
  int i, x, y;
  char* afile;

  unsigned long int *aimage;
  
  // user might have passed 1 or 3 files
  for (i=0; i<nimages; i++) {
    if (i==0) afile=file1;
    else if (i==1) afile=file2;
    else if (i==2) afile=file3;

    // allocate enough memory for each user file
    aimage = malloc(imagesize*sizeof(unsigned long int)/nimages);
    if(!aimage)
      {printf("ERROR: Couldn't allocate aimage \n"); return;}

    // integer 32bit unpacked
    if(inputtype==2) {
      readraw_uint32(afile, aimage, xsize/nimages, ysize);
    }
    else
      {printf("ERROR: Invalid input type\n"); return;}

    // copy to correct position 
    for(y=0;y<panelysize;y++) 
      for(x=0;x<panelxsize*3/nimages;x++) 
        outimage[y*xsize+x+i*panelxsize]=aimage[y*panelxsize+x];

    free(aimage);
  }
}

void read1or3panels(int inputtype, int nimages, char *file1, char*file2, char*file3,
                    int panelxsize, int panelysize, unsigned short int*outimage)
{
  //int panelsize=panelxsize*panelysize;
  int xsize=panelxsize*3;
  int ysize=panelysize;
  int imagesize = xsize*ysize;
  int i, x, y;
  char* afile;

  unsigned short int *aimage;
  
  // user might have passed 1 or 3 files
  for (i=0; i<nimages; i++) {
    if (i==0) afile=file1;
    else if (i==1) afile=file2;
    else if (i==2) afile=file3;

    // allocate enough memory for each user file
    aimage = malloc(imagesize*sizeof(unsigned short int)/nimages);
    if(!aimage)
      {printf("ERROR: Couldn't allocate aimage \n"); return;}

    // integer 12bit packed
    if(inputtype==0) {
      readraw_uint12(afile, aimage, xsize/nimages, ysize);
    }
    // integer 16bit unpacked
    else if(inputtype==1) {
      readraw_uint16(afile, aimage, xsize/nimages, ysize);
    }
    else
      {printf("ERROR: Invalid input type\n"); return;}

    // copy to correct position 
    for(y=0;y<panelysize;y++) 
      for(x=0;x<panelxsize*3/nimages;x++) 
        outimage[y*xsize+x+i*panelxsize]=aimage[y*panelxsize+x];

    free(aimage);
  }
}

//inputtype=3
void readraw_float(char *infilename, float *outimage, int outxsize,
             int outysize)
{
  int outsize = outxsize*outysize;
  int result;
  FILE *infile;

  infile = fopen(infilename, "r");
  if(!infile) 
    {printf("ERROR: Couldn't open infile %s\n", infilename); return;}

  result = fread(outimage, sizeof(float), outsize, infile);
  if(result != outsize)
    {printf("ERROR: Couldn't read infile %s, only %d of %d read\n",
            infilename, result, outsize);}
  fclose(infile);
}

//inputtype=2
void readraw_uint32(char *infilename, unsigned long int*outimage, int outxsize,
             int outysize)
{
  int outsize = outxsize*outysize;
  int result;
  FILE *infile;

  infile = fopen(infilename, "r");
  if(!infile) 
    {printf("ERROR: Couldn't open infile %s\n", infilename); return;}

  result = fread(outimage, sizeof(unsigned long int), outsize, infile);
  if(result != outsize)
    {printf("ERROR: Couldn't read infile %s, only %d of %d read\n",
            infilename, result, outsize);}
  fclose(infile);
}

//inputtype=1
void readraw_uint16(char *infilename, unsigned short int *outimage, int outxsize,
             int outysize)
{
  int outsize = outxsize*outysize;
  int result;
  FILE *infile;

  infile = fopen(infilename, "r");
  if(!infile) 
    {printf("ERROR: Couldn't open infile %s\n", infilename); return;}

  result = fread(outimage, sizeof(unsigned short int), outsize, infile);
  if(result != outsize)
    {printf("ERROR: Couldn't read infile %s, only %d of %d read\n",
            infilename, result, outsize);}
  fclose(infile);
}

//inputtype=0
void readraw_uint12(char *infilename, unsigned short int *outimage, int outxsize,
             int outysize)
{
  
  int outsize = outxsize*outysize;
  int insize = (outsize * 12)/16;
  int bothsize = outsize/4;
  if((outsize/4 != insize/3)||(outsize%4)||(insize%3))
    {printf("Whoops! These values are incompatible.\n");}
  
  int result;
  int i;
  FILE *infile;
  unsigned short int *inimage;
  
  infile=fopen(infilename, "r");
  if(!infile)
    {printf("ERROR: Couldn't open infile %s\n", infilename); return;}
  inimage = malloc(insize*sizeof(unsigned short int));
  if(!inimage)
    {printf("ERROR: Couldn't allocate inimage\n"); return;}
  result = fread(inimage, sizeof(unsigned short int), insize, infile);
  if(result != insize)
    {printf("ERROR: Couldn't read infile %s; only %d of %d read\n", infilename, result,
            insize); return;}
  fclose(infile);
  
  for(i=0;i<bothsize;i++)
    {
      //positive is left shift, negative is right shift 
      outimage[(i*4)+0] = inimage[(i*3)+0] & 0xfff;
      outimage[(i*4)+1] = (inimage[(i*3)+0]>>12 | inimage[(i*3)+1]<<4) & 0xfff;
      outimage[(i*4)+2] = (inimage[(i*3)+1]>>8 | inimage[(i*3)+2]<<8) & 0xfff;
      outimage[(i*4)+3] = inimage[(i*3)+2]>>4;
    }
  
  free(inimage);
  return;
}

//inputtype=-1
void readraw_char(char *infilename, char *outimage, int outxsize,
             int outysize)
{
  int outsize = outxsize*outysize;
  int result;
  FILE *infile;

  infile = fopen(infilename, "r");
  if(!infile) 
    {printf("ERROR: Couldn't open infile %s\n", infilename); return;}

  result = fread(outimage, sizeof(char), outsize, infile);
  if(result != outsize)
    {printf("ERROR: Couldn't read infile %s, only %d of %d read\n",
            infilename, result, outsize);}
  fclose(infile);
}
