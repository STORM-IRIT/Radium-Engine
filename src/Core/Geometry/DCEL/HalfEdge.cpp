#include <Core/Geometry/DCEL/HalfEdge.hpp>

#include <Core/Geometry/DCEL/Vertex.hpp>
#include <Core/Geometry/DCEL/Face.hpp>

namespace Ra {
namespace Core {
namespace Dcel {

/// CONSTRUCTOR
HalfEdge::HalfEdge() : IndexedObject (),
                       m_v( nullptr ),
                       m_next( nullptr ),
                       m_prev( nullptr ),
                       m_twin( nullptr ),
                       m_f( nullptr ) { }

HalfEdge::HalfEdge( const Index& index ) : IndexedObject ( index ),
                                           m_v( nullptr ),
                                           m_next( nullptr ),
                                           m_prev( nullptr ),
                                           m_twin( nullptr ),
                                           m_f( nullptr ) { }

HalfEdge::HalfEdge( const Vertex_ptr& v ) : IndexedObject (),
                                            m_v( v ),
                                            m_next( nullptr ),
                                            m_prev( nullptr ),
                                            m_twin( nullptr ),
                                            m_f( nullptr ) { }

HalfEdge::HalfEdge( const HalfEdge_ptr& next,
                    const HalfEdge_ptr& prev,
                    const HalfEdge_ptr& twin ) : IndexedObject (),
                                                 m_v( nullptr ),
                                                 m_next( next ),
                                                 m_prev( prev ),
                                                 m_twin( twin ),
                                                 m_f( nullptr ) { }

HalfEdge::HalfEdge( const Face_ptr& f ) : IndexedObject (),
                                          m_v( nullptr ),
                                          m_next( nullptr ),
                                          m_prev( nullptr ),
                                          m_twin( nullptr ),
                                          m_f( f ) { }

HalfEdge::HalfEdge( const Index&      index,
                    const Vertex_ptr& v     ) : IndexedObject ( index ),
                                                m_v( v ),
                                                m_next( nullptr ),
                                                m_prev( nullptr ),
                                                m_twin( nullptr ),
                                                m_f( nullptr ) { }
HalfEdge::HalfEdge( const Index&        index,
                    const Vertex_ptr&   v,
                    const HalfEdge_ptr& next,
                    const HalfEdge_ptr& prev,
                    const HalfEdge_ptr& twin,
                    const Face_ptr&     f     ) : IndexedObject ( index ),
                                                  m_v( v ),
                                                  m_next( next ),
                                                  m_prev( prev ),
                                                  m_twin( twin ),
                                                  m_f( f ) { }

HalfEdge::HalfEdge( const HalfEdge& he ) : IndexedObject ( he ),
                                           m_v( he.m_v ),
                                           m_next( he.m_next ),
                                           m_prev( he.m_prev ),
                                           m_twin( he.m_twin ),
                                           m_f( he.m_f ) { }

/// DESTRUCTOR
HalfEdge::~ HalfEdge() { }

} // namespace DCEL
} // namespace Core
} // namespace Ra
