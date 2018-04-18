#include <Core/Index/CircularIndex.hpp>

namespace Ra {
namespace Core {

/// CONSTRUCTOR
CircularIndex::CircularIndex( const uint size, const int i ) : N( size ) {
    setValue( i );
}
CircularIndex::CircularIndex( const CircularIndex& index ) : idx( index.idx ), N( index.N ) {}

/// DESTRUCTOR
CircularIndex::~CircularIndex() {}

} // namespace Core
} // namespace Ra
