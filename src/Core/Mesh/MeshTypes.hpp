#ifndef RADIUMENGINE_MESHTYPES_HPP
#define RADIUMENGINE_MESHTYPES_HPP

#include <Core/RaCore.hpp>
#include <Core/Math/LinearAlgebra.hpp>

namespace Ra
{
    namespace Core
    {

        // todo : enclose this in a more specific namespace ?
        typedef Vector2ui Edge;
        typedef Vector3ui Triangle;
        typedef Vector4ui Quad;

        typedef uint QuadIdx;
        typedef uint TriangleIdx;
        typedef uint VertexIdx;
        typedef uint HalfEdgeIdx;

        enum { InvalidIdx = uint( -1 ) };
    }
}

#endif //RADIUMENGINE_MESHTYPES_HPP
