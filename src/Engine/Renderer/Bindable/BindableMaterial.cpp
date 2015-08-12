#include <Engine/Renderer/Bindable/BindableMaterial.hpp>

#include <Engine/Renderer/RenderTechnique/Material.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>

namespace Ra
{

    Engine::BindableMaterial::BindableMaterial( Material* material )
        : Bindable()
        , m_material( material )
    {
        CORE_ASSERT( material, "Null material passed to bindable material." );
    }

    Engine::BindableMaterial::~BindableMaterial()
    {
    }

    void Engine::BindableMaterial::bind( ShaderProgram* shader ) const
    {
        shader->setUniform( "material.kd", m_material->getKd() );
        shader->setUniform( "material.ks", m_material->getKs() );
        shader->setUniform( "material.ns", m_material->getNs() );

        Texture* tex = nullptr;
        uint texUnit = 0;

        tex = m_material->getTexture( Material::TextureType::TEX_DIFFUSE );
        if ( tex != nullptr )
        {
            tex->bind( texUnit );
            shader->setUniform( "material.tex.kd", tex, texUnit );
            shader->setUniform( "material.tex.hasKd", 1 );
            ++texUnit;
        }
        else
        {
            shader->setUniform( "material.tex.hasKd", 0 );
        }

        tex = m_material->getTexture( Material::TextureType::TEX_SPECULAR );
        if ( tex != nullptr )
        {
            tex->bind( texUnit );
            shader->setUniform( "material.tex.ks", tex, texUnit );
            shader->setUniform( "material.tex.hasKs", 1 );
            ++texUnit;
        }
        else
        {
            shader->setUniform( "material.tex.hasKs", 0 );
        }

        tex = m_material->getTexture( Material::TextureType::TEX_NORMAL );
        if ( tex != nullptr )
        {
            tex->bind( texUnit );
            shader->setUniform( "material.tex.normal", tex, texUnit );
            shader->setUniform( "material.tex.hasNormal", 1 );
            ++texUnit;
        }
        else
        {
            shader->setUniform( "material.tex.hasNormal", 0 );
        }

        tex = m_material->getTexture( Material::TextureType::TEX_SHININESS );
        if ( tex != nullptr )
        {
            tex->bind( texUnit );
            shader->setUniform( "material.tex.ns", tex, texUnit );
            shader->setUniform( "material.tex.hasNs", 1 );
            ++texUnit;
        }
        else
        {
            shader->setUniform( "material.tex.hasNs", 0 );
        }

        tex = m_material->getTexture( Material::TextureType::TEX_ALPHA );
        if ( tex != nullptr )
        {
            tex->bind( texUnit );
            shader->setUniform( "material.tex.alpha", tex, texUnit );
            shader->setUniform( "material.tex.hasAlpha", 1 );
            ++texUnit;
        }
        else
        {
            shader->setUniform( "material.tex.hasAlpha", 0 );
        }
    }

    bool Engine::BindableMaterial::operator< ( const BindableMaterial& other ) const
    {
        return m_material->getName() < other.m_material->getName();
    }

} // namespace Ra
