#pragma once
#include <IO/RaIO.hpp>

#include <Core/Asset/FileLoaderInterface.hpp>

namespace Ra::Core::Asset {
class FileData;
} // namespace Ra::Core::Asset

namespace Ra {
namespace IO {
namespace GLTF {

/**
 * FileLoader for GLTF2.0 file format
 */
class RA_IO_API glTFFileLoader : public Ra::Core::Asset::FileLoaderInterface
{
  public:
    glTFFileLoader();

    ~glTFFileLoader() override;

    /** Radium Loader interface
     *
     * @return {"gltf", "glb"}
     */
    [[nodiscard]] std::vector<std::string> getFileExtensions() const override;

    /**
     * check if an extension is managed by the loader
     * @param extension
     * @return true if extension is gltf or glb
     */
    [[nodiscard]] bool handleFileExtension( const std::string& extension ) const override;

    /** Try to load file, returns nullptr in case of failure
     *
     * @param filename the file to load
     * @return the File data representing the gltf scene, nullptr if loading failed
     */
    Ra::Core::Asset::FileData* loadFile( const std::string& filename ) override;

    /** Unique name of the loader
     *
     * @return "glTF 2.0"
     */
    [[nodiscard]] std::string name() const override;
};

} // namespace GLTF
} // namespace IO
} // namespace Ra
