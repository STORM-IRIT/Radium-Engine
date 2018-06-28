#ifndef RADIUMENGINE_COUPLING_SYSTEM_HPP
#define RADIUMENGINE_COUPLING_SYSTEM_HPP

#include <set>

#include <Engine/System/System.hpp>

namespace Ra {
namespace Engine {

/// Timed Systems are systems which can be disabled or paused, e.g. animation systems.
class RA_ENGINE_API TimedSystem : public System {
  public:
    TimedSystem() {}
    virtual ~TimedSystem() {}

    /// Call this to enable / disable the system according to \p on.
    virtual void play( bool on ) = 0;

    /// Call this to enable the system for only one frame.
    virtual void step() = 0;

    /// Call this to reset the system, and its Components, to their first frame state.
    virtual void reset() = 0;

    /// Saves all the state data related to the \p frameID -th frame into a cache file.
    virtual void cacheFrame( int frameID ) const = 0;

    /// Restores the state data related to the \p frameID -th frame from the cache file.
    /// \returns true if the frame has been successfully restored, false otherwise.
    virtual bool restoreFrame( int frameID ) = 0;
};

/// Coupling Systems are responsible for transmitting calls to a bunch of other Systems.
template <typename CSystem>
class CouplingSystem : public System {
  public:
    CouplingSystem() {}
    virtual ~CouplingSystem() {}

    /// Add management for the given system.
    void addSystem( CSystem* s ) { m_systems.insert( s ); }

    /// Remove management for the given system.
    void removeSystem( CSystem* s ) { m_systems.erase( s ); }

  protected:
    /// Bunch of TimedSystems.
    std::set<CSystem*> m_systems;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_COUPLING_SYSTEM_HPP
