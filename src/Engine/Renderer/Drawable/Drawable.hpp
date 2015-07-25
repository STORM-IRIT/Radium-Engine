#ifndef RADIUMENGINE_DRAWABLE_HPP
#define RADIUMENGINE_DRAWABLE_HPP

#include <string>
#include <mutex>

#include <Core/Math/Matrix.hpp>
#include <Engine/Renderer/Material/Material.hpp>

namespace Ra { namespace Engine { class Light;         } }
namespace Ra { namespace Engine { class ShaderProgram; } }
namespace Ra { namespace Engine { class Component;     } }

namespace Ra { namespace Engine {

class Drawable
{
public:
    // FIXME(Charly): Set component in the constructor ?
	explicit Drawable(const std::string& name);
    virtual ~Drawable();

    virtual void setComponent(Component* component) final;

	virtual const std::string& getName() const final;

    virtual void updateGL() final;

    virtual void draw(const Core::Matrix4& view,
                      const Core::Matrix4& proj,
                      ShaderProgram* shader) = 0;
    virtual void draw(const Core::Matrix4& view,
                      const Core::Matrix4& proj,
                      Light* light) = 0;

	virtual Drawable* clone() final;

    virtual void setVisible(bool visible) final;
    virtual bool isVisible() const final;

    virtual bool isDirty() const final;

	virtual void setMaterial(Material* material);
	virtual void setMaterial(std::shared_ptr<Material> material);

protected:
	virtual void updateGLInternal() = 0;
	virtual Drawable* cloneInternal() = 0;

protected:
    std::string m_name;
    Component* m_component;

	mutable std::mutex m_updateMutex;

    bool m_visible;
    mutable std::mutex m_visibleMutex;

    std::shared_ptr<Material> m_material;

    bool m_isDirty;
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Drawable/Drawable.inl>

#endif // RADIUMENGINE_DRAWABLE_HPP
