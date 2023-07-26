#pragma once
#include <Engine/RaEngine.hpp>

#include <Core/Utils/EnumConverter.hpp>

#include <Core/Material/BaseGLTFMaterial.hpp>
#include <Engine/Data/Material.hpp>
#include <Engine/Data/TextureManager.hpp>

namespace Ra {
namespace Engine {
namespace Data {

class GLTFMaterial;
/**
 * \brief Base class for Radium Engine representation and management of a material extension.
 */
class RA_ENGINE_API GLTFMaterialExtension : public Material
{
  public:
    using TextureSemantic = std::string;

    GLTFMaterialExtension( GLTFMaterial& baseMaterial,
                           const std::string& instanceName,
                           const std::string& typeName ) :
        Material( instanceName, typeName ), m_baseMaterial { baseMaterial } {}
    GLTFMaterialExtension( const GLTFMaterialExtension& )           = delete;
    GLTFMaterialExtension operator=( const GLTFMaterialExtension& ) = delete;

    void updateGL() override {}
    void updateFromParameters() override {};

    [[nodiscard]] bool isTransparent() const override { return false; }
    [[nodiscard]] std::list<std::string> getPropertyList() const override { return {}; }

    /**
     * Add a texture to the material for the given semantic
     * @param semantic
     * @param texture
     * @param sampler
     * @return
     */
    inline Ra::Engine::Data::TextureParameters&
    addTexture( const TextureSemantic& semantic,
                const std::string& texture,
                const Core::Material::GLTFSampler& sampler );

    /**
     * Get the Radium texture associated with the given semantic
     * @param semantic
     * @return
     */
    [[nodiscard]] inline Ra::Engine::Data::Texture*
    getTexture( const TextureSemantic& semantic ) const;

    /**
     * Get the Radium texture (parameter struct) associated with the given semantic
     * @param semantic
     * @return The TextureParameter description
     */
    [[nodiscard]] inline std::shared_ptr<Ra::Engine::Data::TextureParameters>
    getTextureParameter( const TextureSemantic& semantic ) const;

    /**
     * Get the texture transform associated with the given semantic
     * @param semantic
     * @return a raw pointer to the texture transform, nullptr if thereis no transformation.
     * @note ownership is kept by the GLTFMaterial
     */
    [[nodiscard]] inline const Core::Material::GLTFTextureTransform*
    getTextureTransform( const TextureSemantic& semantic ) const;

  private:
    inline Ra::Engine::Data::TextureParameters&
    addTexture( const TextureSemantic& type, const Ra::Engine::Data::TextureParameters& texture );

  protected:
    GLTFMaterial& m_baseMaterial;

    std::map<TextureSemantic, Ra::Engine::Data::Texture*> m_textures;
    std::map<TextureSemantic, Ra::Engine::Data::TextureParameters> m_pendingTextures;
    std::map<TextureSemantic, std::unique_ptr<Core::Material::GLTFTextureTransform>>
        m_textureTransform;
};

/**
 * \brief Radium Engine representation and management of the clearcoat layer
 */
class RA_ENGINE_API GLTFClearcoat : public GLTFMaterialExtension
{
  public:
    GLTFClearcoat( GLTFMaterial& baseMaterial,
                   const std::string& instanceName,
                   const Core::Material::GLTFClearcoatLayer* source );

    /** Texture semantics allowed for this material
     *  GLTFMaterial manage the following semantics :
     *    "TEX_CLEARCOAT"
     *    "TEX_CLEARCOATROUGHNESS"
     *    "TEX_CLEARCOATNORMAL"
     */

    void updateGL() override;
    void updateFromParameters() override;
    [[nodiscard]] std::list<std::string> getPropertyList() const override;

  private:
    float m_clearcoatFactor { 0. };
    float m_clearcoatRoughnessFactor { 0. };
    float m_clearcoatNormalTextureScale { 1 };
};

/**
 * \brief Radium Engine representation and management of the specular layer
 */
class RA_ENGINE_API GLTFSpecular : public GLTFMaterialExtension
{
  public:
    GLTFSpecular( GLTFMaterial& baseMaterial,
                  const std::string& instanceName,
                  const Core::Material::GLTFSpecularLayer* source );

