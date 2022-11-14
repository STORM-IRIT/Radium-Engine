#pragma once
#include <Core/RaCore.hpp>

#include <Core/Utils/StdExperimentalTypeTraits.hpp>

#include <any>
#include <assert.h>
#include <functional>
#include <map>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace Ra {
namespace Core {

/// \brief Heterogeneous container storing "Variables", that maps a name (std::string) to a value
/// (of any type T).
///
/// The container is compatible with any type, from simple arithmetic type to wrapped references to
/// any other type.
///
/// "Variables" can be added, accessed and removed from the container given their type, name and
/// value.
///
/// Given a type, the container gives access to the mapping name->value for all variables with the
/// given type.
///
/// Stored variables can be referenced using a VariableHandle returned when a variable is inserted
/// in the container or fetch giving the type and the name of the variable. Validity of the variable
/// handle follows the same rules than the std::iterator attached to the underlying mapping storage
/// (std::map<std::string, T>::iterator for the actual implementation).
///
/// From a variable handle (possibly invalid), the mapping name->value for all variables with the
/// same type can be fetched. This allows generic, type agnostic, usage of the container as soon as
/// a variable handle (even invalid) is available.
///
/// The container could be visited to apply processing, transformation, ... on all or part of the
/// variables. The accept functions of the visitor are defined by the availability of a callable
/// object whose profile is compatible with the visited types.
///
/// Visiting the container can be made
///  - using a statically typed visitor : accepted types are defined at compile time,
/// as template parameters, and the visit is pre-processed by the compiler.
///  - using a dynamically configurable visitor where functor accepting types can be added/removed
/// at runtime. This kind of visit is a little more expensive while being more configurable.
///
/// \note relies on C++ standard libraries extensions, version 2 or recommended implementation on
/// compilers that do not provide this extension
/// https://en.cppreference.com/w/cpp/experimental/lib_extensions_2
///
/// \see VariableSet unit test source code for examples of usage of this container
class RA_CORE_API VariableSet
{
  public:
    /// \brief Container type for the mapping name->value of variables with type T.
    template <typename T>
    using BaseContainer = std::map<std::string, T>;

    /// \brief Handle of a variable
    /// A handle on a variable with type T is an iterator into the BaseContainer.
    /// De-referencing the handle give access to a non const pair
    /// <const std::string, T> (BaseContainer<T>::value_type).
    /// VariableHandle validity follows the rules of BaseContainer<T>::iterator validity.
    template <typename T>
    using VariableHandle = typename BaseContainer<T>::iterator;

    /// \brief Type of the variable referenced by a VariableHandle
    template <typename H>
    using HandledType = typename std::iterator_traits<H>::value_type::second_type;

    /// \brief CRTP based Type list for statically typed visitors.
    template <class...>
    struct TypeList {};

    /// \brief Base class for visitors with static supported types.
    /// Visiting will be prepared at compile time by unfolding type list and generating all
    /// the required function calls for the visit.
    /// Any class that defines the same alias for a public member "types" can be used as a visitor.
    template <class... TYPES>
    struct StaticVisitor {
        using types = TypeList<TYPES...>;
    };

    // Pre declare base class for all dynamic visitors.
    class DynamicVisitor;

    // ----------------------------------------------------------
    /// Constructors, destructors
    /// \{
    VariableSet() = default;
    ~VariableSet() { clear(); }
    /// A VariableSet is copyable
    VariableSet( const VariableSet& other ) { *this = other; }
    /// A VariableSet is movable
    VariableSet( VariableSet&& other ) { *this = std::move( other ); }
    /// \}

    // ------------------------------------------------------------------------------------------
    // Global variable set operations
    // ------------------------------------------------------------------------------------------
    /// Operators acting on a the whole VariableSet
    /// \{
    /// \brief Copy assignment operator
    VariableSet& operator=( const VariableSet& other );

    /// \brief Move assignment operator
    VariableSet& operator=( VariableSet&& other );

