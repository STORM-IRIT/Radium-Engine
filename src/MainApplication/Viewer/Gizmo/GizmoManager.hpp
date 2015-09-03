#ifndef RADIUMENGINE_GIZMO_MANAGER_HPP_
#define RADIUMENGINE_GIZMO_MANAGER_HPP_

#include <QObject>
#include <Engine/Entity/Component.hpp>
#include <MainApplication/Viewer/Gizmo/Gizmo.hpp>
#include <Engine/DebugDisplay/DebugDisplay.hpp>

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

            enum Mode
            {
                LOCAL,
                GLOBAL,
            };

            GizmoManager(QObject* parent = nullptr);
            ~GizmoManager();

        public slots:

            void setEditable(Engine::EditableInterface* edit);
            void changeMode( Mode mode );
            void changeGizmoType( GizmoType type );
            void updateValues();


        private:
            void getTransform();
            void spawnGizmo();

        private:
            Core::Transform m_transform;
            Engine::EditableInterface* m_currentEdit;
            std::unique_ptr<Gizmo> m_currentGizmo;
            GizmoType m_currentGizmoType;
            Mode m_mode;

        };
    }
}


#endif // RADIUMENGINE_GIZMO_MANAGER_HPP_
