#include <Core/Geometry/MeshPrimitives.hpp>
#include <Core/Geometry/TopologicalMesh.hpp>
#include <Core/Geometry/TriangleMesh.hpp>
#include <catch2/catch.hpp>

#include <OpenMesh/Tools/Subdivider/Uniform/CatmullClarkT.hh>

using namespace Ra::Core;
using namespace Ra::Core::Utils;
using namespace Ra::Core::Geometry;

bool isSameMesh( const Ra::Core::Geometry::TriangleMesh& meshOne,
                 const Ra::Core::Geometry::TriangleMesh& meshTwo,
                 bool expected = true ) {

    bool result = true;
    int i       = 0;
    // Check length
    if ( meshOne.vertices().size() != meshTwo.vertices().size() )
    {
        if ( expected )
        {
            LOG( logINFO ) << "isSameMesh failed vertices.size()" << meshOne.vertices().size()
                           << " " << meshTwo.vertices().size();
        }
        return false;
    }
    if ( meshOne.normals().size() != meshTwo.normals().size() )
    {
        if ( expected ) { LOG( logINFO ) << "isSameMesh failed normals.size()"; }
        return false;
    }

    if ( meshOne.getIndices().size() != meshTwo.getIndices().size() )
    {
        if ( expected ) { LOG( logINFO ) << "isSameMesh failed getIndices().size()"; }
        return false;
    }

    // Check triangles
    std::vector<Vector3> stackVertices;
    std::vector<Vector3> stackNormals;

    bool hasNormals = meshOne.normals().size() > 0;

    i = 0;
    while ( result && i < int( meshOne.getIndices().size() ) )
    {
        std::vector<Ra::Core::Vector3>::iterator it;
        stackVertices.clear();
        stackVertices.push_back( meshOne.vertices()[meshOne.getIndices()[i][0]] );
        stackVertices.push_back( meshOne.vertices()[meshOne.getIndices()[i][1]] );
        stackVertices.push_back( meshOne.vertices()[meshOne.getIndices()[i][2]] );

        if ( hasNormals )
        {
            stackNormals.clear();
            stackNormals.push_back( meshOne.normals()[meshOne.getIndices()[i][0]] );
            stackNormals.push_back( meshOne.normals()[meshOne.getIndices()[i][1]] );
            stackNormals.push_back( meshOne.normals()[meshOne.getIndices()[i][2]] );
        }
        for ( int j = 0; j < 3; ++j )
        {
            it = find( stackVertices.begin(),
                       stackVertices.end(),
                       meshTwo.vertices()[meshTwo.getIndices()[i][j]] );
            if ( it != stackVertices.end() ) { stackVertices.erase( it ); }
            else
            {

                if ( expected ) { LOG( logINFO ) << "isSameMesh failed face not found"; }

                result = false;
            }
        }

        if ( hasNormals )
        {
            for ( int j = 0; j < 3; ++j )
            {
                it = find( stackNormals.begin(),
                           stackNormals.end(),
                           meshTwo.normals()[meshTwo.getIndices()[i][j]] );
                if ( it != stackNormals.end() ) { stackNormals.erase( it ); }
                else
                { result = false; }
            }
        }
        ++i;
    }
    return result;
}

class WedgeDataAndIdx
{
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    TopologicalMesh::WedgeData m_data;

    size_t m_idx;

    bool operator<( const WedgeDataAndIdx& lhs ) const { return m_data < lhs.m_data; }
    bool operator==( const WedgeDataAndIdx& lhs ) const { return !( m_data != lhs.m_data ); }
    bool operator!=( const WedgeDataAndIdx& lhs ) const { return !( *this == lhs ); }
};

