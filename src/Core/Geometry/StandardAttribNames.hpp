#pragma once

#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

/// List of standard vertex attributes.
/// Corresponding standard vertex attribute names are obtained with g_attribName
/// Information which is in the mesh geometry
enum MeshAttrib : uint {
    VERTEX_POSITION,   ///< Vertex positions
    VERTEX_NORMAL,     ///< Vertex normals
    VERTEX_TANGENT,    ///< Vertex tangent 1
    VERTEX_BITANGENT,  ///< Vertex tangent 2
    VERTEX_TEXCOORD,   ///< U,V  texture coords (last coordinate not used)
    VERTEX_COLOR,      ///< RGBA color.
    VERTEX_WEIGHTS,    ///< Skinning weights (not used)
    VERTEX_WEIGHT_IDX, ///< Associated weight bones

    MAX_DATA
};

///@{
/// Get the name expected for a given attrib.
static constexpr char const* g_attribName[MeshAttrib::MAX_DATA] = { "in_position",
                                                                    "in_normal",
                                                                    "in_tangent",
                                                                    "in_bitangent",
                                                                    "in_texcoord",
                                                                    "in_color",
                                                                    "in_weight",
                                                                    "in_weight_idx" };
///@}

} // namespace Geometry
} // namespace Core
} // namespace Ra
