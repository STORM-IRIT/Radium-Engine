#ifndef RIGIDBODYPLUGIN_RIGIDBODYTASK_HPP
#define RIGIDBODYPLUGIN_RIGIDBODYTASK_HPP

#include <Plugins/RigidBodyPlugin/RigidBodyPlugin.hpp>

#include <vector>
#include <Core/Tasks/Task.hpp>

namespace RigidBodyPlugin
{
    class RigidBodyComponent;
}

namespace RigidBodyPlugin
{

    struct RigidBodyTaskParams : public Ra::Core::TaskParams
    {
        Scalar dt;
        std::vector<RigidBodyComponent*> rigidBodies;
    };

    class RigidBodyTask : public Ra::Core::Task
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