#define COPY_TO_WEDGES_VECTOR_HELPER( UPTYPE, REALTYPE )                                      \
    if ( attr->is##UPTYPE() )                                                                 \
    {                                                                                         \
        auto data =                                                                           \
            meshOne.getAttrib( meshOne.getAttribHandle<REALTYPE>( attr->getName() ) ).data(); \
        for ( size_t i = 0; i < size; ++i )                                                   \
        {                                                                                     \
            wedgesMeshOne[i].m_data.getAttribArray<REALTYPE>().push_back( data[i] );          \
        }                                                                                     \
    }

void copyToWedgesVector( size_t size,
                         const TriangleMesh& meshOne,
                         AlignedStdVector<WedgeDataAndIdx>& wedgesMeshOne,
                         AttribBase* attr ) {

    if ( attr->getSize() != meshOne.vertices().size() )
    {
        LOG( logWARNING ) << "[TopologicalMesh test] Skip badly sized attribute "
                          << attr->getName();
    }
    else if ( attr->getName() != std::string( "in_position" ) )
    {
        {
            auto data = meshOne.vertices();
            for ( size_t i = 0; i < size; ++i )
            {
                wedgesMeshOne[i].m_data.m_position = data[i];
            }
        }
        COPY_TO_WEDGES_VECTOR_HELPER( Float, float );
        COPY_TO_WEDGES_VECTOR_HELPER( Vector2, Vector2 );
        COPY_TO_WEDGES_VECTOR_HELPER( Vector3, Vector3 );
        COPY_TO_WEDGES_VECTOR_HELPER( Vector4, Vector4 );
    }
}
#undef COPY_TO_WEDGES_VECTOR_HELPER

bool isSameMeshWedge( const Ra::Core::Geometry::TriangleMesh& meshOne,
                      const Ra::Core::Geometry::TriangleMesh& meshTwo ) {

    using namespace Ra::Core;
    using namespace Ra::Core::Geometry;

    // Check length
    if ( meshOne.vertices().size() != meshTwo.vertices().size() ) return false;
    if ( meshOne.normals().size() != meshTwo.normals().size() ) return false;
    if ( meshOne.getIndices().size() != meshTwo.getIndices().size() ) return false;

    AlignedStdVector<WedgeDataAndIdx> wedgesMeshOne;
    AlignedStdVector<WedgeDataAndIdx> wedgesMeshTwo;

    auto size = meshOne.vertices().size();
    for ( size_t i = 0; i < size; ++i )
    {
        WedgeDataAndIdx wd;
        wd.m_idx = i;
        wedgesMeshOne.push_back( wd );
        wedgesMeshTwo.push_back( wd );
    }
    using namespace std::placeholders;
    auto f1 =
        std::bind( copyToWedgesVector, size, std::cref( meshOne ), std::ref( wedgesMeshOne ), _1 );
    meshOne.vertexAttribs().for_each_attrib( f1 );

    auto f2 =
        std::bind( copyToWedgesVector, size, std::cref( meshTwo ), std::ref( wedgesMeshTwo ), _1 );
    meshTwo.vertexAttribs().for_each_attrib( f2 );

    std::sort( wedgesMeshOne.begin(), wedgesMeshOne.end() );
    std::sort( wedgesMeshTwo.begin(), wedgesMeshTwo.end() );

    if ( wedgesMeshOne != wedgesMeshTwo ) return false;

    std::vector<int> newMeshOneIdx( wedgesMeshOne.size() );
    std::vector<int> newMeshTwoIdx( wedgesMeshOne.size() );
    size_t curIdx                         = 0;
    newMeshOneIdx[wedgesMeshOne[0].m_idx] = 0;
    newMeshTwoIdx[wedgesMeshTwo[0].m_idx] = 0;

    for ( size_t i = 1; i < wedgesMeshOne.size(); ++i )
    {
        if ( wedgesMeshOne[i] != wedgesMeshOne[i - 1] ) ++curIdx;
        newMeshOneIdx[wedgesMeshOne[i].m_idx] = curIdx;
    }

    curIdx = 0;
    for ( size_t i = 1; i < wedgesMeshTwo.size(); ++i )
    {
        if ( wedgesMeshTwo[i] != wedgesMeshTwo[i - 1] ) ++curIdx;
        newMeshTwoIdx[wedgesMeshTwo[i].m_idx] = curIdx;
    }

    auto indices1 = meshOne.getIndices();
    auto indices2 = meshTwo.getIndices();

    for ( auto& triangle : indices1 )
    {
        triangle[0] = newMeshOneIdx[triangle[0]];
        triangle[1] = newMeshOneIdx[triangle[1]];
        triangle[2] = newMeshOneIdx[triangle[2]];
    }
    for ( auto& triangle : indices2 )
    {
        triangle[0] = newMeshTwoIdx[triangle[0]];
        triangle[1] = newMeshTwoIdx[triangle[1]];
        triangle[2] = newMeshTwoIdx[triangle[2]];
    }
    return indices1 == indices2;
}

TEST_CASE( "Core/Geometry/TopologicalMesh", "[Core][Core/Geometry][TopologicalMesh]" ) {
    using Ra::Core::Vector3;
    using Ra::Core::Geometry::TopologicalMesh;
    using Ra::Core::Geometry::TriangleMesh;

    auto testConverter = []( const TriangleMesh& mesh ) {
        auto topologicalMesh      = TopologicalMesh( mesh );
        auto topologicalMeshWedge = TopologicalMesh {};
        topologicalMeshWedge.initWithWedge( mesh );
        auto newMeshWedge        = topologicalMesh.toTriangleMeshFromWedges();
        auto newMesh             = topologicalMesh.toTriangleMesh();
        auto newMeshWedgeWedge   = topologicalMeshWedge.toTriangleMeshFromWedges();
        auto newMeshWedgeWithout = topologicalMeshWedge.toTriangleMesh();
        REQUIRE( isSameMesh( mesh, newMesh ) );
        REQUIRE( isSameMesh( mesh, newMeshWedge ) );
        REQUIRE( isSameMesh( mesh, newMeshWedgeWedge ) );
        REQUIRE( isSameMesh( mesh, newMeshWedgeWithout ) );
        REQUIRE( isSameMeshWedge( mesh, newMeshWedge ) );
        REQUIRE( isSameMeshWedge( mesh, newMeshWedgeWedge ) );
        REQUIRE( topologicalMesh.checkIntegrity() );
        REQUIRE( topologicalMeshWedge.checkIntegrity() );
    };

    SECTION( "Closed mesh" ) {
        testConverter( Ra::Core::Geometry::makeBox() );
        testConverter( Ra::Core::Geometry::makeSharpBox() );
    }

    SECTION( "Mesh with boundaries" ) {
        testConverter( Ra::Core::Geometry::makePlaneGrid( 2, 2 ) );
    }

    SECTION( "With user def attribs" ) {
        using Vector5 = Eigen::Matrix<Scalar, 5, 1>;
        VectorArray<Vector5> array5 {{0_ra, 0_ra, 0_ra, 0_ra, 1_ra},
                                     {0_ra, 0_ra, 0_ra, 0_ra, 1_ra},
                                     {0_ra, 0_ra, 0_ra, 0_ra, 1_ra},
                                     {0_ra, -1_ra, 0_ra, 0_ra, 0_ra},
                                     {0_ra, 0_ra, 0_ra, 0_ra, 1_ra},
                                     {0_ra, 0_ra, 0_ra, 0_ra, 1_ra},
                                     {0_ra, 0_ra, 0_ra, 0_ra, 1_ra},
                                     {0_ra, -1_ra, 0_ra, 0_ra, 0_ra}};
        VectorArray<Vector4> array4 {{0_ra, 0_ra, 0_ra, 1_ra},
                                     {0_ra, 0_ra, 0_ra, 1_ra},
                                     {0_ra, 0_ra, 0_ra, 1_ra},
                                     {-1_ra, 0_ra, 0_ra, 0_ra},
                                     {0_ra, 0_ra, 0_ra, 1_ra},
                                     {0_ra, 0_ra, 0_ra, 1_ra},
                                     {0_ra, 0_ra, 0_ra, 1_ra},
                                     {-1_ra, 0_ra, 0_ra, 0_ra}};
        VectorArray<Vector2> array2 {{0_ra, 1_ra},
                                     {0_ra, 1_ra},
                                     {0_ra, 1_ra},
                                     {0_ra, 0_ra},
                                     {0_ra, 1_ra},
                                     {0_ra, 1_ra},
                                     {0_ra, 1_ra},
                                     {0_ra, 0_ra}};

        auto mesh     = Ra::Core::Geometry::makeBox();
        auto handle2  = mesh.addAttrib<Vector2>( "vector2_attrib" );
        auto handle4  = mesh.addAttrib<Vector4>( "vector4_attrib" );
        auto handle5  = mesh.addAttrib<Vector5>( "vector5_attrib" );
        auto ehandle2 = mesh.addAttrib<Vector2>( "evector2_attrib" );
        auto ehandle4 = mesh.addAttrib<Vector4>( "evector4_attrib" );
        auto ehandle5 = mesh.addAttrib<Vector5>( "evector5_attrib" );

        auto& attrib2 = mesh.getAttrib( handle2 );
        auto& attrib4 = mesh.getAttrib( handle4 );
        auto& attrib5 = mesh.getAttrib( handle5 );
        auto& buf2    = attrib2.getDataWithLock();
        auto& buf4    = attrib4.getDataWithLock();
        auto& buf5    = attrib5.getDataWithLock();
        buf2          = array2;
        buf4          = array4;
        buf5          = array5;
        attrib2.unlock();
        attrib4.unlock();
        attrib5.unlock();

        auto topologicalMesh      = TopologicalMesh( mesh );
        auto topologicalMeshWedge = TopologicalMesh {};
        topologicalMeshWedge.initWithWedge( mesh );
        auto newMesh             = topologicalMesh.toTriangleMesh();
        auto newMeshWedge        = topologicalMesh.toTriangleMeshFromWedges();
        auto newMeshWedgeWedge   = topologicalMeshWedge.toTriangleMeshFromWedges();
        auto newMeshWedgeWithout = topologicalMeshWedge.toTriangleMesh();
        REQUIRE( isSameMesh( mesh, newMesh ) );
        REQUIRE( isSameMesh( mesh, newMeshWedge ) );
        REQUIRE( isSameMesh( mesh, newMeshWedgeWedge ) );
        REQUIRE( isSameMesh( mesh, newMeshWedgeWithout ) );
        REQUIRE( isSameMeshWedge( mesh, newMeshWedge ) );
        REQUIRE( isSameMeshWedge( mesh, newMeshWedgeWedge ) );
        REQUIRE( topologicalMesh.checkIntegrity() );
        REQUIRE( topologicalMeshWedge.checkIntegrity() );

        // oversize attrib not suported
        REQUIRE( !newMesh.hasAttrib( "vector5_attrib" ) );
        REQUIRE( !newMeshWedge.hasAttrib( "vector5_attrib" ) );
        REQUIRE( !newMeshWedgeWedge.hasAttrib( "vector5_attrib" ) );
        REQUIRE( !newMeshWedgeWithout.hasAttrib( "vector5_attrib" ) );

        REQUIRE( newMesh.hasAttrib( "vector2_attrib" ) );
        REQUIRE( newMeshWedge.hasAttrib( "vector2_attrib" ) );
        REQUIRE( newMeshWedgeWedge.hasAttrib( "vector2_attrib" ) );

        REQUIRE( newMesh.hasAttrib( "vector4_attrib" ) );
        REQUIRE( newMeshWedge.hasAttrib( "vector4_attrib" ) );
        REQUIRE( newMeshWedgeWedge.hasAttrib( "vector4_attrib" ) );

        // When init with wedge, and converted from propos (without wedges) wedge, attribs are lost.
        REQUIRE( !newMeshWedgeWithout.hasAttrib( "vector2_attrib" ) );
        REQUIRE( !newMeshWedgeWithout.hasAttrib( "vector4_attrib" ) );

        // empty attrib not converted
        REQUIRE( !newMesh.hasAttrib( "evector2_attrib" ) );
        REQUIRE( !newMeshWedge.hasAttrib( "evector2_attrib" ) );
        REQUIRE( !newMeshWedgeWedge.hasAttrib( "veector2_attrib" ) );
        REQUIRE( !newMeshWedgeWithout.hasAttrib( "veector2_attrib" ) );
        REQUIRE( !newMesh.hasAttrib( "evector4_attrib" ) );
        REQUIRE( !newMeshWedge.hasAttrib( "evector4_attrib" ) );
        REQUIRE( !newMeshWedgeWedge.hasAttrib( "veector4_attrib" ) );
        REQUIRE( !newMeshWedgeWithout.hasAttrib( "veector4_attrib" ) );
        REQUIRE( !newMesh.hasAttrib( "evector5_attrib" ) );
        REQUIRE( !newMeshWedge.hasAttrib( "evector5_attrib" ) );
        REQUIRE( !newMeshWedgeWedge.hasAttrib( "evector5_attrib" ) );
        REQUIRE( !newMeshWedgeWithout.hasAttrib( "evector5_attrib" ) );
    }

    SECTION( "Edit topo mesh" ) {
        auto mesh = Ra::Core::Geometry::makeCylinder( Vector3( 0, 0, 0 ), Vector3( 0, 0, 1 ), 1 );

        auto topologicalMesh = TopologicalMesh( mesh );
        auto newMesh         = topologicalMesh.toTriangleMesh();
        auto newMesh2        = topologicalMesh.toTriangleMeshFromWedges();
        topologicalMesh.setWedgeData(
            TopologicalMesh::WedgeIndex {0}, "in_normal", Vector3( 0, 0, 0 ) );
        auto newMesh3 = topologicalMesh.toTriangleMeshFromWedges();

        REQUIRE( isSameMesh( mesh, newMesh ) );
        REQUIRE( isSameMesh( mesh, newMesh2 ) );
        REQUIRE( isSameMeshWedge( mesh, newMesh2 ) );
        REQUIRE( !isSameMeshWedge( mesh, newMesh3 ) );
        REQUIRE( topologicalMesh.checkIntegrity() );
    }

    SECTION( "Test skip empty attributes" ) {
        auto mesh = Ra::Core::Geometry::makeCylinder( Vector3( 0, 0, 0 ), Vector3( 0, 0, 1 ), 1 );
        mesh.addAttrib<float>( "empty" );
        auto topologicalMesh      = TopologicalMesh( mesh );
        auto topologicalMeshWedge = TopologicalMesh {};
        topologicalMeshWedge.initWithWedge( mesh );
        auto newMesh           = topologicalMesh.toTriangleMesh();
        auto newMeshWedge      = topologicalMesh.toTriangleMeshFromWedges();
        auto newMeshWedgeWedge = topologicalMeshWedge.toTriangleMeshFromWedges();
        REQUIRE( !newMesh.hasAttrib( "empty" ) );
        REQUIRE( !newMeshWedge.hasAttrib( "empty" ) );
        REQUIRE( !newMeshWedgeWedge.hasAttrib( "empty" ) );
        REQUIRE( topologicalMesh.checkIntegrity() );
        REQUIRE( topologicalMeshWedge.checkIntegrity() );
    }

    SECTION( "Test normals" ) {
        auto mesh            = Ra::Core::Geometry::makeBox();
        auto topologicalMesh = TopologicalMesh( mesh );

        for ( TopologicalMesh::ConstVertexIter v_it = topologicalMesh.vertices_begin();
              v_it != topologicalMesh.vertices_end();
              ++v_it )
        {
            topologicalMesh.set_normal(
                *v_it, TopologicalMesh::Normal( Scalar( 1. ), Scalar( 0. ), Scalar( 0. ) ) );
        }

        for ( TopologicalMesh::ConstVertexIter v_it = topologicalMesh.vertices_begin();
              v_it != topologicalMesh.vertices_end();
              ++v_it )
        {
            topologicalMesh.propagate_normal_to_halfedges( *v_it );
        }

        auto newMesh = topologicalMesh.toTriangleMesh();
        bool check1  = true;
        bool check2  = true;
        for ( auto n : newMesh.normals() )
        {
            if ( !Ra::Core::Math::areApproxEqual(
                     n.dot( Vector3( Scalar( 1. ), Scalar( 0. ), Scalar( 0. ) ) ), Scalar( 1. ) ) )
            { check1 = false; }
            if ( n.dot( Vector3( Scalar( 0.5 ), Scalar( 0. ), Scalar( 0. ) ) ) > Scalar( 0.8 ) )
            { check2 = false; }
        }
        REQUIRE( check1 );
        REQUIRE( check2 );
        REQUIRE( topologicalMesh.checkIntegrity() );
    }

    SECTION( "Test without normals" ) {
        VectorArray<Vector3> vertices = {
            {0_ra, 0_ra, 0_ra}, {0_ra, 1_ra, 0_ra}, {1_ra, 1_ra, 0_ra}, {1_ra, 0_ra, 0_ra}};
        VectorArray<Vector3ui> indices {{0, 2, 1}, {0, 3, 2}};
        // well formed mesh

        TriangleMesh mesh;
        mesh.setVertices( std::move( vertices ) );
        mesh.setIndices( std::move( indices ) );
        TopologicalMesh topo1 {mesh};
        TopologicalMesh topo2;
        topo2.initWithWedge( mesh );

        REQUIRE( topo1.checkIntegrity() );
        REQUIRE( topo2.checkIntegrity() );

        TriangleMesh mesh1 = topo1.toTriangleMesh();
        TriangleMesh mesh2 = topo2.toTriangleMeshFromWedges();

        // there is no normals at all.
        REQUIRE( !topo1.has_halfedge_normals() );
        REQUIRE( !topo1.has_face_normals() );
        for ( auto vitr = topo1.vertices_begin(), vend = topo1.vertices_end(); vitr != vend;
              ++vitr )
        {
            for ( auto fitr = topo1.vf_iter( *vitr ); fitr.is_valid(); ++fitr )
            {
                auto n = topo1.normal( *vitr, *fitr );
                REQUIRE( Math::areApproxEqual( n.squaredNorm(), 0_ra ) );
            }
            topo1.propagate_normal_to_halfedges( *vitr );
            REQUIRE( !topo1.has_halfedge_normals() );
            REQUIRE( !topo1.has_face_normals() );
        }

        // nor on faces nor if we try to create them
        REQUIRE( !topo1.has_face_normals() );
        OpenMesh::FPropHandleT<TopologicalMesh::Normal> fProp;
        topo1.createNormalPropOnFaces( fProp );
        auto vh  = *topo1.vertices_begin();
        auto he1 = topo1.halfedge_handle( vh );
        auto he2 = topo1.next_halfedge_handle( he1 );
        auto fh  = topo1.face_handle( he1 );
        REQUIRE( !fProp.is_valid() );
        REQUIRE( !topo1.has_face_normals() );
        // even if we try to copy them, but no access error
        topo1.copyNormal( he1, he2 );
        topo1.copyNormalFromFace( fh, he1, fProp );
        topo1.interpolateNormalOnFaces( fh, fProp );
        REQUIRE( !topo1.has_halfedge_normals() );

        REQUIRE( mesh.vertexAttribs().hasSameAttribs( mesh1.vertexAttribs() ) );
        REQUIRE( mesh.vertexAttribs().hasSameAttribs( mesh2.vertexAttribs() ) );
        REQUIRE( isSameMesh( mesh, mesh1 ) );
        REQUIRE( isSameMesh( mesh, mesh2 ) );

        REQUIRE( mesh1.normals().size() == 0 );
        REQUIRE( mesh2.normals().size() == 0 );
    }
}

void test_split( TopologicalMesh& topo, TopologicalMesh::EdgeHandle eh, float f ) {

    auto he0 = topo.halfedge_handle( eh, 0 );
    auto he1 = topo.halfedge_handle( eh, 1 );
    auto v0  = topo.from_vertex_handle( he0 ); // i.e. to_vertex_handle(he1)
    REQUIRE( v0 == topo.to_vertex_handle( he1 ) );
    auto v1  = topo.to_vertex_handle( he0 );
    auto p0  = topo.point( v0 );
    float f0 = topo.getWedgeData( *( topo.getVertexWedges( v0 ) ).begin() ).m_floatAttrib[0];
    auto p1  = topo.point( v1 );
    float f1 = topo.getWedgeData( *( topo.getVertexWedges( v1 ) ).begin() ).m_floatAttrib[0];
    topo.splitEdgeWedge( eh, f );

    // check validity
    REQUIRE( topo.is_valid_handle( he0 ) );
    REQUIRE( topo.is_valid_handle( he1 ) );

    // he0 is untouched
    REQUIRE( v1 == topo.to_vertex_handle( he0 ) );
    REQUIRE( Math::areApproxEqual( ( p1 - topo.point( v1 ) ).squaredNorm(), 0_ra ) );

    // he1 point to inserted vertex
    auto vsplit = topo.to_vertex_handle( he1 ); // i.e. from_vertex_handle(he0)
    REQUIRE( vsplit == topo.from_vertex_handle( he0 ) );

    auto psplit = topo.point( vsplit );
    auto vcheck = ( f * p1 + ( 1.f - f ) * p0 );
    REQUIRE( Math::areApproxEqual( ( psplit - vcheck ).squaredNorm(), 0.f ) );

    auto wedges = topo.getVertexWedges( vsplit );
    REQUIRE( wedges.size() == 1 );

    auto wd     = topo.getWedgeData( *wedges.begin() );
    auto fsplit = wd.m_floatAttrib[0];
    auto fcheck = ( f * f1 + ( 1.f - f ) * f0 );
    REQUIRE( Math::areApproxEqual( fsplit, fcheck ) );
    REQUIRE( Math::areApproxEqual( ( psplit - wd.m_position ).squaredNorm(), 0.f ) );
}

/// \todo TEST_CASE( "Core/Geometry/TopologicalMesh/Subdivider",
/// "[Core][Core/Geometry][TopologicalMesh]" ) {
// using Catmull =
//     OpenMesh::Subdivider::Uniform::CatmullClarkT<Ra::Core::Geometry::TopologicalMesh>;
// using Loop = OpenMesh::Subdivider::Uniform::LoopT<Ra::Core::Geometry::TopologicalMesh>;
// using Decimater = OpenMesh::Decimater::DecimaterT<Ra::Core::Geometry::TopologicalMesh>;
// using HModQuadric =
//     OpenMesh::Decimater::ModQuadricT<Ra::Core::Geometry::TopologicalMesh>::Handle;
//}

TEST_CASE( "Core/Geometry/TopologicalMesh/EdgeSplit", "[Core][Core/Geometry][TopologicalMesh]" ) {
    using Ra::Core::Vector3;
    using Ra::Core::Geometry::TopologicalMesh;
    using Ra::Core::Geometry::TriangleMesh;

    // create a triangle mesh with 4 vertices
    TriangleMesh meshSplit;
    meshSplit.setVertices( {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}} );
    meshSplit.setNormals( {{-1, -1, 1}, {1, -1, 1}, {1, 1, 1}, {-1, 1, 1}} );
    meshSplit.setIndices( {Vector3ui( 0, 1, 2 ), Vector3ui( 0, 2, 3 )} );
    // add a float attrib
    auto handle = meshSplit.addAttrib<float>( "test", {0.f, 1.f, 2.f, 3.f} );
    CORE_UNUSED( handle ); // until unit test is finished.

    // convert to topomesh
    TopologicalMesh topo = TopologicalMesh( meshSplit );

    // split middle edge
    TopologicalMesh::EdgeHandle eh;
    // iterate over all to find the inner one
    int innerEdgeCount = 0;
    for ( TopologicalMesh::EdgeIter e_it = topo.edges_begin(); e_it != topo.edges_end(); ++e_it )
    {
        if ( !topo.is_boundary( *e_it ) )
        {
            eh = *e_it;
            ++innerEdgeCount;
        }
    }

    REQUIRE( innerEdgeCount == 1 );
    float f = .3f;

    test_split( topo, eh, f );
    /// \todo : split boundary edge,  collapse,  check float attrib value
}

