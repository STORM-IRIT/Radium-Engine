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

    }
}
