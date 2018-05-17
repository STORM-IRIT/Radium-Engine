#ifndef RADIUMENGINE_TINYPLYFILELOADER_HPP
#define RADIUMENGINE_TINYPLYFILELOADER_HPP

#include <Core/Asset/FileLoaderInterface.hpp>
#include <IO/RaIO.hpp>

namespace Ra {
namespace Core {
namespace Asset {
class FileData;
} // namespace Asset
} // namespace Core

namespace IO {

class RA_IO_API TinyPlyFileLoader : public Core::Asset::FileLoaderInterface {
  public:
    TinyPlyFileLoader();

    virtual ~TinyPlyFileLoader();

    std::vector<std::string> getFileExtensions() const override;
    bool handleFileExtension( const std::string& extension ) const override;
    Core::Asset::FileData* loadFile( const std::string& filename ) override;
    std::string name() const override;
};

} // namespace IO
} // namespace Ra

#endif // RADIUMENGINE_ASSIMPFILELOADER_HPP
