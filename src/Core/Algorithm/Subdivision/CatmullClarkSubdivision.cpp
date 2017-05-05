#include <Core/Algorithm/Subdivision/CatmullClarkSubdivision.hpp>

namespace Ra {
namespace Core {
namespace Algorithm {

    typedef std::map< HalfEdgeIdx, VertexIdx > EdgePointMap;
    typedef CatmullClarkOperation::CombinationPoint CP;
    typedef CatmullClarkOperation::CombinationSet CS;
    typedef CatmullClarkOperation::PointOperation PO;

    namespace {
        /// Add a vertice, if not already present, on the edge represented by \p he.
        /// The vertice position is computed as the barycenter of the edge end points and the edge faces barycenters.
        /// If the edge is a hole edge, then the new vertice is set at its center.
        void addEdgePoint( const HalfEdgeIdx& he_idx, const HalfEdgeData& heData,
                           TriangleMesh& mesh, EdgePointMap& edgePoints, uint N,
                           CatmullClarkOperation* operation,
                           bool register_op )
        {
            const HalfEdge& he = heData[ he_idx ];
            if (edgePoints.find( he_idx ) == edgePoints.end())
            {
                const HalfEdge& he_pair = heData[ he.m_pair ];
                Vector3 p;
                Vector3 n;
                CS cs;
                if (he.m_leftTriIdx != InvalidIdx && he_pair.m_leftTriIdx != InvalidIdx)
                {
                    p = (mesh.m_vertices[ N + he.m_leftTriIdx ] + mesh.m_vertices[ N + he_pair.m_leftTriIdx ] +
                         mesh.m_vertices[ he.m_endVertexIdx ] + mesh.m_vertices[ he_pair.m_endVertexIdx ]) / 4.0;
                    n = (mesh.m_normals[ N + he.m_leftTriIdx ] + mesh.m_normals[ N + he_pair.m_leftTriIdx ] +
                         mesh.m_normals[ he.m_endVertexIdx ] + mesh.m_normals[ he_pair.m_endVertexIdx ]) / 4.0;
                    if (register_op)
                    {
                        cs.resize(4);
                        cs[0] = CP( N + he.m_leftTriIdx, 0.25 );
                        cs[1] = CP( N + he_pair.m_leftTriIdx, 0.25 );
                        cs[2] = CP( he.m_endVertexIdx, 0.25 );
                        cs[3] = CP( he_pair.m_endVertexIdx, 0.25 );
                    }
                }
                else
                {
                    p = (mesh.m_vertices[ he.m_endVertexIdx ] + mesh.m_vertices[ he_pair.m_endVertexIdx ]) / 2.0;
                    n = (mesh.m_normals[ he.m_endVertexIdx ] + mesh.m_normals[ he_pair.m_endVertexIdx ]) / 2.0;
                    if (register_op)
                    {
                        cs.resize(2);
                        cs[0] = CP( he.m_endVertexIdx, 0.5 );
                        cs[1] = CP( he_pair.m_endVertexIdx, 0.5 );
                    }
                }
                if (register_op)
                {
                    operation->m_newPointsOperations.push_back( PO(mesh.m_vertices.size(), cs) );
                }
                edgePoints[he_idx   ] = mesh.m_vertices.size();
                edgePoints[he.m_pair] = mesh.m_vertices.size();
                mesh.m_vertices.push_back( p );
                mesh.m_normals.push_back( n );
            }
        }
    }

