#pragma once
#include <Dataflow/Core/Node.hpp>

#include <functional>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Filters {

/**
 * Filter a collection according to a unary predicate
 * Copy each element which satisfies the filter  unary predicate of the input std::vector<T>
 * to the output std::vector<T>.
 * @tparam T The value type of the collection
 */
template <typename T>
class FilterNode : public Node
{
  public:
    /**
     * unaryPredicate Type
     */
    using UnaryPredicate = std::function<bool( const T& )>;

    /**
     * \brief Construct a filter accepting all its input ( true() lambda )
     * \param instanceName
     */
    explicit FilterNode( const std::string& instanceName );

    /**
     * \brief Construct a filter with the given predicate
     * \param instanceName
     * \param filterFunction
     */
    FilterNode( const std::string& instanceName, UnaryPredicate filterFunction );

    void init() override;
    void execute() override;

    /// Sets the filtering predicate on the node
    void setFilterFunction( UnaryPredicate filterFunction );

  protected:
    FilterNode( const std::string& instanceName,
                const std::string& typeName,
                std::function<bool( T )> filterFunction );

    void toJsonInternal( nlohmann::json& data ) const override;
    void fromJsonInternal( const nlohmann::json& data ) override;

  private:
    std::function<bool( T )> m_filterFunction;
    std::vector<T> m_elements;

  public:
    static const std::string getTypename();
};

} // namespace Filters
} // namespace Core
} // namespace Dataflow
} // namespace Ra

#include <Dataflow/Core/Nodes/Filters/FilterNode.inl>
