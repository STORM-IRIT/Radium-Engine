#ifndef RADIUMENGINE_SIGNAL_MANAGER_HPP_
#define RADIUMENGINE_SIGNAL_MANAGER_HPP_

#include <functional>
#include <vector>


namespace Ra {
namespace Engine {
    class SignalManager
    {

    public:
        // Callbacks are functions with no arguments.
        typedef std::function<void(void)> Callback;

    public:
        void fireEntityCreated() const;
        void fireEntityDestroyed() const;
        void fireComponentAdded() const;
        void fireComponentRemoved() const;

    private:
        static void callFunctions( const std::vector<Callback>& funcs );

    public:
        std::vector<Callback> m_entityCreatedCallbacks;
        std::vector<Callback> m_entityDestroyedCallbacks;
        std::vector<Callback> m_componentAddedCallbacks;
        std::vector<Callback> m_componentRemovedCallbacks;

    };
}}

#endif // RADIUMENGINE_SIGNAL_MANAGER_HPP_

