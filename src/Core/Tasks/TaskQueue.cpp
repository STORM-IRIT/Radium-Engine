#include <Core/Tasks/TaskQueue.hpp>
#include <Core/Tasks/Task.hpp>

namespace Ra {
    namespace Core
    {

        TaskQueue::TaskQueue(int numThreads)
            : m_numThreads(numThreads), m_processingTasks(0)
        {}

        TaskQueue::~TaskQueue()
        {
            flushTaskQueue();
        }

        TaskQueue::TaskId TaskQueue::registerTask(Task * task)
        {
            m_tasks.emplace_back(std::unique_ptr<Task>(task));
            m_dependencies.push_back(std::vector<TaskId>());
            CORE_ASSERT(m_tasks.size() == m_dependencies.size(), "Inconsistent task list");
            return TaskId(m_tasks.size() - 1);
        }

        void TaskQueue::addDependency(TaskQueue::TaskId predecessor, TaskQueue::TaskId successor)
        {
            CORE_ASSERT((predecessor != InvalidTaskId) && (predecessor < m_tasks.size()), "Invalid predecessor task");
            CORE_ASSERT((successor != InvalidTaskId) && (successor < m_tasks.size()), "Invalid successor task");
            CORE_ASSERT(predecessor != successor, "Cannot add self-dependency");

            // Todo : check for cycles.

            m_dependencies[predecessor].push_back(successor);
        }

        void TaskQueue::queueTask(TaskQueue::TaskId task)
        {
            m_taskQueue.push_front(task);
        }

        void TaskQueue::processTaskQueue()
        {
            bool isProcessing = true;
            while (isProcessing)
            {
                // Critical section : decide what to do. 
                m_taskQueueMutex.lock();
                if (!m_taskQueue.empty() && m_processingTasks < m_numThreads)
                {
                    TaskId task = m_taskQueue.back();
                    m_taskQueue.pop_back();
                    ++m_processingTasks;
                    CORE_ASSERT(task != InvalidTaskId && task < m_tasks.size(), "Invalid task");
                    std::thread thread(&TaskQueue::runTask, this, task);
                    thread.detach();
                }
                else if (m_taskQueue.empty() && m_processingTasks == 0)
                {
                    isProcessing = false;
                }
                m_taskQueueMutex.unlock();
                // End critical section.
                std::this_thread::yield();
            }
        }

        void TaskQueue::flushTaskQueue()
        {
            CORE_ASSERT(m_processingTasks == 0, "You have tasks still in process");
            CORE_ASSERT(m_taskQueue.empty(), " You have unprocessed tasks ");
            m_tasks.clear();
            m_dependencies.clear();
        }

        void TaskQueue::runTask(TaskQueue::TaskId task)
        {
            // Run task
            m_tasks[task]->process();

            // Critical section : mark task as finished and en-queue dependencies.
            m_taskQueueMutex.lock();
            --m_processingTasks;
            for (auto t : m_dependencies[task])
            {
                queueTask(t);
            }
            m_taskQueueMutex.unlock();
        }
    }
}