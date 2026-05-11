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

    inline explicit ObjectWithSemantic( const ObjectWithSemantic& other ) :
        m_names { other.m_names } {}

    virtual inline ~ObjectWithSemantic() = default;

    inline bool hasSemantic( const SemanticName& name ) const {
        return m_names.find( name ) != m_names.end();
    }

    inline const SemanticNameCollection& semantics() const { return m_names; }

    inline ObjectWithSemantic& operator=( const ObjectWithSemantic& other ) {
        CORE_UNUSED( other );
        CORE_ASSERT( m_names == other.m_names, "Try to assign object with different semantics" );
        return *this;
    }
    inline ObjectWithSemantic& operator=( ObjectWithSemantic&& other ) {
        CORE_UNUSED( other );
        CORE_ASSERT( m_names == other.m_names, "Try to assign object with different semantics" );
        return *this;
    }

    inline bool shareSemantic( const ObjectWithSemantic& other ) const {
        return std::any_of( m_names.begin(), m_names.end(), [&other]( const auto& s ) {
            return other.hasSemantic( s );
        } );
    }

    inline bool sameSemantics( const ObjectWithSemantic& other ) const {
        return m_names == other.m_names;
    }

  protected:
    template <class... SemanticNames>
    inline explicit ObjectWithSemantic( SemanticNames... names ) : m_names { names... } {}

    inline explicit ObjectWithSemantic( const SemanticNameCollection& otherNames ) :
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

} // namespace Utils
} // namespace Core
} // namespace Ra
