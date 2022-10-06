#pragma once
#include <Dataflow/Core/Node.hpp>

#include <Core/Containers/VectorArray.hpp>

#include <functional>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Functionals {

/** \brief Transform an iterable collection
 * \tparam coll_t the collection to transform. Must respect the SequenceContainer requirements
 * \tparam v_t (optional), type of the element in the collection. Default to coll_t::value_type
 * \see https://en.cppreference.com/w/cpp/named_req/SequenceContainer
 *
 * This node has two inputs :
 *   - in : port accepting a coll_t data. Linking to this port is mandatory
 *   - f : port accepting an operator with profile std::function<v_t( const v_t& )>.
 *   Linking to this port is not mandatory, the operator might be set once for the node.
 *   If this port is linked, the operator will be taken from the port.
 *
 * This node has one output :
 *   - out : port giving a coll_t such that out = std::transform(in, f)
 */
template <typename coll_t, typename v_t = typename coll_t::value_type>
class TransformNode : public Node
{
  public:
    /**
     * Trasformation operator profile
     */
    using TransformOperator = std::function<v_t( const v_t& )>;

    /**
     * \brief Construct an identity transformer
     * \param instanceName
     */
    explicit TransformNode( const std::string& instanceName );

    /**
     * \brief Construct a transformer with the given operator
     * \param instanceName
     * \param filterFunction
     */
    TransformNode( const std::string& instanceName, TransformOperator op );

    void init() override;
    void execute() override;

    /// Sets the operator on the node
    void setOperator( TransformOperator op );

  protected:
    TransformNode( const std::string& instanceName,
                   const std::string& typeName,
                   TransformOperator op );

    void toJsonInternal( nlohmann::json& data ) const override;
    void fromJsonInternal( const nlohmann::json& data ) override;

  private:
    TransformOperator m_operator;
    coll_t m_elements;

  public:
    static const std::string& getTypename();
};

} // namespace Functionals
} // namespace Core
} // namespace Dataflow
} // namespace Ra

#include <Dataflow/Core/Nodes/Functionals/TransformNode.inl>
