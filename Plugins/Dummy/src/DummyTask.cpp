#include "DummyTask.hpp"

#include <mutex>

#include "DummySystem.hpp"

namespace {
std::mutex ioMutex;
}

namespace DummyPlugin {
std::string DummyTask::getName() const {
    return "DummyTask";
}

void DummyTask::init( const Ra::Core::TaskParams* p ) {
    auto params = static_cast<const DummyParams*>( p );
    m_data      = params->data;
}

void DummyTask::process() {
    ioMutex.lock();
    LOG( logINFO ) << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-";
    LOG( logINFO ) << "I am the first task, I should be executed first";
    LOG( logINFO ) << "My data before process is " << m_data->foo << " - " << m_data->bar << ".";
    ioMutex.unlock();

    m_data->foo *= 2;
    m_data->bar /= 2;

    ioMutex.lock();
    LOG( logINFO ) << "My data after process is " << m_data->foo << " - " << m_data->bar << ".";

    LOG( logINFO ) << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
    ioMutex.unlock();
}

//----------------------------------------------------------//
std::string DummyOtherTask::getName() const {
    return "DummyOtherTask";
}

void DummyOtherTask::init( const Ra::Core::TaskParams* p ) {
    auto params = static_cast<const DummyParams*>( p );
    m_data      = params->data;
}

void DummyOtherTask::process() {
    ioMutex.lock();
    LOG( logINFO ) << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-";
    LOG( logINFO ) << "I am the second task, I should be executed second";
    LOG( logINFO ) << "My data before process is " << m_data->foo << " - " << m_data->bar << ".";
    ioMutex.unlock();

    m_data->foo -= 10;
    m_data->bar += 100;

    ioMutex.lock();
    LOG( logINFO ) << "My data after process is " << m_data->foo << " - " << m_data->bar << ".";
    LOG( logINFO ) << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
    ioMutex.unlock();
}
} // namespace DummyPlugin
