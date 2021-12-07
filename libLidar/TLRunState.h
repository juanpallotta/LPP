// -*- C++ -*-
#ifndef __TLRunState_h
#define __TLRunState_h


#include <TObject.h>
#include <TObjString.h>
#include <TSystem.h>
#include <TArrayI.h>
#include <TDatime.h>

class TLRunState : public TObject {

public:
  TDatime fStartOfRun;
  TObjString fLogBook;
  Int_t fNLicelModules;
  TArrayI fPMVoltage;
  Int_t fNLicelChannels;
  Int_t fNEvents;
  Int_t fNShotsPerEvent;
  Float_t fLaserPower;
  Int_t fLaserFrequency;
public:
  TLRunState();
  ~TLRunState() {};

  void SetNEvents(int nevents) {
    fNEvents=nevents;
  }
  void SetLogBook(const char* logbook) {
    fLogBook=logbook;
  }
  void SetNLicelChannels(const int nchannels) {
    fNLicelChannels=nchannels;
  }
  void SetNShotsPerEvent(const int shots) {
    fNShotsPerEvent=shots;
  }
  void SetLaserPower(const float laserpower) {
    fLaserPower=laserpower;
  }
  void SetLaserFrequency(const int freq) {
    fLaserFrequency=freq;
  }
  void SetPMVoltage(const int module, const int voltage) {
    fPMVoltage[module]=voltage;
  }

  TDatime GetStartOfRun() const {
    return fStartOfRun;
  }
  Int_t GetNEvents() const {
    return fNEvents;
  }
  Int_t GetNLicelModules() const {
    return fNLicelModules;
  }
  Int_t GetNLicelChannels() const {
    return fNLicelChannels;
  }
  Int_t GetPMVoltage(const int module) const {
    return fPMVoltage[module];
  }
  TObjString GetLogBook() const {
    return fLogBook;
  }
  Float_t GetLaserPower() const {
    return fLaserPower;
  }
  Int_t GetNShotsPerEvent() const {
    return fNShotsPerEvent;
  }
  ClassDef(TLRunState,1)
};
#endif //__TLRunState_h
