#ifndef SKINPLUGIN_SKINNING_TASK_HPP_
#define SKINPLUGIN_SKINNING_TASK_HPP_

#include <SkinningPlugin.hpp>
#include <Core/Tasks/Task.hpp>

namespace SkinningPlugin
{
   class SkinningComponent;

    class SkinnerTask : public Ra::Core::Task
    {
    public:
        SkinnerTask(SkinningComponent* component) : m_component(component) {}

        virtual std::string getName() const override { return "SkinnerTask";}
        virtual void init( const Ra::Core::TaskParams* params ) override{}
        virtual void process() override;

    private:
        SkinningComponent* m_component;
    };

    class SkinnerEndTask : public Ra::Core::Task
    {
    public:
        SkinnerEndTask(SkinningComponent* component) : m_component(component) {}

        virtual std::string getName() const override { return "SkinnerEndTask";}
        virtual void init( const Ra::Core::TaskParams* params ) override{}
        virtual void process() override;

    private:
        SkinningComponent* m_component;
    };


}
#endif //SKINPLUGIN_SKINNING_TASK_HPP_
