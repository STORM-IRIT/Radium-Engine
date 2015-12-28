#include <Engine/Renderer/RenderTechnique/Material.hpp>

#include <Engine/Renderer/Texture/Texture.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>
#include <Engine/Renderer/Texture/TextureManager.hpp>

namespace Ra
{
    namespace Engine
    {

        Material::Material( const std::string& name )
            : m_kd( 1.0, 1.0, 1.0, 1.0 )
            , m_ks( 1.0, 1.0, 1.0, 1.0 )
            , m_ns( 1.0 )
            , m_name( name )
            , m_isDirty( true )
            , m_type( MaterialType::MAT_OPAQUE )
        {
        }

        Material::~Material()
        {
            m_textures.clear();
        }

        void Material::updateGL()
        {
            if ( !m_isDirty )
            {
                return;
            }

            // Load textures
            TextureManager* texManager = TextureManager::getInstance();
            for ( const auto& tex : m_pendingTextures )
            {
                addTexture( tex.first, texManager->getOrLoadTexture( tex.second ) );
            }

            texManager->updateTextures();

            m_pendingTextures.clear();
            m_isDirty = false;
        }

        void Material::bind( const ShaderProgram* shader )
        {
            shader->setUniform( "material.kd", getKd() );
            shader->setUniform( "material.ks", getKs() );
            shader->setUniform( "material.ns", getNs() );

            Texture* tex = nullptr;
            uint texUnit = 0;

            tex = getTexture( Material::TextureType::TEX_DIFFUSE );
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

            tex = getTexture( Material::TextureType::TEX_SPECULAR );
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

            tex = getTexture( Material::TextureType::TEX_NORMAL );
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

            tex = getTexture( Material::TextureType::TEX_SHININESS );
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

            tex = getTexture( Material::TextureType::TEX_ALPHA );
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
    }
}
