
namespace Ra {
namespace Engine {

inline void Material::setMaterialName( std::string newName ) {
    m_materialName = std::move( newName );
}

const std::string& Material::getInstanceName() const {
    return m_instanceName;
}

const std::string& Material::getMaterialName() const {
    return m_materialName;
}

void Material::setMaterialAspect( const MaterialAspect& aspect ) {
    m_aspect = aspect;
}

const Material::MaterialAspect& Material::getMaterialAspect() const {
    return m_aspect;
}

void Material::needUpdate() {
    m_isDirty = true;
}
} // namespace Engine
} // namespace Ra
