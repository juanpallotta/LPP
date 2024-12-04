
// the following has to be done in this manner in order to keep
// some important operators inline

#include "Units.h"
#include "TimeInterval.h"

namespace utl {

  inline
  TimeStamp
  TimeStamp::operator+(const TimeInterval& ti)
    const
  {
    // proper normalization is done by the TimeStamp ctor
    return
      TimeStamp(int(fGPSSecond + ti.GetSecond()),
		int(fGPSNanoSecond + ti.GetNanoSecond()));
  }


  inline
  TimeStamp&
  TimeStamp::operator+=(const TimeInterval& ti)
  {
    SetNormalized(fGPSSecond + ti.GetSecond(),
		  fGPSNanoSecond + ti.GetNanoSecond());
    return *this;
  }


  inline
  TimeInterval
  TimeStamp::operator-(const TimeStamp& ts)
    const
  {
    return
      TimeInterval(
	(int(fGPSSecond) - int(ts.fGPSSecond))         * second +
	(int(fGPSNanoSecond) - int(ts.fGPSNanoSecond)) * nanosecond
      );
  }


  inline
  TimeStamp
  TimeStamp::operator-(const TimeInterval& ti)
    const
  {
    // proper normalization is done by the TimeStamp ctor
    return
      TimeStamp(int(fGPSSecond) - int(ti.GetSecond()),
		int(fGPSNanoSecond) - int(ti.GetNanoSecond()));
  }


  inline
  TimeStamp&
  TimeStamp::operator-=(const TimeInterval& ti)
  {
    SetNormalized(int(fGPSSecond) - int(ti.GetSecond()),
		  int(fGPSNanoSecond) - int(ti.GetNanoSecond()));
    return *this;
  }

} // namespace utl

