#ifndef RADIUMENGINE_HALFEDGE_HPP
#define RADIUMENGINE_HALFEDGE_HPP

#include <vector>
#include <array>

#include <Core/CoreMacros.hpp>
#include <Core/Mesh/MeshTypes.hpp>

namespace Ra
{
    namespace Core
    {
        struct TriangleMesh;

        /// A structure representing a Half edge (i.e. directed edge) of a mesh.
        struct RA_API HalfEdge
        {
            HalfEdgeIdx m_prev; // Previous half edge along the face.
            HalfEdgeIdx m_pair; // Oppositely oriented half-edge
            HalfEdgeIdx m_next; // Next half edge around the face
            TriangleIdx m_leftTriIdx; // Face the half-edge borders (may be invalid)
            VertexIdx m_endVertexIdx; // Vertex at the end of the edge;
        };

        /// Structure holding the half-edge data of one mesh.
        /// Note on non manifold edges : if the mesh is non-closed, two half edges will be constructed
        /// even on the border edges, however the "outer" half-edge will have invalid
        /// face and prev/next indices.
        /// Edges shared by more than 2 triangles are not supported and will cause an assert during
        /// the call to build().
        class HalfEdgeData
        {
        public:
            /// Build the half edge data from a mesh.
            inline explicit RA_API HalfEdgeData( const TriangleMesh& mesh );

            /// Completely rebuilds the data from the given mesh.
            inline RA_API void update( const TriangleMesh& mesh );

            /// Erases all data.
            inline RA_API void clear();

            // Accessors to underlying data.
            inline RA_API const HalfEdge& operator[]( HalfEdgeIdx i ) const;

            /// Returns the half edges starting from given vertex.
            inline RA_API const std::vector<HalfEdgeIdx>& getVertexHalfEdges( VertexIdx i ) const;

            /// Returns one of the half edges around given triangle.
            inline RA_API HalfEdgeIdx getFirstTriangleHalfEdge( TriangleIdx t ) const;

            /// Checks the structure is internally consistent (in debug mode).
            void RA_API checkConsistency() const;

        private:
            // Internal building function called by update and the constructor.
            void RA_API build( const TriangleMesh& mesh );

        private:
            /// Container holding the half edges.
            std::vector<HalfEdge> m_halfEdgeList;
            /// Array mapping one vertex to one of the half edges starting from it.
            std::vector<std::vector<HalfEdgeIdx>> m_vertexToHalfEdge;
            /// Array mapping one triangle to one of the half edges starting from it.
            std::vector<HalfEdgeIdx> m_triangleToHalfEdge;
        };

        namespace AdjacencyQueries
        {
            /// Gets the faces which contain a given vertex.
            RA_API void getVertexFaces( const TriangleMesh& mesh, const HalfEdgeData& heData,
                                        VertexIdx vertex, std::vector<TriangleIdx>& facesOut );

            /// Gets the faces adjacent to a given triangle, in order. Note that
            /// the face indices may be invalid (if the triangle is on the border)
            RA_API void getAdjacentFaces( const TriangleMesh& mesh, const HalfEdgeData& heData,
                                          TriangleIdx triangle, std::array<TriangleIdx, 3>& adjOut );

            /// Gets the neighbors of a vertex, unordered.
            RA_API void getVertexNeighbors( const TriangleMesh& mesh, const HalfEdgeData& heData,
                                            VertexIdx vertex, std::vector<VertexIdx>& neighborsOut );

            /// Gets the neighbors of a vertex in clockwise order.
            /// * If the vertex has a regular neighborhood (i.e. it doesn't sit on the border
            /// of a mesh), then ringOut forms an actual ring and each vertex in it is adjacent to the
            /// previous and next in the array, including the first and last.
            /// * If the vertex is located on the edge but all neighbours still form a connected
            /// graph (i.e. there's only one "missing link" in the ring) then ringOut will be in
            /// the correct order, each vertex adjacent to the previous and next in the array,
            /// except the first and last.
            /// * If there are multiple missing links, this function will assert. Consider using
            /// getVertexNeighbors instead.
            RA_API void getVertexFirstRing( const TriangleMesh& mesh, const HalfEdgeData& heData,
                                            VertexIdx vertex, std::vector<VertexIdx>& ringOut );
        }
    }
}

#include <Core/Mesh/HalfEdge.inl>

#endif //RADIUMENGINE_HALFEDGE_HPP