    /// \brief remove all elements from the container
    void clear();

    /// \brief Merge the VariableSet \b from into this
    /// \param from the VariableSet to merge with the current.
    /// Existing variable into this, with same name as in from, are kept unchanged
    /// \see mergeReplaceVariables
    void mergeKeepVariables( const VariableSet& from );

    /// \brief Merge the VariableSet \b from into this
    /// \param from the VariableSet to merge with the current.
    /// Existing variable into this, with same name as in from, are replaced by from's one.
    /// \see mergeKeepVariables
    void mergeReplaceVariables( const VariableSet& from );

    /// \brief Gets the total number of variables (of any type)
    size_t size() const;

    /// \brief Gets the stored data type
    /// \return constant vector of type_index
    const std::vector<std::type_index>& getStoredTypes() const { return m_storedType; }

    /// \}
    // ------------------------------------------------------------------------------------------
    // Per variable operations
    // ------------------------------------------------------------------------------------------
    /// Operators acting on a per variable basis
    /// \{
    /// \brief Add a variable, i.e. an association name->value, into the container
    /// \return true if the variable is inserted, false if the name was already associated with
    /// another value (of the same type). In this case, keep the old value.
    ///
    template <typename T>
    std::pair<VariableHandle<T>, bool> insertVariable( const std::string& name, const T& value );

    /// \brief get the value of the given variable
    /// \return a const reference to the value.
    /// \pre The element \b name must exists with type \b T.
    template <typename T>
    const T& getVariable( const std::string& name ) const;

    /// \brief get the handle on the variable with the given name
    /// \tparam T the type of the variable
    /// \param name the name of a variable
    /// \return an handle which can be de-referenced to obtain a std::pair<const std::string, T>
    /// representing the name and the value of the variable.
    template <typename T>
    VariableHandle<T> getVariableHandle( const std::string& name ) const;

    /// \brief Test the validity of a handle
    /// \tparam H Type of the handle. Expected to be VariableHandle<T> for some variable type T
    /// \param handle the variable handle
    /// \return true if the handle is valid, false if not.
    template <typename H>
    bool isHandleValid( const H& handle ) const;

    /// \brief reset (or set if the variable does not exist yet) the value of the variable.
    /// \return true if the value was reset, false it was set.
    template <typename T>
    std::pair<VariableHandle<T>, bool> insertOrAssignVariable( const std::string& name,
                                                               const T& value );

    /// \brief Remove a variable, i.e. a name->value association
    /// \return true if the variable was removed, false if
    /// \pre The element \b name must exists with type \b T.
    template <typename T>
    bool deleteVariable( const std::string& name );

    /// \brief delete a variable from its handle
    /// \tparam H Type of the handle. Expected to be VariableHandle<T> for some variable type T
    /// \param handle the variable handle
    /// \return true variable was removed, false if not.
    /// If the variable was removed, handle is invalidated
    /// \pre the handle must be valid
    template <typename H>
    bool deleteVariable( H& handle );

    /// \brief test the existence of the given variable
    /// \return true if a variable with the given name and type exists in the storage, false if not.
    template <typename T>
    bool existsVariable( const std::string& name ) const;

    /// \}

    // ------------------------------------------------------------------------------------------
    // Per type access
    // ------------------------------------------------------------------------------------------
    /// Operators acting on a per type basis
    /// \{
  public:
    /// \brief Test if the storage supports a given variable type
    /// \tparam T The type of variable to test
    /// \return true if the type is managed by the storage
    template <typename T>
    bool existsVariableType() const;

    /// \brief Removes all variables of the given type.
    /// \tparam T The type of variable to remove
    /// \return true if the type is deleted, false if it was not managed before the call.
    /// Unregister all the functions associated with the type and remove the storage of
    /// existing variables. If several variables are still stored, they will be destroyed.
    template <typename T>
    bool deleteAllVariables();

