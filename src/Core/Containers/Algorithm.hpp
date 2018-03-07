#ifndef RADIUMENGINE_ALGORITHM_HPP
#define RADIUMENGINE_ALGORITHM_HPP

#include <algorithm>

namespace Ra {
namespace Core {
/// Really copy elements to out if pred is verified and really remove them from in
/// Uses a back_insert_iterator
template <typename Container, typename Pred>
inline void remove_copy_if( Container& in, Container& out, Pred pred ) {
    auto it = std::remove_if( in.begin(), in.end(), pred );
    std::copy( it, in.end(), std::back_insert_iterator<Container>( out ) );
    in.erase( it, in.end() );
}
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_ALGORITHM_HPP
