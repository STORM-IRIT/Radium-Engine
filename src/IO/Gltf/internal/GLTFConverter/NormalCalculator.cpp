#include <Core/Asset/GeometryData.hpp>
#include <IO/Gltf/internal/GLTFConverter/NormalCalculator.hpp>

namespace Ra {
namespace IO {
namespace GLTF {
using namespace Ra::Core;
using namespace Ra::Core::Asset;

void NormalCalculator::operator()( GeometryData* gdp, bool basic ) {

    auto& geo = gdp->getGeometry();
    const auto& [layerKey, layerBase] =
        geo.getFirstLayerOccurrence( Ra::Core::Geometry::TriangleIndexLayer::staticSemanticName );
    const auto& triangle = static_cast<const Ra::Core::Geometry::TriangleIndexLayer&>( layerBase );
    const auto& faces    = triangle.collection();
    const auto& vertices = geo.vertices();

    auto attribHandle =
        geo.addAttrib<Vector3>( getAttribName( Ra::Core::Geometry::MeshAttrib::VERTEX_NORMAL ) );
    auto& normals = geo.vertexAttribs().getDataWithLock( attribHandle );
    normals.clear();
    normals.resize( geo.vertices().size(), Vector3::Zero() );

    for ( const auto& t : faces ) {
        Vector3 n = getTriangleNormal( t, basic, vertices );
        for ( uint i = 0; i < 3; ++i ) {
            normals[t[i]] += n;
        }
    }
    normals.getMap().colwise().normalize();
    geo.vertexAttribs().unlock( attribHandle );
}

Vector3 NormalCalculator::getTriangleNormal( const Vector3ui& t,
                                             bool basic,
                                             const Vector3Array& vertices ) {
    auto p = vertices[t[0]];
    auto q = vertices[t[1]];
    auto r = vertices[t[2]];

    const Vector3 n = ( q - p ).cross( r - p );
    if ( n.isApprox( Vector3::Zero() ) ) { return Vector3::Zero(); }
    if ( basic ) { return ( n.normalized() ); }
    else { return ( n * 0.5 ); }
}

} // namespace GLTF
} // namespace IO
} // namespace Ra
