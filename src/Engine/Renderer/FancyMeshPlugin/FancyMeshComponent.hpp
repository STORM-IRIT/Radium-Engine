#ifndef FANCYMESHPLUGIN_FANCYMESHCOMPONENT_HPP
#define FANCYMESHPLUGIN_FANCYMESHCOMPONENT_HPP

#include <Engine/Entity/Component.hpp>
#include <Engine/Renderer/FancyMeshPlugin/FancyMeshLoadingData.hpp>

// FIXME(Charly): Do we really want this to be in Engine namespace ? 
namespace Ra { namespace Engine {

class FancyMeshComponent : public Component
{
public:
    FancyMeshComponent(const std::string& name);
	virtual ~FancyMeshComponent();

	virtual void initialize() override;
	virtual void update(Scalar dt) override;

	void handleMeshLoading(const FancyComponentData& data);

private:
	Core::Index m_drawable;
};

} // namespace Engine
} // namespace Engine

#endif // FANCYMESHPLUGIN_FANCYMESHCOMPONENT_HPP
