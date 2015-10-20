#ifndef DCEL_H
#define DCEL_H

#include <vector>
#include <Core/Geometry/DCEL/Definition.hpp>
#include <Core/Index/IndexedObject.hpp>
#include <Core/Index/IndexMap.hpp>

namespace Ra {
namespace Core {
namespace Dcel {

/**
* Class DCEL (a.k.a. Doubly-Connected Edge List).
* The DCEL is a data structure representing a mesh as a collection of
* vertices, halfedges and faces.
*
* For further references read:
* "Computetional Geometry - Geometrys and applications"
* [ Mark de Berg, Otfried Cheong, Mark van Kreveld, Mark Overmars ]
* Chapter 2, Paragraph 2.2, Page 29
*/
class Dcel : public IndexedObject {
public:
    /// CONSTRUCTOR
    Dcel();                     // Build an empty DCEL
    Dcel( const Index& index ); // Build an empty DCEL with index equal to "index"
    Dcel( const Dcel& dcel );   // Copy constructor. The DCEL in input MUST BE COMPACTED first. Eg.: compact( dcel ); Dcel foo( dcel );

    /// DESTRUCTOR
    ~Dcel();

    /// CLEAR
    inline void clear(); // Clear the data from the DCEL, making it empty

    /// VARIABLE
    IndexMap< Vertex_ptr >   m_vertex;   // Vertices  Data
    IndexMap< HalfEdge_ptr > m_halfedge; // HalfEdges Data
    IndexMap< Face_ptr >     m_face;     // Faces     Data
};

} // namespace DCEL
} // namespace Core
} // namespace Ra

#include "Dcel.inl"

#endif // DCEL_H
