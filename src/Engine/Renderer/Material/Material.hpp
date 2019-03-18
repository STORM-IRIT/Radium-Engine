#ifndef RADIUMENGINE_MATERIAL_HPP
#define RADIUMENGINE_MATERIAL_HPP

#include <Engine/RaEngine.hpp>

#include <map>
#include <string>

// This include brings only the macro EIGEN_MAKE_ALIGNED_OPERATOR_NEW in the file scope.
// Need to be separated to reduce compilation time.
#include <Core/Math/Types.hpp>

namespace Ra {
namespace Engine {
class ShaderProgram;
} // namespace Engine
} // namespace Ra

namespace Ra {
namespace Engine {

/**
 * Base class for Materials.
 * Do not assumes a given Material representation but only make the difference
 * between opaque and transparent Materials.
 */
class RA_ENGINE_API Material {
  public:
    /**
     * Transparency aspect of a Material.
     */
    enum class MaterialAspect { MAT_OPAQUE, MAT_TRANSPARENT };

  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    /**
     * Creates a named Material with the given aspect.
     */
    explicit Material( const std::string& name,
                       MaterialAspect aspect = MaterialAspect::MAT_OPAQUE );

    virtual ~Material() = default;

    /**
     * Update the OpenGL states used by the Material.
     * These state could be Textures, precomputed tables or whatever data
     * associated to the Material and given to OpenGL as a buffer object.
     */
    virtual void updateGL() = 0;

    /**
     * Bind the Material to the given shader.
     * This method must set the uniforms and textures of the shader to
     * reflect the state of the Material.
     */
    virtual void bind( const ShaderProgram* shader ) = 0;

    /**
     * Get the basename of the glsl source file to include if one wants to
     * build composite shaders that use this Material.
     * \return The basename (without extension like .frag.glsl or .vert.glsl)
     *         of the glsl source file.
     * \note Not used yet by the Engine.
     */
    virtual const std::string getShaderInclude() const;

    /**
     * Return the name of the Material.
     */
    inline const std::string& getName() const;

    /**
     * Set the aspect (MAT_OPAQUE or MAT_TRANSPARENT) of the Material.
     */
    inline void setMaterialAspect( const MaterialAspect& aspect );

    /**
     * Return the aspect (MAT_OPAQUE or MAT_TRANSPARENT) of the Material.
     */
    inline const MaterialAspect& getMaterialAspect() const;

    /**
     * Return true if the Material is transparent, false otherwise.
     */
    virtual bool isTransparent() const;

  protected:
    /// The Material's name.
    std::string m_name{};

    /// Whether the Material data should be updated to the GPU.
    bool m_isDirty{true};

    /// The aspect of the Material.
    MaterialAspect m_aspect{MaterialAspect::MAT_OPAQUE};
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Material/Material.inl>
#endif
