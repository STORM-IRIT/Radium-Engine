#pragma once
#include <Dataflow/Core/Nodes/Functionals/FilterNode.hpp>
#include <Dataflow/Core/Nodes/Functionals/FunctionNode.hpp>
#include <Dataflow/Core/Nodes/Functionals/ReduceNode.hpp>
#include <Dataflow/Core/Nodes/Functionals/TransformNode.hpp>

#include <Dataflow/Core/Nodes/Functionals/BinaryOpNode.hpp>

#include <Core/Containers/VectorArray.hpp>
#include <Core/Types.hpp>
#include <Core/Utils/Color.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Functionals {

using namespace Ra::Core;

// This macro does not end with semicolon. To be added when calling it
#define DECLARE_FUNCTIONALS( SUFFIX, TYPE )                         \
    using BinaryOp##SUFFIX        = BinaryOpNode<TYPE>;             \
    using BinaryPredicate##SUFFIX = BinaryOpNode<TYPE, TYPE, bool>; \
    using Transform##SUFFIX       = FunctionNode<TYPE>;

/* Not yet supported
    using BinaryPredicate##SUFFIX##Array =  BinaryOpNode<Ra::Core::VectorArray<TYPE>,
   Ra::Core::VectorArray<TYPE>, bool>
*/

DECLARE_FUNCTIONALS( Scalar, Scalar );
DECLARE_FUNCTIONALS( Int, int );
DECLARE_FUNCTIONALS( UInt, unsigned int );
DECLARE_FUNCTIONALS( Color, Utils::Color );
DECLARE_FUNCTIONALS( Vector2, Vector2 );
DECLARE_FUNCTIONALS( Vector3, Vector3 );
DECLARE_FUNCTIONALS( Vector4, Vector4 );

// TODO, instanciate nodes for otherypes ?

#undef DECLARE_FUNCTIONALS
} // namespace Functionals
} // namespace Core
} // namespace Dataflow
} // namespace Ra
