#pragma once
#include <Dataflow/Core/Functionals/BinaryOpNode.hpp>
#include <Dataflow/Core/Functionals/FilterNode.hpp>
#include <Dataflow/Core/Functionals/FunctionNode.hpp>
#include <Dataflow/Core/Functionals/ReduceNode.hpp>
#include <Dataflow/Core/Functionals/TransformNode.hpp>

#include <Core/Containers/VectorArray.hpp>
#include <Core/Types.hpp>
#include <Core/Utils/Color.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Functionals {

using namespace Ra::Core;

// This macro does not end with semicolon. To be added when calling it
#define DECLARE_FUNCTIONALS( SUFFIX, TYPE )                                      \
    using ArrayFilter##SUFFIX      = FilterNode<Ra::Core::VectorArray<TYPE>>;    \
    using ArrayTransformer##SUFFIX = TransformNode<Ra::Core::VectorArray<TYPE>>; \
    using ArrayReducer##SUFFIX     = ReduceNode<Ra::Core::VectorArray<TYPE>>;    \
    using BinaryOp##SUFFIX         = BinaryOpNode<TYPE>;                         \
    using BinaryOp##SUFFIX##Array  = BinaryOpNode<Ra::Core::VectorArray<TYPE>>;  \
    using BinaryPredicate##SUFFIX  = BinaryOpNode<TYPE, TYPE, bool>;             \
    using Transform##SUFFIX        = FunctionNode<TYPE>;

/* Not yet supported
    using BinaryPredicate##SUFFIX##Array =  BinaryOpNode<Ra::Core::VectorArray<TYPE>,
   Ra::Core::VectorArray<TYPE>, bool>
*/

#define DATAFLOW_MACRO( PREFIX, TYPE, FACTORY, NAMESPACE ) DECLARE_FUNCTIONALS( PREFIX, TYPE )
NODE_TYPES( _, _ );
#undef DATAFLOW_MACRO

// TODO, instanciate nodes for otherypes ?

#undef DECLARE_FUNCTIONALS
} // namespace Functionals
} // namespace Core
} // namespace Dataflow
} // namespace Ra
