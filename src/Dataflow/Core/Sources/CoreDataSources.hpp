#pragma once
#include <Dataflow/Core/Nodes/Sources/FunctionSource.hpp>
#include <Dataflow/Core/Nodes/Sources/SingleDataSourceNode.hpp>

#include <Core/Containers/VectorArray.hpp>
#include <Core/Types.hpp>

#include <iostream>
#include <utility>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Sources {

#define DECLARE_SOURCES( PREFIX, TYPE )                                                       \
    using PREFIX##Source                = SingleDataSourceNode<TYPE>;                         \
    using PREFIX##ArraySource           = SingleDataSourceNode<Ra::Core::VectorArray<TYPE>>;  \
    using PREFIX##UnaryFunctionSource   = FunctionSourceNode<TYPE, const TYPE&>;              \
    using PREFIX##BinaryFunctionSource  = FunctionSourceNode<TYPE, const TYPE&, const TYPE&>; \
    using PREFIX##UnaryPredicateSource  = FunctionSourceNode<bool, const TYPE&>;              \
    using PREFIX##BinaryPredicateSource = FunctionSourceNode<bool, const TYPE&, const TYPE&>;

using namespace Ra::Core;

// bool could not be declared as others, because of the specificity of std::vector<bool> that is not
// compatible with Ra::Core::VectorArray implementation see
// https://en.cppreference.com/w/cpp/container/vector_bool Right now, there is no
// Ra::Core::VectorArray of bool
using BooleanSource = SingleDataSourceNode<bool>;

#define DATAFLOW_MACRO( PREFIX, TYPE, FACTORY, NAMESPACE ) DECLARE_SOURCES( PREFIX, TYPE )
NODE_TYPES( _, _ );
#undef DATAFLOW_MACRO

#undef DECLARE_SOURCES

} // namespace Sources
} // namespace Core
} // namespace Dataflow
} // namespace Ra
