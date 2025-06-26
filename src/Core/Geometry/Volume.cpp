#include <Core/Geometry/Volume.hpp>
#include <Core/Utils/Log.hpp>
#include <memory>
#include <ostream>
#include <string>

namespace Ra {
namespace Core {
namespace Geometry {

AbstractVolume::AbstractVolume( const VolumeStorageType& type ) :
    AbstractGeometry(), m_type( type ) {}

bool AbstractVolume::isParametric() const {
    return ( m_type == PARAMETRIC );
}

bool AbstractVolume::isDiscrete() const {
    return ( m_type == DISCRETE_DENSE ) || ( m_type == DISCRETE_SPARSE );
}

bool AbstractVolume::isDense() const {
    return ( m_type == DISCRETE_DENSE );
}

bool AbstractVolume::isSparse() const {
    return ( m_type == DISCRETE_SPARSE );
}

void AbstractVolume::displayInfo() const {
    using namespace Core::Utils; // log
    std::string type;
    switch ( m_type ) {
    case UNKNOWN:
        type = "UNKNOWN";
        break;
    case PARAMETRIC:
        type = "PARAMETRIC";
        break;
    case DISCRETE_DENSE:
        type = "DISCRETE (DENSE)";
        break;
    case DISCRETE_SPARSE:
        type = "DISCRETE (SPARSE)";
        break;
    }
    LOG( logINFO ) << "======== MESH INFO ========";
    LOG( logINFO ) << " Type           : " << type;
}

void AbstractDiscreteVolume::clear() {
    setBinSize( Vector3::Zero() );
    setSize( Vector3i::Zero() );
    invalidateAabb();
}

Aabb AbstractDiscreteVolume::computeAabb() const {
    if ( !isAabbValid() ) {
        setAabb( Aabb( Vector3::Zero(), m_binSize.cwiseProduct( m_size.cast<Scalar>() ) ) );
    }
    return getAabb();
}

VolumeGrid::ValueType VolumeGrid::sample( const IndexType& i ) {
    IndexType idx {
        std::clamp( i.x(), 0, size().x() - 1 ),
        std::clamp( i.y(), 0, size().y() - 1 ),
        std::clamp( i.z(), 0, size().z() - 1 ),
    };
    return m_data[*linearIndex( idx )];
}

void VolumeGrid::computeGradients() {
    m_gradient.resize( m_data.size() );
    auto s = size();

#pragma omp parallel for firstprivate( s )
    for ( int k = 0; k < s.z(); ++k ) {
        for ( int j = 0; j < s.y(); ++j ) {
            for ( int i = 0; i < s.x(); ++i ) {
                Eigen::Matrix<ValueType, 3, 1> s1;
                Eigen::Matrix<ValueType, 3, 1> s2;
                s1( 0 ) = sample( { i - 1, j, k } );
                s2( 0 ) = sample( { i + 1, j, k } );
                s1( 1 ) = sample( { i, j - 1, k } );
                s2( 1 ) = sample( { i, j + 1, k } );
                s1( 2 ) = sample( { i, j, k - 1 } );
                s2( 2 ) = sample( { i, j, k + 1 } );
                IndexType idx { i, j, k };
                Eigen::Matrix<ValueType, 3, 1> gradient = s2 - s1;
                m_gradient[*linearIndex( idx )]         = {
                    gradient[0], gradient[1], gradient[2], sample( { i, j, k } ) };
            }
        }
    }
}

} // namespace Geometry
} // namespace Core
} // namespace Ra
