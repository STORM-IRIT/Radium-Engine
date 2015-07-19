#include "Material.hpp"

namespace Ra
{

inline const std::string& Engine::Material::getName() const
{
    return m_name;
}

inline void Engine::Material::setDefaultShaderProgram(ShaderProgram* shader)
{
    m_defaultShader = shader;

    if (nullptr == m_currentShader)
    {
        m_currentShader = shader;
    }
}

inline void Engine::Material::setContourShaderProgram(ShaderProgram* shader)
{
    m_contourShader = shader;

    if (nullptr == m_currentShader)
    {
        m_currentShader = shader;
    }
}

inline void Engine::Material::setWireframeShaderProgram(ShaderProgram* shader)
{
    m_wireframeShader = shader;

    if (nullptr == m_currentShader)
    {
        m_currentShader = shader;
    }
}

inline Engine::ShaderProgram* Engine::Material::getCurrentShaderProgram() const
{
    return m_currentShader;
}

inline void Engine::Material::changeMode(const Material::MaterialMode& mode)
{
    switch (mode)
    {
        case MODE_DEFAULT:
        {
            m_currentShader = m_defaultShader;
        } break;

        case MODE_CONTOUR:
        {
            m_currentShader = m_contourShader;
        } break;

        case MODE_WIREFRAME:
        {
            m_currentShader = m_wireframeShader;
        } break;

        default:
        {
        } break;
    }
}

inline void Engine::Material::setKd(const Core::Color& kd)
{
    m_kd = kd;
}

inline void Engine::Material::setKs(const Core::Color& ks)
{
    m_ks = ks;
}

inline void Engine::Material::setMaterialType(const MaterialType& type)
{
    m_materialType = type;
}

inline void Engine::Material::addTexture(const TextureType& type, Texture* texture)
{
    // FIXME(Charly): Check if already present ?
    m_textures[type] = texture;
}

inline const Core::Color& Engine::Material::getKd() const
{
    return m_kd;
}

inline const Core::Color& Engine::Material::getKs() const
{
    return m_ks;
}

inline const Engine::Material::MaterialType& Engine::Material::getMaterialType() const
{
    return m_materialType;
}

inline Engine::Texture* Engine::Material::getTexture(const TextureType &type) const
{
    Texture* tex = nullptr;

    auto it = m_textures.find(type);
    if (it != m_textures.end())
    {
        tex = it->second;
    }

    return tex;
}

} // namespace Ra
