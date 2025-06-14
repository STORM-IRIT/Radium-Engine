#include <Dataflow/Core/Port.hpp>
#include <Dataflow/Core/PortIn.hpp>
#include <Dataflow/Core/PortOut.hpp>
#include <string>
#include <typeindex>

namespace Ra {
namespace Dataflow {
namespace Core {
class Node;

bool PortBaseIn::accept( PortBaseOut* portOut ) const {
    return portOut->type() == type();
}

PortBaseIn::PortBaseIn( Node* node, const std::string& name, std::type_index type ) :
    PortBase( name, type, node ) {}

bool PortBaseIn::is_link_mandatory() const {
    return !has_default_value();
}

PortBaseOut::PortBaseOut( Node* node, const std::string& name, std::type_index type ) :
    PortBase( name, type, node ) {}

} // namespace Core
} // namespace Dataflow
} // namespace Ra
