#pragma once

#include <Engine/Data/SimpleMaterial.hpp>

namespace Ra {
namespace Engine {
namespace Data {

/**
 * Implementation of the Lambertian Material BSDF.
 * This material implements a lambertian diffuse BSDF.
 * This material could not be loaded from a file and must be defined and associated to
 * renderobjects programatically.
 *
 * This Material is parameterized either globally, by setting the m_color base color or locally,
 * by setting a vertex color attribute that define the per vertex color and setting m_perVertexColor
 * to true.
 *
 */
class RA_ENGINE_API LambertianMaterial final : public SimpleMaterial
{
  public:
    /**
     * Construct a named Lambertian material
     * \param instanceName The name of the material
     */
    explicit LambertianMaterial( const std::string& instanceName );

    /**
     * Destructor.
     * \note The material does not have ownership on its texture nor its shaders.
     * This destructor do not delete the associated textures and the corresponding shaders.
     */
    ~LambertianMaterial() override;

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

    void updateFromParameters() override;

    /**
     * Get a json containing metadata about the parameters of the material.
     *
     * \return the metadata in json format
     */
    nlohmann::json getParametersMetadata() const override;

  private:
    static nlohmann::json s_parametersMetadata;
};

} // namespace Data
} // namespace Engine
} // namespace Ra
