#include <Engine/Renderer/Material/Material.hpp>

#include <Engine/Renderer/Texture/Texture.hpp>
#include <Engine/Renderer/Shader/ShaderProgram.hpp>

namespace Ra
{

Engine::Material::Material(const std::string& name)
    : m_name(name)
    , m_shader(nullptr)
    , m_kd(1.0, 1.0, 1.0, 1.0)
    , m_ks(1.0, 1.0, 1.0, 1.0)
    , m_materialType(MAT_OPAQUE)
{
}

Engine::Material::~Material()
{
}

void Engine::Material::bind()
{
    if (nullptr == m_shader)
    {
        return;
    }

    m_shader->setUniform("material.kd", m_kd);
    m_shader->setUniform("material.ks", m_ks);

    Texture* tex = nullptr;
    uint texUnit = 0;

    tex = m_textures[TEX_DIFFUSE];
    if (tex != nullptr)
    {
        tex->bind(texUnit);
        m_shader->setUniform("material.tex.kd", tex, texUnit);
        m_shader->setUniform("material.tex.hasKd", 1);
        ++texUnit;
    }
    else
    {
        m_shader->setUniform("material.tex.hasKd", 0);
    }

    tex = m_textures[TEX_SPECULAR];
    if (tex != nullptr)
    {
        tex->bind(texUnit);
        m_shader->setUniform("material.tex.ks", tex, texUnit);
        m_shader->setUniform("material.tex.hasKs", 1);
        ++texUnit;
    }
    else
    {
        m_shader->setUniform("material.tex.hasKs", 0);
    }

    tex = m_textures[TEX_NORMAL];
    if (tex != nullptr)
    {
        tex->bind(texUnit);
        m_shader->setUniform("material.tex.normal", tex, texUnit);
        m_shader->setUniform("material.tex.hasNormal", 1);
        ++texUnit;
    }
    else
    {
        m_shader->setUniform("material.tex.hasNormal", 0);
    }

    tex = m_textures[TEX_ALPHA];
    if (tex != nullptr)
    {
        tex->bind(texUnit);
        m_shader->setUniform("material.tex.alpha", tex, texUnit);
        m_shader->setUniform("material.tex.hasAlpha", 1);
        ++texUnit;
    }
    else
    {
        m_shader->setUniform("material.tex.hasAlpha", 0);
    }
}

}