    void CatmullClark( TriangleMesh& mesh, HalfEdgeData& heData,
                       CatmullClarkOperation *operation,
                       bool register_op )
    {
        const uint N = mesh.m_vertices.size();
        const uint F = mesh.m_faces.size();
        if (register_op)
        {
            operation->m_newPointsOperations.resize( F ); // will push_back() for edge points
            operation->m_oldPointsOperations.resize( N );
        }
        // first: add face points
        mesh.m_vertices.resize( N + F );
        mesh.m_normals.resize( N + F );
        uint nb_quads = 0;
        for (uint f = 0; f < F; ++f)
        {
            const VectorNui& face = mesh.m_faces[ f ];
            uint fs = face.size();
            Scalar fs_inv = 1.0/fs;
            CS cs(face.size());
            Vector3 facePoint( 0, 0, 0 );
            Vector3 faceNormal( 0, 0, 0 );
            for (uint v = 0; v<fs; ++v)
            {
                facePoint += mesh.m_vertices[ face[v] ];
                faceNormal += mesh.m_normals[ face[v] ];
                if (register_op)
                {
                    cs[ v ] = CP( face[v], fs_inv );
                }
            }
            facePoint *= fs_inv;
            faceNormal *= fs_inv;
            if (register_op)
            {
                operation->m_newPointsOperations[ f ] = PO( N+f, cs );
            }
            mesh.m_vertices[ N + f ] = facePoint;
            mesh.m_normals[ N + f ] = faceNormal;
            nb_quads += fs;
        }
        // second: add edge points
        EdgePointMap edgePoints;
        for (HalfEdgeIdx he = 0; he < heData.nbHalfEdges(); ++he)
        {
            addEdgePoint( he, heData, mesh, edgePoints, N, operation, register_op );
        }
        // third: move original vertices
        Vector3Array new_pos( N );
        for (VertexIdx v = 0; v < N; ++v)
        {
            // check if vertex on hole
            bool hole = false;
            const auto& v_he = heData.getVertexHalfEdges(v);
            for (const HalfEdgeIdx& he_idx : v_he)
            {
                const HalfEdge &he = heData[he_idx];
                if (he.m_leftTriIdx == InvalidIdx)
                {
                    hole = true;
                    break;
                }
            }
            // compute new pos
            const Vector3& P = mesh.m_vertices[ v ];
            Vector3 F( 0, 0, 0 );
            Vector3 R( 0, 0, 0 );
            CS cs;
            if (register_op)
            {
                cs.push_back( CP(v, 1.0) );
            }
            if (hole)
            {
                if (register_op)
                {
                    cs.push_back( CP(v, 1.0) );
                }
                uint n = 1;
                for (const HalfEdgeIdx& he_idx : v_he)
                {
                    const HalfEdge &he = heData[he_idx];
                    const HalfEdge &he_pair = heData[he.m_pair];
                    if (he.m_leftTriIdx != InvalidIdx && he_pair.m_leftTriIdx != InvalidIdx)
                    {
                        continue;
                    }
                    R += (mesh.m_vertices[ he.m_endVertexIdx ] +
                          mesh.m_vertices[ heData[he.m_pair].m_endVertexIdx ]) / 2.0;
                    ++n;
                    if (register_op)
                    {
                        cs.push_back( CP(he.m_endVertexIdx, 0.5) );
                        cs.push_back( CP(heData[he.m_pair].m_endVertexIdx, 0.5) );
                    }
                }
                const Scalar n_inv = 1.0/n;
                new_pos[ v ] = (P + R) * n_inv;
                if (register_op)
                {
                    for (uint v = 0; v<n; ++v)
                    {
                        cs[v].second *= n_inv;
                    }
                }
            }
            else
            {
                const uint n = v_he.size();
                const Scalar n_inv = 1.0/n;
                if (register_op)
                {
                    cs.resize( n*3+1 );
                    cs[0] = CP(v, Scalar(n-3) * n_inv);
                }
                for (uint h = 0; h < n; ++h)
                {
                    const HalfEdge &he = heData[ v_he[h] ];
                    F += mesh.m_vertices[ N + he.m_leftTriIdx ];
                    R += (mesh.m_vertices[ he.m_endVertexIdx ] +
                          mesh.m_vertices[ heData[he.m_pair].m_endVertexIdx ]);
                    if (register_op)
                    {
                        cs[ h*3 + 1 ] = CP( N + he.m_leftTriIdx, n_inv*n_inv );
                        cs[ h*3 + 2 ] = CP( he.m_endVertexIdx, n_inv*n_inv );
                        cs[ h*3 + 3 ] = CP( heData[he.m_pair].m_endVertexIdx, n_inv*n_inv );
                    }
                }
                F *= n_inv;
                R *= n_inv;
                new_pos[ v ] = (F + R + (n-3)*P ) * n_inv;
            }
            if (register_op)
            {
                operation->m_oldPointsOperations[ v ] = PO( v, cs );
            }
        }
        for (VertexIdx v = 0; v < N; ++v)
        {
            mesh.m_vertices[ v ] = new_pos[ v ];
        }
        new_pos.clear();
        // fourth: connect all together
        VectorArray<VectorNui> quads( nb_quads, VectorNui(4) );
        uint q = 0;
        for (uint f = 0; f < F; ++f)
        {
            const VectorNui& face = mesh.m_faces[ f ];
            uint fs = face.size();
            const VertexIdx fp  = N + f;
            HalfEdgeIdx he_idx = heData.getFirstTriangleHalfEdge( f );
            HalfEdge cur_he = heData[ he_idx ];
            for (int h = 0; h<fs; ++h)
            {
                // prepare vertices indices
                const VertexIdx p0 = edgePoints[ he_idx ];
                const VertexIdx p1 = cur_he.m_endVertexIdx;
                const VertexIdx p2 = edgePoints[ cur_he.m_next ];
                // connect
                quads[ q++ ] << fp, p0, p1, p2;
                // go to next one
                he_idx = cur_he.m_next;
                cur_he = heData[ he_idx ];
            }
        }
        std::swap(mesh.m_faces, quads);
        if (register_op)
        {
            operation->m_nbVertices = mesh.m_vertices.size();
            operation->m_quads = mesh.m_faces;
        }
        mesh.triangulate();
        heData.update(mesh);
//        Vector3Array normals;
//        Core::MeshUtils::getAutoNormals( mesh, normals );
//        std::swap( mesh.m_normals, normals );
    }

} // namespace Algorithm
} // namespace Core
} // namespace Ra
