#pragma once

namespace Ra {
namespace Dataflow {
namespace Core {

/// \brief Return the human readable version of the type name T with simplified radium types
template <typename T>
const char* simplifiedDemangledType() noexcept;

} // namespace Core
} // namespace Dataflow
} // namespace Ra

#include <Dataflow/Core/TypeDemangler.inl>
