#pragma once

#include <Engine/ItemModel/ItemEntry.hpp>
#include <Engine/RaEngine.hpp>
#include <Engine/System/System.hpp>

namespace Ra {
namespace Engine {

/**
 * The SkeletonBasedAnimationSystem manages both SkeletonComponents and SkinningComponents.
 * It is also responsible for transmitting calls to/from animation-related processes.
 */
class RA_ENGINE_API SkeletonBasedAnimationSystem : public Ra::Engine::System
{
  public:
    /// Create a new animation system
    SkeletonBasedAnimationSystem();

    SkeletonBasedAnimationSystem( const SkeletonBasedAnimationSystem& ) = delete;
    SkeletonBasedAnimationSystem& operator=( const SkeletonBasedAnimationSystem& ) = delete;

    /// \name System Interface
    /// \{

    /**
     * \brief Creates tasks for skeleton-based animation.
     *
     * There is one task per SkeletonComponent to update the skeleton display,
     * which is given the name "AnimatorTask_" + <skeleton_name>.
     * There are two tasks per SkinningComponent:
     *  - one for applying skinning on the FrameData, which is given the name
     *    "SkinnerTask_" + <mesh_name>
     *  - one for updating the GeometryComponent's RenderObject from the
     *    FrameData (thus depending on the first one), which is given the name
     *    "SkinnerEndTask_" + <mesh_name>
     */
    void generateTasks( Ra::Core::TaskQueue* taskQueue,
                        const Ra::Engine::FrameInfo& frameInfo ) override;

    /**
     * Loads Skeletons and Animations from a file data into the given Entity,
     * as well as the skinning data.
     *
     * Skeletons and animations are loaded into SkeletonComponents given names
     * "AC_" + <skeleton_name>.
     * Skinning data are loaded into SkinningComponents given names
     * "SkC_" + <mesh_name>.
     */
    void handleAssetLoading( Ra::Engine::Entity* entity,
                             const Ra::Core::Asset::FileData* fileData ) override;
    /// \}

    /// \name AbstractTimedSystem Interface
    /// \{

    // void goTo( Scalar t ) override;

    // void cacheFrame( const std::string& dir, uint frameID ) const override;

    // bool restoreFrame( const std::string& dir, uint frameID ) override;
    /// \}

    /// \name Skeleton display
    /// \{

    /**
     * Sets bone display xray mode to \p on for all SkeletonComponents.
     */
    void setXray( bool on );

    /**
     * \returns true if bone display xray mode on, false otherwise.
     */
    bool isXrayOn();

    /**
     * Toggles skeleton display for all SkeletonComponents.
     */
    void toggleSkeleton( const bool status );
    /// \}

    /// \name Animation parameters
    /// \{

    /**
     * Toggles the use of the animation timestep for all SkeletonComponents.
     */
    void toggleAnimationTimeStep( const bool status );

    /**
     * Sets the animation speed factor for all SkeletonComponents.
     */
    void setAnimationSpeed( const Scalar value );

    /**
     * Toggles animation auto repeat for all SkeletonComponents.
     */
    void autoRepeat( const bool status );

    /**
     * Toggles animation ping-pong for all SkeletonComponents.
     */
    void pingPong( const bool status );

    /**
     * \returns the animation time of the SkeletonComponents corresponding to \p entry 's entity.
     */
    Scalar getAnimationTime( const Ra::Engine::ItemEntry& entry ) const;
    /// \}

    /**
     * Update the timeline w.r.t. the \p id-th animation of component \p comp.
     */
    void useAnim( class SkeletonComponent* comp, uint id );

    /// Enable display of skinning weights.
    void showWeights( bool on );

    /// Sets the type of skinning weights to display: 0 - standard, 1 - stbs.
    void showWeightsType( int type );

  private:
    /// True if we want to show xray-bones.
    bool m_xrayOn{false};
};

} // namespace Engine
} // namespace Ra
