#include <Engine/Renderer/Mesh/Mesh.hpp>

#include <numeric>

#include <Core/Mesh/HalfEdge.hpp>
#include <Core/Mesh/MeshUtils.hpp>
#include <Engine/Renderer/OpenGL/OpenGL.hpp>
namespace Ra {
namespace Engine {

// Template parameter must be a Core::VectorNArray
template <typename ContainedType>
inline void sendGLData( Ra::Engine::Mesh* mesh,
                        const Ra::Core::VectorArray<ContainedType>& arr,
                        const uint vboIdx ) {
    using VecArray = Ra::Core::VectorArray<ContainedType>;
#ifdef CORE_USE_DOUBLE
    GLenum type = GL_DOUBLE;
#else
    GLenum type = GL_FLOAT;
#endif
    constexpr GLuint size = VecArray::Vector::RowsAtCompileTime;
    const GLboolean normalized = GL_FALSE;
    constexpr GLint64 ptr = 0;

    // This vbo has not been created yet
    if (mesh->m_vbos[vboIdx] == 0 && arr.size() > 0)
    {
        GL_ASSERT(glGenBuffers(1, &(mesh->m_vbos[vboIdx])));
        GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, mesh->m_vbos[vboIdx]));

        // Use (vboIdx - 1) as attribute index because vbo 0 is actually ibo.
        GL_ASSERT(glVertexAttribPointer(vboIdx - 1, size, type, normalized,
            sizeof(typename VecArray::Vector), (GLvoid*)ptr));

        GL_ASSERT(glEnableVertexAttribArray(vboIdx - 1));
        // Set dirty as true to send data, see below
        mesh->m_dataDirty[vboIdx] = true;
    }

    if (mesh->m_dataDirty[vboIdx] == true && mesh->m_vbos[vboIdx] != 0 && arr.size() > 0)
    {
        GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, mesh->m_vbos[vboIdx]));
        GL_ASSERT(glBufferData(GL_ARRAY_BUFFER, arr.size() * sizeof(typename VecArray::Vector),
            arr.data(), GL_DYNAMIC_DRAW));
        mesh->m_dataDirty[vboIdx] = false;
    }
} // sendGLData

// Dirty is initializes as false so that we do not create the vao while
// we have no data to send to the gpu.
Mesh::Mesh( const std::string& name, MeshRenderMode renderMode ) :
    m_name( name ),
    m_vao( 0 ),
    m_renderMode( renderMode ),
    m_numElements( 0 ),
    m_isDirty( false ) {
    CORE_ASSERT( m_renderMode == RM_POINTS || m_renderMode == RM_LINES ||
                     m_renderMode == RM_LINE_LOOP || m_renderMode == RM_LINE_STRIP ||
                     m_renderMode == RM_TRIANGLES || m_renderMode == RM_TRIANGLE_STRIP ||
                     m_renderMode == RM_TRIANGLE_FAN || m_renderMode == RM_LINES_ADJACENCY ||
                     m_renderMode == RM_LINE_STRIP_ADJACENCY,
                 "Unsupported render mode" );
}

Mesh::~Mesh() {
    if ( m_vao != 0 )
    {
        GL_ASSERT( glDeleteVertexArrays( 1, &m_vao ) );

        for ( auto& vbo : m_vbos )
        {
            if ( vbo != 0 )
            {
                glDeleteBuffers( 1, &vbo );
            }
        }
    }
}

void Mesh::render() {
    if ( m_vao != 0 )
    {
        GL_ASSERT( glBindVertexArray( m_vao ) );
        GL_ASSERT( glDrawElements( static_cast<GLenum>( m_renderMode ), m_numElements,
                                   GL_UNSIGNED_INT, (void*)0 ) );
    }
}

void Mesh::loadGeometry( const Core::TriangleMesh& mesh ) {
    m_mesh = mesh;

    if ( m_mesh.m_triangles.empty() )
    {
        m_numElements = mesh.vertices().size();
        m_renderMode = RM_POINTS;
    }
    else
        m_numElements = mesh.m_triangles.size() * 3;

    for ( uint i = 0; i < MAX_MESH; ++i )
    {
        m_dataDirty[i] = true;
    }
    m_isDirty = true;
}

void Mesh::updateMeshGeometry( MeshData type, const Core::Vector3Array& data ) {
    if ( type == VERTEX_POSITION )
        m_mesh.vertices() = data;
    if ( type == VERTEX_NORMAL )
        m_mesh.normals() = data;
    m_dataDirty[static_cast<uint>( type )] = true;
    m_isDirty = true;
}

void Mesh::loadGeometry( const Core::Vector3Array& vertices, const std::vector<uint>& indices ) {
    // Do not remove this function to force everyone to use triangle mesh.
    //  ... because we have some line meshes as well...
    const uint nIdx = indices.size();

    if ( indices.empty() )
    {
        m_numElements = vertices.size();
        m_renderMode = RM_POINTS;
    }
    else
        m_numElements = nIdx;
    m_mesh.vertices() = vertices;

    // Check that when loading a triangle mesh we actually have triangles or lines.
    CORE_ASSERT( m_renderMode != GL_TRIANGLES || nIdx % 3 == 0,
                 "There should be 3 indices per triangle " );
    CORE_ASSERT( m_renderMode != GL_LINES || nIdx % 2 == 0, "There should be 2 indices per line" );
    CORE_ASSERT( m_renderMode != GL_LINES_ADJACENCY || nIdx % 4 == 0,
                 "There should be 4 indices per line adjacency" );

    for ( uint i = 0; i < indices.size(); i = i + 3 )
    {
        // We store all indices in order. This means that for lines we have
        // (L00, L01, L10), (L11, L20, L21) etc. We fill the missing by wrapping around indices.
        m_mesh.m_triangles.push_back(
            {indices[i], indices[( i + 1 ) % nIdx], indices[( i + 2 ) % nIdx]} );
    }

    // Mark mesh as dirty.
    for ( uint i = 0; i < MAX_MESH; ++i )
    {
        m_dataDirty[i] = true;
    }
    m_isDirty = true;
}

