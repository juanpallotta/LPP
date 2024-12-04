#ifndef _utl_TimeInterval_h_
#define _utl_TimeInterval_h_

#include <iostream>

namespace utl {

  /**
     \class TimeInterval TimeInterval.h "utl/TimeInterval.h"

     \brief A TimeInterval is used to represent time elapsed between two events

     This class uses a double precision floating point to represent the 
     elapsed time between two events.  52 bits are allocated for the
     mantissa of a double precision number, this interval is precise to 
     about 1 part in 5.4e15.  If for example 1 ns precision were required,
     one could use this class to describe intervals up to about 62 days.
     Basic arithmetic operations are defined for TimeInterval.

     \author 	T. Paul
     \author    D. Veberic
     \date   	19 Feb 2003
     \version   $Id: TimeInterval.h,v 1.15 2005/05/19 13:29:30 paul Exp $  
  */


  class TimeInterval {

  public:

    TimeInterval(const double interval = 0.) :
      fInterval(interval) { }

    TimeInterval(const TimeInterval& ti) :
      fInterval(ti.fInterval) { }

    TimeInterval& operator=(const TimeInterval& ti)
    { fInterval = ti.fInterval; return *this; }

    /// [TimeInterval] = double
    TimeInterval& operator=(const double& d)
    { fInterval = d; return *this; }

    /// Get lower boundary on number of seconds in the interval
    int GetSecond() const;

    /// Get integer number of nanoseconds past seconds boundary
    unsigned int GetNanoSecond() const;

    /// Get the time interval as a double (in Auger base units)
    double GetInterval() const { return fInterval; }

    /// [TimeInterval] + [TimeInterval] = [TimeInterval]
    TimeInterval operator+(const TimeInterval& ti) const
    { return TimeInterval(fInterval + ti.fInterval); }

    /// [TimeInterval] - [TimeInterval] = [TimeInterval]
    TimeInterval operator-(const TimeInterval& ti) const
    { return TimeInterval(fInterval - ti.fInterval); }

    /// [TimeInterval] * [double] = [TimeInterval]
    TimeInterval operator*(const double& d) const
    { return TimeInterval(fInterval * d); }

    /// [double] * [TimeInterval] = [TimeInterval]
    friend TimeInterval operator*(const double& d, const TimeInterval& ti)
    { return TimeInterval(d * ti.fInterval); }

    /// [TimeInterval] / [TimeInterval] = [double]
    double operator/(const TimeInterval& ti) const 
    { return fInterval / ti.fInterval; } 

    /// [TimeInterval] / [double] = [TimeInterval]
    TimeInterval operator/(const double& d) const
    { return TimeInterval(fInterval / d); }

    /// [TimeInterval] += [TimeInterval] = [TimeInterval]
    TimeInterval operator+=(const TimeInterval& ti)
    { fInterval += ti.fInterval; return *this; }

    /// [TimeInterval] -= [TimeInterval] = [TimeInterval]
    TimeInterval& operator-=(const TimeInterval& ti)
    { fInterval -= ti.fInterval; return *this; }

    /// [TimeInterval] *= [TimeInterval] = [TimeInterval]
    TimeInterval& operator*=(const double& d)
    { fInterval *= d; return *this; }

    /// [TimeInterval] /= [double] = [TimeInterval]
    TimeInterval& operator/=(const double& d)
    { fInterval /= d; return *this; }

    /// unary sign reversal
    TimeInterval operator-() const
    { return TimeInterval(-fInterval); }

    bool operator>(const TimeInterval& ti) const
    { return (fInterval > ti.fInterval); }

    bool operator>=(const TimeInterval& ti) const
    { return (fInterval >= ti.fInterval); }

    bool operator<(const TimeInterval& ti) const
    { return (fInterval < ti.fInterval); }

    bool operator<=(const TimeInterval& ti) const
    { return (fInterval <= ti.fInterval); }

    bool operator==(const TimeInterval& ti) const
    { return (fInterval == ti.fInterval); }

    bool operator!=(const TimeInterval& ti) const
    { return (fInterval != ti.fInterval); }

    /// if (TimeInterval) ...
    // i!! implicit casts to bool may be dangerous: consider the case of
    // programmer mistakenly writing double = TimeInterval. there will be no
    // compiler warning but the result will be 0. or 1. only, since
    // without cast to double, it finds cast to bool and then to double again.
    // for numerical classes the higher level built-in types MUST be
    // provided. darko.
    //operator bool() const { return fInterval; }
    operator double() const { return fInterval; }

    /// if (!TimeInterval) ...
    bool operator!() const { return !fInterval; }

    /// stream output
    friend std::ostream& operator<<(std::ostream& os, const TimeInterval& ti)
    { return os << ti.fInterval; }

    static const TimeInterval kMaxTimeInterval;

   private:
    /** stored in Auger base time units
        (see Utilities/Units/include/utl/AugerUnits.h)
    */
    double fInterval;

  };

} // namespace utl

#endif // _utl_TimeInterval_h_

