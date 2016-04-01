#include <Core/String/StringUtils.hpp>

#include <Core/CoreMacros.hpp>

#ifdef OS_WINDOWS
#define NOMINMAX                // Avoid C2039 MSVC compiler error
//#undef vsnprintf
//#define vsnprintf(buffer, count, format, argptr) vsnprintf_s(buffer, count, count, format, argptr)
#endif

#include <algorithm> // std::min std::max
#include <cstring>
#include <cstdarg>
#include <cstdio>
#include <memory>
#include <sstream>

namespace Ra
{
    namespace Core
    {
        namespace StringUtils
        {
            std::string getFileExt( const std::string& str )
            {
                std::string res;
                ulong pos = str.find_last_of( '.' );
                res = pos + 1 < str.size()? str.substr( pos + 1 ) : "";
                return res;
            }


            std::string getDirName( const std::string& path )
            {
                // We remove any trailing slashes.
                ulong pos = path.find_last_not_of( '/' );

                // Don't strip the last / from "/"
                if ( pos == std::string::npos )
                {
                    pos = path.find_first_of( "/" );
                }

                std::string res;
                res = path.substr( 0, pos + 1 );

                // Now find the previous slash and cut the string.
                pos = res.find_last_of( '/' );

                // The directory is actually "/" because the last slash is in first position.
                // In that case we should return "/"
                if ( pos == 0 )
                {
                    res = "/";
                }
                else if ( pos != std::string::npos )
                {
                    res = res.substr( 0, pos );
                }
                else
                {
                    res = ".";
                }

                return res;
            }

            std::string getBaseName( const std::string& path, bool keepExtension )
            {
                std::string res;
                // We remove any trailing slashes.
                ulong pos = path.find_last_not_of( '/' );
                // Don't strip the last / from "/"
                if ( pos == std::string::npos )
                {
                    pos = path.find_first_of( "/" );
                }

                res = path.substr( 0, pos + 1 );

                // Now find the previous slash and cut the string.
                pos = res.find_last_of( '/' );

                if ( pos != std::string::npos )
                {
                    res = res.substr( pos + 1 );
                }

                if ( !keepExtension )
                {
                    pos = res.find_last_of( '.' );
                    if ( pos != std::string::npos )
                    {
                        res = res.substr( 0, pos );
                    }
                }

                return res;
            }

            // Printf-to-string functions.

            // Each function expecting varargs has a va_list equivalent
            // which does the actual job (because you can't pas varargs to another
            // function). See http://c-faq.com/varargs/handoff.html

            int stringvPrintf( std::string& str, const char* fmt, va_list args )
            {
                // Random guessing value from the size of the format string.
                size_t size = strlen( fmt ) * 2;
                int finalSize = 0;
                str.clear();
                std::unique_ptr<char[]> buffer;

                while ( 1 )
                {
                    // Dynamically allocate a string and assign it to the unique ptr
                    buffer.reset( new char[size] );

                    // Attempt to printf into the buffer
                    va_list argsCopy;
                    va_copy(argsCopy,args);
                    finalSize = vsnprintf( &buffer[0], size, fmt, argsCopy );
                    va_end(argsCopy);
                    // If our buffer was too small, we know that final_size
                    // gives us the required buffer size.
                    if ( uint( finalSize ) >= size )
                    {
                        size = std::max( size + 1, size_t ( finalSize ) );
                    }
                    else
                    {
                        break;
                    }
                }
                if ( finalSize > 0 )
                {
                    str = std::string( buffer.get() );
                }
                return finalSize;
            }

            int appendvPrintf( std::string& str, const char* fmt, va_list args )
            {
                std::string toAppend;
                int result = stringvPrintf( toAppend, fmt, args );
                str += toAppend;
                return result;
            }


            // These functions are exposed to the interface, but they just cal
            // the va_list versions above.
            int stringPrintf( std::string& str, const char* fmt, ... )
            {
                va_list args;
                va_start(args,fmt);
                int result = stringvPrintf(str,fmt,args);
                va_end(args);
                return result;
            }

            int appendPrintf( std::string& str, const char* fmt, ... )
            {
                va_list args;
                va_start(args, fmt);
                int result = appendvPrintf(str, fmt, args);
                va_end(args);
                return result;
            }

            std::vector<std::string> splitString( const std::string& str, char token )
            {
                std::stringstream ss( str );
                std::string item;
                std::vector<std::string> items;

                while ( std::getline( ss, item, token ) )
                {
                    items.push_back( item );
                }

                return items;
            }
        }
    }
}
