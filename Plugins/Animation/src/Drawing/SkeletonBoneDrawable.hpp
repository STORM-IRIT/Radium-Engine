#ifndef ANIMPLUGIN_SKELETON_BONE_DRAWABLE_HPP_
#define ANIMPLUGIN_SKELETON_BONE_DRAWABLE_HPP_

#include <Core/Animation/Skeleton.hpp>
#include <Core/Geometry/MeshUtils.hpp>

#include <Engine/Entity/Entity.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>

#include <AnimationComponent.hpp>
#include <AnimationPlugin.hpp>

//#define K_VERSION

namespace AnimationPlugin {
class SkeletonBoneRenderObject {
  public:
    SkeletonBoneRenderObject( const std::string& name, AnimationComponent* comp, uint id,
                              Ra::Engine::RenderObjectManager* roMgr );

    void update(); // Update local transform of the associated render object

    static Ra::Core::Geometry::TriangleMesh makeBoneShape();

    uint getBoneIndex() const { return m_id; }

    Ra::Core::Container::Index getRenderObjectIndex() const { return m_roIdx; }

    void setXray( bool on );

  private:
    void updateLocalTransform();

  private:
    Ra::Core::Container::Index m_roIdx; /// Index of the corresponding render object
    uint m_id;               /// Bone index

    const Ra::Core::Animation::Skeleton& m_skel; /// Skeleton
    std::shared_ptr<Ra::Engine::RenderTechnique> m_renderParams;
    std::shared_ptr<Ra::Engine::Material> m_material;
    Ra::Engine::RenderObjectManager* m_roMgr;
};

} // namespace AnimationPlugin

#endif // ANIMPLUGIN_SKELETON_BONE_DRAWABLE_HPP_
