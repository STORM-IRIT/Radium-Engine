#include "Mesh.hpp"

#include <globjects/Buffer.h>

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

template <typename Type, typename Vector>
void Mesh::addData( const Type& type, const Core::VectorArray<Vector>& data ) {
    addData( getAttribName( type ), data );
}

template <typename Vector>
void Mesh::addData( const std::string& name, const Core::VectorArray<Vector>& data ) {
    if ( !data.empty() )
    {
        // if this is a new attrib, we need to add observer
        bool alreadyPresent = m_mesh.hasAttrib( name );

        // add attrib return the corresponding attrib if already present.
        Core::Utils::AttribHandle<Vector> handle = m_mesh.addAttrib<Vector>( name );

        //    if ( data.size() != 0 && m_mesh.isValid( handle ) )
        auto itr = m_handleToBuffer.find( name );

        if ( itr == m_handleToBuffer.end() )
        {
            m_handleToBuffer[name] = m_dataDirty.size();
            m_dataDirty.push_back( true );
            m_vbos.emplace_back( nullptr );
        }

        if ( !alreadyPresent )
        {
            auto idx = m_handleToBuffer[name];
            m_mesh.getAttrib( handle ).attach( AttribObserver( this, idx ) );
        }
        m_mesh.getAttrib( handle ).setData( data );

        auto idx = m_handleToBuffer[name];
        CORE_ASSERT( m_dataDirty[idx] == true, "notify failed to set dirty" );
    }
}

} // namespace Engine
} // namespace Ra
