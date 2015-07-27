#include <Core/Tasks/Task.hpp>
#include <thread>
#include <iostream>

namespace Ra {
    namespace Core {
        std::mutex DummyTask::s_ioMutex;
        void DummyTask::init(const TaskParams * params)
        {
            m_data = static_cast<const DummyTaskParams*>(params)->m_param;
        }

        void DummyTask::process()
        {
           /*   s_ioMutex.lock();
                std::cout << "Dummy task reporting" <<m_data<< std::endl;
                std::cout << "on thread " << std::this_thread::get_id() << std::endl;
                s_ioMutex.unlock();*/
        }
    }
}