TEST_CASE( "Core/Geometry/TopologicalMesh/Manifold", "[Core][Core/Geometry][TopologicalMesh]" ) {
    SECTION( "Non manifold faces" ) {
        struct MyNonManifoldCommand {
            inline MyNonManifoldCommand( int target ) : targetNonManifoldFaces( target ) {}
            inline void initialize( const TriangleMesh& /*triMesh*/ ) {}
            inline void
            process( const std::vector<TopologicalMesh::VertexHandle>& /*face_vhandles*/ ) {
                LOG( logINFO ) << "Non Manifold face found";
                nonManifoldFaces++;
            }
            inline void postProcess( TopologicalMesh& /*tm*/ ) {
                // Todo : For each non manifold face, remove the vertices that are not part of a
                // face of the topomesh For the test, this will reduce the mesh_2 to mesh1
                REQUIRE( nonManifoldFaces == targetNonManifoldFaces );
                LOG( logINFO ) << "Process non-manifold faces";
            }

            int nonManifoldFaces {0};
            const int targetNonManifoldFaces;
        };

        auto buildMesh = []( const VectorArray<Vector3>& v,
                             const VectorArray<Vector3>& n,
                             const VectorArray<Vector3ui>& i ) {
            TriangleMesh m;
            m.setVertices( v );
            m.setNormals( n );
            auto& idx = m.getIndicesWithLock();
            std::copy( i.begin(), i.end(), std::back_inserter( idx ) );
            m.indicesUnlock();

            LOG( logINFO ) << " Built a mesh with " << m.vertices().size() << " vertices, "
                           << m.normals().size() << " normals and " << m.getIndices().size()
                           << " indices.";

            return m;
        };

        // test if candidateMesh  -> TopologicalMesh -> TriangleMesh isSameMesh than referenceMesh,
        // with and without the command.
        auto testConverter = []( const TriangleMesh& referenceMesh,
                                 const TriangleMesh& candidateMesh,
                                 MyNonManifoldCommand command ) {
            // test with functor
            TopologicalMesh topoWithCommand {candidateMesh, command};
            TopologicalMesh topoWedgeWithCommand;
            topoWedgeWithCommand.initWithWedge( candidateMesh, command );
            auto convertedMeshWithCommand          = topoWithCommand.toTriangleMesh();
            auto convertedMeshWithCommandFromWedge = topoWithCommand.toTriangleMeshFromWedges();
            auto convertedMeshWedgeWithCommand = topoWedgeWithCommand.toTriangleMeshFromWedges();
            REQUIRE( isSameMesh( referenceMesh, convertedMeshWithCommand ) );
            REQUIRE( isSameMesh( referenceMesh, convertedMeshWedgeWithCommand ) );
            REQUIRE( isSameMesh( referenceMesh, convertedMeshWithCommandFromWedge ) );
            // test without functor
            TopologicalMesh topoWithoutCommand {candidateMesh};
            TopologicalMesh topoWedgeWithoutCommand {};
            topoWedgeWithoutCommand.initWithWedge( candidateMesh );
            auto convertedMeshWithoutCommand = topoWithoutCommand.toTriangleMesh();
            auto convertedMeshWithoutCommandFromWedge =
                topoWithoutCommand.toTriangleMeshFromWedges();
            auto convertedMeshWedgeWithoutCommand = topoWedgeWithoutCommand.toTriangleMesh();
            REQUIRE( isSameMesh( referenceMesh, convertedMeshWithoutCommand ) );
            REQUIRE( isSameMesh( referenceMesh, convertedMeshWedgeWithoutCommand ) );
            REQUIRE( isSameMesh( referenceMesh, convertedMeshWithoutCommandFromWedge ) );
            return convertedMeshWithoutCommand;
        };

        VectorArray<Vector3> vertices = {
            {0_ra, 0_ra, 0_ra}, {0_ra, 1_ra, 0_ra}, {1_ra, 1_ra, 0_ra}, {1_ra, 0_ra, 0_ra}};
        VectorArray<Vector3> normals {
            {0_ra, 0_ra, 1_ra}, {0_ra, 0_ra, 1_ra}, {0_ra, 0_ra, 1_ra}, {0_ra, 0_ra, 1_ra}};
        VectorArray<Vector3ui> indices {{0, 2, 1}, {0, 3, 2}};

        VectorArray<Vector3> vertices_2 = {{0_ra, 0_ra, 0_ra},
                                           {0_ra, 1_ra, 0_ra},
                                           {1_ra, 1_ra, 0_ra},
                                           {1_ra, 0_ra, 0_ra},
                                           {1_ra, 0_ra, 1_ra}};
        VectorArray<Vector3> normals_2 {
            {0_ra, 0_ra, 1_ra},
            {0_ra, 0_ra, 1_ra},
            {0_ra, 0_ra, 1_ra},
            {0_ra, 0_ra, 1_ra},
            {0_ra, -1_ra, 0_ra},
        };

        VectorArray<Vector3ui> indices_2 {{0, 2, 1}, {0, 3, 2}, {0, 2, 4}};

        using Vector5 = Eigen::Matrix<Scalar, 5, 1>;
        VectorArray<Vector5> attrib_array {
            {0_ra, 0_ra, 0_ra, 0_ra, 1_ra},
            {0_ra, 0_ra, 0_ra, 0_ra, 1_ra},
            {0_ra, 0_ra, 0_ra, 0_ra, 1_ra},
            {0_ra, -1_ra, 0_ra, 0_ra, 0_ra},
        };

        // well formed mesh
        auto mesh = buildMesh( vertices, normals, indices );

        // edge shared by three faces
        LOG( logINFO ) << "Test with edge shared by three faces";
        auto mesh2 = buildMesh( vertices_2, normals_2, indices_2 );

        testConverter(
            mesh, mesh2, MyNonManifoldCommand( 1 ) ); // we should find 1 non-manifold face

        // test with unsupported attribute type
        LOG( logINFO ) << "Test with unsupported attribute (all faces are manifold)";
        auto mesh3 {mesh}, mesh4 {mesh};
        auto handle  = mesh3.addAttrib<Vector5>( "vector5_attrib" );
        auto& attrib = mesh3.getAttrib( handle );
        auto& buf    = attrib.getDataWithLock();
        buf          = attrib_array;
        attrib.unlock();

        REQUIRE( mesh4.vertexAttribs().hasSameAttribs( mesh.vertexAttribs() ) );
        REQUIRE( mesh.vertexAttribs().hasSameAttribs( mesh4.vertexAttribs() ) );
        REQUIRE( !mesh4.vertexAttribs().hasSameAttribs( mesh3.vertexAttribs() ) );
        REQUIRE( !mesh3.vertexAttribs().hasSameAttribs( mesh4.vertexAttribs() ) );
        mesh4 = testConverter(
            mesh, mesh3, MyNonManifoldCommand( 0 ) ); // we should find 0 non-manifold face
        REQUIRE( mesh4.vertexAttribs().hasSameAttribs( mesh.vertexAttribs() ) );
        REQUIRE( mesh.vertexAttribs().hasSameAttribs( mesh4.vertexAttribs() ) );
        REQUIRE( !mesh4.vertexAttribs().hasSameAttribs( mesh3.vertexAttribs() ) );
        REQUIRE( !mesh3.vertexAttribs().hasSameAttribs( mesh4.vertexAttribs() ) );

        // TODO : build a functor that add the faces as independant faces in the topomesh and
        // define a manifold mesh that is similar to the result of processing of this non manifold.
        //
    }
    SECTION( "Non manifold vertex : Bow tie" ) {
        VectorArray<Vector3> vertices = {
            {-1_ra, -1_ra, 0_ra},
            {-1_ra, 1_ra, 0_ra},
            {0_ra, 0_ra, 0_ra}, // non manifold vertex
            {1_ra, -1_ra, 0_ra},
            {1_ra, 1_ra, 0_ra},
        };

        VectorArray<Vector3ui> indices {{0, 2, 1}, {2, 3, 4}};
        TriangleMesh mesh;
        // do not move vertices, we need to compare afterward
        mesh.setVertices( vertices );
        mesh.setIndices( std::move( indices ) );

        TopologicalMesh topo {mesh};

        for ( auto itr = topo.vertices_begin(); itr != topo.vertices_end(); ++itr )
        {
            if ( Ra::Core::Math::areApproxEqual( ( topo.point( *itr ) - vertices[2] ).squaredNorm(),
                                                 0_ra ) )
            { REQUIRE( !topo.isManifold( *itr ) ); }
            else
            { REQUIRE( topo.isManifold( *itr ) ); }
        }
    }
    SECTION( "Non manifold vertex : Double pyramid" ) {

        struct MyNonManifoldCommand {
            inline MyNonManifoldCommand(
                std::vector<std::vector<TopologicalMesh::VertexHandle>>& faulty ) :
                m_faulty( faulty ) {}
            inline void initialize( const TriangleMesh& /*triMesh*/ ) {}
            inline void process( const std::vector<TopologicalMesh::VertexHandle>& face_vhandles ) {
                LOG( logINFO ) << "Non Manifold face found";
                m_faulty.push_back( face_vhandles );
                nonManifoldFaces++;
            }
            inline void postProcess( TopologicalMesh& /*tm*/ ) {
                // Todo : For each non manifold face, remove the vertices that are not part of a
                // face of the topomesh For the test, this will reduce the mesh_2 to mesh1
                LOG( logINFO ) << "Process non-manifold faces";
            }
            std::vector<std::vector<TopologicalMesh::VertexHandle>>& m_faulty;
            int nonManifoldFaces {0};
        };

        VectorArray<Vector3> vertices = {{0_ra, 1_ra, 1_ra},
                                         {1_ra, 1_ra, 1_ra},
                                         {0.5_ra, 1_ra, 0_ra},
                                         {0.5_ra, 0.5_ra, 0.5_ra}, // non manifold vertex
                                         {0_ra, 0_ra, 0_ra},
                                         {1_ra, 0_ra, 0_ra},
                                         {0.5_ra, 0_ra, 1_ra}};
        VectorArray<Vector3ui> indices {
            {0, 1, 2}, {2, 1, 3}, {1, 0, 3}, {0, 2, 3}, {4, 5, 6}, {5, 4, 3}, {4, 6, 3}, {6, 5, 3}};

        TriangleMesh mesh;
        mesh.setVertices( std::move( vertices ) );
        mesh.setIndices( std::move( indices ) );
        std::vector<std::vector<TopologicalMesh::VertexHandle>> faulty;

        MyNonManifoldCommand command {faulty};
        TopologicalMesh topo {mesh, command};

        for ( auto itr = faulty.begin(); itr != faulty.end(); ++itr )
        {
            int cpt = 0;
            for ( auto pitr = itr->begin(); pitr != itr->end(); ++pitr )
            {
                // vertex handle is part of the mesh
                REQUIRE( topo.is_valid_handle( *pitr ) );

                // each of the faulty face has one time the non manifold vertex
                if ( Ra::Core::Math::areApproxEqual(
                         ( topo.point( *pitr ) - vertices[3] ).squaredNorm(), 0_ra ) )
                {
                    cpt++;
                    // this vertex is not a boundary (since the faulty face is complex)
                    REQUIRE( !topo.is_boundary( *pitr ) );
                }
            }
            REQUIRE( cpt == 1 );
        }

        for ( auto itr = topo.vertices_begin(); itr != topo.vertices_end(); ++itr )
        {
            REQUIRE( topo.isManifold( *itr ) );
        }
    }
}

