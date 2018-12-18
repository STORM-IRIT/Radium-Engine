    #ifndef RADIUMENGINE_STRINGUTILS_HPP
#define RADIUMENGINE_STRINGUTILS_HPP

#include <Core/RaCore.hpp>
#include <string>
#include <vector>

namespace Ra {
namespace Core {
namespace Utils {
//
// File names and paths.
// Todo : support backward slashes in windows paths.
//

/// @return the file extension or the empty string if not found
RA_CORE_API std::string getFileExt( const std::string& str );

/// @return the path to a parent directory of a given file's full path.
/// Most of the time this function behaves similarly to the unix dirname(1)
/// command. It will ignore any trailing slash.
RA_CORE_API std::string getDirName( const std::string& path );

/// @return the file name from a given file's full path.
/// Most of the time this function behaves similarly to the unix basename(1)
/// command. It will ignore any trailing slash.
/// @param path the fully qualified file name from which extract the base path
/// @param keepExtension If true it will behave like unix basename(1)
/// (path/to/file.ext will give file.ext) ;
/// otherwise it will remove the extension (path/to/file.ext will give file)
RA_CORE_API std::string getBaseName( const std::string& path, bool keepExtension = true );
} // namespace Utils
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_STRINGUTILS_HPP
