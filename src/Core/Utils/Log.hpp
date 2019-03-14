#ifndef RADIUMENGINE_LOG_HPP
#define RADIUMENGINE_LOG_HPP

#include <Core/RaCore.hpp>
#include <ctime>
#include <sstream>
#include <stdio.h>
#include <string>

namespace Ra {
namespace Core {
namespace Utils {

/**
 * Return the current day time as a string.
 */
inline std::string NowTime();

/**
 * The print output level.
 */
enum TLogLevel {
    logERROR,
    logWARNING,
    logINFO,
    logDEBUG,
    logDEBUG1,
    logDEBUG2,
    logDEBUG3,
    logDEBUG4
};

/**
 * The Log class defines the generic behavior to display Log info.
 * \tparam T defines the stream type to display to.
 * \note T must implement void Output( const std::string& msg ).
 */
template <typename T>
class Log {
  public:
    Log();

    /**
     * Upon deletion, the Log performs a new line insertion and
     * flushes the output stream before calling T::Output() on it.
     */
    virtual ~Log();

    /**
     * Return the output stream corresponding to the given print output level.
     * \note The ouput stream contains a "header" part depending on \p level.
     */
    std::ostringstream& Get( TLogLevel level = logINFO );

  public:
    /**
     * Always return logDEBUG4.
     */
    static TLogLevel& ReportingLevel();

    /**
     * Converts the given output level to a string.
     */
    static std::string ToString( TLogLevel level );

    /**
     * Converts the given string to the corresponding output level.
     * If the string does not correspond to any output level, then reports
     * the error on the logWARNING output and returns logINFO.
     */
    static TLogLevel FromString( const std::string& level );

  protected:
    /**
     * The ouput stream.
     */
    std::ostringstream os;

  private:
    Log( const Log& );
    Log& operator=( const Log& );
};

template <typename T>
Log<T>::Log() {}

template <typename T>
std::ostringstream& Log<T>::Get( TLogLevel level ) {
    os << "- " << NowTime();
    os << " " << ToString( level ) << ": ";
    os << std::string( level > logDEBUG ? level - logDEBUG : 0, '\t' );
    return os;
}

template <typename T>
Log<T>::~Log() {
    os << std::endl;
    T::Output( os.str() );
}

template <typename T>
TLogLevel& Log<T>::ReportingLevel() {
    static TLogLevel reportingLevel = logDEBUG4;
    return reportingLevel;
}

template <typename T>
std::string Log<T>::ToString( TLogLevel level ) {
    static const char* const buffer[] = {"ERROR",  "WARNING", "INFO",   "DEBUG",
                                         "DEBUG1", "DEBUG2",  "DEBUG3", "DEBUG4"};
    return buffer[level];
}

template <typename T>
TLogLevel Log<T>::FromString( const std::string& level ) {
    if ( level == "DEBUG4" )
    {
        return logDEBUG4;
    }
    if ( level == "DEBUG3" )
    {
        return logDEBUG3;
    }
    if ( level == "DEBUG2" )
    {
        return logDEBUG2;
    }
    if ( level == "DEBUG1" )
    {
        return logDEBUG1;
    }
    if ( level == "DEBUG" )
    {
        return logDEBUG;
    }
    if ( level == "INFO" )
    {
        return logINFO;
    }
    if ( level == "WARNING" )
    {
        return logWARNING;
    }
    if ( level == "ERROR" )
    {
        return logERROR;
    }
    Log<T>().Get( logWARNING ) << "Unknown logging level '" << level
                               << "'. Using INFO level as default.";
    return logINFO;
}

/**
 * The Output2FILE class defines the Log info stream to be to the sterr output.
 */
class Output2FILE {
  public:
    /**
     * Return the stderr output stream.
     */
    static FILE*& Stream();

    /**
     * Prints the given message to stderr.
     */
    static void Output( const std::string& msg );
};

inline FILE*& Output2FILE::Stream() {
    static FILE* pStream = stderr;
    return pStream;
}

inline void Output2FILE::Output( const std::string& msg ) {
    FILE* pStream = Stream();
    if ( !pStream )
    {
        return;
    }
    fprintf( pStream, "%s", msg.c_str() );
    fflush( pStream );
}

/**
 * Log implementation for the stderr output stream.
 */
class FILELog : public Log<Output2FILE> {};
// using FILELog = Log<Output2FILE>;

inline std::string NowTime() {
    char buffer[100];
    std::time_t t = std::time( nullptr );
    ON_ASSERT( int ok = ) std::strftime( buffer, 100, "%X", std::localtime( &t ) );
    CORE_ASSERT( ok, "Increase buffer size." );
    std::string result( buffer );
    // This doesn't work with minGW. Maybe indicates a serious issue ?
    // Ra::Core::Utils::stringPrintf( result, "%s", buffer );
    return result;
}

} // namespace Utils
} // namespace Core
} // namespace Ra

#ifndef FILELOG_MAX_LEVEL
#    ifdef CORE_DEBUG
#        define FILELOG_MAX_LEVEL Ra::Core::Utils::logDEBUG4
#    else
#        define FILELOG_MAX_LEVEL Ra::Core::Utils::logINFO
#    endif
#endif

/// This macro enables to display Radium Log messages.
/// Example:
/// \code
///     FILE_LOG( Ra::Core::Utils::logInfo ) << "Hello Radium.";
/// \endcode
#define FILE_LOG( level )                                           \
    if ( level > FILELOG_MAX_LEVEL )                                \
        ;                                                           \
    else if ( level > Ra::Core::Utils::FILELog::ReportingLevel() || \
              !Ra::Core::Utils::Output2FILE::Stream() )             \
        ;                                                           \
    else                                                            \
        Ra::Core::Utils::FILELog().Get( level )

/// This macro enables to display Radium Log messages.
/// Example:
/// \code
///     LOG( Ra::Core::Utils::logInfo ) << "Hello Radium.";
/// \endcode
#define LOG( level ) FILE_LOG( level )

#endif // RADIUMENGINE_LOG_HPP
