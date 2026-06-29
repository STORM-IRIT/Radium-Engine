#pragma once

#include <Core/RaCore.hpp>

#include <algorithm>
#include <cstdarg>
#include <set>

namespace Ra {
namespace Core {
namespace Utils {
/// \brief Object associated with one or multiple semantic names
///
/// Holds a static collection of names (#SemanticNameCollection) that are defined by the inheriting
/// class when calling the protected constructor.
/// Provides #hasSemantic to test if one object has a given semantic name, as well as #shareSemantic
/// and #sameSemantics to compare different objects.
class RA_CORE_API ObjectWithSemantic
{
  public:
    using SemanticName = std::string;

    /// Store in set to allow for logarithmic search,
    /// \todo move out of class ? extends set to add helper functions ?
    using SemanticNameCollection = std::set<SemanticName>;

    explicit ObjectWithSemantic( const ObjectWithSemantic& other ) : m_names { other.m_names } {}

    virtual ~ObjectWithSemantic() = default;

    bool hasSemantic( const SemanticName& name ) const;

    const SemanticNameCollection& semantics() const { return m_names; }

    ObjectWithSemantic& operator=( const ObjectWithSemantic& other ) {
        CORE_UNUSED( other );
        CORE_ASSERT( m_names == other.m_names, "Try to assign object with different semantics" );
        return *this;
    }
    ObjectWithSemantic& operator=( ObjectWithSemantic&& other ) {
        CORE_UNUSED( other );
        CORE_ASSERT( m_names == other.m_names, "Try to assign object with different semantics" );
        return *this;
    }

    bool shareSemantic( const ObjectWithSemantic& other ) const;

    bool sameSemantics( const ObjectWithSemantic& other ) const;

  protected:
    template <class... SemanticNames>
    explicit ObjectWithSemantic( SemanticNames... names ) : m_names { names... } {}

    explicit ObjectWithSemantic( const SemanticNameCollection& otherNames ) :
        m_names { otherNames } {}

  private:
    SemanticNameCollection m_names;
};

inline bool hasSemantic( const ObjectWithSemantic::SemanticNameCollection& a,
                         const ObjectWithSemantic::SemanticName& name ) {
    return a.contains( name );
}

inline bool shareSemantic( const ObjectWithSemantic::SemanticNameCollection& a,
                           const ObjectWithSemantic::SemanticNameCollection& b ) {
    return std::any_of( a.begin(), a.end(), [&b]( const auto& s ) { return hasSemantic( b, s ); } );
}

inline bool sameSemantics( const ObjectWithSemantic::SemanticNameCollection& a,
                           const ObjectWithSemantic::SemanticNameCollection& b ) {
    return a == b;
}

inline bool ObjectWithSemantic::hasSemantic( const SemanticName& name ) const {
    return ::Ra::Core::Utils::hasSemantic( m_names, name );
}

inline bool ObjectWithSemantic::shareSemantic( const ObjectWithSemantic& other ) const {
    return ::Ra::Core::Utils::shareSemantic( m_names, other.semantics() );
}

inline bool ObjectWithSemantic::sameSemantics( const ObjectWithSemantic& other ) const {
    return ::Ra::Core::Utils::sameSemantics( m_names, other.m_names );
}

} // namespace Utils
} // namespace Core
} // namespace Ra
