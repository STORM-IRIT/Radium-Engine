#ifndef RADIUMENGINE_DCEL_FACE_FULLEDGE_ITERATOR_HPP
#define RADIUMENGINE_DCEL_FACE_FULLEDGE_ITERATOR_HPP

#include <Core/Mesh/DCEL/Iterator/Face/FaceIterator.hpp>

namespace Ra {
namespace Core {

class [[deprecated]] FFEIterator : public FIterator< FullEdge > {
public:
    /// CONSTRUCTOR
    FFEIterator( Face_ptr& f );
    FFEIterator( const FFEIterator& it ) = default;

    /// DESTRUCTOR
    ~FFEIterator();

    /// LIST
    inline FullEdgeList list() const override;

    /// OPERATOR
    inline FullEdge* operator->() const override;
};

} // namespace Core
} // namespace Ra

#include <Core/Mesh/DCEL/Iterator/Face/FFEIterator.inl>

#endif // RADIUMENGINE_DCEL_FACE_ITERATOR_HPP
