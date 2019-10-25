#include "TriangleMesh.hpp"
#include <Core/Geometry/RayCast.hpp>
#include <Core/Geometry/TriangleOperation.hpp> // triangleArea
#include <Core/Types.hpp>

#include <array>
#include <vector>

namespace Ra {
namespace Core {
namespace Geometry {

/****/

bool AttribArrayGeometry::append( const AttribArrayGeometry& other ) {
    // check same attributes through names
    if ( !m_vertexAttribs.hasSameAttribs( other.m_vertexAttribs ) ) return false;
    // Deal with all attributes the same way (vertices and normals too)
    other.m_vertexAttribs.for_each_attrib( [this]( const auto& attr ) {
        if ( attr->isFloat() ) this->append_attrib<float>( attr );
        if ( attr->isVec2() ) this->append_attrib<Vector2>( attr );
        if ( attr->isVec3() ) this->append_attrib<Vector3>( attr );
        if ( attr->isVec4() ) this->append_attrib<Vector4>( attr );
    } );

    return true;
}

void AttribArrayGeometry::clearAttributes() {
    PointAttribHandle::Container v  = vertices();
    NormalAttribHandle::Container n = normals();
    m_vertexAttribs.clear();
    initDefaultAttribs();
    setVertices( v );
    setNormals( n );
}

void AttribArrayGeometry::colorize( const Utils::Color& color ) {
    static const std::string colorAttribName( "in_color" );
    auto colorAttribHandle = addAttrib<Core::Vector4>( colorAttribName );
    getAttrib( colorAttribHandle ).setData( Vector4Array( vertices().size(), color ) );
}

} // namespace Geometry
} // namespace Core
} // namespace Ra
