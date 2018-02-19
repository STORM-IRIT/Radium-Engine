#include <Core/Mesh/Wrapper/TopologicalMeshConvert.hpp>
#include <Core/Log/Log.hpp>

#include <unordered_map>

namespace Ra
{
    namespace Core
    {

        void MeshConverter::convert( TopologicalMesh& in, TriangleMesh& out )
        {

            struct vertexData
            {
                Vector3 _vertex;
                Vector3 _normal;
            };

            struct comp_vec
            {
                bool operator()( const vertexData& lhv, const vertexData& rhv ) const
                {
                    if ( lhv._vertex[0] < rhv._vertex[0] || ( lhv._vertex[0] == rhv._vertex[0] &&
                                                              lhv._vertex[1] < rhv._vertex[1] ) || ( lhv._vertex[0] == rhv._vertex[0] &&
                                                                                                     lhv._vertex[1] == rhv._vertex[1] && lhv._vertex[2] < rhv._vertex[2] ) )
                    {
                        return true;
                    }
                    return false;
                }
            };

            out.clear();
            typedef std::map<vertexData, int, comp_vec> vMap;

            vMap vertexHandles;

            in.request_face_normals();
            in.request_vertex_normals();
            in.update_vertex_normals();

            // iterator over all faces
            unsigned int vertexIndex = 0;

            // out will have at least least n_vertices and n_normals.
            out.m_vertices.reserve( in.n_vertices() );
            out.m_normals.reserve( in.n_vertices() );
            out.m_triangles.reserve( in.n_faces() );
            
            for ( TopologicalMesh::FaceIter f_it = in.faces_sbegin(); f_it != in.faces_end(); ++f_it )
            {
                vertexData v;
                int indices[3];
                int i = 0;
                // iterator over vertex (thru halfedge to get access to halfedge normals)
                for ( TopologicalMesh::FaceHalfedgeIter fv_it = in.fh_iter( *f_it ); fv_it.is_valid(); ++fv_it )
                {
                    assert( i < 3 );
                    TopologicalMesh::Point p = in.point( in.to_vertex_handle( *fv_it ) );
                    TopologicalMesh::Normal n = in.normal( in.to_vertex_handle( *fv_it ) );
                    v._vertex = p;
                    v._normal = n;

                    int vi;
                    vMap::iterator vtr = vertexHandles.find( v );
                    if ( vtr == vertexHandles.end() )
                    {
                        vi = vertexIndex++;
                        vertexHandles.insert( vtr, vMap::value_type( v, vi ) );
                        out.m_vertices.push_back( v._vertex );
                        out.m_normals.push_back( v._normal );
                    }
                    else
                    {
                        vi = vtr->second;
                    }
                    indices[i] = vi;
                    i ++;
                }
                out.m_triangles.emplace_back( indices[0], indices[1], indices[2] );
            }
            assert( vertexIndex == out.m_vertices.size() );

        }

        void MeshConverter::convert( const TriangleMesh& in, TopologicalMesh& out )
        {

            struct hash_vec
            {
                size_t operator()( const Vector3& lvalue ) const
                {
                    return lvalue[0] + lvalue[1] + lvalue[2] +
                           floor( lvalue[0] ) * 1000.f +
                           floor( lvalue[1] ) * 1000.f +
                           floor( lvalue[2] ) * 1000.f;
                }
            };

            //Delete old data in out mesh
            out = TopologicalMesh();
            out.garbage_collection();
            out.request_vertex_normals();
            typedef std::unordered_map<Vector3, TopologicalMesh::VertexHandle, hash_vec> vMap;
            vMap vertexHandles;

            std::vector<TopologicalMesh::VertexHandle> face_vhandles;

            uint num_halfedge = in.m_triangles.size() * 3;
            for ( unsigned int i = 0; i < num_halfedge; i++ )
            {
                Vector3 p = in.m_vertices[in.m_triangles[i / 3][i % 3]];
                Vector3 n = in.m_normals[in.m_triangles[i / 3][i % 3]];

                vMap::iterator vtr = vertexHandles.find( p );
                TopologicalMesh::VertexHandle vh;
                if ( vtr == vertexHandles.end() )
                {
                    vh = out.add_vertex( p );
                    vertexHandles.insert( vtr, vMap::value_type( p, vh ) );
                    out.set_normal( vh, n );
                }
                else
                {
                    vh = vtr->second;
                }
                face_vhandles.push_back( vh );

                if ( ( ( i + 1 ) % 3 ) == 0 )
                {
                    out.add_face( face_vhandles );
                    face_vhandles.clear();
                }

            }
            assert( out.n_faces() == num_halfedge / 3 );
        }

    }
}
