#include <Tests/CoreTests/Tests.hpp>
#include <Tests/CoreTests/RayCasts/RayCastTest.hpp>

int main()
{
    RaTests::Manager::createInstance();
    RaTests::Manager::getInstance()->m_options.m_breakOnFailure = true;
    RaTests::DummyTestPass p;
    RaTests::RayCastAabbTests a;
    return RaTests::Manager::getInstance()->run();
}

