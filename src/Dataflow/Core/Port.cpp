#include <Dataflow/Core/PortIn.hpp>
#include <Dataflow/Core/PortOut.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {

bool PortBaseIn::accept( PortBaseOut* portOut ) const {
    return portOut->getType() == getType();
}

PortBaseIn::PortBaseIn( Node* node, const std::string& name, std::type_index type ) :
    PortBase( name, type, node ) {}

bool PortBaseIn::isLinkMandatory() const {
    return !hasDefaultValue();
}

PortBaseOut::PortBaseOut( Node* node, const std::string& name, std::type_index type ) :
    PortBase( name, type, node ) {}

} // namespace Core
} // namespace Dataflow
} // namespace Ra