    /// \brief Get the whole container for variables of a given type
    /// \tparam T The variable type to get
    /// \return a reference to the storage of the mapping name->value for the given type.
    /// \pre existsVariableType<T>()
    template <typename T>
    BaseContainer<T>& getAllVariables();

    /// \brief Get the whole container for variables of the same type than the given handled
    /// variable. \tparam H Type of the variable handle, should be VariableHandle<T> for some type T
    /// \param handle the handle to an existing variable
    /// \return a reference to the storage of the mapping name->value for the given type.
    /// \pre existsVariableType<HandledType<H>>()
    /// variable.
    template <typename H>
    auto getAllVariablesFromHandle( const H& handle ) -> BaseContainer<HandledType<H>>&;

    /// \brief Get the number of variables of the given type
    /// \tparam T The type to test
    /// \return the number of variables with type T stored in the container
    template <typename T>
    size_t numberOf() const;
    /// \}

    /// Visiting operators
    /// \{

    /// \brief Visit the container using a dynamically typed visitor
    /// \tparam F The type of the visitor to use (see below)
    /// \param visitor The visitor to use
    ///
    /// This visiting method is adapted when the types to visit are only known at running time.
    /// At running time, this visiting approach relies on two loops.
    ///   - The first loop, done by the visiting logic in the class VariableSet iterate over ALL
    ///     the stored mapping name->value to build a type-safe container for single values.
    ///   - The second loop, done by the visitor, loop over visiting operators to find how to
    ///     process the given type-safe container (https://en.cppreference.com/w/cpp/utility/any)
    /// This visitor then iterate over all the managed type, and, for each of them, over all stored
    /// variables then search for a compatible visiting operator.
    ///
    /// The type of the visiting functor F should be
    ///   - either derived from VariableSet::DynamicVisitor with the needed visiting operators
    ///     registered (\see see VariableSet::DynamicVisitor)
    ///   - either a full user define callable
    ///     (\see https://en.cppreference.com/w/cpp/named_req/Callable) object with profile
    ///     void op(std::any&& param). In this case, the given std::any rvalue reference wraps a
    ///     reference to a std::pair<const std::string, T> such that the first element of the pair
    ///     is the name of the variable, and the second its value of type T.
    template <typename F>
    void visitDynamic( F&& visitor ) const;

    /// \brief Visit the container using a statically typed visitor
    /// \tparam F The type of the visitor to use (see below)
    /// \param visitor The visitor object to use
    ///
    /// This visiting method is well adapted when the visited types are known at compile time.
    /// In this case, part of the visit logic is done at compile time by unfolding the call of the
    /// type-related visitors instead of looping over all the stored type.
    /// This visiting strategy loops on visited types at compile time (loop unrolled) then
    /// on each stored values at running time. So, this visiting strategy is more efficient than
    /// the dynamic one if visited types are always the same.
    ///
    /// The type of the visiting functor F should be
    ///     - either derived from VariableSet::StaticVisitor<Type1, Type2, ...> with function
    ///     operators with profile const operator(const std::string& name, T& value) available for
    ///     all the requested types Type1, Type2, ...
    ///     - either a user define class exposing a type list to unfold F::types
    ///     (see VariableSet::TypeList and VariableSet::StaticVisitor)
    template <typename F>
    void visit( F&& visitor ) const;

    /// \brief Base class for visitors with dynamic supported types.
    /// Visiting will be prepared at running time by dynamically adding visitor operators for each
    /// type one want to visit in the container. The visitor operators should be any callable that
    /// accept to be called using f(const std::string&, T&)
    class RA_CORE_API DynamicVisitor
    {
      public:
        /// allows the class to be derived
        virtual ~DynamicVisitor() = default;

        /// \brief Execute a visiting operator on accepted types
        /// \param in The variable to process
        /// The variable in contains a wrapping of the association name->value whose visit is
        /// accepted if there exists an operator for this type in the Visitor. Visiting the
        /// association is done by calling this operator.
        virtual void operator()( std::any&& in ) const;

