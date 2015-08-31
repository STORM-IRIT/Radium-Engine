#ifndef RADIUMENGINE_GIZMO_MANAGER_HPP_
#define RADIUMENGINE_GIZMO_MANAGER_HPP_

#include <QObject>
#include <Engine/Entity/Component.hpp>
#include <MainApplication/Viewer/Gizmo/Gizmo.hpp>
namespace Ra
{
    namespace Gui
    {
        // Todo : check if we can't refactor with EntityPropertyWidget.
        class GizmoManager: public QObject
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

            GizmoManager(QObject* parent = nullptr)
                    : QObject(parent)
                    , m_currentTarget(nullptr)
                    , m_currentGizmoType(NONE)
                    , m_currentGizmo(nullptr)
            {}

            public slots:
            void setEditable(Engine::EditableInterface* edit)
            {
                delete m_currentGizmo;
                if (edit)
                {
                    Core::AlignedStdVector<Engine::EditableProperty> properties;
                    edit->getProperties(properties);

                    m_currentEdit = edit;
                    // Translation only.
                    for (const auto& p : properties)
                    {
                        if (p.getType() == Engine::EditableProperty::POSITION)
                        {
                            m_currentTarget = nullptr;
                            m_currentTransform = Core::Transform::Identity();
                            m_currentTransform.translation() = p.asPosition();
                            m_currentGizmoType = TRANSLATION;
                            //                        m_currentGizmo = new TranslateGizmo(m_currentTarget, m_currentTransform);
                            return;
                        }
                    }
                }
                m_currentGizmoType = NONE;
                m_currentTarget = nullptr;
                m_currentGizmo = nullptr;

            };

        private:
            Core::Transform  m_currentTransform;
            Engine::Component* m_currentTarget;
            Engine::EditableInterface* m_currentEdit;
            GizmoType m_currentGizmoType;
            Gizmo* m_currentGizmo;

        };
    }
}


#endif // RADIUMENGINE_GIZMO_MANAGER_HPP_
