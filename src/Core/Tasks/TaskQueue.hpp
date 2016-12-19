#ifndef RADIUMENGINE_TASK_QUEUE_HPP_
#define RADIUMENGINE_TASK_QUEUE_HPP_


#include <Core/RaCore.hpp>
#include <memory>
#include <vector>
#include <deque>
#include <thread>
#include <mutex>
#include <string>
#include <condition_variable>

#include <Core/Time/Timer.hpp>

namespace Ra
{
    namespace Core
    {
        class Task;
    }
}

namespace Ra
{
    namespace Core
    {
        /// This class allows tasks to be registered and then executed in parallel on separate threads.
        /// it maintains an internal pool of threads. When instructed, it dispatches the tasks to the
        /// pooled threads.
        /// Task are allowed to have dependencies. A task will be executed only when all its dependencies
        /// are satisfied, i.e. all dependant tasks are finished.
        /// Note that most functions are not thread safe and must not be called when the task queue is running.
        class RA_CORE_API TaskQueue
        {
        public:
            /// Identifier for a task in the task queue.
            typedef uint TaskId;
            enum { InvalidTaskId = TaskId( -1 ) };

            /// Record of a task's start and end time.
            struct TimerData
            {
                Timer::TimePoint start;
                Timer::TimePoint end;
                std::string taskName;
            };

        public:

            /// Constructor. Initializes the thread pools with numThreads threads.
            TaskQueue( uint numThreads );

            /// Destructor. Waits for all the threads and safely deletes them.
            ~TaskQueue();

            //
            // Task management
            //

            /// Registers a task to be executed.
            /// Task must have been created with new and be initialized with its parameter.
            /// The task queue assumes ownership of the task.
            TaskId registerTask( Task* task );

            /// Add dependency between two tasks. The successor task will be executed only when all
            /// its predecessor completed.
            void addDependency( TaskId predecessor, TaskId successor );

            /// Add dependency between a task and all task with a given name.
            /// Will return false if no dependency has been added.
            bool addDependency( const std::string& predecessors, TaskId successor);
            bool addDependency( TaskId predecessor, const std::string& successors);

            /// Add a dependency between a task an all tasks with a given name, even
            /// if the task is not present yet, the name being resolved when task start.
            void addPendingDependency( const std::string& predecessors, TaskId successor);
            void addPendingDependency( TaskId predecessor, const std::string& successors);

            //
            // Task queue operations
            //

            /// Launches the execution of all the threads in the task queue.
            /// No more tasks should be added at this point.
            void startTasks();

            /// Blocks until all tasks and dependencies are finished.
            void waitForTasks();

            /// Access the data from the last frame execution after processTaskQueue();
            const std::vector<TimerData>& getTimerData();

            /// Erases all tasks. Will assert if tasks are unprocessed.
            void flushTaskQueue();

        private:

            /// Function called by a new thread.
            void runThread( uint id );

            /// Puts the task on the queue to be executed. A task can only be queued if it has
            /// no dependencies.
            void queueTask( TaskId task );

            /// Detect if there are any cycles in the task graph, and asserts if it is the case.
            /// (this function is compiled to nothing in release).
            void detectCycles();

            /// Resolves the pending named dependencies. Will assert if dependencies don't resolve.
            void resolveDependencies();

        private:

            /// Threads working on tasks.
            std::vector<std::thread> m_workerThreads;
            /// Storage for the tasks (task will be deleted after flushQueue()).
            std::vector<std::unique_ptr<Task>> m_tasks;
            /// For each task, stores which tasks depend on it.
            std::vector<std::vector <TaskId>> m_dependencies;

            /// List of pending dependencies
            std::vector<std::pair<TaskId,std::string>> m_pendingDepsPre;
            std::vector<std::pair<std::string,TaskId>> m_pendingDepsSucc;

            /// Stores the timings of each frame after execution.
            std::vector<TimerData> m_timerData;

            //
            // mutex protected variables.
            //

            /// Number of tasks each task is waiting on.
            std::vector<uint> m_remainingDependencies;
            /// Queue holding the pending tasks.
            std::deque<TaskId> m_taskQueue;
            /// Number of tasks currently being processed.
            uint m_processingTasks;

            /// Flag to signal threads to quit.
            bool m_shuttingDown;
            /// Variable on which threads wait for new tasks.
            std::condition_variable m_threadNotifier;
            /// Global mutex over thread-sensitive variables.
            std::mutex m_taskQueueMutex;

        };

    }
}

#endif // RADIUMENGINE_TASK_QUEUE_HPP_
