#pragma once

#include <Core/Asset/FileData.hpp>
#include <Core/Asset/FileLoaderInterface.hpp>
#include <IO/RaIO.hpp>
#include <assimp/Importer.hpp>
#include <string>
#include <vector>

namespace Ra {
namespace IO {

//! This class loads scenes containing MESHES only (not point-clouds)
class RA_IO_API AssimpFileLoader : public Core::Asset::FileLoaderInterface
{
  public:
    AssimpFileLoader();

    ~AssimpFileLoader() override;

    std::vector<std::string> getFileExtensions() const override;
    bool handleFileExtension( const std::string& extension ) const override;
    Core::Asset::FileData* loadFile( const std::string& filename ) override;
    std::string name() const override;

  private:
    Assimp::Importer m_importer;
};

} // namespace IO
} // namespace Ra
