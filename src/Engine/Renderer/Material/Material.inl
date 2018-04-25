
namespace Ra {
namespace Engine {

const std::string& Material::getName() const {
    return m_name;
}

void Material::setMaterialAspect(const MaterialAspect &aspect) {
    m_aspect = aspect;
}

const Material::MaterialAspect& Material::getMaterialAspect() const {
    return m_aspect;
}

} // namespace Engine
} // namespace Ra