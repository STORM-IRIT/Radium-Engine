#pragma once
#include <Dataflow/Core/Nodes/Sources/FunctionSource.hpp>
#include <Dataflow/Core/Nodes/Sources/SingleDataSourceNode.hpp>

#include <Core/Containers/VectorArray.hpp>
#include <Core/Types.hpp>
#include <Core/Utils/Color.hpp>

#include <iostream>
#include <utility>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Sources {
// TODO unify editable and non editable data sources

// declare synonyms for convenient sources
// This macro does not end with semicolon. To be added when calling it
#define DECLARE_COREDATA_SOURCES( PREFIX, TYPE )                                              \
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
DECLARE_COREDATA_SOURCES( Scalar, Scalar )
DECLARE_COREDATA_SOURCES( Int, int )
DECLARE_COREDATA_SOURCES( UInt, unsigned int )
DECLARE_COREDATA_SOURCES( Color, Utils::Color )
DECLARE_COREDATA_SOURCES( Vector2, Vector2 )
DECLARE_COREDATA_SOURCES( Vector3, Vector3 )
DECLARE_COREDATA_SOURCES( Vector4, Vector4 )

#undef DECLARE_COREDATA_SOURCES

} // namespace Sources
} // namespace Core
} // namespace Dataflow
} // namespace Ra
