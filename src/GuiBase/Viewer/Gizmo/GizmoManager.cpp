#include <GuiBase/Viewer/Gizmo/GizmoManager.hpp>

#include <Engine/Renderer/Camera/Camera.hpp>
#include <Engine/Managers/SystemDisplay/SystemDisplay.hpp>


#include <GuiBase/Viewer/Viewer.hpp>
#include <GuiBase/Viewer/CameraInterface.hpp>
#include <GuiBase/Viewer/Gizmo/TranslateGizmo.hpp>
#include <GuiBase/Viewer/Gizmo/RotateGizmo.hpp>
#include <QtWidgets/QtWidgets>


namespace Ra
{
    namespace Gui
    {
        GizmoManager::GizmoManager(QObject* parent)
                : QObject(parent),m_currentGizmo(nullptr)
                , m_currentGizmoType(NONE), m_mode(Gizmo::GLOBAL){ }


        GizmoManager::~GizmoManager() { }

        void GizmoManager::setEditable( const Engine::ItemEntry& ent )
        {
            TransformEditor::setEditable(ent);
            spawnGizmo();
        }

        void GizmoManager::spawnGizmo()
        {
            m_currentGizmo.reset(nullptr);
            if (canEdit())
            {
                Core::Transform worldTransform = getWorldTransform();
                switch (m_currentGizmoType)
                {
                    case NONE:
                    {
                        break;
                    }
                    case TRANSLATION:
                    {
                        m_currentGizmo.reset(new TranslateGizmo(Engine::SystemEntity::uiCmp(), worldTransform, m_transform, m_mode));
                        break;
                    }
                    case ROTATION:
                    {
                        m_currentGizmo.reset(new RotateGizmo(Engine::SystemEntity::uiCmp(), worldTransform, m_transform, m_mode));
                        break;
                    }
                    case SCALE:
                        break ; // not implemented yet...
                }
            }
        }

        void GizmoManager::setLocal(bool useLocal)
        {
            m_mode = useLocal ? Gizmo::LOCAL : Gizmo::GLOBAL;
            spawnGizmo();
        }

        void GizmoManager::changeGizmoType(GizmoManager::GizmoType type)
        {
            m_currentGizmoType = type;
            spawnGizmo();
        }

        void GizmoManager::updateValues()
        {
            if (canEdit())
            {
                getTransform();
                if (m_currentGizmo)
                {
                    m_currentGizmo->updateTransform(getWorldTransform(), m_transform);
                }
            }
        }

        bool GizmoManager::handleMousePressEvent(QMouseEvent* event)
        {
            if( event->button() != Qt::LeftButton || !canEdit() || m_currentGizmoType == NONE)
            {
                return false;
            }
            // If we are there it means that we should have a valid gizmo.
            CORE_ASSERT(m_currentGizmo, "Gizmo is not there !");

            // Access the camera from the viewer. (TODO : a cleaner way to access the camera).
            const Engine::Camera& cam = *static_cast<Viewer*>(parent())->getCameraInterface()->getCamera();
            m_currentGizmo->setInitialState(cam, Core::Vector2(Scalar(event->x()), Scalar(event->y())));

            return true;
        }

        bool GizmoManager::handleMouseReleaseEvent(QMouseEvent* event)
        {
            if ( event->button() == Qt::LeftButton && m_currentGizmo)
            {
                m_currentGizmo->selectConstraint(-1);
            }
            return (m_currentGizmo != nullptr);
        }

        bool GizmoManager::handleMouseMoveEvent(QMouseEvent* event)
        {
            if ( event->buttons() & Qt::LeftButton && m_currentGizmo )
            {
                Core::Vector2 currentXY(event->x(), event->y());
                const Engine::Camera& cam = *static_cast<Viewer*>(parent())->getCameraInterface()->getCamera();
                Core::Transform newTransform = m_currentGizmo->mouseMove(cam, currentXY);
                setTransform( newTransform );
            }
            return (m_currentGizmo != nullptr);
        }

        void GizmoManager::handlePickingResult(int drawableId)
        {
            if (m_currentGizmo)
            {
                m_currentGizmo->selectConstraint( drawableId );
            }
        }
    }
}
