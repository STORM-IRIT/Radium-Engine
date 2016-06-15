#ifndef RADIUMENGINE_GIZMO_MANAGER_HPP_
#define RADIUMENGINE_GIZMO_MANAGER_HPP_

#include <QObject>
#include <QMouseEvent>
#include <GuiBase/TransformEditor/TransformEditor.hpp>
#include <MainApplication/Viewer/Gizmo/Gizmo.hpp>

namespace Ra { namespace Engine { struct ItemEntry;}}
namespace Ra
{
    namespace Gui
    {
        /// This class interfaces the gizmos with the ui commands.
        /// It allows to change the gizmo type when editing an editable transform object
        class GizmoManager : public QObject, public GuiBase::TransformEditor
        {
            Q_OBJECT

        public:

            RA_CORE_ALIGNED_NEW;
            enum GizmoType
            {
                NONE,
                TRANSLATION,
                ROTATION,
                SCALE,
            };


            GizmoManager(QObject* parent = nullptr);
            ~GizmoManager();


        public:
            /// Receive mouse events and transmit them to the gizmos.
            virtual bool handleMousePressEvent  ( QMouseEvent* event );
            virtual bool handleMouseReleaseEvent( QMouseEvent* event );
            virtual bool handleMouseMoveEvent   ( QMouseEvent* event );


        public slots:
            void setEditable( const Engine::ItemEntry& ent ) override;

            /// Callback when a drawable is picked.
            void handlePickingResult( int drawableId );

            /// Change mode from local axis to global
            void setLocal(bool useLocal);

            /// Change gizmo type (rotation or translation)
            void changeGizmoType( GizmoType type );

            /// Retrieve the transform from the editable and update the gizmos.
            void updateValues() override;

        private:

            // Helper factory method to create the right gizmo.
            void spawnGizmo();

        private:
            std::unique_ptr<Gizmo> m_currentGizmo;  //! Owning pointer to the gizmo
            GizmoType m_currentGizmoType;           //! Type of the gizmo
            Gizmo::Mode m_mode;                     //! Local/global axis mode.
        };
    }
}


#endif // RADIUMENGINE_GIZMO_MANAGER_HPP_
