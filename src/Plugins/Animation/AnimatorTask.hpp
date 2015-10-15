#ifndef ANIMATORTASK_H
#define ANIMATORTASK_H

#include <Plugins/Animation/AnimationPlugin.hpp>
#include <Core/Tasks/Task.hpp>
#include <Plugins/Animation/AnimationComponent.hpp>

namespace AnimationPlugin
{

class AnimatorTask : public Ra::Core::Task
{
public:
    AnimatorTask(AnimationComponent* component, Scalar dt);
    
    virtual RA_CORE_API std::string getName() const override;
    virtual RA_CORE_API void init( const Ra::Core::TaskParams* params ) override;
    virtual RA_CORE_API void process() override;
    
private:
    AnimationComponent* m_component;
    Scalar m_dt;
};

}

#endif // ANIMATORTASK_H
