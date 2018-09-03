#ifndef RADIUMENGINE_SIGNAL_MANAGER_HPP_
#define RADIUMENGINE_SIGNAL_MANAGER_HPP_

#include <Engine/RaEngine.hpp>

#include <functional>
#include <mutex>
#include <vector>

#include <Engine/ItemModel/ItemEntry.hpp>

namespace Ra {
namespace Engine {
/** This class is used to inform the external world of events happening in the engine.
 * You can register a callback function which will be called any time the event happens.
 * Signals are fired when an object of the engine (entity, component or render object)
 * is either added or removed.
 * All signals sen the item entry indicating the object that was added or removed.
 *
 * Signals for new objects are fired just after they have been initialized and acquired
 * by the object owning them.
 * Signals for destroyed objects are fired just before they are removed from the object
 * owning them and destroyed
 */
class SignalManager {

  public:
    // Callbacks are functions which accept an item entry.
    using Callback = std::function<void( const ItemEntry& )>;
    using EoFCallback = std::function<void( void )>;

  public:
    SignalManager() : m_isOn( true ) {}

    /// Call the callback functions linked to Entity creation.
    void fireEntityCreated( const ItemEntry& entity ) const;

    /// Call the callback functions linked to Entity deletion.
    void fireEntityDestroyed( const ItemEntry& entity ) const;

    /// Call the callback functions linked to Component creation.
    void fireComponentAdded( const ItemEntry& component ) const;

    /// Call the callback functions linked to Component deletion.
    void fireComponentRemoved( const ItemEntry& component ) const;

    /// Call the callback functions linked to RenderObject creation.
    void fireRenderObjectAdded( const ItemEntry& ro ) const;

    /// Call the callback functions linked to RenderObject deletion.
    void fireRenderObjectRemoved( const ItemEntry& ro ) const;

    /// Call the callback functions linked to Engine frame processed.
    void fireFrameEnded() const;

    /// Toggle on/off firing signals.
    void setOn( bool on ) { m_isOn = on; }

  private:
    /// Actual callback function call.
    void callFunctions( const std::vector<Callback>& funcs, const ItemEntry& arg ) const;

    /// Mutex to guard calling callback functions against thread concurrency.
    mutable std::mutex m_mutex;

  public:
    /// Whether the SignalManager fires signals.
    bool m_isOn;

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

    /// The list of callback function linked to Engine frame processed.
    std::vector<EoFCallback> m_frameEndCallbacks;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_SIGNAL_MANAGER_HPP_
