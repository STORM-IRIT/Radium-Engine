#ifndef RADIUMENGINE_VIEWER_HPP
#define RADIUMENGINE_VIEWER_HPP

#include <GuiBase/RaGuiBase.hpp>

#include <memory>
#include <Engine/RadiumEngine.hpp>

#include <QOpenGLWidget>
#include <QThread>

#include <Core/Math/LinearAlgebra.hpp>
#include <GuiBase/Viewer/Gizmo/GizmoManager.hpp>

// Forward declarations
namespace Ra
{
    namespace Core
    {
        struct KeyEvent;
        struct MouseEvent;
    }
}

namespace Ra
{
    namespace Engine
    {
        class Renderer;
    }
}

namespace Ra
{
    namespace Gui
    {
        class CameraInterface;
        class GizmoManager;
    }
}

namespace Ra
{
    namespace Gui
    {

        // FIXME (Charly) : Which way do we want to be able to change renderers ?
        //                  Can it be done during runtime ? Must it be at startup ? ...
        //                  For now, default ForwardRenderer is used.

        /// The Viewer is the main display class. It's the central screen QWidget.
        /// Its acts as a bridge between the interface, the engine and the renderer
        /// Among its responsibilities are :
        /// * Owning the renderer and camera, and managing their lifetime.
        /// * setting up the renderer and camera by keeping it informed of interfaces changes
        ///  (e.g. resize).
        /// * catching user interaction (mouse clicks) at the lowest level and forward it to
        /// the camera and the rest of the application
        /// * Expose the asynchronous rendering interface
        class RA_GUIBASE_API Viewer : public QOpenGLWidget
        {
            Q_OBJECT

        public:
            /// Constructor
            explicit Viewer( QWidget* parent = nullptr );

            /// Destructor
            ~Viewer();

            //
            // Accessors
            //

            /// Access to camera interface.
            CameraInterface* getCameraInterface();

            /// Access to gizmo manager
            GizmoManager* getGizmoManager();

            /// Read-only access to renderer
            const Engine::Renderer* getRenderer() const;

            /// Read-write access to renderer
            Engine::Renderer* getRenderer();

            //
            // Rendering management
            //

            /// Start rendering (potentially asynchronously in a separate thread)
            void startRendering( const Scalar dt );

            /// Blocks until rendering is finished.
            void waitForRendering();

            //
            // Misc functions
            //

            /// Load data from a file.
            void handleFileLoading( const std::string& file );

            /// Emits signals corresponding to picking requests.
            void processPicking();

            /// Moves the camera so that the whole scene is visible.
            void fitCameraToScene( const Core::Aabb& sceneAabb );

            /// Returns the names of the different registred renderers.
            std::vector<std::string> getRenderersName() const;

            /// Write the current frame as an image. Supports either BMP or PNG file names.
            void grabFrame( const std::string& filename );

        signals:
            void rendererReady();               //! Emitted when the rendered is correctly initialized
            void leftClickPicking ( int id );   //! Emitted when the result of a left click picking is known
            void rightClickPicking( int id );   //! Emitted when the resut of a right click picking is known

        public slots:
            /// Tell the renderer to reload all shaders.
            void reloadShaders();

            /// Set the final display texture
            void displayTexture( const QString& tex );

            /// Set the renderer
            void changeRenderer( int index );

            /// Toggle the post-process effetcs
            void enablePostProcess(int enabled);

            /// Toggle the debug drawing
            void enableDebugDraw(int enabled);

            /// Resets the camaera to initial values
            void resetCamera();

        private slots:
            /// These slots are connected to the base class signals to properly handle
            /// concurrent access to the renderer.
            void onAboutToCompose();
            void onAboutToResize();
            void onFrameSwapped();
            void onResized();

        private:

            //
            // QOpenGlWidget primitives
            //

            /// Initialize openGL. Called on by the first "show" call to the main window.
            virtual void initializeGL() override;

            /// Resize the view port and the camera. Called by the resize event.
            virtual void resizeGL( int width, int height ) override;

            /// Paint event is set to a no-op to prevent synchronous rendering.
            /// We don't implement paintGL as well.
            virtual void paintEvent( QPaintEvent* e ) override {}

            //
            // Qt input events.
            //

            virtual void keyPressEvent( QKeyEvent* event ) override;
            virtual void keyReleaseEvent( QKeyEvent* event ) override;

            /// We intercept the mouse events in this widget to get the coordinates of the mouse
            /// in screen space.
            virtual void mousePressEvent( QMouseEvent* event ) override;
            virtual void mouseReleaseEvent( QMouseEvent* event ) override;
            virtual void mouseMoveEvent( QMouseEvent* event ) override;
            virtual void wheelEvent( QWheelEvent* event ) override;

        public:
            Scalar m_dt;

        private:
            /// Owning pointer to the renderers.
            std::vector<std::unique_ptr<Engine::Renderer>> m_renderers;
            Engine::Renderer* m_currentRenderer;

            /// Owning pointer to the camera.
            std::unique_ptr<CameraInterface> m_camera;

            /// Owning (QObject child) pointer to gizmo manager.
            GizmoManager* m_gizmoManager;

            /// Thread in which rendering is done.
            QThread* m_renderThread; // We have to use a QThread for MT rendering
        };

    } // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_VIEWER_HPP
