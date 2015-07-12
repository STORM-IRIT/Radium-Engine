#ifndef RADIUMENGINE_MESHTYPES_HPP
#define RADIUMENGINE_MESHTYPES_HPP

#include <CoreMacros.hpp>
#include <math/Vector.hpp>

namespace Ra
{
    // todo : enclose this in a more specific namespace ?
    typedef Vector2i Edge;
    typedef Vector3i Triangle;

    typedef uint TriangleIdx;
    typedef uint VertexIdx;
    typedef uint HalfEdgeIdx;

    enum {InvalidIdx = uint(-1)};
}

#endif //RADIUMENGINE_MESHTYPES_HPP