    /** Texture semantics allowed for this material
     *  GLTFMaterial manage the following semantics :
     *    "TEXTURE_SPECULAR_EXT"
     *    "TEXTURE_SPECULARCOLOR_EXT"
     */

    void updateGL() override;
    void updateFromParameters() override;
    [[nodiscard]] std::list<std::string> getPropertyList() const override;

  private:
    float m_specularFactor { 1. };
    Ra::Core::Utils::Color m_specularColorFactor { 1.0, 1.0, 1.0, 1.0 };
};

/**
 * \brief Radium Engine representation and management of the sheen layer
 */
class RA_ENGINE_API GLTFSheen : public GLTFMaterialExtension
{
  public:
    GLTFSheen( GLTFMaterial& baseMaterial,
               const std::string& instanceName,
               const Core::Material::GLTFSheenLayer* source );

    /** Texture semantics allowed for this material
     *  GLTFMaterial manage the following semantics :
     *    "TEXTURE_SHEEN_COLOR"
     *    "TEXTURE_SHEEN_ROUGHNESS"
     */

    void updateGL() override;
    void updateFromParameters() override;
    [[nodiscard]] std::list<std::string> getPropertyList() const override;

  private:
    Ra::Core::Utils::Color m_sheenColorFactor { 0.0, 0.0, 0.0, 0.0 };
    float m_sheenRoughnessFactor { 0. };
};

/**
 * Radium Engine material representation of pbrMetallicRoughness
 *
 */
class RA_ENGINE_API GLTFMaterial : public Material, public ParameterSetEditingInterface
{
  public:
    /** Texture semantics allowed for this material
     *  GLTFMaterial manage the following semantics :
     *    "TEX_NORMAL"
     *    "TEX_OCCLUSION"
     *    "TEX_EMISSIVE"
     */
    using TextureSemantic = std::string;

  public:
    /**
     * Constructor of a named material
     * @param name
     */
    explicit GLTFMaterial( const std::string& name, const std::string& materialName );

    /**
     * Destructor
     */
    ~GLTFMaterial() override;

    /**
     * Add a texture to the material for the given semantic
     * @param semantic
     * @param texture
     * @param sampler
     * @return
     */
    inline Ra::Engine::Data::TextureParameters&
    addTexture( const TextureSemantic& semantic,
                const std::string& texture,
                const Core::Material::GLTFSampler& sampler );

    /**
     * Get the Radium texture associated with the given semantic
     * @param semantic
     * @return
     */
    [[nodiscard]] inline Ra::Engine::Data::Texture*
    getTexture( const TextureSemantic& semantic ) const;

    /**
     * Get the Radium texture (parameter struct) associated with the given semantic
     * @param semantic
     * @return The TextureParameter description
     */
    [[nodiscard]] inline std::shared_ptr<Ra::Engine::Data::TextureParameters>
    getTextureParameter( const TextureSemantic& semantic ) const;

    /**
     * Get the texture transform associated with the given semantic
     * @param semantic
     * @return a raw pointer to the texture transform, nullptr if thereis no transformation.
     * @note ownership is kept by the GLTFMaterial
     */
    [[nodiscard]] virtual const Core::Material::GLTFTextureTransform*
    getTextureTransform( const TextureSemantic& semantic ) const;

    /**
     * Update the OpenGL component of the material
     */
    void updateGL() override;

    /**
     * Update the state of the material from its render Parameters
     */
    void updateFromParameters() override;

    /**
     *
     * @return true if the material is transperent. Depends on the material parameters
     */
    [[nodiscard]] bool isTransparent() const override;

    /**
     * Get the list of properties the material might use in a shader.
     */
    [[nodiscard]] std::list<std::string> getPropertyList() const override;

    /**
     * Get a json containing metadata about the parameters of the material.
     */
    nlohmann::json getParametersMetadata() const override;

    /**
     * \brief Makes the Material take its base color from the VERTEX_COLOR attribute of the rendered
     * geometry \param state activate (true) or deactivate (false) VERTEX_COLOR attribute usage
     *
     * Any material that support per-vertex color parameterization should implement this method
     * accordingly
     */
    void setColoredByVertexAttrib( bool state ) override { m_isColoredByVertex = state; };

    /**
     * \brief Indicates if the material takes the VERTEX_COLOR attribute into account.
     */
    bool isColoredByVertexAttrib() const override { return m_isColoredByVertex; }

