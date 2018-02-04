#include <MeshPaintComponent.hpp>

#include <iostream>

#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/Mesh/MeshUtils.hpp>
#include <Core/Tasks/TaskQueue.hpp>

#include <Engine/Entity/Entity.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>
#include <Engine/Renderer/RenderTechnique/Material.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>

namespace MeshPaintPlugin {

MeshPaintComponent::MeshPaintComponent( const std::string& name ) :
    Ra::Engine::Component( name )
{}

MeshPaintComponent::~MeshPaintComponent() { }

void MeshPaintComponent::addTasks( Ra::Core::TaskQueue* taskQueue, const Ra::Engine::FrameInfo& info )
{
}

void MeshPaintComponent::setDataId( const std::string &id )
{
    m_dataId = id;
}

void MeshPaintComponent::initialize()
{
    auto compMess = Ra::Engine::ComponentMessenger::getInstance();
    //Look for the data we need
    bool geometryData = compMess->canGet< Ra::Core::TriangleMesh >( getEntity(), m_dataId );

    if( !geometryData )
    {
        LOG(logWARNING) << "Cannot initialize MeshPaint Component: no geometry data." << std::endl;
        return;
    }

    m_renderObjectReader = compMess->getterCallback< Ra::Core::Index >( getEntity(), m_dataId );

    auto ro = getRoMgr()->getRenderObject( *m_renderObjectReader() );
    m_config = ro->getRenderTechnique()->shaderConfig;
    ro->getMesh()->addData( Ra::Engine::Mesh::VERTEX_COLOR,
                            Ra::Core::Vector4Array( ro->getMesh()->getGeometry().m_vertices.size(),
                                                    Ra::Core::Colors::Skin()) );
}

void MeshPaintComponent::startPaint( bool on )
{
    auto ro = getRoMgr()->getRenderObject( *m_renderObjectReader() );
    if (on)
    {
        ro->getRenderTechnique()->changeShader( Ra::Engine::ShaderConfigurationFactory::getConfiguration( "Plain" ) );
    }
    else
    {
        ro->getRenderTechnique()->changeShader( m_config );
    }
}

void MeshPaintComponent::paintMesh( const Ra::Engine::Renderer::PickingResult& picking, const Ra::Core::Color &color )
{
    // check it's for us
    if ( *m_renderObjectReader() != picking.m_roIdx || picking.m_mode == Ra::Engine::Renderer::RO )
    {
        return;
    }

    auto ro = getRoMgr()->getRenderObject( *m_renderObjectReader() );
    if ( ro->getMesh()->getRenderMode() == Ra::Engine::Mesh::RM_POINTS &&
         (picking.m_mode != Ra::Engine::Renderer::VERTEX &&
          picking.m_mode != Ra::Engine::Renderer::C_VERTEX) )
    {
        return;
    }

    if ( (ro->getMesh()->getRenderMode() == Ra::Engine::Mesh::RM_LINES ||
          ro->getMesh()->getRenderMode() == Ra::Engine::Mesh::RM_LINES_ADJACENCY ||
          ro->getMesh()->getRenderMode() == Ra::Engine::Mesh::RM_LINE_STRIP_ADJACENCY ) )
    {
        // line meshes not supported yet -- because picking not tested
        return;
    }

    const auto& T = ro->getMesh()->getGeometry().m_triangles;
    auto colors = ro->getMesh()->getData( Ra::Engine::Mesh::VERTEX_COLOR );

    switch (picking.m_mode)
    {
    case Ra::Engine::Renderer::VERTEX:
    case Ra::Engine::Renderer::C_VERTEX:
    {
        if ( ro->getMesh()->getRenderMode() == Ra::Engine::Mesh::RM_POINTS )
        {
            for (int t : picking.m_elementIdx)
            {
                colors[t] = color;
            }
        }
        else
        {
            for (int t = 0; t < picking.m_elementIdx.size(); ++t)
            {
                int v = T[ picking.m_elementIdx[t] ]( picking.m_vertexIdx[t] );
                colors[v] = color;
            }
        }
        break;
    }
    case Ra::Engine::Renderer::EDGE:
    case Ra::Engine::Renderer::C_EDGE:
    {
        for (int t = 0; t < picking.m_elementIdx.size(); ++t)
        {
            int v1 = T[ picking.m_elementIdx[t] ]( (picking.m_edgeIdx[t]+1)%3 );
            int v2 = T[ picking.m_elementIdx[t] ]( (picking.m_edgeIdx[t]+2)%3 );
            colors[v1] = colors[v2] = color;
        }
        break;
    }
    case Ra::Engine::Renderer::TRIANGLE:
    case Ra::Engine::Renderer::C_TRIANGLE:
    {
        for (int t = 0; t < picking.m_elementIdx.size(); ++t)
        {
            int v1 = T[ picking.m_elementIdx[t] ]( 0 );
            int v2 = T[ picking.m_elementIdx[t] ]( 1 );
            int v3 = T[ picking.m_elementIdx[t] ]( 2 );
            colors[v1] = colors[v2] = colors[v3] = color;
        }
        break;
    }
    default: break;
    }

    ro->getMesh()->addData( Ra::Engine::Mesh::VERTEX_COLOR, colors );
}

} // namespace MeshPaintPlugin

