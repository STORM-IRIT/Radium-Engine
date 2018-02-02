#ifndef RADIUMENGINE_MESHTYPES_HPP
#define RADIUMENGINE_MESHTYPES_HPP

#include <Core/RaCore.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Index/Index.hpp>

namespace Ra
{
    namespace Core
    {

        // todo : enclose this in a more specific namespace ?
        typedef Vector2ui Edge;
        typedef Vector3ui Triangle;

        using TriangleIdx = Index;
        using VertexIdx   = Index;
        using HalfEdgeIdx = Index;
    }
}

#endif //RADIUMENGINE_MESHTYPES_HPP
