#include <Core/Containers/VariableSet.hpp>
#include <Core/CoreMacros.hpp>

// inspirations :
// Radium dataflow dynamic type management
// Radium attribArray
// factory design pattern
// visitor design pattern
// https://en.cppreference.com/w/cpp/utility/any/type
// https://gieseanw.wordpress.com/2017/05/03/a-true-heterogeneous-container-in-c/
namespace Ra {
namespace Core {

auto VariableSet::VariableSetFunctions::getInstance() -> VariableSetFunctions* {
    static VariableSet::VariableSetFunctions instance;
    return &instance;
}

auto VariableSet::operator=( const VariableSet& other ) -> VariableSet& {
    m_variables              = other.m_variables;
    m_typeIndexToVtableIndex = other.m_typeIndexToVtableIndex;
    m_storedType             = other.m_storedType;
    return *this;
}

auto VariableSet::operator=( VariableSet&& other ) noexcept -> VariableSet& {
    m_variables              = std::move( other.m_variables );
    m_typeIndexToVtableIndex = std::move( other.m_typeIndexToVtableIndex );
    m_storedType             = std::move( other.m_storedType );
    return *this;
}

void VariableSet::clear() {
    m_variables.clear();
}

void VariableSet::mergeKeepVariables( const VariableSet& from ) {
    for ( const auto& type : from.getStoredTypes() ) {
        const auto& index = from.m_typeIndexToVtableIndex.at( type );
        from.m_vtable->m_mergeKeepFunctions[index]( from, *this );
    }
}

void VariableSet::mergeReplaceVariables( const VariableSet& from ) {
    for ( const auto& type : from.getStoredTypes() ) {
        const auto& index = from.m_typeIndexToVtableIndex.at( type );
        from.m_vtable->m_mergeReplaceFunctions[index]( from, *this );
    }
}

size_t VariableSet::size() const {
    size_t sum { 0 };
    for ( const auto& [type, index] : m_typeIndexToVtableIndex ) {
        sum += m_vtable->m_sizeFunctions[index]( *this );
    }
    return sum;
}

void VariableSet::setEnumVariable( const std::string& name, const std::string& value ) {
    auto converterFunc = existsVariable<
        std::function<void( Core::VariableSet&, const std::string&, const std::string& )>>( name );
    if ( converterFunc ) { ( *converterFunc )->second( *this, name, value ); }
    else {
        LOG( Core::Utils::logWARNING ) << "VariableSet: try to set enum value from string without "
                                          "converter, ignored. Variable name: ["
                                       << name << "], value: [" << value << "]";
    }
}

void VariableSet::setEnumVariable( const std::string& name, const char* value ) {
    setEnumVariable( name, std::string( value ) );
}

bool VariableSet::DynamicVisitor::accept( const std::type_index& id ) const {
    return m_visitorOperator.find( id ) != m_visitorOperator.cend();
}

void VariableSet::DynamicVisitor::operator()( std::any&& in, std::any&& userParam ) const {
    m_visitorOperator.at( std::type_index( in.type() ) )( in, std::forward<std::any>( userParam ) );
}

} // namespace Core
} // namespace Ra
