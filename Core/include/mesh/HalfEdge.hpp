#ifndef RADIUMENGINE_HALFEDGE_HPP
#define RADIUMENGINE_HALFEDGE_HPP

#include <mesh/MeshTypes.hpp>

namespace Ra
{
    struct TriangleMesh;

    /// A structure representing a Half edge (i.e. directed edge) of a mesh.
    struct HalfEdge
    {
        HalfEdgeIdx m_prev; // Previous half edge along the face.
        HalfEdgeIdx m_pair; // Oppositely oriented half-edge
        HalfEdgeIdx m_next; // Next half edge around the face
        TriangleIdx m_leftTriIdx; // Face the half-edge borders
        VertexIdx m_endVertexIdx; // Vertex at the end of the edge;
    };

    /// Structure holding the half-edge data of one mesh.
    class HalfEdgeData
    {
    public:
        /// Build the half edge data from a mesh.
        HalfEdgeData(const TriangleMesh& mesh);

        /// Completely rebuilds the data from the given mesh.
        inline void update(const TriangleMesh& mesh);

        /// Erases all data.
        inline void clear();

        // Accessors to underlying data.
        inline HalfEdge& operator[](HalfEdgeIdx i);
        inline const HalfEdge& operator[](HalfEdgeIdx i) const;

        /// Returns one of the half edges starting from given vertex.
        inline HalfEdgeIdx getFirstVertexHalfEdge(VertexIdx i) const;

        /// Returns one of the half edges around given triangle.
        inline HalfEdgeIdx getFirstTriangleHalfEdge(TriangleIdx t) const;

    private:
        // Internal building function called by update and the constructor.
        void build();

    private:
        /// Container holding the half edges.
        std::vector<HalfEdge> m_halfEdgeList;
        /// Array mapping one vertex to one of the half edges starting from it.
        std::vector<HalfEdgeIdx> m_vertexToHalfEdge;
        /// Array mapping one triangle to one of the half edges starting from it.
        std::vector<HalfEdgeIdx> m_triangleToHalfEdge;
    };
}


#endif //RADIUMENGINE_HALFEDGE_HPP
