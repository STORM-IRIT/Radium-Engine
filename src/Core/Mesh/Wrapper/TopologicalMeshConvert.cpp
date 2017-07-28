#include <Core/Mesh/Wrapper/TopologicalMeshConvert.hpp>
#include <Core/Log/Log.hpp>

#include <unordered_map>

namespace Ra {
namespace Core {

    void MeshConverter::convert(TopologicalMesh& in, TriangleMesh& out){

        struct vertexData{
            Vector3 _vertex;
            Vector3 _normal;
        };

        struct comp_vec{
            bool operator()(const vertexData &lhv, const vertexData &rhv) const {
                if( lhv._vertex[0]<rhv._vertex[0] || (lhv._vertex[0]==rhv._vertex[0] &&
                    lhv._vertex[1]<rhv._vertex[1] ) || (lhv._vertex[0]==rhv._vertex[0] &&
                    lhv._vertex[1]==rhv._vertex[1] && lhv._vertex[2]<rhv._vertex[2]))
                {
                    return true;
                }
            }
        };

        out.clear();
        typedef std::map<vertexData, int, comp_vec> vMap;

        vMap vertexHandles;

        //in.garbage_collection();
        in.request_face_normals();
        in.request_vertex_normals();
        in.update_vertex_normals();
//        in.update_face_normals();
//        in.update_halfedge_normals(M_PI);
//        in.release_face_normals();

        // iterator over all faces
        unsigned int vertexIndex = 0;
        // Use skip iterators
        for (TopologicalMesh::FaceIter f_it=in.faces_sbegin(); f_it!=in.faces_end(); ++f_it){
            vertexData v;
            int indices[3];
            int i=0;
            // iterator over vertex (thru haldedge to get access to halfedge normals)
            for(TopologicalMesh::FaceHalfedgeIter fv_it = in.fh_iter(*f_it); fv_it.is_valid(); ++fv_it){
                assert(i<3);
                TopologicalMesh::Point p = in.point(in.to_vertex_handle(*fv_it));
                TopologicalMesh::Normal n = in.normal(in.to_vertex_handle(*fv_it));
                v._vertex = Core::Vector3(p[0], p[1], p[2]);
                v._normal = Core::Vector3(n[0], n[1], n[2]);

                int vi;
                vMap::iterator vtr = vertexHandles.find(v);
                if(vtr == vertexHandles.end()){
                    vi = vertexIndex++;
                    vertexHandles.insert( vtr, vMap::value_type(v, vi));
                    //meshVertices.push_back(v);
                    out.m_vertices.push_back(v._vertex);
                    out.m_normals.push_back(v._normal);
                }
                else{
                    vi = vtr->second;
                }
                indices[i] = vi;
                i ++;
            }
            out.m_triangles.push_back(Triangle(indices[0],indices[1],indices[2]));
        }
        assert(vertexIndex == out.m_vertices.size());

    }

    void MeshConverter::convert(const TriangleMesh& in, TopologicalMesh& out){

        struct hash_vec{
            size_t operator()(const Vector3 &lvalue) const{
                return lvalue[0]+lvalue[1]+lvalue[2]+
                        floor(lvalue[0])*1000.f+
                        floor(lvalue[1])*1000.f+
                        floor(lvalue[2])*1000.f;
            }
        };

        //Delete old data in out mesh
        out = TopologicalMesh();
        out.garbage_collection();
        out.request_vertex_normals();
        //typedef std::unordered_map<Core::Vector3f, TopologicalMesh::VertexHandle, comp_vec> vMap;
        typedef std::unordered_map<Vector3, TopologicalMesh::VertexHandle, hash_vec> vMap;
        vMap vertexHandles;

        std::vector<TopologicalMesh::VertexHandle> face_vhandles;

        uint num_halfedge = in.m_triangles.size() * 3;
        for(unsigned int i=0; i<num_halfedge; i++){
            Vector3 p = in.m_vertices[in.m_triangles[i/3][i%3]];
            Vector3 n = in.m_normals[in.m_triangles[i/3][i%3]];

            vMap::iterator vtr = vertexHandles.find(p);
            TopologicalMesh::VertexHandle vh;
            if(vtr == vertexHandles.end()){

                vh = out.add_vertex( TopologicalMesh::Point(p[0], p[1], p[2]));
                vertexHandles.insert( vtr, vMap::value_type(p, vh) );
                out.set_normal(vh,TopologicalMesh::Normal(n[0], n[1], n[2]));
            }
            else{
                vh = vtr->second;
            }
            face_vhandles.push_back(vh);

            if(((i+1)%3)==0){
                out.add_face(face_vhandles);
                face_vhandles.clear();
            }

        }
        assert(out.n_faces() == num_halfedge/3);
    }

    /// Particular conversion for a progressive mesh
    /// since some faces doesn't exist anymore
//    void convertPM( TopologicalMesh& topologicalMesh, TriangleMesh& mesh )
//    {

//        // TODO !!!

//        const uint v_size = dcel.m_vertex.size();
//        const uint f_size = dcel.m_face.size();
//    //    mesh.m_vertices.resize( v_size );  // ce n'est pas le bon nombre de sommet
//    //    mesh.m_normals.resize( v_size );   // ce n'est pas le bon nombre de sommet
//    //    mesh.m_triangles.resize( f_size ); // ce n'est pas le bon nombre de face
//        //mesh.m_vertices.erase(mesh.m_vertices.begin(), mesh.m_vertices.end());
//        //mesh.m_normals.erase(mesh.m_normals.begin(), mesh.m_normals.end());
//        //mesh.m_triangles.erase(mesh.m_triangles.begin(), mesh.m_triangles.end());
//        std::map< Index, uint > v_table;
//        for( uint i = 0; i < v_size; ++i ) {

//            const Vertex_ptr& v = dcel.m_vertex.at( i );

//            if (v->HE() == NULL) //meaning the vertex is deleted
//                continue;

//            const Vector3 p = v->P();
//            const Vector3 n = v->N();
//            mesh.m_vertices.push_back(p);
//            mesh.m_normals.push_back(n);
//            v_table[ v->idx ] = mesh.m_vertices.size() - 1;
//        }
//        for( uint i = 0; i < f_size; ++i ) {
//            const Face_ptr& f = dcel.m_face.at( i );

//            if (f->HE() == NULL) //meaning the face is deleted
//                continue;

//            Triangle T;
//            T[0] = v_table[ f->HE()->V()->idx ];
//            T[1] = v_table[ f->HE()->Next()->V()->idx ];
//            T[2] = v_table[ f->HE()->Prev()->V()->idx ];
//            mesh.m_triangles.push_back(T);

//        }
//    }
}
}
