#include <Tests/CoreTests/Tests.hpp>
#include <Tests/CoreTests/RayCasts/RayCastTest.hpp>

int main()
{
    RaTests::TestManager::createInstance();
    RaTests::TestManager::getInstance()->m_options.m_breakOnFailure = true;
    RaTests::DummyTestPass p;
    RaTests::RayCastAabbTests a;
    return RaTests::TestManager::getInstance()->run();
}

