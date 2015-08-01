#ifndef RADIUMENGINE_TASK_HPP_
#define RADIUMENGINE_TASK_HPP_

#include <Core/CoreMacros.hpp>

#include <mutex>
#include <string>

namespace Ra { namespace Core 
{
    /// Base class for Task params structs.
    struct RA_API TaskParams {};

    /// A class representing a basic task.
    class RA_API Task
    {
    public:
        Task() {}
        virtual ~Task() {}
        virtual std::string getName() const;
        virtual void init(const TaskParams* params) = 0;
        virtual void process() = 0;
    };

    struct RA_API DummyTaskParams : public TaskParams
    {
        Scalar m_param;
    };

    class DummyTask : public Task
    {
    public:
        virtual RA_API std::string getName() const override;

        virtual RA_API void init(const TaskParams* params) override;

        virtual RA_API void process() override;

        Scalar m_data;

        static std::mutex s_ioMutex;
    };
}}

#endif //RADIUMENGINE_TASK_HPP_
