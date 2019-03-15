#ifndef RADIUMENGINE_SIGNAL_MANAGER_HPP_
#define RADIUMENGINE_SIGNAL_MANAGER_HPP_

#include <Engine/RaEngine.hpp>

#include <functional>
#include <mutex>
#include <vector>

#include <Engine/ItemModel/ItemEntry.hpp>

namespace Ra {
namespace Engine {
/**
 * This class is used to inform the external world of events happening in the Engine.
 * You can register a callback function which will be called any time the event happens.
 * Signals are fired when an object of the engine (Entity, Component or RenderObject)
 * is either added or removed.
 * All signals send the item entry indicating the object that was added or removed.
 *
 * Signals for new objects are fired just after they have been initialized and acquired
 * by the object owning them.
 * Signals for destroyed objects are fired just before they are removed from the object
 * owning them and destroyed
 */
class SignalManager {

  public:
    /**
     * Type for callback functions.
     */
    using Callback = std::function<void( const ItemEntry& )>;

    /**
     * Type for end-of-frame callback functions.
     */
    using EoFCallback = std::function<void( void )>;

  public:
    SignalManager() = default;

    /**
     * Call the callback functions linked to Entity creation.
     */
    void fireEntityCreated( const ItemEntry& entity ) const;

    /**
     * Call the callback functions linked to Entity deletion.
     */
    void fireEntityDestroyed( const ItemEntry& entity ) const;

    /**
     * Call the callback functions linked to Component creation.
     */
    void fireComponentAdded( const ItemEntry& component ) const;

    /**
     * Call the callback functions linked to Component deletion.
     */
    void fireComponentRemoved( const ItemEntry& component ) const;

    /**
     * Call the callback functions linked to RenderObject creation.
     */
    void fireRenderObjectAdded( const ItemEntry& ro ) const;

    /**
     * Call the callback functions linked to RenderObject deletion.
     */
    void fireRenderObjectRemoved( const ItemEntry& ro ) const;

    /**
     * Call the callback functions linked to RadiumEngine end-of-frame signal.
     */
    void fireFrameEnded() const;

    /**
     * Toggle on/off firing signals.
     */
    void setOn( bool on ) { m_isOn = on; }

  public:
    /// Whether the SignalManager fires signals.
    bool m_isOn{true};

    /// The list of callback function linked to Entity creation.
    std::vector<Callback> m_entityCreatedCallbacks;

    /// The list of callback function linked to Entity deletion.
    std::vector<Callback> m_entityDestroyedCallbacks;

    /// The list of callback function linked to Component creation.
    std::vector<Callback> m_componentAddedCallbacks;

    /// The list of callback function linked to Component deletion.
    std::vector<Callback> m_componentRemovedCallbacks;

    /// The list of callback function linked to RenderObject creation.
    std::vector<Callback> m_roAddedCallbacks;

    /// The list of callback function linked to RenderObject deletion.
    std::vector<Callback> m_roRemovedCallbacks;

    /// The list of callback function linked to Engine end-of-frame signal.
    std::vector<EoFCallback> m_frameEndCallbacks;

  private:
    /**
     * Actual callback function call.
     */
    void callFunctions( const std::vector<Callback>& funcs, const ItemEntry& arg ) const;

  private:
    /// Mutex to guard calling callback functions against thread concurrency.
    mutable std::mutex m_mutex;
};
} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_SIGNAL_MANAGER_HPP_
