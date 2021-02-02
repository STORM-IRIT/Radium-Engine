#include <Core/Geometry/MeshPrimitives.hpp>
#include <Core/Geometry/TopologicalMesh.hpp>
#include <Core/Geometry/TriangleMesh.hpp>
#include <catch2/catch.hpp>

#include <OpenMesh/Tools/Subdivider/Uniform/CatmullClarkT.hh>
#include <OpenMesh/Tools/Subdivider/Uniform/LoopT.hh>

#include <OpenMesh/Tools/Decimater/DecimaterT.hh>
#include <OpenMesh/Tools/Decimater/ModQuadricT.hh>

using namespace Ra::Core;
using namespace Ra::Core::Utils;
using namespace Ra::Core::Geometry;

bool isSameMesh( const Ra::Core::Geometry::TriangleMesh& meshOne,
                 const Ra::Core::Geometry::TriangleMesh& meshTwo ) {

    bool result = true;
    int i       = 0;
    // Check length
    if ( meshOne.vertices().size() != meshTwo.vertices().size() ) return false;
    if ( meshOne.normals().size() != meshTwo.normals().size() ) return false;
    if ( meshOne.getIndices().size() != meshTwo.getIndices().size() ) return false;

    // Check triangles
    std::vector<Vector3> stackVertices;
    std::vector<Vector3> stackNormals;

    i = 0;
    while ( result && i < int( meshOne.getIndices().size() ) )
    {
        std::vector<Ra::Core::Vector3>::iterator it;
        stackVertices.clear();
        stackVertices.push_back( meshOne.vertices()[meshOne.getIndices()[i][0]] );
        stackVertices.push_back( meshOne.vertices()[meshOne.getIndices()[i][1]] );
        stackVertices.push_back( meshOne.vertices()[meshOne.getIndices()[i][2]] );

        stackNormals.clear();
        stackNormals.push_back( meshOne.normals()[meshOne.getIndices()[i][0]] );
        stackNormals.push_back( meshOne.normals()[meshOne.getIndices()[i][1]] );
        stackNormals.push_back( meshOne.normals()[meshOne.getIndices()[i][2]] );

        for ( int j = 0; j < 3; ++j )
        {
            it = find( stackVertices.begin(),
                       stackVertices.end(),
                       meshTwo.vertices()[meshTwo.getIndices()[i][j]] );
            if ( it != stackVertices.end() ) { stackVertices.erase( it ); }
            else
            { result = false; }
        }

        for ( int j = 0; j < 3; ++j )
        {
            it = find( stackNormals.begin(),
                       stackNormals.end(),
                       meshTwo.normals()[meshTwo.getIndices()[i][j]] );
            if ( it != stackNormals.end() ) { stackNormals.erase( it ); }
            else
            { result = false; }
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

    // using Catmull =
    //     OpenMesh::Subdivider::Uniform::CatmullClarkT<Ra::Core::Geometry::TopologicalMesh>;
    // using Loop = OpenMesh::Subdivider::Uniform::LoopT<Ra::Core::Geometry::TopologicalMesh>;

    // using Decimater = OpenMesh::Decimater::DecimaterT<Ra::Core::Geometry::TopologicalMesh>;
    // using HModQuadric =
    //     OpenMesh::Decimater::ModQuadricT<Ra::Core::Geometry::TopologicalMesh>::Handle;

    TriangleMesh newMesh;
    TriangleMesh newMesh2;
    TriangleMesh mesh;
    TopologicalMesh topologicalMesh;

    // Test for close mesh
    mesh            = Ra::Core::Geometry::makeBox();
    topologicalMesh = TopologicalMesh( mesh );
    newMesh2        = topologicalMesh.toTriangleMeshFromWedges();
    newMesh         = topologicalMesh.toTriangleMesh();
    REQUIRE( isSameMesh( mesh, newMesh ) );
    REQUIRE( isSameMesh( mesh, newMesh2 ) );
    REQUIRE( isSameMeshWedge( mesh, newMesh2 ) );
    REQUIRE( topologicalMesh.checkIntegrity() );

    mesh            = Ra::Core::Geometry::makeSharpBox();
    topologicalMesh = TopologicalMesh( mesh );
    newMesh         = topologicalMesh.toTriangleMesh();
    newMesh2        = topologicalMesh.toTriangleMeshFromWedges();
    REQUIRE( isSameMesh( mesh, newMesh ) );
    REQUIRE( isSameMesh( mesh, newMesh2 ) );
    REQUIRE( isSameMeshWedge( mesh, newMesh2 ) );
    REQUIRE( topologicalMesh.checkIntegrity() );

    mesh            = Ra::Core::Geometry::makeSharpBox();
    topologicalMesh = TopologicalMesh();
    topologicalMesh.initWithWedge( mesh );
    newMesh  = topologicalMesh.toTriangleMesh();
    newMesh2 = topologicalMesh.toTriangleMeshFromWedges();
    REQUIRE( isSameMesh( mesh, newMesh ) );
    REQUIRE( isSameMesh( mesh, newMesh2 ) );
    REQUIRE( isSameMeshWedge( mesh, newMesh2 ) );
    REQUIRE( topologicalMesh.checkIntegrity() );

    // Test for mesh with boundaries
    mesh            = Ra::Core::Geometry::makePlaneGrid( 2, 2 );
    topologicalMesh = TopologicalMesh( mesh );
    newMesh         = topologicalMesh.toTriangleMesh();
    newMesh2        = topologicalMesh.toTriangleMeshFromWedges();
    REQUIRE( isSameMesh( mesh, newMesh ) );
    REQUIRE( isSameMesh( mesh, newMesh2 ) );
    REQUIRE( isSameMeshWedge( mesh, newMesh2 ) );
    REQUIRE( topologicalMesh.checkIntegrity() );

    mesh = Ra::Core::Geometry::makeCylinder( Vector3( 0, 0, 0 ), Vector3( 0, 0, 1 ), 1 );

    topologicalMesh = TopologicalMesh( mesh );
    newMesh         = topologicalMesh.toTriangleMesh();
    newMesh2        = topologicalMesh.toTriangleMeshFromWedges();
    topologicalMesh.setWedgeData(
        TopologicalMesh::WedgeIndex {0}, "in_normal", Vector3( 0, 0, 0 ) );
    auto newMesh3 = topologicalMesh.toTriangleMeshFromWedges();

    REQUIRE( isSameMesh( mesh, newMesh ) );
    REQUIRE( isSameMesh( mesh, newMesh2 ) );
    REQUIRE( isSameMeshWedge( mesh, newMesh2 ) );
    REQUIRE( !isSameMeshWedge( mesh, newMesh3 ) );
    REQUIRE( topologicalMesh.checkIntegrity() );

    // Test skip empty attributes
    mesh.addAttrib<float>( "empty" );
    topologicalMesh = TopologicalMesh( mesh );
    newMesh         = topologicalMesh.toTriangleMesh();
    REQUIRE( !newMesh.hasAttrib( "empty" ) );
    REQUIRE( topologicalMesh.checkIntegrity() );

    // Test normals
    mesh            = Ra::Core::Geometry::makeBox();
    topologicalMesh = TopologicalMesh( mesh );

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

    {
        newMesh     = topologicalMesh.toTriangleMesh();
        bool check1 = true;
        bool check2 = true;
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
    // split boundary edge
    // collapse
    // check float attrib value
}

TEST_CASE( "Core/Geometry/TopologicalMesh/Manifold", "[Core][Core/Geometry][TopologicalMesh]" ) {

    struct MyNonManifoldCommand {
        inline MyNonManifoldCommand( int target ) : targetNonManifoldFaces( target ) {}
        inline void initialize( const TriangleMesh& /*triMesh*/ ) {}
        inline void process( const std::vector<TopologicalMesh::VertexHandle>& /*face_vhandles*/ ) {
            LOG( logINFO ) << "Non Manifold face found";
            nonManifoldFaces++;
        }
        inline void postProcess( TopologicalMesh& /*tm*/ ) {
            // Todo : For each non manifold face, remove the vertices that are not part of a face of
            // the topomesh For the test, this will reduce the mesh_2 to mesh1
            REQUIRE( nonManifoldFaces == targetNonManifoldFaces );
            LOG( logINFO ) << "Process non-manifold faces";
        }

        int nonManifoldFaces {0};
        const int targetNonManifoldFaces;
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

    auto testConverter =
        []( const TriangleMesh& mesh, const TriangleMesh& mesh2, MyNonManifoldCommand command ) {
            // test with functor
            LOG( logINFO ) << "Converter with custom command";
            TopologicalMesh topo2 {mesh2, command};
            auto mesh3 = topo2.toTriangleMesh();
            REQUIRE( isSameMesh( mesh, mesh3 ) );

            // test without functor
            LOG( logINFO ) << "Converter without custom command";
            TopologicalMesh topo3 {mesh2};
            auto mesh4 = topo3.toTriangleMesh();
            REQUIRE( isSameMesh( mesh, mesh4 ) );
            return mesh4;
        };

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
    testConverter( mesh, mesh2, MyNonManifoldCommand( 1 ) ); // we should find 1 non-manifold face

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
