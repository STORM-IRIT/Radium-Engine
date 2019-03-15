#ifndef RADIUMENGINE_COUPLING_SYSTEM_HPP
#define RADIUMENGINE_COUPLING_SYSTEM_HPP

#include <Engine/RaEngine.hpp>      // RA_ENGINE_API
#include <Engine/System/System.hpp> // System methods declaration
#include <memory>                   // std::unique_ptr
#include <type_traits>              // std::is_base_of
#include <vector>                   // std::vector

namespace Ra {
namespace Engine {

// clang-format off
/**
 * Base class for systems coupling multiple subsystems.
 * \tparam BaseAbstractSystem Base class defining the subsystems API.
 *
 * Provides subsystem storage + dispatching methods for inheriting classes.
 * Also dispatches by default the generateTasks() and handleAssetLoading()
 * methods from Ra::Engine::System.
 *
 * \note Ra::Engine::Component registration methods from Ra::Engine::System
 *       are not dispatched by default, Ra::Engine::Systems managing only their own
 *       Ra::Engine::Components.
 *
 * \see CoupledTimedSystem for practical usage.
 *
 * \warning When overriding non pure virtual methods from BaseAbstractSystem, remind calling the
 *          default implementation:
 * \code
 * inline void generateTasks( Core::TaskQueue* taskQueue, const Engine::FrameInfo& frameInfo ) override
 * {
 *     dispatch( [taskQueue, &frameInfo]( const auto& s ) {
 *     s->generateTasks( taskQueue, frameInfo );
 *     } );
 *     ... // specific stuff
 * }
 * \endcode
 */
// clang-format on
template <typename _BaseAbstractSystem>
class BaseCouplingSystem : public _BaseAbstractSystem {
  public:
    using BaseAbstractSystem = _BaseAbstractSystem;

    inline BaseCouplingSystem() {
        static_assert( std::is_base_of<Ra::Engine::System, BaseAbstractSystem>::value,
                       "BaseAbstractSystem must inherit Ra::Core::System" );
    }

    ~BaseCouplingSystem() override = default;

    /**
     * Copy operator is forbidden.
     */
    BaseCouplingSystem( const BaseCouplingSystem<BaseAbstractSystem>& ) = delete;

    /**
     * Assignment operator is forbidden.
     */
    BaseCouplingSystem<BaseAbstractSystem>&
    operator=( const BaseCouplingSystem<BaseAbstractSystem>& ) = delete;

    inline void generateTasks( Core::TaskQueue* taskQueue,
                               const Engine::FrameInfo& frameInfo ) override {
        dispatch( [taskQueue, &frameInfo]( const auto& s ) {
            s->generateTasks( taskQueue, frameInfo );
        } );
    }

    inline void handleAssetLoading( Entity* entity, const Core::Asset::FileData* data ) override {
        BaseAbstractSystem::handleAssetLoading( entity, data );
        dispatch( [entity, data]( const auto& s ) { s->handleAssetLoading( entity, data ); } );
    }

    /**
     * Add management for the given System.
     * \warning Ownership of the System if given to the BaseCouplingSystem.
     */
    inline void addSystem( BaseAbstractSystem* s ) { m_systems.emplace_back( s ); }

  protected:
    /**
     * Call a functor on subsystems.
     * \see CoupledTimedSystem for practical usage.
     */
    template <typename Functor>
    inline void dispatch( Functor f ) {
        for ( auto& s : m_systems )
            f( s );
    }

    /**
     * Call a functor on subsystems (const version).
     * \see CoupledTimedSystem for practical usage.
     */
    template <typename Functor>
    inline void dispatch( Functor f ) const {
        for ( const auto& s : m_systems )
            f( s );
    }

    // clang-format off
    /**
     * Call a functor on subsystems, and combine return status. Loop is stopped when combined
     * status is `false`.
     * \tparam Functor Unary function object class calling the method to be dispatched on a system.
     *
     * \param f input functor.
     * \param abortWhenInvalid stop dispatching if a subsystem call returns `false`.
     *
     * \return `false` when dispatch is aborted.
     *
     * - Example 1: call `foo` until one subsystem fails, and fails if aborted
     * \code
     * bool foo() override
     * { return conditionnaldispatch([](const auto &s) { return s->foo(); });}
     * \endcode
     *
     * - Example 2: call `foo` for all subsystems, and return `true` if at least one subsystem worked
     * \code
     * bool foo() override
     * {
     *   bool status = false;
     *   conditionnaldispatch([&status](const auto &s) {
     *       status |= s->foo(); return status; }, false);
     *   return status;
     * }
     * \endcode
     *
     * \see CoupledTimedSystem for practical usage.
     */
    // clang-format on
    template <typename Functor>
    inline bool conditionnaldispatch( Functor f, bool abortWhenInvalid = true ) {
        for ( auto& s : m_systems )
        {
            if ( !f( s ) && abortWhenInvalid )
                return false;
        }
        return true;
    }

    /**
     * \see conditionnaldispatch.
     */
    template <typename Functor>
    inline bool conditionnaldispatch( Functor f, bool abortWhenInvalid = true ) const {
        for ( const auto& s : m_systems )
        {
            if ( !f( s ) && abortWhenInvalid )
                return false;
        }
        return true;
    }

  private:
    /**
     * Buffer of BaseAbstractSystem.
     */
    std::vector<std::unique_ptr<BaseAbstractSystem>> m_systems;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_COUPLING_SYSTEM_HPP
