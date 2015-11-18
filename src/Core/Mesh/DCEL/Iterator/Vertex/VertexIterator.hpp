#ifndef RADIUMENGINE_DCEL_VERTEX_ITERATOR_HPP
#define RADIUMENGINE_DCEL_VERTEX_ITERATOR_HPP

#include <Core/Mesh/DCEL/Iterator/Iterator.hpp>

namespace Ra {
namespace Core {

template < typename OBJECT >
class VIterator : public Iterator< OBJECT > {
public:
    /// CONSTRUCTOR
    VIterator( const Vertex& v );
    VIterator( const VIterator& it ) = default;

    /// DESTRUCTOR
    ~VIterator();

    /// SIZE
    inline uint size() const override;

    /// RESET
    inline void reset() override;

    /// OPERATOR
    inline VIterator& operator= ( const VIterator& it ) override;
    inline VIterator& operator++() override;
    inline VIterator& operator--() override;
    inline bool       operator==( const VIterator& it ) const override;

protected:
    /// VARIABLE
    Vertex_ptr   m_v;
};

} // namespace Core
} // namespace Ra

#include <Core/Mesh/DCEL/Iterator/Vertex/VertexIterator.inl>

#endif // RADIUMENGINE_DCEL_VERTEX_ITERATOR_HPP