        /// \brief Add a visiting operator.
        /// \tparam T The accepted type for the visit.
        /// \tparam F The operator type, a callable with profile void(const std::string&, [const
        /// ]T[&]).
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
        template <typename T>
        using VisitedType = typename VariableSet::BaseContainer<T>::value_type;

        template <typename T>
        auto getTypeIndex() -> std::type_index;

        template <class T, class F>
        inline std::pair<const std::type_index, std::function<void( std::any& )>>
        makeVisitorOperator( F& f );

        using OperatorsStorageType =
            std::unordered_map<std::type_index, std::function<void( std::any& )>>;

        OperatorsStorageType m_visitorOperator;
    };

  private:
  private:
    /// \brief Add support for a given type.
    /// \tparam T The type to manage
    /// \return true if the type was correctly inserted
    /// This method create functions for destroying, copying, inspecting and visiting variables
    /// of the given type.
    template <typename T>
    void addVariableType();

    /// The core of a container logic : a template member variable
    template <typename T>
    static std::unordered_map<const VariableSet*, BaseContainer<T>> m_variables;

    /// \brief test if the type F has a callable operator with the right profile
    template <typename F, typename T>
    using VisitFunction = decltype(
        std::declval<F>().operator()( std::declval<const std::string&>(), std::declval<T&>() ) );

    /// Implementation of static visitor
    /// \{
    template <typename F, typename T>
    static constexpr bool has_visit_v = Ra::Core::Utils::is_detected<VisitFunction, F, T>::value;

    template <typename F, template <typename...> typename TYPESLIST, typename... TYPES>
    void visitImpl( F&& visitor, TYPESLIST<TYPES...> ) const;

    template <typename F, typename T>
    void visitImplHelper( F& visitor ) const;
    /// \}

