#ifndef RADIUMENGINE_TASK_QUEUE_HPP_
#define RADIUMENGINE_TASK_QUEUE_HPP_

#include <Core/CoreMacros.hpp>

#include <memory>
#include <vector>
#include <deque>
#include <thread>
#include <mutex>

namespace Ra { namespace Core { class Task; } }

namespace Ra { namespace Core 
{

    class TaskQueue
    {
    public:
        typedef uint TaskId;
        enum { InvalidTaskId = TaskId(-1) };
    public:
        // Constructor and destructor.
        TaskQueue(int numThreads);
        ~TaskQueue();

        /// Registers a task to be executed. Assume task is initialized with parameters.
        /// the task queue assumes ownership of the task.
        TaskId registerTask(Task * task);

        /// Add dependency between two tasks.
        void addDependency(TaskId predecessor, TaskId successor);
        
        /// Puts the task on the queue to be executed.
        void queueTask(TaskId task);

        /// Executes the task queue. Blocks until all tasks are finished.
        void processTaskQueue();

        /// Erases all tasks. Will assert if tasks are unprocessed.
        void flushTaskQueue();

    private:
        // Function called by a new thread given a new task. 
        void runTask(TaskId task);

    private:
        const uint m_numThreads; 
        std::vector< std::unique_ptr<Task> > m_tasks;
        std::vector< std::vector <TaskId> > m_dependencies;

        // mutex protected variables.
        std::mutex m_taskQueueMutex;
        std::deque<TaskId> m_taskQueue;
        uint m_processingTasks;

    };

}}

#endif // RADIUMENGINE_TASK_QUEUE_HPP_