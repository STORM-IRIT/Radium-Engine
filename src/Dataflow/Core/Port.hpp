#pragma once

#include <Dataflow/RaDataflow.hpp>

#include <string>
#include <typeinfo>

#include <Dataflow/Core/TypeDemangler.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {

class Node;

/**
 * \brief Base class for nodes' ports
 * A port is a strongly typed extremity of connections between nodes.
 * \warning when comparing and using typed port, beware of the const qualifier
 * that is not always exposed by the C++ type system. There are some undefined behavior concerning
 * const_casts and const qualifier in the C++ documentation
 * (https://en.cppreference.com/w/cpp/language/const_cast).
 *
 */
class RA_DATAFLOW_API PortBase
{
  public:
    /// \name Constructors
    /// @{
    /// \brief delete default constructors.
    /// \see https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rc-copy-virtual
    /// Constructors.
    PortBase()                             = delete;
    PortBase( const PortBase& )            = delete;
    PortBase& operator=( const PortBase& ) = delete;

    /// @param name The name of the port.
    /// @param type The data's type's hash.
    /// @param node The pointer to the node associated with the port.
    PortBase( const std::string& name, std::type_index type, Node* node ) :
        m_name( name ), m_type( type ), m_node( node ) {}
    /// @}

    /// \brief Make PortBase a base abstract class
    virtual ~PortBase() = default;

    /// Gets the port's name.
    const std::string& getName() const { return m_name; }
    void setName( const std::string& name ) { m_name = name; }
    /// Gets the type of the data (efficient for comparisons).
    std::type_index getType() const { return m_type; }
    /// Gets a pointer to the node this port belongs to.
    Node* getNode() const { return m_node; }

    ///\brief Gets the human readable type of the port object.
    ///
    ///\return std::string The simplified demangled type.
    std::string getTypeName() const { return simplifiedDemangledType( m_type ); }

    /// \todo Remove this since we only manipulate PortBaseIn or PortBaseOut
    virtual bool hasData() { return false; }

    /// \todo Remove this since we only manipulate PortBaseIn or PortBaseOut
    /// Returns true if the port is an input port
    virtual bool is_input() { return false; }

    /// \todo Remove this since we only manipulate PortBaseIn or PortBaseOut
    /// Allows to get data stored at this port if it is an output port.
    /// This method copy the data onto the given object
    /// @params t The reference to store the data of this port
    template <typename T>
    void getData( T& t );

    /// \todo Remove this since we only manipulate PortBaseIn or PortBaseOut
    /// Allows to get data stored at this port if it is an output port.
    /// This method do not copy the data but gives a reference to the transmitted object.
    /// TODO Verify the robustness of this
    /// @params t The reference to store the data of this port
    template <typename T>
    T& getData();

    /// \todo Remove this since we only manipulate PortBaseIn or PortBaseOut
    /// Check if this port is an output port, then takes a pointer to the data this port will point
    /// to.
    /// @param data The pointer to the data.
    template <typename T>
    void setData( T* data );

  private:
    std::string m_name { "" }; ///< The name of the port.
    std::type_index m_type;    ///< The port's data's type's index.
    Node* m_node { nullptr };  ///< A pointer to the node this port belongs to.
    /// \todo switch to shared_ptr ?
};

} // namespace Core
} // namespace Dataflow
} // namespace Ra
