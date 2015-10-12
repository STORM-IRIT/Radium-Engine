#ifndef DCEL_FACE_H
#define DCEL_FACE_H

#include "Definition.hpp"
#include <Core/Index/Index.hpp>
#include <Core/Index/IndexedObject.hpp>

namespace Ra {
namespace Core {
namespace Dcel {

/**
* Class Face
* The class Face describe a face of the DCEL. The face is a planar polygon
* described by a chain of halfedges, ordered in anticlockwise order.
* The face store a reference to one of its halfedges.
*
* For further references read:
* "Computetional Geometry - Geometrys and applications"
* [ Mark de Berg, Otfried Cheong, Mark van Kreveld, Mark Overmars ]
* Chapter 2, Paragraph 2.2, Page 29
*
*/
class Face : public IndexedObject  {
public:
    /// CONSTRUCTOR
    Face();                           // Default constructor
    Face( const Index& index );       // Build a Face with the given index
    Face( const HalfEdge_ptr& he );   // Build a Face with he as its first halfedge
    Face( const Index&        index,
          const HalfEdge_ptr& he );   // Build a Face with the given index and having he as its first halfedge
    Face( const Face& f );            // Copy constructor

    /// DESTRUCTOR
    ~Face();

    /// HALFEDGE
    inline HalfEdge_ptr  HE() const;                      // Return the first halfedge of the face
    inline HalfEdge_ptr& HE();                            // Return the first halfedge of the face
    inline void          setHE( const HalfEdge_ptr& he ); // Set the halfedge of the face to he

protected:
    /// VARIABLE
    HalfEdge_ptr m_he; // The halfedge reference
};

} // namespace DCEL
} // namespace Core
} // namespace Ra

#include "Face.inl"

#endif // DCEL_FACE_H
