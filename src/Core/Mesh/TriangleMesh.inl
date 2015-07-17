#include "TriangleMesh.hpp"

namespace Ra { namespace Core {
    inline void TriangleMesh::clear() {
        m_vertices.clear();
        m_normals.clear();
        m_tangents.clear();
        m_triangles.clear();
    }

    inline void TriangleMesh::append(const TriangleMesh &other) {
		const std::size_t verticesBefore = m_vertices.size();
		const std::size_t trianglesBefore = m_triangles.size();

        m_vertices.insert(m_vertices.end(), other.m_vertices.cbegin(), other.m_vertices.cend());
        m_normals.insert(m_normals.end(), other.m_normals.cbegin(), other.m_normals.cend());
        m_tangents.insert(m_tangents.end(), other.m_tangents.cbegin(), other.m_tangents.cend());
        m_triangles.insert(m_triangles.end(), other.m_triangles.cbegin(), other.m_triangles.cend());

        // Offset the vertex indices in the faces
		for (std::size_t t = trianglesBefore; t < m_triangles.size(); ++t)
		{
			for (std::size_t i = 0; i < 3; ++i)
			{
                m_triangles[t][i] += verticesBefore;
            }
        }
    }
}}
