#ifndef ANIMPLUGIN_SKELETON_BONE_DRAWABLE_HPP_
#define ANIMPLUGIN_SKELETON_BONE_DRAWABLE_HPP_

#include <Core/Animation/Skeleton.hpp>

#include <Engine/Entity/Entity.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>

#include <AnimationComponent.hpp>
#include <AnimationPlugin.hpp>

namespace AnimationPlugin {

/**
 * The SkeletonBoneRenderObject class manages the display of an animation bone.
 */
class SkeletonBoneRenderObject {
  public:
    SkeletonBoneRenderObject( const std::string& name, AnimationComponent* comp, uint id,
                              Ra::Engine::RenderObjectManager* roMgr );

    /**
     * Update internal data.
     */
    void update();

    /**
     * Returns the index of the bone.
     */
    uint getBoneIndex() const { return m_id; }

    /**
     * Returns the index of the RO used to render the bone.
     */
    Ra::Core::Utils::Index getRenderObjectIndex() const { return m_roIdx; }

    /**
     * Toggle on/off using xray to display the bone.
     */
    void setXray( bool on );

    /**
     * Creates a mesh used to display a bone.
     */
    static Ra::Core::Geometry::TriangleMesh makeBoneShape();

  private:
    /**
     * Update local transform of the associated render object.
     */
    void updateLocalTransform();

  private:
    /// The index of the RenderObject.
    Ra::Core::Utils::Index m_roIdx;

    /// The index of the bone.
    uint m_id;

    /// The animation skeleton.
    const Ra::Core::Animation::Skeleton& m_skel;

    /// The RenderObject manager.
    Ra::Engine::RenderObjectManager* m_roMgr;
};

} // namespace AnimationPlugin

#endif // ANIMPLUGIN_SKELETON_BONE_DRAWABLE_HPP_
