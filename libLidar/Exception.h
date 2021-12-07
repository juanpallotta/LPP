
#ifndef __utl_LidarException_h__
#define __utl_LidarException_h__

#include <exception>
#include <string>
#include <sstream>
#include <map>

namespace utl {
  
  // ---------------------------------------------------------------------------
  /// \class LidarException
  /// \brief Base class for all exceptions used in the auger offline code.
  // ---------------------------------------------------------------------------
  class LidarException : public std::exception { 

    public:

      /// Construct exception with message.
      LidarException(const std::string message = std::string()) :
        fMessage(message) { }

      /// Virtual destructor for LidarExceptions.
      virtual ~LidarException() throw () { }

      /// Retrieve the exception name as a string.
      virtual std::string GetExceptionName() const
        { return "Generic Exception"; }

      /// Retrieve the message from the exception.
      std::string GetMessage() const { return fMessage; }

    protected:
      std::string fMessage;

  };


  // ---------------------------------------------------------------------------
  /// \class NonExistentComponentException
  /// \brief Base class for exceptions trying to access non-existing components
  /// The access protocol for composit objects like the lidar::LScan states 
  /// that an exception is thrown when calling the getter function for a 
  /// non-existing sub-component (i.e., the pointer to the sub-component is 
  /// NULL). This class is the base for all exceptions thrown in this case.
  // ---------------------------------------------------------------------------
  class NonExistentComponentException : public LidarException {

    public:

      /// Construct non-existent sub-object exception with message.
      NonExistentComponentException(const std::string message = std::string()) :
        LidarException(message) { }

      /// Retrieve verbose exception name.
      virtual std::string GetExceptionName() const
      { return "Access to non-existent sub-component exception"; }
  };


  // ---------------------------------------------------------------------------
  /// \class IOFailureException
  /// \brief Base class to report exceptions in IO.
  // ---------------------------------------------------------------------------
  class IOFailureException : public LidarException {

    public:

      /// Construct IO Failure exception with message.
      IOFailureException(const std::string message = std::string()) :
        LidarException(message) { }

      /// Retrieve verbose exception name.
      virtual std::string GetExceptionName() const
      { return "IO failure exception"; }
  };


  // ---------------------------------------------------------------------------
  /// \class InvalidIteratorException
  /// \brief Exception for invalid operation on Iterators.
  //
  //   This exception is thrown when one tries to 
  //   - dereference an invalid itertor
  //   - operate on (e.g. increment) an invalid iterator
  //
  //   Iterators can be invalid if they point beyond the end of the container
  //   (the end iterators) or the iterator returned by the post-increment
  //   operator. The details depend on the concrete implementation.
  // ---------------------------------------------------------------------------
  class InvalidIteratorException : public LidarException {

    public:
      /// Construct Invalid Iterator Exception w/ message
      InvalidIteratorException(const std::string message = std::string()) :
        LidarException(message) { }

      /// Retrieve verbose exception name
      virtual std::string GetExceptionName() const
      { return "Invalid Iterator Exception"; }
  };


  // ---------------------------------------------------------------------------
  /// \class OutOfBoundException
  /// \brief Exception for reporting variable out of valid range.
  // ---------------------------------------------------------------------------
  class OutOfBoundException : public LidarException {

    public:
      /// Construct Invalid Iterator Exception w/ message
      OutOfBoundException(const std::string message = std::string()) :
        LidarException(message) { }

      /// Retrieve verbose exception name
      virtual std::string GetExceptionName() const
      { return "Out of Bounds Exception"; }
  };


  // ---------------------------------------------------------------------------
  /// \class InitSequenceException LidarException.h "utl/LidarException.h"
  /// \brief Exception to use if sequence of initializations violated.
  //
  //   This exception is thrown when one tries to 
  //   - Use an object or class before a required initialisation routine
  //   has been called
  //   - If one tried to re-initialize an object or class that has already 
  //   been initalized
  //
  //   This exception is intended e.g. for registries that require some
  //   extra information to get initialised. 
  // ---------------------------------------------------------------------------
  class InitSequenceException : public LidarException {

