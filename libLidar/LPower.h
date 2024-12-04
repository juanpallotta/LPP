// -*- C++ -*-
#ifndef LPOWER_H
#define LPOWER_H

#include "TLEvent.h"

class LArrayD;

class LPower {
public:
  //licel const
// PARA CORRERLO LOCALMENTE EN MI PC O EN LA DE CITEDEF  
//  static const double pm_gain=7.81e3/4096/10.; //photons ???
//  static const double power_search_min=1.8;
//  static const double power_search_max=2.2;
//  static const double power_offset_frac=0.4;
//  static const int    licel_trigger_delay=0 ; // 16 [bins]
//  static const double licel_dr=7.5e-3/2.; //km

// SOLUCION DIEGO PARA ROOT 6.XX Y UBUNTU 18.04
  static constexpr double   pm_gain=7.81e3/4096/10.; //photons ???
  static constexpr double   power_search_min=1.8;
  static constexpr double   power_search_max=2.2;
  static constexpr double   power_offset_frac=0.4;
  static constexpr int      licel_trigger_delay=0 ; // 16 [bins]
  static constexpr double   licel_dr=7.5e-3/2.; //km

  // raw
  int raw_n;
  int *rawdata; // RAW DATA READ FROM FILE
  int *countData ; // RAW DATA READ FROM FILE
  int data_n;
  int *data;
  int avg;
  int add;
  // power return
  int power_n;
  double* power_r;
  double* power_h;
  double* power;
  double* sigma_power;
  double* s;
  double* sigma_s;
  double dr,dh;
  int r0_n;
  double r0,h0;
  double rm,hm;
  double zenith,azimuth;
  //new
  void ClearData();
  void AddData(const TLEvent* e, const int module, const double zenith_offset=0, const double azimuth_offset=0);
  void SetData(const TLEvent* e, const int module, const double zenith_offset=0, const double azimuth_offset=0);
  void DecodeLicel(const UShort_t* lsb, const UShort_t* msb);
  void MakeAvg(const int avg);
  void MakePower();
  int  GetNch_in_File( const TLEvent* ) ;
  void Get_nCh_nBinsRaw( const TLEvent*, int*, int* ) ;

  void SetPower(const TLEvent* e, const int module, const int avg, const double zenith_offset=0, const double azimuth_offset=0)
  {
    SetData(e,module,zenith_offset,azimuth_offset);
    MakeAvg(avg);
    MakePower();
  };
  bool GetSSigmaS(double& s, double& sigma_s, const double h);
  // Klett's alpha
  double kalpha_k;
  int kalpha_n;
  double* kalpha_r;
  double* kalpha_h;
  double* kalpha;
  double* ksigma_alpha;
  //
  void MakeKlett(const double k, double rm, const double am);
  //
  LPower();
  ~LPower();
};

//=======================================================

class LArrayD {
public:
  int fN;
  double* fArray;
  //
  void Set(const int n);
  //
  template<class T>
  void Set(const int n, const T* array);
  //
  void Clear() { delete[] fArray; fN=0; fArray=NULL; };
  void Add(const LArrayD* array);
  LArrayD() : fN(0), fArray(NULL) {};
  LArrayD(const int n) : fN(n), fArray(new double[n]) {};
  //
  template<class T>
  LArrayD(const int n, const T* array) : fArray(NULL) {
    Set(n,array);
  };
  //
  ~LArrayD() { delete[] fArray; };
};

inline
void
LArrayD::Set(const int n)
{

  delete[] fArray;
  fN=n;
  fArray=new double[n];
}

template<class T>
void
LArrayD::Set(const int n, const T* array)
{
  delete[] fArray;
  fN=n;
  fArray=new double[n];
  for(int i=0;i<n;++i)
    fArray[i]=array[i];
}

inline
void
LArrayD::Add(const LArrayD* array)
{
  if(fN==array->fN)
    for(int i=0;i<fN;++i)
      fArray[i]+=array->fArray[i];
}

//==========================================================

#define SQR(x) ((x)*(x))

inline
double
interp(const double x1, const double y1,
       const double x2, const double y2,
       const double x)
{
  const double eps=(x2-x)/(x2-x1);
  return y1*eps+y2*(1.-eps);
}

inline
double
sigma2_interp(const double x1, const double y1,
         const double x2, const double y2,
         const double x)
{
  const double eps=(x2-x)/(x2-x1);
  return SQR(y1*eps)+SQR(y2*(1.-eps));
}

#endif
