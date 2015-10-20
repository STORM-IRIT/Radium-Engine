#ifndef DCEL_VERTEX_H
#define DCEL_VERTEX_H

#include <Core/Geometry/DCEL/Definition.hpp>
#include <Core/Index/Index.hpp>
#include <Core/Index/IndexedObject.hpp>

namespace Ra {
namespace Core {
namespace Dcel {

/**
* Class Vertex
* The class vertex represents a vertex of the DCEL.
* A vertex is described by its position in space and a reference to an outgoing halfedge.
*
* For further references read:
* "Computetional Geometry - Geometrys and applications"
* [ Mark de Berg, Otfried Cheong, Mark van Kreveld, Mark Overmars ]
* Chapter 2, Paragraph 2.2, Page 29
*
*/
class Vertex : public IndexedObject {
public:
    /// CONSTRUCTOR
    Vertex();                           // Default constructor
    Vertex( const Index& index );       // Build a vertex with a given index
    Vertex( const Vector3& p );         // Build a vertex with the position p
    Vertex( const HalfEdge_ptr& he );   // Build a vertex having he as its halfedge
    Vertex( const Vector3& p,
            const Vector3& n );         // Build a vertex with the position p and its normal n
    Vertex( const Index&   index,
            const Vector3& p,
            const Vector3& n );         // Build a vertex with the given index, the position p and its normal n
    Vertex( const Index&        index,
            const Vector3&      p,
            const Vector3&      n,
            const HalfEdge_ptr& he );   // Build a complete vertex
    Vertex( const Vertex& v );          // Copy constructor

    /// DESTRUCTOR
    ~Vertex();

    /// POINT
    inline Vector3  P() const;                  // Return the position of the vertex in space
    inline Vector3& P();                        // Return the position of the vertex in space
    inline void     setP( const Vector3& p );   // Set the position of the vertex to p

    /// NORMAL
    inline Vector3  N() const;                  // Return the normal of the vertex
    inline Vector3& N();                        // Return the normal of the vertex
    inline void     setN( const Vector3& n );   // Set the normal of the vertex to n

    /// HALFEDGE
    inline HalfEdge_ptr  HE() const;                      // Return the reference to the vertex halfedge
    inline HalfEdge_ptr& HE();                            // Return the reference to the vertex halfedge
    inline void          setHE( const HalfEdge_ptr& he ); // Set the halfedge of the vertex to he

protected:
    /// VARIABLE
    Vector3      m_p;   // Vertex position
    Vector3      m_n;   // Vertex normal
    HalfEdge_ptr m_he;  // Reference to the halgedge
};

} // namespace DCEL
} // namespace Core
} // namespace Ra

#include "Vertex.inl"

#endif // DCEL_VERTEX_H
