#pragma once

#include <Core/RaCore.hpp>
#include <cstddef>
#include <string>
#include <string_view>

namespace Ra {
namespace Core {
namespace Utils {
//
// File names and paths.
// Todo : support backward slashes in windows paths.
//

/// \return the file extension or the empty string if not found
RA_CORE_API std::string getFileExt( const std::string& str );

/// \return the path to a parent directory of a given file's full path.
/// Most of the time this function behaves similarly to the unix dirname(1)
/// command. It will ignore any trailing slash.
RA_CORE_API std::string getDirName( const std::string& path );

/// \return the file name from a given file's full path.
/// Most of the time this function behaves similarly to the unix basename(1)
/// command. It will ignore any trailing slash.
/// \param path the fully qualified file name from which extract the base path
/// \param keepExtension If true it will behave like unix basename(1)
/// (path/to/file.ext will give file.ext) ;
/// otherwise it will remove the extension (path/to/file.ext will give file)
RA_CORE_API std::string getBaseName( const std::string& path, bool keepExtension = true );

/**
 * \brief Replace all occurrences of a substring by another substring into a string
 * \param inout the string to modify
 * \param what the substring to change
 * \param with the new value of the substring
 * \return The number of substitutions
 */
RA_CORE_API std::size_t
replaceAllInString( std::string& inout, std::string_view what, std::string_view with );

/**
 * \brief Remove all occurrences of a substring into a string
 * \param inout the string to modify
 * \param what the substring to remove
 * \return The number of removed substrings
 */
RA_CORE_API std::size_t removeAllInString( std::string& inout, std::string_view what );

} // namespace Utils
} // namespace Core
} // namespace Ra
