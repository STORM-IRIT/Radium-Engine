#pragma once
#include <Dataflow/RaDataflow.hpp>

#include <Core/Utils/TypesUtils.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {

/// \brief Return the human readable version of the type name T with simplified radium types
template <typename T>
const char* simplifiedDemangledType() noexcept;

/// \brief Return the human readable version of the type name T with simplified radium types
template <typename T>
const char* simplifiedDemangledType( const T& ) noexcept;

RA_DATAFLOW_API std::string simplifiedDemangledType( const std::type_index& typeName ) noexcept;

// -----------------------------------------------------------------
// ---------------------- inline methods ---------------------------

namespace TypeInternal {
RA_DATAFLOW_API std::string makeTypeReadable( const std::string& );
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

inline std::string simplifiedDemangledType( const std::type_index& typeName ) noexcept {
    return TypeInternal::makeTypeReadable( Ra::Core::Utils::demangleType( typeName ) );
}

} // namespace Core
} // namespace Dataflow
} // namespace Ra
