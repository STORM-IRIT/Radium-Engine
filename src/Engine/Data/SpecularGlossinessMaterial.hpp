#pragma once
#include <Engine/RaEngine.hpp>

#include <Engine/Data/GLTFMaterial.hpp>

namespace Ra::Engine {
class Texture;
} // namespace Ra::Engine

namespace Ra {
namespace Engine {
namespace Data {

/**
 * Radium IO to Engine conversion for pbrSpecularGlossiness
 */
class RA_ENGINE_API SpecularGlossinessMaterialConverter
{
  public:
    Ra::Engine::Data::Material* operator()( const Ra::Core::Asset::MaterialData* toconvert );
};

/**
 * Radium Engine material representation of GLTF SpecularGlossiness Material
 * Texture semantics defined by this material :
 * "TEX_DIFFUSE"
 * "TEX_SPECULARGLOSSINESS"
 *
 */
class RA_ENGINE_API SpecularGlossiness final : public GLTFMaterial
{
    friend class SpecularGlossinessMaterialConverter;

  public:
  public:
    /**
     * Register the material to the Radium Material subsystem
     */
    static void registerMaterial();

    /**
     * Remove the material from the Radium material subsystem
     */
    static void unregisterMaterial();

    /**
     * Constructor of a named material
     * @param instanceName
     */
    explicit SpecularGlossiness( const std::string& instanceName );

    /**
     * Destructor
     */
    ~SpecularGlossiness() override;

    /**
     * Update the OpenGL component of the material
     */
    void updateGL() override;

    /**
     * Update the state of the material from its render Parameters
     */
    void updateFromParameters() override;

    /**
     * Get the list of properties the material migh use in a shader.
     */
    [[nodiscard]] std::list<std::string> getPropertyList() const override;

    /**
     * Get the texture transform associated with the given semantic
     * @param semantic
     * @return a raw pointer to the texture transform, nullptr if thereis no transformation.
     * @note ownership is kept by the GLTFMaterial
     */
    [[nodiscard]] const Core::Material::GLTFTextureTransform*
    getTextureTransform( const TextureSemantic& semantic ) const override;

    /******************************************************************/
    /**
     * @return the diffuse factor of the material
     */
    const Ra::Core::Utils::Color& getDiffuseFactor() const { return m_diffuseFactor; }
    /**
     * @param diffuseFactor the diffuse factor to set
     */
    void setDiffuseFactor( const Ra::Core::Utils::Color& diffuseFactor ) {
        m_diffuseFactor = diffuseFactor;
    }
    /**
     * @return the specular factor of the material
     */
    const Ra::Core::Utils::Color& getSpecularFactor() const { return m_specularFactor; }
    /**
     * @param specularFactor the specular factor to set
     */
    void setSpecularFactor( const Ra::Core::Utils::Color& specularFactor ) {
        m_specularFactor = specularFactor;
    }
    /**
     * @return the glossiness factor of the material
     */
    float getGlossinessFactor() const { return m_glossinessFactor; }
    /**
     * @param glossinessFactor the glossiness factor to set
     */
    void setGlossinessFactor( float glossinessFactor ) { m_glossinessFactor = glossinessFactor; }
    /******************************************************************/
  private:
    // attributes of SpecularGlossiness
    Ra::Core::Utils::Color m_diffuseFactor { 1.0, 1.0, 1.0, 1.0 };
    Ra::Core::Utils::Color m_specularFactor { 1.0, 1.0, 1.0, 1.0 };
    float m_glossinessFactor { 1 };

    static const std::string m_materialName;

    std::unique_ptr<Core::Material::GLTFTextureTransform> m_diffuseTextureTransform { nullptr };
    std::unique_ptr<Core::Material::GLTFTextureTransform> m_specularGlossinessTransform { nullptr };
};

} // namespace Data
} // namespace Engine
} // namespace Ra
