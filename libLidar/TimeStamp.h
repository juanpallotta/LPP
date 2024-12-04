#ifndef _utl_TimeStamp_h_
#define _utl_TimeStamp_h_

#include "Units.h"

#include <string>
#include <ostream>


struct tm;

namespace utl {

  class TimeInterval;

  /**
     \class TimeStamp TimeStamp.h "utl/TimeStamp.h"

     \brief A TimeStamp holds GPS second and nanosecond for some event

     <p> This class contains the GPS second and nanosecond and
     provides methods to convert to seconds since Unix Epoch
     (accounting for leap seconds) as well Modified Julian Date and
     formatted strings with UTC printout consistent with the
     conventions document. A TimeStamp, in contrast to a TimeInterval,
     should represent the time of occurrence of some event. To
     represent time intervals between events, use the TimeInterval
     class. A number of arithmetic operations between TimeStamp and
     TimeInterval objects are defined.

     <p> What is time? This is a brief summary of time systems,
     relevant for the Auger project operation (see
     <a href="http://tycho.usno.navy.mil/gpstt.html">
     http://tycho.usno.navy.mil/gpstt.html</a> for details).

     <ul>

     <li><strong>International Atomic Time (TAI)</strong> is a
     statistical time scale based on a large number of <strong>atomic
     clocks</strong> operating at standards laboratories around the
     world. Its unit interval is exactly one SI second at sea
     level. Neglecting the relativistic effects, this is probably the
     time scale in which Auger physics is happening.

     <li><strong>Universal Time (UT)</strong> (and its variant
     <strong>UT1</strong>) is a time scale with unit of duration the
     <strong>mean solar day</strong>. It is based on the Earth's
     rotation and thus important for transformation of events into the
     galactic coordinates. Otherwise, not very useful for physics.

     <li><strong>Coordinated Universal Time (UTC)</strong> differs
     from TAI by an integral number of seconds. UTC is kept within 0.9
     seconds of UT1 by the introduction of one-second steps to UTC,
     the <strong>leap seconds</strong>. In contrast to UT1, UTC is
     running in parallel to TAI with piecewise constant offset.

     <li><strong>Global Positioning System (GPS)</strong> time is
     synchronized to UTC but it is NOT adjusted for leap
     seconds. Since TAI and GPS epoch did not start on the same date,
     they run in parallel with TAI ahead of GPS by (constant) 19
     seconds. GPS thus can be a candidate for physics time scale.

     </ul>

     \author 	T. Paul
     \author    D. Veberic
     \date   	19 Feb 2003
     \version   $Id: TimeStamp.h,v 1.22 2005/11/26 02:52:37 veberic Exp $
  */

  class TimeStamp {

  public:
    TimeStamp() { fGPSSecond = fGPSNanoSecond = 0; }

    /// Construct from GPS second and nanosecond
    TimeStamp(const unsigned int sec, const unsigned int nsec = 0)
    { fGPSSecond = sec; fGPSNanoSecond = nsec; }

    TimeStamp(const int sec, const int nsec = 0)
    { SetNormalized(sec, nsec); }

    /// Construct from UTC date and time
    /*! <b> WARNING: this constructor does not (currently) carry out a complete
      set of checks that the input numbers are all within sensible ranges </b>
    */
    TimeStamp(const unsigned int year,
	      const unsigned int month,
	      const unsigned int day,
	      const unsigned int hour = 0,
	      const unsigned int minute = 0,
	      const unsigned int second = 0,
	      const unsigned int nanosecond = 0)
    { SetUTCTime(year, month, day, hour, minute, second, nanosecond); }

    TimeStamp(const TimeStamp& ts) :
      fGPSSecond(ts.fGPSSecond), fGPSNanoSecond(ts.fGPSNanoSecond) { }

    TimeStamp& operator=(const TimeStamp& ts)
    { fGPSSecond = ts.fGPSSecond; fGPSNanoSecond = ts.fGPSNanoSecond;
      return *this; }

    // --------
    // Setters
    // --------

    /// Set GPS second and (optionally) nanosecond
    void SetGPSTime(const unsigned int sec, const unsigned int nsec = 0) 
    { fGPSSecond = sec; fGPSNanoSecond = nsec; }

    /// Set UTC date and time
    /*! This method is meant as a convenience for tasks such as 
        converting certain time stamps (for example station 
	or telescope commission times) from human-readable UTC into 
	a TimeStamp.  In other words, one might use this method to 
	convert time stamps stored in an external file where it may
	be more convenient to store UTC than GPS second.
	<b> WARNING: this method does not (currently) carry out a complete
	set of checks that the input numbers are all within sensible ranges </b>
    */
    void SetUTCTime(const unsigned int year, 
		    const unsigned int month,
		    const unsigned int day,
		    const unsigned int hour,
		    const unsigned int minute = 0,
		    const unsigned int second = 0,
		    const unsigned int nanosecond = 0);

