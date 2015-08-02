#ifndef RADIUMENGINE_RENDEROBJECT_HPP
#define RADIUMENGINE_RENDEROBJECT_HPP

#include <string>
#include <mutex>
#include <memory>

#include <Core/Index/IndexedObject.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Engine/Renderer/Material/Material.hpp>

namespace Ra { namespace Engine { class Light;         } }
namespace Ra { namespace Engine { class ShaderProgram; } }
namespace Ra { namespace Engine { class Component;     } }
namespace Ra { namespace Engine { class Mesh;          } }
namespace Ra { namespace Engine { class RenderQueue;   } }

namespace Ra { namespace Engine {

// FIXME(Charly): Does this need a bit of cleanup ?
class RA_API RenderObject : public Core::IndexedObject
{
public:
    enum RenderObjectType
    {
        OPAQUE = 0,
        TRANSPARENT = 1,
        DEBUG = 2
    };

public:
    // FIXME(Charly): Set component in the constructor ?
    explicit RenderObject(const std::string& name);
    ~RenderObject();

    void setRenderObjectType(uint type);
    uint getRenderObjectType() const;

    void setComponent(Component* component);

    const std::string& getName() const;

    void updateGL();
    void feedRenderQueue(RenderQueue* queue);

    RenderObject* clone();

    void setVisible(bool visible);
    bool isVisible() const;

    bool isDirty() const;

    void setShader(ShaderProgram* shader);
    ShaderProgram* getShader() const;

    void setMaterial(Material* material);
    Material* getMaterial() const;

    void setMesh(Mesh* mesh);
    Mesh* getMesh() const;

private:
    std::string m_name;
    uint m_type;

    Component* m_component;

    bool m_visible;
    bool m_isDirty;

    ShaderProgram* m_shader;
    Material* m_material;
    Mesh* m_mesh;

    mutable std::mutex m_updateMutex;
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/RenderObject/RenderObject.inl>

#endif // RADIUMENGINE_RENDEROBJECT_HPP
