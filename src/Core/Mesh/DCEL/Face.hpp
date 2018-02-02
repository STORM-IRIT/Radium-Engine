#ifndef RADIUMENGINE_DCEL_FACE_HPP
#define RADIUMENGINE_DCEL_FACE_HPP

#include <Core/Index/Index.hpp>
#include <Core/Index/IndexedObject.hpp>
#include <Core/Mesh/DCEL/Definition.hpp>

namespace Ra
{
    namespace Core
    {

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

        class [[deprecated]] RA_CORE_API Face : public IndexedObject
        {
        public:
            /// CONSTRUCTOR
            Face( const Index& index = Index::Invalid() );       // Build a Face with the given index
            Face( const HalfEdge_ptr& he,
                  const Index&        index =
                      Index::Invalid() ); // Build a Face with the given index and having he as its first halfedge
            Face( const Face& f ) = default;                         // Copy constructor

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

    } // namespace Core
} // namespace Ra

#include <Core/Mesh/DCEL/Face.inl>

#endif // RADIUMENGINE_DCEL_FACE_HPP
