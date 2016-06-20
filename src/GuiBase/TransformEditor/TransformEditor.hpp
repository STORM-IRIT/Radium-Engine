#ifndef RADIUMENGINE_TRANSFORM_EDITOR_HPP_
#define RADIUMENGINE_TRANSFORM_EDITOR_HPP_
#include <Core/Math/LinearAlgebra.hpp>
#include <Engine/ItemModel/ItemEntry.hpp>
namespace Ra
{
    namespace GuiBase
    {
        class TransformEditor
        {
        public:
            RA_CORE_ALIGNED_NEW

            TransformEditor() {}
            virtual ~TransformEditor();

            /// Change the current editable object,
            virtual void setEditable(const Engine::ItemEntry& entry);

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
            Core::Transform m_transform; //! The transform being edited.
            Engine::ItemEntry m_currentEdit; //! The current item being edited.
        };
    }
}
#endif //RADIUMENGINE_TRANSFORM_EDITOR_HPP_
