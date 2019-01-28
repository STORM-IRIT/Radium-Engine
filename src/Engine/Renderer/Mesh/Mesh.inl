#include "Mesh.hpp"

namespace Ra {
namespace Engine {

void Mesh::setRenderMode( MeshRenderMode mode ) {
    m_renderMode = mode;
    updatePickingRenderMode();
}

const Core::Geometry::AbstractGeometry& Mesh::getGeometry() const {
    return m_mesh;
}

Core::Geometry::AbstractGeometry& Mesh::getGeometry() {
    return m_mesh;
}

const Core::Geometry::TriangleMesh& Mesh::getTriangleMesh() const {
    return m_mesh;
}

Core::Geometry::TriangleMesh& Mesh::getTriangleMesh() {
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

void Mesh::setDirty( const Mesh::Vec3Data& type, bool handleAdded ) {
    if ( handleAdded )
    {
        m_v3DataHandle[int( type )] =
            m_mesh.getAttribHandle<Core::Vector3>( getAttribName( type ) );
    }
    m_dataDirty[MAX_MESH + type] = true;
    m_isDirty = true;
}

void Mesh::setDirty( const Mesh::Vec4Data& type, bool handleAdded ) {
    if ( handleAdded )
    {
        m_v4DataHandle[int( type )] =
            m_mesh.getAttribHandle<Core::Vector4>( getAttribName( type ) );
    }
    m_dataDirty[MAX_MESH + MAX_VEC3 + type] = true;
    m_isDirty = true;
}

std::string Mesh::getAttribName( Vec3Data type ) {
    return std::string( "Vec3_attr_" ) + std::to_string( uint( type ) );
}

std::string Mesh::getAttribName( Vec4Data type ) {
    return std::string( "Vec4_attr_" ) + std::to_string( uint( type ) );
}

// void Mesh::colorize( const Core::Utils::Color& color ) {
//    Core::Vector4Array colors( getTriangleMesh().vertices().size(), color );
//    addData( Engine::Mesh::VERTEX_COLOR, colors );
//}

} // namespace Engine
} // namespace Ra
