#ifndef RADIUMENGINE_DCEL_EDGE_ITERATOR_HPP
#define RADIUMENGINE_DCEL_EDGE_ITERATOR_HPP

#include <Core/Mesh/DCEL/Iterator/Iterator.hpp>

namespace Ra {
namespace Core {

template < typename OBJECT >
class EIterator : public Iterator< OBJECT > {
public:
    /// CONSTRUCTOR
    EIterator( HalfEdge_ptr& he );
    EIterator( const EIterator& it ) = default;

    /// DESTRUCTOR
    ~EIterator();

    /// SIZE
    inline uint size() const override;

    /// RESET
    inline void reset() override;

    /// OPERATOR
    inline EIterator& operator= ( const EIterator& it );
    inline EIterator& operator++() override;
    inline EIterator& operator--() override;
    inline bool       operator==( const EIterator& it ) const;

protected:

};

} // namespace Core
} // namespace Ra

#include <Core/Mesh/DCEL/Iterator/Edge/EdgeIterator.inl>

#endif // RADIUMENGINE_DCEL_EDGE_ITERATOR_HPP
