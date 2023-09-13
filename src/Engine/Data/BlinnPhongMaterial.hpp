#pragma once

#include <Engine/RaEngine.hpp>

#include <Core/Utils/Color.hpp>
#include <Engine/Data/Material.hpp>
#include <Engine/Data/Texture.hpp>
#include <Engine/Data/TextureManager.hpp>
#include <Engine/RadiumEngine.hpp>

#include <map>
#include <string>

namespace Ra {
namespace Core {
namespace Asset {
class MaterialData;
}
} // namespace Core

namespace Engine {

namespace Data {
class ShaderProgram;

namespace TextureSemantics {
enum class BlinnPhongMaterial { TEX_DIFFUSE, TEX_SPECULAR, TEX_NORMAL, TEX_SHININESS, TEX_ALPHA };
} // namespace TextureSemantics

/**
 * Implementation of the Blinn-Phong Material BSDF.
 * \todo due to "Material.glsl" interface modification, must test this version with all plugins,
 * apps, ... that uses Radium Renderer
 */

class RA_ENGINE_API BlinnPhongMaterial final
    : public Material,
      public ParameterSetEditingInterface,
      public MaterialTextureSet<TextureSemantics::BlinnPhongMaterial>
{
    friend class BlinnPhongMaterialConverter;

  public:
    using TextureSemantic = TextureSemantics::BlinnPhongMaterial;

    /**
     * Construct a named Blinn-Phongmaterial
     * \param instanceName The name of this instance of the material
     */
    explicit BlinnPhongMaterial( const std::string& instanceName );

    using MaterialTextureSet<TextureSemantic>::addTexture;

    void updateGL() override;
    void updateFromParameters() override;
    bool isTransparent() const override;

    /**
     * Register the material in the material library.
     * After registration, the material could be instantiated by any Radium system, renderer,
     * plugin, ...
     */
    static void registerMaterial();

    /**
     * Remove the material from the material library.
     * After removal, the material is no more available, ...
     */
    static void unregisterMaterial();

    /**
     * Get a json containing metadata about the parameters of the material.
     * \return the metadata in json format
     */
    inline nlohmann::json getParametersMetadata() const override;

    inline void setColoredByVertexAttrib( bool state ) override;

    inline bool isColoredByVertexAttrib() const override;

    inline void setDiffuseColor( Core::Utils::Color c );
    inline void setSpecularColor( Core::Utils::Color c );
    inline void setSpecularExponent( Scalar n );
    inline void setRenderAsSplat( bool state );

    inline const Core::Utils::Color& getDiffuseColor() { return m_kd; }
    inline const Core::Utils::Color& getSpecularColor() { return m_ks; }
    inline Scalar getSpecularExponent() { return m_ns; }
    inline bool isRenderAsSplat() { return m_renderAsSplat; }

  private:
    Core::Utils::Color m_kd { 0.7, 0.7, 0.7, 1.0 };
    Core::Utils::Color m_ks { 0.3, 0.3, 0.3, 1.0 };
    Scalar m_ns { 64.0 };
    Scalar m_alpha { 1.0 };
    bool m_perVertexColor { false };
    bool m_renderAsSplat { false };
    static nlohmann::json s_parametersMetadata;

    /**
     * Update the rendering parameters for the Material
     */
    void updateRenderingParameters();
};

/**
 * Converter from an external representation comming from FileData to internal representation.
 */
class RA_ENGINE_API BlinnPhongMaterialConverter final
{
  public:
    BlinnPhongMaterialConverter()  = default;
    ~BlinnPhongMaterialConverter() = default;

    Material* operator()( const Ra::Core::Asset::MaterialData* toconvert );
};

inline nlohmann::json BlinnPhongMaterial::getParametersMetadata() const {
    return s_parametersMetadata;
}

inline void BlinnPhongMaterial::setColoredByVertexAttrib( bool state ) {
    if ( state != m_perVertexColor ) {
        m_perVertexColor = state;
        needUpdate();
    }
}

inline bool BlinnPhongMaterial::isColoredByVertexAttrib() const {
    return m_perVertexColor;
}

inline void BlinnPhongMaterial::setSpecularExponent( Scalar n ) {
    m_ns = n;
    needUpdate();
}

inline void BlinnPhongMaterial::setSpecularColor( Core::Utils::Color c ) {
    m_ks = std::move( c );
    needUpdate();
}

inline void BlinnPhongMaterial::setDiffuseColor( Core::Utils::Color c ) {
    m_kd = std::move( c );
    needUpdate();
}

inline void BlinnPhongMaterial::setRenderAsSplat( bool state ) {
    if ( state != m_renderAsSplat ) {
        m_renderAsSplat = state;
        needUpdate();
    }
}

} // namespace Data
} // namespace Engine
} // namespace Ra
