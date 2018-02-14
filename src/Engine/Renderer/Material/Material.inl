#include <Engine/Renderer/Material/Material.hpp>
namespace Ra {
  namespace Engine {

    const std::string &Material::getName() const
    {
        return m_name;
    }

    void Material::setMaterialType(const MaterialType &type)
    {
        m_type = type;
    }

    const Material::MaterialType &Material::getMaterialType() const
    {
        return m_type;
    }

  }
}
