#pragma once

#include <cstdarg>
#include <set>

namespace Ra {
namespace Core {
namespace Utils {
class RA_CORE_API ObjectWithSemantic
{
  public:
    using SemanticName = std::string;

    /// Store in set to allow for logarithmic search
    using SemanticNameCollection = std::set<SemanticName>;

    inline explicit ObjectWithSemantic( const ObjectWithSemantic& other ) :
        _names( other._names ) {}

    inline bool hasSemantic( const SemanticName& name ) const {
        return _names.find( name ) != _names.end();
    }

    inline const SemanticNameCollection& semantics() const { return _names; }

    inline ObjectWithSemantic& operator=( const ObjectWithSemantic& other ) {
        CORE_UNUSED( other );
        CORE_ASSERT( _names == other._names, "Try to assign object with different semantics" );
        return *this;
    }
    inline ObjectWithSemantic& operator=( ObjectWithSemantic&& other ) {
        CORE_UNUSED( other );
        CORE_ASSERT( _names == other._names, "Try to assign object with different semantics" );
        return *this;
    }

    inline bool shareSemantic( const ObjectWithSemantic& other ) const {
        return std::any_of( _names.begin(), _names.end(), [&other]( const auto& s ) {
            return other.hasSemantic( s );
        } );
    }

    inline bool sameSemantics( const ObjectWithSemantic& other ) const {
        return _names == other._names;
    }

  protected:
    template <class... SemanticNames>
    inline ObjectWithSemantic( SemanticNames... names ) : _names( {names...} ) {}

    inline ObjectWithSemantic( const SemanticNameCollection& otherNames ) : _names( otherNames ) {}

  private:
    SemanticNameCollection _names;
};

} // namespace Utils
} // namespace Core
} // namespace Ra
