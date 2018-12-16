#include <Core/Mesh/TopologicalTriMesh/TopologicalMesh.hpp>

#include <Core/Log/Log.hpp>
#include <Core/RaCore.hpp>

#include <Eigen/StdVector>

#include <unordered_map>
#include <utility>
#include <vector>

namespace Ra {
namespace Core {

template <typename T>
using PropPair = std::pair<Utils::AttribHandle<T>, OpenMesh::HPropHandleT<T>>;

template <typename T>
void addAttribPairToTopo( const TriangleMesh& triMesh, TopologicalMesh* topoMesh,
                          Utils::AttribManager::value_type attr, std::vector<PropPair<T>>& vprop,
                          std::vector<OpenMesh::HPropHandleT<T>>& pph ) {
    Utils::AttribHandle<T> h = triMesh.getAttribHandle<T>( attr->getName() );
    OpenMesh::HPropHandleT<T> oh;
    topoMesh->add_property( oh, attr->getName() );
    vprop.push_back( std::make_pair( h, oh ) );
    pph.push_back( oh );
}

template <typename T>
void addAttribPairToCore( TriangleMesh& triMesh, const TopologicalMesh* topoMesh,
                          OpenMesh::HPropHandleT<T> oh, std::vector<PropPair<T>>& vprop ) {
    Utils::AttribHandle<T> h = triMesh.addAttrib<T>( topoMesh->property( oh ).name() );
    vprop.push_back( std::make_pair( h, oh ) );
}

template <typename T>
void copyAttribToTopo( const TriangleMesh& triMesh, TopologicalMesh* topoMesh,
                       const std::vector<PropPair<T>>& vprop, TopologicalMesh::HalfedgeHandle heh,
                       unsigned int vindex ) {
    for ( auto pp : vprop )
    {
        topoMesh->property( pp.second, heh ) = triMesh.getAttrib( pp.first ).data()[vindex];
    }
}

template <typename T>
using HandleAndValueVector = std::vector<std::pair<Utils::AttribHandle<T>, T>,
                                         Eigen::aligned_allocator<std::pair<Utils::AttribHandle<T>, T>>>;

template <typename T>
void copyAttribToCoreVertex( HandleAndValueVector<T>& data, const TopologicalMesh* topoMesh,
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
        triMesh.getAttrib( pp.first ).data().push_back( pp.second );
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
    triMesh.m_vertexAttribs.for_each_attrib(
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

    uint num_triangles = triMesh.m_triangles.size();

    for ( unsigned int i = 0; i < num_triangles; i++ )
    {
        std::vector<TopologicalMesh::VertexHandle> face_vhandles( 3 );
        std::vector<TopologicalMesh::Normal> face_normals( 3 );
        std::vector<unsigned int> face_vertexIndex( 3 );

        const auto& triangle = triMesh.m_triangles[i];
        for ( int j = 0; j < 3; ++j )
        {
            unsigned int inMeshVertexIndex = triangle[j];
            const Vector3& p = triMesh.vertices()[inMeshVertexIndex];
            const Vector3& n = triMesh.normals()[inMeshVertexIndex];

            VertexMap::iterator vtr = vertexHandles.find( p );

            TopologicalMesh::VertexHandle vh;
            if ( vtr == vertexHandles.end() )
            {
                vh = add_vertex( p );
                vertexHandles.insert( vtr, VertexMap::value_type( p, vh ) );
            } else
            { vh = vtr->second; }

            face_vhandles[j] = vh;
            face_normals[j] = n;
            face_vertexIndex[j] = inMeshVertexIndex;
        }

        // Add the face, then add attribs to vh
        TopologicalMesh::FaceHandle fh = add_face( face_vhandles );

        for ( int vindex = 0; vindex < face_vhandles.size(); vindex++ )
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
    struct VertexData {
        Vector3 _vertex;
        Vector3 _normal;

        HandleAndValueVector<float> _float;
        HandleAndValueVector<Vector2> _vec2;
        HandleAndValueVector<Vector3> _vec3;
        HandleAndValueVector<Vector4> _vec4;

        EIGEN_MAKE_ALIGNED_OPERATOR_NEW

        bool operator==( const VertexData& lhs ) const {
            return _vertex == lhs._vertex && _normal == lhs._normal && _float == lhs._float &&
                   _vec2 == lhs._vec2 && _vec3 == lhs._vec3 && _vec4 == lhs._vec4;
        }
    };

    struct hash_vec {
        size_t operator()( const VertexData& lvalue ) const {
            size_t hx = std::hash<Scalar>()( lvalue._vertex[0] );
            size_t hy = std::hash<Scalar>()( lvalue._vertex[1] );
            size_t hz = std::hash<Scalar>()( lvalue._vertex[2] );
            return ( hx ^ ( hy << 1 ) ) ^ hz;
        }
    };

    TriangleMesh out;

    using VertexMap = std::unordered_map<VertexData, int, hash_vec>;

    VertexMap vertexHandles;

    if ( !get_property_handle( m_outputTriangleMeshIndexPph, "OutputTriangleMeshIndices" ) )
    {
        add_property( m_outputTriangleMeshIndexPph, "OutputTriangleMeshIndices" );
    }
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
    out.vertices().reserve( n_vertices() );
    out.normals().reserve( n_vertices() );
    out.m_triangles.reserve( n_faces() );

    for ( TopologicalMesh::FaceIter f_it = faces_sbegin(); f_it != faces_end(); ++f_it )
    {
        int indices[3];
        int i = 0;

        // iterator over vertex (through halfedge to get access to halfedge normals)
        for ( TopologicalMesh::ConstFaceHalfedgeIter fh_it = cfh_iter( *f_it ); fh_it.is_valid();
              ++fh_it )
        {
            VertexData v;
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
                vi = vertexIndex++;
                vertexHandles.insert( vtr, VertexMap::value_type( v, vi ) );
                out.vertices().push_back( v._vertex );
                out.normals().push_back( v._normal );

                copyAttribToCore( out, v._float );
                copyAttribToCore( out, v._vec2 );
                copyAttribToCore( out, v._vec3 );
                copyAttribToCore( out, v._vec4 );
            } else
            { vi = vtr->second; }
            indices[i] = vi;
            property( m_outputTriangleMeshIndexPph, *fh_it ) = vi;
            i++;
        }
        out.m_triangles.emplace_back( indices[0], indices[1], indices[2] );
    }

    CORE_ASSERT( vertexIndex == out.vertices().size(),
                 "Inconsistent number of faces in generated TriangleMesh." );

    return out;
}

} // namespace Core
} // namespace Ra
