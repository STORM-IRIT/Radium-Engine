#include <Core/Utils/CircularIndex.hpp>
#include <stddef.h>

namespace Ra {
namespace Core {
namespace Utils {

/// CONSTRUCTOR
CircularIndex::CircularIndex( const size_t size, const size_t i ) : N( size ) {
    setValue( i );
}
CircularIndex::CircularIndex( const CircularIndex& index ) : idx( index.idx ), N( index.N ) {}

/// DESTRUCTOR
CircularIndex::~CircularIndex() {}

} // namespace Utils
} // namespace Core
} // namespace Ra
