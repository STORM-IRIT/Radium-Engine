#include <MainApplication/Viewer/Gizmo/GizmoManager.hpp>

namespace Ra
{
    namespace Gui
    {
        GizmoManager::GizmoManager(QObject* parent)
                : QObject(parent),m_currentGizmo(nullptr), m_currentGizmoType(TRANSLATION), m_mode(LOCAL) { }


        GizmoManager::~GizmoManager() { }

        void GizmoManager::setEditable(Engine::EditableInterface* edit)
        {
            m_currentEdit = edit;
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
                    m_transform = Core::Transform::Identity();
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
                }
            }
            spawnGizmo();
        }

        void GizmoManager::spawnGizmo()
        {
            m_currentGizmo.reset(nullptr);
            if (m_currentEdit)
            {
                switch (m_currentGizmoType)
                {
                    case NONE:
                    {
                        break;
                    }
                    case TRANSLATION:
                    {
                        m_currentGizmo.reset(new TranslateGizmo(Engine::DebugEntity::dbgCmp(), m_transform));
                        break;
                    }
                    case ROTATION:
                    case SCALE:
                        break ; // not implemented yet...
                }
            }
        }

        void GizmoManager::changeMode(GizmoManager::Mode mode)
        {
            m_mode = mode;
            spawnGizmo();
        }

        void GizmoManager::changeGizmoType(GizmoManager::GizmoType type)
        {
            m_currentGizmoType = type;
            spawnGizmo();
        }
    }
}