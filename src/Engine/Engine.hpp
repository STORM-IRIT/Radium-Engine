#ifndef RADIUMENGINE_ENGINE_HPP
#define RADIUMENGINE_ENGINE_HPP

#include <mutex>
#include <map>
#include <string>
#include <memory>

#include <Core/CoreMacros.hpp>

namespace Ra { namespace Engine { class System; } }
namespace Ra { namespace Engine { class Entity; } }

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

private:
    void run();
    bool quitRequested();

private:
    bool m_quit;
    std::mutex m_quitMutex;

    std::map<std::string, std::shared_ptr<System>> m_systems;

    Entity* m_entity;
    Scalar m_angle;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_ENGINE_HPP