    /**
     * Register the material to the Radium Material subsystem
     */
    static void registerMaterial();

    /**
     * Remove the material from the Radium material subsystem
     */
    static void unregisterMaterial();

    /**
     * Initialize from a BaseGLTFMaterial after reading
     */
    void fillBaseFrom( const Core::Material::BaseGLTFMaterial* source );

    /******************************************************************/
    /*          Inline methods          */
    /******************************************************************/

    float getNormalTextureScale() const { return m_normalTextureScale; }
    void setNormalTextureScale( float normalTextureScale ) {
        m_normalTextureScale = normalTextureScale;
    }

    float getOcclusionStrength() const { return m_occlusionStrength; }
    void seOcclusionStrength( float occlusionStrength ) { m_occlusionStrength = occlusionStrength; }

    const Ra::Core::Utils::Color& getEmissiveFactor() const { return m_emissiveFactor; }
    void setEmissiveFactor( const Ra::Core::Utils::Color& emissiveFactor ) {
        m_emissiveFactor = emissiveFactor;
    }

    Core::Material::AlphaMode getAlphaMode() const { return m_alphaMode; }
    void setAlphaMode( Core::Material::AlphaMode alphaMode ) { m_alphaMode = alphaMode; }

    float getAlphaCutoff() const { return m_alphaCutoff; }
    void setAlphaCutoff( float alphaCutoff ) { m_alphaCutoff = alphaCutoff; }

    bool isDoubleSided() const { return m_doubleSided; }
    void setDoubleSided( bool doubleSided ) { m_doubleSided = doubleSided; }

    float getIndexOfRefraction() const { return m_indexOfRefraction; }
    void setIndexOfRefraction( float ior ) { m_indexOfRefraction = ior; }

    float getUnlitStatus() const { return m_isUnlit; }
    void setUnlitStatus( bool status ) { m_isUnlit = status; }

    /******************************************************************/

  protected:
    inline Ra::Engine::Data::TextureParameters&
    addTexture( const TextureSemantic& type, const Ra::Engine::Data::TextureParameters& texture );
    inline void addTexture( const TextureSemantic& type, Ra::Engine::Data::Texture* texture );

    float m_normalTextureScale { 1 };
    float m_occlusionStrength { 1 };
    Ra::Core::Utils::Color m_emissiveFactor { 0.0, 0.0, 0.0, 1.0 };
    Core::Material::AlphaMode m_alphaMode { Core::Material::AlphaMode::Opaque };
    float m_alphaCutoff { 0.5 };
    bool m_doubleSided { false };

    // attributes having default value in the spec with allowed modifications from extensions
    float m_indexOfRefraction { 1.5 };

    // Should the material be lit ? (manage by the extension KHR_materials_unlit),
    bool m_isUnlit { false };

    std::map<TextureSemantic, Ra::Engine::Data::Texture*> m_textures;
    std::map<TextureSemantic, Ra::Engine::Data::TextureParameters> m_pendingTextures;

    std::unique_ptr<Core::Material::GLTFTextureTransform> m_normalTextureTransform { nullptr };
    std::unique_ptr<Core::Material::GLTFTextureTransform> m_occlusionTextureTransform { nullptr };
    std::unique_ptr<Core::Material::GLTFTextureTransform> m_emissiveTextureTransform { nullptr };

    std::vector<std::unique_ptr<GLTFMaterialExtension>> m_layers {};

    static std::string s_shaderBasePath;

    static nlohmann::json m_parametersMetadata;

  private:
    friend class GLTFSheen;

    static bool s_bsdfLutsLoaded;
    static Ra::Engine::Data::Texture* s_ggxlut;
    static Ra::Engine::Data::Texture* s_sheenElut;
    static Ra::Engine::Data::Texture* s_charlielut;

    using GltfAlphaModeEnumConverter = typename Ra::Core::Utils::EnumConverter<
        typename std::underlying_type<Core::Material::AlphaMode>::type>;
    static std::shared_ptr<GltfAlphaModeEnumConverter> s_AlphaModeEnum;

    bool m_isColoredByVertex { false };

