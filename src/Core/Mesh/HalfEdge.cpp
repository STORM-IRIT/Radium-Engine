#include <Core/Mesh/HalfEdge.hpp>

#include <map>
#include <Core/Mesh/TriangleMesh.hpp>

namespace Ra { namespace Core
{
    namespace
    {
        // Allow to store the edges in a pair of half edges.
        struct EdgeKey
        {
            VertexIdx vSmaller;
            VertexIdx vLarger;

            bool operator<(const EdgeKey& other) const
            {
                return vLarger != other.vLarger ?
                       vLarger < other.vLarger : vSmaller < other.vSmaller;
            }

            bool operator==(const EdgeKey& other) const
            {
                return (vLarger == other.vLarger && vSmaller == other.vSmaller);
            }
        };
    }

    void HalfEdgeData::checkConsistency() const
    {
#if defined CORE_DEBUG
        for (HalfEdgeIdx i = 0; i < m_halfEdgeList.size(); ++i)
        {
            const HalfEdge& he = m_halfEdgeList[i];
            const HalfEdge& pair = m_halfEdgeList[he.m_pair];
            const HalfEdge& prev = m_halfEdgeList[he.m_prev];
            const HalfEdge& next = m_halfEdgeList[he.m_next];
            // Check that the outer half-edges are not in any link.
            if (he.m_leftTriIdx == InvalidIdx)
            {
                CORE_ASSERT ( prev.m_next == InvalidIdx, "Boundary edge should be isolated" );
                CORE_ASSERT ( next.m_prev == InvalidIdx, "Boundary edge should be isolated" );
                CORE_ASSERT ( pair.m_leftTriIdx != InvalidIdx, "Isolated edge ");
            }
            else
            {
                // Check that half-edge belonging to a face are in a good linked list.

                CORE_ASSERT ( prev.m_next == i, "Edge chain is broken" );
                CORE_ASSERT ( next.m_prev == i, "Edge chain is broken" );
                CORE_ASSERT ( next.m_leftTriIdx == he.m_leftTriIdx, "Inconsistent face index" );
            }
            CORE_ASSERT ( pair.m_pair== i, "Edge pair is inconsistent");
        }
        // Check vertex to half edge mapping.
        for (VertexIdx v = 0; v < m_vertexToHalfEdge.size(); ++v)
        {
            CORE_ASSERT(! m_vertexToHalfEdge[v].empty(), "Isolated vertex");
            for (auto idx : m_vertexToHalfEdge[v])
            {
                const HalfEdge& he = m_halfEdgeList[idx];
                const HalfEdge& pair = m_halfEdgeList[he.m_pair];
                CORE_ASSERT(pair.m_endVertexIdx == v, "Inconsistent vertex index");
            }
        }

        for (TriangleIdx t = 0; t < m_triangleToHalfEdge.size(); ++t)
        {
            // Check that we can go around a face.
            HalfEdgeIdx heIdx = m_triangleToHalfEdge[t];
            const HalfEdge& he = m_halfEdgeList[heIdx];
            const HalfEdge& next = m_halfEdgeList[he.m_next];
            const HalfEdge& nextNext = m_halfEdgeList[next.m_next];

            CORE_ASSERT(he.m_leftTriIdx 		== t, "Inconsistent triangle index");
            CORE_ASSERT(next.m_leftTriIdx 		== t, "Inconsistent triangle index");
            CORE_ASSERT(nextNext.m_leftTriIdx 	== t, "Inconsistent triangle index");

            CORE_ASSERT(nextNext.m_next == heIdx, "We didn't end back where we started");
        }
#endif
    }

