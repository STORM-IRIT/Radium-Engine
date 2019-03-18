#ifndef RADIUMENGINE_RENDERTECHNIQUE_HPP
#define RADIUMENGINE_RENDERTECHNIQUE_HPP

#include <Engine/RaEngine.hpp>

#include <Engine/Renderer/RenderTechnique/ShaderConfiguration.hpp>

#include <functional>
#include <map>
#include <memory>

namespace Ra {
namespace Engine {
class ShaderProgram;
class Material;
} // namespace Engine
} // namespace Ra

namespace Ra {
namespace Engine {
/* Radium V2 : make this class non final and extensible.
 * passName must be renderer independant (use a map so that each renderer could set its own passes.
 * Rely this to future composition based material and renderer architecture.
 */
/**
 * Set of shaders to be used by the Renderer to RenderObjects with Materials.
 * \see The Render technique and materials section in the Material management
 *      in the Radium Engine documentation.
 */
class RA_ENGINE_API RenderTechnique final {
  public:
    /**
     * Type of rendering pass to apply.
     */
    enum PassName {
        Z_PREPASS = 0x1 << 0,
        LIGHTING_OPAQUE = 0x1 << 1,
        LIGHTING_TRANSPARENT = 0x1 << 2,
        NO_PASS = 0
    };

    RenderTechnique();

    RenderTechnique( const RenderTechnique& );

    ~RenderTechnique();

    /**
     * Set the ShaderConfiguration to be used for the given rendering pass.
     */
    void setConfiguration( const ShaderConfiguration& newConfig, PassName pass = LIGHTING_OPAQUE );

    /**
     * Return the ShaderConfiguration to be used for the given rendering pass.
     */
    ShaderConfiguration getConfiguration( PassName pass = LIGHTING_OPAQUE ) const;

    /**
     * Return the Shader to be used for the given rendering pass.
     */
    const ShaderProgram* getShader( PassName pass = LIGHTING_OPAQUE ) const;

    /**
     * Set the MAterial tu be used for rendering.
     */
    void setMaterial( const std::shared_ptr<Material>& material );

    /**
     * Return the Shader to be used for rendering.
     */
    const std::shared_ptr<Material>& getMaterial() const;

    /**
     * Set the Material tu be used for rendering.
     */
    void resetMaterial( Material* mat );

    /**
     * Upload all the rendering data to the GPU.
     */
    void updateGL();

    /**
     * Return true if the Shader for the given rendering pass is not up-to-date.
     */
    bool shaderIsDirty( PassName pass = LIGHTING_OPAQUE ) const;

    /**
     * creates a Radium default rendertechnique with passes:
     *   - Z_PREPASS = DepthDepthAmbientPass
     *   - LIGHTING_OPAQUE = BlinnPhong
     *   - LIGHTING_TRANSPARENT = LitOIT
     */
    static Ra::Engine::RenderTechnique createDefaultRenderTechnique();

  private:
    using ConfigurationSet = std::map<PassName, ShaderConfiguration>;

    using ShaderSet = std::map<PassName, const ShaderProgram*>;

    /// The list of per-rendering pass ShaderConfigurations.
    ConfigurationSet shaderConfig;

    /// The list of per-rendering pass Shaders.
    ShaderSet shaders;

    /// The material to be used for rendering.
    std::shared_ptr<Material> material{nullptr};

    // Change this if there is more than 8 configurations
    /// Dirty bits for each pass Shader.
    unsigned char dirtyBits{Z_PREPASS | LIGHTING_OPAQUE | LIGHTING_TRANSPARENT};

    /// Activation bits for each rendering pass.
    unsigned char setPasses{NO_PASS};
};

/**
 * Radium defined Technique.
 */
namespace EngineRenderTechniques {

/**
 * A default technique function is a function that will fill the given
 * RenderTechnique with the default configuration associated to a material.
 */
using DefaultTechniqueBuilder = std::function<void( RenderTechnique&, bool )>;

/**
 * Register a new default builder for a technique.
 * \return true if the builder has been successfully added, false otherwise
 *         (e.g., a builder with the same name already exists).
 */
RA_ENGINE_API bool registerDefaultTechnique( const std::string& name,
                                             DefaultTechniqueBuilder builder );

/**
 * Remove a default builder.
 * \return true if the builder has been successfully removed, false otherwise
 *         (e.g.\, a builder with the given name does't exist).
 */
RA_ENGINE_API bool removeDefaultTechnique( const std::string& name );

/**
  Look for a RenderTechnique builder with the given name.
 * \return a pair containing:
 *          - true if a RenderTechnique builder has been found, false otherwise.
 *          - the found RenderTechnique builder if it has been found.
 */
RA_ENGINE_API std::pair<bool, DefaultTechniqueBuilder>
getDefaultTechnique( const std::string& name );

} // namespace EngineRenderTechniques

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_RENDERTECHNIQUE_HPP