  protected:
    // todo : make this private with set/reset methods
    bool m_isOpenGlConfigured { false };
};

/* -------------------------------------------------------------------------------------------- */

inline void GLTFMaterial::addTexture( const TextureSemantic& type,
                                      Ra::Engine::Data::Texture* texture ) {
    m_textures[type] = texture;
    m_pendingTextures.erase( type );
}

inline Ra::Engine::Data::TextureParameters&
GLTFMaterial::addTexture( const TextureSemantic& semantic,
                          const std::string& texture,
                          const Core::Material::GLTFSampler& sampler ) {

    Ra::Engine::Data::TextureParameters textureParams;
    textureParams.name = texture;
    switch ( sampler.wrapS ) {
    case Core::Material::GLTFSampler::WrappingMode::Repeat:
        textureParams.wrapS = gl::GL_REPEAT;
        break;
    case Core::Material::GLTFSampler::WrappingMode::MirroredRepeat:
        textureParams.wrapS = gl::GL_MIRRORED_REPEAT;
        break;
    case Core::Material::GLTFSampler::WrappingMode::ClampToEdge:
        textureParams.wrapS = gl::GL_CLAMP_TO_EDGE;
        break;
    }
    switch ( sampler.wrapT ) {
    case Core::Material::GLTFSampler::WrappingMode::Repeat:
        textureParams.wrapT = gl::GL_REPEAT;
        break;
    case Core::Material::GLTFSampler::WrappingMode::MirroredRepeat:
        textureParams.wrapT = gl::GL_MIRRORED_REPEAT;
        break;
    case Core::Material::GLTFSampler::WrappingMode::ClampToEdge:
        textureParams.wrapT = gl::GL_CLAMP_TO_EDGE;
        break;
    }
    switch ( sampler.magFilter ) {
    case Core::Material::GLTFSampler::MagFilter::Nearest:
        textureParams.magFilter = gl::GL_NEAREST;
        break;
    case Core::Material::GLTFSampler::MagFilter::Linear:
        textureParams.magFilter = gl::GL_LINEAR;
        break;
    }
    switch ( sampler.minFilter ) {
    case Core::Material::GLTFSampler::MinFilter::Nearest:
        textureParams.minFilter = gl::GL_NEAREST;
        break;
    case Core::Material::GLTFSampler::MinFilter::Linear:
        textureParams.minFilter = gl::GL_LINEAR;
        break;
    case Core::Material::GLTFSampler::MinFilter::NearestMipMapNearest:
        textureParams.minFilter = gl::GL_NEAREST_MIPMAP_NEAREST;
        break;
    case Core::Material::GLTFSampler::MinFilter::LinearMipMapNearest:
        textureParams.minFilter = gl::GL_LINEAR_MIPMAP_NEAREST;
        break;
    case Core::Material::GLTFSampler::MinFilter::NearestMipMapLinear:
        textureParams.minFilter = gl::GL_NEAREST_MIPMAP_LINEAR;
        break;
    case Core::Material::GLTFSampler::MinFilter::LinearMipMapLinear:
        textureParams.minFilter = gl::GL_LINEAR_MIPMAP_LINEAR;
        break;
    }

    return addTexture( semantic, textureParams );
}

inline Ra::Engine::Data::TextureParameters&
GLTFMaterial::addTexture( const TextureSemantic& type,
                          const Ra::Engine::Data::TextureParameters& texture ) {
    m_pendingTextures[type] = texture;
    m_isDirty               = true;

    return m_pendingTextures[type];
}

inline Ra::Engine::Data::Texture*
GLTFMaterial::getTexture( const TextureSemantic& semantic ) const {
    Ra::Engine::Data::Texture* tex = nullptr;

    auto it = m_textures.find( semantic );
    if ( it != m_textures.end() ) { tex = it->second; }

    return tex;
}

inline std::shared_ptr<Ra::Engine::Data::TextureParameters>
GLTFMaterial::getTextureParameter( const TextureSemantic& semantic ) const {
    Ra::Engine::Data::Texture* tex = getTexture( semantic );
    if ( tex == nullptr ) {
        auto it = m_pendingTextures.find( semantic );
        if ( it != m_pendingTextures.end() ) {
            return std::make_shared<Ra::Engine::Data::TextureParameters>( it->second );
        }
    }
    else { return std::make_shared<Ra::Engine::Data::TextureParameters>( tex->getParameters() ); }
    return nullptr;
}

inline Ra::Engine::Data::TextureParameters&
GLTFMaterialExtension::addTexture( const TextureSemantic& semantic,
                                   const std::string& texture,
                                   const Core::Material::GLTFSampler& sampler ) {

    Ra::Engine::Data::TextureParameters textureParams;
    textureParams.name = texture;
    switch ( sampler.wrapS ) {
    case Core::Material::GLTFSampler::WrappingMode::Repeat:
        textureParams.wrapS = gl::GL_REPEAT;
        break;
    case Core::Material::GLTFSampler::WrappingMode::MirroredRepeat:
        textureParams.wrapS = gl::GL_MIRRORED_REPEAT;
        break;
    case Core::Material::GLTFSampler::WrappingMode::ClampToEdge:
        textureParams.wrapS = gl::GL_CLAMP_TO_EDGE;
        break;
    }
    switch ( sampler.wrapT ) {
    case Core::Material::GLTFSampler::WrappingMode::Repeat:
        textureParams.wrapT = gl::GL_REPEAT;
        break;
    case Core::Material::GLTFSampler::WrappingMode::MirroredRepeat:
        textureParams.wrapT = gl::GL_MIRRORED_REPEAT;
        break;
    case Core::Material::GLTFSampler::WrappingMode::ClampToEdge:
        textureParams.wrapT = gl::GL_CLAMP_TO_EDGE;
        break;
    }
    switch ( sampler.magFilter ) {
    case Core::Material::GLTFSampler::MagFilter::Nearest:
        textureParams.magFilter = gl::GL_NEAREST;
        break;
    case Core::Material::GLTFSampler::MagFilter::Linear:
        textureParams.magFilter = gl::GL_LINEAR;
        break;
    }
    switch ( sampler.minFilter ) {
    case Core::Material::GLTFSampler::MinFilter::Nearest:
        textureParams.minFilter = gl::GL_NEAREST;
        break;
    case Core::Material::GLTFSampler::MinFilter::Linear:
        textureParams.minFilter = gl::GL_LINEAR;
        break;
    case Core::Material::GLTFSampler::MinFilter::NearestMipMapNearest:
        textureParams.minFilter = gl::GL_NEAREST_MIPMAP_NEAREST;
        break;
    case Core::Material::GLTFSampler::MinFilter::LinearMipMapNearest:
        textureParams.minFilter = gl::GL_LINEAR_MIPMAP_NEAREST;
        break;
    case Core::Material::GLTFSampler::MinFilter::NearestMipMapLinear:
        textureParams.minFilter = gl::GL_NEAREST_MIPMAP_LINEAR;
        break;
    case Core::Material::GLTFSampler::MinFilter::LinearMipMapLinear:
        textureParams.minFilter = gl::GL_LINEAR_MIPMAP_LINEAR;
        break;
    }

    return addTexture( semantic, textureParams );
}

inline Ra::Engine::Data::TextureParameters&
GLTFMaterialExtension::addTexture( const TextureSemantic& type,
                                   const Ra::Engine::Data::TextureParameters& texture ) {
    m_pendingTextures[type] = texture;
    m_isDirty               = true;

    return m_pendingTextures[type];
}

inline Ra::Engine::Data::Texture*
GLTFMaterialExtension::getTexture( const TextureSemantic& semantic ) const {
    auto it = m_textures.find( semantic );
    if ( it != m_textures.end() ) { return it->second; }
    return nullptr;
}

inline std::shared_ptr<Ra::Engine::Data::TextureParameters>
GLTFMaterialExtension::getTextureParameter( const TextureSemantic& semantic ) const {
    Ra::Engine::Data::Texture* tex = getTexture( semantic );
    if ( tex == nullptr ) {
        auto it = m_pendingTextures.find( semantic );
        if ( it != m_pendingTextures.end() ) {
            return std::make_shared<Ra::Engine::Data::TextureParameters>( it->second );
        }
    }
    else { return std::make_shared<Ra::Engine::Data::TextureParameters>( tex->getParameters() ); }
    return nullptr;
}

const Core::Material::GLTFTextureTransform*
GLTFMaterialExtension::getTextureTransform( const TextureSemantic& semantic ) const {
    auto it = m_textureTransform.find( semantic );
    if ( it != m_textureTransform.end() ) { return it->second.get(); }
    return nullptr;
}

} // namespace Data
} // namespace Engine
} // namespace Ra
