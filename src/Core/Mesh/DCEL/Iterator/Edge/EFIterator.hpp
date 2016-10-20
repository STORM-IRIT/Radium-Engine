#ifndef RADIUMENGINE_DCEL_EDGE_FACE_ITERATOR_HPP
#define RADIUMENGINE_DCEL_EDGE_FACE_ITERATOR_HPP

#include <Core/Mesh/DCEL/Iterator/Edge/EdgeIterator.hpp>

namespace Ra {
namespace Core {

class EFIterator : public EIterator< Face > {
public:
    /// CONSTRUCTOR
    EFIterator( HalfEdge_ptr& he );
    EFIterator( const EFIterator& it ) = default;

    /// DESTRUCTOR
    ~EFIterator();

    /// LIST
    inline FaceList list() const override;

    /// OPERATOR
    inline Face* operator->() const override;
};

} // namespace Core
} // namespace Ra

#include <Core/Mesh/DCEL/Iterator/Edge/EFIterator.inl>

#endif // RADIUMENGINE_DCEL_EDGE_FACE_ITERATOR_HPP
