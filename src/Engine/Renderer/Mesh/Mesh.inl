#include "Mesh.hpp"

namespace Ra {
namespace Engine {

const std::string& Mesh::getName() const {
    return m_name;
}

void Mesh::setRenderMode( MeshRenderMode mode ) {
    m_renderMode = mode;
}

const Core::Geometry::TriangleMesh& Mesh::getGeometry() const {
    return m_mesh;
}

Core::Geometry::TriangleMesh& Mesh::getGeometry() {
    return m_mesh;
}

const Core::Vector3Array& Mesh::getData( const Mesh::Vec3Data& type ) const {
    const int index = static_cast<uint>( type );
    const auto& h = m_v3DataHandle[index];
    if ( !m_mesh.isValid( h ) )
        return m_dummy3;
    return m_mesh.getAttrib( h ).data();
}

const Core::Vector4Array& Mesh::getData( const Mesh::Vec4Data& type ) const {
    const int index = static_cast<uint>( type );
    const auto& h = m_v4DataHandle[index];
    if ( !m_mesh.isValid( h ) )
        return m_dummy4;
    return m_mesh.getAttrib( h ).data();
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

void Mesh::colorize( const Core::Color& color ) {
    Core::Vector4Array colors( getGeometry().vertices().size(), color );
    addData( Engine::Mesh::VERTEX_COLOR, colors );
}

} // namespace Engine
} // namespace Ra
