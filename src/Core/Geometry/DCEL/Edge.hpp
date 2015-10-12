#ifndef DCEL_EDGE_H
#define DCEL_EDGE_H

#include "Definition.hpp"
#include <Core/Index/Index.hpp>
#include <Core/Index/IndexedObject.hpp>

namespace Ra {
namespace Core {
namespace Dcel {

/**
* Class Edge.
* The class Edge represent an edge of the mesh.
* The edge contains a reference to one of its halfedges.
* It is possible to sort edges in lexicographical order,
* based on the vertices indices.
*/
class Edge : public IndexedObject  {
public:
    /// CONSTRUCTOR
    Edge();                           // Build a default Edge
    Edge( const Index& index );       // Build an Edge with the given index
    Edge( const HalfEdge_ptr& he );   // Build an Edge having he as its halfedge
    Edge( const Index&        index,
          const HalfEdge_ptr& he );   // Build an Edge with the given index, having he as its halfedge
    Edge( const Edge& edge );         // Copy constructor

    /// DESTRUCTOR
    ~Edge();

    /// HALFEDGE
    inline HalfEdge_ptr  HE() const;             // Return the reference to the halfedge
    inline HalfEdge_ptr& HE();                   // Return the reference to the halfedge
    inline void setHE( const HalfEdge_ptr& he ); // Set the HalfEdge reference to he

    /// OPERATOR
    inline bool operator==( const Edge& e ) const; // Return true if the two edges points to the same vertices ( index-wise ). False otherwise
    inline bool operator< ( const Edge& e ) const; // Return true if the first edge is less than the other, in lexographical order

protected:
    /// VARIABLE
    HalfEdge_ptr m_he; // The halfedge reference
};

} // namespace DCEL
} // namespace Core
} // namespace Ra

#include "Edge.inl"

#endif // DCEL_EDGE_H
