#pragma once
#include <Dataflow/Core/EditableParameter.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {

inline EditableParameterBase::EditableParameterBase( std::string& name, size_t hashedType ) :
    m_name( name ), m_hashedType( hashedType ) {}

template <typename T>
EditableParameter<T>::EditableParameter( std::string name, T& data ) :
    EditableParameterBase( name, typeid( T ).hash_code() ), m_data( data ) {}

template <typename T>
void EditableParameter<T>::addAdditionalData( T newData ) {
    additionalData.push_back( newData );
}

inline std::string EditableParameterBase::getName() {
    return m_name;
}

} // namespace Core
} // namespace Dataflow
} // namespace Ra
