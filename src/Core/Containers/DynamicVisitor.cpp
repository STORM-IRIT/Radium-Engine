#include <Core/Containers/DynamicVisitor.hpp>
#include <any>
#include <functional>
#include <typeindex>
#include <unordered_map>
#include <utility>

namespace Ra {
namespace Core {

bool DynamicVisitor::accept( const std::type_index& id ) const {
    return m_visitorOperator.find( id ) != m_visitorOperator.cend();
}

void DynamicVisitor::operator()( std::any&& in, std::any&& userParam ) const {
    m_visitorOperator.at( std::type_index( in.type() ) )( in, std::forward<std::any>( userParam ) );
}

} // namespace Core
} // namespace Ra
