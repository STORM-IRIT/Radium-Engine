#pragma once
#include <string>

// TODO : For the moment, the scene tree is flattened. Make Radium accept scene trees
namespace Ra::Core::Asset {
class FileData;
} // namespace Ra::Core::Asset

namespace fx::gltf {
struct Document;
}

namespace Ra {
namespace IO {
namespace GLTF {

/**
 * Functor that process a json tree that represents a gltf scene and convert it to Radium FileData
 * object.
 */
class Converter
{
  public:
    /**
     * Constructor of the functor
     * @param fd the FileData to fill
     * @param baseDir The base directory of the loaded file to access external assets.
     */
    explicit Converter( Ra::Core::Asset::FileData* fd,
                        const std::string& baseDir = std::string {} );
    /**
     * Convert the given gltf json scene to Radium FileData
     * @param gltfscene
     * @return
     */
    bool operator()( const fx::gltf::Document& gltfscene );

  private:
    Ra::Core::Asset::FileData* fileData;
    std::string filePath;
};

} // namespace GLTF
} // namespace IO
} // namespace Ra
