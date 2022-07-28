#include "SynchronizableObject.hpp"

#include <mutex>
#include <set>

namespace Ra {
namespace Engine {
namespace Data {

///////////////////////////////////////////////
////            Synchronizer                ///
///////////////////////////////////////////////

namespace Synchronizer {

// Use set of object pointer to ensure there is only one reference/update
// for each Synchronizable object even if it added twice for the current frame.
// Each synchronizable object is synchronize at most once per frame with the latest available data.
static std::set<Synchronizable*> s_synchronizables;
static std::mutex s_mtx;

bool registerSynchronizable( Synchronizable* synchronizable ) {
    // cannot add synchronizable object if the synchronizer is running
    s_mtx.lock();
    s_synchronizables.insert( synchronizable );
    s_mtx.unlock();
    return true;
}

void start() {
    s_mtx.lock();
    for ( auto& synchronizable : s_synchronizables ) {
        synchronizable->updateGL();
    }
    s_synchronizables.clear();
    s_mtx.unlock();
}

} // namespace Synchronizer

///////////////////////////////////////////////
////           Synchronizable               ///
///////////////////////////////////////////////
void Synchronizable::needSync() {
    Synchronizer::registerSynchronizable( this );
}

} // namespace Data
} // namespace Engine
} // namespace Ra
