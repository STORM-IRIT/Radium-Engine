#ifndef RADIUMENGINE_TASK_HPP_
#define RADIUMENGINE_TASK_HPP_

#include <Core/RaCore.hpp>
#include <string>

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
    }
}

#endif //RADIUMENGINE_TASK_HPP_
