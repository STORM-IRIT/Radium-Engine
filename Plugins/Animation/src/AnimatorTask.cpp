#include <AnimatorTask.hpp>

namespace AnimationPlugin
{

AnimatorTask::AnimatorTask(AnimationComponent* component, Scalar dt) : m_component(component), m_dt(dt)
{

}

std::string AnimatorTask::getName() const
{
    return std::string("AnimatorTask");
}

void AnimatorTask::init( const Ra::Core::TaskParams* params )
{

}

void AnimatorTask::process()
{
    m_component->update(m_dt);
}

}
