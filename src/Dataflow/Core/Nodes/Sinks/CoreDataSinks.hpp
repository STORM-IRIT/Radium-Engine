#pragma once
#include <Dataflow/Core/Nodes/Sinks/SinkNode.hpp>

#include <Core/Containers/VectorArray.hpp>
#include <Core/Types.hpp>
#include <Core/Utils/Color.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Sinks {

using namespace Ra::Core;

#define DECLARE_SINKS( PREFIX, TYPE )         \
    using PREFIX##Sink      = SinkNode<TYPE>; \
    using PREFIX##ArraySink = SinkNode<Ra::Core::VectorArray<TYPE>>;

// bool could not be declared as others, because of the specificity of std::vector<bool> that is not
// compatible with Ra::Core::VectorArray implementation see
// https://en.cppreference.com/w/cpp/container/vector_bool Right now, there is no
// Ra::Core::VectorArray of bool
using BooleanSink = SinkNode<bool>;

#define DATAFLOW_MACRO( PREFIX, TYPE, FACTORY, NAMESPACE ) DECLARE_SINKS( PREFIX, TYPE )
NODE_TYPES( _, _ );
#undef DATAFLOW_MACRO

#undef DECLARE_SINKS
} // namespace Sinks
} // namespace Core
} // namespace Dataflow
} // namespace Ra
