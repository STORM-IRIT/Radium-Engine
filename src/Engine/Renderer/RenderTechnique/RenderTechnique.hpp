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
/* Radium V2 : make this class  non final and extensible.
 * passName must be renderer independant (use a map so that each renderer could set its own passes.
 * Rely this to future composition based material and renderer architecture.
 */
/**
 * Set of shaders to be used by the renderer to render objects with materials.
 * @see Render technique and materials section in the Material management in the Radium Engine
 * documentation
 */
class RA_ENGINE_API RenderTechnique final
{
  public:
    enum PassName {
        Z_PREPASS            = 0x1 << 0,
        LIGHTING_OPAQUE      = 0x1 << 1,
        LIGHTING_TRANSPARENT = 0x1 << 2,
        NO_PASS              = 0
    };

    RenderTechnique();
    RenderTechnique( const RenderTechnique& );
    ~RenderTechnique();

    void setConfiguration( const ShaderConfiguration& newConfig, PassName pass = LIGHTING_OPAQUE );

    bool hasConfiguration( PassName pass ) const;
    const ShaderConfiguration& getConfiguration( PassName pass = LIGHTING_OPAQUE ) const;

    const ShaderProgram* getShader( PassName pass = LIGHTING_OPAQUE ) const;

    void setMaterial( const std::shared_ptr<Material>& material );
    const std::shared_ptr<Material>& getMaterial() const;

    void resetMaterial( Material* mat );

    void updateGL();
    bool shaderIsDirty( PassName pass = LIGHTING_OPAQUE ) const;

    // creates a Radium default rendertechnique :
    //      Z_PREPASS = DepthDepthAmbientPass
    //      LIGHTING_OPAQUE = BlinnPhong
    //      LIGHTING_TRANSPARENT = LitOIT
    static Ra::Engine::RenderTechnique createDefaultRenderTechnique();

  private:
    using ConfigurationSet = std::map<PassName, ShaderConfiguration>;
    using ShaderSet        = std::map<PassName, const ShaderProgram*>;
    ConfigurationSet m_shaderConfig;
    ShaderSet m_shaders;

    std::shared_ptr<Material> m_material{nullptr};

    // Change this if there is more than 8 configurations
    unsigned char m_dirtyBits{Z_PREPASS | LIGHTING_OPAQUE | LIGHTING_TRANSPARENT};
    unsigned char m_setPasses{NO_PASS};
};

///////////////////////////////////////////////
////        Radium defined technique        ///
///////////////////////////////////////////////
namespace EngineRenderTechniques {

/// A default technique builder is a function that will fill the given RenderTechnique with the
/// default configurations associated with a material
/// This function will take as param the render technique to fill and a boolean
/// set to true if the material might be transparent under som parameter values
using DefaultTechniqueBuilder = std::function<void( RenderTechnique&, bool )>;

/** register a new default builder for a material class
 *  @return true if builder added, false else (e.g, a builder with the same name already exists)
 */
RA_ENGINE_API bool registerDefaultTechnique( const std::string& name,
                                             DefaultTechniqueBuilder builder );

/** remove a default builder
 *  @return true if builder removed, false else (e.g, a builder with the same name does't exists)
 */
RA_ENGINE_API bool removeDefaultTechnique( const std::string& name );

/**
 * @param name name of the technique to construct
 * @return a pair containing the search result and, if true, the functor to call to build the
 * technique.
 */
RA_ENGINE_API std::pair<bool, DefaultTechniqueBuilder>
getDefaultTechnique( const std::string& name );

RA_ENGINE_API bool cleanup();
} // namespace EngineRenderTechniques

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_RENDERTECHNIQUE_HPP
