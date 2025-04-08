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
    using PREFIX##ArraySink = SinkNode<Ra::Core::VectorArray<TYPE>>

// bool could not be declared as others, because of the specificity of std::vector<bool> that is not
// compatible with Ra::Core::VectorArray implementation see
// https://en.cppreference.com/w/cpp/container/vector_bool Right now, there is no
// Ra::Core::VectorArray of bool
using BooleanSink = SinkNode<bool>;
DECLARE_SINKS( Float, float );
DECLARE_SINKS( Double, double );
DECLARE_SINKS( Int, int );
DECLARE_SINKS( UInt, unsigned int );
DECLARE_SINKS( Color, Utils::Colorf );
DECLARE_SINKS( Vector2f, Vector2f );
DECLARE_SINKS( Vector3f, Vector3f );
DECLARE_SINKS( Vector4f, Vector4f );
DECLARE_SINKS( Vector2d, Vector2d );
DECLARE_SINKS( Vector3d, Vector3d );
DECLARE_SINKS( Vector4d, Vector4d );

#undef DECLARE_SINKS
} // namespace Sinks
} // namespace Core
} // namespace Dataflow
} // namespace Ra
