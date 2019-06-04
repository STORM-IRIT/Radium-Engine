#ifndef SKINPLUGIN_SKINNING_COMPONENT_HPP_
#define SKINPLUGIN_SKINNING_COMPONENT_HPP_

#include <SkinningPluginMacros.hpp>

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

namespace SkinningPlugin {

/// The SkinningComponent class is responsible for applying Geometric Skinning Methods
/// on an animated object's mesh.
class SKIN_PLUGIN_API SkinningComponent : public Ra::Engine::Component {
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

    /// Loads the skinning weights from the given Handledata.
    // TODO: for now, weights are stored in the AnimationComponent.
    virtual void handleWeightsLoading( const Ra::Core::Asset::HandleData* data );

    /// @returns the reference skinning data.
    const Ra::Core::Skinning::RefData* getRefData() const { return &m_refData; }

    /// @returns the current Pose data.
    const Ra::Core::Skinning::FrameData* getFrameData() const { return &m_frameData; }

    /// @returns the list of DualQuaternions used for DQS.
    const Ra::Core::AlignedStdVector<Ra::Core::DualQuaternion>* getDQ() const { return &m_DQ; }

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
    std::string m_contentsName;

  private:
    /// The refrence Skinning data.
    Ra::Core::Skinning::RefData m_refData;

    /// The current Pose data.
    Ra::Core::Skinning::FrameData m_frameData;

    /// Getter for the animation skeletton.
    Ra::Engine::ComponentMessenger::CallbackTypes<Ra::Core::Animation::Skeleton>::Getter
        m_skeletonGetter;

    /// Getter/Setter for the mesh vertices.
    Ra::Engine::ComponentMessenger::CallbackTypes<Ra::Core::Vector3Array>::ReadWrite
        m_verticesWriter;

    /// Getter/Setter for the mesh normals.
    Ra::Engine::ComponentMessenger::CallbackTypes<Ra::Core::Vector3Array>::ReadWrite
        m_normalsWriter;

    // Read FMC's RO idx.
    Ra::Engine::ComponentMessenger::CallbackTypes<Ra::Core::Utils::Index>::Getter
        m_renderObjectReader;

    // Getter/Setter to the mesh
    Ra::Engine::ComponentMessenger::CallbackTypes<Ra::Core::Geometry::TriangleMesh>::Getter
        m_meshWritter;

    /// The Skinning Method.
    SkinningType m_skinningType;

    /// Are all the required data available.
    bool m_isReady;

    /// Whether skinning is mandatory for the current frame.
    bool m_forceUpdate;

    /// The list of DualQuaternions used for DQS.
    Ra::Core::AlignedStdVector<Ra::Core::DualQuaternion> m_DQ;

    /// The duplicate vertices map, used to recompute smooth normals.
    std::vector<Ra::Core::Utils::Index> m_duplicatesMap;

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
} // namespace SkinningPlugin

#endif //  SKINPLUGIN_SKINNING_COMPONENT_HPP_
