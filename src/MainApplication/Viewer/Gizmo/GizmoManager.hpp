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
                    , m_currentGizmoType(NONE)
                    , m_currentGizmo(nullptr)
            {}

            public slots:
            void setEditable(Engine::EditableInterface* edit)
            {
                delete m_currentGizmo;
                if (edit)
                {
                    Core::AlignedStdVector<Engine::EditableProperty> props;
                    edit->getProperties(props);
                    int transformFound = -1;
                    for (uint i = 0; i < props.size(); ++i)
                    {
                        if (props[i].type == Engine::EditableProperty::TRANSFORM)
                        {
                            transformFound = i;
                            break;
                        }
                    }
                    if (transformFound >= 0)
                    {
                        const Engine::EditableProperty& transformProp = props[transformFound];
                        m_currentEdit = edit;
                        m_transform = Core::Transform::Identity();
                        m_currentGizmoType = TRANSLATION;
                        // Grab translation.
                        for (const auto& p : transformProp.primitives)
                        {
                            if (p.primitive.getType() == Engine::EditablePrimitive::POSITION)
                            {
                                m_transform.translation() = p.primitive.asPosition();
                            }

                            if (p.primitive.getType() == Engine::EditablePrimitive::ROTATION)
                            {
                                m_transform.linear() = p.primitive.asRotation().toRotationMatrix();
                            }
                        }
                        m_currentGizmo = new TranslateGizmo(Engine::DebugEntity::dbgCmp(), m_transform);
                        return;
                    }
                }
                m_currentGizmoType = NONE;
                m_currentGizmo = nullptr;

            };

        private:
            Core::Transform m_transform;
            Engine::EditableInterface* m_currentEdit;
            GizmoType m_currentGizmoType;
            Gizmo* m_currentGizmo;
        };
    }
}


#endif // RADIUMENGINE_GIZMO_MANAGER_HPP_
