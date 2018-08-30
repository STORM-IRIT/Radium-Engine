#include <Core/Utils/Graph/AdjacencyList.hpp>

namespace Ra {
namespace Core {
namespace Graph {

AdjacencyList::AdjacencyList() : m_child(), m_parent() {}

AdjacencyList::AdjacencyList( const uint n ) : m_child( n ), m_parent( n, -1 ) {}

AdjacencyList::AdjacencyList( const AdjacencyList& adj ) :
    m_child( adj.m_child ),
    m_parent( adj.m_parent ) {}

AdjacencyList::~AdjacencyList() {}

} // namespace Graph
} // namespace Core
} // namespace Ra
