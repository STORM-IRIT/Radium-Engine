#pragma once
#include <Dataflow/Core/Nodes/Filters/FilterNode.hpp>

#include <Core/Containers/VectorArray.hpp>
#include <Core/Utils/Color.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Filters {

using FloatArrayFilter  = FilterNode<Ra::Core::VectorArray<float>>;
using DoubleArrayFilter = FilterNode<Ra::Core::VectorArray<double>>;
using IntArrayFilter    = FilterNode<Ra::Core::VectorArray<int>>;
using UIntArrayFilter   = FilterNode<Ra::Core::VectorArray<unsigned int>>;
using ColorArrayFilter  = FilterNode<Ra::Core::VectorArray<Ra::Core::Utils::Color>>;

} // namespace Filters
} // namespace Core
} // namespace Dataflow
} // namespace Ra
