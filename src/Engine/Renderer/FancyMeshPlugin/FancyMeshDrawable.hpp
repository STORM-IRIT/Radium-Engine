#ifndef FANCYMESHPLUGIN_FANCYMESHDRAWABLE_HPP
#define FANCYMESHPLUGIN_FANCYMESHDRAWABLE_HPP

#include <map>
#include <vector>
#include <string>
#include <memory>

#include <Core/Math/Vector.hpp>
#include <Engine/Renderer/Drawable/Drawable.hpp>

namespace Ra { namespace Engine { class ShaderProgram; } }
namespace Ra { namespace Engine { class Material;      } }
namespace Ra { namespace Engine { class Light;         } }
namespace Ra { namespace Engine { class Mesh;          } }

// FIXME(Charly): Do we really want this to be in Engine namespace ? 
namespace Ra { namespace Engine {

class RA_API FancyMeshDrawable : public Drawable
{
public:
    FancyMeshDrawable(const std::string& name);
    virtual ~FancyMeshDrawable();

    virtual void draw(const Core::Matrix4& viewMatrix,
                      const Core::Matrix4& projMatrix,
                      ShaderProgram* shader) override;

    virtual void draw(const Core::Matrix4& viewMatrix,
                      const Core::Matrix4& projMatrix,
                      Light* light) override;

    /**
     * @brief Attach a drawable to a DrawableComponent.
     * The DrawableComponent takes drawable ownership.
     * @param drawable The drawable to attach.
     */
    void addMesh(Mesh* mesh);

    /**
     * @brief Remove a drawable.
     * This method also deletes the drawable since DrawableComponent
     * has drawables responsibility.
     * @param name Drawable to remove.
     */
    void removeMesh(Mesh* drawable);

    /**
     * @brief Remove a drawable given its name.
     * This method also deletes the drawable since DrawableComponent
     * has drawables responsibility.
     * @param name Name of the drawable to remove.
     */
    void removeMesh(const std::string& name);

    /**
     * @brief Get a drawable given its name.
     * @param name Name of the drawable to retrieve.
     * @return The corresponding drawable if found in the map, nullptr otherwise.
     */
    Mesh* getMesh(const std::string& name) const;

//    virtual void setSelected(bool selected);

protected:
	virtual void updateGLInternal() override;
	virtual Drawable* cloneInternal() override;

private:
	void recomputeBbox();

private:
    typedef std::pair<std::string, std::shared_ptr<Mesh>> MeshByName;
    std::map<std::string, std::shared_ptr<Mesh>> m_meshes;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_DRAWABLECOMPONENT_HPP
