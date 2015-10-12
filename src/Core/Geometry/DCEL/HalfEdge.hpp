#ifndef DCEL_HALFEDGE_H
#define DCEL_HALFEDGE_H

#include "Definition.hpp"
#include <Core/Index/Index.hpp>
#include <Core/Index/IndexedObject.hpp>

namespace Ra {
namespace Core {
namespace Dcel {

/**
* Class HalfEdge
* The class HalfEdge represents a halfedge of the DCEL.
* A HalfEdge is the directed edge outgoing a vertex, pointing the next halfedge of a face.
* The halfedge also points to the previous one in the chain, points to the face it belongs to,
* and points to the incident halfedge belonging to the neighboring face (the so called twin halfedge).
*
* For further references read:
* "Computetional Geometry - Geometrys and applications"
* [ Mark de Berg, Otfried Cheong, Mark van Kreveld, Mark Overmars ]
* Chapter 2, Paragraph 2.2, Page 29
*
*/
class HalfEdge : public IndexedObject {
public:
    /// CONSTRUCTOR
    HalfEdge();                           // Default constructor
    HalfEdge( const Index& index );       // Build a HalfEdge with the given index
    HalfEdge( const Vertex_ptr& v );      // Build a HalfEdge outgoing the vertex v
    HalfEdge( const HalfEdge_ptr& next,
              const HalfEdge_ptr& prev,
              const HalfEdge_ptr& twin ); // Build a HalfEdge pointing to next, prev and twin halfedge
    HalfEdge( const Face_ptr& f );        // Build a HalfEdge pointing to the face f
    HalfEdge( const Index&      index,
              const Vertex_ptr& v );      // Build a HalfEdge with the given index, outgoing the vertex v
    HalfEdge( const Index&        index,
              const Vertex_ptr&   v,
              const HalfEdge_ptr& next,
              const HalfEdge_ptr& prev,
              const HalfEdge_ptr& twin,
              const Face_ptr&     f );    // Build a complete Halfedge
    HalfEdge( const HalfEdge& he );       // Copy constructor

    /// DESTRUCTOR
    ~HalfEdge();

    /// VERTEX
    inline Vertex_ptr  V() const;                   // Return the reference to the halfedge vertex
    inline Vertex_ptr& V();                         // Return the reference to the halfedge vertex
    inline void        setV( const Vertex_ptr& v ); // Set the halfedge vertex to v

    /// NEXT
    inline HalfEdge_ptr  Next() const;                        // Return the reference to the next halfedge
    inline HalfEdge_ptr& Next();                              // Return the reference to the next halfedge
    inline void          setNext( const HalfEdge_ptr& next ); // Set the next halfedge to next

    /// PREV
    inline HalfEdge_ptr  Prev() const;                        // Return the reference to the prev halfedge
    inline HalfEdge_ptr& Prev();                              // Return the reference to the prev halfedge
    inline void          setPrev( const HalfEdge_ptr& prev ); // Set the prev halfedge to prev

    /// TWIN
    inline HalfEdge_ptr  Twin() const;                        // Return the reference to the twin halfedge
    inline HalfEdge_ptr& Twin();                              // Return the reference to the twin halfedge
    inline void          setTwin( const HalfEdge_ptr& twin ); // Set the twin halfedge to twin

    /// HALFEDGE
    inline void setNeighbor( const HalfEdge_ptr& next,
                             const HalfEdge_ptr& prev,
                             const HalfEdge_ptr& twin ); // Set the neighboring halfedges

    /// FACE
    inline Face_ptr  F() const;                 // Return the reference to the face
    inline Face_ptr& F();                       // Return the reference to the face
    inline void      setF( const Face_ptr& f ); // Set the face to f

protected:
    /// VARIABLE
    Vertex_ptr   m_v;    // Vertex reference
    HalfEdge_ptr m_next; // Next halfedge reference
    HalfEdge_ptr m_prev; // Prev halfedge reference
    HalfEdge_ptr m_twin; // Twin halfedge reference
    Face_ptr     m_f;    // Face reference
};

} // namespace DCEL
} // namespace Core
} // namespace Ra

#include "HalfEdge.inl"

#endif // DCEL_HALFEDGE_H
