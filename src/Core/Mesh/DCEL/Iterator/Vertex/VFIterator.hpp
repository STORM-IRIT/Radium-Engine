#ifndef RADIUMENGINE_DCEL_VERTEX_FACE_ITERATOR_HPP
#define RADIUMENGINE_DCEL_VERTEX_FACE_ITERATOR_HPP

#include <Core/Mesh/DCEL/Iterator/Vertex/VertexIterator.hpp>

namespace Ra {
namespace Core {

class [[deprecated]] VFIterator : public VIterator< Face > {
public:
    /// CONSTRUCTOR
    VFIterator( Vertex_ptr& v );
    VFIterator( const VFIterator& it ) = default;

    /// DESTRUCTOR
    ~VFIterator();

    /// LIST
    inline FaceList list() const override;

    /// OPERATOR
    inline Face* operator->() const override;
};

} // namespace Core
} // namespace Ra

#include <Core/Mesh/DCEL/Iterator/Vertex/VFIterator.inl>

#endif // RADIUMENGINE_DCEL_VERTEX_FACE_ITERATOR_HPP
