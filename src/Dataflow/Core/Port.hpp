#pragma once
#include <Dataflow/RaDataflow.hpp>

#include <string>
#include <typeinfo>

#include <Core/Utils/Observable.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {

class Node;

template <typename T>
class PortOut;
template <typename T>
class PortIn;

/**
 * \brief Base class for nodes' ports
 * A port is a strongly typed extremity of connections between nodes.
 *
 */
class RA_DATAFLOW_API PortBase
{
  private:
    /// The name of the port.
    std::string m_name { "" };
    /// The port's data's type's hash.
    size_t m_type { 0 };
    /// A pointer to the node this port belongs to.
    Node* m_node { nullptr };

  protected:
    /// Flag that tells if the port is linked.
    bool m_isLinked { false };
    /// Flag that tells if the port must have a connection
    bool m_isLinkMandatory { false };

  public:
    /// \name Constructors
    /// @{
    /// \brief delete default constructors.
    /// \see https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rc-copy-virtual
    /// Constructors.
    PortBase()                  = delete;
    PortBase( const PortBase& ) = delete;
    PortBase& operator=( const PortBase& ) = delete;

    /// @param name The name of the port.
    /// @param type The data's type's hash.
    /// @param node The pointer to the node associated with the port.
    PortBase( const std::string& name, size_t type, Node* node );
    /// @}

    /// \brief make PortBase a base abstract class
    virtual ~PortBase() = default;

    /// Gets the port's name.
    const std::string& getName();
    /// Gets the hash of the type of the data.
    size_t getType();
    /// Gets a pointer to the node this port belongs to.
    Node* getNode();
    virtual bool hasData();
    // TODO : getData() to avoid dynamic_cast to get the data of the PortOut.
    /// Returns true if the port is linked
    bool isLinked();
    /// Returns true if the port is flagged as being mandatory linked
    bool isLinkMandatory();
    /// Flags the port as being mandatory linked
    void mustBeLinked();
    virtual PortBase* getLink() = 0;
    virtual bool accept( PortBase* other );
    virtual bool connect( PortBase* other ) = 0;
    virtual bool disconnect()               = 0;
    /// Returns a reflected (In <-> Out) port of the same type
    virtual PortBase* reflect( Node* node, std::string name ) = 0;
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

    virtual std::string getTypeName() = 0;
};

/**
 * \brief Output port delivering data of Type T.
 * Output port stores a non-owning pointer to the data that will be made available on a connection.
 * \tparam T The type of the delivered data.
 */
template <typename T>
class PortOut : public PortBase
{
  private:
    /// The data the port points to.
    T* m_data { nullptr };

  public:
    using DataType = T;
    /// \name Constructors
    /// @{
    /// \brief delete default constructors.
    PortOut()                 = delete;
    PortOut( const PortOut& ) = delete;
    PortOut& operator=( const PortOut& ) = delete;
    /// Constructor.
    /// @param name The name of the port.
    /// @param node The pointer to the node associated with the port.
    PortOut( const std::string& name, Node* node );
    /// @}

    /// Gets a reference to the data this ports points to.
    T& getData();
    /// Takes a pointer to the data this port will point to.
    /// @param data The pointer to the data.
    void setData( T* data );
    /// Returns true if the pointer to the data is not null.
    bool hasData() override;
    /// Returns nullptr because this port is an out port.
    PortBase* getLink() override;
    /// Returns false because out ports can not accept connection.
    /// @param o The other port to test the connection.
    bool accept( PortBase* ) override;
    /// Calls the connect(PortBase* o) function of the o node because out ports can not connect.
    /// Also sets m_isLinked.
    /// @param o The other port to connect.
    bool connect( PortBase* o ) override;
    /// Returns false because out ports can not disconnect.
    bool disconnect() override;
    /// Returns a portIn of the same type
    PortBase* reflect( Node* node, std::string name ) override;

    std::string getTypeName() override;
};

/**
 * \brief Input port accepting data of type T.
 * An input port does not staore the data but is an accessor to the data stored on the connected
 * output port. An Input port is observable and notify its observers at each  connect/disconnect
 * event. \tparam T The accepted data type
 */
template <typename T>
class PortIn : public PortBase,
               public Ra::Core::Utils::Observable<const std::string&, const PortIn<T>&, bool>
{
  private:
    /// A pointer to the out port this port is connected to.
    PortOut<T>* m_from = nullptr;

  public:
    using DataType = T;
    /// \name Constructors
    /// @{
    /// \brief delete default constructors.
    PortIn()                = delete;
    PortIn( const PortIn& ) = delete;
    PortIn& operator=( const PortIn& ) = delete;
    /// Constructor.
    /// @param name The name of the port.
    /// @param node The pointer to the node associated with the port.
    PortIn( const std::string& name, Node* node );
    /// @}

    /// Gets the out port this port is connected to.
    PortBase* getLink() override;
    /// Gets a reference to the data pointed by the connected out port.
    T& getData();
    /// Checks if there is not out port already connected and if the data types are the same.
    /// @param o The other port to test the connection
    bool accept( PortBase* other ) override;
    /// Connects this in port and the other out port if there is no out port already connected and
    /// if the data types are the same. Also sets m_isLinked.
    /// @param o The other port to connect.
    bool connect( PortBase* other ) override;
    /// Disconnects this port if it is connected.
    /// Also sets m_isLinked to false.
    bool disconnect() override;
    /// Returns a portOut of the same type
    PortBase* reflect( Node* node, std::string name ) override;
    /// Returns true if the port is an input port
    bool is_input() override;

    std::string getTypeName() override;
};

} // namespace Core
} // namespace Dataflow
} // namespace Ra

#include <Dataflow/Core/Port.inl>
