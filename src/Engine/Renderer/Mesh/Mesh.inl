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

} // namespace Engine
} // namespace Ra
