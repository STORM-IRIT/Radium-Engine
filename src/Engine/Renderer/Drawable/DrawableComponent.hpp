#ifndef RADIUMENGINE_DRAWABLECOMPONENT_HPP
#define RADIUMENGINE_DRAWABLECOMPONENT_HPP

#include <map>
#include <string>
#include <memory>

#include <Core/Math/Matrix.hpp>
#include <Engine/Entity/Component.hpp>

namespace Ra { namespace Engine { class Drawable;      } }
namespace Ra { namespace Engine { class ShaderProgram; } }
namespace Ra { namespace Engine { class Material;      } }
namespace Ra { namespace Engine { class Light;         } }

namespace Ra { namespace Engine {

class DrawableComponent : public Component
{
public:
    DrawableComponent();
    virtual ~DrawableComponent();

    virtual void update() override {}
    virtual void update(Scalar dt) override {}

    void draw(const Core::Matrix4& viewMatrix,
              const Core::Matrix4& projMatrix,
              Light* light);

    /**
     * @brief Attach a drawable to a DrawableComponent.
     * The DrawableComponent takes drawable ownership.
     * @param drawable The drawable to attach.
     */
    void addDrawable(Drawable* drawable);

    /**
     * @brief Remove a drawable.
     * This method also deletes the drawable since DrawableComponent
     * has drawables responsibility.
     * @param name Drawable to remove.
     */
    void removeDrawable(Drawable* drawable);

    /**
     * @brief Remove a drawable given its name.
     * This method also deletes the drawable since DrawableComponent
     * has drawables responsibility.
     * @param name Name of the drawable to remove.
     */
    void removeDrawable(const std::string& name);

    /**
     * @brief Get a drawable given its name.
     * @param name Name of the drawable to retrieve.
     * @return The corresponding drawable if found in the map, nullptr otherwise.
     */
    Drawable* getDrawable(const std::string& name) const;

    /**
     * @brief Get all drawables from a DrawableComponent.
     * @return A vector containing all Drawables from a DrawableComponent.
     */
    std::vector<Drawable*> getDrawables() const;

    void setMaterial(Material* material);

private:
    typedef std::pair<std::string, std::shared_ptr<Drawable>> DrawableByName;
    std::map<std::string, std::shared_ptr<Drawable>> m_drawables;

    ShaderProgram* m_shaderProgram;
    Material* m_material;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_DRAWABLECOMPONENT_HPP
