#ifndef RADIUMENGINE_SIGNAL_MANAGER_HPP_
#define RADIUMENGINE_SIGNAL_MANAGER_HPP_

#include <Engine/RaEngine.hpp>

#include <functional>
#include <vector>

#include <Engine/ItemModel/ItemEntry.hpp>


namespace Ra {
namespace Engine {
    class SignalManager
    {

    public:
        // Callbacks are functions which accept an item entry.
        typedef std::function<void( const ItemEntry& )> Callback;

    public:
        SignalManager() : m_isOn( true ) {}

        void fireEntityCreated      (const ItemEntry& entity ) const;
        void fireEntityDestroyed    (const ItemEntry& entity ) const;

        void fireComponentAdded     (const ItemEntry& component ) const;
        void fireComponentRemoved   (const ItemEntry& component ) const;

        void fireRenderObjectAdded  ( const ItemEntry& ro) const;
        void fireRenderObjectRemoved(const ItemEntry& ro ) const;

        void setOn( bool on )  { m_isOn = on; }

    private:
        void callFunctions( const std::vector<Callback>& funcs, const ItemEntry& arg ) const;


    public:
        bool m_isOn;

        std::vector<Callback> m_entityCreatedCallbacks;
        std::vector<Callback> m_entityDestroyedCallbacks;
        std::vector<Callback> m_componentAddedCallbacks;
        std::vector<Callback> m_componentRemovedCallbacks;
        std::vector<Callback> m_roAddedCallbacks;
        std::vector<Callback> m_roRemovedCallbacks;
    };
}}

#endif // RADIUMENGINE_SIGNAL_MANAGER_HPP_

