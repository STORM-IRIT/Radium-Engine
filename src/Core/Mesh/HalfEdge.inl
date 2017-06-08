#include "HalfEdge.hpp"

namespace Ra
{
    namespace Core
    {
        inline HalfEdgeData::HalfEdgeData( const TriangleMesh& mesh )
        {
            build( mesh );
        }

        inline HalfEdgeData::HalfEdgeData( const VectorArray<Vector3>& vertices,
                                           const VectorArray<Triangle>& triangles )
        {
            build( vertices, triangles );
        }

        inline const HalfEdge& HalfEdgeData::operator[]( HalfEdgeIdx i ) const
        {
            CORE_ASSERT( i != InvalidIdx && i < m_halfEdgeList.size(), "Invalid Index" );
            return m_halfEdgeList[i];
        }

        inline const std::vector<HalfEdgeIdx>& HalfEdgeData::getVertexHalfEdges( VertexIdx i ) const
        {
            CORE_ASSERT( i != InvalidIdx && i < m_vertexToHalfEdge.size(), "Invalid Index" );
            CORE_ASSERT( !m_vertexToHalfEdge[i].empty(), "Isolated vertex" );
            return m_vertexToHalfEdge[i];
        }

        inline HalfEdgeIdx HalfEdgeData::getFirstTriangleHalfEdge( TriangleIdx t ) const
        {
            CORE_ASSERT( t != InvalidIdx && t < m_triangleToHalfEdge.size(), "Invalid Index" );
            return m_triangleToHalfEdge[t];
        }

        inline void HalfEdgeData::update( const TriangleMesh& mesh )
        {
            clear();
            build( mesh );
        }

        inline void HalfEdgeData::clear()
        {
            m_halfEdgeList.clear();
            m_vertexToHalfEdge.clear();
            m_triangleToHalfEdge.clear();
            m_faceSize.clear();
        }

    }
}
