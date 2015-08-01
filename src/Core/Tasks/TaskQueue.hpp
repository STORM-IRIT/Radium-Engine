#ifndef RADIUMENGINE_TASK_QUEUE_HPP_
#define RADIUMENGINE_TASK_QUEUE_HPP_

#include <Core/CoreMacros.hpp>

#include <memory>
#include <vector>
#include <deque>
#include <thread>
#include <mutex>

#include <Core/Time/Timer.hpp>

namespace Ra { namespace Core { class Task; } }

namespace Ra { namespace Core 
{

    class RA_API TaskQueue
    {
    public:
        typedef uint TaskId;
        enum { InvalidTaskId = TaskId(-1) };

        /// Record of a task's start and end time.
        struct TimerData
        {
            Timer::TimePoint start;
            Timer::TimePoint end;
            std::string taskName;
        };

    public:
        // Constructor and destructor.
        TaskQueue(int numThreads);
        ~TaskQueue();

        // Note : functions are not thread safe and should only be called from the main thread.

        /// Registers a task to be executed.
        /// Task must have been created with new and be initialized with its parameter.
        /// The task queue assumes ownership of the task.
        TaskId registerTask(Task * task);

        /// Add dependency between two tasks. A task will be executed only when all
        /// its dependencies are satisfied.
        void addDependency(TaskId predecessor, TaskId successor);
        
        /// Puts the task on the queue to be executed. A task can only be queued if it has
        /// no dependencies.
        void queueTask(TaskId task);

        /// Executes the task queue. Blocks until all tasks in queue and dependencies are finished.
        void processTaskQueue();

        const std::vector<TimerData>& getTimerData();

        /// Erases all tasks. Will assert if tasks are unprocessed.
        void flushTaskQueue();

    private:
        /// Function called by a new thread given a new task. 
        void runTask(TaskId task);

    private:
        /// Maximum number of concurently running tasks.
        const uint m_numThreads; 
        /// Storage for the tasks (task will be deleted
        std::vector< std::unique_ptr<Task> > m_tasks;
        /// For each task, stores which tasks depend on it.
        std::vector< std::vector <TaskId> > m_dependencies;
        /// Stores the timings of each frame after execution.
        std::vector<TimerData> m_timerData;

        // mutex protected variables.

        /// Number of tasks each task is waiting on.
        std::vector<uint> m_remainingDependencies;
        /// Queue holding the pending tasks.
        std::deque<TaskId> m_taskQueue;
        /// Number of tasks currently being processed.
        uint m_processingTasks;

        /// Global mutex over thread-sensitive variables.
        std::mutex m_taskQueueMutex;

    };

}}

#endif // RADIUMENGINE_TASK_QUEUE_HPP_
