#ifndef RADIUMENGINE_TASK_HPP_
#define RADIUMENGINE_TASK_HPP_

#include <Core/RaCore.hpp>
#include <string>
#include <functional>

namespace Ra
{
    namespace Core
    {
        /// An interface class representing a basic task.
        class Task
        {
        public:
            /// Destructor
            virtual ~Task() {}

            /// Return the name of the task.
            virtual std::string getName() const = 0;

            /// Do the task job. Will be called from the task queue threads.
            virtual void process() = 0;
        };

        /// A wrapper for a task around a std::function.
        /// The function must be of type void(void)
        /// Use std::bind to bind the arguments to the function object when creating the task.
        class FunctionTask : public Task
        {
        public:
            /// Create a function task
            FunctionTask(const std::function<void(void)>& f, const std::string& name)
                    : m_f(f), m_name(name) {}

            /// Return the provided task name
            virtual std::string getName() const override { return m_name; }

            /// Call the function.
            virtual void process() override { m_f(); }

        protected:
            std::function<void(void)> m_f; /// The function to call
            std::string m_name; /// Name of the task
        };


    }
}

#endif //RADIUMENGINE_TASK_HPP_
