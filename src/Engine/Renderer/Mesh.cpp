#include <Engine/Renderer/Mesh.hpp>

#include <Engine/Renderer/OpenGL.hpp>

namespace Ra
{

Mesh::Mesh(const std::string& name)
    : Drawable(name)
    , m_initialized(false)
    , m_numVertices(0)
    , m_numIndices(0)
{
}

Mesh::~Mesh()
{
    GL_ASSERT(glDeleteBuffers(1, &m_ibo));
    GL_ASSERT(glDeleteBuffers(1, &m_vbo));
    GL_ASSERT(glDeleteVertexArrays(1, &m_vao));
}

GLuint vbo;
void Mesh::draw()
{
    if (!m_initialized)
    {
        return;
    }

    GL_ASSERT(glBindVertexArray(m_vao));
    GL_ASSERT(glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, (void*)0));
}

void Mesh::loadGeometry(const MeshData& data, bool recomputeNormals)
{
    m_data = data;

    m_numVertices = m_data.vertices.size();
    m_numIndices  = m_data.indices.size();

    if (recomputeNormals)
    {
        computeNormals();
    }

    initGL();
}

void Mesh::initGL()
{
    GL_ASSERT(glGenVertexArrays(1, &m_vao));
    GL_ASSERT(glBindVertexArray(m_vao));

    GL_ASSERT(glGenBuffers(1, &m_vbo));

    GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
    GL_ASSERT(glBufferData(GL_ARRAY_BUFFER, m_numVertices * sizeof(VertexData),
                           m_data.vertices[0].position.data(), GL_STATIC_DRAW));

    GLuint step = sizeof(VertexData);
    GLboolean normalized = GL_FALSE;
    GLenum type = GL_FLOAT;

    GLuint index = 0;
    GLuint size = 3;
    GLvoid* ptr = (void*)0;

    // Position
    GL_ASSERT(glVertexAttribPointer(index, size, type, normalized, step, ptr));
    GL_ASSERT(glEnableVertexAttribArray(index));

#if 0
    // Normal
    ++index;
    ptr = static_cast<char*>(nullptr) + 1 * sizeof(Vector3);
    GL_ASSERT(glVertexAttribPointer(index, size, type, normalized, step, ptr));
    GL_ASSERT(glEnableVertexAttribArray(index));

    // Tangent
    ++index;
    ptr = static_cast<char*>(nullptr) + 2 * sizeof(Vector3);
    GL_ASSERT(glVertexAttribPointer(index, size, type, normalized, step, ptr));
    GL_ASSERT(glEnableVertexAttribArray(index));

    // Texcoord
    ++index;
    ptr = static_cast<char*>(nullptr) + 3 * sizeof(Vector3);
    GL_ASSERT(glVertexAttribPointer(index, size, type, normalized, step, ptr));
    GL_ASSERT(glEnableVertexAttribArray(index));

#endif
    // Indices
    GL_ASSERT(glGenBuffers(1, &m_ibo));
    GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo));
    GL_ASSERT(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_numIndices * sizeof(uint),
                           m_data.indices.data(), GL_STATIC_DRAW));

    GL_ASSERT(glBindVertexArray(0));

    GL_CHECK_ERROR;

    m_initialized = true;
}

void Mesh::computeNormals()
{
#if 0
    // Set normals to 0
    for (auto& vertex : m_data.vertices)
    {
        vertex.normal = Vector3(0, 0, 0);
    }

    // Compute face normals and accumulate
    for (unsigned int i = 0; i < m_data.indices.size(); i += 3)
    {
        Vector3 v0 = m_data.vertices.at(m_data.indices.at(i + 0)).position;
        Vector3 v1 = m_data.vertices.at(m_data.indices.at(i + 1)).position;
        Vector3 v2 = m_data.vertices.at(m_data.indices.at(i + 2)).position;

        Vector3 v0v1 = v1 - v0;
        Vector3 v0v2 = v2 - v0;
        Vector3 tmp = v0v1.cross(v0v2);
        Vector3 n = tmp.normalized();

        m_data.vertices.at(m_data.indices.at(i + 0)).normal += n;
        m_data.vertices.at(m_data.indices.at(i + 1)).normal += n;
        m_data.vertices.at(m_data.indices.at(i + 2)).normal += n;

        assert(!isnan(n.x()) || !isnan(n.y()) || !isnan(n.z()));
    }

    // Normalize
    for (auto& vertex : m_data.vertices)
    {
        vertex.normal.normalize();
    }
#endif
}

} // namespace Ra
