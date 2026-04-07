#include <Dataflow/Core/GraphNodes.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {

const std::string& GraphNode::node_typename() {
    static std ::string demangledName { "GraphNode" };
    return demangledName;
}

const std::string& GraphInputNode::node_typename() {
    static std::string demangledName { "GraphInputNode" };
    //    std::cerr << " node typename\n";
    return demangledName;
}

const std::string& GraphOutputNode::node_typename() {
    static std ::string demangledName { "GraphOutputNode" };
    return demangledName;
}

} // namespace Core
} // namespace Dataflow
} // namespace Ra
