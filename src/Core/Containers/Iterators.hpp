#ifndef RADIUMENGINE_ITERATORS_HPP
#define RADIUMENGINE_ITERATORS_HPP

namespace Ra {
namespace Core {

namespace _internalIterator {
template <class T>
struct _reversed {
    T& t;
    explicit _reversed( T& _t ) : t( _t ) {}
};
template <class T>
struct _creversed {
    const T& t;
    explicit _creversed( const T& _t ) : t( _t ) {}
};
} // namespace _internalIterator

// Provide reverse iterators for range loops and std::containers:
// \code for (auto x: reversed(c)) ; \endcode
// source: https://stackoverflow.com/a/21510185
template <class T>
_internalIterator::_reversed<T> reversed( T& t ) {
    return _internalIterator::_reversed<T>( t );
}
template <class T>
_internalIterator::_reversed<T const> reversed( T const& t ) {
    return _internalIterator::_reversed<T const>( t );
}
} // namespace Core
} // namespace Ra

namespace std {
template <class T>
auto begin( Ra::Core::_internalIterator::_reversed<T>& r ) -> decltype( r.t.rbegin() ) {
    return r.t.rbegin();
}
template <class T>
auto end( Ra::Core::_internalIterator::_reversed<T>& r ) -> decltype( r.t.rend() ) {
    return r.t.rend();
}

template <class T>
auto begin( Ra::Core::_internalIterator::_creversed<T> const& cr ) -> decltype( cr.t.rbegin() ) {
    return cr.t.rbegin();
}
template <class T>
auto end( Ra::Core::_internalIterator::_creversed<T> const& cr ) -> decltype( cr.t.rend() ) {
    return cr.t.rend();
}
} // namespace std

#endif // RADIUMENGINE_ALGORITHM_HPP
