#pragma once
#include <IO/Gltf/internal/fx/gltf.h>

namespace Ra::Core::Asset {
class MaterialData;
} // namespace Ra::Core::Asset

namespace Ra {
namespace IO {
namespace GLTF {

/**
 * Representation of the material extracted from a json gltf scene
 */
class MaterialData
{
  public:
    /**
     * constructor
     */
    MaterialData() = default;

    /**
     * Initialize the data from a json node
     * @param material
     */
    void SetData( fx::gltf::Material const& material ) {
        m_material = material;
        m_hasData  = true;
    }

    /**
     * Access to the data
     * @return
     */
    [[nodiscard]] fx::gltf::Material const& Data() const noexcept { return m_material; }

    /**
     * Test if material is specularGlossiness.
     * Right now, two materials are defined for RadiumGLTF : specularGlossiness and
     * MetallicRoughness Each of these materials accepts some extensions described in the spec.
     * \note specularGlossiness is deprecated in the spec
     */
    [[nodiscard]] bool isSpecularGlossiness() const noexcept {
        auto extensionsAndExtras = Data().extensionsAndExtras;
        if ( !extensionsAndExtras.empty() ) {
            auto extensions = extensionsAndExtras.find( "extensions" );
            if ( extensions != extensionsAndExtras.end() ) {
                auto iter = extensions->find( "KHR_materials_pbrSpecularGlossiness" );
                if ( iter != extensions->end() ) { return true; }
            }
        }
        return false;
    }

    [[nodiscard]] bool hasData() const noexcept { return m_hasData; }

  private:
    fx::gltf::Material m_material {};
    bool m_hasData {};
};

Ra::Core::Asset::MaterialData* buildMaterial( const fx::gltf::Document& doc,
                                              int32_t meshIndex,
                                              const std::string& filePath,
                                              int32_t meshPartNumber,
                                              const MaterialData& meshMaterial );

} // namespace GLTF
} // namespace IO
} // namespace Ra
