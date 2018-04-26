#include <Tests/Tests.hpp>

#include <Tests/CoreTests/src/AlgebraTests.hpp>
#include <Tests/CoreTests/src/AnimationTest.hpp>
#include <Tests/CoreTests/src/ContainersTest.hpp>
#include <Tests/CoreTests/src/DistanceTests.hpp>
#include <Tests/CoreTests/src/GeometryTests.hpp>
#include <Tests/CoreTests/src/IndexMapTest.hpp>
#include <Tests/CoreTests/src/RayCastTest.hpp>
#include <Tests/CoreTests/src/StringTest.hpp>
#include <Tests/CoreTests/src/TopologicalMesh.hpp>

int main() {
    if ( !RaTests::TestManager::getInstance() )
    {
        RaTests::TestManager::createInstance();
    }
    RaTests::TestManager::getInstance()->m_options.m_breakOnFailure = true;
    return RaTests::TestManager::getInstance()->run();
}
