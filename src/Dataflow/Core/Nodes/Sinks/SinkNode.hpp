#pragma once
#include <Dataflow/Core/Node.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Sinks {

/**
 * \brief Base class for nodes that will store the result of a computation graph.
 * @tparam T The type of the data to serve.
 */
template <typename T>
class SinkNode : public Node
{
  protected:
    SinkNode( const std::string& instanceName, const std::string& typeName );

  public:
    explicit SinkNode( const std::string& name ) : SinkNode( name, SinkNode<T>::getTypename() ) {}

    /**
     * \brief initialize the interface port data pointer
     */
    void init() override;
    bool execute() override;

    /**
     * Get the delivered data
     * @return a copy of the delivered data.
     */
    T getData() const;
    /**
     * Get the delivered data
     * @return a const ref to the delivered data.
     */
    const T& getDataByRef() const;

  protected:
    void toJsonInternal( nlohmann::json& data ) const override;
    bool fromJsonInternal( const nlohmann::json& data ) override;

  private:
    /// \todo : allow user to specify where to store the data ? (i.e. make this a shared_ptr ?).
    // If yes, add a method setDataStorage(std::shared_ptr<T> v)
    T m_data;

    /// @{
    /// \brief Alias for the ports (allow simpler access)
    PortIn<T>* m_portIn { new PortIn<T>( "from", this ) };
    /// @}
  public:
    static const std::string& getTypename();
};

} // namespace Sinks
} // namespace Core
} // namespace Dataflow
} // namespace Ra

#include <Dataflow/Core/Nodes/Sinks/SinkNode.inl>
