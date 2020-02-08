#include <Core/Geometry/MeshPrimitives.hpp>
#include <Core/Geometry/TopologicalMesh.hpp>
#include <Core/Geometry/TriangleMesh.hpp>
#include <catch2/catch.hpp>

#include <OpenMesh/Tools/Subdivider/Uniform/CatmullClarkT.hh>
#include <OpenMesh/Tools/Subdivider/Uniform/LoopT.hh>

#include <OpenMesh/Tools/Decimater/DecimaterT.hh>
#include <OpenMesh/Tools/Decimater/ModQuadricT.hh>

using namespace Ra::Core;
using namespace Ra::Core::Geometry;

bool isSameMesh( Ra::Core::Geometry::TriangleMesh& meshOne,
                 Ra::Core::Geometry::TriangleMesh& meshTwo ) {

    using Ra::Core::Vector3;
    using Ra::Core::Geometry::TopologicalMesh;
    using Ra::Core::Geometry::TriangleMesh;

    bool result = true;
    int i       = 0;
    // Check length
    if ( meshOne.vertices().size() != meshTwo.vertices().size() ) return false;
    if ( meshOne.normals().size() != meshTwo.normals().size() ) return false;
    if ( meshOne.m_indices.size() != meshTwo.m_indices.size() ) return false;

    // Check triangles
    std::vector<Ra::Core::Vector3> stackVertices;
    std::vector<Ra::Core::Vector3> stackNormals;

    i = 0;
    while ( result && i < int( meshOne.m_indices.size() ) )
    {
        std::vector<Ra::Core::Vector3>::iterator it;
        stackVertices.clear();
        stackVertices.push_back( meshOne.vertices()[meshOne.m_indices[i][0]] );
        stackVertices.push_back( meshOne.vertices()[meshOne.m_indices[i][1]] );
        stackVertices.push_back( meshOne.vertices()[meshOne.m_indices[i][2]] );

        stackNormals.clear();
        stackNormals.push_back( meshOne.normals()[meshOne.m_indices[i][0]] );
        stackNormals.push_back( meshOne.normals()[meshOne.m_indices[i][1]] );
        stackNormals.push_back( meshOne.normals()[meshOne.m_indices[i][2]] );

        for ( int j = 0; j < 3; ++j )
        {
            it = find( stackVertices.begin(),
                       stackVertices.end(),
                       meshTwo.vertices()[meshTwo.m_indices[i][j]] );
            if ( it != stackVertices.end() ) { stackVertices.erase( it ); }
            else
            { result = false; }
        }

        for ( int j = 0; j < 3; ++j )
        {
            it = find( stackNormals.begin(),
                       stackNormals.end(),
                       meshTwo.normals()[meshTwo.m_indices[i][j]] );
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

    // return 1 : equals, 2: strict less, 3: strich greater
    int comp_vec( const Vector3& a, const Vector3& b ) const {
        if ( a == b ) return 1;
        if ( a[0] < b[0] || ( a[0] == b[0] && a[1] < b[1] ) ||
             ( a[0] == b[0] && a[1] == b[1] && a[2] < b[2] ) )
            return 2;
        return 3;
    }

    int comp_vec( const Vector2& a, const Vector2& b ) const {
        if ( a == b ) return 1;
        if ( a[0] < b[0] || ( a[0] == b[0] && a[1] < b[1] ) ) return 2;
        return 3;
    }
    int comp_vec( const Vector4& a, const Vector4& b ) const {
        if ( a == b ) return 1;
        if ( a[0] < b[0] || ( a[0] == b[0] && a[1] < b[1] ) ||
             ( a[0] == b[0] && a[1] == b[1] && a[2] < b[2] ) ||
             ( a[0] == b[0] && a[1] == b[1] && a[2] == b[2] && a[3] < b[3] ) )
            return 2;
        return 3;
    }

    bool operator<( const WedgeDataAndIdx& lhs ) const {
        {
            int comp = comp_vec( m_data.m_position, lhs.m_data.m_position );
            if ( comp == 2 ) return true;
            if ( comp == 3 ) return false;
        }
        for ( size_t i = 0; i < m_data.m_floatAttrib.size(); i++ )
        {
            if ( m_data.m_floatAttrib[i] < lhs.m_data.m_floatAttrib[i] )
                return true;
            else if ( m_data.m_floatAttrib[i] > lhs.m_data.m_floatAttrib[i] )
                return false;
        }

        for ( size_t i = 0; i < m_data.m_vector2Attrib.size(); i++ )
        {
            int comp = comp_vec( m_data.m_vector2Attrib[i], lhs.m_data.m_vector2Attrib[i] );
            if ( comp == 2 ) return true;
            if ( comp == 3 ) return false;
        }
        for ( size_t i = 0; i < m_data.m_vector3Attrib.size(); i++ )
        {
            int comp = comp_vec( m_data.m_vector3Attrib[i], lhs.m_data.m_vector3Attrib[i] );
            if ( comp == 2 ) return true;
            if ( comp == 3 ) return false;
        }
        for ( size_t i = 0; i < m_data.m_vector4Attrib.size(); i++ )
        {
            int comp = comp_vec( m_data.m_vector4Attrib[i], lhs.m_data.m_vector4Attrib[i] );
            if ( comp == 2 ) return true;
            if ( comp == 3 ) return false;
        }
        return false;
    }

    bool operator==( const WedgeDataAndIdx& lhs ) const { return !( *this != lhs ); }

    bool operator!=( const WedgeDataAndIdx& lhs ) const {
        {
            int comp = comp_vec( m_data.m_position, lhs.m_data.m_position );
            if ( comp == 2 ) return true;
            if ( comp == 3 ) return true;
        }
        for ( size_t i = 0; i < m_data.m_floatAttrib.size(); i++ )
        {
            if ( m_data.m_floatAttrib[i] < lhs.m_data.m_floatAttrib[i] )
                return true;
            else if ( m_data.m_floatAttrib[i] > lhs.m_data.m_floatAttrib[i] )
                return true;
        }

        for ( size_t i = 0; i < m_data.m_vector2Attrib.size(); i++ )
        {
            int comp = comp_vec( m_data.m_vector2Attrib[i], lhs.m_data.m_vector2Attrib[i] );
            if ( comp == 2 ) return true;
            if ( comp == 3 ) return true;
        }
        for ( size_t i = 0; i < m_data.m_vector3Attrib.size(); i++ )
        {
            int comp = comp_vec( m_data.m_vector3Attrib[i], lhs.m_data.m_vector3Attrib[i] );
            if ( comp == 2 ) return true;
            if ( comp == 3 ) return true;
        }
        for ( size_t i = 0; i < m_data.m_vector4Attrib.size(); i++ )
        {
            int comp = comp_vec( m_data.m_vector4Attrib[i], lhs.m_data.m_vector4Attrib[i] );
            if ( comp == 2 ) return true;
            if ( comp == 3 ) return true;
        }
        return false;
    }
};

#define COPY_TO_WEDGES_VECTOR_HELPER( UPTYPE, DOWNTYPE, REALTYPE )                            \
    if ( attr->is##UPTYPE() )                                                                 \
    {                                                                                         \
        auto data =                                                                           \
            meshOne.getAttrib( meshOne.getAttribHandle<REALTYPE>( attr->getName() ) ).data(); \
        for ( size_t i = 0; i < size; ++i )                                                   \
        {                                                                                     \
            wedgesMeshOne[i].m_data.m_##DOWNTYPE##Attrib.push_back( data[i] );                \
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
        auto data = meshOne.vertices();
        for ( size_t i = 0; i < size; ++i )
        {
            wedgesMeshOne[i].m_data.m_position = data[i];
        }
    }
    if ( attr->getName() != std::string( "in_position" ) )
    {
        COPY_TO_WEDGES_VECTOR_HELPER( Float, float, float );
        COPY_TO_WEDGES_VECTOR_HELPER( Vector2, vector2, Vector2 );
        COPY_TO_WEDGES_VECTOR_HELPER( Vector3, vector3, Vector3 );
        COPY_TO_WEDGES_VECTOR_HELPER( Vector4, vector4, Vector4 );
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
    if ( meshOne.m_indices.size() != meshTwo.m_indices.size() ) return false;

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

    auto indices1 = meshOne.m_indices;
    auto indices2 = meshTwo.m_indices;

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

    using Catmull =
        OpenMesh::Subdivider::Uniform::CatmullClarkT<Ra::Core::Geometry::TopologicalMesh>;
    using Loop = OpenMesh::Subdivider::Uniform::LoopT<Ra::Core::Geometry::TopologicalMesh>;

    using Decimater = OpenMesh::Decimater::DecimaterT<Ra::Core::Geometry::TopologicalMesh>;
    using HModQuadric =
        OpenMesh::Decimater::ModQuadricT<Ra::Core::Geometry::TopologicalMesh>::Handle;

    TriangleMesh newMesh;
    TriangleMesh newMesh2;
    TriangleMesh mesh;
    TopologicalMesh topologicalMesh;

    // Test for close mesh
    mesh            = Ra::Core::Geometry::makeBox();
    topologicalMesh = TopologicalMesh( mesh );
    newMesh         = topologicalMesh.toTriangleMesh();
    newMesh2        = topologicalMesh.toTriangleMeshFromWedges();
    REQUIRE( isSameMesh( mesh, newMesh ) );
    REQUIRE( isSameMesh( mesh, newMesh2 ) );
    REQUIRE( isSameMeshWedge( mesh, newMesh2 ) );

    mesh            = Ra::Core::Geometry::makeSharpBox();
    topologicalMesh = TopologicalMesh( mesh );
    newMesh         = topologicalMesh.toTriangleMesh();
    newMesh2        = topologicalMesh.toTriangleMeshFromWedges();
    REQUIRE( isSameMesh( mesh, newMesh ) );
    REQUIRE( isSameMesh( mesh, newMesh2 ) );
    REQUIRE( isSameMeshWedge( mesh, newMesh2 ) );

    // Test for mesh with boundaries
    mesh            = Ra::Core::Geometry::makePlaneGrid( 2, 2 );
    topologicalMesh = TopologicalMesh( mesh );
    newMesh         = topologicalMesh.toTriangleMesh();
    newMesh2        = topologicalMesh.toTriangleMeshFromWedges();
    REQUIRE( isSameMesh( mesh, newMesh ) );
    REQUIRE( isSameMesh( mesh, newMesh2 ) );
    REQUIRE( isSameMeshWedge( mesh, newMesh2 ) );

    mesh = Ra::Core::Geometry::makeCylinder( Vector3( 0, 0, 0 ), Vector3( 0, 0, 1 ), 1 );

    topologicalMesh = TopologicalMesh( mesh );
    newMesh         = topologicalMesh.toTriangleMesh();
    newMesh2        = topologicalMesh.toTriangleMeshFromWedges();
    REQUIRE( isSameMesh( mesh, newMesh ) );
    REQUIRE( isSameMesh( mesh, newMesh2 ) );
    REQUIRE( isSameMeshWedge( mesh, newMesh2 ) );

    // Test skip empty attributes
    mesh.addAttrib<float>( "empty" );
    topologicalMesh = TopologicalMesh( mesh );
    newMesh         = topologicalMesh.toTriangleMesh();
    REQUIRE( !newMesh.hasAttrib( "empty" ) );

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
    }
}
