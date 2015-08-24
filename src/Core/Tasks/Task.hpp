#ifndef RADIUMENGINE_TASK_HPP_
#define RADIUMENGINE_TASK_HPP_

#include <Core/RaCore.hpp>
#include <mutex>
#include <string>

namespace Ra
{
    namespace Core
    {
        /// Base class for Task params structs.
        struct TaskParams {};

        /// A class representing a basic task.
        class Task
        {
        public:
            /// Constructor. Most initialization should go to the init() method.
            inline Task() {}

            /// Destructor
            virtual ~Task() {}

            /// Return the name of the task.
            virtual std::string getName() const = 0;

            /// Initialize the task with according parameters.
            virtual void init( const TaskParams* params ) = 0;

            /// Do the task job. Will be called from the task queue threads.
            virtual void process() = 0;
        };

        struct DummyTaskParams : public TaskParams
        {
            Scalar m_param;
        };

        class DummyTask : public Task
        {
        public:
            virtual RA_CORE_API std::string getName() const override;

            virtual RA_CORE_API void init( const TaskParams* params ) override;

            virtual RA_CORE_API void process() override;

            Scalar m_data;

            static std::mutex s_ioMutex;
        };
    }
}

#endif //RADIUMENGINE_TASK_HPP_
