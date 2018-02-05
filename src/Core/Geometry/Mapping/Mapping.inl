#include <Core/Geometry/Mapping/Mapping.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

/// CONSTRUCTOR
Mapping::Mapping(const Scalar alpha, const Scalar beta, const Scalar delta, Index id ) :
    m_coord( Vector2( alpha, beta ) ),
    m_delta( delta ),
    m_id( id ) { }

/// DESTRUCTOR
Mapping::~Mapping() { }

/// BARYCENTRIC COORDINATE
inline Scalar Mapping::getAlpha() const {
    return m_coord[0];
}

inline Scalar Mapping::getBeta() const {
    return m_coord[1];
}

inline Scalar Mapping::getGamma() const {
    return ( 1.0 - getAlpha() - getBeta() );
}

inline void Mapping::setAlpha( const Scalar alpha ) {
    m_coord[0] = alpha;
}

inline void Mapping::setBeta( const Scalar beta ) {
    m_coord[1] = beta;
}

inline Vector3 Mapping::getCoord() const {
    return Vector3( getAlpha(), getBeta(), getGamma() );
}

/// DELTA
inline Scalar Mapping::getDelta() const {
    return m_delta;
}

inline void Mapping::setDelta( const Scalar delta ) {
    m_delta = delta;
}

/// ID
inline Index Mapping::getID() const {
    return m_id;
}

inline void Mapping::setID( const Index& id ) {
    m_id = id;
}

/// POINT
inline Vector3 Mapping::getPoint( const Vector3& p0,
                                  const Vector3& p1,
                                  const Vector3& p2,
                                  const Vector3& n ) const {
    return ( ( ( getAlpha() * p0 ) + ( getBeta() * p1 ) + ( getGamma() * p2 ) ) + ( getDelta() * n ) );
}

/// QUERY
inline bool Mapping::isFinite() const {
    if( std::isnan( getAlpha() ) || std::isinf( getAlpha() ) ) {
        return false;
    }
    if( std::isnan( getBeta() ) || std::isinf( getBeta() ) ) {
        return false;
    }
    if( std::isnan( getGamma() ) || std::isinf( getGamma() ) ) {
        return false;
    }
    if( std::isnan( getDelta() ) || std::isinf( getDelta() ) ) {
        return false;
    }
    return true;
}

inline bool Mapping::isInside() const {
    return ( ( getAlpha() >= 0.0 ) &&
             ( getBeta()  >= 0.0 ) &&
             ( getGamma() >= 0.0 ) &&
             ( ( getAlpha() + getBeta() + getGamma() ) == 1.0 ) );
}

inline bool Mapping::isBoundToElement() const {
    return ( getID().isValid() );
}

} // namespace Geometry
} // namespace Core
} // namespace Ra
