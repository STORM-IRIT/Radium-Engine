#include <Core/Math/Math.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Scene/Camera.hpp>
#include <Engine/Scene/Entity.hpp>
#include <iostream>

#include <catch2/catch.hpp>

TEST_CASE( "Engine/Camera" ) {
    using namespace Ra::Engine::Scene;
    using namespace Ra::Engine;
    using namespace Ra::Core;

    RadiumEngine::createInstance();
    RadiumEngine::getInstance()->initialize();

    Entity* ent = new Entity {"test"};

    REQUIRE( ent->getName() == "test" );

    Camera* cam = new Camera {ent, "test", 10, 10};
    cam->initialize();

    REQUIRE( Math::areApproxEqual( cam->getZNear(), 0.1_ra ) );
    REQUIRE( Math::areApproxEqual( cam->getZFar(), 1000_ra ) );

    auto dir = Ra::Core::Vector3 {1, 1, 1};
    auto pos = Ra::Core::Vector3 {0, 0, 0};
    cam->setPosition( pos );
    cam->setDirection( dir );

    REQUIRE( dir.normalized().isApprox( cam->getDirection() ) );

    /// \todo add section when cleanup is ok, now it locks tests in case of failure ...
    //    SECTION( "aabb" ) {
    Ra::Core::Aabb aabb {Ra::Core::Vector3 {3, 3, 3}, Ra::Core::Vector3 {6, 6, 6}};
    cam->fitZRange( aabb );
    REQUIRE(
        ( Math::areApproxEqual( cam->getZNear(), ( cam->getPosition() - aabb.min() ).norm() ) ||
          cam->getZNear() == cam->m_minZNear ) );
    REQUIRE(
        Math::areApproxEqual( cam->getZFar(), ( cam->getPosition() - aabb.max() ).norm(), 16_ra ) );
    REQUIRE( ( cam->getZNear() > cam->m_minZNear ||
               Math::areApproxEqual( cam->getZNear(), cam->m_minZNear ) ) );
    REQUIRE( ( ( cam->getZFar() - cam->getZNear() ) > cam->m_minZRange ||
               Math::areApproxEqual( ( cam->getZFar() - cam->getZNear() ), cam->m_minZRange ) ) );

    aabb = {Ra::Core::Vector3 {-1, -1, -1}, Ra::Core::Vector3 {1, 1, 1}};
    cam->fitZRange( aabb );
    REQUIRE( Math::areApproxEqual( cam->getZNear(), cam->m_minZNear ) );
    REQUIRE( ( Math::areApproxEqual( cam->getZFar(), ( cam->getPosition() - aabb.max() ).norm() ) ||
               Math::areApproxEqual( cam->getZFar() - cam->getZNear(), cam->m_minZRange ) ) );
    REQUIRE( ( cam->getZNear() > cam->m_minZNear ||
               Math::areApproxEqual( cam->getZNear(), cam->m_minZNear ) ) );
    REQUIRE( ( ( cam->getZFar() - cam->getZNear() ) > cam->m_minZRange ||
               Math::areApproxEqual( ( cam->getZFar() - cam->getZNear() ), cam->m_minZRange ) ) );

    aabb = {Ra::Core::Vector3 {-2, -1, 1}, Ra::Core::Vector3 {-4, -1, 0}};
    cam->fitZRange( aabb );
    REQUIRE( Math::areApproxEqual( cam->getZNear(), cam->m_minZNear ) );
    REQUIRE( Math::areApproxEqual( cam->getZFar(), cam->getZNear() + cam->m_minZRange ) );

    aabb = {Ra::Core::Vector3 {0, -3, 0}, Ra::Core::Vector3 {1, -2, 1}};
    cam->fitZRange( aabb );
    REQUIRE( Math::areApproxEqual( cam->getZNear(), cam->m_minZNear ) );
    REQUIRE( Math::areApproxEqual( cam->getZFar(), cam->getZNear() + cam->m_minZRange ) );
    //}

    /// \todo cleanup ?
    //    RadiumEngine::destroyInstance();
}
