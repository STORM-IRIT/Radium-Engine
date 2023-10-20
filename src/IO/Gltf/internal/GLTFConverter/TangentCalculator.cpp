#include <Core/Types.hpp>
#include <IO/Gltf/internal/GLTFConverter/TangentCalculator.hpp>

namespace Ra {
namespace IO {
namespace GLTF {

using namespace Ra::Core;
using namespace Ra::Core::Asset;

// Initialize MikkTSpaceInterface with callbacks and run calculator.
void TangentCalculator::operator()( GeometryData* gdp, bool basic ) {
    SMikkTSpaceInterface iface;
    iface.m_getNumFaces          = getNumFaces;
    iface.m_getNumVerticesOfFace = getNumVerticesOfFace;
    iface.m_getPosition          = getPosition;
    iface.m_getNormal            = getNormal;
    iface.m_getTexCoord          = getTexCoord;
    iface.m_setTSpaceBasic       = basic ? setTSpaceBasic : nullptr;
    iface.m_setTSpace            = basic ? nullptr : setTSpace;

    auto& geo = gdp->getGeometry();
    // According to gltf specification, this should be Vector4, but RAdium requires Vector3 as
    // tangents
    auto attribHandle =
        geo.addAttrib<Vector3>( getAttribName( Ra::Core::Geometry::MeshAttrib::VERTEX_TANGENT ) );
    auto& tangents = geo.vertexAttribs().getDataWithLock( attribHandle );
    tangents.resize( geo.vertices().size() );
    geo.vertexAttribs().unlock( attribHandle );
    if ( !basic ) {
        auto attribBiTangents = geo.addAttrib<Vector3>(
            getAttribName( Ra::Core::Geometry::MeshAttrib::VERTEX_BITANGENT ) );
        auto& bitangents = geo.vertexAttribs().getDataWithLock( attribBiTangents );
        bitangents.resize( geo.vertices().size() );
        geo.vertexAttribs().unlock( attribBiTangents );
    }
    SMikkTSpaceContext context;
    context.m_pInterface = &iface;
    context.m_pUserData  = gdp;

    genTangSpaceDefault( &context );

    // Do we need to renormalize/orthogonalize ?
}

// Return number of primitives in the geometry.
int TangentCalculator::getNumFaces( const SMikkTSpaceContext* context ) {
    // Cast the void pointer from context data to our GeometryData pointer.
    auto gdp = static_cast<GeometryData*>( context->m_pUserData );
    return int( gdp->getPrimitiveCount() );
}

// Return number of vertices in the primitive given by index.
// Right now, GLTF only manage triangle meshes
int TangentCalculator::getNumVerticesOfFace( const SMikkTSpaceContext* /*context*/,
                                             int /*primnum*/ ) {
    return 3;
}

// Write 3-float position of the vertex's point.
void TangentCalculator::getPosition( const SMikkTSpaceContext* context,
                                     float outpos[],
                                     int primnum,
                                     int vtxnum ) {
    auto gdp  = static_cast<GeometryData*>( context->m_pUserData );
    auto& geo = gdp->getGeometry();
    const auto& [layerKey, layerBase] =
        geo.getFirstLayerOccurrence( Ra::Core::Geometry::TriangleIndexLayer::staticSemanticName );
    const auto& triangle = static_cast<const Ra::Core::Geometry::TriangleIndexLayer&>( layerBase );
    const auto& face     = triangle.collection()[primnum];
    const auto& vertex   = geo.vertices()[face[vtxnum]];

    // Write into the input 3-float array.
    outpos[0] = vertex[0];
    outpos[1] = vertex[1];
    outpos[2] = vertex[2];
}

// Write 3-float vertex normal.
void TangentCalculator::getNormal( const SMikkTSpaceContext* context,
                                   float outnormal[],
                                   int primnum,
                                   int vtxnum ) {
    auto gdp  = static_cast<GeometryData*>( context->m_pUserData );
    auto& geo = gdp->getGeometry();
    const auto& [layerKey, layerBase] =
        geo.getFirstLayerOccurrence( Ra::Core::Geometry::TriangleIndexLayer::staticSemanticName );
    const auto& triangle = static_cast<const Ra::Core::Geometry::TriangleIndexLayer&>( layerBase );
    const auto& face     = triangle.collection()[primnum];
    const auto& normal   = geo.normals()[face[vtxnum]];

    outnormal[0] = normal[0];
    outnormal[1] = normal[1];
    outnormal[2] = normal[2];
}

// Write 2-float vertex uv.
void TangentCalculator::getTexCoord( const SMikkTSpaceContext* context,
                                     float outuv[],
                                     int primnum,
                                     int vtxnum ) {
    auto gdp  = static_cast<GeometryData*>( context->m_pUserData );
    auto& geo = gdp->getGeometry();
    const auto& [layerKey, layerBase] =
        geo.getFirstLayerOccurrence( Ra::Core::Geometry::TriangleIndexLayer::staticSemanticName );
    const auto& triangle = static_cast<const Ra::Core::Geometry::TriangleIndexLayer&>( layerBase );
    const auto& face     = triangle.collection()[primnum];

    auto attribHandle =
        geo.addAttrib<Vector3>( getAttribName( Ra::Core::Geometry::MeshAttrib::VERTEX_TEXCOORD ) );
    const auto& texCoords = geo.vertexAttribs().getData( attribHandle );
    const auto& uv        = texCoords[face[vtxnum]];

    outuv[0] = uv[0];
    outuv[1] = uv[1];
}

// Compute and set attributes on the geometry vertex. Basic version.
void TangentCalculator::setTSpaceBasic( const SMikkTSpaceContext* context,
                                        const float tangentu[],
                                        float sign,
                                        int primnum,
                                        int vtxnum ) {
    auto gdp  = static_cast<GeometryData*>( context->m_pUserData );
    auto& geo = gdp->getGeometry();
    const auto& [layerKey, layerBase] =
        geo.getFirstLayerOccurrence( Ra::Core::Geometry::TriangleIndexLayer::staticSemanticName );
    const auto& triangle = static_cast<const Ra::Core::Geometry::TriangleIndexLayer&>( layerBase );
    const auto& face     = triangle.collection()[primnum];

    auto attribHandle =
        geo.addAttrib<Vector3>( getAttribName( Ra::Core::Geometry::MeshAttrib::VERTEX_TANGENT ) );
    Vector3Array& tangents = geo.vertexAttribs().getDataWithLock( attribHandle );
    int tgtindex           = face[vtxnum];

    // well, some liberties against the spec and the MKKTSpace algo ...
    // tangents[tgtindex] = tangents[tgtindex] + Ra::Core::Vector3(tangentu[0], tangentu[1],
    // tangentu[2]) * sign;
    tangents[tgtindex] = Vector3( tangentu[0], tangentu[1], tangentu[2] ) * sign;

    geo.vertexAttribs().unlock( attribHandle );
}

// Compute and set attributes on the geometry vertex.
void TangentCalculator::setTSpace( const SMikkTSpaceContext* context,
                                   const float tangentu[],
                                   const float tangentv[],
                                   const float magu,
                                   const float magv,
                                   const tbool /*keep*/,
                                   const int primnum,
                                   const int vtxnum ) {
    auto gdp  = static_cast<GeometryData*>( context->m_pUserData );
    auto& geo = gdp->getGeometry();
    auto [layerKey, layerBase] =
        geo.getFirstLayerOccurrence( Ra::Core::Geometry::TriangleIndexLayer::staticSemanticName );
    const auto& triangle = static_cast<const Ra::Core::Geometry::TriangleIndexLayer&>( layerBase );
    const auto& face     = triangle.collection()[primnum];

    auto attribTangents =
        geo.addAttrib<Vector3>( getAttribName( Ra::Core::Geometry::MeshAttrib::VERTEX_TANGENT ) );
    auto attribBiTangents =
        geo.addAttrib<Vector3>( getAttribName( Ra::Core::Geometry::MeshAttrib::VERTEX_BITANGENT ) );
    auto unlocker = geo.vertexAttribs().getScopedLockState();

    Vector3Array& tangents   = geo.vertexAttribs().getDataWithLock( attribTangents );
    Vector3Array& bitangents = geo.vertexAttribs().getDataWithLock( attribBiTangents );
    int tgtindex             = face[vtxnum];

    tangents[tgtindex]   = Vector3( tangentu[0], tangentu[1], tangentu[2] ) * magu;
    bitangents[tgtindex] = Vector3( tangentv[0], tangentv[1], tangentv[2] ) * magv;
}

} // namespace GLTF
} // namespace IO
} // namespace Ra
