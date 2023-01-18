#include <Core/Containers/VariableSet.hpp>
// inspirations :
// Radium dataflow dynamic type management
// Radium attribArray
// factory design pattern
// visitor design pattern
// https://en.cppreference.com/w/cpp/utility/any/type
// https://gieseanw.wordpress.com/2017/05/03/a-true-heterogeneous-container-in-c/
namespace Ra {
namespace Core {

VariableSet& VariableSet::operator=( const VariableSet& other ) {
    clear();
    m_clearFunctions        = other.m_clearFunctions;
    m_copyFunctions         = other.m_copyFunctions;
    m_moveFunctions         = other.m_moveFunctions;
    m_mergeKeepFunctions    = other.m_mergeKeepFunctions;
    m_mergeReplaceFunctions = other.m_mergeReplaceFunctions;
    m_sizeFunctions         = other.m_sizeFunctions;
    m_visitFunctions        = other.m_visitFunctions;
    m_storedType            = other.m_storedType;
    for ( auto&& copyFunction : m_copyFunctions ) {
        copyFunction( other, *this );
    }
    return *this;
}

VariableSet& VariableSet::operator=( VariableSet&& other ) {
    clear();
    m_clearFunctions        = std::move( other.m_clearFunctions );
    m_copyFunctions         = std::move( other.m_copyFunctions );
    m_moveFunctions         = std::move( other.m_moveFunctions );
    m_mergeKeepFunctions    = std::move( other.m_mergeKeepFunctions );
    m_mergeReplaceFunctions = std::move( other.m_mergeReplaceFunctions );
    m_sizeFunctions         = std::move( other.m_sizeFunctions );
    m_visitFunctions        = std::move( other.m_visitFunctions );
    m_storedType            = std::move( other.m_storedType );
    for ( auto&& moveFunction : m_moveFunctions ) {
        moveFunction( other, *this );
    }
    return *this;
}

void VariableSet::clear() {
    for ( auto&& clearFunc : m_clearFunctions ) {
        clearFunc( *this );
    }
    m_clearFunctions.clear();
    m_copyFunctions.clear();
    m_moveFunctions.clear();
    m_mergeKeepFunctions.clear();
    m_mergeReplaceFunctions.clear();
    m_sizeFunctions.clear();
    m_visitFunctions.clear();
    m_storedType.clear();
}

void VariableSet::mergeKeepVariables( const VariableSet& from ) {
    for ( auto&& mergeFunc : from.m_mergeKeepFunctions ) {
        mergeFunc( from, *this );
    }
}

void VariableSet::mergeReplaceVariables( const VariableSet& from ) {
    for ( auto&& mergeFunc : from.m_mergeReplaceFunctions ) {
        mergeFunc( from, *this );
    }
}

size_t VariableSet::size() const {
    size_t sum = 0;
    for ( auto&& sizeFunc : m_sizeFunctions ) {
        sum += sizeFunc( *this );
    }
    return sum;
}

bool VariableSet::DynamicVisitor::accept( const std::type_index& id ) const {
    return m_visitorOperator.find( id ) != m_visitorOperator.cend();
}

void VariableSet::DynamicVisitor::operator()( std::any&& in, std::any&& userParam ) const {
    m_visitorOperator.at( std::type_index( in.type() ) )( in, std::forward<std::any>( userParam ) );
}

} // namespace Core
} // namespace Ra
