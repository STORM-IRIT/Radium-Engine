#pragma once
#include <Dataflow/RaDataflow.hpp>

#include <string>
#include <typeinfo>
#include <vector>

namespace Ra {
namespace Dataflow {
namespace Core {

/// \brief Basic introspection for Node internal data edition
/// This base class gives key information to associate editing capabilities (and gui) to
/// an node internal data.
///
struct RA_DATAFLOW_API EditableParameterBase {
    /// \name Constructors
    /// @{
    /// \brief delete default constructors.
    EditableParameterBase()                               = delete;
    EditableParameterBase( const EditableParameterBase& ) = delete;
    EditableParameterBase& operator=( const EditableParameterBase& ) = delete;

    /// Construct an base editable parameter from its name and type hash
    EditableParameterBase( std::string& name, size_t hashedType );
    ///@}

    virtual ~EditableParameterBase() = default;
    std::string getName();
    std::string m_name { "" };
    size_t m_hashedType { 0 };
};

template <typename T>
struct EditableParameter : public EditableParameterBase {
    /// \name Constructors
    /// @{
    /// \brief delete default constructors.
    EditableParameter()                           = delete;
    EditableParameter( const EditableParameter& ) = delete;
    EditableParameter& operator=( const EditableParameter& ) = delete;

    /// Construct an editable parameter from its name and type hash
    EditableParameter( std::string name, T& data );
    ///@}

    /// Add constraints or associated data to the editable.
    /// \todo, replace this with a json object describing the constraints
    /// with value convertible to T ....
    void addAdditionalData( T newData );

    /// The data to edit.
    /// This is a reference to any data stored in a node and that the user could change
    T& m_data;

    /// Constraints on the edited data
    std::vector<T> additionalData;
};

} // namespace Core
} // namespace Dataflow
} // namespace Ra

#include <Dataflow/Core/EditableParameter.inl>
