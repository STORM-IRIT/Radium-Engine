#pragma once
#include <Dataflow/Core/TypeDemangler.hpp>

#include <Core/Utils/TypesUtils.hpp>
namespace Ra {
namespace Dataflow {
namespace Core {

namespace TypeInternal {
std::string makeTypeReadable( std::string );

}
/// Return the human readable version of the type name T with simplified radium types
template <typename T>
const char* simplifiedDemangledType() noexcept {
    static auto demangled_name = []() {
        std::string demangledType =
            TypeInternal::makeTypeReadable( Ra::Core::Utils::demangleType<T>() );
        return demangledType;
    }();
    return demangled_name.data();
}

} // namespace Core
} // namespace Dataflow
} // namespace Ra
