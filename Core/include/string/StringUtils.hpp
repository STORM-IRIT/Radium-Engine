#ifndef RADIUMENGINE_STRINGUTILS_HPP
#define RADIUMENGINE_STRINGUTILS_HPP

#include <string>

namespace Ra
{
    /// A collection of utilities for std::string.
    namespace StringUtils
    {
        //
        // File names and paths.
        // Todo : support bacward slashes in windows paths.
        //

        /// @return the file extension or the empty string if not found
        std::string getFileExt(const std::string& str);

        /// @return the path to a parent directory of a given file's full path.
        /// Most of the time this function behaves similarly to the unix dirname(1)
        /// command. It will ignore any trailing slash.
        std::string getDirName(const std::string& path);

        /// @return the file name from a given file's full path.
        /// Most of the time this function behaves similarly to the unix basename(1)
        /// command. It will ignore any trailing slash.
        std::string getBaseName(const std::string& path);

        //
        // Printf-like interface for people who dislike streams.
        //

        /// Writes a formatted print into the given string, similarly to sprintf.
        /// @return the number of characters printed, or a negative value if there was
        ///  any error.
        int stringPrintf(std::string& str, const char* fmt, ...);


        /// Appends a formatted print to the given string.
        /// @return the number of characters appended or a negative value if there was
        /// any error.
        int appendPrintf(std::string& str, const char* fmt, ...);
    }
}

#endif //RADIUMENGINE_STRINGUTILS_HPP
