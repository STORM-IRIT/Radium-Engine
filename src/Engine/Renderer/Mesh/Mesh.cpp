#include <Engine/Renderer/Mesh/Mesh.hpp>

#include <Core/Mesh/MeshUtils.hpp>
#include <Core/Mesh/HalfEdge.hpp>
#include <Engine/Renderer/OpenGL/OpenGL.hpp>

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

void Engine::Mesh::loadGeometry(const Core::TriangleMesh &data, bool recomputeNormals)
{
    m_data = data;

    if (m_data.m_normals.size() == 0 || recomputeNormals)
    {
        computeNormals();
    }

    initGL();
}

void Engine::Mesh::loadGeometry(const Core::TriangleMesh &data,
                                const Core::VectorArray<Core::Vector3>& tangents,
                                const Core::VectorArray<Core::Vector3>& bitangents,
                                bool recomputeNormals)
{
    m_data = data;
    if (m_data.m_normals.size() == 0 || recomputeNormals)
    {
        computeNormals();
    }

    m_tangents = tangents;
    m_bitangents = bitangents;

    initGL();
}

void Engine::Mesh::initGL()
{
    std::vector<uint> adjacency;

    Core::HalfEdgeData heData(m_data);

    Core::TriangleIdx triangleIdx = 0;
    for (; triangleIdx < m_data.m_triangles.size(); ++triangleIdx)
    {
        Core::Triangle triangle = m_data.m_triangles[triangleIdx];
        Core::HalfEdgeIdx currentHe = heData.getFirstTriangleHalfEdge(triangleIdx);

        const Core::HalfEdge& he0 = heData[currentHe];
        const Core::HalfEdge& he1 = heData[he0.m_next];
        const Core::HalfEdge& he2 = heData[he1.m_next];
        const Core::HalfEdge& he3 = heData[he2.m_next];

        const Core::HalfEdge& ph0 = heData[he0.m_pair];
        const Core::HalfEdge& ph1 = heData[he1.m_pair];
        const Core::HalfEdge& ph2 = heData[he2.m_pair];

        Core::HalfEdgeIdx v0 = he2.m_endVertexIdx;
        Core::HalfEdgeIdx v1 = he0.m_endVertexIdx;
        Core::HalfEdgeIdx v2 = he1.m_endVertexIdx;

        Core::HalfEdgeIdx a0 = ph0.m_leftTriIdx != Core::InvalidIdx ? heData[ph0.m_next].m_endVertexIdx : v0;
        Core::HalfEdgeIdx a1 = ph1.m_leftTriIdx != Core::InvalidIdx ? heData[ph1.m_next].m_endVertexIdx : v1;
        Core::HalfEdgeIdx a2 = ph2.m_leftTriIdx != Core::InvalidIdx ? heData[ph2.m_next].m_endVertexIdx : v2;

        adjacency.push_back(v0);
        adjacency.push_back(a1);
        adjacency.push_back(v1);
        adjacency.push_back(a0);
        adjacency.push_back(v2);
        adjacency.push_back(a2);
//        for (uint i = 0 ; i < 3 ; ++i)
//        {
//            // L'half edge de l'autre cote te donne acces a la face voisine
//            const Core::HalfEdge& flipHe = heData[heData[currentHe].m_pair];

//            // Si cette face existe (i.e. on est pas au bord)
//            if (flipHe.m_leftTriIdx != Core::InvalidIdx)
//            {
//                // Alors le vertex qu'on cherche est au bout de l'half edge suivante !
//                verticesOut[i] = heData[flipHe.m_next].m_endVertexIdx;
//            }
//            else
//            {
//                // Sinon on fait quoi ?
//            }

//            // Maintenant on avance d'un cran !
//            currentHe = heData[currentHe].m_next;
//        }
    }

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

    if (m_tangents.size() != 0)
    {
        CORE_ASSERT(m_tangents.size() == m_data.m_vertices.size(),
                    "Tangent data is incomplete.");

        ++index;
        GL_ASSERT(glGenBuffers(1, &m_tbo));
        GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, m_tbo));
        GL_ASSERT(glBufferData(GL_ARRAY_BUFFER,
                               m_tangents.size() * sizeof(Core::Vector3),
                               m_tangents.data(), GL_STATIC_DRAW));

        GL_ASSERT(glVertexAttribPointer(index, size, type, normalized, sizeof(Core::Vector3), ptr));
        GL_ASSERT(glEnableVertexAttribArray(index));
    }

    if (m_bitangents.size() != 0)
    {
        CORE_ASSERT(m_bitangents.size() == m_data.m_vertices.size(),
                    "Bitangent data is incomplete.");

        ++index;
        GL_ASSERT(glGenBuffers(1, &m_tbo));
        GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, m_tbo));
        GL_ASSERT(glBufferData(GL_ARRAY_BUFFER,
                               m_bitangents.size() * sizeof(Core::Vector3),
                               m_bitangents.data(), GL_STATIC_DRAW));

        GL_ASSERT(glVertexAttribPointer(index, size, type, normalized, sizeof(Core::Vector3), ptr));
        GL_ASSERT(glEnableVertexAttribArray(index));
    }

#if 0
    // Texcoord
    ++index;
    ptr = static_cast<char*>(nullptr) + 3 * sizeof(Vector3);
    GL_ASSERT(glVertexAttribPointer(index, size, type, normalized, step, ptr));
    GL_ASSERT(glEnableVertexAttribArray(index));

#endif
    // Indices
    GL_ASSERT(glGenBuffers(1, &m_ibo));
    GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo));
    GL_ASSERT(glBufferData(GL_ELEMENT_ARRAY_BUFFER, adjacency.size() * sizeof(uint), adjacency.data(), GL_STATIC_DRAW));
//    GL_ASSERT(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_data.m_triangles.size() * sizeof(Core::Triangle), m_data.m_triangles.data(), GL_STATIC_DRAW));

    GL_ASSERT(glBindVertexArray(0));

    GL_CHECK_ERROR;

    m_initialized = true;
}

void Engine::Mesh::computeNormals()
{
    Core::MeshUtils::getAutoNormals(m_data, m_data.m_normals);
}

} // namespace Ra
