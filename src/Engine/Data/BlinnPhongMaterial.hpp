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

/**
 * Implementation of the Blinn-Phong Material BSDF.
 * \todo due to "Material.glsl" interface modification, must test this version with all plugins,
 * apps, ... that uses Radium Renderer
 */
class RA_ENGINE_API BlinnPhongMaterial final : public Material, public ParameterSetEditingInterface
{
    friend class BlinnPhongMaterialConverter;

  public:
    /// Semantic of the texture : define which BSDF parameter is controled by the texture
    enum class TextureSemantic { TEX_DIFFUSE, TEX_SPECULAR, TEX_NORMAL, TEX_SHININESS, TEX_ALPHA };

  public:
    /**
     * Construct a named Blinn-Phongmaterial
     * \param instanceName The name of this instance of the material
     */
    explicit BlinnPhongMaterial( const std::string& instanceName );

    /**
     * Destructor.
     * \note The material does not have ownership on its texture. This destructor do not delete the
     * associated textures.
     */
    ~BlinnPhongMaterial() override;

    void updateGL() override;
    void updateFromParameters() override;
    bool isTransparent() const override;

    /**
     * Add an already existing texture to control the specified BSDF parameter.
     * \param semantic The texture semantic
     * \param texture  The texture to use
     */
    inline void addTexture( const TextureSemantic& semantic,
                            const TextureManager::TextureHandle& texture );
    inline void addTexture( const TextureSemantic& semantic, const std::string& texture );
    /**
     * Get the texture associated to the given semantic.
     * \param semantic
     * \return
     */
    inline Texture* getTexture( const TextureSemantic& semantic ) const;

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

  public:
    Core::Utils::Color m_kd { 0.7, 0.7, 0.7, 1.0 };
    Core::Utils::Color m_ks { 0.3, 0.3, 0.3, 1.0 };
    Scalar m_ns { 64.0 };
    Scalar m_alpha { 1.0 };
    bool m_perVertexColor { false };
    bool m_renderAsSplat { false };

    /**
     * Add an new texture, from a TextureData, to control the specified BSDF parameter.
     * \param semantic The texture semantic
     * \param texture  The texture to use (file)
     * \return the corresponding TextureData struct
     */
    inline void addTexture( const TextureSemantic& semantic, const TextureParameters& texture );

  private:
    std::map<TextureSemantic, TextureManager::TextureHandle> m_textures;
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

// Add a texture as material parameter from an already existing Radium Texture
inline void BlinnPhongMaterial::addTexture( const TextureSemantic& semantic,
                                            const TextureManager::TextureHandle& texture ) {
    m_textures[semantic] = texture;
}

// Add a texture as material parameter with texture parameter set by default for this material
inline void BlinnPhongMaterial::addTexture( const TextureSemantic& semantic,
                                            const std::string& texture ) {
    CORE_ASSERT( !texture.empty(), "Invalid texture name" );
    auto texManager = RadiumEngine::getInstance()->getTextureManager();
    auto texHandle  = texManager->getTextureHandle( texture );
    if ( texHandle.isValid() ) { addTexture( semantic, texHandle ); }
    else {
        TextureParameters data;
        data.name          = texture;
        data.sampler.wrapS = GL_REPEAT;
        data.sampler.wrapT = GL_REPEAT;
        if ( semantic != TextureSemantic::TEX_NORMAL ) {
            data.sampler.minFilter = GL_LINEAR_MIPMAP_LINEAR;
        }
        addTexture( semantic, data );
    }
}

// Add a texture as material parameter with texture parameter set by the caller
// The textures will be finalized (i.e loaded from a file if needed and transformed to OpenGL
// texture) only when needed by the updateGL method.
inline void BlinnPhongMaterial::addTexture( const TextureSemantic& semantic,
                                            const TextureParameters& texture ) {
    auto texManager      = RadiumEngine::getInstance()->getTextureManager();
    m_textures[semantic] = texManager->addTexture( texture );
}

inline Texture* BlinnPhongMaterial::getTexture( const TextureSemantic& semantic ) const {
    Texture* tex    = nullptr;
    auto texManager = RadiumEngine::getInstance()->getTextureManager();
    auto it         = m_textures.find( semantic );
    if ( it != m_textures.end() ) { tex = texManager->getTexture( it->second ); }
    return tex;
}

inline nlohmann::json BlinnPhongMaterial::getParametersMetadata() const {
    return s_parametersMetadata;
}

inline void BlinnPhongMaterial::setColoredByVertexAttrib( bool state ) {
    bool oldState    = m_perVertexColor;
    m_perVertexColor = state;
    if ( oldState != m_perVertexColor ) { needUpdate(); }
}

inline bool BlinnPhongMaterial::isColoredByVertexAttrib() const {
    return m_perVertexColor;
}

} // namespace Data
} // namespace Engine
} // namespace Ra
