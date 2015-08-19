#include <Plugins/RigidBodyPlugin/RigidBodyTask.hpp>

#include <Core/Log/Log.hpp>

#include <Plugins/RigidBodyPlugin/RigidBodyComponent.hpp>

namespace RigidBodyPlugin
{
    RigidBodyTask::RigidBodyTask()
    {
    }

    RigidBodyTask::~RigidBodyTask()
    {
    }


    std::string RigidBodyTask::getName() const
    {
        return std::string( "RigidBody" );
    }

    void RigidBodyTask::init( const Ra::Core::TaskParams* params )
    {
        const RigidBodyTaskParams* p = static_cast<const RigidBodyTaskParams*>( params );

        m_dt = p->dt;
        m_rigidBodies = p->rigidBodies;
    }

    void RigidBodyTask::process()
    {
        static const Ra::Core::Vector3 GRAVITY( 0, -9.81, 0 );
        //LOG(logINFO) << "Integrating " << m_rigidBodies.size() << " rigid bodies with dt = " << m_dt;

        for ( auto& rb : m_rigidBodies )
        {
            // Do integration
            if ( !rb->isKinematic() )
            {
                rb->addForce( GRAVITY * rb->getMass() );
                rb->integrate( m_dt );
            }
        }
    }

} // namespace RigidBodyPlugin