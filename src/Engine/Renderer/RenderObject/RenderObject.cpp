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
        RenderObject::RenderObject( const std::string& name, const Component* comp,
                                    const RenderObjectType& type )
            : IndexedObject()
            , m_localTransform( Core::Transform::Identity() )
            , m_component( comp )
            , m_name( name )
            , m_type( type )
            , m_renderTechnique( nullptr )
            , m_mesh( nullptr )
            , m_visible( true )
            , m_isDirty( true )
        {
            // FIXME(Charly): Render parameters should get out of here
            /*
            Engine::RenderParameters params;
            int fixedSize = drawFixedSize ? 1 : 0;
            params.addParameter( "drawFixedSize", fixedSize );
            params.addParameter( "outputValue", 0 );
            addRenderParameters(params);
            */
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
//            ShaderKey shader( m_renderTechnique->shader );
//            BindableMaterial material( m_renderTechnique->material );
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

            RenderObject* newRO = new RenderObject( m_name, m_component, m_type );

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

