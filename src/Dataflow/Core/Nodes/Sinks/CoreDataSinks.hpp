#pragma once
#include <Dataflow/Core/Nodes/Sinks/SinkNode.hpp>

#include <Core/Containers/VectorArray.hpp>
#include <Core/Utils/Color.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Sinks {

using BooleanSink = SinkNode<bool>;
using IntSink     = SinkNode<int>;
using UIntSink    = SinkNode<unsigned int>;
using ScalarSink  = SinkNode<Scalar>;
using ColorSink   = SinkNode<Ra::Core::Utils::Color>;

using FloatArraySink  = SinkNode<Ra::Core::VectorArray<float>>;
using DoubleArraySink = SinkNode<Ra::Core::VectorArray<double>>;
using IntArraySink    = SinkNode<Ra::Core::VectorArray<int>>;
using UIntArraySink   = SinkNode<Ra::Core::VectorArray<unsigned int>>;
using ColorArraySink  = SinkNode<Ra::Core::VectorArray<Ra::Core::Utils::Color>>;

} // namespace Sinks
} // namespace Core
} // namespace Dataflow
} // namespace Ra

#include <Dataflow/Core/Nodes/Sinks/CoreDataSinks.inl>
