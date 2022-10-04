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
 * \tparam v_t (optional, type of the element in the collection)
 * \see https://en.cppreference.com/w/cpp/named_req/SequenceContainer
 */
template <typename coll_t, typename v_t = typename coll_t::value_type>
class MapNode : public Node
{
  public:
    /**
     * unaryPredicate Type
     */
    using MapOperator = std::function<v_t( const v_t& )>;

    /**
     * \brief Construct a filter accepting all its input ( true() lambda )
     * \param instanceName
     */
    explicit MapNode( const std::string& instanceName );

    /**
     * \brief Construct a filter with the given predicate
     * \param instanceName
     * \param filterFunction
     */
    MapNode( const std::string& instanceName, MapOperator mapOperator );

    void init() override;
    void execute() override;

    /// Sets the filtering predicate on the node
    void setFilterFunction( MapOperator mapOperator );

  protected:
    MapNode( const std::string& instanceName,
             const std::string& typeName,
             MapOperator mapOperator );

    void toJsonInternal( nlohmann::json& data ) const override;
    void fromJsonInternal( const nlohmann::json& data ) override;

  private:
    MapOperator m_mapOperator;
    coll_t m_elements;

  public:
    static const std::string& getTypename();
};

} // namespace Functionals
} // namespace Core
} // namespace Dataflow
} // namespace Ra

#include <Dataflow/Core/Nodes/Functionals/MapNode.inl>
