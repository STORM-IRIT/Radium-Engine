#include <Tests/CoreTests/Tests.hpp>

#include <Tests/CoreTests/Algebra/AlgebraTests.hpp>
#include <Tests/CoreTests/Geometry/GeometryTests.hpp>
#include <Tests/CoreTests/RayCasts/RayCastTest.hpp>

int main()
{
    if (! RaTests::TestManager::getInstance()) {RaTests::TestManager::createInstance();}
    RaTests::TestManager::getInstance()->m_options.m_breakOnFailure = true;
    return RaTests::TestManager::getInstance()->run();
}

