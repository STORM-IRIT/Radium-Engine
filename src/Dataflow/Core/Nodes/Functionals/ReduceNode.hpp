#pragma once
#include <Dataflow/Core/Node.hpp>

#include <functional>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Functionals {

/** \brief Reduce an iterable collection using a given operator
 * \tparam coll_t the collection to reduce. Must respect the SequenceContainer requirements
 * \tparam v_t (optional), type of the element in the collection. Default to coll_t::value_type
 * \see https://en.cppreference.com/w/cpp/named_req/SequenceContainer
 *
 * This node has three inputs :
 *   - in : port accepting a coll_t data. Linking to this port is mandatory
 *   - f : port accepting an operator with profile std::function<v_t( const v_t& )>.
 *   Linking to this port is not mandatory, the operator might be set once for the node or is the
 * default value for v_t. If this port is linked, the operator will be taken from the port.
 *   - init : port accepting a v_t to serve as initial value.
 *   Linking to this port is not mandatory.
 *   If this port is linked, the initial value will be taken from the port.
 *
 * This node has one output :
 *   - out : port giving a v_t such that out = std::reduce(in, f, init) (std::accumulate if less
 * than C++17)
 */
// TODO, allow to specify the type of the reduced information. This will allow, e.g, given a
//  collection of X, to compute a tuple containing mean and standard deviation of the collection
//  as reduction (using online Welford algo).
template <typename coll_t, typename v_t = typename coll_t::value_type>
class ReduceNode : public Node
{
  public:
    /**
     * Trasformation operator profile
     */
    using ReduceOperator = std::function<v_t( const v_t&, const v_t& )>;

    /**
     * \brief Construct an identity transformer
     * \param instanceName
     */
    explicit ReduceNode( const std::string& instanceName );

    /**
     * \brief Construct a transformer with the given operator
     * \param instanceName
     * \param op
     * \param initialValue
     */
    ReduceNode( const std::string& instanceName, ReduceOperator op, v_t initialValue = v_t {} );

    void init() override;
    void execute() override;

    /// Sets the operator on the node
    void setOperator( ReduceOperator op, v_t initialValue = v_t {} );

  protected:
    ReduceNode( const std::string& instanceName,
                const std::string& typeName,
                ReduceOperator op,
                v_t initialValue );

    void toJsonInternal( nlohmann::json& data ) const override;
    bool fromJsonInternal( const nlohmann::json& ) override;

  private:
    ReduceOperator m_operator;
    v_t m_result;
    v_t m_init;

    /// @{
    /// \brief Alias for the ports (allow simpler access)
    PortIn<coll_t>* m_portIn { new PortIn<coll_t>( "in", this ) };
    PortIn<ReduceOperator>* m_portF { new PortIn<ReduceOperator>( "f", this ) };
    PortIn<v_t>* m_portInit { new PortIn<v_t>( "init", this ) };
    PortOut<v_t>* m_portOut { new PortOut<v_t>( "out", this ) };
    /// @}

  public:
    static const std::string& getTypename();
};

} // namespace Functionals
} // namespace Core
} // namespace Dataflow
} // namespace Ra

#include <Dataflow/Core/Nodes/Functionals/ReduceNode.inl>
