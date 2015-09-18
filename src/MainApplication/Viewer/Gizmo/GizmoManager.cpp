#include <MainApplication/Viewer/Gizmo/GizmoManager.hpp>
#include <Engine/Renderer/Camera/Camera.hpp>
#include <MainApplication/Viewer/Viewer.hpp>
#include <MainApplication/Viewer/CameraInterface.hpp>
#include <Core/Math/ColorPresets.hpp>

namespace Ra
{
    namespace Gui
    {
        GizmoManager::GizmoManager(QObject* parent)
                : QObject(parent),m_currentEdit(nullptr),m_currentGizmo(nullptr), m_currentGizmoType(TRANSLATION), m_mode(LOCAL) { }


        GizmoManager::~GizmoManager() { }

        void GizmoManager::setEditable(Engine::EditableInterface* edit)
        {
            m_currentEdit = edit;
            getTransform();
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

        void GizmoManager::getTransform()
        {
            if (m_currentEdit)
            {
                Core::AlignedStdVector<Engine::EditableProperty> props;
                m_currentEdit->getProperties(props);
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
                    Core::Transform transform = Core::Transform::Identity();
                    // Grab translation.
                    for (const auto& p : transformProp.primitives)
                    {
                        if (p.primitive.getType() == Engine::EditablePrimitive::POSITION)
                        {
                            transform.translation() = p.primitive.asPosition();
                        }

                        if (p.primitive.getType() == Engine::EditablePrimitive::ROTATION)
                        {
                            transform.linear() = p.primitive.asRotation().toRotationMatrix();
                        }
                    }
                    m_transform = transform;
                }
            }
        }

        void GizmoManager::updateValues()
        {
            getTransform();
            if(m_currentGizmo)
            {
                m_currentGizmo->updateTransform(m_transform);
            }
        }

        bool GizmoManager::handleMousePressEvent(QMouseEvent* event)
        {
            uint x = uint(event->pos().x());
            uint y = uint(event->pos().y());
            Core::Ray r = static_cast<Viewer*>(parent())->getCameraInterface()->getCamera()->getRayFromScreen(x,y);

            RA_DISPLAY_RAY(r, Core::Colors::Yellow());
            if( event->button() != Qt::LeftButton || !m_currentEdit || m_currentGizmoType == NONE)
            {
                return false;
            }
            CORE_ASSERT(m_currentGizmo, "Gizmo is not there !");

            m_currentGizmo->selectConstraint(r);

            m_lastMouseX = x;
            m_lastMouseY = y;

            return true;
        }

        bool GizmoManager::handleMouseReleaseEvent(QMouseEvent* event)
        {
            return false;
        }

        bool GizmoManager::handleMouseMoveEvent(QMouseEvent* event)
        {
            return false;
        }
    }
}