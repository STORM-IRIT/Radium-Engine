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
    using Handle  = Core::Geometry::TriangleMesh::Vec3AttribHandle;
    auto name     = getAttribName( type );
    Handle handle = m_mesh.getAttribHandle<Core::Vector3>( name );
    if ( !m_mesh.isValid( handle ) ) return m_dummy3;
    return m_mesh.getAttrib( handle ).data();
}

const Core::Vector4Array& Mesh::getData( const Mesh::Vec4Data& type ) const {
    using Handle  = Core::Geometry::TriangleMesh::Vec4AttribHandle;
    auto name     = getAttribName( type );
    Handle handle = m_mesh.getAttribHandle<Core::Vector4>( name );
    if ( !m_mesh.isValid( handle ) ) return m_dummy4;
    return m_mesh.getAttrib( handle ).data();
}

std::string Mesh::getAttribName( MeshData type ) {
    if ( type == VERTEX_POSITION ) return {"in_position"};
    if ( type == VERTEX_NORMAL ) return {"in_normal"};
    return {"indices but should not happend"};
}

std::string Mesh::getAttribName( Vec3Data type ) {
    if ( type == VERTEX_TANGENT ) return {"in_tangent"};
    if ( type == VERTEX_BITANGENT ) return {"in_bitangent"};
    if ( type == VERTEX_TEXCOORD ) return {"in_texcoord"};
    return {"invalid vec3 attr"};
}

std::string Mesh::getAttribName( Vec4Data type ) {
    if ( type == VERTEX_COLOR ) return {"in_color"};
    if ( type == VERTEX_WEIGHTS ) return {"in_weight"};
    if ( type == VERTEX_WEIGHT_IDX ) return {"in_weight_idx"};
    return {"invalid vec4 attr"};
}

// void Mesh::colorize( const Core::Utils::Color& color ) {
//    Core::Vector4Array colors( getTriangleMesh().vertices().size(), color );
//    addData( Engine::Mesh::VERTEX_COLOR, colors );
//}

} // namespace Engine
} // namespace Ra
