#pragma once
#include <Dataflow/Core/Node.hpp>

#include <iostream>
#include <utility>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Sources {
/**
 * \brief Base class for nodes that will give access to some input data to the graph.
 * This class can be used to feed nodes on a dataflow graph with some data coming
 * from outside the graph or from the source node itself.
 *
 * The data delivered by the node can be explicitly set/get or can be made editable.
 *
 * @tparam T The type of the data to serve.
 */
template <typename T>
class SingleDataSourceNode : public Node
{
  protected:
    SingleDataSourceNode( const std::string& instanceName, const std::string& typeName );

  public:
    explicit SingleDataSourceNode( const std::string& name ) :
        SingleDataSourceNode( name, SingleDataSourceNode<T>::getTypename() ) {}

    void execute() override;

    /** \brief Set the data to be delivered by the node.
     * @param data
     * \warning This will copy the given data into the node.
     * To prevent copy prefer using the corresponding dataSetter on the owning graph.
     */
    void setData( T* data );

    /**
     * \brief Get the delivered data
     * @return The non owning pointer (alias) to the delivered data.
     */
    T* getData() const;

    /**
     * \brief Set the delivered data editable using the given name
     * Give access to the internal data storage.
     * If the node interface is connected, the edition will not result on a propagation to the
     * graph as internal data storage will be superseeded by the data from the interface.
     * @param name Name of the data as it will appear on edition gui. If not given, the default
     * name "Data" will be used.
     */
    void setEditable( const std::string& name = "Data" );

    /**
     * \brief Remove the delivered data from being editable
     * @param name Name of the data given when calling setEditable
     */
    void removeEditable( const std::string& name = "Data" );

  protected:
    void fromJsonInternal( const nlohmann::json& ) override;
    void toJsonInternal( nlohmann::json& ) const override;

    /// The data provided by the node
    /// Ownership of this pointer is left to the caller
    T* m_data { nullptr };

    /// Used to deliver (and edit) data when the interface is not connected.
    T m_localData;

    /// Alias to the output port
    PortOut<T>* m_portOut { nullptr };

    /// used only at deserialization
    void setData( T& data );

  public:
    static const std::string& getTypename();
};

} // namespace Sources
} // namespace Core
} // namespace Dataflow
} // namespace Ra

#include <Dataflow/Core/Nodes/Sources/SingleDataSourceNode.inl>
