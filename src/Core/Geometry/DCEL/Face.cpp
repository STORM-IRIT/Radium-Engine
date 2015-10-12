#include "Face.hpp"

#include "HalfEdge.hpp"

namespace Ra {
namespace Core {
namespace Dcel {

/// CONSTRUCTOR
Face::Face()                             : IndexedObject ()       , m_he( nullptr ) { }

Face::Face( const Index& index )         : IndexedObject ( index ), m_he( nullptr ) { }

Face::Face( const HalfEdge_ptr& he )     : IndexedObject ()       , m_he( he )      { }

Face::Face( const Index&        index,
            const HalfEdge_ptr& he     ) : IndexedObject ( index ), m_he( he )      { }

Face::Face( const Face& f )              : IndexedObject ( f )    , m_he( f.m_he )  { }

/// DESTRUCTOR
Face::~Face() { }

} // namespace DCEL
} // namespace Core
} // namespace Ra

