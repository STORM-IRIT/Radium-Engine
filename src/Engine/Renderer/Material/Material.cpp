#include <Engine/Renderer/Material/Material.hpp>


namespace Ra {
  namespace Engine {

    Material::Material(const std::string &name, MaterialType type)
        : m_name(name), m_isDirty(true), m_type(MaterialType::MAT_OPAQUE)
    {
    }

    Material::~Material()
    {
    }

    bool Material::isTransparent() const
    {
        return m_type == MaterialType::MAT_TRANSPARENT;
    }

    const std::string Material::getShaderInclude() const
    {
        return std::string("");
    }

  }
}

