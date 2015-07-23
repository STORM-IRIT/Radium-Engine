#ifndef RADIUMENGINE_DRAWABLE_HPP
#define RADIUMENGINE_DRAWABLE_HPP

#include <string>
#include <mutex>

#include <Core/Math/Matrix.hpp>

namespace Ra { namespace Engine { class Light;         } }
namespace Ra { namespace Engine { class Material;      } }
namespace Ra { namespace Engine { class ShaderProgram; } }
namespace Ra { namespace Engine { class Component;     } }

namespace Ra { namespace Engine {

class Drawable
{
public:
    // FIXME(Charly): Set component in the constructor ?
    explicit Drawable(const std::string& name) :
        m_name(name), m_material(nullptr), m_isDirty(true) {}
    ~Drawable() = default;

    virtual void setComponent(Component* component) final;

    virtual const std::string& getName() const final { return m_name; }

    virtual void updateGL() = 0;

    virtual void draw(const Core::Matrix4& view,
                      const Core::Matrix4& proj,
                      ShaderProgram* shader) = 0;
    virtual void draw(const Core::Matrix4& view,
                      const Core::Matrix4& proj,
                      Light* light) = 0;

    virtual void setVisible(bool visible) final;
    virtual bool isVisible() const final;

    virtual bool isDirty() const final;

protected:
    std::string m_name;
    Component* m_component;

    bool m_visible;
    mutable std::mutex m_visibleMutex;

    Material* m_material;

    bool m_isDirty;
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Drawable/Drawable.inl>

#endif // RADIUMENGINE_DRAWABLE_HPP
