#ifndef RADIUMENGINE_DCEL_VERTEX_FACE_ITERATOR_HPP
#define RADIUMENGINE_DCEL_VERTEX_FACE_ITERATOR_HPP

#include <Core/Mesh/DCEL/Iterator/Vertex/VertexIterator.hpp>

namespace Ra {
namespace Core {

class VFIterator : public VIterator< Face_ptr > {
public:
    /// CONSTRUCTOR
    VFIterator( const Vertex& v );
    VFIterator( const VFIterator& it ) = default;

    /// DESTRUCTOR
    ~VFIterator();

    /// LIST
    inline FaceList list() const override;

    /// OPERATOR
    inline Face_ptr operator->() const override;
};

} // namespace Core
} // namespace Ra

#include <Core/Mesh/DCEL/Iterator/Vertex/VFIterator.inl>

#endif // RADIUMENGINE_DCEL_VERTEX_FACE_ITERATOR_HPP
