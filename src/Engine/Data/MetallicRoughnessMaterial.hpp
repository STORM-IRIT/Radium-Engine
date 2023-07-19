#pragma once

#include <Engine/RaEngine.hpp>

#include <Engine/Data/GLTFMaterial.hpp>

namespace Ra::Engine {
class Texture;
} // namespace Ra::Engine

namespace Ra {
namespace Engine {
namespace Data {

class MetallicRoughnessMaterialConverter;

/**
 * Radium Engine material representation of pbrMetallicRoughness
 *
 * Texture semantics defined by this material :
 * "TEX_BASECOLOR"
 * "TEX_METALLICROUGHNESS"
 *
 */
class RA_ENGINE_API MetallicRoughness final : public GLTFMaterial
{
    friend class MetallicRoughnessMaterialConverter;

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
    explicit MetallicRoughness( const std::string& instanceName );

    /**
     * Destructor
     */
    ~MetallicRoughness() override;

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
    const Ra::Core::Utils::Color& getBaseColorFactor() const { return m_baseColorFactor; }
    void setBaseColorFactor( const Ra::Core::Utils::Color& baseColorFactor ) {
        m_baseColorFactor = baseColorFactor;
    }

    float getMetallicFactor() const { return m_metallicFactor; }
    void setMetallicFactor( float metallicFactor ) { m_metallicFactor = metallicFactor; }

    float getRoughnessFactor() const { return m_roughnessFactor; }
    void setRoughnessFactor( float roughnessFactor ) { m_roughnessFactor = roughnessFactor; }

    /******************************************************************/

  private:
    // attributes of MetallicRoughness
    Ra::Core::Utils::Color m_baseColorFactor { 1.0, 1.0, 1.0, 1.0 };
    float m_metallicFactor { 1 };
    float m_roughnessFactor { 1 };
    static const std::string m_materialName;

    std::unique_ptr<Core::Material::GLTFTextureTransform> m_baseTextureTransform { nullptr };
    std::unique_ptr<Core::Material::GLTFTextureTransform> m_metallicRoughnessTextureTransform {
        nullptr };
};

} // namespace Data
} // namespace Engine
} // namespace Ra
