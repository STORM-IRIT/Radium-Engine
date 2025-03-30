#pragma once
#include <Core/RaCore.hpp>

#include <Core/Containers/DynamicVisitorBase.hpp>
#include <Core/Containers/VariableSet.hpp>
#include <Core/Utils/Log.hpp>
#include <Core/Utils/StdExperimentalTypeTraits.hpp>
#include <Core/Utils/StdOptional.hpp>
#include <Core/Utils/TypesUtils.hpp>

#include <any>
#include <functional>
#include <map>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace Ra {
namespace Core {

/// \brief Base class for visitors with configurable per-type callbacks.
/// Visiting will be prepared at running time by dynamically adding visitor operators for each
/// type one want to visit in the container.
/// The visitor accept type based on either the added operators or an external acceptance
/// functor which can be associated/changed at runtime. This functor is only allowed to reduce
/// the number of visited type as only type for which a visitor operator is registered can be
/// visited.
/// The visitor operators should be any callable that accept to be called using
/// f(const std::string&, T&[, std::any&&])
class RA_CORE_API DynamicVisitor : public DynamicVisitorBase
{
  public:
    /// allows the class to be derived
    ~DynamicVisitor() override = default;

    /// \brief Execute a visiting operator on accepted types
    /// \param in The variable to process
    /// \param userParam The optional user defined parameter to forward to type associated
    /// functor The variable \b in contains a wrapping of the association name->value whose
    /// visit is accepted. Visiting the association is done by calling the visit operator
    /// associated with the underlying type of the variable \b in.
    void operator()( std::any&& in, std::any&& userParam ) const override;

    /// \brief Acceptance function for the visitor
    /// \param id The type to test
    /// \return true if the type is visitable, false if not
    [[nodiscard]] bool accept( const std::type_index& id ) const override;

    /// \brief Add a visiting operator.
    /// \tparam T The accepted type for the visit.
    /// \tparam F The operator type, a callable with profile void(const std::string&, [const
    /// ]T[&], std::any&&).
    /// \param f The operator object.
    /// \return true if the operator was added, false if not (e.g. there is already an operator
    /// associated with the type T)
    template <typename T, typename F>
    bool addOperator( F&& f );

    /// \brief Test the existence of an operator associated with a type
    /// \tparam T
    /// \return
    template <typename T>
    bool hasOperator();

    /// \brief Add or replace a visiting operator
    /// \tparam T
    /// \tparam F
    /// \param f
    /// This method is similar to DynamicVisitor::addOperator but replace any existing operator
    /// previously associated with the type T
    template <typename T, typename F>
    void addOrReplaceOperator( F&& f );

    /// \brief Remove a visiting operator
    /// \tparam T
    /// \return
    /// \pre The type \b T must be accepted by an operator.
    template <typename T>
    bool removeOperator();

  private:
    /// \brief Type of the callback function that type-erased the user registered operators
    using CallbackFunction = std::function<void( std::any&, std::any&& )>;

    /// \brief Storage of the type erased operators
    using OperatorsStorageType = std::unordered_map<std::type_index, CallbackFunction>;

    /// \brief Helper struct to build type-erased operators (allowing to test calling profile)
    template <typename T, typename F, bool WithUserParam>
    struct MakeVisitOperatorHelper {
        auto makeOperator( F& f ) -> OperatorsStorageType::value_type;
    };

    /// \brief construct a type-erased operator from a user define functor with profile
    /// void(const std::string, [const]T[&] [, [const] std::any&&])
    template <typename T, typename F>
    auto makeVisitorOperator( F& f ) -> OperatorsStorageType::value_type;

    /// \brief The type erased operators
    OperatorsStorageType m_visitorOperator;
};

template <typename T, typename F>
bool DynamicVisitor::addOperator( F&& f ) {
    auto [it, inserted] = m_visitorOperator.insert( makeVisitorOperator<T, F>( f ) );
    return inserted;
}

template <typename T>
bool DynamicVisitor::hasOperator() {
    return m_visitorOperator.find( VariableSet::getVariableVisitTypeIndex<T>() ) !=
           m_visitorOperator.end();
}

template <typename T, typename F>
void DynamicVisitor::addOrReplaceOperator( F&& f ) {
    auto op = makeVisitorOperator<T, F>( f );
    m_visitorOperator.insert_or_assign( op.first, op.second );
}

template <typename T>
bool DynamicVisitor::removeOperator() {
    assert( hasOperator<T>() );
    auto res = m_visitorOperator.erase( VariableSet::getVariableVisitTypeIndex<T>() ) > 0;
    return res;
}

template <typename T, typename F>
struct DynamicVisitor::MakeVisitOperatorHelper<T, F, true> {
    inline auto makeOperator( F& f ) -> OperatorsStorageType::value_type {
        return {
            VariableSet::getVariableVisitTypeIndex<T>(), [&f]( std::any& a, std::any&& userParam ) {
                auto rp = std::any_cast<std::reference_wrapper<VariableSet::Variable<T>>&>( a );
                auto& p = rp.get();
                f( p.first, p.second, std::forward<std::any>( userParam ) );
            } };
    }
};

template <typename T, typename F>
struct DynamicVisitor::MakeVisitOperatorHelper<T, F, false> {
    inline auto makeOperator( F& f ) -> OperatorsStorageType::value_type {
        return { VariableSet::getVariableVisitTypeIndex<T>(), [&f]( std::any& a, std::any&& ) {
                    auto rp = std::any_cast<std::reference_wrapper<VariableSet::Variable<T>>&>( a );
                    auto& p = rp.get();
                    f( p.first, p.second );
                } };
    }
};

template <class T, class F>
inline auto DynamicVisitor::makeVisitorOperator( F& f ) -> OperatorsStorageType::value_type {
    auto opBuilder = MakeVisitOperatorHelper < T, F,
         std::is_invocable<F, const std::string&, T, std::any&&>::value ||
             std::is_invocable<F, const std::string&, T&, std::any&&>::value > {};
    return opBuilder.makeOperator( f );
}

} // namespace Core
} // namespace Ra
