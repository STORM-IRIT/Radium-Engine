#include <PostSubdivComponent.hpp>

#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>

namespace PostSubdivPlugin {

PostSubdivComponent::~PostSubdivComponent() {
    for ( auto s : m_loopData )
    {
        std::get<0>( s.second )->detach();
        delete std::get<0>( s.second );
    }
    for ( auto s : m_catmullClarkData )
    {
        std::get<0>( s.second )->detach();
        delete std::get<0>( s.second );
    }
}

void PostSubdivComponent::initialize() {
    auto compMsg = Ra::Engine::ComponentMessenger::getInstance();

    bool hasMesh = compMsg->canGet<TriangleMesh>( getEntity(), m_contentsName );
    if ( hasMesh )
    {
        m_roIdxGetter = compMsg->getterCallback<Ra::Core::Index>( getEntity(), m_contentsName );
        auto RO = getRoMgr()->getRenderObject( *( m_roIdxGetter() ) );
        // create the display RO
        std::shared_ptr<Ra::Engine::RenderTechnique> technique( new Ra::Engine::RenderTechnique );
        *technique = *( RO->getRenderTechnique() );
        std::shared_ptr<Ra::Engine::Mesh> mesh( new Ra::Engine::Mesh( "PostSubdiv_Mesh" ) );
        m_ro = new Ra::Engine::RenderObject( "PostSubdiv_RO", this,
                                             Ra::Engine::RenderObjectType::Geometry );
        m_ro->setVisible( true );
        m_ro->setMesh( mesh );
        m_ro->setRenderTechnique( technique );
        addRenderObject( m_ro );
        // subdiv data will be created at first run
    }
}

template <typename Subdiv>
const TriangleMesh&
getSubdivMesh( const TriangleMesh& mesh, int iter,
               std::map<uint, std::tuple<Subdiv*, TopologicalMesh, TriangleMesh>>& data ) {
    auto it = data.find( iter );
    if ( it == data.end() )
    {
        data[iter] = std::make_tuple( new Subdiv(), TopologicalMesh( mesh ), TriangleMesh() );
        std::get<0>( data[iter] )->attach( std::get<1>( data[iter] ) );
        ( *std::get<0>( data[iter] ) )( iter );
        std::get<2>( data[iter] ) = std::get<1>( data[iter] ).toTriangleMesh();
    } else
    {
        std::get<0>( data[iter] )
            ->recompute( mesh.vertices(), mesh.normals(), std::get<2>( data[iter] ).vertices(),
                         std::get<2>( data[iter] ).normals(), std::get<1>( data[iter] ) );
    }
    return std::get<2>( data[iter] );
}

void PostSubdivComponent::subdiv() {
    auto RO = getRoMgr()->getRenderObject( *( m_roIdxGetter() ) );
    if ( m_iter == 0 )
    {
        RO->setVisible( true );
        m_ro->setVisible( false );
        return;
    }
    RO->setVisible( false );
    m_ro->setVisible( true );
    // get mesh to deform
    auto compMsg = Ra::Engine::ComponentMessenger::getInstance();
    const TriangleMesh& mesh = compMsg->get<TriangleMesh>( getEntity(), m_contentsName );
    // apply subdiv
    switch ( m_subdivMethod )
    {
    case LOOP:
    { m_ro->getMesh()->loadGeometry( getSubdivMesh( mesh, m_iter, m_loopData ) ); }
    break;
    case CATMULLCLARK:
    { m_ro->getMesh()->loadGeometry( getSubdivMesh( mesh, m_iter, m_catmullClarkData ) ); }
    break;
    default:
        break;
    }
}

} // namespace PostSubdivPlugin
