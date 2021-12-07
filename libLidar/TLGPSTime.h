// -*- C++ -*-
#ifndef __TLGPSTIME_H
#define __TLGPSTIME_H

#include "TObject.h"

class TLGPSTime : public TObject {
private:
  UInt_t fSeconds;
  UInt_t fNSeconds;
public:

public:
  TLGPSTime();
  ~TLGPSTime() {};
  void Set(UInt_t seconds, UInt_t nanosecs=0);
  void Set(int year, int month, int day, int hour, int minute, int second, UInt_t nanosecs=0);
  void Set(TLGPSTime& reftime, int hour, int minute, int second, UInt_t nanosecs=0);
  void Set(const TLGPSTime* time) {
    fSeconds=time->fSeconds;
    fNSeconds=time->fNSeconds;
  }
  UInt_t GetTime() { return fSeconds; }
  UInt_t GetNanoSecs() { return fNSeconds; }
  const char* AsString() const;
  ClassDef(TLGPSTime,1)
};

#endif //GPSTIME_H
