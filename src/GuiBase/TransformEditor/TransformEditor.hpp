#pragma once

#include <Core/Types.hpp>
#include <Engine/Scene/ItemEntry.hpp>
#include <GuiBase/RaGuiBase.hpp>
namespace Ra {
namespace GuiBase {
class RA_GUIBASE_API TransformEditor
{
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    TransformEditor() : m_transform( Core::Transform::Identity() ) {}
    virtual ~TransformEditor();

    /// Change the current editable object,
    virtual void setEditable( const Engine::ItemEntry& entry );

    /// Retrieve the transform from the editable and update the editor.
    virtual void updateValues() = 0;

  protected:
    // Helper to get the transform property from the editable.
    void getTransform();

    /// Helper to set the transform to the editable.
    void setTransform( const Ra::Core::Transform& tr );

    bool canEdit() const;

    Core::Transform getWorldTransform() const;

  protected:
    Core::Transform m_transform;     //! The transform being edited.
    Engine::ItemEntry m_currentEdit; //! The current item being edited.
};
} // namespace GuiBase
} // namespace Ra
