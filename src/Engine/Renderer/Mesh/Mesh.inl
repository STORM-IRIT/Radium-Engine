#include "Mesh.hpp"

namespace Ra {
namespace Engine {

const std::string& Mesh::getName() const {
    return m_name;
}

void Mesh::setRenderMode( MeshRenderMode mode ) {
    m_renderMode = mode;
}

const Core::TriangleMesh& Mesh::getGeometry() const {
    return m_mesh;
}
Core::TriangleMesh& Mesh::getGeometry() {
    return m_mesh;
}

const Core::Vector3Array& Mesh::getData( const Mesh::Vec3Data& type ) const {
    const int index = static_cast<uint>( type );
    CORE_ASSERT( m_v3DataHandle[index].isValid(), "Attrib must be initialized" );
    return m_mesh.attribManager().getAttrib( m_v3DataHandle[index] ).data();
}

const Core::Vector4Array& Mesh::getData( const Mesh::Vec4Data& type ) const {
    const int index = static_cast<uint>( type );
    if ( !m_v4DataHandle[index].isValid() )
        return m_dummy;
    return m_mesh.attribManager().getAttrib( m_v4DataHandle[index] ).data();
}

Core::Vector3Array& Mesh::getData( const Mesh::Vec3Data& type ) {
    const int index = static_cast<uint>( type );
    CORE_ASSERT( m_v3DataHandle[index].isValid(), "Attrib must be initialized" );
    return m_mesh.attribManager().getAttrib( m_v3DataHandle[index] ).data();
}

Core::Vector4Array& Mesh::getData( const Mesh::Vec4Data& type ) {
    const int index = static_cast<uint>( type );
    if ( !m_v4DataHandle[index].isValid() )
        return m_dummy;
    return m_mesh.attribManager().getAttrib( m_v4DataHandle[index] ).data();
}

void Mesh::setDirty( const Mesh::MeshData& type ) {
    m_dataDirty[type] = true;
    m_isDirty = true;
}
void Mesh::setDirty( const Mesh::Vec3Data& type ) {
    m_dataDirty[MAX_MESH + type] = true;
    m_isDirty = true;
}
void Mesh::setDirty( const Mesh::Vec4Data& type ) {
    m_dataDirty[MAX_MESH + MAX_VEC3 + type] = true;
    m_isDirty = true;
}


// Template parameter must be a Core::VectorNArray
template <typename VecArray>
void Mesh::sendGLData( const VecArray& arr, const uint vboIdx ) {

#ifdef CORE_USE_DOUBLE
    GLenum type = GL_DOUBLE;
#else
    GLenum type = GL_FLOAT;
#endif
    constexpr GLuint size = VecArray::Vector::RowsAtCompileTime;
    const GLboolean normalized = GL_FALSE;
    constexpr GLint64 ptr = 0;

    // This vbo has not been created yet
    if ( m_vbos[vboIdx] == 0 && arr.size() > 0 )
    {
        GL_ASSERT( glGenBuffers( 1, &m_vbos[vboIdx] ) );
        GL_ASSERT( glBindBuffer( GL_ARRAY_BUFFER, m_vbos[vboIdx] ) );

        // Use (vboIdx - 1) as attribute index because vbo 0 is actually ibo.
        GL_ASSERT( glVertexAttribPointer( vboIdx - 1, size, type, normalized,
                                          sizeof( typename VecArray::Vector ), (GLvoid*)ptr ) );

        GL_ASSERT( glEnableVertexAttribArray( vboIdx - 1 ) );
        // Set dirty as true to send data, see below
        m_dataDirty[vboIdx] = true;
    }

    if ( m_dataDirty[vboIdx] == true && m_vbos[vboIdx] != 0 && arr.size() > 0 )
    {
        GL_ASSERT( glBindBuffer( GL_ARRAY_BUFFER, m_vbos[vboIdx] ) );
        GL_ASSERT( glBufferData( GL_ARRAY_BUFFER, arr.size() * sizeof( typename VecArray::Vector ),
                                 arr.data(), GL_DYNAMIC_DRAW ) );
        m_dataDirty[vboIdx] = false;
    }
}

} // namespace Engine
} // namespace Ra
