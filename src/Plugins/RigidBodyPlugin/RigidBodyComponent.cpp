#include <Plugins/RigidBodyPlugin/RigidBodyComponent.hpp>

#include <vector>

#include <Core/Log/Log.hpp>
#include <Engine/Entity/Entity.hpp>

namespace RigidBodyPlugin
{

    RigidBodyComponent::RigidBodyComponent( const std::string& name )
        : Ra::Engine::Component( name )
        , m_mass( std::numeric_limits<Scalar>::infinity() )
        , m_invMass( 0.0 )
        , m_isKinematic( true )
        , m_totalForces( 0.0, 0.0, 0.0 )
        , m_linearPosition( 0.0, 0.0, 0.0 )
        , m_linearVelocity( 0.0, 0.0, 0.0 )
        , m_angularOrientation( 0.0, 0.0, 0.0 )
        , m_angularVelocity( 0.0, 0.0, 0.0 )
    {
    }

    RigidBodyComponent::~RigidBodyComponent()
    {
    }

    void RigidBodyComponent::initialize()
    {
    }

    void RigidBodyComponent::setMass( Scalar mass )
    {
        m_mass = mass;
        m_invMass = Scalar( 1.0 / mass );

        if ( m_invMass != 0.0 )
        {
            m_isKinematic = false;
        }
    }

    Scalar RigidBodyComponent::getMass() const
    {
        return m_mass;
    }

    void RigidBodyComponent::setKinematic()
    {
        m_invMass = Scalar( 0.0 );
        m_isKinematic = true;
    }

    bool RigidBodyComponent::isKinematic() const
    {
        return m_isKinematic;
    }

    void RigidBodyComponent::addForce( const Ra::Core::Vector3& force )
    {
        m_totalForces += force;
    }

    void RigidBodyComponent::integrate( Scalar dt )
    {
        Ra::Core::Transform trans = m_entity->getTransform();
        m_linearPosition = trans.translation();

        Ra::Core::Vector3 accel = m_totalForces * m_invMass;
        m_linearVelocity += accel * dt;
        m_linearPosition += m_linearVelocity * dt;

        if ( 0 )
        {
            LOG( logDEBUG ) << "";
            LOG( logDEBUG ) << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-";
            LOG( logDEBUG ) << "Position before : " << trans.translation().x() << " " << trans.translation().y() << " " <<
                            trans.translation().z();
            LOG( logDEBUG ) << "Position after  : " << m_linearPosition.x() << " " << m_linearPosition.y() << " " <<
                            m_linearPosition.z();
            LOG( logDEBUG ) << "Velocity        : " << m_linearVelocity.x() << " " << m_linearVelocity.y() << " " <<
                            m_linearVelocity.z();
            LOG( logDEBUG ) << "Total Force     : " << m_totalForces.x() << " " << m_totalForces.y() << " " << m_totalForces.z();
            LOG( logDEBUG ) << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-";
            LOG( logDEBUG ) << "";
        }

        trans.translation() = m_linearPosition;
        m_entity->setTransform( trans );

        m_totalForces = Ra::Core::Vector3( 0.0, 0.0, 0.0 );
    }

} // namespace RigidBodyPlugin