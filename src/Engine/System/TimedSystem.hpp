#ifndef RADIUMENGINE_TIMED_SYSTEM_HPP
#define RADIUMENGINE_TIMED_SYSTEM_HPP

#include <Engine/RaEngine.hpp>              // RA_ENGINE_API
#include <Engine/System/CouplingSystem.hpp> // BaseCouplingSystem

namespace Ra {
namespace Engine {

/// Timed Systems are systems which are time dependent, e.g. animation systems.
class RA_ENGINE_API AbstractTimedSystem : public System
{
  public:
    AbstractTimedSystem()           = default;
    ~AbstractTimedSystem() override = default;

    /// Call this for the system to advance/rewind to time \p t.
    virtual void goTo( Scalar t ) = 0;

    /// Saves all the state data related to the \p frameID -th frame into a cache file.
    virtual void cacheFrame( const std::string& dir, uint frameID ) const = 0;

    /// Restores the state data related to the \p frameID -th frame from the cache file.
    /// \returns true if the frame has been successfully restored, false otherwise.
    virtual bool restoreFrame( const std::string& dir, uint frameID ) = 0;
};

/**
 * The TimeSystem is the System managing time.
 * It dispatches time-related events to its AbstractTimedSystem.
 * Time is managed in a time window define by two time values, e.g. `start` and `end` time.
 * When time flow is enabled, time flows between consecutive frames according to
 * a given time delta, going from the `start` time to the `end` time.
 * Once the `end` time is reached, time flows back from the `start` time.
 * In case `ping-pong` mode is enabled, time flows back and forth between the
 * `start` time and the `end` time.
 * \todo Implement time flow policy as ONCE, REPLAY, PINGPONG, FOREVER...
 */
class RA_ENGINE_API TimeSystem : public BaseCouplingSystem<AbstractTimedSystem>
{
  public:
    /**
     * Builds a TimeSystem.
     * \param dt time delta between two frames.
     * \param endTime end time for the time window.
     */
    TimeSystem( Scalar dt, Scalar endTime );

    /// \name AbstractTimedSystem Management
    /// {

    /**
     * Updates the current time, send it to the AbstractTimedSystem and then
     * calls generateTasks() on them.
     */
    void generateTasks( Ra::Core::TaskQueue* taskQueue,
                        const Ra::Engine::FrameInfo& frameInfo ) override;

    /**
     * Caches the current frame.
     */
    void cacheFrame( const std::string& dir ) const;

    /**
     * Dispatches cacheFrame to the AbstractTimedSystem.
     */
    void cacheFrame( const std::string& dir, uint frameID ) const override;

    /**
     * Dispatches restoreFrame to the AbstractTimedSystem.
     * In case one of them could not restore to frame \p frameID, then
     * rewound the restoration to the current frame.
     * \return true if all AbstractTimedSystem could restore to frame \p
     *         frameID, false otherwise.
     */
    bool restoreFrame( const std::string& dir, uint frameID ) override;

    /**
     * Dispatches time modification to the AbstractTimedSystem.
     */
    void goTo( Scalar t ) override;
    /// \}

    /// \name Time Management.
    /// {

    /**
     * Toggles on/off time flow.
     */
    void play( bool isPlaying );

    /**
     * Toggles time flow for one frame.
     */
    void step();

    /**
     * Resets time to 0, same as \code goTo( uint( 0 ) ); \endCode.
     */
    void reset();

    /**
     * Sets the `start` time for the time window.
     */
    void setStartTime( Scalar t );

    /**
     * Sets the `start` time for the time window.
     */
    Scalar getStartTime() const;

    /**
     * Sets the `end` time for the time window.
     */
    void setEndTime( Scalar t );

    /**
     * \Returns the `end` time for the time window.
     */
    Scalar getEndTime() const;

    /**
     * Toggles the PingPong mode.
     */
    void setPingPongMode( bool mode );

    /**
     * \returns the current time.
     */
    Scalar getTime() const;

    /**
     * \returns the current frame.
     */
    uint getFrame() const;
    /// \}

  private:
    /// \name Time Management.
    /// {

    /**
     * Update the current time.
     */
    void updateTime();
    /// \}

  private:
    Scalar m_dt;                ///< The time delta between 2 consecutive frames.
    Scalar m_startTime{0_ra};   ///< The `start` time for the time window.
    Scalar m_endTime;           ///< The `end` time for the time window.
    Scalar m_time{0_ra};        ///< The current time.
    uint m_frame{0};            ///< The current play frame, w.r.t. the PingPong mode.
    bool m_play{false};         ///< Shall time flow.
    bool m_step{true};          ///< Shall time flow for only one frame.
    bool m_pingPongMode{false}; ///< Is PingPong mode enabled.
    bool m_lockFrame{false};    ///< lock on m_frame for internal calls to goTo.
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_TIMED_SYSTEM_HPP
