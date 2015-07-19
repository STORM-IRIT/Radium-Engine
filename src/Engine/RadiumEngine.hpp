#ifndef RADIUMENGINE_ENGINE_HPP
#define RADIUMENGINE_ENGINE_HPP

#include <mutex>
#include <map>
#include <string>
#include <memory>
#include <vector>

#include <Core/CoreMacros.hpp>

namespace Ra { namespace Core   { class MouseEvent;         } }
namespace Ra { namespace Core   { class KeyEvent;           } }
namespace Ra { namespace Engine { class System;             } }
namespace Ra { namespace Engine { class Entity;             } }
namespace Ra { namespace Engine { class Component;          } }
namespace Ra { namespace Engine { class EntityManager;      } }
namespace Ra { namespace Engine { class ComponentManager;   } }

namespace Ra { namespace Engine {

class RadiumEngine
{
public:
    RadiumEngine();
    ~RadiumEngine() {}

    void start();

    void initialize();
    void setupScene();
    void cleanup();

    void quit();

    System* getSystem(const std::string& system) const;

    Entity* createEntity();
    void addComponent(Component* component, Entity* entity, const std::string& system);

    std::vector<Entity*> getEntities() const;

    bool loadFile(const std::string& file);

    bool handleMouseEvent(const Core::MouseEvent& event);
    bool handleKeyEvent(const Core::KeyEvent& event);

private:
    void run();
    bool quitRequested();

private:
    bool m_quit;
    mutable std::mutex m_quitMutex;
    mutable std::mutex m_managersMutex;

    std::map<std::string, std::shared_ptr<System>> m_systems;

    EntityManager* m_entityManager;
    ComponentManager* m_componentManager;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_ENGINE_HPP
