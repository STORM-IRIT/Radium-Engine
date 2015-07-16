namespace Ra { namespace Core
{
    HalfEdgeData::HalfEdgeData(const TriangleMesh& mesh)
    {
        build(mesh);
    }

    inline const HalfEdge& HalfEdgeData::operator[](HalfEdgeIdx i) const
    {
        CORE_ASSERT(i != InvalidIdx && i < m_halfEdgeList.size(), "Invalid Index");
        return m_halfEdgeList[i];
    }

    inline HalfEdge& HalfEdgeData::operator[](HalfEdgeIdx i)
    {
        CORE_ASSERT(i != InvalidIdx && i < m_halfEdgeList.size(), "Invalid Index");
        return m_halfEdgeList[i];
    }

    inline HalfEdgeIdx HalfEdgeData::getFirstVertexHalfEdge(VertexIdx i) const
    {
        CORE_ASSERT(i != InvalidIdx && i < m_vertexToHalfEdge.size(), "Invalid Index");
        return m_vertexToHalfEdge[i];
    }

    inline HalfEdgeIdx HalfEdgeData::getFirstTriangleHalfEdge(TriangleIdx t) const
    {
        CORE_ASSERT(t != InvalidIdx && t < m_triangleToHalfEdge.size(), "Invalid Index");
        return m_triangleToHalfEdge[t];
    }

    inline void HalfEdgeData::update(const TriangleMesh& mesh)
    {
        clear();
        build(mesh);
    }

    inline void HalfEdgeData::clear()
    {
        m_halfEdgeList.clear();
        m_vertexToHalfEdge.clear();
        m_triangleToHalfEdge.clear();
    }
}}
