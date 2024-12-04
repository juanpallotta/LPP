/** \file
    Interface to the Auger Error logger
    \author Lukas Nellen
    \version $Id: ErrorLogger.h 4799 2006-07-18 12:00:24Z darko $
    \date 24-Jan-2003
*/

#ifndef _utl_ErrorLogger_h_
#define _utl_ErrorLogger_h_

const static char CvsId_utl_ErrorLogger[] = 
  "$Id: ErrorLogger.h 4799 2006-07-18 12:00:24Z darko $";

#include <string>
#include <iostream>
#include <sstream>

namespace utl {

  /**
     \class ErrorLogger ErrorLogger.h "utl/ErrorLogger.h"

     \brief Class to handle routing and writing of error messages

     \todo
     General routing

     \todo
     Improve message formating

  */
  class ErrorLogger {
  public:
    /// Message severity levels
    enum SeverityLevel {
      eDebug   = -1,		///< Debugging message
      eInfo    = 0,		///< General (informational) message
      eWarning = 1,		///< Warning message
      eError   = 2,		///< Error message
      eFatal   = 3		///< Fatal error message
    };

    /// Message verbosity
    enum  VerbosityLevel {
      eDefaultVerbosity = -1,	///< Use the default verbosity
      eTerse,			///< Terse error messages
      eVerbose			///< Verbose error messages
    };
					  
    /// General interface for logging a message
    void Log(const SeverityLevel severity,
	     const std::string functionName,
	     const std::string fileName,
	     const int lineNumber,
	     const std::string message,
	     VerbosityLevel verbosity = eDefaultVerbosity) const;

    /// \brief Dump message from \c ostringstream instead of of a \c string
    void Log(const SeverityLevel severity,
	     const std::string functionName,
	     const std::string fileName,
	     const int lineNumber,
	     const std::ostringstream& message,
	     const VerbosityLevel verbosity = eDefaultVerbosity) const
    { Log(severity, functionName, fileName, lineNumber, message.str(), verbosity); }

    /**
      \brief Set the error logging stream

      \note 
      This method will be replaced by a more general mechanism for the
      routing of error messages.
    */
    void SetStream(std::ostream* const stream) { fOStream = stream; }

    /// Set the verbosity level
    void SetVerbosity(VerbosityLevel verbosity)
    { if (verbosity != eDefaultVerbosity) fVerbosity = verbosity; }

    /// Get the instance of the error logger singleton
    static ErrorLogger &GetInstance();

  private:
    ErrorLogger() : fOStream(0), fVerbosity(eTerse) { }
    ErrorLogger(const ErrorLogger&);
    ErrorLogger& operator=(const ErrorLogger& rhs);

    std::ostream* fOStream;	///< Current stream for logging messages
    VerbosityLevel fVerbosity;	////< Verbosity level
    
    static ErrorLogger* fgErrorLoggerInstance; ///< Instance of the singleton

  };


} // namespace utl


/**
  \brief Standard message logging macro

  This macro is used by the convenience macros defined below to write
  a log message. It automatically sets the function name, the file
  name and the line number.
*/

#define LOG_MESSAGE_(severity, message) \
utl::ErrorLogger::GetInstance().Log(severity, __func__, __FILE__, __LINE__, \
  message)

/// Brief message logging macro - always do \e terse logging
#define LOG_TERSE_MESSAGE_(severity, message) \
utl::ErrorLogger::GetInstance().Log(severity, __func__, __FILE__,  __LINE__, \
  message, utl::ErrorLogger::eTerse)

/**
  \brief Macro for logging debugging messages.
  
  This macro is only active if the \c DEBUG macro is
  defined. Otherwise, debug messages are suppressed at compile time.

  \remark This macro is not named \c DEBUG since \c DEBUG is usually
  used to activate conditionally compiled debugging code.
*/
#ifdef DEBUG 
#  define DEBUGLOG(message) LOG_MESSAGE_(utl::ErrorLogger::eDebug, message)
#else
#  define DEBUGLOG(message)
#endif

/// Macro for logging informational messages
#define INFO(message)     LOG_MESSAGE_(utl::ErrorLogger::eInfo, message)
/// Macro for logging warning messages
#define WARNING(message)  LOG_MESSAGE_(utl::ErrorLogger::eWarning, message)
/// Macro for logging error messages
#define ERROR(message)    LOG_MESSAGE_(utl::ErrorLogger::eError, message)
/// Macro for logging fatal messages
#define FATAL(message)    LOG_MESSAGE_(utl::ErrorLogger::eFatal, message)

/// Macro for logging informational messages
#define INFO_TERSE(message)    LOG_TERSE_MESSAGE_(utl::ErrorLogger::eInfo, \
						  message)
/// Macro for logging warning messages
#define WARNING_TERSE(message) LOG_TERSE_MESSAGE_(utl::ErrorLogger::eWarning, \
                                                  message)
/// Macro for logging error messages
#define ERROR_TERSE(message)   LOG_TERSE_MESSAGE_(utl::ErrorLogger::eError, \
						  message)
/// Macro for logging fatal messages
#define FATAL_TERSE(message)   LOG_TERSE_MESSAGE_(utl::ErrorLogger::eFatal, \
						  message)

#endif // _utl_ErrorLogger_h_

// Configure (x)emacs for this file ...
// Local Variables:
// mode:c++
// compile-command: "make -k -C .. ErrorLogger/ErrorLogger.o"
// End:
