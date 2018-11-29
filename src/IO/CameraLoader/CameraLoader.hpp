#ifndef RADIUMENGINE_CAMERAFILELOADER_HPP
#define RADIUMENGINE_CAMERAFILELOADER_HPP

#include <Core/File/FileLoaderInterface.hpp>
#include <IO/RaIO.hpp>

namespace Ra {

namespace Asset {
class FileData;
} // namespace Asset

namespace IO {

class RA_IO_API CameraFileLoader : public Asset::FileLoaderInterface {
  public:
    CameraFileLoader();

    ~CameraFileLoader() override;

    std::vector<std::string> getFileExtensions() const override;
    bool handleFileExtension( const std::string& extension ) const override;
    Asset::FileData* loadFile( const std::string& filename ) override;
    std::string name() const override;
};

} // namespace IO
} // namespace Ra

#endif // RADIUMENGINE_CAMERAFILELOADER_HPP