TEST_CASE( "Core/Geometry/TopologicalMesh/Initialization",
           "[Core][Core/Geometry][TopologicalMesh]" ) {
    Ra::Core::Geometry::TopologicalMesh topologicalMesh;
    Ra::Core::Geometry::TopologicalMesh::VertexHandle vhandle[3];
    Ra::Core::Geometry::TopologicalMesh::FaceHandle fhandle;

    vhandle[0] =
        topologicalMesh.add_vertex( Ra::Core::Geometry::TopologicalMesh::Point( 1, -1, -1 ) );
    vhandle[1] =
        topologicalMesh.add_vertex( Ra::Core::Geometry::TopologicalMesh::Point( 1, -1, 1 ) );
    vhandle[2] =
        topologicalMesh.add_vertex( Ra::Core::Geometry::TopologicalMesh::Point( -1, -1, 1 ) );

    std::vector<Ra::Core::Geometry::TopologicalMesh::VertexHandle> face_vhandles;
    face_vhandles.push_back( vhandle[0] );
    face_vhandles.push_back( vhandle[1] );
    face_vhandles.push_back( vhandle[2] );
    fhandle = topologicalMesh.add_face( face_vhandles );

    // newly created face have invalid wedges on halfedges
    auto heh = topologicalMesh.halfedge_handle( fhandle );
    REQUIRE( topologicalMesh.property( topologicalMesh.getWedgeIndexPph(), heh ).isInvalid() );
    heh = topologicalMesh.next_halfedge_handle( heh );
    REQUIRE( topologicalMesh.property( topologicalMesh.getWedgeIndexPph(), heh ).isInvalid() );
    heh = topologicalMesh.next_halfedge_handle( heh );
    REQUIRE( topologicalMesh.property( topologicalMesh.getWedgeIndexPph(), heh ).isInvalid() );

    std::cout << "faces: " << topologicalMesh.n_faces() << std::endl;
    REQUIRE( topologicalMesh.n_faces() == 1 );

    topologicalMesh.request_face_status();
    topologicalMesh.delete_face( fhandle, false );
    topologicalMesh.garbage_collection();
    std::cout << "faces: " << topologicalMesh.n_faces() << std::endl;
    REQUIRE( topologicalMesh.n_faces() == 0 );
}

