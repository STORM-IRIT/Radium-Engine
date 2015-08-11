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

    Engine::RenderObject::RenderObject( const std::string& name )
        : IndexedObject()
        , m_name( name )
        , m_type( RenderObjectType::RO_OPAQUE )
        , m_renderTechnique( nullptr )
        , m_mesh( nullptr )
        , m_isDirty( true )
    {
    }

    Engine::RenderObject::~RenderObject()
    {
    }

    void Engine::RenderObject::updateGL()
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

    void Engine::RenderObject::feedRenderQueue( RenderQueue& queue, const Core::Matrix4& view, const Core::Matrix4& proj )
    {
        ShaderKey shader( m_renderTechnique->shader );
        BindableMaterial material( m_renderTechnique->material );
        BindableTransform transform( m_component->getEntity()->getTransformAsMatrix(), view, proj );
        BindableMesh mesh( m_mesh, m_component->getEntity()->idx.getValue() );

        queue[shader][material][transform].push_back( mesh );
    }

    Engine::RenderObject* Engine::RenderObject::clone()
    {
        // Do not clone while we are updating GL internals
        std::lock_guard<std::mutex> lock( m_updateMutex );

        RenderObject* newRO = new RenderObject( m_name );

        newRO->setRenderObjectType( m_type );
        newRO->setRenderTechnique( m_renderTechnique );
        newRO->setVisible( m_visible );
        newRO->setComponent( m_component );

        if ( m_mesh )
        {
            Mesh* newMesh = new Mesh( m_mesh->getName() );
            newMesh->loadGeometry( m_mesh->getData( Mesh::VERTEX_POSITION ), m_mesh->getIndices() );
            newMesh->addData( Mesh::VERTEX_NORMAL, m_mesh->getData( Mesh::VERTEX_NORMAL ) );
            newMesh->addData( Mesh::VERTEX_TANGENT, m_mesh->getData( Mesh::VERTEX_TANGENT ) );
            newMesh->addData( Mesh::VERTEX_BITANGENT, m_mesh->getData( Mesh::VERTEX_BITANGENT ) );
            newMesh->addData( Mesh::VERTEX_TEXCOORD, m_mesh->getData( Mesh::VERTEX_TEXCOORD ) );
            newRO->setMesh( newMesh );
        }

        return newRO;
    }

}
