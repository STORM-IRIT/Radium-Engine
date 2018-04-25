#ifndef RADIUMENGINE_DCEL_HPP
#define RADIUMENGINE_DCEL_HPP

#include <Core/Container/IndexMap.hpp>
#include <Core/Container/IndexedObject.hpp>
#include <Core/Geometry/DCEL/Definition.hpp>
#include <vector>

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

class[[deprecated]] RA_CORE_API Dcel : public Container::IndexedObject {
  public:
    /// CONSTRUCTOR
    Dcel( const Container::Index& index =
              Container::Index::Invalid() ); // Build an empty DCEL with index equal to "index"
    Dcel( const Dcel& dcel );     // Copy constructor

    /// DESTRUCTOR
    ~Dcel();

    /// CLEAR
    inline void clear(); // Clear the data from the DCEL, making it empty

    /// QUERY
    inline bool empty() const;

    /// VARIABLE
    Container::IndexMap<Vertex_ptr> m_vertex;     // Vertices  Data
    Container::IndexMap<HalfEdge_ptr> m_halfedge; // HalfEdges Data
    Container::IndexMap<FullEdge_ptr> m_fulledge; // FullEdge  Data
    Container::IndexMap<Face_ptr> m_face;         // Faces     Data
};

} // namespace Core
} // namespace Ra

#include <Core/Geometry/DCEL/Dcel.inl>

#endif // RADIUMENGINE_DCEL_HPP
