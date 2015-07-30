#ifndef FANCYMESHPLUGIN_FANCYMESHCOMPONENT_HPP
#define FANCYMESHPLUGIN_FANCYMESHCOMPONENT_HPP

#include <Engine/Entity/Component.hpp>
#include <Engine/Renderer/FancyMeshPlugin/FancyMeshLoadingData.hpp>

namespace Ra { namespace Engine { class Material; } }

// FIXME(Charly): Do we really want this to be in Engine namespace ? 
namespace Ra { namespace Engine {

class RA_API FancyMeshComponent : public Component
{
public:
    FancyMeshComponent(const std::string& name);
	virtual ~FancyMeshComponent();

	virtual void initialize() override;

    void addMeshDrawable(const Core::TriangleMesh& mesh, const std::string& name);
    void addMeshDrawable(const Core::TriangleMesh& mesh, const std::string& name, Material* material);

    void handleMeshLoading(const FancyComponentData& data);

private:
	Core::Index m_drawable;
};

} // namespace Engine
} // namespace Engine

#endif // FANCYMESHPLUGIN_FANCYMESHCOMPONENT_HPP
