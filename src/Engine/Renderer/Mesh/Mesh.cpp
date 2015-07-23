#include <Engine/Renderer/Mesh/Mesh.hpp>

#include <Core/Mesh/MeshUtils.hpp>
#include <Core/Mesh/HalfEdge.hpp>
#include <Engine/Renderer/OpenGL/OpenGL.hpp>

namespace Ra
{

Engine::Mesh::Mesh(const std::string& name)
    : m_name(name)
    , m_initialized(false)
    , m_vao(0)
    , m_vbo(0)
    , m_nbo(0)
    , m_tbo(0)
    , m_bbo(0)
    , m_cbo(0)
    , m_ibo(0)
{
}

Engine::Mesh::~Mesh()
{
    GL_ASSERT(glDeleteBuffers(1, &m_ibo));

    GL_ASSERT(glDeleteBuffers(1, &m_cbo));
    GL_ASSERT(glDeleteBuffers(1, &m_bbo));
    GL_ASSERT(glDeleteBuffers(1, &m_tbo));
    GL_ASSERT(glDeleteBuffers(1, &m_nbo));
    GL_ASSERT(glDeleteBuffers(1, &m_vbo));

    GL_ASSERT(glDeleteVertexArrays(1, &m_vao));
}

void Engine::Mesh::draw()
{
    if (!m_initialized)
    {
        return;
    }

    GL_ASSERT(glBindVertexArray(m_vao));
    GL_ASSERT(glDrawElements(GL_TRIANGLES_ADJACENCY, 6 * m_data.m_triangles.size(), GL_UNSIGNED_INT, (void*)0));
//    GL_ASSERT(glDrawElements(GL_TRIANGLES, 3 * m_data.m_triangles.size(), GL_UNSIGNED_INT, (void*)0));
}

void Engine::Mesh::loadGeometry(const Core::TriangleMesh &data,
                                const Vector3Array& tangents,
                                const Vector3Array& bitangents,
                                const Vector3Array& texcoords,
                                bool recomputeNormals)
{
    m_data = data;
    if (m_data.m_normals.size() == 0 || recomputeNormals)
    {
        computeNormals();
    }

    m_tangents = tangents;
    m_bitangents = bitangents;
    m_texcoords = texcoords;

    Core::HalfEdgeData heData(m_data);

    Core::TriangleIdx triangleIdx = 0;
    for (; triangleIdx < m_data.m_triangles.size(); ++triangleIdx)
    {
        Core::HalfEdgeIdx currentHe = heData.getFirstTriangleHalfEdge(triangleIdx);

        // For each side of the triangle.
        const Core::HalfEdge& he0 = heData[currentHe];
        const Core::HalfEdge& he1 = heData[he0.m_next];
        const Core::HalfEdge& he2 = heData[he1.m_next];

        // We get the opposing half edge.
        const Core::HalfEdge& ph0 = heData[he0.m_pair];
        const Core::HalfEdge& ph1 = heData[he1.m_pair];
        const Core::HalfEdge& ph2 = heData[he2.m_pair];

        // The vertices of the triangle.
        Core::HalfEdgeIdx v0 = he2.m_endVertexIdx;
        Core::HalfEdgeIdx v1 = he0.m_endVertexIdx;
        Core::HalfEdgeIdx v2 = he1.m_endVertexIdx;

        // And the vertices opposite each edge.
        Core::HalfEdgeIdx a0 = ph0.m_leftTriIdx != Core::InvalidIdx ? heData[ph0.m_next].m_endVertexIdx : v0;
        Core::HalfEdgeIdx a1 = ph1.m_leftTriIdx != Core::InvalidIdx ? heData[ph1.m_next].m_endVertexIdx : v1;
        Core::HalfEdgeIdx a2 = ph2.m_leftTriIdx != Core::InvalidIdx ? heData[ph2.m_next].m_endVertexIdx : v2;

        m_adjacentTriangles.push_back(v0);
        m_adjacentTriangles.push_back(a1);
        m_adjacentTriangles.push_back(v1);
        m_adjacentTriangles.push_back(a0);
        m_adjacentTriangles.push_back(v2);
        m_adjacentTriangles.push_back(a2);
    }
}

void Engine::Mesh::updateGL()
{
    if (m_vao == 0)
    {
        // VAO activation.
        GL_ASSERT(glGenVertexArrays(1, &m_vao));
        GL_ASSERT(glBindVertexArray(m_vao));

        GL_ASSERT(glGenBuffers(1, &m_vbo));
        GL_ASSERT(glGenBuffers(1, &m_nbo));
        GL_ASSERT(glGenBuffers(1, &m_tbo));
        GL_ASSERT(glGenBuffers(1, &m_bbo));
        GL_ASSERT(glGenBuffers(1, &m_cbo));
        GL_ASSERT(glGenBuffers(1, &m_ibo));
    }
        // Common values for GL data functions.
#if defined CORE_USE_DOUBLE
    GLenum type = GL_DOUBLE;
#else
    GLenum type = GL_FLOAT;
#endif
    GLboolean normalized = GL_FALSE;
    GLuint index = 0;
    GLuint size = 3;
    GLvoid* ptr = nullptr;

    // Position
    GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
    GL_ASSERT(glBufferData(GL_ARRAY_BUFFER,
                           m_data.m_vertices.size() * sizeof(Core::Vector3),
                           m_data.m_vertices.data(), GL_STATIC_DRAW));

    GL_ASSERT(glVertexAttribPointer(index, size, type, normalized,
                                    sizeof(Core::Vector3), ptr));
    GL_ASSERT(glEnableVertexAttribArray(index));

    // Normal
    CORE_ASSERT(m_data.m_normals.size() == m_data.m_vertices.size(),
                " Normal data is missing or incomplete.");

    ++index;
    GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, m_nbo));
    GL_ASSERT(glBufferData(GL_ARRAY_BUFFER,
                           m_data.m_normals.size() * sizeof(Core::Vector3),
                           m_data.m_normals.data(), GL_STATIC_DRAW));

    GL_ASSERT(glVertexAttribPointer(index, size, type, normalized,
                                    sizeof(Core::Vector3), ptr));
    GL_ASSERT(glEnableVertexAttribArray(index));

    if (m_tangents.size() != 0)
    {
        CORE_ASSERT(m_tangents.size() == m_data.m_vertices.size(),
                    "Tangent data is incomplete.");

        ++index;
        GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, m_tbo));
        GL_ASSERT(glBufferData(GL_ARRAY_BUFFER,
                               m_tangents.size() * sizeof(Core::Vector3),
                               m_tangents.data(), GL_STATIC_DRAW));

        GL_ASSERT(glVertexAttribPointer(index, size, type, normalized,
                                        sizeof(Core::Vector3), ptr));
        GL_ASSERT(glEnableVertexAttribArray(index));
    }

    if (m_bitangents.size() != 0)
    {
        CORE_ASSERT(m_bitangents.size() == m_data.m_vertices.size(),
                    "Bitangent data is incomplete.");

        ++index;
        GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, m_bbo));
        GL_ASSERT(glBufferData(GL_ARRAY_BUFFER,
                               m_bitangents.size() * sizeof(Core::Vector3),
                               m_bitangents.data(), GL_STATIC_DRAW));

        GL_ASSERT(glVertexAttribPointer(index, size, type, normalized,
                                        sizeof(Core::Vector3), ptr));
        GL_ASSERT(glEnableVertexAttribArray(index));
    }

    if (m_texcoords.size() != 0)
    {
        CORE_ASSERT(m_texcoords.size() == m_data.m_vertices.size(),
                    "Texcoords data is incomplete");

        ++index;
        GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, m_cbo));
        GL_ASSERT(glBufferData(GL_ARRAY_BUFFER,
                               m_texcoords.size() * sizeof(Core::Vector3),
                               m_texcoords.data(), GL_STATIC_DRAW));

        GL_ASSERT(glVertexAttribPointer(index, size, type, normalized,
                                        sizeof(Core::Vector3), ptr));
        GL_ASSERT(glEnableVertexAttribArray(index));
    }

    // Indices
    GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo));
    GL_ASSERT(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                           m_adjacentTriangles.size() * sizeof(uint),
                           m_adjacentTriangles.data(), GL_STATIC_DRAW));
    /*
    GL_ASSERT(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                           m_data.m_triangles.size() * sizeof(Core::Triangle),
                           m_data.m_triangles.data(), GL_STATIC_DRAW));
                           */

    GL_ASSERT(glBindVertexArray(0));

    GL_CHECK_ERROR;

    m_initialized = true;
}

void Engine::Mesh::computeNormals()
{
    Core::MeshUtils::getAutoNormals(m_data, m_data.m_normals);
}

} // namespace Ra
