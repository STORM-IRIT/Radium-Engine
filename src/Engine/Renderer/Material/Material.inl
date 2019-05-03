
namespace Ra {
namespace Engine {

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

} // namespace Engine
} // namespace Ra
