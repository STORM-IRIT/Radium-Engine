#ifndef RADIUMENGINE_TRANSFORM_EDITOR_HPP_
#define RADIUMENGINE_TRANSFORM_EDITOR_HPP_

#include <Core/Math/Types.hpp>
#include <Engine/ItemModel/ItemEntry.hpp>
#include <GuiBase/RaGuiBase.hpp>
namespace Ra {
namespace GuiBase {
/**
 * The TransformEditor allows to edit the transform of an object.
 */
class RA_GUIBASE_API TransformEditor {
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    TransformEditor() : m_transform( Core::Transform::Identity() ) {}

    virtual ~TransformEditor();

    /**
     * Change the current editable object.
     */
    virtual void setEditable( const Engine::ItemEntry& entry );

    /**
     * Retrieve the transform from the editable and update the editor.
     */
    virtual void updateValues() = 0;

  protected:
    /**
     * Return the transform property from the editable.
     */
    void getTransform();

    /**
     * Helper to set the transform to the editable.
     */
    void setTransform( const Ra::Core::Transform& tr );

    /**
     * Return true if the transform is editable.
     */
    bool canEdit() const;

    /**
     * Return the transform in world space.
     */
    Core::Transform getWorldTransform() const;

  protected:
    Core::Transform m_transform;     ///< The transform being edited.
    Engine::ItemEntry m_currentEdit; ///< The current item being edited.
};
} // namespace GuiBase
} // namespace Ra
#endif // RADIUMENGINE_TRANSFORM_EDITOR_HPP_
