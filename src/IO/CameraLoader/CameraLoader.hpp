#ifndef RADIUMENGINE_CAMERAFILELOADER_HPP
#define RADIUMENGINE_CAMERAFILELOADER_HPP

#include <Core/Asset/FileLoaderInterface.hpp>
#include <IO/RaIO.hpp>

namespace Ra {
namespace IO {

/**
 * The CameraFileLoader class loads camera data from a file and converts it
 * to the Core::Asset::CameraData format.
 */
class RA_IO_API CameraFileLoader : public Core::Asset::FileLoaderInterface {
  public:
    CameraFileLoader();

    ~CameraFileLoader() override;

    std::vector<std::string> getFileExtensions() const override;

    bool handleFileExtension( const std::string& extension ) const override;

    Core::Asset::FileData* loadFile( const std::string& filename ) override;

    std::string name() const override;
};

} // namespace IO
} // namespace Ra

#endif // RADIUMENGINE_CAMERAFILELOADER_HPP
