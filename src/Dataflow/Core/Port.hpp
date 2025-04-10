#pragma once

#include <Dataflow/RaDataflow.hpp>

#include <Core/Utils/TypesUtils.hpp>

#include <string>
#include <typeinfo>

namespace Ra {
namespace Dataflow {
namespace Core {

class Node;

// forward, defined in port facotory
template <typename T>
void add_port_type();
/**
 * \brief Base class for nodes' ports
 * A port is a strongly typed extremity of connections between nodes.
 * \warning when comparing and using typed port, beware of the const qualifier
 * that is not always exposed by the C++ type system. There are some undefined behavior
 * concerning const_casts and const qualifier in the C++ documentation
 * (https://en.cppreference.com/w/cpp/language/const_cast).
 *
 */
class RA_DATAFLOW_CORE_API PortBase
{
  public:
    /// \name Constructors
    /// @{
    /// \brief delete default copy constructors.
    /// \see https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rc-copy-virtual
    PortBase()                             = delete;
    PortBase( const PortBase& )            = delete;
    PortBase& operator=( const PortBase& ) = delete;
    /**
     * \param name The name of the port.
     * \param type The data's type's hash.
     * \param node The pointer to the node associated with the port.
     */
    PortBase( const std::string& name, std::type_index type, Node* node ) :
        m_name( name ), m_type( type ), m_node( node ) {}
    /// @}

    /// \brief Make PortBase a base abstract class
    virtual ~PortBase() = default;

    /// \brief Gets the port's name.
    const std::string& name() const { return m_name; }
    /// \brief Set's port name
    void set_name( const std::string& name ) { m_name = name; }
    /// \brief Gets the type of the data (efficient for comparisons).
    std::type_index type() const { return m_type; }
    /// \brief Gets a pointer to the node this port belongs to.
    Node* node() const { return m_node; }

    /// \brief Gets the human readable type of the port object.
    /// \return The simplified demangled type.
    std::string port_typename() const { return Ra::Core::Utils::simplifiedDemangledType( m_type ); }

    /// can we get data from the port ?
    virtual bool has_data() { return false; }

    virtual void to_json( nlohmann::json& data ) { data["name"] = name(); }
    virtual void from_json( const nlohmann::json& data ) {
        if ( auto it = data.find( "name" ); it != data.end() ) { set_name( *it ); }
    }

  private:
    std::string m_name { "" }; ///< The name of the port.
    std::type_index m_type;    ///< The port's data's type's index.
    Node* m_node { nullptr };  ///< A pointer to the node this port belongs to.
    /// \todo switch to shared_ptr ?
};

template <typename Port>
using PortPtr = std::shared_ptr<Port>;

template <typename Port>
using PortRawPtr = typename PortPtr<Port>::element_type*;

using PortBasePtr    = PortPtr<PortBase>;
using PortBaseRawPtr = PortRawPtr<PortBase>;

} // namespace Core
} // namespace Dataflow
} // namespace Ra
