#include <Core/Mesh/HalfEdge.hpp>

#include <map>
#include <Core/Mesh/TriangleMesh.hpp>

namespace Ra
{
    namespace Core
    {
        namespace
        {
            // Allow to store the edges in a pair of half edges.
            struct EdgeKey
            {
                VertexIdx vSmaller;
                VertexIdx vLarger;

                bool operator< ( const EdgeKey& other ) const
                {
                    return vLarger != other.vLarger ?
                           vLarger < other.vLarger : vSmaller < other.vSmaller;
                }

                bool operator== ( const EdgeKey& other ) const
                {
                    return ( vLarger == other.vLarger && vSmaller == other.vSmaller );
                }
            };
        }

        void HalfEdgeData::checkConsistency() const
        {
#if defined CORE_DEBUG
            for ( HalfEdgeIdx i = 0; i < m_halfEdgeList.size(); ++i )
            {
                const HalfEdge& he = m_halfEdgeList[i];

                // Check that each half edge has an opposite half.
                CORE_ASSERT( he.m_pair != InvalidIdx, "Half edge has no pair." );
                const HalfEdge& pair = m_halfEdgeList[he.m_pair];
                CORE_ASSERT( pair.m_pair == i, "Edge pair is inconsistent" );

                // Check that the outer half-edges are not in any link.
                if ( he.m_leftTriIdx == InvalidIdx )
                {
                    CORE_ASSERT( he.m_next == InvalidIdx, "Boundary half edge should be isolated" );
                    CORE_ASSERT( he.m_prev == InvalidIdx, "Boundary half edge should be isolated" );
                    CORE_ASSERT( pair.m_leftTriIdx != InvalidIdx, "Isolated edge " );
                }
                else
                {
                    // Check that half-edge belonging to a face are in a good linked list.
                    const HalfEdge& prev = m_halfEdgeList[he.m_prev];
                    const HalfEdge& next = m_halfEdgeList[he.m_next];

                    CORE_ASSERT( prev.m_next == i, "Edge chain is broken" );
                    CORE_ASSERT( next.m_prev == i, "Edge chain is broken" );
                    CORE_ASSERT( next.m_leftTriIdx == he.m_leftTriIdx, "Inconsistent face index" );
                }
            }
            // Check vertex to half edge mapping.
            for ( VertexIdx v = 0; v < m_vertexToHalfEdge.size(); ++v )
            {
                CORE_ASSERT( !m_vertexToHalfEdge[v].empty(), "Isolated vertex" );
                for ( auto idx : m_vertexToHalfEdge[v] )
                {
                    const HalfEdge& he = m_halfEdgeList[idx];
                    const HalfEdge& pair = m_halfEdgeList[he.m_pair];
                    CORE_ASSERT( pair.m_endVertexIdx == v, "Inconsistent vertex index" );
                }
            }
            for ( uint f = 0; f < m_triangleToHalfEdge.size(); ++f )
            {
                // Check that we can go around a face.
                HalfEdgeIdx heIdx = m_triangleToHalfEdge[f];
                HalfEdge cur_he = m_halfEdgeList[heIdx];
                for (uint i=0; i<m_faceSize[f]-1; ++i)
                {
                    CORE_ASSERT( cur_he.m_leftTriIdx == f, "Inconsistent triangle index" );
                    cur_he = m_halfEdgeList[cur_he.m_next];
                }
                CORE_ASSERT( cur_he.m_leftTriIdx == f, "Inconsistent triangle index" );
                CORE_ASSERT( cur_he.m_next == heIdx, "We didn't end back where we started" );
            }
#endif
        }

