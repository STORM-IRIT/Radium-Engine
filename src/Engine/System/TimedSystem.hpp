#ifndef RADIUMENGINE_TIMED_SYSTEM_HPP
#define RADIUMENGINE_TIMED_SYSTEM_HPP

#include <Engine/RaEngine.hpp>              // RA_ENGINE_API
#include <Engine/System/CouplingSystem.hpp> // BaseCouplingSystem

namespace Ra::Engine {

/// Timed Systems are systems which can be disabled or paused, e.g. animation systems.
class RA_ENGINE_API AbstractTimedSystem : public System
{
  public:
    AbstractTimedSystem()           = default;
    ~AbstractTimedSystem() override = default;

    /// Call this to enable / disable the system according to \p on.
    virtual void play( bool on ) = 0;

    /// Call this to enable the system for only one frame.
    virtual void step() = 0;

    /// Call this to reset the system, and its Components, to their first frame state.
    virtual void reset() = 0;

    /// Saves all the state data related to the \p frameID -th frame into a cache file.
    virtual void cacheFrame( const std::string& dir, uint frameID ) const = 0;

    /// Restores the state data related to the \p frameID -th frame from the cache file.
    /// \returns true if the frame has been successfully restored, false otherwise.
    virtual bool restoreFrame( const std::string& dir, uint frameID ) = 0;
};

class RA_ENGINE_API CoupledTimedSystem : public BaseCouplingSystem<AbstractTimedSystem>
{
  public:
    void play( bool on ) override {
        dispatch( [on]( const auto& s ) { s->play( on ); } );
    }
    void step() override {
        dispatch( []( const auto& s ) { s->step(); } );
    }
    void reset() override {
        dispatch( []( const auto& s ) { s->reset(); } );
    }
    void cacheFrame( const std::string& dir, uint frameID ) const override {
        dispatch( [&dir, frameID]( const auto& s ) { s->cacheFrame( dir, frameID ); } );
    }
    bool restoreFrame( const std::string& dir, uint frameID ) override {
        return conditionaldispatch(
            [&dir, frameID]( const auto& s ) { return s->restoreFrame( dir, frameID ); } );
    }
};

} // namespace Ra::Engine

#endif // RADIUMENGINE_TIMED_SYSTEM_HPP
