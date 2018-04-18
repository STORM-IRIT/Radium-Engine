#ifndef RADIUMENGINE_MATERIAL_HPP
#define RADIUMENGINE_MATERIAL_HPP

#include <Engine/RaEngine.hpp>

#include <map>
#include <string>

#include <Core/Math/LinearAlgebra.hpp>

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
 */
class RA_ENGINE_API Material {
  public:
    enum class MaterialType { MAT_OPAQUE, MAT_TRANSPARENT };

  public:
    RA_CORE_ALIGNED_NEW

    explicit Material( const std::string& name, MaterialType type = MaterialType::MAT_OPAQUE );

    virtual ~Material();

    virtual void updateGL() = 0;

    virtual void bind( const ShaderProgram* shader ) = 0;

    inline const std::string& getName() const;

    virtual const std::string getShaderInclude() const;

    inline void setMaterialType( const MaterialType& type );

    inline const MaterialType& getMaterialType() const;

    virtual bool isTransparent() const;

  protected:
    std::string m_name;
    bool m_isDirty;
    MaterialType m_type;
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Material/Material.inl>
#endif
