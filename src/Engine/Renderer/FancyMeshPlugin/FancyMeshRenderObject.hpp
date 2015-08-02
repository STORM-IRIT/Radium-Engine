#ifndef FANCYMESHPLUGIN_FANCYMESHRENDEROBJECT_HPP
#define FANCYMESHPLUGIN_FANCYMESHRENDEROBJECT_HPP

#include <map>
#include <vector>
#include <string>
#include <memory>

#include <Core/Math/Vector.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>

namespace Ra { namespace Engine { class ShaderProgram; } }
namespace Ra { namespace Engine { class Material;      } }
namespace Ra { namespace Engine { class Light;         } }
namespace Ra { namespace Engine { class Mesh;          } }

// FIXME(Charly): Do we really want this to be in Engine namespace ? 
namespace Ra { namespace Engine {

class RA_API FancyMeshRenderObject : public RenderObject
{
public:
    FancyMeshRenderObject(const std::string& name);
    virtual ~FancyMeshRenderObject();

    virtual void draw(const Core::Matrix4& viewMatrix,
                      const Core::Matrix4& projMatrix,
                      ShaderProgram* shader) override;

    virtual void draw(const Core::Matrix4& viewMatrix,
                      const Core::Matrix4& projMatrix,
                      Light* light) override;

    void addMesh(Mesh* mesh);
    void removeMesh(Mesh* mesh);
    void removeMesh(const std::string& name);
    Mesh* getMesh(const std::string& name) const;

//    virtual void setSelected(bool selected);

protected:
	virtual void updateGLInternal() override;
    virtual RenderObject* cloneInternal() override;

private:
	void recomputeBbox();

private:
    typedef std::pair<std::string, std::shared_ptr<Mesh>> MeshByName;
    std::map<std::string, std::shared_ptr<Mesh>> m_meshes;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_RENDEROBJECTCOMPONENT_HPP
