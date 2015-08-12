#ifndef RIGIDBODYPLUGIN_RIGIDBODYCOMPONENT_HPP
#define RIGIDBODYPLUGIN_RIGIDBODYCOMPONENT_HPP

#include <Engine/Entity/Component.hpp>

namespace RigidBodyPlugin
{

    class RA_API RigidBodyComponent : public Ra::Engine::Component
    {
    public:
        RigidBodyComponent( const std::string& name );
        virtual ~RigidBodyComponent();

        virtual void initialize() override;

        void setMass( Scalar mass );
        Scalar getMass() const;
        void setKinematic();

        void addForce( const Ra::Core::Vector3& force );
        void integrate( Scalar dt );

        bool isKinematic() const;

    private:
        Scalar m_mass;
        Scalar m_invMass;

        bool m_isKinematic;
        // TODO(Charly): Add inertia, etc

        Ra::Core::Vector3 m_totalForces;

        Ra::Core::Vector3 m_linearVelocity;
        Ra::Core::Vector3 m_linearPosition;

        Ra::Core::Vector3 m_angularVelocity;
        Ra::Core::Vector3 m_angularOrientation;
    };

} // namespace RigidBodyPlugin

#endif // RIGIDBODYPLUGIN_RIGIDBODYCOMPONENT_HPP