TEST_CASE( "Core/Geometry/TopologicalMesh/MergeWedges", "[Core][Core/Geometry][TopologicalMesh]" ) {

    auto mesh = Ra::Core::Geometry::makeSharpBox();
    auto topo = TopologicalMesh {};
    topo.initWithWedge( mesh );

    std::set<TopologicalMesh::WedgeIndex> wedgesIndices;
    for ( auto itr = topo.halfedges_begin(), stop = topo.halfedges_end(); itr != stop; ++itr )
    {
        wedgesIndices.insert( topo.getWedgeIndex( *itr ) );
    }
    // each 8 vertices of the cube has 3 wedges
    REQUIRE( wedgesIndices.size() == 8 * 3 );
    REQUIRE( topo.checkIntegrity() );
    auto wdRef = topo.getWedgeData( topo.getWedgeIndex( *topo.halfedges_begin() ) );
    for ( auto itr = topo.halfedges_begin(), stop = topo.halfedges_end(); itr != stop; ++itr )
    {
        auto wdCur       = topo.getWedgeData( topo.getWedgeIndex( *itr ) );
        auto wdNew       = wdRef;
        wdNew.m_position = wdCur.m_position;
        topo.setWedgeData( topo.getWedgeIndex( *itr ), wdNew );
    }

    wedgesIndices.clear();
    for ( auto itr = topo.halfedges_begin(), stop = topo.halfedges_end(); itr != stop; ++itr )
    {
        wedgesIndices.insert( topo.getWedgeIndex( *itr ) );
    }
    // each 8 vertices of the cube still has 3 wedges
    REQUIRE( wedgesIndices.size() == 8 * 3 );
    REQUIRE( topo.checkIntegrity() );

    topo.mergeEqualWedges();
    wedgesIndices.clear();
    for ( auto itr = topo.halfedges_begin(), stop = topo.halfedges_end(); itr != stop; ++itr )
    {
        wedgesIndices.insert( topo.getWedgeIndex( *itr ) );
    }
    // after merge, each vertex has only on wedge
    REQUIRE( wedgesIndices.size() == 8 );
    REQUIRE( topo.checkIntegrity() );
}
