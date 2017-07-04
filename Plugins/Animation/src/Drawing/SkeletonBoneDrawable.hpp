#ifndef ANIMPLUGIN_SKELETON_BONE_DRAWABLE_HPP_
#define ANIMPLUGIN_SKELETON_BONE_DRAWABLE_HPP_

#include <Core/Mesh/MeshUtils.hpp>
#include <Core/Animation/Handle/Skeleton.hpp>

#include <Engine/Entity/Entity.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>

#include <AnimationPlugin.hpp>
#include <AnimationComponent.hpp>

//#define K_VERSION

namespace AnimationPlugin
{
class SkeletonBoneRenderObject
{
public:
    SkeletonBoneRenderObject(const std::string& name, AnimationComponent* comp, uint id, Ra::Engine::RenderObjectManager* roMgr);

    void update(); // Update local transform of the associated render object

    static Ra::Core::TriangleMesh makeBoneShape();

    uint getBoneIndex() const { return m_id; }

    Ra::Core::Index getRenderObjectIndex() const { return m_roIdx;}

    void setXray(bool on);

private:
    void updateLocalTransform();

private:
    Ra::Core::Index m_roIdx; /// Index of the corresponding render object
    uint m_id;               /// Bone index

    const Ra::Core::Animation::Skeleton& m_skel;    /// Skeleton
    std::shared_ptr<Ra::Engine::RenderTechnique> m_renderParams;
    std::shared_ptr<Ra::Engine::Material> m_material;
    Ra::Engine::RenderObjectManager* m_roMgr;
};

}

#endif //ANIMPLUGIN_SKELETON_BONE_DRAWABLE_HPP_
