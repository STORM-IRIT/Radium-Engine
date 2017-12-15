#ifndef RADIUMENGINE_DCEL_FACE_HALFEDGE_ITERATOR_HPP
#define RADIUMENGINE_DCEL_FACE_HALFEDGE_ITERATOR_HPP

#include <Core/Mesh/DCEL/Iterator/Face/FaceIterator.hpp>

namespace Ra {
namespace Core {

class [[deprecated]] FHEIterator : public FIterator< HalfEdge > {
public:
    /// CONSTRUCTOR
    FHEIterator( Face_ptr& f );
    FHEIterator( const FHEIterator& it ) = default;

    /// DESTRUCTOR
    ~FHEIterator();

    /// LIST
    inline HalfEdgeList list() const override;

    /// OPERATOR
    inline HalfEdge* operator->() const override;
};

} // namespace Core
} // namespace Ra

#include <Core/Mesh/DCEL/Iterator/Face/FHEIterator.inl>

#endif // RADIUMENGINE_DCEL_FACE_HALFEDGE_ITERATOR_HPP
