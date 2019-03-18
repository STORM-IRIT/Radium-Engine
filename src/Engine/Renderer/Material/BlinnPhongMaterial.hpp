#ifndef RADIUMENGINE_BLINNPHONGMATERIAL_HPP
#define RADIUMENGINE_BLINNPHONGMATERIAL_HPP

#include <Engine/RaEngine.hpp>

#include <map>
#include <string>

#include <Core/Utils/Color.hpp>
#include <Engine/Renderer/Material/Material.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>

namespace Ra {
namespace Core {
namespace Asset {
class MaterialData;
} // namespace Asset
} // namespace Core

namespace Engine {

class ShaderProgram;

/**
 * Implementation of the Blinn-Phong Material BSDF.
 */
class RA_ENGINE_API BlinnPhongMaterial final : public Material {
    friend class BlinnPhongMaterialConverter;

  public:
    /**
     * Semantic of the texture: define which BSDF parameter is controled by the texture.
     */
    enum class TextureSemantic { TEX_DIFFUSE, TEX_SPECULAR, TEX_NORMAL, TEX_SHININESS, TEX_ALPHA };

  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    /**
     * Construct a named Blinn-Phong Material.
     * \param name The name of the Material.
     */
    explicit BlinnPhongMaterial( const std::string& name );

    /**
     * Destructor.
     * \note The Material does not have ownership on its textures.
     *       This destructor do not delete the associated textures.
     */
    ~BlinnPhongMaterial() override;

    void updateGL() override;

    void bind( const ShaderProgram* shader ) override;

    const std::string getShaderInclude() const override;

    bool isTransparent() const override;

    /**
     * Add an already existing Texture to control the specified BSDF parameter.
     * \param semantic The TextureSemantic.
     * \param texture  The Texture to use.
     */
    inline void addTexture( const TextureSemantic& semantic, Texture* texture );

    /**
     * Add an new Texture to control the specified BSDF parameter.
     * \param semantic The TextureSemantic.
     * \param texture  The TextureParameters.
     * \return the corresponding TextureParameters.
     */
    inline TextureParameters& addTexture( const TextureSemantic& semantic,
                                          const TextureParameters& texture );

    /**
     * Return the Texture associated to the given TextureSemantic.
     */
    inline Texture* getTexture( const TextureSemantic& semantic ) const;

    /**
     * Register the Material in the Material library.
     * After registration, the Material could be instantiated by any Radium
     * System, Renderer, Plugin, ...
     */
    static void registerMaterial();

    /**
     * Remove the Material from the Material library.
     * After removal, the Material is no more available.
     */
    static void unregisterMaterial();

  public:
    /// The diffuse color of the Material.
    Core::Utils::Color m_kd{0.9, 0.9, 0.9, 1.0};

    /// The specular color of the Material.
    Core::Utils::Color m_ks{0.0, 0.0, 0.0, 1.0};

    /// The shininess exponent of the Material.
    Scalar m_ns{1.0};

    /// The alpha value of the Material.
    Scalar m_alpha{1.0};

  private:
    /**
     * Add an new Texture, from a given file, to control the specified BSDF parameter.
     * \param semantic The TextureSemantic.
     * \param texture  The Texture filename to use.
     * \return the corresponding TextureParameters.
     */
    inline TextureParameters& addTexture( const TextureSemantic& semantic,
                                          const std::string& texture );

  private:
    /// The list of initialized Textures.
    std::map<TextureSemantic, Texture*> m_textures;

    /// The lits of to-initialize Textures.
    std::map<TextureSemantic, TextureParameters> m_pendingTextures;
};

/**
 * Converter from an external representation coming as a FileData
 * to the internal representation.
 */
class RA_ENGINE_API BlinnPhongMaterialConverter final {
  public:
    BlinnPhongMaterialConverter() = default;

    ~BlinnPhongMaterialConverter() = default;

    Material* operator()( const Ra::Core::Asset::MaterialData* toconvert );
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Material/BlinnPhongMaterial.inl>
#endif // RADIUMENGINE_BLINNPHONGMATERIAL_HPP
