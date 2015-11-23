#include <Engine/Renderer/Mesh/Mesh.hpp>

#include <Core/Mesh/MeshUtils.hpp>
#include <Core/Mesh/HalfEdge.hpp>

namespace Ra
{

    // Dirty is initializes as false so that we do not create the vao while
    // we have no data to send to the gpu.
    Engine::Mesh::Mesh( const std::string& name, GLenum renderMode )
        : m_name( name )
        , m_isDirty( false )
        , m_vao( 0 )
        , m_renderMode(renderMode)
        , m_ibo( 0 )
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
        GL_ASSERT( glDrawElements( m_renderMode, m_indices.size(), GL_UNSIGNED_INT, ( void* ) 0 ) );
    }

    void Engine::Mesh::loadGeometry( const Core::Vector3Array& positions,
                                     const std::vector<uint>& indices )
    {
        addData(VERTEX_POSITION, positions);
        m_indices = indices;
        m_isDirty = true;

        m_iboDirty = true;
        m_dirtyArray[VERTEX_POSITION] = true;
    }

    void Engine::Mesh::loadGeometry( const Core::Vector4Array& positions,
                                     const std::vector<uint>& indices )
    {
        addData(VERTEX_POSITION, positions);
        m_indices = indices;
        m_isDirty = true;

        m_iboDirty = true;
        m_dirtyArray[VERTEX_POSITION] = true;
    }

    void Engine::Mesh::loadGeometry(const Core::TriangleMesh& mesh)
    {
        std::vector<uint> indices;
        for (const auto& t : mesh.m_triangles)
        {
            indices.push_back(t[0]);
            indices.push_back(t[1]);
            indices.push_back(t[2]);
        }
        loadGeometry(mesh.m_vertices, indices);
        if (mesh.m_normals.size() > 0)
        {
            addData(VERTEX_NORMAL, mesh.m_normals);
        }
    }

    void Engine::Mesh::addData( const DataType& type, const Core::Vector3Array& data )
    {
        m_data[type].resize(data.size());
        for (uint i =0; i < data.size(); ++i)
        {
            m_data[type][i].head<3>() = data[i];
            m_data[type][i].w()       = 0.f;
        }
        m_isDirty = true;
        m_dirtyArray[type] = true;
    }

    void Engine::Mesh::addData( const DataType& type, const Core::Vector4Array& data )
    {
        m_data[type] = data;
        m_isDirty = true;
        m_dirtyArray[type] = true;
    }

    void Engine::Mesh::updateGL()
    {
        if ( !m_isDirty )
        {
            return;
        }

        if ( m_data[VERTEX_POSITION].empty() || m_indices.empty() )
        {
            LOG( logWARNING ) << "Either vertices or indices are empty arrays.";
            return;
        }

        if ( m_vao == 0 )
        {
            // Create VAO if it does not exist
            GL_ASSERT( glGenVertexArrays( 1, &m_vao ) );
        }

        // Bind it
        GL_ASSERT( glBindVertexArray( m_vao ) );

        // Common values for GL data functions.
#if defined CORE_USE_DOUBLE
        GLenum type = GL_DOUBLE;
#else
        GLenum type = GL_FLOAT;
#endif
        GLboolean normalized = GL_FALSE;
        GLuint size = 4;
        GLvoid* ptr = nullptr;

        for ( uint i = 0; i < m_data.size(); ++i )
        {
            // This vbo has not been created yet
            if ( m_vbos[i] == 0 && m_data[i].size() > 0 )
            {
                GL_ASSERT( glGenBuffers( 1, &m_vbos[i] ) );
                GL_ASSERT( glBindBuffer( GL_ARRAY_BUFFER, m_vbos[i] ) );
                GL_ASSERT( glBufferData( GL_ARRAY_BUFFER, m_data[i].size() * sizeof( Core::Vector4 ),
                                         m_data[i].data(), GL_DYNAMIC_DRAW ) );

                GL_ASSERT( glVertexAttribPointer( i, size, type, normalized,
                                                  sizeof( Core::Vector4 ), ptr ) );

                GL_ASSERT( glEnableVertexAttribArray( i ) );

                m_dirtyArray[i] = false;
            }

            if ( m_dirtyArray[i] == true && m_vbos[i] != 0 && m_data[i].size() > 0 )
            {
                GL_ASSERT( glBindBuffer( GL_ARRAY_BUFFER, m_vbos[i] ) );
                GL_ASSERT( glBufferData( GL_ARRAY_BUFFER, m_data[i].size() * sizeof( Core::Vector4 ),
                                         m_data[i].data(), GL_DYNAMIC_DRAW ) );

                m_dirtyArray[i] = false;
            }
        }

        // Indices has not been initialized yet
        if ( m_ibo == 0 )
        {
            GL_ASSERT( glGenBuffers( 1, &m_ibo ) );
            GL_ASSERT( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_ibo ) );
        }

        if ( m_iboDirty )
        {
            GL_ASSERT( glBufferData( GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof( uint ),
                                     m_indices.data(), GL_DYNAMIC_DRAW ) );
            m_iboDirty = false;
        }

        GL_ASSERT( glBindVertexArray( 0 ) );

        GL_CHECK_ERROR;
        m_isDirty = false;
    }

    std::shared_ptr<Engine::Mesh> Engine::Mesh::clone()
    {
        //std::shared_ptr<Mesh> mesh( new Mesh( m_name, m_renderMode ) );
        Mesh* mesh = new Mesh( m_name, m_renderMode );

        mesh->m_vao = m_vao;
        mesh->m_vbos = m_vbos;
        mesh->m_ibo = m_ibo;

        mesh->m_isDirty = true;
        mesh->m_iboDirty = false;
        mesh->m_dirtyArray = {{ false }};

        mesh->m_data = m_data;
        mesh->m_indices = m_indices;

        return std::shared_ptr<Mesh>( mesh );
    }

} // namespace Ra
