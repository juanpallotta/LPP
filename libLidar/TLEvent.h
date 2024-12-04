// -*- C++ -*-
#ifndef __TLEvent_H
#define __TLEvent_H

#include "TObject.h"
#include "TDatime.h"
#include "TClonesArray.h"
#include "TString.h"
#include "LicelData.h"

class TLRunHeader : public TObject {
protected:
  TDatime fStartOfRun;
  Int_t   fNEvents;
  Int_t   fNModules;
  Int_t  *fPMVoltage; //[fNModules]
  Float_t fLaserPower;
  Int_t   fNChannels;
  Bool_t *fMSBOn;    //[fNModules]
  Bool_t *fLSBOn;    //[fNModules]
  Bool_t *fPhotonOn; //[fNModules]
  TString fLogBook;
  
public:
  TLRunHeader(const int nmodules=kLICEL_MODULES);
  ~TLRunHeader();
  void SetNEvents(const int nevents) { fNEvents=nevents; }
  void SetNChannels(const int nchannels) { fNChannels=nchannels; }
  void SetPMVoltage(const int module, const int voltage); 
  void SetLaserPower(const float power) { fLaserPower=power; }
  void SetStartOfRun() { fStartOfRun.Set(); }
  void SetLogBook(const char* text);
  TString GetLogBook() const { return fLogBook; };
  ClassDef(TLRunHeader,1)
};


class TLRunTrailer : public TObject {
protected:
  TDatime fEndOfRun;
public:
  TLRunTrailer();
  ~TLRunTrailer();
  void SetEndOfRun() { fEndOfRun.Set(); }
  ClassDef(TLRunTrailer,1)
};

class TLEventHeader : public TObject {
protected:
  Float_t fZenith;
  Float_t fAzimuth;
  Int_t fNLaserShots;
  TDatime fTimeStamp;
  Int_t fEventNumber; 

public:
    TLEventHeader();
    ~TLEventHeader();
    void SetZenith(const float zenith) { fZenith=zenith; }
    void SetAzimuth(const float azimuth) { fAzimuth=azimuth; }
    void SetNLaserShots(const int shots) { fNLaserShots=shots; }
    Int_t GetNLaserShots() const { return fNLaserShots; }
    void SetTimeStamp() { fTimeStamp.Set(); }
    void GetTimeStamp(int *year,int *month,int *day,int *hour,int *min,int *sec);
    Float_t GetZenith() const { return fZenith; }
    Float_t GetAzimuth() const { return fAzimuth; }
    void SetEventNumber(const Int_t nevent) { fEventNumber=nevent; }
    Int_t GetEventNumber() const { return fEventNumber; }
    ClassDef(TLEventHeader,2)
};


class TLTrace : public TObject {
protected:
  Int_t  fLength;
  UShort_t *fData; //[fLength]
public:
  TLTrace(const int length=kLICEL_MAXCHANNELS);
  ~TLTrace();
  void Set(const UShort_t* data);
  void Set(const UInt_t* data);
  void SetLength(const int length);
  UShort_t* Get() const { return fData; }
  Int_t GetLength() const { return fLength; }
  void TDump();
  ClassDef(TLTrace,2)
};

class TLEvent : public TObject {

protected:
  TLEventHeader fEventHeader;
  Int_t  fNModules;
  //LTrace *fMSB;    //[fNModules]
  //LTrace *fLSB;    //[fNModules]
  //LTrace *fPhoton; //[fNModules]
  TClonesArray *fMSB;    //->
  TClonesArray *fLSB;    //->
  TClonesArray *fPhoton; //->
  static TClonesArray *fgMSB;
  static TClonesArray *fgLSB;
  static TClonesArray *fgPhoton;
  Int_t fNShots;

public:
  TLEvent(const int nmodules=kLICEL_MODULES);
  ~TLEvent();
  void SetMSB(const int module, const UShort_t* data);
  void SetLSB(const int module, const UShort_t* data);
  void SetPhoton(const int module, const UShort_t* data);
  void SetMSB(const int module, const UInt_t* data);
  void SetLSB(const int module, const UInt_t* data);
  void SetPhoton(const int module, const UInt_t* data);
  void SetLength(const int length);
  void SetZenith(const float zenith) { fEventHeader.SetZenith(zenith); }
  void SetAzimuth(const float azimuth) { fEventHeader.SetAzimuth(azimuth); }
  void SetNLaserShots(const int shots) { fEventHeader.SetNLaserShots(shots); }
  Int_t GetNLaserShots() const { return fEventHeader.GetNLaserShots(); }
  void SetEventNumber(const int nevent) { fEventHeader.SetEventNumber(nevent); }
  Int_t GetEventNumber() const { return fEventHeader.GetEventNumber(); }
  void SetTimeStamp() { fEventHeader.SetTimeStamp(); }
  void GetTimeStamp(int *y,int *m,int *d,int *h,int *min,int *sec)  { fEventHeader.GetTimeStamp(y,m,d,h,min,sec); }
  void Clear(Option_t *option="");
  void Reset(Option_t *option="");
  void TDump();
  const TLEventHeader* GetEventHeader() const { return &fEventHeader; }
  TLTrace* GetLSBTrace(const int module) const { return (TLTrace*)((*fLSB)[module]); }
  TLTrace* GetMSBTrace(const int module) const { return (TLTrace*)((*fMSB)[module]); }
  UShort_t* GetLSB(const int module) const;
  UShort_t* GetMSB(const int module) const;
  UShort_t* GetPhoton(const int module) const;
  Float_t GetZenith() const { return fEventHeader.GetZenith();}
  Float_t GetAzimuth() const { return fEventHeader.GetAzimuth();}
  //void SetNShots(const int nshots);
  Int_t GetNShots() const { return fNShots; }
  ClassDef(TLEvent,4)
	  /*
I change ClassDef from 3 to 4 for TLEvent because I got this warning:
(only for the "new" data)

 WARNING, the StreamerInfo of class TLEvent read from file new_data
        has the same version (=3) as the active class
        but a different checksum.
        You should update the version to ClassDef(TLEvent,4).
        Do not try to write objects with the current class definition,
        the files will not be readable.

	   */
};

#endif //__TLEvent_H