        void HalfEdgeData::build( const TriangleMesh& mesh )
        {
            m_vertexToHalfEdge.resize( mesh.m_vertices.size() );
            m_triangleToHalfEdge.resize( mesh.m_faces.size(), InvalidIdx );
            m_faceSize.resize( mesh.m_faces.size(), 0 );

            std::map<EdgeKey, HalfEdgeIdx> edgeToHalfEdges;

            // For all triangles.
            for ( uint f = 0; f < mesh.m_faces.size(); ++f )
            {
                const VectorNui& face = mesh.m_faces[f];
                const uint fs = face.size();
                m_faceSize[f] = fs;

                // This arrays contains the indices of all three half edges of the current face
                // in halfEdgeList.
                VectorNui faceHalfEdges( fs );
                #pragma parallel for
                for (uint i=0; i<fs; ++i)
                {
                    faceHalfEdges[i] = InvalidIdx;
                }

                // for all edges in face.
                for ( uint i = 0; i < fs; ++i )
                {
                    VertexIdx vStart = face[i];
                    VertexIdx vEnd = face[( i + 1 ) % fs];

                    EdgeKey key;
                    key.vSmaller = std::min( vStart, vEnd );
                    key.vLarger = std::max( vStart, vEnd );

                    // If we never visited this edge before, that means we must create the two half edges.
                    auto edgeFound = edgeToHalfEdges.find( key );
                    if ( edgeFound == edgeToHalfEdges.end() )
                    {
                        HalfEdge he1; // Half edge from vStart to vEnd (belonging to this triangle)
                        HalfEdge he2; // Half edge from vEnd to VStart (belonging to a triangle not yet visited)

                        // Setup data on half edges
                        he1.m_prev = InvalidIdx;
                        he1.m_next = InvalidIdx;
                        he2.m_prev = InvalidIdx;
                        he2.m_next = InvalidIdx;

                        he1.m_endVertexIdx = vEnd;
                        he2.m_endVertexIdx = vStart;

                        // he1 is the half edge belonging to this triangle.
                        he1.m_leftTriIdx = f;
                        // he2's triangle will be set when its triangle is visited.
                        he2.m_leftTriIdx = InvalidIdx;

                        uint baseIdx = m_halfEdgeList.size();
                        he1.m_pair = baseIdx + 1;
                        he2.m_pair = baseIdx;

                        // Adds them to the vector
                        m_halfEdgeList.push_back( he1 );
                        m_halfEdgeList.push_back( he2 );
                        edgeToHalfEdges.insert( std::make_pair( key, baseIdx ) );
                        faceHalfEdges[i] = baseIdx;

                        m_vertexToHalfEdge[vStart].push_back( baseIdx );
                        m_vertexToHalfEdge[vEnd].push_back( baseIdx + 1 );
                    }
                    else
                    {
                        // If we found an existing edge, it means only one of them was belonging to a face
                        // so we look for the other, which should be next to the first.
                        uint firstHeIdx = edgeFound->second;

                        // Check face index consistency
                        CORE_ASSERT( m_halfEdgeList[firstHeIdx].m_leftTriIdx != InvalidIdx,
                                     "First part of the half edge was not visited" );
                        CORE_ASSERT( m_halfEdgeList[firstHeIdx + 1].m_leftTriIdx == InvalidIdx,
                                     "Second part of the half edge already visited (3 or more faces share an edge?)" );

                        m_halfEdgeList[firstHeIdx + 1].m_leftTriIdx = f;
                        faceHalfEdges[i] = firstHeIdx + 1;
                    }
                }
                // Now all our half edges must be consistent.
                bool ok = true;
                for (uint he=0; he<fs; ++he)
                {
                    ok &= faceHalfEdges[he] != InvalidIdx;
                }
                CORE_ASSERT( ok, "face half edges are missing !" );

                m_triangleToHalfEdge[f] = faceHalfEdges[0];
                // We can finally fixup the looping path on the half edges.
                for ( uint i = 0; i < fs; ++i )
                {
                    CORE_ASSERT( m_halfEdgeList[faceHalfEdges[i]].m_leftTriIdx == f, "Inconsistent face index" );

                    m_halfEdgeList[faceHalfEdges[i]].m_prev = faceHalfEdges[( i + fs - 1 ) % fs];
                    m_halfEdgeList[faceHalfEdges[i]].m_next = faceHalfEdges[( i + 1 ) % fs];
                }
            }
            checkConsistency();
        }

