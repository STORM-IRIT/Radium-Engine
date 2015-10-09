#include <Core/Index/CircularIndex.hpp>

namespace Ra {
namespace Core {

/// CONSTRUCTOR
CircularIndex::CircularIndex( const uint size,
                              const int  i     ) : N( size ) { setValue( i ); }
CircularIndex::CircularIndex( const CircularIndex& index ) : N( index.N ), idx( index.idx ) { }

/// DESTRUCTOR
CircularIndex::~CircularIndex() { }


} // namespace Core
} // namespace Ra

