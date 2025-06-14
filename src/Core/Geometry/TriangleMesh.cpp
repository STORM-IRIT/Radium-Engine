#include <Core/Containers/VectorArray.hpp>
#include <Core/Geometry/StandardAttribNames.hpp>
#include <Core/Geometry/TriangleMesh.hpp>
#include <Core/Types.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

/****/

bool AttribArrayGeometry::append( const AttribArrayGeometry& other ) {
    // check same attributes through names
    if ( !m_vertexAttribs.hasSameAttribs( other.m_vertexAttribs ) ) return false;
    // Deal with all attributes the same way (vertices and normals too)
    other.m_vertexAttribs.for_each_attrib( [this]( const auto& attr ) {
        if ( attr->isFloat() ) this->append_attrib<Scalar>( attr );
        if ( attr->isVector2() ) this->append_attrib<Vector2>( attr );
        if ( attr->isVector3() ) this->append_attrib<Vector3>( attr );
        if ( attr->isVector4() ) this->append_attrib<Vector4>( attr );
    } );

    invalidateAabb();

    return true;
}

void AttribArrayGeometry::clearAttributes() {
    PointAttribHandle::Container v  = vertices();
    NormalAttribHandle::Container n = normals();
    m_vertexAttribs.clear();
    initDefaultAttribs();
    setVertices( v );
    setNormals( n );

    invalidateAabb();
}

void AttribArrayGeometry::colorize( const Utils::Color& color ) {
    auto colorAttribHandle = addAttrib<Core::Vector4>( getAttribName( MeshAttrib::VERTEX_COLOR ) );
    getAttrib( colorAttribHandle ).setData( Vector4Array( vertices().size(), color ) );
}

} // namespace Geometry
} // namespace Core
} // namespace Ra
