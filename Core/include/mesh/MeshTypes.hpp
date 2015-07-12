#ifndef RADIUMENGINE_MESH_TYPES_HPP
#define RADIUMENGINE_MESH_TYPES_HPP

#include <Eigen/Core>
#include <CoreMacros.hpp>

namespace Ra
{
    // todo : enclose this in a more specific namespace ?
    typedef Eigen::Vector2i Edge;
    typedef Eigen::Vector3i Triangle;

    typedef uint TriangleIdx;
    typedef uint VertexIdx;
    typedef uint HalfEdgeIdx;

    enum {InvalidIdx = uint(-1)};
}

#endif //RADIUMENGINE_MESH_TYPES_HPP
