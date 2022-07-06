#pragma once

#include <Engine/Scene/Component.hpp>
#include "CurveComponent.hpp"
#include <Engine/Scene/EntityManager.hpp>

class CurveFactory
{
public:
    static CurveComponent* createCurveComponent(Ra::Engine::Scene::Entity* e, 
                                                Ra::Core::Vector3Array ctrlPts,
                                                const std::string& name ){
        auto c = new CurveComponent(e, ctrlPts, name);
        c->initialize();
        return c;
    }
    static Ra::Engine::Scene::Entity* createCurveEntity( Ra::Core::Vector3Array ctrlPts, const std::string& name ){
        auto engine = Ra::Engine::RadiumEngine::getInstance();
        Ra::Engine::Scene::Entity* e = engine->getEntityManager()->createEntity( name );
        createCurveComponent(e, ctrlPts, name);
        return e;
    }

private:

};