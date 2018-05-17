#ifndef RADIUMENGINE_STDUTILS_HPP_
#define RADIUMENGINE_STDUTILS_HPP_

#include <Core/RaCore.hpp>

#include <functional>

/// This file contains utilities and wrapper to the standard library
/// that didn't fit into any other category (see Strings and Containers)
namespace Ra {
namespace Core {
namespace Utils {
/// Helper function to combine multiple hashes by accumulation
/// Called repeatedly to accumulate the hash value.
/// Taken from boost.hash. http://www.boost.org/doc/libs/1_61_0/doc/html/hash.html
template <typename T>
void hash_combine( std::size_t& seed, const T& key ) {
    std::hash<T> hasher;
    seed ^= hasher( key ) + 0x9e3779b9 + ( seed << 6 ) + ( seed >> 2 );
}

/// Hashes a std::pair of items.
template <typename T1, typename T2>
inline std::size_t hash( const std::pair<T1, T2>& p ) {
    std::size_t result( 0 );
    hash_combine( result, p.first );
    hash_combine( result, p.second );
    return result;
};
} // namespace Utils
} // namespace Core
} // namespace Ra

#endif
