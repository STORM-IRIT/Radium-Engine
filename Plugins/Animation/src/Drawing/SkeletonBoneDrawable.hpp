#ifndef ANIMPLUGIN_SKELETON_BONE_DRAWABLE_HPP_
#define ANIMPLUGIN_SKELETON_BONE_DRAWABLE_HPP_

#include <Core/Animation/Handle/Skeleton.hpp>
#include <Core/Mesh/MeshUtils.hpp>

#include <Engine/Entity/Entity.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>

#include <AnimationComponent.hpp>
#include <AnimationPlugin.hpp>

//#define K_VERSION

namespace AnimationPlugin {

/// The SkeletonBoneRenderObject class manages the display of an animation bone.
class SkeletonBoneRenderObject {
  public:
    SkeletonBoneRenderObject( const std::string& name, AnimationComponent* comp, uint id,
                              Ra::Engine::RenderObjectManager* roMgr );

    /// Update internal data.
    void update();

    /// Creates a mesh used to display a bone.
    static Ra::Core::TriangleMesh makeBoneShape();

    /// @returns the index of the bone represented by this SkeletonBoneRenderObject.
    uint getBoneIndex() const { return m_id; }

    /// @returns the index of the RO used to render the bone.
    Ra::Core::Index getRenderObjectIndex() const { return m_roIdx; }

    /// Toggle on/off using xray to display the bone.
    void setXray( bool on );

  private:
    /// Update local transform of the associated render object.
    void updateLocalTransform();

  private:
    /// Index of the corresponding render object
    Ra::Core::Index m_roIdx;

    /// Bone index
    uint m_id;

    /// Skeleton
    const Ra::Core::Animation::Skeleton& m_skel;

    /// Render Parameters for bone display.
    std::shared_ptr<Ra::Engine::RenderTechnique> m_renderParams;

    /// Material for bone display.
    std::shared_ptr<Ra::Engine::Material> m_material;

    /// RO for bone display.
    Ra::Engine::RenderObjectManager* m_roMgr;
};

} // namespace AnimationPlugin

#endif // ANIMPLUGIN_SKELETON_BONE_DRAWABLE_HPP_
