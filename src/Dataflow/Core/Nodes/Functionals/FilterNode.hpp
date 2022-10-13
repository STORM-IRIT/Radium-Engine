#pragma once
#include <Dataflow/Core/Node.hpp>

#include <functional>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Functionals {

/** \brief Filter on iterable collection.
 * \tparam coll_t the collection to filter. Must respect the SequenceContainer requirements
 * \tparam v_t (optional), type of the element in the collection. Default to coll_t::value_type
 * \see https://en.cppreference.com/w/cpp/named_req/SequenceContainer
 *
 * This node apply an operator f on its input such that to keep only elements validated by a
 * predicate :
 *
 * This node has two inputs :
 *   - in : port accepting the input data of type coll_t. Must be linked.
 *   - f : port accepting an operator with profile std::function<bool( const v_t& )>.
 *   Link to this port is not mandatory, the operator might be set once for the node.
 *   If this port is linked, the operator will be taken from the port.
 *
 * This node has one output :
 *   - out : port giving a coll_t such that out = std::copy_if(a, f)
 */
template <typename coll_t, typename v_t = typename coll_t::value_type>
class FilterNode : public Node
{
  public:
    /**
     * unaryPredicate Type
     */
    using UnaryPredicate = std::function<bool( const v_t& )>;

    /**
     * \brief Construct a filter accepting all its input ( true() lambda )
     * \param instanceName
     */
    explicit FilterNode( const std::string& instanceName );

    /**
     * \brief Construct a filter with the given predicate
     * \param instanceName
     * \param predicate
     */
    FilterNode( const std::string& instanceName, UnaryPredicate predicate );

    void init() override;
    void execute() override;

    /// Sets the filtering predicate on the node
    void setFilterFunction( UnaryPredicate predicate );

  protected:
    FilterNode( const std::string& instanceName,
                const std::string& typeName,
                UnaryPredicate predicate );

    void toJsonInternal( nlohmann::json& data ) const override;
    void fromJsonInternal( const nlohmann::json& data ) override;

  private:
    UnaryPredicate m_predicate;
    coll_t m_elements;

    /// @{
    /// \brief Alias for the ports (allow simpler access)
    PortIn<coll_t>* m_portIn { new PortIn<coll_t>( "in", this ) };
    PortIn<UnaryPredicate>* m_portPredicate { new PortIn<UnaryPredicate>( "f", this ) };
    PortOut<coll_t>* m_portOut { new PortOut<coll_t>( "out", this ) };
    /// @}
  public:
    static const std::string& getTypename();
};

} // namespace Functionals
} // namespace Core
} // namespace Dataflow
} // namespace Ra

#include <Dataflow/Core/Nodes/Functionals/FilterNode.inl>
