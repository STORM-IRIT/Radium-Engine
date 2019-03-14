#ifndef RADIUMENGINE_STRINGUTILS_HPP
#define RADIUMENGINE_STRINGUTILS_HPP

#include <Core/RaCore.hpp>
#include <string>
#include <vector>

namespace Ra {
namespace Core {
namespace Utils {

/// \name Files paths and names
/// \{
// Todo : support backward slashes in windows paths.

/**
 * Return the file extension or an empty string if not found.
 */
RA_CORE_API std::string getFileExt( const std::string& str );

/**
 * Return the path to a parent directory of a given file's full path.
 * Most of the time this function behaves similarly to the unix dirname(1)
 * command. It will ignore any trailing slash.
 */
RA_CORE_API std::string getDirName( const std::string& path );

/**
 * Return the file name from a given file's full path.
 * Most of the time this function behaves similarly to the unix basename(1)
 * command. It will ignore any trailing slash.
 * \param path          the fully qualified file name from which to extract
 *                      the base path.
 * \param keepExtension if true it will behave like unix basename(1)
 *                      (path/to/file.ext will give file.ext),
 *                      otherwise it will remove the extension
 *                      (path/to/file.ext will give file)
 */
RA_CORE_API std::string getBaseName( const std::string& path, bool keepExtension = true );
} // namespace Utils
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_STRINGUTILS_HPP
