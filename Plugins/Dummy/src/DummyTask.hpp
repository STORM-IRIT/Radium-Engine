#ifndef DUMMYPLUGIN_DUMMYTASK_HPP
#define DUMMYPLUGIN_DUMMYTASK_HPP

#include <Core/Tasks/Task.hpp>

namespace DummyPlugin
{
    struct DummyData;

    struct DummyParams : public Ra::Core::TaskParams
    {
        DummyData* data;
    };

    class DummyTask : public Ra::Core::Task
    {
    public:
        virtual std::string getName() const override;
        virtual void init( const Ra::Core::TaskParams* params ) override;
        virtual void process() override;

    private:
        DummyData* m_data;
    };

    class DummyOtherTask : public Ra::Core::Task
    {
    public:
        virtual std::string getName() const override;
        virtual void init( const Ra::Core::TaskParams* params ) override;
        virtual void process() override;

    private:
        DummyData* m_data;
    };
}

#endif
