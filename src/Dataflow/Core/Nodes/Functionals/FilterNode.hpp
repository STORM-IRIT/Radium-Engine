#pragma once
#include <Dataflow/Core/Node.hpp>

#include <Core/Containers/VectorArray.hpp>

#include <functional>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Functionals {

/** \brief Filter on iterable collection
 * \tparam coll_t the collection to filter. Must respect the SequenceContainer requirements
 * \tparam v_t (optional), type of the element in the collection. Default to coll_t::value_type
 * \see https://en.cppreference.com/w/cpp/named_req/SequenceContainer
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

  public:
    static const std::string& getTypename();
};

} // namespace Functionals
} // namespace Core
} // namespace Dataflow
} // namespace Ra

#include <Dataflow/Core/Nodes/Functionals/FilterNode.inl>
