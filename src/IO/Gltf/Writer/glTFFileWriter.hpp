#pragma once
#include <IO/RaIO.hpp>

#include <string>
#include <vector>

namespace Ra::Engine::Scene {
class Entity;
}

namespace Ra {
namespace IO {
namespace GLTF {

/**
 * FileWriter for GLTF2.0 file format
 *
 * TODO : make the loader keep the names of elements and save elements with the same name
 */
class RA_IO_API glTFFileWriter
{
  public:
    /**
     * Create a GLTF file writer
     * @param filename the file to save
     * @param texturePrefix the texture uri prefix
     * @param writeImages set it to true to also export texture images.
     *
     * @note Images are not yet exported. It is expected that they are in the texturePrefix relative
     * path (uri)
     */
    explicit glTFFileWriter( std::string filename,
                             std::string texturePrefix = { "textures/" },
                             bool writeImages          = false );
    ~glTFFileWriter();

    void write( std::vector<Ra::Engine::Scene::Entity*> toExport );

  private:
    std::string m_fileName;
    std::string m_texturePrefix;
    bool m_writeImages { false };
    std::string m_bufferName;
    std::string m_rootName;
};

} // namespace GLTF
} // namespace IO
} // namespace Ra
