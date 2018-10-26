#ifndef __PARTICLEDATA_H__
#define __PARTICLEDATA_H__

#include "PBDutils/PBDConfig.h"
#include <Core/Containers/VectorArray.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <vector>

namespace PBD {
class ParticleData {
  private:
    // Mass
    // If the mass is zero, the particle is static
    std::vector<Scalar> m_masses;
    std::vector<Scalar> m_invMasses;

    // Dynamic state
    std::vector<Ra::Core::Vector3> m_x0;
    std::vector<Ra::Core::Vector3> m_x;
    std::vector<Ra::Core::Vector3> m_v;
    std::vector<Ra::Core::Vector3> m_a;
    std::vector<Ra::Core::Vector3> m_oldX;
    std::vector<Ra::Core::Vector3> m_lastX;

    std::vector<Scalar> m_weight;

  public:
    FORCE_INLINE ParticleData( void ) :
        m_masses(),
        m_invMasses(),
        m_x0(),
        m_x(),
        m_v(),
        m_a(),
        m_oldX(),
        m_lastX() {}

    FORCE_INLINE ~ParticleData( void ) {
        m_masses.clear();
        m_invMasses.clear();
        m_x0.clear();
        m_x.clear();
        m_v.clear();
        m_a.clear();
        m_oldX.clear();
        m_lastX.clear();
    }

    FORCE_INLINE void addVertex( const Ra::Core::Vector3& vertex ) {
        m_x0.push_back( vertex );
        m_x.push_back( vertex );
        m_oldX.push_back( vertex );
        m_lastX.push_back( vertex );
        m_masses.push_back( 1.0 );
        m_invMasses.push_back( 1.0 );
        m_v.push_back( Ra::Core::Vector3( 0.0, 0.0, 0.0 ) );
        m_a.push_back( Ra::Core::Vector3( 0.0, 0.0, 0.0 ) );
    }

    FORCE_INLINE void resetCurrentPositions() {
        for ( auto i = 0; i < size(); ++i )
        {
            getOldPosition( i ) = getPosition( i );
            getPosition( i ) = Ra::Core::Vector3( 0.0, 0.0, 0.0 );
        }
    }

    FORCE_INLINE Ra::Core::Vector3& getPosition( const unsigned int i ) { return m_x[i]; }

    FORCE_INLINE const Ra::Core::Vector3& getPosition( const unsigned int i ) const {
        return m_x[i];
    }

    FORCE_INLINE void setPosition( const unsigned int i, const Ra::Core::Vector3& pos ) {
        m_x[i] = pos;
    }

    FORCE_INLINE Ra::Core::Vector3& getPosition0( const unsigned int i ) { return m_x0[i]; }

    FORCE_INLINE const Ra::Core::Vector3& getPosition0( const unsigned int i ) const {
        return m_x0[i];
    }

    FORCE_INLINE void setPosition0( const unsigned int i, const Ra::Core::Vector3& pos ) {
        m_x0[i] = pos;
    }

    FORCE_INLINE Ra::Core::Vector3& getLastPosition( const unsigned int i ) { return m_lastX[i]; }

    FORCE_INLINE const Ra::Core::Vector3& getLastPosition( const unsigned int i ) const {
        return m_lastX[i];
    }

    FORCE_INLINE void setLastPosition( const unsigned int i, const Ra::Core::Vector3& pos ) {
        m_lastX[i] = pos;
    }

    FORCE_INLINE Ra::Core::Vector3& getOldPosition( const unsigned int i ) { return m_oldX[i]; }

    FORCE_INLINE const Ra::Core::Vector3& getOldPosition( const unsigned int i ) const {
        return m_oldX[i];
    }

    FORCE_INLINE void setOldPosition( const unsigned int i, const Ra::Core::Vector3& pos ) {
        m_oldX[i] = pos;
    }

    FORCE_INLINE Ra::Core::Vector3& getVelocity( const unsigned int i ) { return m_v[i]; }

    FORCE_INLINE const Ra::Core::Vector3& getVelocity( const unsigned int i ) const {
        return m_v[i];
    }

    FORCE_INLINE void setVelocity( const unsigned int i, const Ra::Core::Vector3& vel ) {
        m_v[i] = vel;
    }

    FORCE_INLINE Ra::Core::Vector3& getAcceleration( const unsigned int i ) { return m_a[i]; }

    FORCE_INLINE const Ra::Core::Vector3& getAcceleration( const unsigned int i ) const {
        return m_a[i];
    }

    FORCE_INLINE void setAcceleration( const unsigned int i, const Ra::Core::Vector3& accel ) {
        m_a[i] = accel;
    }

    FORCE_INLINE Scalar getMass( const unsigned int i ) const { return m_masses[i]; }

    FORCE_INLINE Scalar& getMass( const unsigned int i ) { return m_masses[i]; }

    FORCE_INLINE void setMass( const unsigned int i, const Scalar mass ) {
        m_masses[i] = mass;
        if ( mass != 0.0 )
            m_invMasses[i] = 1.0f / mass;
        else
            m_invMasses[i] = 0.0;
    }

    FORCE_INLINE Scalar getInvMass( const unsigned int i ) const { return m_invMasses[i]; }

    FORCE_INLINE unsigned int getNumberOfParticles() const { return (unsigned int)m_x.size(); }

    /** Resize the array containing the particle data.
     */
    FORCE_INLINE void resize( const unsigned int newSize ) {
        m_masses.resize( newSize );
        m_invMasses.resize( newSize );
        m_x0.resize( newSize );
        m_x.resize( newSize );
        m_v.resize( newSize );
        m_a.resize( newSize );
        m_oldX.resize( newSize );
        m_lastX.resize( newSize );
    }

    /** Reserve the array containing the particle data.
     */
    FORCE_INLINE void reserve( const unsigned int newSize ) {
        m_masses.reserve( newSize );
        m_invMasses.reserve( newSize );
        m_x0.reserve( newSize );
        m_x.reserve( newSize );
        m_v.reserve( newSize );
        m_a.reserve( newSize );
        m_oldX.reserve( newSize );
        m_lastX.reserve( newSize );
    }

    /** Release the array containing the particle data.
     */
    FORCE_INLINE void release() {
        m_masses.clear();
        m_invMasses.clear();
        m_x0.clear();
        m_x.clear();
        m_v.clear();
        m_a.clear();
        m_oldX.clear();
        m_lastX.clear();
    }

    /** Release the array containing the particle data.
     */
    FORCE_INLINE unsigned int size() const { return (unsigned int)m_x.size(); }
};
} // namespace PBD

#endif
