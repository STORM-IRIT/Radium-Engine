#include <Core/RaCore.hpp>

#include <unordered_map>
#include <Core/Mesh/TopologicalTriMesh/TopologicalMesh.hpp>

namespace Ra {
namespace Core {

TopologicalMesh::TopologicalMesh( const TriangleMesh& triMesh ) {
    struct hash_vec {
        size_t operator()( const Vector3& lvalue ) const {
            return lvalue[0] + lvalue[1] + lvalue[2] + floor( lvalue[0] ) * 1000.f +
                   floor( lvalue[1] ) * 1000.f + floor( lvalue[2] ) * 1000.f;
        }
    };

    // Delete old data in out mesh
    using vMap = std::unordered_map<Vector3, TopologicalMesh::VertexHandle, hash_vec>;
    vMap vertexHandles;

    std::vector<TopologicalMesh::VertexHandle> face_vhandles;
    std::vector<TopologicalMesh::Normal> face_normals;
    ///\todo handle other TriangleMesh attribs.

    uint num_halfedge = triMesh.m_triangles.size() * 3;
    for ( unsigned int i = 0; i < num_halfedge; i++ )
    {
        Vector3 p = triMesh.vertices()[triMesh.m_triangles[i / 3][i % 3]];
        Vector3 n = triMesh.normals()[triMesh.m_triangles[i / 3][i % 3]];

        vMap::iterator vtr = vertexHandles.find( p );
        TopologicalMesh::VertexHandle vh;
        if ( vtr == vertexHandles.end() )
        {
            vh = this->add_vertex( p );
            vertexHandles.insert( vtr, vMap::value_type( p, vh ) );
            this->set_normal( vh, TopologicalMesh::Normal( n[0], n[1], n[2] ) );
        }
        else
        { vh = vtr->second; }

        face_vhandles.push_back( vh );
        face_normals.push_back( n );

        ///\todo also consider non triangular faces
        if ( ( ( i + 1 ) % 3 ) == 0 )
        {
            TopologicalMesh::FaceHandle fh = this->add_face( face_vhandles );
            for ( int vindex = 0; vindex < face_vhandles.size(); vindex++ )
            {
                TopologicalMesh::HalfedgeHandle heh =
                    this->halfedge_handle( face_vhandles[vindex], fh );
                this->property( this->halfedge_normals_pph(), heh ) = face_normals[vindex];
            }

            face_vhandles.clear();
        }
    }
    ///\todo also consider non triangular faces
    CORE_ASSERT( this->n_faces() == num_halfedge / 3,
                 "Inconsistent number of faces in generated TopologicalMesh." );
}


TriangleMesh TopologicalMesh::toTriangleMesh() {
    struct vertexData {
        Vector3 _vertex;
        Vector3 _normal;

        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    };

    struct comp_vec {
        bool operator()( const vertexData& lhv, const vertexData& rhv ) const {
            if ( lhv._vertex[0] < rhv._vertex[0] ||
                 ( lhv._vertex[0] == rhv._vertex[0] && lhv._vertex[1] < rhv._vertex[1] ) ||
                 ( lhv._vertex[0] == rhv._vertex[0] && lhv._vertex[1] == rhv._vertex[1] &&
                   lhv._vertex[2] < rhv._vertex[2] ) )
            {
                return true;
            }
            return false;
        }
    };

    TriangleMesh out;
    using vMap = std::map<vertexData, int, comp_vec,
                          Eigen::aligned_allocator<std::pair<const vertexData, int>>>;

    vMap vertexHandles;

    request_face_normals();
    request_vertex_normals();
    update_vertex_normals();

    // iterator over all faces
    unsigned int vertexIndex = 0;

    // out will have at least least n_vertices and n_normals.
    out.vertices().reserve( n_vertices() );
    out.normals().reserve( n_vertices() );
    out.m_triangles.reserve( n_faces() );

    for ( TopologicalMesh::FaceIter f_it = faces_sbegin(); f_it != faces_end(); ++f_it )
    {
        vertexData v;
        int indices[3];
        int i = 0;
        // iterator over vertex (thru halfedge to get access to halfedge normals)
        for ( TopologicalMesh::FaceHalfedgeIter fv_it = fh_iter( *f_it ); fv_it.is_valid();
              ++fv_it )
        {
            CORE_ASSERT( i < 3, "Non-triangular face found." );
            TopologicalMesh::Point p = point( to_vertex_handle( *fv_it ) );
            TopologicalMesh::Normal n = normal( to_vertex_handle( *fv_it ), *f_it );
            v._vertex = p;
            v._normal = n;

            int vi;
            vMap::iterator vtr = vertexHandles.find( v );
            if ( vtr == vertexHandles.end() )
            {
                vi = vertexIndex++;
                vertexHandles.insert( vtr, vMap::value_type( v, vi ) );
                out.vertices().push_back( v._vertex );
                out.normals().push_back( v._normal );
            }
            else
            { vi = vtr->second; }
            indices[i] = vi;
            i++;
        }
        out.m_triangles.emplace_back( indices[0], indices[1], indices[2] );
    }
    CORE_ASSERT( vertexIndex == out.vertices().size(),
                 "Inconsistent number of faces in generated TriangleMesh." );

    return out;
}
}
}
