#include <MeshFeatureTrackingComponent.hpp>

#include <Core/Geometry/MeshPrimitives.hpp>
#include <Core/Geometry/TriangleMesh.hpp>
#include <Core/Utils/Log.hpp>

#include <Engine/Entity/Entity.hpp>
#include <Engine/Managers/ComponentMessenger/ComponentMessenger.hpp>
#include <Engine/Managers/SystemDisplay/SystemDisplay.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Material/BlinnPhongMaterial.hpp>
#include <Engine/Renderer/Material/Material.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>

constexpr Scalar POINT_SCALE_FACTOR = 1_ra / 500;

using Ra::Engine::ComponentMessenger;
using MeshRenderMode = Ra::Engine::Mesh::MeshRenderMode;
using PickingMode    = Ra::Engine::Renderer::PickingMode;
using Ra::Core::Utils::TLogLevel;

namespace MeshFeatureTrackingPlugin {
MeshFeatureTrackingComponent::MeshFeatureTrackingComponent( const std::string& name ) :
    Component( name, Ra::Engine::SystemEntity::getInstance() ),
    m_pickedMesh( nullptr ) {
    m_data.m_mode = Ra::Engine::Renderer::PickingMode::RO;
}

MeshFeatureTrackingComponent::~MeshFeatureTrackingComponent() {}

void MeshFeatureTrackingComponent::initialize() {
    std::shared_ptr<Ra::Engine::Mesh> display( new Ra::Engine::Mesh( "PickingManagerSphere" ) );
    display->loadGeometry( Ra::Core::Geometry::makeParametricSphere( 1_ra ) );
    std::shared_ptr<Ra::Engine::Material> material;
    auto bpMaterial = new Ra::Engine::BlinnPhongMaterial( "PickingManageSphereMaterial" );
    material.reset( bpMaterial );
    bpMaterial->m_kd = Ra::Core::Utils::Color::Green();
    m_RO             = Ra::Engine::RenderObject::createRenderObject(
        "FeaturePickingManagerSphereRO",
        this,
        Ra::Engine::RenderObjectType::Geometry,
        display,
        Ra::Engine::RenderTechnique::createDefaultRenderTechnique(),
        material );
    m_RO->setPickable( false );
    m_RO->setVisible( false );
    addRenderObject( m_RO );
}

void MeshFeatureTrackingComponent::setPosition( Ra::Core::Vector3 position ) {
    Ra::Core::Translation aa( position );
    Ra::Core::Transform rot( aa );
    m_RO->setLocalTransform( rot );
}

void MeshFeatureTrackingComponent::setScale( Scalar scale ) {
    auto T = m_RO->getLocalTransform();
    m_RO->setLocalTransform( T.scale( scale ) );
}

int MeshFeatureTrackingComponent::getMaxV() const {
    if ( m_data.m_mode != PickingMode::RO && getRoMgr()->exists( m_pickedRoIdx ) )
    { return int( m_pickedMesh->getNumVertices() ); }
    return 0;
}

int MeshFeatureTrackingComponent::getMaxT() const {
    if ( m_data.m_mode != PickingMode::RO && getRoMgr()->exists( m_pickedRoIdx ) )
    { return int( m_pickedMesh->getNumFaces() ); }
    return 0;
}

namespace { // anonymous namespace for line mesh indices retrieval from triangles -- according to
            // the render mode
// returns the triangles where the vertices can be found, along with the corresponding vertex
// indices within them.
void getPos_L2T( int l, int& v0, int& v1, int& t0, int& t1 ) {
    v0 = 0;
    v1 = 1;
    t0 = l / 3 * 2;
    t1 = t0;
    if ( l % 3 == 1 )
    {
        v0 = 2;
        v1 = 0;
        ++t1;
    }
    else if ( l % 3 == 2 )
    {
        v0 = 1;
        v1 = 2;
        ++t0;
        ++t1;
    }
}
void getPos_L2T_strip( int l, int& v0, int& v1, int& t0, int& t1 ) {
    v0 = 0;
    v1 = 1;
    t0 = l / 3;
    t1 = t0;
    if ( l % 3 == 1 )
    {
        ++v0;
        ++v1;
    }
    else if ( l % 3 == 2 )
    {
        v0 = 2;
        v1 = 0;
        ++t1;
    }
}
void getPos_L2T_adjacency( int l, int& v0, int& v1, int& t0, int& t1 ) {
    v0 = 1;
    v1 = 2;
    t0 = ( l * 4 ) / 3;
    t1 = t0;
    if ( l % 3 == 1 )
    {
        v0 = 2;
        v1 = 0;
        ++t1;
    }
    else if ( l % 3 == 2 )
    {
        v0 = 0;
        v1 = 1;
        ++t0;
        ++t1;
    }
}
void getPos_L2T_strip_adjacency( int l, int& v0, int& v1, int& t0, int& t1 ) {
    v0 = 0;
    v1 = 1;
    t0 = ( l + 1 ) / 3;
    t1 = t0;
    if ( l % 3 == 0 )
    {
        ++v0;
        ++v1;
    }
    else if ( l % 3 == 1 )
    {
        v0 = 2;
        v1 = 0;
        ++t1;
    }
}

void getPos_TS2T( int ts, int& v0, int& v1, int& v2, int& t0, int& t1, int& t2 ) {
    v0 = 0;
    v1 = 1;
    v2 = 2;
    t0 = ts / 3;
    t1 = t0;
    t2 = t0;
    if ( ts % 3 == 1 )
    {
        v0 = 1;
        v1 = 2;
        v2 = 0;
        ++t2;
    }
    else if ( ts % 3 == 2 )
    {
        v0 = 2;
        v1 = 0;
        v2 = 1;
        ++t1;
        ++t2;
    }
}

// for fans, first triangle vertex is always triangle[0](0)
void getPos_TF2T( int tf, int& v1, int& v2, int& t1, int& t2 ) {
    v1 = 0;
    v2 = 1;
    t1 = ( tf + 1 ) / 3;
    t2 = t1;
    if ( tf % 3 == 0 )
    {
        v1 = 1;
        v2 = 2;
    }
    else if ( tf % 3 == 1 )
    {
        v1 = 2;
        v2 = 0;
        ++t2;
    }
}
} // namespace

void MeshFeatureTrackingComponent::setData( const Ra::Engine::Renderer::PickingResult& data ) {
    using Ra::Engine::Displayable;

    if ( !getRoMgr()->exists( data.m_roIdx ) )
    {
        m_data.m_mode = PickingMode::RO;
        return;
    }

    {
        auto ro                = getRoMgr()->getRenderObject( data.m_roIdx );
        Ra::Engine::Mesh* mesh = dynamic_cast<Ra::Engine::Mesh*>( ro->getMesh().get() );
        if ( mesh == nullptr )
        {
            LOG( TLogLevel::logWARNING ) << "MeshFeatureTracking is available only for meshes";
            return;
        }

        // We set both objects at once, they should remain consistent while m_pickedRoIdx is valid
        m_pickedRoIdx = data.m_roIdx;
        m_pickedMesh  = mesh;
    }
    // check picking mode / render config OK
    if ( data.m_mode == PickingMode::RO || data.m_mode > PickingMode::TRIANGLE ) { return; }
    m_data.m_mode = data.m_mode;
    m_data.m_data = {-1, -1, -1, -1};

    auto rm = m_pickedMesh->getRenderMode();
    if ( rm == MeshRenderMode::RM_POINTS && m_data.m_mode != PickingMode::VERTEX )
    {
        m_data.m_mode = PickingMode::RO;
        return;
    }
    if ( ( rm == MeshRenderMode::RM_LINES || rm == MeshRenderMode::RM_LINE_LOOP ||
           rm == MeshRenderMode::RM_LINE_STRIP || rm == MeshRenderMode::RM_LINES_ADJACENCY ||
           rm == MeshRenderMode::RM_LINE_STRIP_ADJACENCY ) &&
         m_data.m_mode == PickingMode::TRIANGLE )
    {
        m_data.m_mode = PickingMode::RO;
        return;
    }
    // fill data accordingly
    if ( rm == MeshRenderMode::RM_POINTS )
    {
        m_data.m_data[0] = data.m_elementIdx[0];
        return;
    }
    // if lines, retrieve triangle-based indices
    if ( rm == MeshRenderMode::RM_LINES )
    {
        int v0, v1, t0, t1;
        getPos_L2T( data.m_elementIdx[0], v0, v1, t0, t1 );
        const auto& t = m_pickedMesh->getTriangleMesh().m_triangles;
        if ( m_data.m_mode == PickingMode::VERTEX )
        {
            m_data.m_data[0] = data.m_vertexIdx[0] == 0 ? t[t0]( v0 ) : t[t1]( v1 );
            m_data.m_data[1] = data.m_vertexIdx[0] == 0 ? t[t1]( v1 ) : t[t0]( v0 );
        }
        else
        {
            m_data.m_data[0] = t[t0]( v0 );
            m_data.m_data[1] = t[t1]( v1 );
        }
        return;
    }
    if ( rm == MeshRenderMode::RM_LINE_LOOP || rm == MeshRenderMode::RM_LINE_STRIP )
    {
        int v0, v1, t0, t1;
        getPos_L2T_strip( data.m_elementIdx[0], v0, v1, t0, t1 );
        const auto& t = m_pickedMesh->getTriangleMesh().m_triangles;
        if ( m_data.m_mode == PickingMode::VERTEX )
        {
            m_data.m_data[0] = data.m_vertexIdx[0] == 0 ? t[t0]( v0 ) : t[t1]( v1 );
            m_data.m_data[1] = data.m_vertexIdx[0] == 0 ? t[t1]( v1 ) : t[t0]( v0 );
        }
        else
        {
            m_data.m_data[0] = t[t0]( v0 );
            m_data.m_data[1] = t[t1]( v1 );
        }
        return;
    }
    if ( rm == MeshRenderMode::RM_LINES_ADJACENCY )
    {
        int v0, v1, t0, t1;
        getPos_L2T_adjacency( data.m_elementIdx[0], v0, v1, t0, t1 );
        const auto& t = m_pickedMesh->getTriangleMesh().m_triangles;
        if ( m_data.m_mode == PickingMode::VERTEX )
        {
            m_data.m_data[0] = data.m_vertexIdx[0] == 0 ? t[t0]( v0 ) : t[t1]( v1 );
            m_data.m_data[1] = data.m_vertexIdx[0] == 0 ? t[t1]( v1 ) : t[t0]( v0 );
        }
        else
        {
            m_data.m_data[0] = t[t0]( v0 );
            m_data.m_data[1] = t[t1]( v1 );
        }
        return;
    }
    if ( rm == MeshRenderMode::RM_LINE_STRIP_ADJACENCY )
    {
        int v0, v1, t0, t1;
        getPos_L2T_strip_adjacency( data.m_elementIdx[0], v0, v1, t0, t1 );
        const auto& t = m_pickedMesh->getTriangleMesh().m_triangles;
        if ( m_data.m_mode == PickingMode::VERTEX )
        {
            m_data.m_data[0] = data.m_vertexIdx[0] == 0 ? t[t0]( v0 ) : t[t1]( v1 );
            m_data.m_data[1] = data.m_vertexIdx[0] == 0 ? t[t1]( v1 ) : t[t0]( v0 );
        }
        else
        {
            m_data.m_data[0] = t[t0]( v0 );
            m_data.m_data[1] = t[t1]( v1 );
        }
        return;
    }
    if ( rm == MeshRenderMode::RM_TRIANGLES && m_data.m_mode == PickingMode::VERTEX )
    {
        const auto& T    = m_pickedMesh->getTriangleMesh().m_triangles[data.m_elementIdx[0]];
        m_data.m_data[0] = T( data.m_vertexIdx[0] );
        m_data.m_data[1] = T( ( data.m_vertexIdx[0] + 1 ) % 3 );
        return;
    }
    if ( rm == MeshRenderMode::RM_TRIANGLES && m_data.m_mode == PickingMode::EDGE )
    {
        const auto& T    = m_pickedMesh->getTriangleMesh().m_triangles[data.m_elementIdx[0]];
        m_data.m_data[0] = T( ( data.m_edgeIdx[0] + 1 ) % 3 );
        m_data.m_data[1] = T( ( data.m_edgeIdx[0] + 2 ) % 3 );
        return;
    }
    if ( rm == MeshRenderMode::RM_TRIANGLES )
    {
        const auto& T    = m_pickedMesh->getTriangleMesh().m_triangles[data.m_elementIdx[0]];
        m_data.m_data[0] = T( 0 );
        m_data.m_data[1] = T( 1 );
        m_data.m_data[2] = T( 2 );
        m_data.m_data[3] = data.m_elementIdx[0];
        return;
    }
    if ( rm == MeshRenderMode::RM_TRIANGLE_STRIP && m_data.m_mode == PickingMode::VERTEX )
    {
        int v0, v1, v2, t0, t1, t2;
        getPos_TS2T( data.m_elementIdx[0], v0, v1, v2, t0, t1, t2 );
        const auto& t    = m_pickedMesh->getTriangleMesh().m_triangles;
        m_data.m_data[0] = data.m_vertexIdx[0] == 0
                               ? t[t0]( v0 )
                               : ( data.m_vertexIdx[0] == 1 ? t[t1]( v1 ) : t[t2]( v2 ) );
        m_data.m_data[1] = data.m_vertexIdx[0] == 0 ? t[t1]( v1 ) : t[t0]( v0 );
        return;
    }
    if ( rm == MeshRenderMode::RM_TRIANGLE_STRIP && m_data.m_mode == PickingMode::EDGE )
    {
        int v0, v1, v2, t0, t1, t2;
        getPos_TS2T( data.m_elementIdx[0], v0, v1, v2, t0, t1, t2 );
        const auto& t    = m_pickedMesh->getTriangleMesh().m_triangles;
        m_data.m_data[0] = data.m_edgeIdx[0] == 0 ? t[t1]( v1 ) : t[t0]( v0 );
        m_data.m_data[1] = data.m_edgeIdx[0] == 2 ? t[t1]( v1 ) : t[t2]( v2 );
        return;
    }
    if ( rm == MeshRenderMode::RM_TRIANGLE_STRIP )
    {
        int v0, v1, v2, t0, t1, t2;
        getPos_TS2T( data.m_elementIdx[0], v0, v1, v2, t0, t1, t2 );
        const auto& t    = m_pickedMesh->getTriangleMesh().m_triangles;
        m_data.m_data[0] = t[t0]( v0 );
        m_data.m_data[1] = t[t1]( v1 );
        m_data.m_data[2] = t[t2]( v2 );
        m_data.m_data[3] = data.m_elementIdx[0];
        return;
    }
    if ( rm == MeshRenderMode::RM_TRIANGLE_FAN && m_data.m_mode == PickingMode::VERTEX )
    {
        int v1, v2, t1, t2;
        getPos_TF2T( data.m_elementIdx[0], v1, v2, t1, t2 );
        const auto& t    = m_pickedMesh->getTriangleMesh().m_triangles;
        m_data.m_data[0] = data.m_vertexIdx[0] == 0
                               ? t[0]( 0 )
                               : ( data.m_vertexIdx[0] == 1 ? t[t1]( v1 ) : t[t2]( v2 ) );
        m_data.m_data[1] = data.m_vertexIdx[0] == 0 ? t[t1]( v1 ) : t[0]( 0 );
        return;
    }
    if ( rm == MeshRenderMode::RM_TRIANGLE_FAN && m_data.m_mode == PickingMode::EDGE )
    {
        int v1, v2, t1, t2;
        getPos_TF2T( data.m_elementIdx[0], v1, v2, t1, t2 );
        const auto& t    = m_pickedMesh->getTriangleMesh().m_triangles;
        m_data.m_data[0] = data.m_edgeIdx[0] == 0 ? t[t1]( v1 ) : t[0]( 0 );
        m_data.m_data[1] = data.m_edgeIdx[0] == 2 ? t[t1]( v1 ) : t[t2]( v2 );
        return;
    }
    if ( rm == MeshRenderMode::RM_TRIANGLE_FAN )
    {
        int v1, v2, t1, t2;
        getPos_TF2T( data.m_elementIdx[0], v1, v2, t1, t2 );
        const auto& t    = m_pickedMesh->getTriangleMesh().m_triangles;
        m_data.m_data[0] = t[0]( 0 );
        m_data.m_data[1] = t[t1]( v1 );
        m_data.m_data[2] = t[t2]( v2 );
        m_data.m_data[3] = data.m_elementIdx[0];
        return;
    }
}

void MeshFeatureTrackingComponent::setVertexIdx( int idx ) {
    if ( !getRoMgr()->exists( m_pickedRoIdx ) ) { return; }
    CORE_ASSERT( m_pickedMesh != nullptr, "m_pickedRoIdx and m_pickedMesh are inconsistent" );

    m_data.m_data[0] = idx;
    // also need to change second for feature Scale
    const auto rm = m_pickedMesh->getRenderMode();
    if ( rm == MeshRenderMode::RM_POINTS ) { return; }
    const auto& t = m_pickedMesh->getTriangleMesh().m_triangles;
    if ( rm == MeshRenderMode::RM_LINES )
    {
        for ( uint i = 0; i < t.size(); ++i )
        {
            const auto& T = t[i];
            if ( T( 0 ) == idx )
            {
                m_data.m_data[1] = ( i % 2 == 0 ? T( 1 ) : t[i - 1]( 2 ) );
                return;
            }
            if ( T( 1 ) == idx )
            {
                m_data.m_data[1] = ( i % 2 == 0 ? T( 0 ) : T( 2 ) );
                return;
            }
            if ( T( 2 ) == idx )
            {
                m_data.m_data[1] = ( i % 2 == 0 ? t[i + 1]( 0 ) : T( 1 ) );
                return;
            }
        }
    }
    if ( rm == MeshRenderMode::RM_LINE_LOOP || rm == MeshRenderMode::RM_LINE_STRIP )
    {
        for ( uint i = 0; i < t.size(); ++i )
        {
            const auto& T = t[i];
            if ( T( 0 ) == idx )
            {
                m_data.m_data[1] = T( 1 );
                return;
            }
            if ( T( 1 ) == idx )
            {
                m_data.m_data[1] = T( 0 );
                return;
            }
            if ( T( 2 ) == idx )
            {
                m_data.m_data[1] = T( 1 );
                return;
            }
        }
    }
    if ( rm == MeshRenderMode::RM_LINES_ADJACENCY )
    {
        for ( uint i = 0; i < t.size(); ++i )
        {
            const auto& T = t[i];
            if ( T( 0 ) == idx && i % 4 > 1 )
            {
                m_data.m_data[1] = ( i % 4 == 2 ? t[i - 1]( 2 ) : T( 1 ) );
                return;
            }
            if ( T( 1 ) == idx && ( i + 3 ) % 4 > 1 )
            {
                m_data.m_data[1] = ( i % 4 == 0 ? T( 2 ) : T( 0 ) );
                return;
            }
            if ( T( 2 ) == idx && i % 4 < 2 )
            {
                m_data.m_data[1] = ( i % 4 == 0 ? T( 1 ) : t[i + 1]( 0 ) );
                return;
            }
        }
    }
    if ( rm == MeshRenderMode::RM_LINE_STRIP_ADJACENCY )
    {
        for ( uint i = 0; i < t.size(); ++i )
        {
            const auto& T = t[i];
            if ( T( 0 ) == idx && i != 0 )
            {
                m_data.m_data[1] = T( 1 );
                return;
            }
            if ( T( 1 ) == idx && i != 0 )
            {
                m_data.m_data[1] = T( 0 );
                return;
            }
            if ( T( 2 ) == idx )
            {
                m_data.m_data[1] = T( 1 );
                return;
            }
        }
    }
    if ( rm == MeshRenderMode::RM_TRIANGLES || rm == MeshRenderMode::RM_TRIANGLE_STRIP )
    {
        for ( uint i = 0; i < t.size(); ++i )
        {
            const auto& T = t[i];
            if ( T( 0 ) == idx )
            {
                m_data.m_data[1] = T( 1 );
                return;
            }
            if ( T( 1 ) == idx || T( 2 ) == idx )
            {
                m_data.m_data[1] = T( 0 );
                return;
            }
        }
    }
    if ( rm == MeshRenderMode::RM_TRIANGLE_FAN )
    {
        m_data.m_data[1] = ( idx == 0 ? t[0]( 1 ) : 0 );
        return;
    }
}

void MeshFeatureTrackingComponent::setTriangleIdx( int idx ) {
    if ( !getRoMgr()->exists( m_pickedRoIdx ) ) { return; }
    CORE_ASSERT( m_pickedMesh != nullptr, "m_pickedRoIdx and m_pickedMesh are inconsistent" );

    // also need to change all for feature Scale, Position and Vector
    const auto rm = m_pickedMesh->getRenderMode();
    const auto& t = m_pickedMesh->getTriangleMesh().m_triangles;
    if ( rm == MeshRenderMode::RM_TRIANGLES )
    {
        const auto& T    = t[idx];
        m_data.m_data[0] = T( 0 );
        m_data.m_data[1] = T( 1 );
        m_data.m_data[2] = T( 2 );
        m_data.m_data[3] = idx;
        return;
    }
    if ( rm == MeshRenderMode::RM_TRIANGLE_STRIP )
    {
        int v0, v1, v2, t0, t1, t2;
        getPos_TS2T( idx, v0, v1, v2, t0, t1, t2 );
        const auto& t    = m_pickedMesh->getTriangleMesh().m_triangles;
        m_data.m_data[0] = t[t0]( v0 );
        m_data.m_data[1] = t[t1]( v1 );
        m_data.m_data[2] = t[t2]( v2 );
        m_data.m_data[3] = idx;
        return;
    }
    if ( rm == MeshRenderMode::RM_TRIANGLE_FAN )
    {
        int v1, v2, t1, t2;
        getPos_TF2T( idx, v1, v2, t1, t2 );
        const auto& t    = m_pickedMesh->getTriangleMesh().m_triangles;
        m_data.m_data[0] = t[0]( 0 );
        m_data.m_data[1] = t[t1]( v1 );
        m_data.m_data[2] = t[t2]( v2 );
        m_data.m_data[3] = idx;
        return;
    }
}

void MeshFeatureTrackingComponent::update() {
    // check supported picking mode
    if ( m_data.m_mode != PickingMode::RO && m_data.m_mode <= PickingMode::TRIANGLE &&
         getRoMgr()->exists( m_pickedRoIdx ) )
    {
        CORE_ASSERT( m_pickedMesh != nullptr, "m_pickedRoIdx and m_pickedMesh are inconsistent" );

        setPosition( getFeaturePosition() );
        setScale( getFeatureScale() );
        m_RO->setVisible( true );
    }
    else
    { m_RO->setVisible( false ); }
}

FeatureData MeshFeatureTrackingComponent::getFeatureData() const {
    return m_data;
}

Scalar MeshFeatureTrackingComponent::getFeatureScale() const {
    if ( !getRoMgr()->exists( m_pickedRoIdx ) ) { return 1_ra; }
    CORE_ASSERT( m_pickedMesh != nullptr, "m_pickedRoIdx and m_pickedMesh are inconsistent" );

    // check supported picking mode
    if ( m_data.m_mode == PickingMode::RO ) { return 1_ra; }

    // manage picking mode
    auto ro = getRoMgr()->getRenderObject( m_pickedRoIdx );
    if ( m_pickedMesh->getRenderMode() == MeshRenderMode::RM_POINTS )
    { return ro->computeAabb().sizes().norm() * POINT_SCALE_FACTOR; }
    const auto& v = m_pickedMesh->getTriangleMesh().vertices();
    switch ( m_data.m_mode )
    {
    case PickingMode::VERTEX:
    {
        // return 1 fourth of the edge length of the first edge we can find with the vertex
        const Ra::Core::Vector3& v0 = v[m_data.m_data[0]];
        const Ra::Core::Vector3& v1 = v[m_data.m_data[1]];
        return ( v1 - v0 ).norm() / 4_ra;
    }
    case PickingMode::EDGE:
    {
        // return 1 fourth of the edge length
        const Ra::Core::Vector3& v0 = v[m_data.m_data[0]];
        const Ra::Core::Vector3& v1 = v[m_data.m_data[1]];
        return ( v1 - v0 ).norm() / 4_ra;
    }
    case PickingMode::TRIANGLE:
    {
        // return half the smallest distance from C to an edge
        const Ra::Core::Vector3& v0 = v[m_data.m_data[0]];
        const Ra::Core::Vector3& v1 = v[m_data.m_data[1]];
        const Ra::Core::Vector3& v2 = v[m_data.m_data[2]];
        const Ra::Core::Vector3 C   = ( v0 + v1 + v2 ) / 3_ra;
        const Ra::Core::Vector3 C0  = C - v0;
        const Ra::Core::Vector3 C1  = C - v1;
        const Ra::Core::Vector3 C2  = C - v2;
        return sqrt( std::min(
                   std::min( C0.squaredNorm() *
                                 ( v1 - v0 ).normalized().cross( C0.normalized() ).squaredNorm(),
                             C1.squaredNorm() *
                                 ( v2 - v1 ).normalized().cross( C1.normalized() ).squaredNorm() ),
                   C2.squaredNorm() *
                       ( v0 - v2 ).normalized().cross( C2.normalized() ).squaredNorm() ) ) /
               2.0;
    }
    default:
        return 1_ra;
    }
}

Ra::Core::Vector3 MeshFeatureTrackingComponent::getFeaturePosition() const {
    if ( !getRoMgr()->exists( m_pickedRoIdx ) ) { return Ra::Core::Vector3(); }
    CORE_ASSERT( m_pickedMesh != nullptr, "m_pickedRoIdx and m_pickedMesh are inconsistent" );

    // check supported picking mode
    if ( m_data.m_mode == PickingMode::RO ) { return Ra::Core::Vector3(); }

    // manage picking mode
    const auto& v = m_pickedMesh->getTriangleMesh().vertices();
    Ra::Core::Vector3 P( 0, 0, 0 );
    switch ( m_data.m_mode )
    {
    case PickingMode::VERTEX:
    {
        P = v[m_data.m_data[0]];
        break;
    }
    case PickingMode::EDGE:
    {
        P = ( v[m_data.m_data[0]] + v[m_data.m_data[1]] ) / 2_ra;
        break;
    }
    case PickingMode::TRIANGLE:
    {
        P = ( v[m_data.m_data[0]] + v[m_data.m_data[1]] + v[m_data.m_data[2]] ) / 3_ra;
        break;
    }
    default:
        break;
    }

    // deal with transformations
    auto ro = getRoMgr()->getRenderObject( m_pickedRoIdx );
    return ro->getTransform() * P;
}

Ra::Core::Vector3 MeshFeatureTrackingComponent::getFeatureVector() const {
    if ( !getRoMgr()->exists( m_pickedRoIdx ) ) { return Ra::Core::Vector3(); }
    CORE_ASSERT( m_pickedMesh != nullptr, "m_pickedRoIdx and m_pickedMesh are inconsistent" );

    // check supported picking mode
    if ( m_data.m_mode == PickingMode::RO ) { return Ra::Core::Vector3(); }

    // manage picking mode
    const auto& n = m_pickedMesh->getTriangleMesh().normals();
    if ( m_data.m_mode == PickingMode::VERTEX )
    {
        if ( !n.empty() ) { return n[m_data.m_data[0]]; }
        return Ra::Core::Vector3();
    }
    const auto& v = m_pickedMesh->getTriangleMesh().vertices();
    Ra::Core::Vector3 V( 0, 0, 0 );
    switch ( m_data.m_mode )
    {
    case PickingMode::EDGE:
    {
        // for edges, the edge vector
        V = v[m_data.m_data[0]] - v[m_data.m_data[1]];
        break;
    }
    case PickingMode::TRIANGLE:
    {
        // for triangles, the normal
        const Ra::Core::Vector3& p0 = v[m_data.m_data[0]];
        const Ra::Core::Vector3& p1 = v[m_data.m_data[1]];
        const Ra::Core::Vector3& p2 = v[m_data.m_data[2]];
        V                           = ( p1 - p0 ).cross( p2 - p0 ).normalized();
        break;
    }
    default:
        break;
    }

    // deal with transformations
    auto ro = getRoMgr()->getRenderObject( m_pickedRoIdx );
    return ( ro->getTransformAsMatrix().inverse().transpose() *
             Ra::Core::Vector4( V( 0 ), V( 1 ), V( 2 ), 0_ra ) )
        .head<3>();
}

} // namespace MeshFeatureTrackingPlugin
