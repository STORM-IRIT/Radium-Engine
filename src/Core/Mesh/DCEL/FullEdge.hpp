#ifndef RADIUMENGINE_DCEL_EDGE_HPP
#define RADIUMENGINE_DCEL_EDGE_HPP

#include <Core/Mesh/DCEL/Definition.hpp>
#include <Core/Index/Index.hpp>
#include <Core/Index/IndexedObject.hpp>

namespace Ra {
namespace Core {

/**
* Class Edge.
* The class Edge represent an edge of the mesh.
* The edge contains a reference to one of its halfedges.
* It is possible to sort edges in lexicographical order,
* based on the vertices indices.
*/
class FullEdge : public IndexedObject  {
public:
    /// CONSTRUCTOR
    FullEdge( const Index& index = Index::INVALID_IDX() );        // Build an Edge with the given index
    FullEdge( const HalfEdge_ptr& he,
              const Index&        index = Index::INVALID_IDX() ); // Build an Edge with the given index, having he as its halfedge
    FullEdge( const FullEdge& edge ) = default;                   // Copy constructor

    /// DESTRUCTOR
    ~FullEdge();

    /// HALFEDGE
    inline HalfEdge_ptr  HE() const;             // Return the reference to the halfedge
    inline HalfEdge_ptr& HE();                   // Return the reference to the halfedge
    inline void setHE( const HalfEdge_ptr& he ); // Set the HalfEdge reference to he

    /// OPERATOR
    inline bool operator==( const FullEdge& e ) const; // Return true if the two edges points to the same vertices ( index-wise ). False otherwise
    inline bool operator< ( const FullEdge& e ) const; // Return true if the first edge is less than the other, in lexographical order

protected:
    /// VARIABLE
    HalfEdge_ptr m_he; // The halfedge reference
};

} // namespace Core
} // namespace Ra

#include <Core/Mesh/DCEL/FullEdge.inl>

#endif // RADIUMENGINE_DCEL_EDGE_HPP