    public:
      /// Construct Invalid Iterator Exception w/ message
      InitSequenceException(const std::string message = std::string()) :
        LidarException(message) { }

      /// Retrieve verbose exception name
      virtual std::string GetExceptionName() const
      { return "Invalid order of initialisation and/or use"; }
  };


  // ---------------------------------------------------------------------------
  /// \class ModuleSequenceException LidarException.h "utl/LidarException.h"
  /// \brief Exception to use in case of module sequencing failures
  //
  //  This exception is thrown when one  
  //   - Requests module that has no module creator
  //   - Encounters an unknown instruction from the sequencing file
  //   - Receives a VModule::eFailure flag from a VModule method
  //
  //   This exception is mainly meant to be used by the RunController in
  //   cases where module sequencing fails.
  // ---------------------------------------------------------------------------
  class ModuleSequenceException : public LidarException {

    public:

      ModuleSequenceException(const std::string message = std::string()) :
        LidarException(message) { }

      virtual std::string GetExceptionName() const
      { return "Error during module sequencing"; }
  };


  // ---------------------------------------------------------------------------
  /// \class NoDataForModelException
  /// \brief Exception to use when an atmosphere model cannot find data it
  ///        needs.
  //
  //  This exception is thrown in cases when an atmosphere model cannot fulfill
  //  a Get request because the required data are not available.
  // ---------------------------------------------------------------------------
  class NoDataForModelException : public LidarException {

    public:

      NoDataForModelException(const std::string message = std::string()) :
        LidarException(message) { }

      virtual std::string GetExceptionName() const
      { return "Atmosphere model could not find data needed to perform requested calculation."; }

  };


  // ---------------------------------------------------------------------------
  /// \class DataNotFoundInDBException
  /// \brief Exception to use in case requested data are not found in the data
  ///        base, with detailed printout.
  //
  //  This exception is thrown when data requested from a MySQL database is
  //  not found.  The arguments passed to this exception are:
  //   - a string indicating the table being requested
  //   - a string indicating the column being requested
  //   - a map<string,string> containing column name(s) and value(s) in query
  //   - a string with UTC data and time from the Detector description
  // ---------------------------------------------------------------------------
  class DataNotFoundInDBException : public LidarException {

    public:

      DataNotFoundInDBException(const std::string table,
          const std::string column,
          const std::map< std::string, std::string > indexMap,
          const std::string time) :
        fTableName(table),
        fColumnName(column),
        fIndexMap(indexMap),
        fTime(time)
      {
        std::ostringstream msg;
        msg.clear();
        msg << "Did not find requested data for: \n"
            << "Component name     : " << fTableName << "\n"
            << "Component property : " << fColumnName << "\n";
        
        for (std::map< std::string, std::string >::const_iterator it = fIndexMap.begin(); it != fIndexMap.end(); ++it)
        {
          msg << "  " << it->first << " = " << it->second << "\n";
        }

        msg << "Detector time = " << fTime << "\n";
        fMessage = msg.str();
      }

      virtual std::string GetExceptionName() const
      { return "Could not locate requested data in the database"; }

      virtual ~DataNotFoundInDBException() throw() { }

    private:

      std::string fTableName;
      std::string fColumnName;
      std::map< std::string, std::string > fIndexMap;
      std::string fTime;

  };


  // ---------------------------------------------------------------------------
  /// \class XMLParseException LidarException.h "utl/LidarException.h"
  /// \brief Exception for errors encountered when parsing XML
  //
  //   This exception is thrown if an error is encountered during
  //   the parsing of an XML file.
  // ---------------------------------------------------------------------------
  class XMLParseException : public LidarException {

    public:

      /// Construct Invalid Iterator Exception w/ message
      XMLParseException(const std::string message = std::string()) :
        LidarException(message) { }

      /// Retrieve verbose exception name
      virtual std::string GetExceptionName() const
      { return "XML Parse Exception"; }
  };

}

#endif // __utl_LidarException_h__

