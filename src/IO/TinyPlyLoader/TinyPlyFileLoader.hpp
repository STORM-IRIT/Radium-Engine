#ifndef RADIUMENGINE_TINYPLYFILELOADER_HPP
#define RADIUMENGINE_TINYPLYFILELOADER_HPP

#include <Core/Asset/FileLoaderInterface.hpp>
#include <IO/RaIO.hpp>

namespace Ra {
namespace IO {

/**
 * The TinyPlyFileLoader class loads point clouds from a PLY file and converts it
 * to the Core::Asset::GeometryData format.
 * \note This class loads scenes containing point clouds only (not meshes).
 */
class RA_IO_API TinyPlyFileLoader : public Core::Asset::FileLoaderInterface {
  public:
    TinyPlyFileLoader();

    virtual ~TinyPlyFileLoader() override;

    std::vector<std::string> getFileExtensions() const override;

    bool handleFileExtension( const std::string& extension ) const override;

    Core::Asset::FileData* loadFile( const std::string& filename ) override;

    std::string name() const override;
};

} // namespace IO
} // namespace Ra

#endif // RADIUMENGINE_ASSIMPFILELOADER_HPP
