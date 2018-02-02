#ifndef RADIUMENGINE_DCEL_HPP
#define RADIUMENGINE_DCEL_HPP

#include <vector>
#include <Core/Index/IndexMap.hpp>
#include <Core/Index/IndexedObject.hpp>
#include <Core/Mesh/DCEL/Definition.hpp>

namespace Ra {
namespace Core {

/**
* Class DCEL (a.k.a. Doubly-Connected Edge List).
* The DCEL is a data structure representing a mesh as a collection of
* vertices, halfedges and faces.
*
* For further references read:
* "Computational Geometry - Geometry and applications"
* [ Mark de Berg, Otfried Cheong, Mark van Kreveld, Mark Overmars ]
* Chapter 2, Paragraph 2.2, Page 29
*/

class [[deprecated]] RA_CORE_API Dcel : public IndexedObject {
public:
    /// CONSTRUCTOR
    Dcel( const Index& index = Index::Invalid() ); // Build an empty DCEL with index equal to "index"
    Dcel( const Dcel& dcel );                          // Copy constructor

    /// DESTRUCTOR
    ~Dcel();

    /// CLEAR
    inline void clear(); // Clear the data from the DCEL, making it empty

    /// QUERY
    inline bool empty() const;

    /// VARIABLE
    IndexMap< Vertex_ptr >   m_vertex;   // Vertices  Data
    IndexMap< HalfEdge_ptr > m_halfedge; // HalfEdges Data
    IndexMap< FullEdge_ptr > m_fulledge; // FullEdge  Data
    IndexMap< Face_ptr >     m_face;     // Faces     Data
};

} // namespace Core
} // namespace Ra

#include <Core/Mesh/DCEL/Dcel.inl>

#endif // RADIUMENGINE_DCEL_HPP
