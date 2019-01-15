#ifndef RADIUMENGINE_BLINNPHONGMATERIAL_HPP
#define RADIUMENGINE_BLINNPHONGMATERIAL_HPP

#include <Engine/RaEngine.hpp>

#include <map>
#include <string>

#include <Core/Utils/Color.hpp>
#include <Engine/Renderer/Material/Material.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>

namespace Ra {
namespace Asset {
class MaterialData;
}
namespace Engine {

class ShaderProgram;

/**
 * Implementation of the Blinn-Phong Material BSDF.
 */
class RA_ENGINE_API BlinnPhongMaterial final : public Material {
    friend class BlinnPhongMaterialConverter;

  public:
    /// Semantic of the texture : define which BSDF parameter is controled by the texture
    enum class TextureSemantic { TEX_DIFFUSE, TEX_SPECULAR, TEX_NORMAL, TEX_SHININESS, TEX_ALPHA };

  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    /**
     * Construct a named Blinn-Phongmaterial
     * @param name The name of the material
     */
    explicit BlinnPhongMaterial( const std::string& name );
    /**
     * Destructor.
     * @note The material does not have ownership on its texture. This destructor do not delete the
     * associated textures.
     */
    ~BlinnPhongMaterial() override;

    /**
     * Get the basename of the glsl source file to include if one want to build composite shaders
     * that use this material.
     * @return The basename (without extension .frag.glsl or .vert.glsl) of the glsl source file.
     */
    const std::string getShaderInclude() const override;

    void updateGL() override;
    void bind( const ShaderProgram* shader ) override;
    bool isTransparent() const override;

    /**
     * Add an already existing texture to control the specified BSDF parameter.
     * @param semantic The texture semantic
     * @param texture  The texture to use
     */
    inline void addTexture( const TextureSemantic& semantic, Texture* texture );

    /**
     * Get the texture associated to the given semantic.
     * @param semantic
     * @return
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

  public:
    Core::Utils::Color m_kd{0.9, 0.9, 0.9, 1.0};
    Core::Utils::Color m_ks{0.0, 0.0, 0.0, 1.0};
    Scalar m_ns{1.0};
    Scalar m_alpha{1.0};

    /**
     * Add an new texture, from a TextureData, to control the specified BSDF parameter.
     * @param semantic The texture semantic
     * @param texture  The texture to use (file)
     * @return the corresponding TextureData struct
     */
    inline TextureParameters& addTexture( const TextureSemantic& semantic,
                                          const TextureParameters& texture );

  private:
    std::map<TextureSemantic, Texture*> m_textures;
    std::map<TextureSemantic, TextureParameters> m_pendingTextures;

    /**
     * Add an new texture, from a given file, to control the specified BSDF parameter.
     * @param semantic The texture semantic
     * @param texture  The texture to use (file)
     * @return the corresponding TextureData struct
     */
    inline TextureParameters& addTexture( const TextureSemantic& semantic,
                                          const std::string& texture );
};

/**
 * Converter from an external representation comming from FileData to internal representation.
 */
class RA_ENGINE_API BlinnPhongMaterialConverter final {
  public:
    BlinnPhongMaterialConverter() = default;
    ~BlinnPhongMaterialConverter() = default;

    Material* operator()( const Ra::Asset::MaterialData* toconvert );
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Material/BlinnPhongMaterial.inl>
#endif // RADIUMENGINE_BLINNPHONGMATERIAL_HPP
