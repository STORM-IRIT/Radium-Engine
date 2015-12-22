#ifndef ANIMPLUGIN_SKELETON_BONE_DRAWABLE_HPP_
#define ANIMPLUGIN_SKELETON_BONE_DRAWABLE_HPP_

#include "../AnimationPlugin.hpp"

#include <Core/Mesh/MeshUtils.hpp>
#include <Core/Animation/Handle/Skeleton.hpp>

#include <Engine/Entity/Entity.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>

#include "../AnimationComponent.hpp"

//#define K_VERSION

namespace AnimationPlugin
{
class SkeletonBoneRenderObject : public Ra::Engine::RenderObject
{
public:
    SkeletonBoneRenderObject(const std::string& name, AnimationComponent* comp, uint id, Ra::Engine::RenderObjectManager* roMgr);

    void update(); // Update local transform of the associated render object

    static Ra::Core::TriangleMesh makeBoneShape();

    uint getBoneIndex() const { return m_id;}

    void updateLocalTransform(Ra::Engine::RenderObject *ro);

private:
    const Ra::Core::Animation::Skeleton& m_skel;
    uint m_id;
    std::unique_ptr<Ra::Engine::RenderTechnique> m_renderParams;
    std::unique_ptr<Ra::Engine::Material> m_material;
    Ra::Engine::RenderObjectManager* m_roMgr;
};

}

#endif //ANIMPLUGIN_SKELETON_BONE_DRAWABLE_HPP_
