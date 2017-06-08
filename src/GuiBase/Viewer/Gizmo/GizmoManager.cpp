#include <Engine/Renderer/OpenGL/OpenGL.hpp>
#include <GuiBase/Viewer/Gizmo/GizmoManager.hpp>
#include <GuiBase/Viewer/CameraInterface.hpp>
#include <GuiBase/Viewer/Gizmo/TranslateGizmo.hpp>
#include <GuiBase/Viewer/Gizmo/RotateGizmo.hpp>

#include <Engine/Renderer/Camera/Camera.hpp>
#include <Engine/Managers/SystemDisplay/SystemDisplay.hpp>


namespace Ra
{
    namespace Gui
    {
        /*
         * TODO : Mathias -- Beuurk. Creating gizmos by default implies that all
         * applications developped on top of the engine will have them.
         * This is not a good idea. Applications mus be able to define and
         * create their their own gizmos
         *
         * \see issue #194
         */
        GizmoManager::GizmoManager(QObject* parent)
                : QObject(parent)
                , m_currentGizmoType(NONE), m_mode(Gizmo::GLOBAL)
        {
            m_gizmos[0].reset(new TranslateGizmo(Engine::SystemEntity::uiCmp(), Ra::Core::Transform::Identity(), m_transform, m_mode));
            m_gizmos[1].reset(new RotateGizmo(Engine::SystemEntity::uiCmp(), Ra::Core::Transform::Identity(), m_transform, m_mode));
            m_gizmos[2].reset(nullptr) ; // add scale gizmo when implemented
            for (auto& g : m_gizmos)
            {
                if (g)
                {
                    g->show(false);
                }
            }
        }


        GizmoManager::~GizmoManager() { }

        void GizmoManager::setEditable( const Engine::ItemEntry& ent )
        {
            TransformEditor::setEditable(ent);
            updateGizmo();
        }

        void GizmoManager::updateGizmo()
        {
            for (auto& g : m_gizmos)
            {
                if (g)
                {
                    g->show(false);
                }
            }

            if (canEdit())
            {
                Core::Transform worldTransform = getWorldTransform();
                auto g = currentGizmo();
                if (g)
                {
                    g->updateTransform(m_mode, worldTransform, m_transform);
                    g->show(true);
                }
            }
        }

        void GizmoManager::setLocal(bool useLocal)
        {
            m_mode = useLocal ? Gizmo::LOCAL : Gizmo::GLOBAL;
            updateGizmo();
        }

        void GizmoManager::changeGizmoType(GizmoManager::GizmoType type)
        {
            m_currentGizmoType = type;
            updateGizmo();
        }

        void GizmoManager::updateValues()
        {
            if (canEdit())
            {
                getTransform();
                if (currentGizmo())
                {
                    currentGizmo()->updateTransform(m_mode, getWorldTransform(), m_transform);
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
            CORE_ASSERT(currentGizmo(), "Gizmo is not there !");

            // Access the camera from the viewer. (TODO : a cleaner way to access the camera).
            const Engine::Camera& cam = CameraInterface::getCameraFromViewer(parent());
            currentGizmo()->setInitialState(cam, Core::Vector2(Scalar(event->x()), Scalar(event->y())));

            return true;
        }

        bool GizmoManager::handleMouseReleaseEvent(QMouseEvent* event)
        {
            if ( event->button() == Qt::LeftButton && currentGizmo())
            {
                currentGizmo()->selectConstraint(-1);
            }
            return (currentGizmo() != nullptr);
        }

        bool GizmoManager::handleMouseMoveEvent(QMouseEvent* event)
        {
            if ( event->buttons() & Qt::LeftButton && currentGizmo() )
            {
                Core::Vector2 currentXY(event->x(), event->y());
                const Engine::Camera& cam = CameraInterface::getCameraFromViewer(parent());
                Core::Transform newTransform = currentGizmo()->mouseMove(cam, currentXY);
                setTransform( newTransform );
            }
            return (currentGizmo() != nullptr);
        }

        void GizmoManager::handlePickingResult(int drawableId)
        {
            if (currentGizmo())
            {
                currentGizmo()->selectConstraint( drawableId );
            }
        }

        Gizmo* GizmoManager::currentGizmo()
        {
            return ( m_currentGizmoType == NONE )?
                   nullptr :
                   m_gizmos[ m_currentGizmoType - 1].get();
        }

        void GizmoManager::cleanup()
        {
            for (auto& g : m_gizmos)
            {
                g.reset(nullptr);
            }
        }
    }
}
