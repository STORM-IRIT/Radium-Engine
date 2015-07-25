#include <Core/Tasks/Task.hpp>
#include <thread>
#include <iostream>

namespace Ra {
    namespace Core {
        std::mutex DummyTask::s_ioMutex;
        void DummyTask::init(const TaskParams * params)
        {
            m_iters = static_cast<const DummyTaskParams*>(params)->m_iters;
        }

        void DummyTask::process()
        {
            for (uint i = 0; i < m_iters; ++i)
            {
                s_ioMutex.lock();
                //std::cout << "Dummy task reporting" << std::endl;
                //std::cout << "on thread " << std::this_thread::get_id() << std::endl;
                s_ioMutex.unlock();
                //std::this_thread::sleep_for(std::chrono::microseconds(100));
            }
        }
    }
}
