#include <Core/Algorithm/Subdivision/CatmullClarkSubdivision.hpp>

namespace Ra {
namespace Core {
namespace Algorithm {

    typedef std::map< HalfEdgeIdx, VertexIdx > EdgePointMap;

    namespace {
        /// Add a vertice, if not already present, on the edge represented by \p he.
        /// The vertice position is computed as the barycenter of the edge end points and the edge faces barycenters.
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
                if (he_pair.m_leftTriIdx != InvalidIdx || he_pair.m_leftTriIdx != InvalidIdx)
                {
                    p = (mesh.m_vertices[ N + he.m_leftTriIdx ] + mesh.m_vertices[ N + he_pair.m_leftTriIdx ] +
                         mesh.m_vertices[ he.m_endVertexIdx ] + mesh.m_vertices[ he_pair.m_endVertexIdx ]) / 4.0;
                    n = (mesh.m_normals[ N + he.m_leftTriIdx ] + mesh.m_normals[ N + he_pair.m_leftTriIdx ] +
                         mesh.m_normals[ he.m_endVertexIdx ] + mesh.m_normals[ he_pair.m_endVertexIdx ]) / 4.0;
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

    void CatmullClark( TriangleMesh& mesh, HalfEdgeData& heData )
    {
        const uint N = mesh.m_vertices.size();
        const uint T = mesh.m_triangles.size();
        // first: add face points
        mesh.m_vertices.resize( N + T );
        mesh.m_normals.resize( N + T );
        for (TriangleIdx t = 0; t < T; ++t)
        {
            const Triangle& tri = mesh.m_triangles[t];
            mesh.m_vertices[ N + t ] = ( mesh.m_vertices[ tri[0] ] +
                                         mesh.m_vertices[ tri[1] ] +
                                         mesh.m_vertices[ tri[2] ] ) / 3.0;
            mesh.m_normals[ N + t ] = ( mesh.m_normals[ tri[0] ] +
                                        mesh.m_normals[ tri[1] ] +
                                        mesh.m_normals[ tri[2] ] ) / 3.0;
        }
        // second: add edge points
        EdgePointMap edgePoints;
        for (HalfEdgeIdx he = 0; he < heData.nbHalfEdges(); ++he)
        {
            addEdgePoint( he, heData, mesh, edgePoints, N );
        }
        // third: move original vertices
        Vector3Array new_pos( N );
        for (VertexIdx v = 0; v < N; ++v)
        {
            const Vector3& P = mesh.m_vertices[ v ];
            Vector3 F( 0, 0, 0 );
            Vector3 R( 0, 0, 0 );
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
                    R += (mesh.m_vertices[ he.m_endVertexIdx ] + mesh.m_vertices[ heData[he.m_pair].m_endVertexIdx ]) / 2.0;
                    ++n;
                }
                new_pos[ v ] = (P + R) / n;
            }
            else
            {
                for (const HalfEdgeIdx& he_idx : v_he)
                {
                    const HalfEdge &he = heData[he_idx];
                    F += mesh.m_vertices[ N + he.m_leftTriIdx ];
                    R += (mesh.m_vertices[ he.m_endVertexIdx ] + mesh.m_vertices[ heData[he.m_pair].m_endVertexIdx ]) / 2.0;
                }
                uint n = heData.getVertexNbHalfEdges( v );
                F /= n;
                R /= n;
                new_pos[ v ] = (F + 2*R + (n-3)*P ) / n;
            }
        }
        for (VertexIdx v = 0; v < N; ++v)
        {
            mesh.m_vertices[ v ] = new_pos[ v ];
        }
        new_pos.clear();
        // fourth: connect all together
        mesh.m_triangles.resize( 6 * T );
        for (TriangleIdx t = 0; t < T; ++t)
        {
            // prepare triangles
            Triangle& tri0 = mesh.m_triangles[ t       ];
            Triangle& tri1 = mesh.m_triangles[ t + T   ];
            Triangle& tri2 = mesh.m_triangles[ t + T*2 ];
            Triangle& tri3 = mesh.m_triangles[ t + T*3 ];
            Triangle& tri4 = mesh.m_triangles[ t + T*4 ];
            Triangle& tri5 = mesh.m_triangles[ t + T*5 ];
            // prepare vertices indices
            const HalfEdgeIdx he_idx = heData.getFirstTriangleHalfEdge( t );
            const HalfEdge he = heData[ he_idx ];
            const VertexIdx f  = N + t;
            const VertexIdx e0 = edgePoints[ he_idx ];
            const VertexIdx e1 = edgePoints[ he.m_next ];
            const VertexIdx e2 = edgePoints[ he.m_prev ];
            const VertexIdx p0 = he.m_endVertexIdx;
            const VertexIdx p1 = heData[ he.m_next ].m_endVertexIdx;
            const VertexIdx p2 = heData[ he.m_prev ].m_endVertexIdx;
            // connect
            tri0 = Triangle(f, e0, p0);
            tri1 = Triangle(f, p0, e1);
            tri2 = Triangle(f, e1, p1);
            tri3 = Triangle(f, p1, e2);
            tri4 = Triangle(f, e2, p2);
            tri5 = Triangle(f, p2, e0);
        }
        heData.update(mesh);
//        Vector3Array normals;
//        Core::MeshUtils::getAutoNormals( mesh, normals );
//        std::swap( mesh.m_normals, normals );
    }

} // namespace Algorithm
} // namespace Core
} // namespace Ra
