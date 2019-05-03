#ifndef RADIUMENGINE_MATERIAL_HPP
#define RADIUMENGINE_MATERIAL_HPP

#include <Engine/RaEngine.hpp>

#include <map>
#include <string>

// This include brings only the macro EIGEN_MAKE_ALIGNED_OPERATOR_NEW in the file scope.
// Need to be separated to reduce compilation time
#include <Core/Types.hpp>

namespace Ra {
namespace Engine {
class ShaderProgram;
}
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
    enum class MaterialAspect { MAT_OPAQUE, MAT_TRANSPARENT };

  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  protected:
    /**
     * Creates a named material with the given aspect
     * @param name
     * @param aspect
     */
    explicit Material( const std::string& instanceName, const std::string& materialName,
                       MaterialAspect aspect = MaterialAspect::MAT_OPAQUE );

  public:
    virtual ~Material() = default;

    /** Update the OpenGL states used by the material.
     * These state could be textures, precomputed tables or whater data associated to the material
     * and given to OpenGL as a buffer object.
     */
    virtual void updateGL() = 0;

    /** Bind the material to the given shader.
     * This method must set the uniforms and textures of the shader to reflect the state of the
     * material.
     * @param shader
     */
    virtual void bind( const ShaderProgram* shader ) = 0;

    /**
     * @return the name of the material instance
     */
    inline const std::string& getInstanceName() const;

    /**
     * @return the name of the material, can be used a UUID
     * @note the material name is expected to be used to define the ShaderConfiguration name
     */
    inline const std::string& getMaterialName() const;

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
    std::string m_instanceName{};
    bool m_isDirty{true};
    MaterialAspect m_aspect{MaterialAspect::MAT_OPAQUE};

  private:
    /// Unique material name that can be used to identify the material
    const std::string m_materialName;
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Material/Material.inl>
#endif
