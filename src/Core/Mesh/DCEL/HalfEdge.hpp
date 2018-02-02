#ifndef RADIUMENGINE_DCEL_HALFEDGE_HPP
#define RADIUMENGINE_DCEL_HALFEDGE_HPP

#include <Core/Index/Index.hpp>
#include <Core/Index/IndexedObject.hpp>
#include <Core/Mesh/DCEL/Definition.hpp>

namespace Ra
{
    namespace Core
    {

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

        class [[deprecated]] RA_CORE_API HalfEdge : public IndexedObject
        {
        public:
            /// CONSTRUCTOR
            HalfEdge( const Index& index = Index::Invalid() );        // Build a HalfEdge with the given index
            HalfEdge( const Vertex_ptr&   v,
                      const HalfEdge_ptr& next  = nullptr,
                      const HalfEdge_ptr& prev  = nullptr,
                      const HalfEdge_ptr& twin  = nullptr,
                      const Face_ptr&     f     = nullptr,
                      const Index&        index = Index::Invalid() ); // Build a complete Halfedge
            HalfEdge( const HalfEdge& he ) = default;                     // Copy constructor

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
            inline void setNeighborHE( const HalfEdge_ptr& next,
                                       const HalfEdge_ptr& prev,
                                       const HalfEdge_ptr& twin ); // Set the neighboring halfedges

            /// FULLEDGE
            inline FullEdge_ptr  FE() const;                      // Return the reference to the fulledge
            inline FullEdge_ptr& FE();                            // Return the reference to the fulledge
            inline void          setFE( const FullEdge_ptr& fe ); // Set the fulledge to fe

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
            FullEdge_ptr m_fe;   // FullEdge reference
            Face_ptr     m_f;    // Face reference
        };

    } // namespace Core
} // namespace Ra

#include <Core/Mesh/DCEL/HalfEdge.inl>

#endif // RADIUMENGINE_DCEL_HALFEDGE_HPP
