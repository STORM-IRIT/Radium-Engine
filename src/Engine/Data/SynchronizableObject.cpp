#include "SynchronizableObject.hpp"

#include <Core/Utils/Log.hpp>

#include <mutex>
#include <set>

// Use set of object pointer to ensure there is only one reference/update for each Synchronizable
// object even if it added twice for the current frame.
// Each dirty object is cleaned at most once per frame with the latest available data.
static std::set<Synchronizable*> s_dirts;
static std::mutex s_mtx;

void Synchronizer::add( Synchronizable* dirtyable ) {
    // cannot add dirt if the washing machine is running
    s_mtx.lock();
    s_dirts.insert( dirtyable );
    s_mtx.unlock();
}

void Synchronizer::start() {
    s_mtx.lock();
    for ( auto& dirt : s_dirts ) {
        CORE_ASSERT( dirt->m_dirty == true, "Already cleaned -> inconsistent state" );
        dirt->clean();
        dirt->m_dirty = true;
    }
    //    LOG(Ra::Core::Utils::logINFO) << "Synchronizer cleaned " << s_dirts.size() << "
    //    dirtyable objects.";
    s_dirts.clear();
    s_mtx.unlock();
}

void Synchronizable::setDirty() {
    m_dirty = true;
    Synchronizer::add( this );
}
