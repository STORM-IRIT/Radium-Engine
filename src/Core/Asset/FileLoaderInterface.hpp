#ifndef RADIUMENGINE_FILELOADERINTERFACE_HPP
#define RADIUMENGINE_FILELOADERINTERFACE_HPP

#include <string>
#include <vector>

#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {
namespace Asset {
class FileData;

/**
 * The FileLoaderInterface class is the basic class for FileLoaders.
 */
class FileLoaderInterface {
  public:
    virtual ~FileLoaderInterface() {}

    /**
     * Return the list of file extensions supported by the loader.
     */
    virtual std::vector<std::string> getFileExtensions() const = 0;

    /**
     * Return true if the loader supports the given file extension.
     */
    virtual bool handleFileExtension( const std::string& extension ) const = 0;

    /**
     * Try to load file, returns nullptr in case of failure.
     */
    virtual FileData* loadFile( const std::string& filename ) = 0;

    /**
     * Return the name of the loader.
     */
    virtual std::string name() const = 0;
};

} // namespace Asset
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_FILELOADER_HPP
