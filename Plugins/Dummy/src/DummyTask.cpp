#include "DummyTask.hpp"

#include <mutex>

#include "DummySystem.hpp"

namespace
{
    std::mutex ioMutex;
}

namespace DummyPlugin
{
    std::string DummyTask::getName() const
    {
        return "DummyTask";
    }

    void DummyTask::init( const DummyParams* params )
    {
        m_data = params->data;
    }

    void DummyTask::process()
    {
        LOG( logINFO ) << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-";
        LOG( logINFO ) << "I am the first task, I should be executed first";
        LOG( logINFO ) << "My data before process is " << m_data->foo
                       << " - " << m_data->bar << ".";

        m_data->foo *= 2;
        m_data->bar /= 2;

        LOG( logINFO ) << "My data after process is " << m_data->foo
                       << " - " << m_data->bar << ".";

        LOG( logINFO ) << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
    }

    //----------------------------------------------------------//
    std::string DummyOtherTask::getName() const
    {
        return "DummyOtherTask";
    }

    void DummyOtherTask::init( const DummyParams* params )
    {
        m_data = params->data;
    }

    void DummyOtherTask::process()
    {
        LOG( logINFO ) << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-";
        LOG( logINFO ) << "I am the first task, I should be executed first";
        LOG( logINFO ) << "My data before process is " << m_data->foo
                       << " - " << m_data->bar << ".";

        m_data->foo -= 10;
        m_data->bar += 100;

        LOG( logINFO ) << "My data after process is " << m_data->foo
                       << " - " << m_data->bar << ".";

        LOG( logINFO ) << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
    }
}

