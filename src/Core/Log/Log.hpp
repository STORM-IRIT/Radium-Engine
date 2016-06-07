#ifndef RADIUMENGINE_LOG_HPP
#define RADIUMENGINE_LOG_HPP

#include <Core/RaCore.hpp>
#include <sstream>
#include <string>
#include <stdio.h>

#include <Core/String/StringUtils.hpp>

inline std::string NowTime();

enum TLogLevel { logERROR, logWARNING, logINFO, logDEBUG, logDEBUG1, logDEBUG2, logDEBUG3, logDEBUG4 };

template <typename T>
class Log
{
public:
    Log();
    virtual ~Log();
    std::ostringstream& Get( TLogLevel level = logINFO );
public:
    static TLogLevel& ReportingLevel();
    static std::string ToString( TLogLevel level );
    static TLogLevel FromString( const std::string& level );
protected:
    std::ostringstream os;
private:
    Log( const Log& );
    Log& operator = ( const Log& );
};

template <typename T>
Log<T>::Log()
{
}

template <typename T>
std::ostringstream& Log<T>::Get( TLogLevel level )
{
    os << "- " << NowTime();
    os << " " << ToString( level ) << ": ";
    os << std::string( level > logDEBUG ? level - logDEBUG : 0, '\t' );
    return os;
}

template <typename T>
Log<T>::~Log()
{
    os << std::endl;
    T::Output( os.str() );
}

template <typename T>
TLogLevel& Log<T>::ReportingLevel()
{
    static TLogLevel reportingLevel = logDEBUG4;
    return reportingLevel;
}

template <typename T>
std::string Log<T>::ToString( TLogLevel level )
{
    static const char* const buffer[] = { "ERROR", "WARNING", "INFO", "DEBUG", "DEBUG1", "DEBUG2", "DEBUG3", "DEBUG4" };
    return buffer[level];
}

template <typename T>
TLogLevel Log<T>::FromString( const std::string& level )
{
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
    Log<T>().Get( logWARNING ) << "Unknown logging level '" << level << "'. Using INFO level as default.";
    return logINFO;
}

class Output2FILE
{
public:
    static FILE*& Stream();
    static void Output( const std::string& msg );
};

inline FILE*& Output2FILE::Stream()
{
    static FILE* pStream = stderr;
    return pStream;
}

inline void Output2FILE::Output( const std::string& msg )
{
    FILE* pStream = Stream();
    if ( !pStream )
    {
        return;
    }
    fprintf( pStream, "%s", msg.c_str() );
    fflush( pStream );
}

class FILELog : public Log<Output2FILE> {};
//typedef Log<Output2FILE> FILELog;

#ifndef FILELOG_MAX_LEVEL
#ifdef CORE_DEBUG
    #define FILELOG_MAX_LEVEL logDEBUG4
#else
    #define FILELOG_MAX_LEVEL logINFO
#endif
#endif

#define FILE_LOG(level) \
    if (level > FILELOG_MAX_LEVEL) ;\
    else if (level > FILELog::ReportingLevel() || !Output2FILE::Stream()) ; \
    else FILELog().Get(level)

#define LOG(level) FILE_LOG(level)

#include <ctime>

inline std::string NowTime()
{
    char buffer[100];
    std::time_t t = std::time( nullptr );
    ON_DEBUG(int ok =) std::strftime( buffer, 100, "%X", std::localtime( &t ) );
    CORE_ASSERT (ok, "Increase buffer size.");
    std::string result(buffer);
    // This doesn't work with minGW. Maybe indicates a serious issue ?
    //Ra::Core::StringUtils::stringPrintf( result, "%s", buffer );
    return result;
}

#endif // RADIUMENGINE_LOG_HPP
