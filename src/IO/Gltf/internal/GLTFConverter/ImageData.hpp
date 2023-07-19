#pragma once
#include <IO/Gltf/internal/fx/gltf.h>

namespace Ra {
namespace IO {
namespace GLTF {

/**
 * Represent an image extracted from json file.
 */
class ImageData
{
  public:
    /**
     * Representation of an image.
     * This representation is either the filename for external images of the image extracted from a
     * binary stream.
     *
     * @todo, support binary stream
     */
    struct ImageInfo {
        std::string FileName {};

        uint32_t BinarySize {};
        uint8_t const* BinaryData {};

        [[nodiscard]] bool IsBinary() const noexcept { return BinaryData != nullptr; }
    };

    /**
     * Constructor of the image data
     * @param doc the json document the image must be extracted from
     * @param textureIndex The index of the image in the json representation
     * @param modelPath the base directory of the json scenes
     */
    ImageData( fx::gltf::Document const& doc, int32_t textureIndex, std::string const& modelPath ) {
        fx::gltf::Image const& image = doc.images[doc.textures[textureIndex].source];

        bool isEmbedded = image.IsEmbeddedResource();
        if ( !image.uri.empty() && !isEmbedded ) {
            m_info.FileName = fx::gltf::detail::GetDocumentRootPath( modelPath ) + "/" + image.uri;
        }
        else {
            if ( isEmbedded ) {
                image.MaterializeData( m_embeddedData );
                m_info.BinaryData = &m_embeddedData[0];
                m_info.BinarySize = static_cast<uint32_t>( m_embeddedData.size() );
            }
            else {
                fx::gltf::BufferView const& bufferView = doc.bufferViews[image.bufferView];
                fx::gltf::Buffer const& buffer         = doc.buffers[bufferView.buffer];

                m_info.BinaryData = &buffer.data[bufferView.byteOffset];
                m_info.BinarySize = bufferView.byteLength;
            }
        }
    }

    /**
     * Access to the ImageInfo structure extracted from the json file
     * @return
     */
    [[nodiscard]] ImageInfo const& Info() const noexcept { return m_info; }

  private:
    ImageInfo m_info {};

    std::vector<uint8_t> m_embeddedData {};
};

} // namespace GLTF
} // namespace IO
} // namespace Ra
