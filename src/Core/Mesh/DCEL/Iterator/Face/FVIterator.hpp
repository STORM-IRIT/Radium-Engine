#ifndef RADIUMENGINE_DCEL_FACE_VERTEX_ITERATOR_HPP
#define RADIUMENGINE_DCEL_FACE_VERTEX_ITERATOR_HPP

#include <Core/Mesh/DCEL/Iterator/Face/FaceIterator.hpp>

namespace Ra {
namespace Core {

class FVIterator : public FIterator< Vertex_ptr > {
public:
    /// CONSTRUCTOR
    FVIterator( const Face& f );
    FVIterator( const FVIterator& it ) = default;

    /// DESTRUCTOR
    ~FVIterator();

    /// LIST
    inline VertexList list() const override;

    /// OPERATOR
    inline Vertex_ptr operator->() const override;
};

} // namespace Core
} // namespace Ra

#include <Core/Mesh/DCEL/Iterator/Face/FVIterator.inl>

#endif // RADIUMENGINE_DCEL_FACE_VERTEX_ITERATOR_HPP
