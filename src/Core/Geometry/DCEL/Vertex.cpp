#include "Vertex.hpp"

#include "HalfEdge.hpp"

namespace Ra {
namespace Core {
namespace Dcel {

/// CONSTRUCTOR
Vertex::Vertex()                             : IndexedObject ()       , m_p()       , m_n()       , m_he( nullptr ) { }

Vertex::Vertex( const Index& index )         : IndexedObject ( index ), m_p()       , m_n()       , m_he( nullptr ) { }

Vertex::Vertex( const Vector3& p )           : IndexedObject ()       , m_p( p )    , m_n()       , m_he( nullptr ) { }

Vertex::Vertex( const Vector3& p,
                const Vector3& n )           : IndexedObject ()       , m_p( p )    , m_n( n )    , m_he( nullptr ) { }

Vertex::Vertex( const HalfEdge_ptr& he )     : IndexedObject ()       , m_p()       , m_n()       , m_he( he )      { }

Vertex::Vertex( const Index&        index,
                const Vector3&      p,
                const Vector3&      n )      : IndexedObject ( index ), m_p( p )    , m_n( n )    , m_he( nullptr ) { }

Vertex::Vertex( const Index&        index,
                const Vector3&      p,
                const Vector3&      n,
                const HalfEdge_ptr& he     ) : IndexedObject ( index ), m_p( p )    , m_n( n )    , m_he( he )      { }

Vertex::Vertex( const Vertex& v )            : IndexedObject ( v )    , m_p( v.m_p ), m_n( v.m_n ), m_he( v.m_he )  { }

/// DESTRUCTOR
Vertex::~Vertex() { }

} // namespace DCEL
} // namespace Core
} // namespace Ra
