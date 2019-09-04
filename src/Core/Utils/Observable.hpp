#pragma once

#include <Core/Utils/Log.hpp>

#include <functional>
#include <map>

namespace Ra {
namespace Core {
namespace Utils {

/// Simple observable implementation with void observer() notification
template <typename... Args>
class Observable
{
  public:
    /// Observer functor type
    using Observer = std::function<void( Args... )>;
    Observable()   = default;

    /// attach an \p observer that will be call on subsecant notify
    int attach( Observer observer ) {
        m_observers.insert( std::make_pair( ++m_currentId, observer ) );
        return m_currentId;
    }

    /// notify (call) each attached observer
    void notify( Args... p ) {
        LOG( logDEBUG ) << "notify has " << m_observers.size() << " observers";
        for ( const auto& o : m_observers )
            o.second( std::forward<Args>( p )... );
    }

    /// detach all observers
    void detachAll() { m_observers.clear(); }

    /// detach the \p observerId, observerId must have been saved from a
    /// previous call to attach

    void detach( int id ) { m_observers.erase( id ); }

  private:
    std::map<int, Observer> m_observers;

    int m_currentId{0};
};
} // namespace Utils
} // namespace Core
} // namespace Ra
