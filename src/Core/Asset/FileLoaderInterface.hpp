#ifndef RADIUMENGINE_FILELOADERINTERFACE_HPP
#define RADIUMENGINE_FILELOADERINTERFACE_HPP

#include <string>
#include <vector>

#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {
namespace Asset {
class FileData;

class FileLoaderInterface
{
  public:
    virtual ~FileLoaderInterface() = default;

    virtual std::vector<std::string> getFileExtensions() const = 0;

    virtual bool handleFileExtension( const std::string& extension ) const = 0;

    //! Try to load file, returns nullptr in case of failure
    virtual FileData* loadFile( const std::string& filename ) = 0;

    //! Unique name of the loader
    virtual std::string name() const = 0;
};

} // namespace Asset
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_FILELOADER_HPP
