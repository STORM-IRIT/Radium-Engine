#include <Engine/Renderer/RenderObject/RenderObject.hpp>

#include <Engine/Entity/Component.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Renderer/RenderTechnique/Material.hpp>
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
            params.addParameter("drawFixedSize", fixedSize);
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

        void RenderObject::feedRenderQueue( RenderQueue& queue, const Core::Matrix4& view, const Core::Matrix4& proj )
        {
            if ( !m_visible )
            {
                return;
            }

            Core::Transform model =  m_component->getEntity()->getTransform() * m_localTransform;

            ShaderKey shader( m_renderTechnique->shader );
            BindableMaterial material( m_renderTechnique->material );
            BindableTransform transform( model.matrix(), view, proj );
            BindableMesh mesh( m_mesh.get(), idx );
            mesh.addRenderParameters( m_renderParameters );

            queue[shader][material][transform].push_back( mesh );
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

