
#include <Engine/Renderer/Mesh/Mesh.hpp>

#include <Core/Mesh/MeshUtils.hpp>
#include <Core/Mesh/HalfEdge.hpp>
#include <Engine/Renderer/OpenGL/OpenGL.hpp>

namespace Ra
{

    // Dirty is initializes as false so that we do not create the vao while
    // we have no data to send to the gpu.
    Engine::Mesh::Mesh ( const std::string& name )
        : m_name ( name )
        , m_isDirty ( false )
        , m_vao ( 0 )
        , m_renderMode ( GL_TRIANGLES )
        , m_ibo()
    {
    }

    Engine::Mesh::~Mesh()
    {
        GL_ASSERT ( glDeleteVertexArrays ( 1, &m_vao ) );
    }

    void Engine::Mesh::setRenderMode ( const GLenum& mode )
    {
        m_renderMode = mode;
    }

    void Engine::Mesh::render()
    {
        if ( m_vao == 0 )
        {
            // Not initialized yet
            return;
        }
        // FIXME(Charly): This seems to crash on windows
        GL_ASSERT ( glBindVertexArray ( m_vao ) );


        //    GL_ASSERT(glDrawElements(GL_TRIANGLES_ADJACENCY, 6 * m_data.m_triangles.size(), GL_UNSIGNED_INT, (void*)0));
        GL_ASSERT ( glDrawElements ( m_renderMode, m_indices.size(), GL_UNSIGNED_INT, ( void* ) 0 ) );
    }

    void Engine::Mesh::loadGeometry ( const Vector3Array& positions,
                                      const std::vector<uint>& indices )
    {
        m_data[VERTEX_POSITION] = positions;
        m_indices = indices;

        m_isDirty = true;
    }

    void Engine::Mesh::addData ( const DataType& type, const Vector3Array& data )
    {
        m_data[type] = data;

        m_isDirty = true;
    }

    // TODO(Charly): Move this somewhere else
    /*
    void Engine::Mesh::computeAdjacency()
    {
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
    */

    void Engine::Mesh::updateGL()
    {
        if ( !m_isDirty )
        {
            return;
        }

        if ( m_data[VERTEX_POSITION].empty() || m_indices.empty() )
        {
            LOG ( logWARNING ) << "Either vertices or indices are empty arrays.";
            return;
        }

        if ( m_vao == 0 )
        {
            // Create VAO if it does not exist
            GL_ASSERT ( glGenVertexArrays ( 1, &m_vao ) );
        }

        // Bind it
        GL_ASSERT ( glBindVertexArray ( m_vao ) );

        // Common values for GL data functions.
#if defined CORE_USE_DOUBLE
        GLenum type = GL_DOUBLE;
#else
        GLenum type = GL_FLOAT;
#endif
        GLboolean normalized = GL_FALSE;
        GLuint size = 3;
        GLvoid* ptr = nullptr;


        for ( uint i = 0; i < m_data.size(); ++i )
        {
            // This vbo has not been created yet
            if ( m_vbos[i].size() == 0 && m_data[i].size() > 0 )
            {
                m_vbos[i].setData ( m_data[i], GL_STATIC_DRAW );
                GL_ASSERT ( glVertexAttribPointer ( i, size, type, normalized,
                                                    sizeof ( Core::Vector3 ), ptr ) );

                GL_ASSERT ( glEnableVertexAttribArray ( i ) );
            }
        }

        // Indices has not been initialized yet
        if ( m_ibo.size() == 0 )
        {
            m_ibo.setData ( m_indices );
        }
        GL_ASSERT ( glBindVertexArray ( 0 ) );

        GL_CHECK_ERROR;
        m_isDirty = false;
    }

} // namespace Ra
