#include <Core/Utils/AdjacencyList.hpp>

namespace Ra {
namespace Core {
namespace Utils {

/// CONSTRUCTOR
AdjacencyList::AdjacencyList() : m_child(), m_parent() {}
AdjacencyList::AdjacencyList( const uint n ) : m_child( n ), m_parent( n, -1 ) {}
AdjacencyList::AdjacencyList( const AdjacencyList& adj ) :
    m_child( adj.m_child ),
    m_parent( adj.m_parent ) {}

/// DESTRUCTOR
AdjacencyList::~AdjacencyList() {}

} // namespace Utils
} // namespace Core
} // namespace Ra
