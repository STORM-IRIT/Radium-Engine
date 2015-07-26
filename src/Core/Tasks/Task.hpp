#ifndef RADIUMENGINE_TASK_HPP_
#define RADIUMENGINE_TASK_HPP_

#include <Core/CoreMacros.hpp>

#include <mutex>

namespace Ra { namespace Core 
{
    /// Base class for Task params structs.
    struct TaskParams {};

    /// A class representing a basic task.
    class Task
    {
    public:
        Task() {}
        virtual ~Task() {}
        virtual void init(const TaskParams* params) = 0;
        virtual void process() = 0;
    };

    struct DummyTaskParams : public TaskParams
    {
        Scalar m_param;
    };

    class DummyTask : public Task
    {
    public:
        virtual void init(const TaskParams* params) override;

        virtual void process() override;

        Scalar m_data;

        static std::mutex s_ioMutex;
    };
}}

#endif //RADIUMENGINE_TASK_HPP_ 