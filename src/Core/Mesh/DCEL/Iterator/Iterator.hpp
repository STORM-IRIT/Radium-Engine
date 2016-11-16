#ifndef RADIUMENGINE_DCEL_ITERATOR_HPP
#define RADIUMENGINE_DCEL_ITERATOR_HPP

#include <vector>
#include <Core/Mesh/DCEL/Definition.hpp>

namespace Ra {
namespace Core {

template < typename OBJECT >
class Iterator {
public:
    /// CONSTRUCTOR
    Iterator();
    Iterator( const Iterator& it ) = default;

    /// DESTRUCTOR
    virtual ~Iterator();

    /// SIZE
    virtual uint size() const = 0;

    /// LIST
    virtual std::vector< std::shared_ptr< OBJECT > > list() const = 0;

    /// RESET
    virtual void reset() = 0;

    /// OPERATOR
    virtual OBJECT*   operator->() const = 0;
    virtual Iterator& operator++() = 0;
    virtual Iterator& operator--() = 0;
    inline  Iterator& operator+=( const uint n ) const;
    inline  Iterator& operator-=( const uint n ) const;

protected:
    /// VARIABLE
    HalfEdge_ptr m_he;
};


} // namespace Core
} // namespace Ra

#include <Core/Mesh/DCEL/Iterator/Iterator.inl>

#endif // RADIUMENGINE_DCEL_ITERATOR_HPP
