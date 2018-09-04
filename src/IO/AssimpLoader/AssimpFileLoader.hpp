#ifndef RADIUMENGINE_ASSIMPFILELOADER_HPP
#define RADIUMENGINE_ASSIMPFILELOADER_HPP

#include <assimp/Importer.hpp>

#include <Core/File/FileLoaderInterface.hpp>
#include <IO/RaIO.hpp>

namespace Ra {

namespace Asset {
class FileData;
} // namespace Asset

namespace IO {

/// The AssimpFileLoader class loads data from a file through the AssImp interface,
/// and then converts the assimp data format to the Core::Asset format.
/// \note This class loads scenes containing MESHES only (not point-clouds).
class RA_IO_API AssimpFileLoader : public Asset::FileLoaderInterface {
  public:
    AssimpFileLoader();

    virtual ~AssimpFileLoader();

    std::vector<std::string> getFileExtensions() const override;

    bool handleFileExtension( const std::string& extension ) const override;

    Asset::FileData* loadFile( const std::string& filename ) override;

    std::string name() const override;

  private:
    /// The AssImp loader.
    Assimp::Importer m_importer;
};

} // namespace IO
} // namespace Ra

#endif // RADIUMENGINE_ASSIMPFILELOADER_HPP
