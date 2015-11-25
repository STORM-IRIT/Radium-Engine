#ifndef ANIMATORTASK_H
#define ANIMATORTASK_H

#include <Plugins/Animation/AnimationPlugin.hpp>
#include <Core/Tasks/Task.hpp>
#include <Plugins/Animation/AnimationComponent.hpp>

namespace AnimationPlugin
{

class ANIM_PLUGIN_API AnimatorTask : public Ra::Core::Task
{
public:
    AnimatorTask(AnimationComponent* component, Scalar dt);
    
    virtual std::string getName() const override;
    virtual void init( const Ra::Core::TaskParams* params ) override;
    virtual void process() override;
    
private:
    AnimationComponent* m_component;
    Scalar m_dt;
};

}

#endif // ANIMATORTASK_H
