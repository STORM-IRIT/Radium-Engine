#pragma once
#include <Dataflow/Core/TypeDemangler.hpp>

#include <Core/Utils/TypesUtils.hpp>
namespace Ra {
namespace Dataflow {
namespace Core {

/// Return the human readable version of the type name T with simplified radium types
template <typename T>
const char* simplifiedDemangledType() noexcept {
    static std::string demangledType = Ra::Core::Utils::demangleType<T>();
    Ra::Core::Utils::replaceAllInString( demangledType, "Ra::Core::VectorArray", "RaVector" );
    Ra::Core::Utils::replaceAllInString(
        demangledType, "Ra::Core::Utils::ColorBase<float>", "RaColor" );
    Ra::Core::Utils::replaceAllInString(
        demangledType, "Ra::Core::Utils::ColorBase<double>", "RaColor" );
    return demangledType.c_str();
}

} // namespace Core
} // namespace Dataflow
} // namespace Ra
