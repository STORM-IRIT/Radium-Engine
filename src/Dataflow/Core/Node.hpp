#pragma once
#include <Dataflow/RaDataflow.hpp>

#include <Dataflow/Core/EditableParameter.hpp>
#include <Dataflow/Core/Port.hpp>

#include <nlohmann/json.hpp>

#include <uuid.h>

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <typeinfo>
#include <vector>

namespace Ra {
namespace Dataflow {
namespace Core {

class RA_DATAFLOW_API Node
{
  public:
    /// Constructor.
    Node()              = delete;
    Node( const Node& ) = delete;
    Node& operator=( const Node& ) = delete;
    virtual ~Node()                = default;

    /// \brief Initializes the node content
    /// The init() function is called once at the start of the application.
    /// Its goal is to initialize the node's internal data if any.
    /// The base version do nothing
    virtual void init();

    /// \brief Executes the node
    /// The execute() function on an active node is called each time the graph is executed.
    /// Execute the node processing on the input ports and write the results to the output ports.
    virtual void execute() = 0;

    /// \brief delete the node content
    /// The destroy() function is called once at the end of the application.
    /// Its goal is to free the internal data that have been allocated.
    virtual void destroy();

    /// TODO : specify the json format for nodes and what is expected from the following ethods

    /// \brief serialize the content of the node.
    /// Fill the given json object with the json representation of the concrete node.
    virtual void toJson( nlohmann::json& data ) const;

    /// \brief unserialized the content of the node.
    /// Fill the node from its json representation
    virtual void fromJson( const nlohmann::json& data );

    /// \brief Compile the node to check its validity
    virtual bool compile();

    /// \bried Add a metadata to the node to store application specific informations.
    /// used, e.g. by the node editor gui to save node position in the graphical canvas.
    void addJsonMetaData( const nlohmann::json& data );

    /// \bried Give access to extra json data stored on the node.
    nlohmann::json& getJsonMetaData();

    /// \brief Gets the type name of the node.
    const std::string& getTypeName() const;

    /// \brief Gets the instance name of the node.
    const std::string& getInstanceName() const;

    /// \brief Sets the instance name (rename) the node
    void setInstanceName( const std::string& newName );

    /// \brief Generates the uuid of the node
    void generateUuid();

    /// \brief Gets the UUID of the node as a string
    std::string getUuid() const;

    /// \brief Sets the UUID of the node from a valid string string
    /// \return true if the uuid is set, false if the node already have a valid uid
    bool setUuid( const std::string& uid );

    /// \brief Tests if the node is deletable (deprecated)
    /// \return the deletable status of the node
    [[deprecated]] bool isDeletable();

    /// \brief Set the deletable status of the node
    [[deprecated]] void setDeletableStatus( bool deletable = true );

    /// \brief Gets the in ports of the node.
    const std::vector<std::unique_ptr<PortBase>>& getInputs();

    /// \brief Gets the out ports of the node.
    const std::vector<std::unique_ptr<PortBase>>& getOutputs();

    /// \brief Get the interface ports of the node
    const std::vector<PortBase*>& getInterface();

    /// Gets the editable parameters of the node.
    const std::vector<std::unique_ptr<EditableParameterBase>>& getEditableParameters();

    /// \brief Sets the filesystem (real or virtual) location for the pass resources
    inline void setResourcesDir( std::string resourcesRootDir );

    /// \brief Flag that checks if the node is already initialized
    bool m_initialized { false };

    /// \brief Two nodes are considered equal if there names are the same.
    bool operator==( const Node& o_node );

    /// Adds an interface port to the node.
    /// This function checks if there is no interface port with the same name already associated
    /// with this node.
    /// \param port The interface port to add.
    bool addInterface( PortBase* port );

  protected:
    /// \param instanceName The name of the node
    /// \param typeName The type name of the node

    Node( const std::string& instanceName, const std::string& typeName );

    /// internal json representation of the Node.
    /// Must be implemented by inheriting classes.
    /// Be careful with template specialization and function member overriding when implementing
    /// this method.
    virtual void fromJsonInternal( const nlohmann::json& ) = 0;

    /// internal json representation of the Node.
    /// Must be implemented by inheriting classes.
    /// Be careful with template specialization and function member overriding when implementing
    /// this method.
    virtual void toJsonInternal( nlohmann::json& ) const = 0;

    /// Adds an in port to the node.
    /// This function checks if the port is an input port, then if there is no in port with the same
    /// name already associated with this node.
    /// \param in The in port to add.
    bool addInput( PortBase* in );

    /// Adds an out port to the node and the data associated with it.
    /// This function checks if there is no out port with the same name already associated with this
    /// node.
    /// \param out The in port to add.
    /// \param data The data associated with the port.
    template <typename T>
    void addOutput( PortOut<T>* out, T* data );

    /// Adds an out port for a GRAPH. This port is also an interface port whose reference is stored
    /// in the source and sink nodes of the graph. This function checks if there is no out port with
    /// the same name already associated with the graph.
    /// \param out The port to add.
    bool addOutput( PortBase* out );

    /// \brief Adds an editable parameter to the node if it does not already exist.
    /// \note the node will take ownership of the editable object.
    /// \param editableParameter The editable parameter to add.
    template <typename T>
    bool addEditableParameter( EditableParameter<T>* editableParameter );

    /// Remove an editable parameter to the node if it does exist.
    /// \param name The name of the editable parameter to remove.
    /// \return true if the editable parameter is found and removed.
    template <typename T>
    bool removeEditableParameter( const std::string& name );

    /// The uuid of the node (TODO, use https://github.com/mariusbancila/stduuid instead of a
    /// string)
    // std::string m_uuid;
    uuids::uuid m_uuid;
    /// The deletable status of the node
    bool m_isDeletable { true };
    /// The type name of the node. Initialized once at construction
    std::string m_typeName;
    /// The instance name of the node
    std::string m_instanceName;
    /// The in ports of the node
    std::vector<std::unique_ptr<PortBase>> m_inputs;
    /// The out ports of the node
    std::vector<std::unique_ptr<PortBase>> m_outputs;
    /// The reflected ports of the node if it is only a source or sink node
    std::vector<PortBase*> m_interface;
    /// The editable parameters of the node
    std::vector<std::unique_ptr<EditableParameterBase>> m_editableParameters;

    /// The base resources directory
    std::string m_resourceDir { "./" };

    /// Additional data on the node, added by application or gui or ...
    nlohmann::json m_extraJsonData;

    /// generator for uuid
    static bool s_uuidGeneratorInitialized;
    static uuids::uuid_random_generator* s_uidGenerator;
    static void createUuidGenerator();

  public:
    /// \brief Returns the demangled type name of the node or any human readable representation of
    /// the type name.
    /// This is a public static member each node must define to be serializable
    static const std::string& getTypename();
};

} // namespace Core
} // namespace Dataflow
} // namespace Ra

#include <Dataflow/Core/Node.inl>
