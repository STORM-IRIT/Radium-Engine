#include <Core/Algorithm/Subdivision/LoopSubdivision.hpp>
#include <Core/Mesh/MeshUtils.hpp>

namespace Ra {
namespace Core {
namespace Algorithm {

    typedef std::map< HalfEdgeIdx, VertexIdx > EdgePointMap;

    namespace {
        /// Add a vertice, if not already present, on the edge represented by \p he.
        /// The vertice position is computed as the barycenter of the edge end points and the edge faces barycenters,
        /// with respective weigths 3/8 and 1/8.
        /// If the edge is a hole edge, then the new vertice is set at its center.
        void addEdgePoint( const HalfEdgeIdx& he_idx, const HalfEdgeData& heData,
                           TriangleMesh& mesh, EdgePointMap& edgePoints, uint N )
        {
            const HalfEdge& he = heData[ he_idx ];
            if (edgePoints.find( he_idx ) == edgePoints.end())
            {
                const HalfEdge& he_pair = heData[ he.m_pair ];
                Vector3 p;
                Vector3 n;
                if (he.m_leftTriIdx != InvalidIdx && he_pair.m_leftTriIdx != InvalidIdx)
                {
                    const Triangle &tri1 = mesh.m_triangles[ he.m_leftTriIdx ];
                    const Triangle &tri2 = mesh.m_triangles[ he_pair.m_leftTriIdx ];
                    p = (mesh.m_vertices[ tri1[0] ] + mesh.m_vertices[ tri1[1] ] + mesh.m_vertices[ tri1[2] ] +
                         mesh.m_vertices[ tri2[0] ] + mesh.m_vertices[ tri2[1] ] + mesh.m_vertices[ tri2[2] ] +
                         mesh.m_vertices[ he.m_endVertexIdx ] + mesh.m_vertices[ he_pair.m_endVertexIdx ]) / 8.0;
                    n = (mesh.m_normals[ tri1[0] ] + mesh.m_normals[ tri1[1] ] + mesh.m_normals[ tri1[2] ] +
                         mesh.m_normals[ tri2[0] ] + mesh.m_normals[ tri2[1] ] + mesh.m_normals[ tri2[2] ] +
                         mesh.m_normals[ he.m_endVertexIdx ] + mesh.m_normals[ he_pair.m_endVertexIdx ]) / 8.0;
                }
                else
                {
                    p = (mesh.m_vertices[ he.m_endVertexIdx ] + mesh.m_vertices[ he_pair.m_endVertexIdx ]) / 2.0;
                    n = (mesh.m_normals[ he.m_endVertexIdx ] + mesh.m_normals[ he_pair.m_endVertexIdx ]) / 2.0;
                }
                edgePoints[he_idx   ] = mesh.m_vertices.size();
                edgePoints[he.m_pair] = mesh.m_vertices.size();
                mesh.m_vertices.push_back( p );
                mesh.m_normals.push_back( n );
            }
        }
    }

    void LoopSubdivision( TriangleMesh &mesh , HalfEdgeData &heData )
    {
        const uint N = mesh.m_vertices.size();
        const uint T = mesh.m_triangles.size();
        // first: add edge points
        EdgePointMap edgePoints;
        for (HalfEdgeIdx he = 0; he < heData.nbHalfEdges(); ++he)
        {
            addEdgePoint( he, heData, mesh, edgePoints, N );
        }
        // second: move original points
        Vector3Array new_pos( N );
        Vector3Array new_nor( N );
        for (VertexIdx v = 0; v < N; ++v)
        {
            const Vector3& P = mesh.m_vertices[ v ];
            const Vector3& PN = mesh.m_normals[ v ];
            Vector3 R( 0, 0, 0 );
            Vector3 RN( 0, 0, 0 );
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
            if (hole)
            {
                uint n = 1;
                for (const HalfEdgeIdx& he_idx : v_he)
                {
                    const HalfEdge &he = heData[he_idx];
                    const HalfEdge &he_pair = heData[he.m_pair];
                    if (he.m_leftTriIdx != InvalidIdx && he_pair.m_leftTriIdx != InvalidIdx)
                    {
                        continue;
                    }
                    R += mesh.m_vertices[ he.m_endVertexIdx ];
                    RN += mesh.m_normals[ he.m_endVertexIdx ];
                    ++n;
                }
                new_pos[ v ] = (R + 6*P) / 8.0;
                new_nor[ v ] = (RN + 6*PN) / 8.0;
            }
            else
            {
                for (const HalfEdgeIdx& he_idx : v_he)
                {
                    const HalfEdge &he = heData[he_idx];
                    R += mesh.m_vertices[ he.m_endVertexIdx ];
                    RN += mesh.m_normals[ he.m_endVertexIdx ];
                }
                uint n = v_he.size();
//                const Scalar b = ( n>3 ? 3.0/(8*n) : 3.0/(16*n) );  // FISHER'S FORMULA
                const Scalar b_ = (3.0/8.0 + 0.25*cos(2.0*M_PI / n)); // LOIC'S FORMULA
                const Scalar b = 1.0/n*(5.0/8.0-b_*b_);               //
                new_pos[ v ] = (b*R + (1-n*b)*P);
                new_nor[ v ] = (b*RN + (1-n*b)*PN);
            }
        }
        for (VertexIdx v = 0; v < N; ++v)
        {
            mesh.m_vertices[ v ] = new_pos[ v ];
        }
        new_pos.clear();
        // third: connect all together
        mesh.m_triangles.resize( 4 * T );
        for (TriangleIdx t = 0; t < T; ++t)
        {
            // prepare triangles
            Triangle& tri0 = mesh.m_triangles[ t       ];
            Triangle& tri1 = mesh.m_triangles[ t + T   ];
            Triangle& tri2 = mesh.m_triangles[ t + T*2 ];
            Triangle& tri3 = mesh.m_triangles[ t + T*3 ];
            // prepare vertices indices
            const HalfEdgeIdx he_idx = heData.getFirstTriangleHalfEdge( t );
            const HalfEdge he = heData[ he_idx ];
            const VertexIdx e0 = edgePoints[ he_idx ];
            const VertexIdx e1 = edgePoints[ he.m_next ];
            const VertexIdx e2 = edgePoints[ he.m_prev ];
            const VertexIdx p0 = he.m_endVertexIdx;
            const VertexIdx p1 = heData[ he.m_next ].m_endVertexIdx;
            const VertexIdx p2 = heData[ he.m_prev ].m_endVertexIdx;
            // connect
            tri0 = Triangle(e0, p0, e1);
            tri1 = Triangle(e1, p1, e2);
            tri2 = Triangle(e2, p2, e0);
            tri3 = Triangle(e0, e1, e2);
        }
        heData.update( mesh );
//        Vector3Array normals;
//        Core::MeshUtils::getAutoNormals( mesh, normals );
//        std::swap( mesh.m_normals, normals );
    }

} // namespace Algorithm
} // namespace Core
} // namespace Ra
