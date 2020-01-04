#ifndef RADIUMENGINE_PLAINMATERIAL_HPP
#define RADIUMENGINE_PLAINMATERIAL_HPP

#include <Engine/RaEngine.hpp>

#include <map>
#include <string>

#include <Core/Utils/Color.hpp>
#include <Engine/Renderer/Material/Material.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>

namespace Ra {
namespace Engine {

class ShaderProgram;

/**
 * Implementation of the Plain Material BSDF.
 * This material implements a lambertian diffuse BSDF.
 * This material could not be loaded from a file and must be defined and associated to
 * renderobjects programatically.
 */
class RA_ENGINE_API PlainMaterial final : public Material
{
  public:
    /// Semantic of the texture : define which BSDF parameter is controled by the texture
    enum class TextureSemantic { TEX_COLOR, TEX_MASK };

    /**
     * Construct a named Plain material
     * @param name The name of the material
     */
    explicit PlainMaterial( const std::string& instanceName );

    /**
     * Destructor.
     * @note The material does not have ownership on its texture nor its shaders.
     * This destructor do not delete the associated textures and the corresponding shaders.
     */
    ~PlainMaterial() override;

    /**
     * Update the openGL state of the material.
     * This state only consistes on associated textures. Shaders are automatically updated if needed
     * when accessed.
     */
    void updateGL() override;

    /**
     * Set the uniform parameters of the given shaders according to the Material properties.
     * @param shader the GLSL program to parameterize.
     */
    void bind( const ShaderProgram* shader ) override;

    /**
     * Add an new texture, from a TextureParameters, to control the specified BSDF parameter.
     *
     * @param semantic The texture semantic
     * @param texture  The texture to use (Descriptor of the texture by its parameters)
     * @return the corresponding TextureData struct
     */
    inline TextureParameters& addTexture( const TextureSemantic& semantic,
                                          const TextureParameters& texture );

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
    /// The base color of the material
    Core::Utils::Color m_color{0.9, 0.9, 0.9, 1.0};
    /// Indicates if the material will takes its base color from vertices' attributes.
    bool m_perVertexColor{false};
    /// Indicates if the material must be shaded or not.
    bool m_shaded{false};

  private:
    /**
     * The openGL initialized textures.
     */
    std::map<TextureSemantic, Texture*> m_textures;
    /**
     * The textures that are associated with the material but ar not yet loaded nor initialized.
     */
    std::map<TextureSemantic, TextureParameters> m_pendingTextures;
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Material/PlainMaterial.inl>
#endif // RADIUMENGINE_PLAINMATERIAL_HPP
