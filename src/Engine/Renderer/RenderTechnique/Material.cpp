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
            : m_kd( 0.9, 0.9, 0.9, 1.0 )
            , m_ks( 0.0, 0.0, 0.0, 1.0 )
            , m_ns( 1.0 )
            , m_alpha(1.0)
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
            shader->setUniform( "material.kd", m_kd );
            shader->setUniform( "material.ks", m_ks );
            shader->setUniform( "material.ns", m_ns );
            shader->setUniform( "material.alpha", m_alpha );

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

        const std::string& Material::getName() const
        {
            return m_name;
        }

        void Material::addTexture( const TextureType& type, Texture* texture )
        {
            // FIXME(Charly): Check if already present ?
            m_textures[type] = texture;
        }

        TextureData& Material::addTexture( const TextureType& type, const std::string& texture )
        {
            CORE_ASSERT(!texture.empty(), "Invalid texture name");

            TextureData data;
            data.name = texture;
            data.wrapS = GL_REPEAT;
            data.wrapT = GL_REPEAT;
            data.magFilter = GL_NEAREST;
            data.minFilter = GL_NEAREST;

            return addTexture(type, data);
        }

        TextureData& Material::addTexture(const TextureType& type, const TextureData& texture)
        {
            m_pendingTextures[type] = texture;
            m_isDirty = true;

            return m_pendingTextures[type];
        }

        Texture* Material::getTexture( const TextureType& type ) const
        {
            Texture* tex = nullptr;

            auto it = m_textures.find( type );
            if ( it != m_textures.end() )
            {
                tex = it->second;
            }

            return tex;
        }

        void Material::setMaterialType( const MaterialType& type )
        {
            m_type = type;
        }

        const Material::MaterialType& Material::getMaterialType() const
        {
            return m_type;
        }
    }
}
