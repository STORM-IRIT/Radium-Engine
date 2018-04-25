#ifndef RADIUMENGINE_DCEL_VERTEX_HPP
#define RADIUMENGINE_DCEL_VERTEX_HPP

#include <Core/Container/Index.hpp>
#include <Core/Container/IndexedObject.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Geometry/DCEL/Definition.hpp>

namespace Ra {
namespace Core {

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

class[[deprecated]] RA_CORE_API Vertex : public Container::IndexedObject {
  public:
    /// CONSTRUCTOR

    Vertex( const Container::Index& index = Container::Index::Invalid() ); // Build a vertex with a given index
    Vertex( const HalfEdge_ptr& he );                // Build a vertex having he as its halfedge
    Vertex( const Math::Vector3& p, const Math::Vector3& n, const HalfEdge_ptr& he = nullptr,
            const Container::Index& index = Container::Index::Invalid() ); // Build a complete vertex
    Vertex( const Vertex& v ) = default;             // Copy constructor

    /// DESTRUCTOR
    ~Vertex();

    /// POINT
    inline Math::Vector3 P() const;             // Return the position of the vertex in space
    inline Math::Vector3& P();                  // Return the position of the vertex in space
    inline void setP( const Math::Vector3& p ); // Set the position of the vertex to p

    /// NORMAL
    inline Math::Vector3 N() const;             // Return the normal of the vertex
    inline Math::Vector3& N();                  // Return the normal of the vertex
    inline void setN( const Math::Vector3& n ); // Set the normal of the vertex to n

    /// HALFEDGE
    inline HalfEdge_ptr HE() const;              // Return the reference to the vertex halfedge
    inline HalfEdge_ptr& HE();                   // Return the reference to the vertex halfedge
    inline void setHE( const HalfEdge_ptr& he ); // Set the halfedge of the vertex to he

  protected:
    /// VARIABLE
    Math::Vector3 m_p;       // Vertex position
    Math::Vector3 m_n;       // Vertex normal
    HalfEdge_ptr m_he; // Reference to the halgedge
};

} // namespace Core
} // namespace Ra

#include <Core/Geometry/DCEL/Vertex.inl>

#endif // RADIUMENGINE_DCEL_VERTEX_HPP
