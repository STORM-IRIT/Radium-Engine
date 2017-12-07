#ifndef RADIUMENGINE_DCEL_FACE_VERTEX_ITERATOR_HPP
#define RADIUMENGINE_DCEL_FACE_VERTEX_ITERATOR_HPP

#include <Core/Mesh/DCEL/Iterator/Face/FaceIterator.hpp>

namespace Ra {
namespace Core {

class [[deprecated]] FVIterator : public FIterator< Vertex > {
public:
    /// CONSTRUCTOR
    FVIterator( Face_ptr& f );
    FVIterator( const FVIterator& it ) = default;

    /// DESTRUCTOR
    ~FVIterator();

    /// LIST
    inline VertexList list() const override;

    /// OPERATOR
    inline Vertex* operator->() const override;
};

} // namespace Core
} // namespace Ra

#include <Core/Mesh/DCEL/Iterator/Face/FVIterator.inl>

#endif // RADIUMENGINE_DCEL_FACE_VERTEX_ITERATOR_HPP
