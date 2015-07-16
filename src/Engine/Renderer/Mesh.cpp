#include <Engine/Renderer/Mesh.hpp>

#include <Core/Mesh/MeshUtils.hpp>
#include <Engine/Renderer/OpenGL.hpp>

namespace Ra
{

Engine::Mesh::Mesh(const std::string& name)
    : Engine::Drawable(name)
    , m_initialized(false)
{
}

Engine::Mesh::~Mesh()
{
    GL_ASSERT(glDeleteBuffers(1, &m_ibo));
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
    GL_ASSERT(glDrawElements(GL_TRIANGLES, 3* m_data.m_triangles.size(), GL_UNSIGNED_INT, (void*)0));
}

void Engine::Mesh::loadGeometry(const Core::TriangleMesh &data, bool recomputeNormals)
{
    m_data = data;

    if ( m_data.m_normals.size() == 0 || recomputeNormals)
    {
        computeNormals();
    }

    initGL();
}

void Engine::Mesh::initGL()
{

    // VAO activation.
    GL_ASSERT(glGenVertexArrays(1, &m_vao));
    GL_ASSERT(glBindVertexArray(m_vao));

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
    GL_ASSERT(glGenBuffers(1, &m_vbo));
    GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
    GL_ASSERT(glBufferData(GL_ARRAY_BUFFER,
                           m_data.m_vertices.size() * sizeof(Core::Vector3),
                           m_data.m_vertices.data(), GL_STATIC_DRAW));

    GL_ASSERT(glVertexAttribPointer(index, size, type, normalized, sizeof(Core::Vector3), ptr));
    GL_ASSERT(glEnableVertexAttribArray(index));

    // Normal
    CORE_ASSERT(m_data.m_normals.size() == m_data.m_vertices.size(),
                " Normal data is missing or incomplete.");

    ++index;
    GL_ASSERT(glGenBuffers(1, &m_nbo));
    GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, m_nbo));
    GL_ASSERT(glBufferData(GL_ARRAY_BUFFER,
                           m_data.m_normals.size() * sizeof(Core::Vector3),
                           m_data.m_normals.data(), GL_STATIC_DRAW));

    GL_ASSERT(glVertexAttribPointer(index, size, type, normalized, sizeof(Core::Vector3), ptr));
    GL_ASSERT(glEnableVertexAttribArray(index));

#if 0
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
    GL_ASSERT(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_data.m_triangles.size() * sizeof(Core::Vector3i),
                           m_data.m_triangles.data(), GL_STATIC_DRAW));

    GL_ASSERT(glBindVertexArray(0));

    GL_CHECK_ERROR;

    m_initialized = true;
}

void Engine::Mesh::computeNormals()
{
    Core::MeshUtils::getAutoNormals( m_data, m_data.m_normals );
}

} // namespace Ra
