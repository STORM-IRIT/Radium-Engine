#include "SynchronizableObject.hpp"

//#include <Core/Utils/Log.hpp>

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
// for each synchronizable object even if it added twice for the current frame.
// Each synchronizable object is synchronized at most once per frame with the latest available data.
static std::set<Synchronizable*> s_synchronizables;
static std::mutex s_mtx;

bool registerSynchronizable( Synchronizable* synchronizable ) {
    // cannot add synchronizable object if the synchronizer is running
    s_mtx.lock();
    s_synchronizables.insert( synchronizable );
    //    LOG( Core::Utils::logINFO ) << "Synchronizer::registerSynchronizable(" << synchronizable
    //    << ")";
    s_mtx.unlock();
    return true;
}

void start() {
    s_mtx.lock();
    for ( auto& synchronizable : s_synchronizables ) {
        synchronizable->updateGL();
    }
    //    LOG( Core::Utils::logINFO ) << "Synchronizer::start : update " << s_synchronizables.size()
    //    << " synchronizables.";
    s_synchronizables.clear();
    s_mtx.unlock();
}

} // namespace Synchronizer

///////////////////////////////////////////////
////           Synchronizable               ///
///////////////////////////////////////////////

// The destruction by the client of a synchronizable object after having
// made an update of the cpu data and before entering the synchronization
// loop (Synchronizer::start) would imply a pointer of a dead object.
// A task system would have the same behavior because the object no longer
// exists to update it.
// The only way to avoid this is to remove the dead object from the
// synchronization list before doing the synchronization.
Synchronizable::~Synchronizable() {
    Synchronizer::s_mtx.lock();
    Synchronizer::s_synchronizables.erase( this );
    Synchronizer::s_mtx.unlock();
    CORE_ASSERT( Synchronizer::s_synchronizables.find( this ) ==
                     Synchronizer::s_synchronizables.end(),
                 "Cannot synchronize dead ptr" );
}

void Synchronizable::needSync() {
    Synchronizer::registerSynchronizable( this );
}

} // namespace Data
} // namespace Engine
} // namespace Ra
