#include <Engine/Renderer/Material/Material.hpp>

#include <Engine/Renderer/Texture/Texture.hpp>
#include <Engine/Renderer/Shader/ShaderProgram.hpp>
#include <Engine/Renderer/Shader/ShaderProgramManager.hpp>
#include <Engine/Renderer/Texture/TextureManager.hpp>

namespace Ra
{

Engine::Material::Material(const std::string& name)
    : m_kd(1.0, 1.0, 1.0, 1.0)
    , m_ks(1.0, 1.0, 1.0, 1.0)
    , m_name(name)
    , m_isDirty(true)
    , m_mode(MODE_DEFAULT)
    , m_currentShader(nullptr)
    , m_defaultShader(nullptr)
    , m_contourShader(nullptr)
    , m_wireframeShader(nullptr)
    , m_materialType(MAT_OPAQUE)
{
    ShaderProgramManager& manager  = ShaderProgramManager::getInstanceRef();
    m_defaultShaderConfiguration   = manager.getDefaultShaderProgram()->getBasicConfiguration();
    m_contourShaderConfiguration   = manager.getDefaultShaderProgram()->getBasicConfiguration();
    m_wireframeShaderConfiguration = manager.getDefaultShaderProgram()->getBasicConfiguration();
}

Engine::Material::~Material()
{
	m_textures.clear();
}

void Engine::Material::updateGL()
{
    if (!m_isDirty)
    {
        return;
    }

    // FIXME(Charly): Get shader manager through engine ?
    ShaderProgramManager& manager = ShaderProgramManager::getInstanceRef();

    m_defaultShader = manager.getShaderProgram(m_defaultShaderConfiguration);
    m_contourShader = manager.getShaderProgram(m_contourShaderConfiguration);
    m_wireframeShader = manager.getShaderProgram(m_wireframeShaderConfiguration);

    switch (m_mode)
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
    }

    // Load textures
    TextureManager& texManager = TextureManager::getInstanceRef();
    for (const auto& tex : m_pendingTextures)
    {
        addTexture(tex.first, texManager.getOrLoadTexture(tex.second));
    }
    m_pendingTextures.clear();

    m_isDirty = false;
}

void Engine::Material::bind()
{
    bind(m_currentShader);
}

void Engine::Material::bind(ShaderProgram *shader)
{
    if (nullptr == shader)
    {
        return;
    }

    shader->setUniform("material.kd", m_kd);
    shader->setUniform("material.ks", m_ks);

    Texture* tex = nullptr;
    uint texUnit = 0;

    tex = m_textures[TEX_DIFFUSE];
    if (tex != nullptr)
    {
        tex->bind(texUnit);
        shader->setUniform("material.tex.kd", tex, texUnit);
        shader->setUniform("material.tex.hasKd", 1);
        ++texUnit;
    }
    else
    {
        shader->setUniform("material.tex.hasKd", 0);
    }

    tex = m_textures[TEX_SPECULAR];
    if (tex != nullptr)
    {
        tex->bind(texUnit);
        shader->setUniform("material.tex.ks", tex, texUnit);
        shader->setUniform("material.tex.hasKs", 1);
        ++texUnit;
    }
    else
    {
        shader->setUniform("material.tex.hasKs", 0);
    }

    tex = m_textures[TEX_NORMAL];
    if (tex != nullptr)
    {
        tex->bind(texUnit);
        shader->setUniform("material.tex.normal", tex, texUnit);
        shader->setUniform("material.tex.hasNormal", 1);
        ++texUnit;
    }
    else
    {
        shader->setUniform("material.tex.hasNormal", 0);
    }

    tex = m_textures[TEX_ALPHA];
    if (tex != nullptr)
    {
        tex->bind(texUnit);
        shader->setUniform("material.tex.alpha", tex, texUnit);
        shader->setUniform("material.tex.hasAlpha", 1);
        ++texUnit;
    }
    else
    {
        shader->setUniform("material.tex.hasAlpha", 0);
    }
}

}
