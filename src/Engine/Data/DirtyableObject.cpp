#include "DirtyableObject.hpp"

#include <Core/Utils/Log.hpp>

#include <mutex>
#include <set>

// Use set of object pointer to ensure there is only one reference/update for each Dirtyable
// object even if it added twice for the current frame.
// Each dirty object is cleaned at most once per frame with the latest available data.
static std::set<Dirtyable*> s_dirts;
static std::mutex s_mtx;

void WashingMachine::add( Dirtyable* dirtyable ) {
    // cannot add dirt if the washing machine is running
    s_mtx.lock();
    s_dirts.insert( dirtyable );
    s_mtx.unlock();
}

void WashingMachine::start() {
    s_mtx.lock();
    for ( auto& dirt : s_dirts ) {
        CORE_ASSERT( dirt->m_dirty == true, "Already cleaned -> inconsistent state" );
        dirt->clean();
        dirt->m_dirty = true;
    }
    //    LOG(Ra::Core::Utils::logINFO) << "WashingMachine cleaned " << s_dirts.size() << "
    //    dirtyable objects.";
    s_dirts.clear();
    s_mtx.unlock();
}

void Dirtyable::setDirty() {
    m_dirty = true;
    WashingMachine::add( this );
}
