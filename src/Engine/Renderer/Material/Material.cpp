#include <Engine/Renderer/Material/Material.hpp>

#include <Engine/Renderer/Texture/Texture.hpp>
#include <Engine/Renderer/Shader/ShaderProgram.hpp>

namespace Ra
{

Engine::Material::Material(const std::string& name)
    : m_kd(1.0, 1.0, 1.0, 1.0)
    , m_ks(1.0, 1.0, 1.0, 1.0)
    , m_name(name)
    , m_currentShader(nullptr)
    , m_defaultShader(nullptr)
    , m_contourShader(nullptr)
    , m_wireframeShader(nullptr)

    , m_materialType(MAT_OPAQUE)
{
}

Engine::Material::~Material()
{
}

void Engine::Material::bind()
{
    if (nullptr == m_currentShader)
    {
        return;
    }

    m_currentShader->setUniform("material.kd", m_kd);
    m_currentShader->setUniform("material.ks", m_ks);

    Texture* tex = nullptr;
    uint texUnit = 0;

    tex = m_textures[TEX_DIFFUSE];
    if (tex != nullptr)
    {
        tex->bind(texUnit);
        m_currentShader->setUniform("material.tex.kd", tex, texUnit);
        m_currentShader->setUniform("material.tex.hasKd", 1);
        ++texUnit;
    }
    else
    {
        m_currentShader->setUniform("material.tex.hasKd", 0);
    }

    tex = m_textures[TEX_SPECULAR];
    if (tex != nullptr)
    {
        tex->bind(texUnit);
        m_currentShader->setUniform("material.tex.ks", tex, texUnit);
        m_currentShader->setUniform("material.tex.hasKs", 1);
        ++texUnit;
    }
    else
    {
        m_currentShader->setUniform("material.tex.hasKs", 0);
    }

    tex = m_textures[TEX_NORMAL];
    if (tex != nullptr)
    {
        tex->bind(texUnit);
        m_currentShader->setUniform("material.tex.normal", tex, texUnit);
        m_currentShader->setUniform("material.tex.hasNormal", 1);
        ++texUnit;
    }
    else
    {
        m_currentShader->setUniform("material.tex.hasNormal", 0);
    }

    tex = m_textures[TEX_ALPHA];
    if (tex != nullptr)
    {
        tex->bind(texUnit);
        m_currentShader->setUniform("material.tex.alpha", tex, texUnit);
        m_currentShader->setUniform("material.tex.hasAlpha", 1);
        ++texUnit;
    }
    else
    {
        m_currentShader->setUniform("material.tex.hasAlpha", 0);
    }
}

}
