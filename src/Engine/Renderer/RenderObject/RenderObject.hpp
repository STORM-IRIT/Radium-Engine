#ifndef RADIUMENGINE_RENDEROBJECT_HPP
#define RADIUMENGINE_RENDEROBJECT_HPP

#include <string>
#include <mutex>
#include <memory>

#include <Core/CoreMacros.hpp>
#include <Core/Index/IndexedObject.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Engine/Renderer/RenderTechnique/Material.hpp>

namespace Ra { namespace Engine { class Light;            } }
namespace Ra { namespace Engine { struct RenderTechnique; } }
namespace Ra { namespace Engine { class Component;        } }
namespace Ra { namespace Engine { class Mesh;             } }
namespace Ra { namespace Engine { class RenderQueue;      } }

namespace Ra { namespace Engine {

// FIXME(Charly): Does this need a bit of cleanup ?
class RA_API RenderObject : public Core::IndexedObject
{
public:
    enum RenderObjectType
    {
        RO_OPAQUE = 0,
        RO_TRANSPARENT = 1,
        RO_DEBUG = 2
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
	void feedRenderQueue(RenderQueue& queue,
						 const Core::Matrix4& view,
						 const Core::Matrix4& proj);

    RenderObject* clone();

    void setVisible(bool visible);
    bool isVisible() const;

    bool isDirty() const;

    void setRenderTechnique(RenderTechnique* technique);
    RenderTechnique* getRenderTechnique() const;

    void setMesh(Mesh* mesh);
    Mesh* getMesh() const;

private:
    std::string m_name;
    uint m_type;

    Component* m_component;

    bool m_visible;
    bool m_isDirty;

    RenderTechnique* m_renderTechnique;
    Mesh* m_mesh;

    mutable std::mutex m_updateMutex;
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/RenderObject/RenderObject.inl>

#endif // RADIUMENGINE_RENDEROBJECT_HPP
