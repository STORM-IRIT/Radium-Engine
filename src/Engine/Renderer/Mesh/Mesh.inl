#include "Mesh.hpp"

#include <globjects/Buffer.h>

// #include <numeric>

// #include <Core/Utils/Attribs.hpp>
// #include <Core/Utils/Log.hpp>
#include <Engine/Renderer/OpenGL/OpenGL.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>

#include <globjects/Program.h>

#include <globjects/Buffer.h>
#include <globjects/VertexArray.h>
#include <globjects/VertexAttributeBinding.h>

namespace Ra {
namespace Engine {

void VaoDisplayable::setRenderMode( MeshRenderMode mode ) {
    m_renderMode = mode;
    updatePickingRenderMode();
}

template <typename CoreGeometry>
const Ra::Core::Geometry::AbstractGeometry&
DisplayableGeometry<CoreGeometry>::getAbstractGeometry() const {
    return m_mesh;
}

template <typename CoreGeometry>
Ra::Core::Geometry::AbstractGeometry& DisplayableGeometry<CoreGeometry>::getAbstractGeometry() {
    return m_mesh;
}

template <typename CoreGeometry>
const CoreGeometry& DisplayableGeometry<CoreGeometry>::getTriangleMesh() const {
    return m_mesh;
}

template <typename CoreGeometry>
CoreGeometry& DisplayableGeometry<CoreGeometry>::getTriangleMesh() {
    return m_mesh;
}

std::string VaoDisplayable::getAttribName( MeshData type ) {
    if ( type == VERTEX_POSITION ) return {"in_position"};
    if ( type == VERTEX_NORMAL ) return {"in_normal"};
    return {"indices but should not happend"};
}

std::string VaoDisplayable::getAttribName( Vec3Data type ) {
    if ( type == VERTEX_TANGENT ) return {"in_tangent"};
    if ( type == VERTEX_BITANGENT ) return {"in_bitangent"};
    if ( type == VERTEX_TEXCOORD ) return {"in_texcoord"};
    return {"invalid vec3 attr"};
}

std::string VaoDisplayable::getAttribName( Vec4Data type ) {
    if ( type == VERTEX_COLOR ) return {"in_color"};
    if ( type == VERTEX_WEIGHTS ) return {"in_weight"};
    if ( type == VERTEX_WEIGHT_IDX ) return {"in_weight_idx"};
    return {"invalid vec4 attr"};
}

} // namespace Engine
} // namespace Ra