    void SetARTTime(const unsigned int year, 
		    const unsigned int month,
		    const unsigned int day,
		    const unsigned int hour,
		    const unsigned int minute = 0,
		    const unsigned int second = 0,
		    const unsigned int nanosecond = 0);

    // -------
    // Getters
    // -------

    /// GPS second
    unsigned int GetGPSSecond() const { return fGPSSecond; }

    /// GPS nanosecond
    unsigned int GetGPSNanoSecond() const { return fGPSNanoSecond; }

    /// Formatted UTC date and time string 
    std::string GetUTCDateTime() const;

    /// Formatted ART date and time string.
    std::string GetARTDateTime() const;

    /// Return UTC date and time as unsigned ints passed in argument list
    /*! note that months are numbered beginning with January = 1 (not 0) */
    void GetUTCDateTime(unsigned int& year,
                        unsigned int& month,
                        unsigned int& day,
                        unsigned int& hour,
                        unsigned int& minute,
                        unsigned int& second,
                        unsigned int& nanosecond) const;

    /// Get the year for UTC 
    unsigned int GetYear() const;

    /// Get the month for UTC.  (range of months is 1-12)
    unsigned int GetMonth() const;

    /// Get the day for UTC
    unsigned int GetDay() const;
    
    /// Get the hour for UTC
    unsigned int GetHour() const;

    /// Get the minute for UTC
    unsigned int GetMinute() const;

    /// Get the second for UTC
    unsigned int GetSecond() const;

    /// Get the nanosecond for UTC
    unsigned int GetNanoSecond() const { return fGPSNanoSecond; }

    // Formatted date and time string for local time zone
    std::string GetLocalDateTime() const;    

    /// Modified Julian Date
    double GetMJD() const;

    // ---------------------
    // Arithmetic operations
    // ---------------------

    /// [TimeStamp] + [TimeInterval] = [TimeStamp]
    TimeStamp operator+(const TimeInterval& ti) const;
    
    /// [TimeStamp] += [TimeInterval] = [TimeStamp]
    TimeStamp& operator+=(const TimeInterval& ti);

    /// [TimeStamp] - [TimeStamp] = [TimeInterval]
    TimeInterval operator-(const TimeStamp& ts) const;

    /// [TimeStamp] - [TimeInterval] = [TimeStamp]
    TimeStamp operator-(const TimeInterval& ti) const;

    /// [TimeStamp] -= [TimeInterval] = [TimeStamp]
    TimeStamp& operator-=(const TimeInterval& ti);

    // ------------------
    // Boolean operations
    // ------------------

    bool operator==(const TimeStamp& ts) const
    { return (fGPSSecond     == ts.fGPSSecond &&
	      fGPSNanoSecond == ts.fGPSNanoSecond); }

    bool operator!=(const TimeStamp& ts) const
    { return !operator==(ts); }

    bool operator>(const TimeStamp& ts) const
    { return (fGPSSecond > ts.fGPSSecond ||
	      (fGPSSecond     == ts.fGPSSecond &&
	       fGPSNanoSecond >  ts.fGPSNanoSecond)); }

    bool operator>=(const TimeStamp& ts) const
    { return !operator<(ts); }

    bool operator<(const TimeStamp& ts) const
    { return (fGPSSecond < ts.fGPSSecond ||
	      (fGPSSecond     == ts.fGPSSecond &&
	       fGPSNanoSecond <  ts.fGPSNanoSecond)); }

    bool operator<=(const TimeStamp& ts) const
    { return !operator>(ts); }

    operator bool() { return fGPSSecond || fGPSNanoSecond; }

    bool operator!() { return !operator bool(); }

  private:
    // with this we will have a y2k problem in approx. 136 years -
    // or for some operations in 68 years - from 1980 (GPS epoch)
    unsigned int fGPSSecond;
    unsigned int fGPSNanoSecond;

    /// Take care of positive nanosecond and GPS epoch
    void SetNormalized(int sec, int nsec);

    /// Puts the tm structure into format defined in conventions document
    std::string GetFormattedFromTm(const tm* const timeStruct) const;

    /// Get seconds after Unix Epoch
    time_t GetUnixSecond() const;
    
    /// Calculate leap seconds since GPS Epoch.
    unsigned int GetLeapSeconds() const;

    /// Check if the time sits directly on a leap second
    bool IsLeapBoundary() const;

  public:
    static const TimeStamp kMaxTimeStamp;

    // ------------
    // Conveniences
    // ------------
    friend std::ostream& operator<<(std::ostream& os, const TimeStamp& ts);

  };


  std::ostream& operator<<(std::ostream& os, const TimeStamp& ts);


} // namespace utl


#include "TimeStamp.inl"


#endif // _utl_TimeStamp_h_

