#pragma once
#pragma once
#include <Dataflow/Core/Node.hpp>

#include <iostream>
#include <utility>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Sources {

/**
 * \brief Node that deliver a std::function<R( Args... )>
 * \tparam R return type of the function
 * \tparam Type of the function arguments
 */
template <class R, class... Args>
class FunctionSourceNode : public Node
{

  public:
    using function_type = std::function<R( Args... )>;

    explicit FunctionSourceNode( const std::string& name ) :
        FunctionSourceNode( name, FunctionSourceNode<R, Args...>::getTypename() ) {}

    bool execute() override;

    /** \brief Set the function to be delivered by the node.
     * @param data
     */
    void setData( function_type* data );

    /**
     * \brief Get the delivered data
     * @return The non owning pointer (alias) to the delivered data.
     */
    function_type* getData() const;

  protected:
    FunctionSourceNode( const std::string& instanceName, const std::string& typeName );

    bool fromJsonInternal( const nlohmann::json& ) override;
    void toJsonInternal( nlohmann::json& data ) const override;

    /// @{
    /// The data provided by the node
    function_type m_localData { []( Args... ) { return R {}; } };
    function_type* m_data { &m_localData };
    /// @}

    /// Alias to the output port
    PortOut<function_type>* m_portOut { new PortOut<function_type>( "f", this ) };

  public:
    static const std::string& getTypename();
};

} // namespace Sources
} // namespace Core
} // namespace Dataflow
} // namespace Ra

#include <Dataflow/Core/Nodes/Sources/FunctionSource.inl>
