#include <Engine/Renderer/Mesh/Mesh.hpp>

#include <Core/Mesh/MeshUtils.hpp>
#include <Core/Mesh/HalfEdge.hpp>

namespace Ra
{

    // Dirty is initializes as false so that we do not create the vao while
    // we have no data to send to the gpu.
    Engine::Mesh::Mesh( const std::string& name, GLenum renderMode )
        : m_name( name )
        , m_vao( 0 )
        , m_renderMode(renderMode)
        , m_isDirty( false )
    {
    }

    Engine::Mesh::~Mesh()
    {
        //GL_ASSERT( glDeleteVertexArrays( 1, &m_vao ) );
    }

    void Engine::Mesh::setRenderMode( GLenum mode )
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
        GL_ASSERT( glBindVertexArray( m_vao ) );

        //    GL_ASSERT(glDrawElements(GL_TRIANGLES_ADJACENCY, 6 * m_data.m_triangles.size(), GL_UNSIGNED_INT, (void*)0));
        GL_ASSERT( glDrawElements( m_renderMode, m_mesh.m_triangles.size() * 3 , GL_UNSIGNED_INT, ( void* ) 0 ) );
    }

    void Engine::Mesh::loadGeometry(const Core::TriangleMesh& mesh)
    {
        m_mesh = mesh;
        for (uint i = 0; i < MAX_MESH; ++i)
        {
            m_dataDirty[i] = true;
        }
        m_isDirty = true;

    }

    void Engine::Mesh::loadGeometry(const Core::Vector3Array &vertices, const std::vector<uint> &indices)
    {
        // TODO : remove this function and force everyone to use triangle mesh.
        Core::TriangleMesh m;
        m.m_vertices = vertices;
        for (uint i = 0; i  < indices.size(); i = i + 3)
        {
            m.m_triangles.push_back({indices[i], indices[i+1], indices[i+2]});
        }
        loadGeometry(m);

    }

    void Engine::Mesh::addData( const Vec3Data& type, const Core::Vector3Array& data )
    {
        m_v3Data[static_cast<uint>(type)] = data;
        m_dataDirty[MAX_MESH + static_cast<uint>(type)] = true;
        m_isDirty = true;
    }

    void Engine::Mesh::addData( const Vec4Data& type, const Core::Vector4Array& data )
    {
        m_v4Data[static_cast<uint>(type)] = data;
        m_dataDirty[MAX_MESH + MAX_VEC3 + static_cast<uint>(type)] = true;
        m_isDirty = true;
    }

    template< typename VecArray >
    void Engine::Mesh::sendGLData( const VecArray& arr, const uint vboIdx )
    {

#ifdef CORE_USE_DOUBLE
        GLenum type = GL_DOUBLE;
#else
        GLenum type = GL_FLOAT;
#endif
        constexpr GLuint size = VecArray::Vector::RowsAtCompileTime;
        constexpr GLboolean normalized  = GL_FALSE;
        constexpr GLvoid* ptr = nullptr;

        // This vbo has not been created yet
        if ( m_vbos[vboIdx] == 0 && arr.size() > 0 )
        {
            GL_ASSERT( glGenBuffers( 1, &m_vbos[vboIdx] ) );
            GL_ASSERT( glBindBuffer( GL_ARRAY_BUFFER, m_vbos[vboIdx] ) );
            GL_ASSERT( glBufferData( GL_ARRAY_BUFFER, arr.size() * sizeof( typename VecArray::Vector ),
                                     arr.data(), GL_DYNAMIC_DRAW ) );

            // Use vboIdx -1 as attribute index because vbo  0 is actualyl ibo.
            GL_ASSERT( glVertexAttribPointer( vboIdx -1, size, type, normalized,
                                              sizeof( typename VecArray::Vector ), ptr ) );

            GL_ASSERT( glEnableVertexAttribArray( vboIdx -1 ) );

        }

        if ( m_dataDirty[vboIdx] == true && m_vbos[vboIdx] != 0 && arr.size() > 0 )
        {
            GL_ASSERT( glBindBuffer( GL_ARRAY_BUFFER, m_vbos[vboIdx] ) );
            GL_ASSERT( glBufferData( GL_ARRAY_BUFFER, arr.size() * sizeof( typename VecArray::Vector ),
                                     arr.data(), GL_DYNAMIC_DRAW ) );
            m_dataDirty[vboIdx] = false;
        }
    }



    void Engine::Mesh::updateGL()
    {
        if ( m_isDirty )
        {
            ON_DEBUG(bool dirtyTest = false; for (const auto& d : m_dataDirty) { dirtyTest = dirtyTest || d;});
            CORE_ASSERT( dirtyTest == m_isDirty, "Dirty flags inconsistency");

            CORE_ASSERT( ! ( m_mesh.m_vertices.empty()|| m_mesh.m_triangles.empty() ),
                    "Either vertices or indices are empty arrays.");

            if ( m_vao == 0 )
            {
                // Create VAO if it does not exist
                GL_ASSERT( glGenVertexArrays( 1, &m_vao ) );
            }

            // Bind it
            GL_ASSERT( glBindVertexArray( m_vao ) );

            if (m_vbos[INDEX] == 0 )
            {
                GL_ASSERT( glGenBuffers( 1, &m_vbos[INDEX]) );
                GL_ASSERT( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_vbos[INDEX]) );
            }
            if (m_dataDirty[INDEX])
            {
                GL_ASSERT( glBufferData( GL_ELEMENT_ARRAY_BUFFER, m_mesh.m_triangles.size() * sizeof( Ra::Core::Triangle ),
                                         m_mesh.m_triangles.data(), GL_DYNAMIC_DRAW ) );
                m_dataDirty[INDEX] = false;

            }

            // Geometry data
            sendGLData( m_mesh.m_vertices, VERTEX_POSITION);
            sendGLData( m_mesh.m_normals,  VERTEX_NORMAL);

            // Vec3 data
            sendGLData( m_v3Data[VERTEX_TANGENT],   MAX_MESH + VERTEX_TANGENT);
            sendGLData( m_v3Data[VERTEX_BITANGENT], MAX_MESH + VERTEX_BITANGENT);
            sendGLData( m_v3Data[VERTEX_TEXCOORD],  MAX_MESH + VERTEX_TEXCOORD);

            // Vec4 data
            sendGLData( m_v4Data[VERTEX_COLOR],  MAX_MESH + MAX_VEC3 + VERTEX_COLOR );
            sendGLData( m_v4Data[VERTEX_WEIGHTS], MAX_MESH + MAX_VEC3 + VERTEX_WEIGHTS);

            GL_ASSERT( glBindVertexArray( 0 ) );

            GL_CHECK_ERROR;
            m_isDirty = false;
        }
    }


} // namespace Ra
