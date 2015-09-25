#ifndef RADIUMENGINE_GIZMO_MANAGER_HPP_
#define RADIUMENGINE_GIZMO_MANAGER_HPP_

#include <QObject>
#include <QMouseEvent>
#include <Engine/Entity/Component.hpp>
#include <MainApplication/Viewer/Gizmo/Gizmo.hpp>
#include <Engine/SystemDisplay/SystemDisplay.hpp>

namespace Ra
{
    namespace Gui
    {
        // Todo : check if we can't refactor with PropertyEditorWidget.
        class GizmoManager : public QObject
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
            virtual bool handleMousePressEvent( QMouseEvent* event );
            virtual bool handleMouseReleaseEvent( QMouseEvent* event );
            virtual bool handleMouseMoveEvent( QMouseEvent* event );


        public slots:

            void setEditable(Engine::EditableInterface* edit);
            void handlePickingResult( int drawableId );
            void setLocal(bool useLocal);
            void changeGizmoType( GizmoType type );
            void updateValues();


        private:
            void getTransform();
            void spawnGizmo();

        private:
            Core::Transform m_transform;

            Engine::EditableInterface* m_currentEdit;
            Engine::EditableProperty m_transformProperty;

            std::unique_ptr<Gizmo> m_currentGizmo;
            GizmoType m_currentGizmoType;
            Gizmo::Mode m_mode;

            bool m_dragMode;
        };
    }
}


#endif // RADIUMENGINE_GIZMO_MANAGER_HPP_
