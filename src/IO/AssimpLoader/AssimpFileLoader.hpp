#ifndef RADIUMENGINE_ASSIMPFILELOADER_HPP
#define RADIUMENGINE_ASSIMPFILELOADER_HPP

#include <assimp/Importer.hpp>

#include <Core/Asset/FileLoaderInterface.hpp>
#include <IO/RaIO.hpp>

namespace Ra {
namespace Core {
namespace Asset {
class FileData;
} // namespace Asset
} // namespace Core
namespace IO {

//! This class loads scenes containing MESHES only (not point-clouds)
class RA_IO_API AssimpFileLoader : public Core::Asset::FileLoaderInterface {
  public:
    AssimpFileLoader();

    virtual ~AssimpFileLoader();

    std::vector<std::string> getFileExtensions() const override;
    bool handleFileExtension( const std::string& extension ) const override;
    Core::Asset::FileData* loadFile( const std::string& filename ) override;
    std::string name() const override;

  private:
    Assimp::Importer m_importer;
};

} // namespace IO
} // namespace Ra

#endif // RADIUMENGINE_ASSIMPFILELOADER_HPP
