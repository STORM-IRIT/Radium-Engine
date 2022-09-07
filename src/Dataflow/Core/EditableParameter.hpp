#pragma once
#include <Dataflow/RaDataflow.hpp>

#include <string>
#include <typeinfo>
#include <vector>

namespace Ra {
namespace Dataflow {
namespace Core {

struct RA_DATAFLOW_API EditableParameterBase {
    EditableParameterBase( std::string& name, size_t hashedType );
    virtual ~EditableParameterBase() = default;
    std::string getName();
    std::string m_name { "" };
    size_t m_hashedType { 0 };
};

template <typename T>
struct EditableParameter : public EditableParameterBase {
    EditableParameter() = delete;
    explicit EditableParameter( std::string name, T& data );
    void addAdditionalData( T newData );

    T& m_data;
    std::vector<T> additionalData;
};

} // namespace Core
} // namespace Dataflow
} // namespace Ra

#include <Dataflow/Core/EditableParameter.inl>