        void HalfEdgeData::build( const VectorArray<Vector3>& vertices,
                                  const VectorArray<Triangle>& triangles )
        {
            m_vertexToHalfEdge.resize( vertices.size() );
            m_triangleToHalfEdge.resize( triangles.size(), InvalidIdx );
            m_faceSize.resize( triangles.size(), 3 );

            std::map<EdgeKey, HalfEdgeIdx> edgeToHalfEdges;

            // For all triangles.
            for ( TriangleIdx t = 0; t < triangles.size(); ++t )
            {
                const Triangle& tri = triangles[t];

                // This arrays contains the indices of all three half edges of the current triangle
                // in halfEdgeList.
                uint triangleHalfEdges[3] = {InvalidIdx, InvalidIdx, InvalidIdx};

                // for all edges in triangle.
                for ( uint i = 0; i < 3; ++i )
                {
                    VertexIdx vStart = tri[i];
                    VertexIdx vEnd = tri[( i + 1 ) % 3];

                    EdgeKey key;
                    key.vSmaller = std::min( vStart, vEnd );
                    key.vLarger = std::max( vStart, vEnd );

                    // If we never visited this edge before, that means we must create the two half edges.
                    auto edgeFound = edgeToHalfEdges.find( key );
                    if ( edgeFound == edgeToHalfEdges.end() )
                    {
                        HalfEdge he1; // Half edge from vStart to vEnd (belonging to this triangle)
                        HalfEdge he2; // Half edge from vEnd to VStart (belonging to a triangle not yet visited)

                        // Setup data on half edges
                        he1.m_prev = InvalidIdx;
                        he1.m_next = InvalidIdx;
                        he2.m_prev = InvalidIdx;
                        he2.m_next = InvalidIdx;

                        he1.m_endVertexIdx = vEnd;
                        he2.m_endVertexIdx = vStart;

                        // he1 is the half edge belonging to this triangle.
                        he1.m_leftTriIdx = t;
                        // he2's triangle will be set when its triangle is visited.
                        he2.m_leftTriIdx = InvalidIdx;

                        uint baseIdx = m_halfEdgeList.size();
                        he1.m_pair = baseIdx + 1;
                        he2.m_pair = baseIdx;

                        // Adds them to the vector
                        m_halfEdgeList.push_back( he1 );
                        m_halfEdgeList.push_back( he2 );
                        edgeToHalfEdges.insert( std::make_pair( key, baseIdx ) );
                        triangleHalfEdges[i] = baseIdx;

                        m_vertexToHalfEdge[vStart].push_back( baseIdx );
                        m_vertexToHalfEdge[vEnd].push_back( baseIdx + 1 );
                    }
                    else
                    {
                        // If we found an existing edge, it means only one of them was belonging to a triangle
                        // so we look for the other, which should be next to the first.
                        uint firstHeIdx = edgeFound->second;

                        // Check triangle index consistency
                        CORE_ASSERT( m_halfEdgeList[firstHeIdx].m_leftTriIdx != InvalidIdx,
                                     "First part of the half edge was not visited" );
                        CORE_ASSERT( m_halfEdgeList[firstHeIdx + 1].m_leftTriIdx == InvalidIdx,
                                     "Second part of the half edge already visited (3 or more triangle share an edge?)" );

                        m_halfEdgeList[firstHeIdx + 1].m_leftTriIdx = t;
                        triangleHalfEdges[i] = firstHeIdx + 1;
                    }
                }
                // Now all our half edges must be consistent.
                CORE_ASSERT( ( triangleHalfEdges[0] != InvalidIdx )
                             && ( triangleHalfEdges[1] != InvalidIdx )
                             && ( triangleHalfEdges[2] != InvalidIdx ), "Triangle half edges are missing !" );

                m_triangleToHalfEdge[t] = triangleHalfEdges[0];
                // We can finally fixup the looping path on the half edges.
                for ( uint i = 0; i < 3; ++i )
                {
                    CORE_ASSERT( m_halfEdgeList[triangleHalfEdges[i]].m_leftTriIdx == t, "Inconsistent triangle index" );

                    m_halfEdgeList[triangleHalfEdges[i]].m_prev = triangleHalfEdges[( i + 2 ) % 3];
                    m_halfEdgeList[triangleHalfEdges[i]].m_next = triangleHalfEdges[( i + 1 ) % 3];
                }
            }
            checkConsistency();
        }

