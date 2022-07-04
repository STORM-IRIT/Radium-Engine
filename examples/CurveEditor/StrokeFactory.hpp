#pragma once

#include "StrokeComponent.hpp"
#include <Engine/RadiumEngine.hpp>
#include <Engine/Scene/Component.hpp>
#include <Engine/Scene/EntityManager.hpp>

class StrokeFactory
{
  public:
    static StrokeComponent* createStrokeComponent( Ra::Engine::Scene::Entity* e,
                                                   Ra::Core::Vector3Array strokePts ) {
        auto c = new StrokeComponent( e, strokePts );
        c->initialize();
        return c;
    }
    static Ra::Engine::Scene::Entity* createCurveEntity( std::vector<Ra::Core::Vector2f> polyline,
                                                         const std::string& name ) {
        auto engine                  = Ra::Engine::RadiumEngine::getInstance();
        Ra::Engine::Scene::Entity* e = engine->getEntityManager()->createEntity( name );
        Ra::Core::Vector3Array strokePts;
        for ( auto& pt : polyline ) {
            strokePts.push_back( Ra::Core::Vector3( pt.x(), 0, pt.y() ) );
        }
        createStrokeComponent( e, strokePts );
        return e;
    }

  private:
};
