#pragma once
#include <Dataflow/Core/TypeDemangler.hpp>

#include <Core/Utils/TypesUtils.hpp>
namespace Ra {
namespace Dataflow {
namespace Core {

namespace TypeInternal {
RA_DATAFLOW_API std::string makeTypeReadable( std::string );
}

template <typename T>
const char* simplifiedDemangledType() noexcept {
    static auto demangled_name = []() {
        std::string demangledType =
            TypeInternal::makeTypeReadable( Ra::Core::Utils::demangleType<T>() );
        return demangledType;
    }();
    return demangled_name.data();
}

template <typename T>
const char* simplifiedDemangledType( const T& ) noexcept {
    return simplifiedDemangledType<T>();
}

} // namespace Core
} // namespace Dataflow
} // namespace Ra