    /// Storage for the data management functions
    /// \{
    std::vector<std::function<void( VariableSet& )>> m_clearFunctions;
    std::vector<std::function<void( const VariableSet&, VariableSet& )>> m_copyFunctions;
    std::vector<std::function<void( VariableSet&, VariableSet& )>> m_moveFunctions;
    std::vector<std::function<void( const VariableSet&, VariableSet& )>> m_mergeKeepFunctions;
    std::vector<std::function<void( const VariableSet&, VariableSet& )>> m_mergeReplaceFunctions;
    std::vector<std::function<size_t( const VariableSet& )>> m_sizeFunctions;
    std::vector<std::function<void( const VariableSet&, DynamicVisitor& )>> m_visitFunctions;
    std::vector<std::type_index> m_storedType;
    /// \}
};

// ------------------------------------------------------------------------------------------
// Templated methods definition
// ------------------------------------------------------------------------------------------
template <typename T>
std::pair<VariableSet::VariableHandle<T>, bool>
VariableSet::insertVariable( const std::string& name, const T& value ) {
    // If it is the first parameter of the given type, first register the type
    if ( !existsVariableType<T>() ) { addVariableType<T>(); }
    // insert the parameter.
    auto inserted = m_variables<T>[this].insert( { name, value } );
    return inserted;
}

template <typename T>
const T& VariableSet::getVariable( const std::string& name ) const {
    assert( existsVariable<T>( name ) );
    auto it = getVariableHandle<T>( name );
    return it->second;
}

template <typename T>
VariableSet::VariableHandle<T> VariableSet::getVariableHandle( const std::string& name ) const {
    auto iter = m_variables<T>.find( this );
    auto it   = iter->second.find( name );
    return it;
}

template <typename H>
bool VariableSet::isHandleValid( const H& handle ) const {
    auto iter = m_variables<HandledType<H>>.find( this );
    if ( iter != m_variables<HandledType<H>>.end() ) { return handle != iter->second.end(); }
    return false;
}

template <typename T>
std::pair<VariableSet::VariableHandle<T>, bool>
VariableSet::insertOrAssignVariable( const std::string& name, const T& value ) {
    if ( !existsVariableType<T>() ) { addVariableType<T>(); }
    auto result = m_variables<T>[this].insert_or_assign( name, value );
    return result;
}

template <typename T>
bool VariableSet::deleteVariable( const std::string& name ) {
    assert( existsVariable<T>( name ) );
    auto iter    = m_variables<T>.find( this );
    auto removed = iter->second.erase( name ) > 0;

    // do we want to remove the type related function when the container has no more data of
    // this type ?
    if ( numberOf<T>() == 0 ) { deleteAllVariables<T>(); }

    return removed;
}

template <typename H>
bool VariableSet::deleteVariable( H& handle ) {
    assert( isHandleValid( handle ) );
    auto varname = handle->first;
    handle       = m_variables<HandledType<H>>[this].end();
    deleteVariable<HandledType<H>>( varname );
    return !isHandleValid( handle );
}

template <typename T>
bool VariableSet::existsVariable( const std::string& name ) const {
    auto iter = m_variables<T>.find( this );
    if ( iter != m_variables<T>.cend() ) {
        auto it = iter->second.find( name );
        return it != iter->second.cend();
    }
    return false;
}

template <typename T>
void VariableSet::addVariableType() {
    assert( !existsVariableType<T>() );
    // used to remove all stored data at deletion time
    m_clearFunctions.emplace_back( []( VariableSet& c ) { m_variables<T>.erase( &c ); } );
    // used to copy the stored data when copying the object
    m_copyFunctions.emplace_back( []( const VariableSet& from, VariableSet& to ) {
        m_variables<T>[&to] = m_variables<T>[&from];
    } );
    m_moveFunctions.emplace_back( []( VariableSet& from, VariableSet& to ) {
        m_variables<T>[&to] = std::move( m_variables<T>[&from] );
    } );
    // used to merge (keep) the stored data from container "from" to container "to"
    m_mergeKeepFunctions.emplace_back( []( const VariableSet& from, VariableSet& to ) {
        if ( !to.existsVariableType<T>() ) { to.addVariableType<T>(); }
        for ( const auto& t : m_variables<T>[&from] ) {
            m_variables<T>[&to].insert( t );
        }
    } );
    // used to merge (replace) the stored data from container "from" to container "to"
    m_mergeReplaceFunctions.emplace_back( []( const VariableSet& from, VariableSet& to ) {
        if ( !to.existsVariableType<T>() ) { to.addVariableType<T>(); }
        for ( const auto& t : m_variables<T>[&from] ) {
            m_variables<T>[&to].insert_or_assign( t.first, t.second );
        }
    } );
    // use to compute gauge on the stored data
    m_sizeFunctions.emplace_back(
        []( const VariableSet& c ) { return m_variables<T>[&c].size(); } );
    // used to call dynamically typed visitors
    m_visitFunctions.emplace_back( []( const VariableSet& c, DynamicVisitor& v ) {
        for ( auto&& t : m_variables<T>[&c] ) {
            v( { std::ref( t ) } );
        }
    } );
    // remember the stored type and its rank
    m_storedType.emplace_back( std::type_index( typeid( T ) ) );
}

template <typename T>
bool VariableSet::existsVariableType() const {
    auto iter = m_variables<T>.find( this );
    return iter != m_variables<T>.cend();
}

template <typename T>
bool VariableSet::deleteAllVariables() {
    auto iter = m_variables<T>.find( this );
    if ( iter != m_variables<T>.end() ) {
        auto tidx = std::type_index( typeid( T ) );
        auto it   = std::find( m_storedType.begin(), m_storedType.end(), tidx );
        auto idx  = it - m_storedType.begin();
        m_clearFunctions.erase( m_clearFunctions.begin() + idx );
        m_copyFunctions.erase( m_copyFunctions.begin() + idx );
        m_moveFunctions.erase( m_moveFunctions.begin() + idx );
        m_mergeKeepFunctions.erase( m_mergeKeepFunctions.begin() + idx );
        m_mergeReplaceFunctions.erase( m_mergeReplaceFunctions.begin() + idx );
        m_sizeFunctions.erase( m_sizeFunctions.begin() + idx );
        m_visitFunctions.erase( m_visitFunctions.begin() + idx );
        m_storedType.erase( it );
        m_variables<T>.erase( iter );
        return true;
    }
    return false;
}

template <typename T>
VariableSet::BaseContainer<T>& VariableSet::getAllVariables() {
    assert( existsVariableType<T>() );
    auto iter = m_variables<T>.find( this );
    return iter->second;
}

template <typename H>
auto VariableSet::getAllVariablesFromHandle( const H& ) -> BaseContainer<HandledType<H>>& {
    assert( existsVariableType<HandledType<H>>() );
    return getAllVariables<HandledType<H>>();
}

template <typename T>
size_t VariableSet::numberOf() const {
    auto iter = m_variables<T>.find( this );
    if ( iter != m_variables<T>.cend() ) { return iter->second.size(); }
    return 0;
}

template <typename F>
void VariableSet::visitDynamic( F&& visitor ) const {
    for ( auto&& visitFunc : m_visitFunctions ) {
        visitFunc( *this, visitor );
    }
}

template <typename F>
void VariableSet::visit( F&& visitor ) const {
    visitImpl( visitor, typename std::decay_t<F>::types {} );
}

template <typename F, template <typename...> typename TYPESLIST, typename... TYPES>
void VariableSet::visitImpl( F&& visitor, TYPESLIST<TYPES...> ) const {
    ( ..., visitImplHelper<std::decay_t<F>, TYPES>( visitor ) );
}

template <typename F, typename T>
void VariableSet::visitImplHelper( F& visitor ) const {
    static_assert( has_visit_v<F, T>,
                   "Static visitors must provide a function with profile "
                   "void( const std::string& name, [const ]T[&] value) for each "
                   "declared visitable type T" );
    if ( !existsVariableType<T>() ) { return; }
    for ( auto& element : m_variables<T>[this] ) {
        visitor( element.first, element.second );
    }
}

template <typename T>
std::unordered_map<const VariableSet*, std::map<std::string, T>> VariableSet::m_variables;

template <typename T, typename F>
bool VariableSet::DynamicVisitor::addOperator( F&& f ) {
    auto [it, inserted] = m_visitorOperator.insert( makeVisitorOperator<T, F>( f ) );
    return inserted;
}

template <typename T>
bool VariableSet::DynamicVisitor::hasOperator() {
    return m_visitorOperator.find( getTypeIndex<T>() ) != m_visitorOperator.end();
}

template <typename T, typename F>
void VariableSet::DynamicVisitor::addOrReplaceOperator( F&& f ) {
    auto op = makeVisitorOperator<T, F>( f );
    m_visitorOperator.insert_or_assign( op.first, op.second );
}

template <typename T>
bool VariableSet::DynamicVisitor::removeOperator() {
    assert( hasOperator<T>() );
    auto res = m_visitorOperator.erase( getTypeIndex<T>() ) > 0;
    return res;
}

template <typename T>
auto VariableSet::DynamicVisitor::getTypeIndex() -> std::type_index {
    return std::type_index( typeid( std::reference_wrapper<VisitedType<T>> ) );
}

template <class T, class F>
inline std::pair<const std::type_index, std::function<void( std::any& )>>
VariableSet::DynamicVisitor::makeVisitorOperator( F& f ) {
    return { getTypeIndex<T>(), [&f]( std::any& a ) {
                auto rp = std::any_cast<std::reference_wrapper<VisitedType<T>>&>( a );
                auto& p = rp.get();
                f( p.first, p.second );
            } };
}

} // namespace Core
} // namespace Ra
