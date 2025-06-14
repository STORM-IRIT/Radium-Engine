#pragma once
#include <Core/RaCore.hpp>

#include <any>
#include <typeindex>

namespace Ra {
namespace Core {

/// \brief Base class for dynamically configurable visitors
/// Users can implement this interface to build custom visitors without any restriction.
/// To ease visitor configuration, see class DynamicVisitor
/// \see DynamicVisitor
class DynamicVisitorBase
{
  public:
    virtual ~DynamicVisitorBase() = default;

    /** \brief Execute a visiting operator on accepted types.
     *
     * Visiting the association is done by calling the visit operator
     * associated with the underlying type of the variable \b in.
     *
     * \param name Varible name in the visited variable set.
     * \param in The variable value to process
     * \param userParam The optional user define parameter to forward to type associated
     * functor.
     */
    virtual void
    operator()( const std::string& name, std::any&& in, std::any&& userParam ) const = 0;

    /// \brief Acceptance function for the visitor
    /// \param id the std::type_index associated to the type to visit
    /// \return true if the type is visitable, false if not
    [[nodiscard]] virtual bool accept( const std::type_index& id ) const = 0;
};
} // namespace Core
} // namespace Ra
