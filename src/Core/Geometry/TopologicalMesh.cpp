#include <Core/Geometry/TopologicalMesh.hpp>

#include <Core/RaCore.hpp>
#include <Core/Utils/Log.hpp>

#include <Eigen/StdVector>

#include <unordered_map>
#include <utility>
#include <vector>

namespace Ra {
namespace Core {
namespace Geometry {

using namespace Utils; // log, AttribXXX

template <typename T>
using PropPair = std::pair<AttribHandle<T>, OpenMesh::HPropHandleT<T>>;

template <typename T>
void addAttribPairToTopo( const TriangleMesh& triMesh,
                          TopologicalMesh* topoMesh,
                          AttribManager::pointer_type attr,
                          std::vector<PropPair<T>>& vprop,
                          std::vector<OpenMesh::HPropHandleT<T>>& pph ) {
    AttribHandle<T> h = triMesh.getAttribHandle<T>( attr->getName() );
    if ( attr->getSize() == triMesh.vertices().size() )
    {
        OpenMesh::HPropHandleT<T> oh;
        topoMesh->add_property( oh, attr->getName() );
        vprop.push_back( std::make_pair( h, oh ) );
        pph.push_back( oh );
    }
    else
    {
        LOG( logWARNING ) << "[TopologicalMesh] Skip badly sized attribute " << attr->getName()
                          << ".";
    }
}

template <typename T>
void addAttribPairToCore( TriangleMesh& triMesh,
                          const TopologicalMesh* topoMesh,
                          OpenMesh::HPropHandleT<T> oh,
                          std::vector<PropPair<T>>& vprop ) {
    AttribHandle<T> h = triMesh.addAttrib<T>( topoMesh->property( oh ).name() );
    vprop.push_back( std::make_pair( h, oh ) );
}

template <typename T>
void copyAttribToTopo( const TriangleMesh& triMesh,
                       TopologicalMesh* topoMesh,
                       const std::vector<PropPair<T>>& vprop,
                       TopologicalMesh::HalfedgeHandle heh,
                       unsigned int vindex ) {
    for ( auto pp : vprop )
    {
        topoMesh->property( pp.second, heh ) = triMesh.getAttrib( pp.first ).data()[vindex];
    }
}

template <typename T>
using HandleAndValueVector = std::vector<std::pair<AttribHandle<T>, T>,
                                         Eigen::aligned_allocator<std::pair<AttribHandle<T>, T>>>;

template <typename T>
void copyAttribToCoreVertex( HandleAndValueVector<T>& data,
                             const TopologicalMesh* topoMesh,
                             const std::vector<PropPair<T>>& vprop,
                             TopologicalMesh::HalfedgeHandle heh ) {
    for ( auto pp : vprop )
    {
        data.push_back( std::make_pair( pp.first, topoMesh->property( pp.second, heh ) ) );
    }
}

template <typename T>
void copyAttribToCore( TriangleMesh& triMesh, const HandleAndValueVector<T>& data ) {

    for ( auto pp : data )
    {
        auto& attr     = triMesh.getAttrib( pp.first );
        auto& attrData = attr.getDataWithLock();
        attrData.push_back( pp.second );
        attr.unlock();
    }
}

TopologicalMesh::TopologicalMesh( const TriangleMesh& triMesh ) {
    struct hash_vec {
        size_t operator()( const Vector3& lvalue ) const {
            size_t hx = std::hash<Scalar>()( lvalue[0] );
            size_t hy = std::hash<Scalar>()( lvalue[1] );
            size_t hz = std::hash<Scalar>()( lvalue[2] );
            return ( hx ^ ( hy << 1 ) ) ^ hz;
        }
    };
    // use a hashmap for fast search of existing vertex position
    using VertexMap = std::unordered_map<Vector3, TopologicalMesh::VertexHandle, hash_vec>;
    VertexMap vertexHandles;

    add_property( m_inputTriangleMeshIndexPph );
    std::vector<PropPair<float>> vprop_float;
    std::vector<PropPair<Vector2>> vprop_vec2;
    std::vector<PropPair<Vector3>> vprop_vec3;
    std::vector<PropPair<Vector4>> vprop_vec4;

    // loop over all attribs and build correspondance pair
    triMesh.vertexAttribs().for_each_attrib(
        [&triMesh, this, &vprop_float, &vprop_vec2, &vprop_vec3, &vprop_vec4]( const auto& attr ) {
            // skip builtin attribs
            if ( attr->getName() != std::string( "in_position" ) &&
                 attr->getName() != std::string( "in_normal" ) )
            {
                if ( attr->isFloat() )
                    addAttribPairToTopo( triMesh, this, attr, vprop_float, m_floatPph );
                else if ( attr->isVec2() )
                    addAttribPairToTopo( triMesh, this, attr, vprop_vec2, m_vec2Pph );
                else if ( attr->isVec3() )
                    addAttribPairToTopo( triMesh, this, attr, vprop_vec3, m_vec3Pph );
                else if ( attr->isVec4() )
                    addAttribPairToTopo( triMesh, this, attr, vprop_vec4, m_vec4Pph );
                else
                    LOG( logWARNING )
                        << "Warning, mesh attribute " << attr->getName()
                        << " type is not supported (only float, vec2, vec3 nor vec4 are supported)";
            }
        } );

    size_t num_triangles = triMesh.m_indices.size();

    for ( unsigned int i = 0; i < num_triangles; i++ )
    {
        std::vector<TopologicalMesh::VertexHandle> face_vhandles( 3 );
        std::vector<TopologicalMesh::Normal> face_normals( 3 );
        std::vector<unsigned int> face_vertexIndex( 3 );

        const auto& triangle = triMesh.m_indices[i];
        for ( size_t j = 0; j < 3; ++j )
        {
            unsigned int inMeshVertexIndex = triangle[j];
            const Vector3& p               = triMesh.vertices()[inMeshVertexIndex];
            const Vector3& n               = triMesh.normals()[inMeshVertexIndex];

            VertexMap::iterator vtr = vertexHandles.find( p );

            TopologicalMesh::VertexHandle vh;
            if ( vtr == vertexHandles.end() )
            {
                vh = add_vertex( p );
                vertexHandles.insert( vtr, VertexMap::value_type( p, vh ) );
            }
            else
            { vh = vtr->second; }

            face_vhandles[j]    = vh;
            face_normals[j]     = n;
            face_vertexIndex[j] = inMeshVertexIndex;
        }

        // Add the face, then add attribs to vh
        TopologicalMesh::FaceHandle fh = add_face( face_vhandles );

        for ( size_t vindex = 0; vindex < face_vhandles.size(); vindex++ )
        {
            TopologicalMesh::HalfedgeHandle heh = halfedge_handle( face_vhandles[vindex], fh );
            set_normal( heh, face_normals[vindex] );
            property( m_inputTriangleMeshIndexPph, heh ) = face_vertexIndex[vindex];
            copyAttribToTopo( triMesh, this, vprop_float, heh, face_vertexIndex[vindex] );
            copyAttribToTopo( triMesh, this, vprop_vec2, heh, face_vertexIndex[vindex] );
            copyAttribToTopo( triMesh, this, vprop_vec3, heh, face_vertexIndex[vindex] );
            copyAttribToTopo( triMesh, this, vprop_vec4, heh, face_vertexIndex[vindex] );
        }

        face_vhandles.clear();
        face_normals.clear();
        face_vertexIndex.clear();
    }
}

TriangleMesh TopologicalMesh::toTriangleMesh() {
    struct VertexDataInternal {
        Vector3 _vertex;
        Vector3 _normal;

        HandleAndValueVector<float> _float;
        HandleAndValueVector<Vector2> _vec2;
        HandleAndValueVector<Vector3> _vec3;
        HandleAndValueVector<Vector4> _vec4;

        EIGEN_MAKE_ALIGNED_OPERATOR_NEW

        bool operator==( const VertexDataInternal& lhs ) const {
            return _vertex == lhs._vertex && _normal == lhs._normal && _float == lhs._float &&
                   _vec2 == lhs._vec2 && _vec3 == lhs._vec3 && _vec4 == lhs._vec4;
        }
    };

    struct hash_vec {
        size_t operator()( const VertexDataInternal& lvalue ) const {
            size_t hx = std::hash<Scalar>()( lvalue._vertex[0] );
            size_t hy = std::hash<Scalar>()( lvalue._vertex[1] );
            size_t hz = std::hash<Scalar>()( lvalue._vertex[2] );
            return ( hx ^ ( hy << 1 ) ) ^ hz;
        }
    };

    TriangleMesh out;

    using VertexMap = std::unordered_map<VertexDataInternal, int, hash_vec>;

    VertexMap vertexHandles;

    if ( !get_property_handle( m_outputTriangleMeshIndexPph, "OutputTriangleMeshIndices" ) )
    { add_property( m_outputTriangleMeshIndexPph, "OutputTriangleMeshIndices" ); }
    std::vector<PropPair<float>> vprop_float;
    std::vector<PropPair<Vector2>> vprop_vec2;
    std::vector<PropPair<Vector3>> vprop_vec3;
    std::vector<PropPair<Vector4>> vprop_vec4;

    // loop over all attribs and build correspondance pair
    vprop_float.reserve( m_floatPph.size() );
    for ( auto oh : m_floatPph )
        addAttribPairToCore( out, this, oh, vprop_float );
    vprop_vec2.reserve( m_vec2Pph.size() );
    for ( auto oh : m_vec2Pph )
        addAttribPairToCore( out, this, oh, vprop_vec2 );
    vprop_vec3.reserve( m_vec3Pph.size() );
    for ( auto oh : m_vec3Pph )
        addAttribPairToCore( out, this, oh, vprop_vec3 );
    vprop_vec4.reserve( m_vec4Pph.size() );
    for ( auto oh : m_vec4Pph )
        addAttribPairToCore( out, this, oh, vprop_vec4 );

    // iterator over all faces
    unsigned int vertexIndex = 0;

    // out will have at least n_vertices vertices and normals.
    TriangleMesh::PointAttribHandle::Container vertices;
    TriangleMesh::NormalAttribHandle::Container normals;

    vertices.reserve( n_vertices() );
    normals.reserve( n_vertices() );
    out.m_indices.reserve( n_faces() );

    for ( TopologicalMesh::FaceIter f_it = faces_sbegin(); f_it != faces_end(); ++f_it )
    {
        int indices[3];
        int i = 0;

        // iterator over vertex (through halfedge to get access to halfedge normals)
        for ( TopologicalMesh::ConstFaceHalfedgeIter fh_it = cfh_iter( *f_it ); fh_it.is_valid();
              ++fh_it )
        {
            VertexDataInternal v;
            CORE_ASSERT( i < 3, "Non-triangular face found." );
            v._vertex = point( to_vertex_handle( *fh_it ) );
            v._normal = normal( to_vertex_handle( *fh_it ), *f_it );

            copyAttribToCoreVertex( v._float, this, vprop_float, *fh_it );
            copyAttribToCoreVertex( v._vec2, this, vprop_vec2, *fh_it );
            copyAttribToCoreVertex( v._vec3, this, vprop_vec3, *fh_it );
            copyAttribToCoreVertex( v._vec4, this, vprop_vec4, *fh_it );

            int vi;
            VertexMap::iterator vtr = vertexHandles.find( v );
            if ( vtr == vertexHandles.end() )
            {
                vi = int( vertexIndex++ );
                vertexHandles.insert( vtr, VertexMap::value_type( v, vi ) );
                vertices.push_back( v._vertex );
                normals.push_back( v._normal );

                copyAttribToCore( out, v._float );
                copyAttribToCore( out, v._vec2 );
                copyAttribToCore( out, v._vec3 );
                copyAttribToCore( out, v._vec4 );
            }
            else
            { vi = vtr->second; }
            indices[i]                                       = vi;
            property( m_outputTriangleMeshIndexPph, *fh_it ) = vi;
            i++;
        }
        out.m_indices.emplace_back( indices[0], indices[1], indices[2] );
    }
    out.setVertices( vertices );
    out.setNormals( normals );
    CORE_ASSERT( vertexIndex == vertices.size(),
                 "Inconsistent number of faces in generated TriangleMesh." );

    return out;
}

void TopologicalMesh::updateTriangleMesh( Ra::Core::Geometry::TriangleMesh& /*mesh*/ ) {
    ///\todo ;)
}

bool TopologicalMesh::splitEdge( TopologicalMesh::EdgeHandle eh, Scalar f ) {
    // Global schema of operation
    /*
               TRIANGLES ONLY
         before                after
               A                       A
            / F0 \                / F2 | F0 \
           /      \              /     |     \
          /h1    h0\            /h1  e2|e0  h0\
         /    he0   \          /  he2  |  he0  \
        V1 -------- V0       V1 ------ V ------ V0
         \    he1   /          \  he3  |  he1  /
          \o1    o0/            \o1  e3|e1  o0/
           \      /              \     |     /
            \ F1 /                \ F3 | F1 /
               B                       B

    */

    // incorrect factor
    if ( f < 0 || f > 1 ) { return false; }

    // get existing topology data
    HalfedgeHandle he0 = halfedge_handle( eh, 0 );
    HalfedgeHandle he1 = halfedge_handle( eh, 1 );
    VertexHandle v0    = to_vertex_handle( he0 );
    VertexHandle v1    = to_vertex_handle( he1 );
    FaceHandle F0      = face_handle( he0 );
    FaceHandle F1      = face_handle( he1 );

    // not triangles or holes
    if ( ( !is_boundary( he0 ) && valence( F0 ) != 3 ) ||
         ( !is_boundary( he1 ) && valence( F1 ) != 3 ) )
    { return false; } // add the new vertex
    const Point p  = Point( f * point( v0 ) + ( Scalar( 1. ) - f ) * point( v1 ) );
    VertexHandle v = add_vertex( p );

    // create the new faces and reconnect the topology
    HalfedgeHandle he3 = new_edge( v, v1 );
    HalfedgeHandle he2 = opposite_halfedge_handle( he3 );
    set_halfedge_handle( v, he0 );
    set_vertex_handle( he1, v );

    // does F0 exist
    if ( !is_boundary( he0 ) )
    {
        HalfedgeHandle h0 = next_halfedge_handle( he0 );
        HalfedgeHandle h1 = next_halfedge_handle( h0 );
        // create new edge
        VertexHandle A    = to_vertex_handle( h0 );
        HalfedgeHandle e2 = new_edge( v, A );
        HalfedgeHandle e0 = opposite_halfedge_handle( e2 );
        // split F0
        FaceHandle F2 = new_face();
        set_halfedge_handle( F0, he0 );
        set_halfedge_handle( F2, h1 );
        // update F0
        set_face_handle( h0, F0 );
        set_face_handle( e0, F0 );
        set_face_handle( he0, F0 );
        set_next_halfedge_handle( he0, h0 );
        set_next_halfedge_handle( h0, e0 );
        set_next_halfedge_handle( e0, he0 );
        // update F2
        set_face_handle( h1, F2 );
        set_face_handle( he2, F2 );
        set_face_handle( e2, F2 );
        set_next_halfedge_handle( e2, h1 );
        set_next_halfedge_handle( h1, he2 );
        set_next_halfedge_handle( he2, e2 );
        // deal with custom properties
        // interpolate at he2
        interpolateAllProps( h1, he0, he2, 0.5 );
        // copy at e0, and e2
        copyAllProps( he2, e0 );
    }
    else
    {
        HalfedgeHandle h1 = prev_halfedge_handle( he0 );
        set_next_halfedge_handle( h1, he2 );
        set_next_halfedge_handle( he2, he0 );
        // next halfedge handle of he0 already is h0
        // halfedge handle of V already is he0
    }

    // does F1 exist
    if ( !is_boundary( he1 ) )
    {
        HalfedgeHandle o1 = next_halfedge_handle( he1 );
        HalfedgeHandle o0 = next_halfedge_handle( o1 );
        // create new edge
        VertexHandle B    = to_vertex_handle( o1 );
        HalfedgeHandle e1 = new_edge( v, B );
        HalfedgeHandle e3 = opposite_halfedge_handle( e1 );
        // split F1
        FaceHandle F3 = new_face();
        set_halfedge_handle( F3, o1 );
        set_halfedge_handle( F1, he1 );
        // update F1
        set_face_handle( o1, F3 );
        set_face_handle( e3, F3 );
        set_face_handle( he3, F3 );
        set_next_halfedge_handle( he3, o1 );
        set_next_halfedge_handle( o1, e3 );
        set_next_halfedge_handle( e3, he3 );
        // update F3
        set_face_handle( o0, F1 );
        set_face_handle( he1, F1 );
        set_face_handle( e1, F1 );
        set_next_halfedge_handle( he1, e1 );
        set_next_halfedge_handle( e1, o0 );
        set_next_halfedge_handle( o0, he1 );
        // deal with custom properties
        // first copy at he3
        copyAllProps( he1, he3 );
        // interpolate at he1
        interpolateAllProps( o0, he3, he1, 0.5 );
        // copy at e1, and e3
        copyAllProps( he1, e3 );
        copyAllProps( o1, e1 );
    }
    else
    {
        HalfedgeHandle o1 = next_halfedge_handle( he1 );
        // next halfedge handle of o0 already is he1
        set_next_halfedge_handle( he1, he3 );
        set_next_halfedge_handle( he3, o1 );
        // halfedge handle of V already is he0
    }

    // ensure consistency at v1
    if ( halfedge_handle( v1 ) == he0 ) { set_halfedge_handle( v1, he2 ); }

    return true;
}

} // namespace Geometry
} // namespace Core
} // namespace Ra
