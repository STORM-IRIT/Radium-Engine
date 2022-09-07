#pragma once
#include <Dataflow/Core/Node.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Sinks {

template <typename T>
class SinkNode : public Node
{
  public:
    explicit SinkNode( const std::string& name );

    void execute() override;
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
    void fromJsonInternal( const nlohmann::json& data ) override;

  private:
    T m_data;

  public:
    static const std::string getTypename();
};

} // namespace Sinks
} // namespace Core
} // namespace Dataflow
} // namespace Ra

#include <Dataflow/Core/Nodes/Sinks/SinkNode.inl>
