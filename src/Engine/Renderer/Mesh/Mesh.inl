
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

const Core::Container::Vector3Array& Mesh::getData( const Mesh::Vec3Data& type ) const {
    return m_v3Data[static_cast<uint>( type )];
}

const Core::Container::Vector4Array& Mesh::getData( const Mesh::Vec4Data& type ) const {
    return m_v4Data[static_cast<uint>( type )];
}

Core::Container::Vector3Array& Mesh::getData( const Mesh::Vec3Data& type ) {
    return m_v3Data[static_cast<uint>( type )];
}

Core::Container::Vector4Array& Mesh::getData( const Mesh::Vec4Data& type ) {
    return m_v4Data[static_cast<uint>( type )];
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
