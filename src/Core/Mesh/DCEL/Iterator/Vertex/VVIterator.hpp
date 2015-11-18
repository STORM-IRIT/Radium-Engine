#ifndef RADIUMENGINE_DCEL_VERTEX_VERTEX_ITERATOR_HPP
#define RADIUMENGINE_DCEL_VERTEX_VERTEX_ITERATOR_HPP

#include <Core/Mesh/DCEL/Iterator/Vertex/VertexIterator.hpp>

namespace Ra {
namespace Core {

class VVIterator : public VIterator< Vertex_ptr > {
public:
    /// CONSTRUCTOR
    VVIterator( const Vertex& v );
    VVIterator( const VVIterator& it ) = default;

    /// DESTRUCTOR
    ~VVIterator();

    /// LIST
    inline VertexList list() const override;

    /// OPERATOR
    inline Vertex_ptr operator->() const override;
};

} // namespace Core
} // namespace Ra

#include <Core/Mesh/DCEL/Iterator/Vertex/VVIterator.inl>

#endif // RADIUMENGINE_DCEL_VERTEX_VERTEX_ITERATOR_HPP
