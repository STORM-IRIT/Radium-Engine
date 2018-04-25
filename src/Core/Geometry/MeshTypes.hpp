#ifndef RADIUMENGINE_MESHTYPES_HPP
#define RADIUMENGINE_MESHTYPES_HPP

#include <Core/Container/Index.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {
namespace Geometry {


// todo : enclose this in a more specific namespace ?
using Edge = Math::Vector2ui;
using Triangle = Math::Vector3ui;

using TriangleIdx = Container::Index;
using VertexIdx = Container::Index;
using HalfEdgeIdx = Container::Index;
} // namespace Geometry
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_MESHTYPES_HPP
