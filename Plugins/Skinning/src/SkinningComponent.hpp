#ifndef SKINPLUGIN_SKINNING_COMPONENT_HPP_
#define SKINPLUGIN_SKINNING_COMPONENT_HPP_

#include <SkinningPluginMacros.hpp>

#include <Core/Animation/Handle/HandleWeight.hpp>
#include <Core/Animation/Pose/Pose.hpp>
#include <Core/Animation/Skinning/SkinningData.hpp>
#include <Core/File/HandleData.hpp>
#include <Core/Index/Index.hpp>
#include <Core/Math/DualQuaternion.hpp>
#include <Core/Geometry/TriangleMesh.hpp>

#include <Engine/Component/Component.hpp>
#include <Engine/Managers/ComponentMessenger/ComponentMessenger.hpp>

namespace SkinningPlugin {

/// The SkinningComponent class is responsible for applying Geometric Skinning Methods
/// on an animated object's mesh.
class SKIN_PLUGIN_API SkinningComponent : public Ra::Engine::Component {
  public:
    /// The Geometric Skinning Method.
    enum SkinningType {
        LBS = 0, ///< Linear Blend Skinning
        DQS,     ///< Dual Quaternion Skinning
        COR      ///< Center of Rotation skinning
    };

    SkinningComponent( const std::string& name, SkinningType type, Ra::Engine::Entity* entity ) :
        Component( name, entity ),
        m_skinningType( type ),
        m_isReady( false ) {}

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
    virtual void handleWeightsLoading( const Ra::Asset::HandleData* data );

    /// @returns the reference skinning data.
    const Ra::Core::Skinning::RefData* getRefData() const { return &m_refData; }

    /// @returns the current Pose data.
    const Ra::Core::Skinning::FrameData* getFrameData() const { return &m_frameData; }

    /// @returns the list of DualQuaternions used for DQS.
    const Ra::Core::AlignedStdVector<Ra::Core::DualQuaternion>* getDQ() const { return &m_DQ; }

  public:
    /// Registers the Entity name for Component communication (out).
    void setupIO( const std::string& id );

    /// Computes internal data related to the Skinning method.
    void setupSkinningType( SkinningType type );

    /// Registers the Entity name for Component communication (in/out).
    void setContentsName( const std::string name );

  private:
    /// The Entity name for Component communication.
    std::string m_contentsName;

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

    /// The Skinning Method.
    SkinningType m_skinningType;

    /// Are all the required data available.
    bool m_isReady;

    /// The list of DualQuaternions used for DQS.
    Ra::Core::AlignedStdVector<Ra::Core::DualQuaternion> m_DQ;

    /// The duplicate vertices map, used to recompute smooth normals.
    std::vector<Ra::Core::Index> m_duplicatesMap;
};
} // namespace SkinningPlugin

#endif //  SKINPLUGIN_SKINNING_COMPONENT_HPP_
