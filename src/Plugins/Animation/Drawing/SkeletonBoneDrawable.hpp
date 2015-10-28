#ifndef ANIMPLUGIN_SKELETON_BONE_DRAWABLE_HPP_
#define ANIMPLUGIN_SKELETON_BONE_DRAWABLE_HPP_

#include <Plugins/Animation/AnimationPlugin.hpp>

#include <Core/Mesh/MeshUtils.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>

#include <Plugins/Animation/AnimationComponent.hpp>
#include <Core/Animation/Handle/Skeleton.hpp>

namespace AnimationPlugin
{
class SkeletonBoneRenderObject : public Ra::Engine::RenderObject
{
public:
    SkeletonBoneRenderObject(const std::string& name, AnimationComponent* comp, Ra::Core::Edge edge, Ra::Engine::RenderObjectManager* roMgr);
	void update(); // Update local transform of the associated render object
    
	static Ra::Core::TriangleMesh makeBoneShape();

    void updateLocalTransform(Ra::Engine::RenderObject *ro);
	
    Ra::Core::Index getIndex() const { return m_index;}

private:
    const Ra::Core::Animation::Skeleton& m_skel;
    Ra::Core::Edge m_edge;
    std::unique_ptr<Ra::Engine::RenderTechnique> m_renderParams;
    std::unique_ptr<Ra::Engine::Material> m_material;
    Ra::Core::Index m_index;
    Ra::Engine::RenderObjectManager* m_roMgr;
};

}

#endif //ANIMPLUGIN_SKELETON_BONE_DRAWABLE_HPP_
