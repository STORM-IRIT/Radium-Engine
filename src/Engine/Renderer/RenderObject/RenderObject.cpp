#include <Engine/Renderer/RenderObject/RenderObject.hpp>

#include <Engine/Entity/Component.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Renderer/RenderTechnique/Material.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/RenderQueue/RenderQueue.hpp>

namespace Ra
{
    namespace Engine
    {
        RenderObject::RenderObject( const std::string& name, const Component* comp, bool drawFixedSize )
            : IndexedObject()
            , m_localTransform( Core::Transform::Identity() )
            , m_component( comp )
            , m_name( name )
            , m_type( Type::RO_OPAQUE )
            , m_renderTechnique( nullptr )
            , m_mesh( nullptr )
            , m_visible( true )
            , m_isDirty( true )
        {
            Engine::RenderParameters params;
            int fixedSize = drawFixedSize ? 1 : 0;
            params.addParameter( "drawFixedSize", fixedSize );
            params.addParameter( "outputValue", 0 );
            addRenderParameters(params);
        }

        RenderObject::~RenderObject()
        {

        }

        void RenderObject::updateGL()
        {
            // Do not update while we are cloning
            std::lock_guard<std::mutex> lock( m_updateMutex );

            if ( m_renderTechnique )
            {
                m_renderTechnique->updateGL();
            }

            if ( m_mesh )
            {
                m_mesh->updateGL();
            }

            m_isDirty = false;
        }

        void RenderObject::feedRenderQueue( RenderQueue& queue )
        {
            ShaderKey shader( m_renderTechnique->shader );
            BindableMaterial material( m_renderTechnique->material );
            BindableMesh mesh( this, idx );

//            queue[shader][material].push_back( mesh );
            queue.push_back( mesh );
        }

        void RenderObject::bind() const
        {
            RenderParameters params;
            bind( m_renderTechnique->shader, params );
        }

        void RenderObject::bind( const RenderParameters& params ) const
        {
            bind( m_renderTechnique->shader, params );
        }

        void RenderObject::bind( ShaderProgram* shader ) const
        {
            RenderParameters params;
            bind( shader, params );
        }

        void RenderObject::bind( ShaderProgram* shader, const RenderParameters& params ) const
        {
            if ( !m_visible )
            {
                return;
            }

            shader->bind();

            // TODO(Charly): This is freaking horrible,
            //               find a better way to bind materials
            auto material = m_renderTechnique->material;

            shader->setUniform( "material.kd", material->getKd() );
            shader->setUniform( "material.ks", material->getKs() );
            shader->setUniform( "material.ns", material->getNs() );

            Texture* tex = nullptr;
            uint texUnit = 0;

            tex = material->getTexture( Material::TextureType::TEX_DIFFUSE );
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

            tex = material->getTexture( Material::TextureType::TEX_SPECULAR );
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

            tex = material->getTexture( Material::TextureType::TEX_NORMAL );
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

            tex = material->getTexture( Material::TextureType::TEX_SHININESS );
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

            tex = material->getTexture( Material::TextureType::TEX_ALPHA );
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

            Core::Matrix4 mat = ( m_component->getEntity()->getTransform() * m_localTransform ).matrix();
            Core::Matrix4 inv = mat.inverse().transpose();

            m_renderParameters.bind( shader );
            params.bind( shader );

            shader->setUniform( "transform.model", mat );
            shader->setUniform( "transform.worldNormal", inv );
        }

        void RenderObject::render() const
        {
            if ( !m_visible )
            {
                return;
            }

            m_mesh->render();
        }

        RenderObject* RenderObject::clone( bool cloneMesh )
        {
            // Do not clone while we are updating GL internals
            std::lock_guard<std::mutex> lock( m_updateMutex );

            RenderObject* newRO = new RenderObject( m_name, m_component );

            newRO->setType( m_type );
            newRO->setRenderTechnique( m_renderTechnique );
            newRO->setVisible( m_visible );
            newRO->addRenderParameters( m_renderParameters );

            newRO->idx = idx;

            if ( m_mesh )
            {
                newRO->setMesh( cloneMesh ? m_mesh->clone() : m_mesh );
            }

            return newRO;
        }
    } // namespace Engine
} // namespace Ra

