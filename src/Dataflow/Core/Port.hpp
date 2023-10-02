#pragma once

#include "Core/Utils/Log.hpp"
#include <Dataflow/RaDataflow.hpp>

#include <Dataflow/Core/TypeDemangler.hpp>

#include <optional>
#include <string>
#include <typeinfo>

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
  private:
    /// The name of the port.
    std::string m_name { "" };
    /// The port's data's type's index.
    std::type_index m_type;
    /// A pointer to the node this port belongs to.
    Node* m_node { nullptr }; /// \todo switch to shared_ptr ?

  protected:
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
    PortBase( const std::string& name, std::type_index type, Node* node );
    /// @}

    /// \brief make PortBase a base abstract class
    virtual ~PortBase() = default;

    /// Gets the port's name.
    const std::string& getName() const;
    void setName( const std::string& name ) { m_name = name; }
    /// Gets the type of the data (efficient for comparisons).
    std::type_index getType() const;
    /// Gets a pointer to the node this port belongs to.
    Node* getNode() const;
    virtual bool hasData();

    /// Returns true if the port is linked
    // virtual bool isLinked() const { return false; }
    /// Returns true if the port is flagged as being mandatory linked
    // virtual bool isLinkMandatory() const { return false; }

    // virtual PortBase* getLink() = 0;
    // virtual bool accept( PortBase* other );
    // virtual bool connect( PortBase* other ) = 0;
    // virtual bool disconnect() = 0;
    /// Returns a reflected (In <-> Out) port of the same type
    // virtual PortBase* reflect( Node* node, std::string name ) = 0;
    /// Returns true if the port is an input port
    virtual bool is_input() { return false; }

    /// Allows to get data stored at this port if it is an output port.
    /// This method copy the data onto the given object
    /// @params t The reference to store the data of this port
    template <typename T>
    void getData( T& t );

    /// Allows to get data stored at this port if it is an output port.
    /// This method do not copy the data but gives a reference to the transmitted object.
    /// TODO Verify the robustness of this
    /// @params t The reference to store the data of this port
    template <typename T>
    T& getData();

    /// Check if this port is an output port, then takes a pointer to the data this port will point
    /// to.
    /// @param data The pointer to the data.
    template <typename T>
    void setData( T* data );

    std::string getTypeName() const;
};

// -----------------------------------------------------------------
// ---------------------- inline methods ---------------------------

inline PortBase::PortBase( const std::string& name, std::type_index type, Node* node ) :
    m_name( name ), m_type( type ), m_node( node ) {}

inline const std::string& PortBase::getName() const {
    return m_name;
}

inline std::type_index PortBase::getType() const {
    return m_type;
}

inline std::string PortBase::getTypeName() const {
    return simplifiedDemangledType( m_type );
}

inline Node* PortBase::getNode() const {
    return m_node;
}

inline bool PortBase::hasData() {
    return false;
}

} // namespace Core
} // namespace Dataflow
} // namespace Ra
