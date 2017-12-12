#include <Engine/Renderer/Material/BlinnPhongMaterial.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>

namespace Ra {
  namespace Engine {

    BlinnPhongMaterial::BlinnPhongMaterial(const std::string &name) : Material(name, Material::MaterialType::MAT_OPAQUE)
        , m_kd(0.9, 0.9, 0.9, 1.0), m_ks(0.0, 0.0, 0.0, 1.0), m_ns(1.0), m_alpha(1.0)
    {
    }

    BlinnPhongMaterial::~BlinnPhongMaterial()
    {
        m_textures.clear();
    }

    void BlinnPhongMaterial::updateGL()
    {
        if (!m_isDirty)
        {
            return;
        }

        // Load textures
        TextureManager *texManager = TextureManager::getInstance();
        for (const auto &tex : m_pendingTextures)
        {
            addTexture(tex.first, texManager->getOrLoadTexture(tex.second));
        }

        texManager->updateTextures();

        m_pendingTextures.clear();
        m_isDirty = false;
    }

    void BlinnPhongMaterial::bind(const ShaderProgram *shader)
    {
        shader->setUniform("material.kd", m_kd);
        shader->setUniform("material.ks", m_ks);
        shader->setUniform("material.ns", m_ns);
        shader->setUniform("material.alpha", m_alpha);

        Texture *tex = nullptr;
        uint texUnit = 0;

        tex = getTexture(BlinnPhongMaterial::TextureType::TEX_DIFFUSE);
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

        tex = getTexture(BlinnPhongMaterial::TextureType::TEX_SPECULAR);
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

        tex = getTexture(BlinnPhongMaterial::TextureType::TEX_NORMAL);
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

        tex = getTexture(BlinnPhongMaterial::TextureType::TEX_SHININESS);
        if (tex != nullptr)
        {
            tex->bind(texUnit);
            shader->setUniform("material.tex.ns", tex, texUnit);
            shader->setUniform("material.tex.hasNs", 1);
            ++texUnit;
        }
        else
        {
            shader->setUniform("material.tex.hasNs", 0);
        }

        tex = getTexture(BlinnPhongMaterial::TextureType::TEX_ALPHA);
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

    bool BlinnPhongMaterial::isTransparent() const
    {
        return ( m_alpha < 1.0 ) || Material::isTransparent();
    }
  }
}
