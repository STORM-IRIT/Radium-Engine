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

#ifdef LEGACY_IMPLEMENTATION
    /***
     * Test if the data are valid
     * @return
     */
    [[nodiscard]] bool isMetallicRoughness() const noexcept {
        return m_hasData && !m_material.pbrMetallicRoughness.empty();
    }
#else
    /**
     * Test if material is specularGlossiness
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
#endif
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
