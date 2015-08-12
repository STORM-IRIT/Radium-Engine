#ifndef RIGIDBODYPLUGIN_RIGIDBODYTASK_HPP
#define RIGIDBODYPLUGIN_RIGIDBODYTASK_HPP

#include <vector>

#include <Core/Tasks/Task.hpp>

namespace RigidBodyPlugin
{
    class RigidBodyComponent;
}

namespace RigidBodyPlugin
{

    struct RA_API RigidBodyTaskParams : public Ra::Core::TaskParams
    {
        Scalar dt;
        std::vector<RigidBodyComponent*> rigidBodies;
    };

    class RA_API RigidBodyTask : public Ra::Core::Task
    {
    public:
        RigidBodyTask();
        virtual ~RigidBodyTask();


        virtual std::string getName() const override;
        virtual void init( const Ra::Core::TaskParams* params ) override;
        virtual void process() override;

    private:
        Scalar m_dt;
        std::vector<RigidBodyComponent*> m_rigidBodies;
    };

} // namespace RigidBodyPlugin

#endif // RIGIDBODYPLUGIN_RIGIDBODYTASK_HPP