        void AdjacencyQueries::getVertexFaces( const TriangleMesh& mesh, const HalfEdgeData& heData, VertexIdx vertex,
                                               std::vector<TriangleIdx>& facesOut )
        {
            CORE_ASSERT( vertex < mesh.m_vertices.size(), "Invalid vertex index" );
            for ( HalfEdgeIdx idx : heData.getVertexHalfEdges( vertex ) )
            {
                const TriangleIdx t = heData[idx].m_leftTriIdx;
                if ( t != InvalidIdx )
                {
                    facesOut.push_back( t );
                }
            }
        }
        void AdjacencyQueries::getVertexNeighbors( const TriangleMesh& mesh, const HalfEdgeData& heData, VertexIdx vertex,
                                                   std::vector<VertexIdx>& neighborsOut )
        {
            CORE_ASSERT( vertex < mesh.m_vertices.size(), "Invalid vertex index" );
            for ( HalfEdgeIdx idx : heData.getVertexHalfEdges( vertex ) )
            {
                neighborsOut.push_back( heData[idx].m_endVertexIdx );
            }
        }

        void AdjacencyQueries::getAdjacentFaces( const TriangleMesh& mesh, const HalfEdgeData& heData, TriangleIdx triangle,
                                                 std::array<TriangleIdx, 3>& adjOut )
        {
            HalfEdgeIdx currentHe =  heData.getFirstTriangleHalfEdge( triangle );
            for ( uint i = 0; i < 3 ; ++i )
            {
                const HalfEdge& flipHe = heData[heData[currentHe].m_pair];
                adjOut[i] = flipHe.m_leftTriIdx;
                currentHe = heData[currentHe].m_next;
            }
        }

        // This function is a good example on how to process the neighbors of a vertex
        // in order. We could have an iterator-like interface(TODO).
        void AdjacencyQueries::getVertexFirstRing( const TriangleMesh& mesh,
                                                   const HalfEdgeData& heData,
                                                   VertexIdx vertex,
                                                   std::vector<VertexIdx>& ringOut )
        {
            CORE_ASSERT( vertex < mesh.m_vertices.size(), "Invalid vertex index" );
            CORE_ASSERT( heData.getVertexHalfEdges( vertex ).size() > 0, "Vertex has no neighbors" );

            // If we are on a border of the mesh, we must start from one of the border edges.
            // since we walk counter clockwise, the edge we must start from is the one whose
            // pair edge has no face. If no such edge exist, then we are not on the border
            // so any edge will do.
            // TODO : we could guarantee that the first half edge is always the good one to
            // start with in build(), which would optimizes this function.
            HalfEdgeIdx starterEdge = heData.getVertexHalfEdges( vertex ) [0];
            for ( auto heIdx : heData.getVertexHalfEdges( vertex ) )
            {
                const HalfEdge& opposite = heData[heData[heIdx].m_pair];
                if ( opposite.m_leftTriIdx == InvalidIdx )
                {
                    starterEdge = heIdx;
                    break;
                }
            }

            CORE_ASSERT( heData[starterEdge].m_leftTriIdx != InvalidIdx, "Starter edge does not belong to a face" );

            const VertexIdx startVertex = heData[starterEdge].m_endVertexIdx;
            VertexIdx currentVertex = startVertex;
            HalfEdgeIdx currentEdgeIdx = heData[starterEdge].m_next;

            do
            {
                // Add current vertex to ring.
                ringOut.push_back( currentVertex );

                // Advance on the ring.
                currentVertex = heData[currentEdgeIdx].m_endVertexIdx;

                // To get the next edge along the ring, we must look at the next
                // half edge from current (which should point to the center vertex,
                // take its pair half edge so we end up on the next triangle,
                // and take the next half edge to advance.
                const HalfEdge& next = heData[heData[currentEdgeIdx].m_next];
                const HalfEdge& flip = heData[next.m_pair];
                currentEdgeIdx = flip.m_next;

                // Some debug checks.
                CORE_ASSERT( next.m_endVertexIdx == vertex, " Inconsistent half edge data" );
                CORE_ASSERT( flip.m_endVertexIdx == currentVertex, " Inconsistent half edge data" );

            }
            while ( currentVertex != startVertex && currentEdgeIdx != InvalidIdx );

            // Now we check that we have all the neighbors.
            CORE_ASSERT( ringOut.size() == heData.getVertexHalfEdges( vertex ).size(),
                         " Missing some neighbors (the first ring might be broken in more than one place)" );
        }
    }
}
