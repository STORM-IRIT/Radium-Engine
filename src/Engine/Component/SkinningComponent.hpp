#ifndef SKELETONBASEDANIMATIONPLUGIN_SKINNINGCOMPONENT_HPP_
#define SKELETONBASEDANIMATIONPLUGIN_SKINNINGCOMPONENT_HPP_

#include <SkeletonBasedAnimationPluginMacros.hpp>

#include <Core/Animation/HandleWeight.hpp>
#include <Core/Animation/Pose.hpp>
#include <Core/Animation/SkinningData.hpp>
#include <Core/Asset/HandleData.hpp>
#include <Core/Geometry/TriangleMesh.hpp>
#include <Core/Math/DualQuaternion.hpp>
#include <Core/Utils/Index.hpp>

#include <Engine/Component/Component.hpp>
#include <Engine/Managers/ComponentMessenger/ComponentMessenger.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>

namespace SkeletonBasedAnimationPlugin {

/// The SkinningComponent class is responsible for applying Geometric Skinning Methods
/// on an animated object's mesh.
class SKEL_ANIM_PLUGIN_API SkinningComponent : public Ra::Engine::Component
{
  public:
    /// The Geometric Skinning Method.
    enum SkinningType {
        LBS = 0,  ///< Linear Blend Skinning
        DQS,      ///< Dual Quaternion Skinning
        COR,      ///< Center of Rotation skinning
        STBS_LBS, ///< Stretchable Twistable Bone Skinning with LBS
        STBS_DQS  ///< Stretchable Twistable Bone Skinning with DQS
    };

    SkinningComponent( const std::string& name, SkinningType type, Ra::Engine::Entity* entity ) :
        Component( name, entity ),
        m_skinningType( type ),
        m_isReady( false ),
        m_forceUpdate( false ),
        m_weightBone( 0 ),
        m_weightType( 0 ),
        m_showingWeights( false ) {}

    virtual ~SkinningComponent() {}

    virtual void initialize() override;

    /// Apply the Skinning Method.
    void skin();

    /// Update internal data and apply postprocesses to the mesh, e.g. normal computation.
    void endSkinning();

    /// Sets the Skinning method to use.
    void setSkinningType( SkinningType type );

    /// \returns the current skinning method.
    inline SkinningType getSkinningType() const { return m_skinningType; }

    /// Loads the skinning data from the given Handledata.
    /// \note Call initialize() afterwards to finalize data registration.
    void handleSkinDataLoading( const Ra::Core::Asset::HandleData* data,
                                const std::string& meshName,
                                const Ra::Core::Transform& meshFrame );

    /// @returns the reference skinning data.
    const Ra::Core::Skinning::RefData* getRefData() const { return &m_refData; }

    /// @returns the current Pose data.
    const Ra::Core::Skinning::FrameData* getFrameData() const { return &m_frameData; }

    const std::string getMeshName() const;
    const std::string getSkeletonName() const;

    /// Toggles display of skinning weights.
    void showWeights( bool on );

    /// Set the type of skinning weight to display:
    ///  - 0 for standard skinning weights
    ///  - 1 for stbs weights
    void showWeightsType( int type );

    /// Set the bone to show the weights of.
    void setWeightBone( uint bone );

  public:
    /// Registers the Entity name for Component communication (out).
    void setupIO( const std::string& id );

    /// Computes internal data related to the Skinning method.
    void setupSkinningType( SkinningType type );

    /// Registers the Entity name for Component communication (in/out).
    void setContentsName( const std::string& name );

  public:
    /// The Entity name for Component communication.
    std::string m_skelName;

  private:
    // Internal function to create the skinning weights.
    void createWeightMatrix();

    /// Skinning Weight Matrix getter for CC.
    const Ra::Core::Animation::WeightMatrix* getWeightsOutput() const;

    /// Applies the bones bindMatrices to the given skeleton pose.
    void applyBindMatrices( Ra::Core::Animation::Pose& pose ) const;

  private:
    /// The mesh name for Component communication.
    std::string m_meshName;

    /// Inverse of the initial mesh transform.
    Ra::Core::Transform m_meshFrameInv;

    /// The refrence Skinning data.
    Ra::Core::Skinning::RefData m_refData;

    /// The current Pose data.
    Ra::Core::Skinning::FrameData m_frameData;

    /// Getter for the animation skeletton.
    Ra::Engine::ComponentMessenger::CallbackTypes<Ra::Core::Animation::Skeleton>::Getter
        m_skeletonGetter;

    /// Read FMC's RO idx.
    Ra::Engine::ComponentMessenger::CallbackTypes<Ra::Core::Utils::Index>::Getter
        m_renderObjectReader;

    /// Getter/Setter to the mesh
    Ra::Engine::ComponentMessenger::CallbackTypes<Ra::Core::Geometry::TriangleMesh>::ReadWrite
        m_meshWriter;

    /// The Skinning Method.
    SkinningType m_skinningType;

    /// Are all the required data available.
    bool m_isReady;

    /// Whether skinning is mandatory for the current frame.
    bool m_forceUpdate;

    /// The duplicate vertices map, used to recompute smooth normals.
    // FIXME: implement proper normal skinning such as http://vcg.isti.cnr.it/deformFactors/
    std::vector<Ra::Core::Utils::Index> m_duplicatesMap;

    /// The skinning weights, stored per bone.
    /// \note These are stored this way because we cannot build the weight matrix
    ///       without data from other components (skeleton).
    std::map<std::string, std::vector<std::pair<uint, Scalar>>> m_loadedWeights;
    /// The bind matrices, stored per bone.
    /// \note These are stored this way because we cannot fill m_refData
    ///       without data from other components (skeleton).
    std::map<std::string, Ra::Core::Transform> m_loadedBindMatrices;

    /// The STBS weights.
    Ra::Core::Animation::WeightMatrix m_weightSTBS;

    /// Initial RO shader config when not showing skinning weights.
    std::shared_ptr<Ra::Engine::RenderTechnique> m_baseTechnique;
    std::shared_ptr<Ra::Engine::RenderTechnique> m_weightTechnique;
    Ra::Core::Vector3Array m_baseUV;
    Ra::Core::Vector3Array m_weightsUV;
    uint m_weightBone;
    uint m_weightType;
    bool m_showingWeights;
};
} // namespace SkeletonBasedAnimationPlugin

#endif //  SKELETONBASEDANIMATIONPLUGIN_SKINNINGCOMPONENT_HPP_
