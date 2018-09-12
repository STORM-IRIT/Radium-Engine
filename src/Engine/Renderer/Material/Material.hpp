#ifndef RADIUMENGINE_MATERIAL_HPP
#define RADIUMENGINE_MATERIAL_HPP

#include <Engine/RaEngine.hpp>

#include <map>
#include <string>

#include <Core/Math/LinearAlgebra.hpp>

namespace Ra {
namespace Engine {
class ShaderProgram;
} // namespace Engine
} // namespace Ra

namespace Ra {
namespace Engine {

/**
 * Base class for materials/
 * Do not assume a given Material representation but only make the difference between opaque and
 * transparent materials.
 *
 */
class RA_ENGINE_API Material {
  public:
    /// Used to indicate if the Material is opaque or transparent.
    enum class MaterialAspect { MAT_OPAQUE, MAT_TRANSPARENT };

  public:
    RA_CORE_ALIGNED_NEW

    explicit Material( const std::string& name,
                       MaterialAspect aspect = MaterialAspect::MAT_OPAQUE );

    virtual ~Material();

    /** Update the OpenGL states used by the material.
     * These state could be textures, precomputed tables or whater data
     * associated to the material and given to OpenGL as a buffer object.
     */
    virtual void updateGL() = 0;

    /** Bind the material to the given shader.
     * This method must set the uniforms and textures of the shader to reflect
     * the state of the material.
     * @param shader
     */
    virtual void bind( const ShaderProgram* shader ) = 0;

    /**
     * @return the name of the material.
     */
    inline const std::string& getName() const;

    /** Get the shader file that define the glsl code to evaluate the material.
     * @note Not used yet by the Engine.
     * @return the glsl file implementing the GLSL material interface.
     */
    virtual const std::string getShaderInclude() const;

    /** set the aspect (MAT_OPAQUE or MAT_TRANSPARENT) of the material.
     * @param aspect
     */
    inline void setMaterialAspect( const MaterialAspect& aspect );

    /** Get the aspect (MAT_OPAQUE or MAT_TRANSPARENT) of the material.
     *
     * @return
     */
    inline const MaterialAspect& getMaterialAspect() const;

    /** Test if material is transperent.
     * @return true if the material is transparent
     */
    virtual bool isTransparent() const;

  protected:
    /// The material's name.
    std::string m_name;

    /// Whether the material data should be updated to the GPU.
    bool m_isDirty;

    /// The aspect of the material.
    MaterialAspect m_aspect;
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Material/Material.inl>
#endif