bool Mesh::addData( const Vec3Data& type, const Core::Vector3Array& data ) {
    const int index = static_cast<uint>( type );
    auto handle = m_v3DataHandle[index];
    if ( data.size() != 0 && handle.isValid() )
    {
        m_dataDirty[MAX_MESH + index] = true;
        m_isDirty = true;
        return true;
    }
    return false;
}

bool Mesh::addData( const Vec4Data& type, const Core::Vector4Array& data ) {
    const int index = static_cast<uint>(type);
    auto handle = m_v4DataHandle[index];
    if ( data.size() != 0 && handle.isValid() )
    {
        m_dataDirty[MAX_MESH + MAX_VEC3 + index] = true;
        m_isDirty = true;
        return true;
    }
    return false;
}

void Mesh::updateGL() {
    if ( m_isDirty )
    {
        // Check that our dirty bits are consistent.
        ON_ASSERT( bool dirtyTest = false; for ( const auto& d
                                                 : m_dataDirty ) { dirtyTest = dirtyTest || d; } );
        CORE_ASSERT( dirtyTest == m_isDirty, "Dirty flags inconsistency" );

        CORE_ASSERT( !( m_mesh.vertices().empty() ), "No vertex." );

        if ( m_vao == 0 )
        {
            // Create VAO if it does not exist
            GL_ASSERT( glGenVertexArrays( 1, &m_vao ) );
        }

        // Bind it
        GL_ASSERT( glBindVertexArray( m_vao ) );

        if ( m_vbos[INDEX] == 0 )
        {
            GL_ASSERT( glGenBuffers( 1, &m_vbos[INDEX] ) );
            GL_ASSERT( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_vbos[INDEX] ) );
        }
        if ( m_dataDirty[INDEX] )
        {
            if ( m_renderMode == RM_POINTS )
            {
                std::vector<int> indices( m_numElements );
                std::iota( indices.begin(), indices.end(), 0 );
                GL_ASSERT( glBufferData( GL_ELEMENT_ARRAY_BUFFER, m_numElements * sizeof( int ),
                                         indices.data(), GL_DYNAMIC_DRAW ) );
            }
            else
            {
                GL_ASSERT( glBufferData( GL_ELEMENT_ARRAY_BUFFER,
                                         m_mesh.m_triangles.size() * sizeof( Ra::Core::Triangle ),
                                         m_mesh.m_triangles.data(), GL_DYNAMIC_DRAW ) );
            }
            m_dataDirty[INDEX] = false;
        }

        // Geometry data
        sendGLData( this, m_mesh.vertices(), VERTEX_POSITION );
        sendGLData( this, m_mesh.normals(), VERTEX_NORMAL );

        // Vec3 data

        if ( m_v3DataHandle[VERTEX_TANGENT].isValid() )
            sendGLData( this, m_mesh.attribManager().getAttrib( m_v3DataHandle[VERTEX_TANGENT] ).data(),
                        MAX_MESH + VERTEX_TANGENT );
        if ( m_v3DataHandle[VERTEX_BITANGENT].isValid() )
            sendGLData( this, m_mesh.attribManager().getAttrib( m_v3DataHandle[VERTEX_BITANGENT] ).data(),
                        MAX_MESH + VERTEX_BITANGENT );
        if ( m_v3DataHandle[VERTEX_TEXCOORD].isValid() )
            sendGLData( this, m_mesh.attribManager().getAttrib( m_v3DataHandle[VERTEX_TEXCOORD] ).data(),
                        MAX_MESH + VERTEX_TEXCOORD );

        // Vec4 data
        if ( m_v4DataHandle[VERTEX_COLOR].isValid() )
            sendGLData( this, m_mesh.attribManager().getAttrib( m_v4DataHandle[VERTEX_COLOR] ).data(),
                        MAX_MESH + MAX_VEC3 + VERTEX_COLOR );

        if ( m_v4DataHandle[VERTEX_WEIGHTS].isValid() )
            sendGLData( this, m_mesh.attribManager().getAttrib( m_v4DataHandle[VERTEX_WEIGHTS] ).data(),
                        MAX_MESH + MAX_VEC3 + VERTEX_WEIGHTS );

        if ( m_v4DataHandle[VERTEX_WEIGHT_IDX].isValid() )
            sendGLData( this,
                m_mesh.attribManager().getAttrib( m_v4DataHandle[VERTEX_WEIGHT_IDX] ).data(),
                MAX_MESH + MAX_VEC3 + VERTEX_WEIGHT_IDX );
        GL_ASSERT( glBindVertexArray( 0 ) );
        GL_CHECK_ERROR;
        m_isDirty = false;
    }
}

} // namespace Engine
} // namespace Ra