    void HalfEdgeData::build(const TriangleMesh& mesh)
    {
        m_vertexToHalfEdge.resize(mesh.m_vertices.size());
        m_triangleToHalfEdge.resize(mesh.m_triangles.size(), InvalidIdx);

        std::map<EdgeKey, HalfEdgeIdx> edgeToHalfEdges;

        // For all triangles.
        for (TriangleIdx t = 0; t < mesh.m_triangles.size(); ++t)
        {
            const Triangle& tri = mesh.m_triangles[t];

            // This arrays contains the indices of all three half edges of the current triangle
            // in halfEdgeList.
            uint triangleHalfEdges[3] = {InvalidIdx, InvalidIdx, InvalidIdx};

            // for all edges in triangle.
            for (uint i = 0; i < 3; ++i)
            {
                VertexIdx vStart = tri[i];
                VertexIdx vEnd = tri[(i + 1) % 3];

                EdgeKey key;
                key.vSmaller = std::min(vStart, vEnd);
                key.vLarger = std::max(vStart, vEnd);

                // If we never visited this edge before, that means we must create the two half edges.
                auto edgeFound = edgeToHalfEdges.find(key);
                if (edgeFound == edgeToHalfEdges.end())
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
                    m_halfEdgeList.push_back(he1);
                    m_halfEdgeList.push_back(he2);
                    edgeToHalfEdges.insert(std::make_pair(key, baseIdx));
                    triangleHalfEdges[i] = baseIdx;

                    m_vertexToHalfEdge[vStart].push_back(baseIdx);
                    m_vertexToHalfEdge[vEnd].push_back(baseIdx + 1);
                }
                else
                {
                    // If we found an existing edge, it means only one of them was belonging to a triangle
                    // so we look for the other, which should be next to the first.
                    uint firstHeIdx = edgeFound->second;

                    // Check triangle index consistency
                    CORE_ASSERT(m_halfEdgeList[firstHeIdx].m_leftTriIdx != InvalidIdx,
                                "First part of the half edge was not visited");
                    CORE_ASSERT(m_halfEdgeList[firstHeIdx + 1].m_leftTriIdx == InvalidIdx,
                                "Second part of the half edge already visited (3 or more triangle share an edge?)");

                    m_halfEdgeList[firstHeIdx + 1].m_leftTriIdx = t;
                    triangleHalfEdges[i] = firstHeIdx + 1;
                }
            }
            // Now all our half edges must be consistent.
            CORE_ASSERT((triangleHalfEdges[0] != InvalidIdx)
                        && (triangleHalfEdges[1] != InvalidIdx)
                        && (triangleHalfEdges[2] != InvalidIdx), "Triangle half edges are missing !");

            m_triangleToHalfEdge[t] = triangleHalfEdges[0];
            // We can finally fixup the looping path on the half edges.
            for (uint i = 0; i < 3; ++i)
            {
                CORE_ASSERT(m_halfEdgeList[triangleHalfEdges[i]].m_leftTriIdx == t, "Inconsistent triangle index");

                m_halfEdgeList[triangleHalfEdges[i]].m_prev = triangleHalfEdges[(i + 2) % 3];
                m_halfEdgeList[triangleHalfEdges[i]].m_next = triangleHalfEdges[(i + 1) % 3];
            }
        }
    }

    void AdjacencyQueries::getVertexFaces(const TriangleMesh& mesh, const HalfEdgeData& heData, VertexIdx vertex, std::vector<TriangleIdx>& facesOut)
    {
        CORE_ASSERT( vertex < mesh.m_vertices.size(), "Invalid vertex index");
        for ( HalfEdgeIdx idx : heData.getVertexHalfEdges(vertex))
        {
            const TriangleIdx t = heData[idx].m_leftTriIdx;
            if (t != InvalidIdx) { facesOut.push_back(t); }
        }
    }
    void AdjacencyQueries::getVertexNeighbors(const TriangleMesh& mesh, const HalfEdgeData& heData, VertexIdx vertex, std::vector<VertexIdx>& neighborsOut)
    {
        CORE_ASSERT( vertex < mesh.m_vertices.size(), "Invalid vertex index");
        for ( HalfEdgeIdx idx : heData.getVertexHalfEdges(vertex))
        {
            neighborsOut.push_back(heData[idx].m_endVertexIdx);
        }
    }

    void AdjacencyQueries::getAdjacentFaces(const TriangleMesh& mesh, const HalfEdgeData& heData, TriangleIdx triangle, std::array<TriangleIdx,3>& adjOut)
    {
        HalfEdgeIdx currentHe =  heData.getFirstTriangleHalfEdge(triangle);
        for (uint i = 0; i < 3 ; ++i)
        {
            const HalfEdge& flipHe = heData[heData[currentHe].m_pair];
            adjOut[i] = flipHe.m_leftTriIdx;
            currentHe = heData[currentHe].m_next;
        }
    }

}}
