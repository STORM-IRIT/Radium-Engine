#include <Tests/CoreTests/Tests.hpp>

int main()
{
    RaTests::Manager::createInstance();
    RaTests::DummyTestPass p;
    RaTests::DummyTestFail f;
    return RaTests::Manager::getInstance()->run();
}

